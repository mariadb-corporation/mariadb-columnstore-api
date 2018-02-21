package com.mariadb.columnstore.api.kettle;

import org.pentaho.di.core.Const;
import org.pentaho.di.core.database.DatabaseInterface;
import org.pentaho.di.core.database.DatabaseMeta;
import org.pentaho.di.core.exception.KettleDatabaseException;
import org.pentaho.di.core.logging.LoggingObjectInterface;
import org.pentaho.di.core.row.RowMeta;
import org.pentaho.di.core.row.RowMetaInterface;
import org.pentaho.di.core.row.ValueMetaInterface;
import org.pentaho.di.core.variables.VariableSpace;
import org.pentaho.di.core.variables.Variables;

public class MariaDBColumnStoreDatabase extends org.pentaho.di.core.database.Database {


    public MariaDBColumnStoreDatabase(LoggingObjectInterface parentObject, DatabaseMeta databaseMeta) {
        super(parentObject, databaseMeta);
    }

    /**
     * Generates SQL
     *
     * @param tableName   the table name or schema/table combination: this needs to be quoted properly in advance.
     * @param fields      the fields
     * @param tk          the name of the technical key field
     * @param use_autoinc true if we need to use auto-increment fields for a primary key
     * @param pk          the name of the primary/technical key field
     * @param semicolon   append semicolon to the statement
     * @return the SQL needed to create the specified table and fields.
     */
    public String getCreateTableStatement(String tableName, RowMetaInterface fields, String tk,
                                          boolean use_autoinc, String pk, boolean semicolon) {
        StringBuilder retval = new StringBuilder();
        DatabaseInterface databaseInterface = super.getDatabaseMeta().getDatabaseInterface();
        retval.append(databaseInterface.getCreateTableStatement());

        retval.append(tableName + Const.CR);
        retval.append("(").append(Const.CR);
        for (int i = 0; i < fields.size(); i++) {
            if (i > 0) {
                retval.append(", ");
            } else {
                retval.append("  ");
            }

            ValueMetaInterface v = fields.getValueMeta(i);
            retval.append(super.getDatabaseMeta().getFieldDefinition(v, tk, pk, use_autoinc));
        }
        // At the end, before the closing of the statement, we might need to add
        // some constraints...
        // Technical keys
        if (tk != null) {
            if (super.getDatabaseMeta().requiresCreateTablePrimaryKeyAppend()) {
                retval.append(", PRIMARY KEY (").append(tk).append(")").append(Const.CR);
            }
        }

        // Primary keys
        if (pk != null) {
            if (super.getDatabaseMeta().requiresCreateTablePrimaryKeyAppend()) {
                retval.append(", PRIMARY KEY (").append(pk).append(")").append(Const.CR);
            }
        }
        retval.append(")").append(Const.CR);

        VariableSpace variables = new Variables();
        for (String variableName : super.listVariables()) {
            variables.setVariable(variableName, super.getVariable(variableName));
        }

        retval.append(super.getDatabaseMeta().getDatabaseInterface().getDataTablespaceDDL(variables, super.getDatabaseMeta()));

        retval.append("ENGINE=COLUMNSTORE").append(Const.CR);

        if (semicolon) {
            retval.append(";");
        }

        return retval.toString();
    }

    public String getAlterTableStatement(String tableName, RowMetaInterface fields, String tk, boolean use_autoinc,
                                         String pk, boolean semicolon) throws KettleDatabaseException {
        StringBuilder retval = new StringBuilder();

        // Get the fields that are in the table now:
        RowMetaInterface tabFields = getTableFields(tableName);

        // Don't forget to quote these as well...
        super.getDatabaseMeta().quoteReservedWords(tabFields);

        // Find the missing fields
        RowMetaInterface missing = new RowMeta();
        for (int i = 0; i < fields.size(); i++) {
            ValueMetaInterface v = fields.getValueMeta(i);
            // Not found?
            if (tabFields.searchValueMeta(v.getName()) == null) {
                missing.addValueMeta(v); // nope --> Missing!
            }
        }

        if (missing.size() != 0) {
            for (int i = 0; i < missing.size(); i++) {
                ValueMetaInterface v = missing.getValueMeta(i);
                retval.append(super.getDatabaseMeta().getAddColumnStatement(tableName, v, tk, use_autoinc, pk, true));
            }
        }

        // Find the surplus fields
        RowMetaInterface surplus = new RowMeta();
        for (int i = 0; i < tabFields.size(); i++) {
            ValueMetaInterface v = tabFields.getValueMeta(i);
            // Found in table, not in input ?
            if (fields.searchValueMeta(v.getName()) == null) {
                surplus.addValueMeta(v); // yes --> surplus!
            }
        }

        if (surplus.size() != 0) {
            for (int i = 0; i < surplus.size(); i++) {
                ValueMetaInterface v = surplus.getValueMeta(i);
                retval.append(super.getDatabaseMeta().getDropColumnStatement(tableName, v, tk, use_autoinc, pk, true));
            }
        }

        //
        // OK, see if there are fields for which we need to modify the type...
        // (length, precision)
        //
        RowMetaInterface modify_current = new RowMeta();
        RowMetaInterface modify_desired = new RowMeta();
        for (int i = 0; i < fields.size(); i++) {
            ValueMetaInterface desiredField = fields.getValueMeta(i);
            ValueMetaInterface currentField = tabFields.searchValueMeta(desiredField.getName());
            if (desiredField != null && currentField != null) {
                String desiredDDL = super.getDatabaseMeta().getFieldDefinition(desiredField, tk, pk, use_autoinc);
                String currentDDL = super.getDatabaseMeta().getFieldDefinition(currentField, tk, pk, use_autoinc);

                boolean mod = !desiredDDL.equalsIgnoreCase(currentDDL);
                if (mod) {
                    modify_current.addValueMeta(currentField);
                    modify_desired.addValueMeta(desiredField);
                }
            }
        }

        // As ColumnStore doesn't support MODIFY commands we have to add the temporary desired field, copy all data from the old field to the temporary, drop the old field and rename the temporary.
        if (modify_desired.size() > 0) {
            for (int i = 0; i < modify_desired.size(); i++) {
                ValueMetaInterface v_current = modify_current.getValueMeta(i);
                ValueMetaInterface v_desired = modify_desired.getValueMeta(i);

                v_desired.setName(v_desired.getName().concat("_tmp"));

                retval.append(super.getDatabaseMeta().getAddColumnStatement(tableName, v_desired, tk, use_autoinc, pk, true)); // create temporary column
                retval.append(getColumnCopyDataStatement(tableName, v_current.getName(), v_desired.getName())); // copy data into temporary column
                retval.append(super.getDatabaseMeta().getDropColumnStatement(tableName, v_current, tk, use_autoinc, pk, true)); // drop old column
                retval.append(getRenameColumnStatement(tableName, v_desired, null, false, null, v_current.getName())); // rename temporary into new column
            }
        }

        return retval.toString();
    }

    /**
     * Copies data from source column to target column in table.
     * @param table
     * @param sourceColumnName
     * @param targetColumnName
     * @return
     */
    private String getColumnCopyDataStatement(String table, String sourceColumnName, String targetColumnName) {
        return "UPDATE " + table + " SET " + targetColumnName + "=" + sourceColumnName +  Const.CR + ";" + Const.CR;
    }

    /**
     * Renames old_column to new_column in table
     * @param table
     * @param sourceColumn
     * @param targetColumnName
     * @return
     */
    private String getRenameColumnStatement(String table, ValueMetaInterface sourceColumn, String tk, boolean use_autoinc, String pk, String targetColumnName) {
        StringBuilder retval = new StringBuilder("ALTER TABLE " + table + " CHANGE COLUMN " + sourceColumn.getName() + " " + targetColumnName);
        String[] fieldDefinition = super.getDatabaseMeta().getFieldDefinition(sourceColumn, tk, pk, use_autoinc).split(" ");
        for(int i=1; i<fieldDefinition.length; i++){
            retval.append(" ").append(fieldDefinition[i]);
        }
        retval.append(";" + Const.CR);

        return retval.toString();
    }
}
