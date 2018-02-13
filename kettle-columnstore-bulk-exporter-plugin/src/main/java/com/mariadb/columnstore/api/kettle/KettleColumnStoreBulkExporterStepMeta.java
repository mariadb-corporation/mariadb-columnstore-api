/*
 Copyright (c) 2018, MariaDB Corporation. All rights reserved.
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA 02110-1301  USA
*/

package com.mariadb.columnstore.api.kettle;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import com.mariadb.columnstore.api.*;
import org.eclipse.swt.widgets.Shell;
import org.pentaho.di.core.CheckResult;
import org.pentaho.di.core.CheckResultInterface;
import org.pentaho.di.core.annotations.Step;
import org.pentaho.di.core.database.DatabaseMeta;
import org.pentaho.di.core.exception.KettleException;
import org.pentaho.di.core.exception.KettleXMLException;
import org.pentaho.di.core.injection.Injection;
import org.pentaho.di.core.injection.InjectionSupported;
import org.pentaho.di.core.row.RowMetaInterface;
import org.pentaho.di.core.row.ValueMetaInterface;
import org.pentaho.di.core.variables.VariableSpace;
import org.pentaho.di.core.xml.XMLHandler;
import org.pentaho.di.i18n.BaseMessages;
import org.pentaho.di.repository.ObjectId;
import org.pentaho.di.repository.Repository;
import org.pentaho.di.trans.Trans;
import org.pentaho.di.trans.TransMeta;
import org.pentaho.di.trans.step.BaseStepMeta;
import org.pentaho.di.trans.step.StepDataInterface;
import org.pentaho.di.trans.step.StepDialogInterface;
import org.pentaho.di.trans.step.StepInterface;
import org.pentaho.di.trans.step.StepMeta;
import org.pentaho.di.trans.step.StepMetaInterface;
import org.pentaho.metastore.api.IMetaStore;
import org.w3c.dom.Node;

import static org.pentaho.di.core.row.ValueMetaInterface.*;

/**
 * This class is part of the demo step plug-in implementation.
 * It demonstrates the basics of developing a plug-in step for PDI. 
 * 
 * The demo step adds a new string field to the row stream and sets its
 * value to "Hello World!". The user may select the name of the new field.
 *   
 * This class is the implementation of StepMetaInterface.
 * Classes implementing this interface need to:
 * 
 * - keep track of the step settings
 * - serialize step settings both to xml and a repository
 * - provide new instances of objects implementing StepDialogInterface, StepInterface and StepDataInterface
 * - report on how the step modifies the meta-data of the row-stream (row structure and field types)
 * - perform a sanity-check on the settings provided by the user 
 * 
 */

@Step(
  id = "KettleColumnStoreBulkExporterPlugin",
  name = "KettleColumnStoreBulkExporterPlugin.Name",
  description = "KettleColumnStoreBulkExporterPlugin.TooltipDesc",
  image = "com/mariadb/columnstore/api/kettle/resources/CS.svg",
  categoryDescription = "i18n:org.pentaho.di.trans.step:BaseStep.Category.Bulk",
  i18nPackageName = "com.mariadb.columnstore.api.kettle",
  documentationUrl = "MariaDB+ColumnStore+Bulk+Loader",
  casesUrl = "KettleColumnStoreBulkExporterPlugin.CasesURL",
  forumUrl = "KettleColumnStoreBulkExporterPlugin.ForumURL"
  )
@InjectionSupported( localizationPrefix = "KettleColumnStoreBulkExporterStepMeta.Injection." )
public class KettleColumnStoreBulkExporterStepMeta extends BaseStepMeta implements StepMetaInterface {

  /**
   *  The PKG member is used when looking up internationalized strings.
   *  The properties file with localized keys is expected to reside in 
   *  {the package of the class specified}/messages/messages_{locale}.properties   
   */
  private static final Class<?> PKG = KettleColumnStoreBulkExporterStepMeta.class; // for i18n purposes

  /**
   * Stores the name of the target database to export into.
   */
  @Injection( name = "TARGET_DATABASE" )
  private String targetDatabase;

  /**
   * Stores the name of the target table to export into.
   */
  @Injection( name = "TARGET_TABLE" )
  private String targetTable;

  /**
   * Wrapper class to store the mapping between input and output
   */
  protected static class InputTargetMapping{
    private String[] inputStreamFields;
    private String[] targetColumnStoreColumns;

    public InputTargetMapping(){
      this(0);
    }

    public InputTargetMapping(int entries){
      this.inputStreamFields = new String[entries];
      this.targetColumnStoreColumns = new String[entries];
    }

    public int getNumberOfEntries(){
      return inputStreamFields.length;
    }

    public String[] getInputStreamFields() {
      return inputStreamFields;
    }

    public String[] getTargetColumnStoreColumns() {
      return targetColumnStoreColumns;
    }

    public String getInputStreamField(int i){
      return inputStreamFields[i];
    }

    public String getTargetColumnStoreColumn(int i){
      return targetColumnStoreColumns[i];
    }

    public void setInputFieldMetaData(int index, String metaData){
      inputStreamFields[index] = metaData;
    }

    public void setTargetColumnStoreColumn(int index, String metaData){
      targetColumnStoreColumns[index] = metaData;
    }

    public String getTargetInputMappingField(String ColumnStoreTargetName){
      for (int i=0; i< targetColumnStoreColumns.length; i++){
        if(targetColumnStoreColumns[i].toLowerCase().equals(ColumnStoreTargetName.toLowerCase())){ //quick fix for MCOL-1213
          return inputStreamFields[i];
        }
      }
      return null;
    }
  }

  private InputTargetMapping fieldMapping;

  /**
   * Constructor should call super() to make sure the base class has a chance to initialize properly.
   */
  public KettleColumnStoreBulkExporterStepMeta() {
    super();
  }

  /**
   * Called by Spoon to get a new instance of the SWT dialog for the step.
   * A standard implementation passing the arguments to the constructor of the step dialog is recommended.
   * 
   * @param shell    an SWT Shell
   * @param meta     description of the step 
   * @param transMeta  description of the the transformation 
   * @param name    the name of the step
   * @return       new instance of a dialog for this step 
   */
  public StepDialogInterface getDialog( Shell shell, StepMetaInterface meta, TransMeta transMeta, String name ) {
    return new KettleColumnStoreBulkExporterStepDialog( shell, meta, transMeta, name );
  }

  /**
   * Called by PDI to get a new instance of the step implementation. 
   * A standard implementation passing the arguments to the constructor of the step class is recommended.
   * 
   * @param stepMeta        description of the step
   * @param stepDataInterface    instance of a step data class
   * @param cnr          copy number
   * @param transMeta        description of the transformation
   * @param disp          runtime implementation of the transformation
   * @return            the new instance of a step implementation 
   */
  public StepInterface getStep( StepMeta stepMeta, StepDataInterface stepDataInterface, int cnr, TransMeta transMeta,
      Trans disp ) {
    return new KettleColumnStoreBulkExporterStep( stepMeta, stepDataInterface, cnr, transMeta, disp );
  }

  /**
   * Called by PDI to get a new instance of the step data class.
   */
  public StepDataInterface getStepData() {
    return new KettleColumnStoreBulkExporterStepData();
  }

  /**
   * This method is called every time a new step is created and should allocate/set the step configuration
   * to sensible defaults. The values set here will be used by Spoon when a new step is created.    
   */
  public void setDefault() {
      setTargetDatabase("target database");
      setTargetTable("target table");
      fieldMapping = new InputTargetMapping();
  }

  /**
   * Getter for the name of the target database to export to
   * @return the name of the target database
   */
  public String getTargetDatabase() {
    return targetDatabase;
  }

  /**
   * Setter for the name of the target database to export to
   * @param targetDatabase the name of the target database
   */
  public void setTargetDatabase( String targetDatabase ) {
    this.targetDatabase = targetDatabase;
  }

    /**
     * Getter for the name of the target table to export to
     * @return the name of the target table
     */
    public String getTargetTable() {
        return targetTable;
    }

    /**
     * Setter for the name of the target table to export to
     * @param targetTable the name of the target table
     */
    public void setTargetTable( String targetTable ) {
        this.targetTable = targetTable;
    }

  public InputTargetMapping getFieldMapping(){
    return fieldMapping;
  }

  public void setFieldMapping(InputTargetMapping itm){
      this.fieldMapping = itm;
  }

  /**
   * This method is used when a step is duplicated in Spoon. It needs to return a deep copy of this
   * step meta object. Be sure to create proper deep copies if the step configuration is stored in
   * modifiable objects.
   * 
   * See org.pentaho.di.trans.steps.rowgenerator.RowGeneratorMeta.clone() for an example on creating
   * a deep copy.
   * 
   * @return a deep copy of this
   */
  public Object clone() {
    KettleColumnStoreBulkExporterStepMeta retval = (KettleColumnStoreBulkExporterStepMeta) super.clone();
    InputTargetMapping itm = new InputTargetMapping(fieldMapping.getNumberOfEntries());

    for (int i=0; i<fieldMapping.getNumberOfEntries(); i++){
      itm.setInputFieldMetaData(i, fieldMapping.getInputStreamField(i));
      itm.setTargetColumnStoreColumn(i, fieldMapping.getTargetColumnStoreColumn(i));
    }

    retval.setFieldMapping(itm);

    return retval;
  }

  /**
   * This method is called by Spoon when a step needs to serialize its configuration to XML. The expected
   * return value is an XML fragment consisting of one or more XML tags.  
   * 
   * Please use org.pentaho.di.core.xml.XMLHandler to conveniently generate the XML.
   * 
   * @return a string containing the XML serialization of this step
   */
  public String getXML() {
    StringBuilder xml = new StringBuilder();

    // only one field to serialize
    xml.append( XMLHandler.addTagValue( "targetdatabase", targetDatabase ) );
    xml.append( XMLHandler.addTagValue( "targettable", targetTable ) );

    xml.append( XMLHandler.addTagValue("numberOfMappingEntries", fieldMapping.getNumberOfEntries()));
    for(int i=0; i<fieldMapping.getNumberOfEntries(); i++){
      xml.append( XMLHandler.addTagValue("inputField_"+i+"_Name", fieldMapping.getInputStreamField(i)));
      xml.append( XMLHandler.addTagValue("targetField_"+i+"_Name", fieldMapping.getTargetColumnStoreColumn(i)));
    }
    return xml.toString();
  }

  /**
   * This method is called by PDI when a step needs to load its configuration from XML.
   * 
   * Please use org.pentaho.di.core.xml.XMLHandler to conveniently read from the
   * XML node passed in.
   * 
   * @param stepnode  the XML node containing the configuration
   * @param databases  the databases available in the transformation
   * @param metaStore the metaStore to optionally read from
   */
  public void loadXML( Node stepnode, List<DatabaseMeta> databases, IMetaStore metaStore ) throws KettleXMLException {
    try {
      setTargetDatabase( XMLHandler.getNodeValue( XMLHandler.getSubNode( stepnode, "targetdatabase" ) ) );
      setTargetTable( XMLHandler.getNodeValue( XMLHandler.getSubNode( stepnode, "targettable" ) ) );

      fieldMapping = new InputTargetMapping(Integer.parseInt(XMLHandler.getNodeValue(XMLHandler.getSubNode(stepnode, "numberOfMappingEntries"))));
      for(int i=0; i<fieldMapping.getNumberOfEntries(); i++){
        fieldMapping.setInputFieldMetaData(i,XMLHandler.getNodeValue(XMLHandler.getSubNode(stepnode, "inputField_"+i+"_Name")));
        fieldMapping.setTargetColumnStoreColumn(i,XMLHandler.getNodeValue(XMLHandler.getSubNode(stepnode, "targetField_"+i+"_Name")));
      }
    } catch ( Exception e ) {
      throw new KettleXMLException( "MariaDB ColumnStore Exporter Plugin unable to read step info from XML node", e );
    }
  }

  /**
   * This method is called by Spoon when a step needs to serialize its configuration to a repository.
   * The repository implementation provides the necessary methods to save the step attributes.
   *
   * @param rep                 the repository to save to
   * @param metaStore           the metaStore to optionally write to
   * @param id_transformation   the id to use for the transformation when saving
   * @param id_step             the id to use for the step  when saving
   */
  public void saveRep( Repository rep, IMetaStore metaStore, ObjectId id_transformation, ObjectId id_step )
      throws KettleException {
    try {
      rep.saveStepAttribute( id_transformation, id_step, "targetdatabase", targetDatabase ); //$NON-NLS-1$
      rep.saveStepAttribute( id_transformation, id_step, "targettable", targetTable ); //$NON-NLS-1$

      rep.saveStepAttribute( id_transformation, id_step, "numberOfMappingEntries", targetTable );
      for(int i=0; i<fieldMapping.getNumberOfEntries(); i++){
        rep.saveStepAttribute( id_transformation, id_step, "inputField_"+i+"_Name", fieldMapping.getInputStreamField(i));
        rep.saveStepAttribute( id_transformation, id_step, "targetField_"+i+"_Name", fieldMapping.getTargetColumnStoreColumn(i));
      }

    } catch ( Exception e ) {
      throw new KettleException( "Unable to save step into repository: " + id_step, e );
    }
  }

  /**
   * This method is called by PDI when a step needs to read its configuration from a repository.
   * The repository implementation provides the necessary methods to read the step attributes.
   * 
   * @param rep        the repository to read from
   * @param metaStore  the metaStore to optionally read from
   * @param id_step    the id of the step being read
   * @param databases  the databases available in the transformation
   */
  public void readRep( Repository rep, IMetaStore metaStore, ObjectId id_step, List<DatabaseMeta> databases )
      throws KettleException {
    try {
      targetDatabase  = rep.getStepAttributeString( id_step, "targetdatabase" ); //$NON-NLS-1$
      targetTable  = rep.getStepAttributeString( id_step, "targettable" ); //$NON-NLS-1$

      fieldMapping = new InputTargetMapping((int)rep.getStepAttributeInteger(id_step, "numberOfMappingEntries"));
      for(int i=0; i<fieldMapping.getNumberOfEntries(); i++){
        fieldMapping.setInputFieldMetaData(i,rep.getStepAttributeString(id_step, "inputField_"+i+"_Name"));
        fieldMapping.setTargetColumnStoreColumn(i,rep.getStepAttributeString(id_step, "targetField_"+i+"_Name"));
      }
    } catch ( Exception e ) {
      throw new KettleException( "Unable to load step from repository", e );
    }
  }

  /**
   * This method is called to determine the changes the step is making to the row-stream.
   * To that end a RowMetaInterface object is passed in, containing the row-stream structure as it is when entering
   * the step. This method must apply any changes the step makes to the row stream. Usually a step adds fields to the
   * row-stream.
   * 
   * @param inputRowMeta    the row structure coming in to the step
   * @param name         the name of the step making the changes
   * @param info        row structures of any info steps coming in
   * @param nextStep      the description of a step this step is passing rows to
   * @param space        the variable space for resolving variables
   * @param repository    the repository instance optionally read from
   * @param metaStore      the metaStore to optionally read from
   */
  public void getFields( RowMetaInterface inputRowMeta, String name, RowMetaInterface[] info, StepMeta nextStep,
      VariableSpace space, Repository repository, IMetaStore metaStore ) {

      //nothing to do here as we only dump the row stream to ColumnStore

  }

  /**
   * This method is called when the user selects the "Verify Transformation" option in Spoon. 
   * A list of remarks is passed in that this method should add to. Each remark is a comment, warning, error, or ok.
   * The method should perform as many checks as necessary to catch design-time errors.
   * 
   * Typical checks include:
   * - verify that all mandatory configuration is given
   * - verify that the step receives any input, unless it's a row generating step
   * - verify that the step does not receive any input if it does not take them into account
   * - verify that the step finds fields it relies on in the row-stream
   * 
   *   @param remarks    the list of remarks to append to
   *   @param transMeta  the description of the transformation
   *   @param stepMeta  the description of the step
   *   @param prev      the structure of the incoming row-stream
   *   @param input     names of steps sending input to the step
   *   @param output    names of steps this step is sending output to
   *   @param info      fields coming in from info steps 
   *   @param metaStore  metaStore to optionally read from
   */
  public void check( List<CheckResultInterface> remarks, TransMeta transMeta, StepMeta stepMeta, RowMetaInterface prev,
      String[] input, String[] output, RowMetaInterface info, VariableSpace space, Repository repository,
      IMetaStore metaStore ) {
    CheckResult cr;

    // See if there are input streams leading to this step!
    if ( input != null && input.length > 0 ) {
      cr = new CheckResult( CheckResult.TYPE_RESULT_OK, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.ReceivingRows.OK" ), stepMeta );
      remarks.add( cr );
      // check if the specified db and table exists
      ColumnStoreDriver d = new ColumnStoreDriver();
      ColumnStoreSystemCatalog catalog = d.getSystemCatalog();
      ColumnStoreSystemCatalogTable table = null;

      try {
        table = catalog.getTable(targetDatabase, targetTable);
      } catch(ColumnStoreException e){
        remarks.add(new CheckResult(CheckResult.TYPE_RESULT_ERROR, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.TableExistent.ERROR" ), stepMeta));
      }

      if (table != null){
        remarks.add(new CheckResult(CheckResult.TYPE_RESULT_OK, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.TableExistent.OK" ), stepMeta));
        // check if the input columns would fit into ColumnStore
        List<ValueMetaInterface> inputValueTypes = prev.getValueMetaList();
        ArrayList<String> inputFields = new ArrayList<String>(Arrays.asList(prev.getFieldNames()));

        if(fieldMapping.getNumberOfEntries() == table.getColumnCount()){
          remarks.add(new CheckResult(CheckResult.TYPE_RESULT_OK, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.TableSizes.OK" ), stepMeta));
          // check if the input column layout and ColumnStore column layout are compatible (data type wise and that there is a mapping for every columnstore column)
          for(int i = 0; i<table.getColumnCount(); i++){

            columnstore_data_types_t outputColumnType = table.getColumn(i).getType();
            String outputColumnName = table.getColumn(i).getColumnName();

            String mappedInputField = fieldMapping.getTargetInputMappingField(outputColumnName);
            int mappedInputIndex = inputFields.indexOf(mappedInputField);

            if(mappedInputIndex > -1) {
              remarks.add(new CheckResult(CheckResult.TYPE_RESULT_OK, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.MappingAvailable.OK") + " " + table.getColumn(i).getColumnName(), stepMeta));
              int inputColumnType = inputValueTypes.get(i).getType();

              //(input column name, type), (output column name, type)
              String types = "("+prev.getFieldNames()[i]+", "+typeCodes[inputColumnType]+"), ("+table.getColumn(i).getColumnName()+ ", "+outputColumnType.toString()+ ")";

              if(checkCompatibility(inputColumnType, outputColumnType)){
                remarks.add(new CheckResult(CheckResult.TYPE_RESULT_OK, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.ColumnTypeCompatible.OK") + types, stepMeta));
              }else{
                remarks.add(new CheckResult(CheckResult.TYPE_RESULT_ERROR, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.ColumnTypeCompatible.ERROR") + types, stepMeta));
              }
            }else{
              remarks.add(new CheckResult(CheckResult.TYPE_RESULT_ERROR, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.MappingAvailable.ERROR") +  " " + table.getColumn(i).getColumnName(), stepMeta));
            }
          }
        } else{
          remarks.add(new CheckResult(CheckResult.TYPE_RESULT_ERROR, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.TableSizes.ERROR" ), stepMeta));
        }
      }

    } else {
      cr = new CheckResult( CheckResult.TYPE_RESULT_ERROR, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.CheckResult.ReceivingRows.ERROR" ), stepMeta );
      remarks.add( cr );
    }
  }

  /**
   * Checks compatibility of Kettle Input Field Type and ColumnStore output Column Type
   * @param inputColumnType Kettle input type
   * @param outputColumnType ColumnStore output column data type
   * @return true if compatible, otherwise false
   */
  public boolean checkCompatibility(int inputColumnType, columnstore_data_types_t outputColumnType){
    boolean compatible = false;
    switch(inputColumnType) {
      case TYPE_STRING:
        if(outputColumnType == columnstore_data_types_t.DATA_TYPE_TEXT ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_CHAR ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_VARCHAR){
          compatible = true;
        }
        break;
      case TYPE_INTEGER:
        if(outputColumnType == columnstore_data_types_t.DATA_TYPE_BIGINT ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_DECIMAL||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_DOUBLE||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_FLOAT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_INT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_MEDINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_SMALLINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_TINYINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UBIGINT ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UDECIMAL||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UDOUBLE||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UFLOAT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UMEDINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_USMALLINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UTINYINT){
          compatible = true;
        }
        break;
      case TYPE_NUMBER:
        if(outputColumnType == columnstore_data_types_t.DATA_TYPE_BIGINT ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_DECIMAL||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_DOUBLE||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_FLOAT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_INT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_MEDINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_SMALLINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_TINYINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UBIGINT ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UDECIMAL||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UDOUBLE||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UFLOAT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UMEDINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_USMALLINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UTINYINT){
          compatible = true;
        }
        break;
      case TYPE_BIGNUMBER:
        if(outputColumnType == columnstore_data_types_t.DATA_TYPE_BIGINT ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_DECIMAL||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_DOUBLE||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_FLOAT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_INT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_MEDINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_SMALLINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_TINYINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UBIGINT ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UDECIMAL||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UDOUBLE||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UFLOAT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UMEDINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_USMALLINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UTINYINT){
          compatible = true;
        }
        break;
      case TYPE_DATE:
        if(outputColumnType == columnstore_data_types_t.DATA_TYPE_DATE ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_DATETIME){
          compatible = true;
        }
        break;
      case TYPE_TIMESTAMP:
        if(outputColumnType == columnstore_data_types_t.DATA_TYPE_DATE ||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_DATETIME){
          compatible = true;
        }
        break;
      case TYPE_BOOLEAN:
        if(outputColumnType == columnstore_data_types_t.DATA_TYPE_TINYINT||
                outputColumnType == columnstore_data_types_t.DATA_TYPE_UTINYINT){
          compatible = true;
        }
        break;
    }
    return compatible;
  }
}

