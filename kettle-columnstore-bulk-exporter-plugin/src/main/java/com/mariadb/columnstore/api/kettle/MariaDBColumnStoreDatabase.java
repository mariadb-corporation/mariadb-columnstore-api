package com.mariadb.columnstore.api.kettle;

import com.mariadb.columnstore.api.ColumnStoreDriver;
import com.mariadb.columnstore.api.ColumnStoreSystemCatalog;
import com.mariadb.columnstore.api.ColumnStoreSystemCatalogTable;
import com.mariadb.columnstore.api.columnstore_data_types_t;
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

    private KettleColumnStoreBulkExporterStepMeta meta;
    private ColumnStoreDriver d;

    public MariaDBColumnStoreDatabase(LoggingObjectInterface parentObject, DatabaseMeta databaseMeta, KettleColumnStoreBulkExporterStepMeta meta, ColumnStoreDriver d) {
        super(parentObject, databaseMeta);
        this.meta = meta;
        this.d = d;
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
        for(String variableName : super.listVariables()){
            variables.setVariable(variableName, super.getVariable(variableName));
        }

        retval.append(super.getDatabaseMeta().getDatabaseInterface().getDataTablespaceDDL(variables, super.getDatabaseMeta()));

        retval.append("ENGINE=COLUMNSTORE").append(Const.CR);

        if (semicolon) {
            retval.append(";");
        }

        return retval.toString();
    }

    public String getAlterTableStatement( String tableName, RowMetaInterface fields, String tk, boolean use_autoinc,
                                          String pk, boolean semicolon ) throws KettleDatabaseException {
        String retval = "";

        // Get the fields that are in the table now:
        RowMetaInterface tabFields = getTableFields( tableName );

        // Don't forget to quote these as well...
        super.getDatabaseMeta().quoteReservedWords( tabFields );

        // Find the missing fields
        RowMetaInterface missing = new RowMeta();
        for ( int i = 0; i < fields.size(); i++ ) {
            ValueMetaInterface v = fields.getValueMeta( i );
            // Not found?
            if ( tabFields.searchValueMeta( v.getName() ) == null ) {
                missing.addValueMeta( v ); // nope --> Missing!
            }
        }

        if ( missing.size() != 0 ) {
            for ( int i = 0; i < missing.size(); i++ ) {
                ValueMetaInterface v = missing.getValueMeta( i );
                retval += super.getDatabaseMeta().getAddColumnStatement( tableName, v, tk, use_autoinc, pk, true );
            }
        }

        // Find the surplus fields
        RowMetaInterface surplus = new RowMeta();
        for ( int i = 0; i < tabFields.size(); i++ ) {
            ValueMetaInterface v = tabFields.getValueMeta( i );
            // Found in table, not in input ?
            if ( fields.searchValueMeta( v.getName() ) == null ) {
                surplus.addValueMeta( v ); // yes --> surplus!
            }
        }

        if ( surplus.size() != 0 ) {
            for ( int i = 0; i < surplus.size(); i++ ) {
                ValueMetaInterface v = surplus.getValueMeta( i );
                retval += super.getDatabaseMeta().getDropColumnStatement( tableName, v, tk, use_autoinc, pk, true );
            }
        }

        //
        // OK, see if there are fields for which we need to modify the type...
        // (length, precision)
        //
        RowMetaInterface modify_current = new RowMeta();
        RowMetaInterface modify_desired = new RowMeta();

        ColumnStoreSystemCatalog c = d.getSystemCatalog();
        ColumnStoreSystemCatalogTable t = c.getTable(tableName.split(".")[0], tableName.split(".")[1]);


        for ( int i = 0; i < fields.size(); i++ ) {
            ValueMetaInterface desiredField = fields.getValueMeta( i );
            ValueMetaInterface currentField = tabFields.searchValueMeta( desiredField.getName() );

            if ( desiredField != null && currentField != null ) {

                columnstore_data_types_t columnStoreType = t.getColumn(currentField.getName()).getType();

                // only change if types are not compatible
                if ( !meta.checkCompatibility(desiredField.getType(),columnStoreType) ) {
                    modify_current.addValueMeta( currentField );
                    modify_desired.addValueMeta( desiredField );
                }
            }
        }

        if ( modify_desired.size() > 0 ) {
            for ( int i = 0; i < modify_desired.size(); i++ ) {
                ValueMetaInterface v_current = modify_current.getValueMeta( i );
                ValueMetaInterface v_desired = modify_desired.getValueMeta( i );

                // As ColumnStore doesn't support MODIFY commands we have to remove the current field and add the desired field.
                retval += super.getDatabaseMeta().getDropColumnStatement( tableName, v_current, tk, use_autoinc, pk, true );
                retval += super.getDatabaseMeta().getAddColumnStatement( tableName, v_desired, tk, use_autoinc, pk, true );
            }
        }

        return retval;
    }
}

