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

import com.mariadb.columnstore.api.*;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.custom.TextChangeListener;
import org.eclipse.swt.custom.TextChangedEvent;
import org.eclipse.swt.custom.TextChangingEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.ShellAdapter;
import org.eclipse.swt.events.ShellEvent;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;
import org.pentaho.di.core.Const;
import org.pentaho.di.core.DBCache;
import org.pentaho.di.core.SQLStatement;
import org.pentaho.di.core.database.DatabaseMeta;
import org.pentaho.di.core.exception.KettleException;
import org.pentaho.di.core.row.RowMetaInterface;
import org.pentaho.di.core.row.ValueMetaInterface;
import org.pentaho.di.i18n.BaseMessages;
import org.pentaho.di.trans.TransMeta;
import org.pentaho.di.trans.step.StepMeta;
import org.pentaho.di.ui.core.database.dialog.SQLEditor;
import org.pentaho.di.ui.core.dialog.ErrorDialog;
import org.pentaho.di.ui.core.widget.LabelText;
import org.pentaho.di.ui.core.widget.TextVar;
import org.pentaho.di.ui.trans.step.BaseStepDialog;
import org.pentaho.di.trans.step.BaseStepMeta;
import org.pentaho.di.trans.step.StepDialogInterface;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static org.pentaho.di.core.row.ValueMetaInterface.*;

/**
 * This class is part of the demo step plug-in implementation.
 * It demonstrates the basics of developing a plug-in step for PDI.
 *
 * The demo step adds a new string field to the row stream and sets its
 * value to "Hello World!". The user may select the name of the new field.
 *
 * This class is the implementation of StepDialogInterface.
 * Classes implementing this interface need to:
 *
 * - build and open a SWT dialog displaying the step's settings (stored in the step's meta object)
 * - write back any changes the user makes to the step's meta object
 * - report whether the user changed any settings when confirming the dialog
 *
 */
public class KettleColumnStoreBulkExporterStepDialog extends BaseStepDialog implements StepDialogInterface {

  /**
   *  The PKG member is used when looking up internationalized strings.
   *  The properties file with localized keys is expected to reside in
   *  {the package of the class specified}/messages/messages_{locale}.properties
   */
  private static Class<?> PKG = KettleColumnStoreBulkExporterStepMeta.class; // for i18n purposes

  // this is the object that stores the step's settings
  // the dialog reads the settings from it when opening
  // the dialog writes the settings to it when confirmed
  private KettleColumnStoreBulkExporterStepMeta meta;

  // text field holding the name of the field of the target database
  private LabelText wTargetDatabaseFieldName;

  // text field holding the name of the field of the target table
  private LabelText wTargetTableFieldName;

  //table to display mapping
  private Table table;

  //jdbc connection
  private CCombo wConnection;

  //columnstore xml connection configuration file
  private Label	wlColumnStoreXML;
  private Button wbColumnStoreXML;
  private TextVar wColumnStoreXML;
  private FormData fdlColumnStoreXML, fdbColumnStoreXML, fdColumnStoreXML;

  private ColumnStoreDriver d;

  private KettleColumnStoreBulkExporterStepMeta.InputTargetMapping itm;

  /**
   * The constructor should simply invoke super() and save the incoming meta
   * object to a local variable, so it can conveniently read and write settings
   * from/to it.
   *
   * @param parent   the SWT shell to open the dialog in
   * @param in    the meta object holding the step's settings
   * @param transMeta  transformation description
   * @param sname    the step name
   */
  public KettleColumnStoreBulkExporterStepDialog( Shell parent, Object in, TransMeta transMeta, String sname ) {
    super( parent, (BaseStepMeta) in, transMeta, sname );
    meta = (KettleColumnStoreBulkExporterStepMeta) in;
  }

  /**
   * This method is called by Spoon when the user opens the settings dialog of the step.
   * It should open the dialog and return only once the dialog has been closed by the user.
   *
   * If the user confirms the dialog, the meta object (passed in the constructor) must
   * be updated to reflect the new step settings. The changed flag of the meta object must
   * reflect whether the step configuration was changed by the dialog.
   *
   * If the user cancels the dialog, the meta object must not be updated, and its changed flag
   * must remain unaltered.
   *
   * The open() method must return the name of the step after the user has confirmed the dialog,
   * or null if the user cancelled the dialog.
   */
  public String open() {
    // store some convenient SWT variables
    Shell parent = getParent();
    Display display = parent.getDisplay();

    // SWT code for preparing the dialog
    shell = new Shell( parent, SWT.DIALOG_TRIM | SWT.RESIZE | SWT.MIN | SWT.MAX );
    props.setLook( shell );
    setShellImage( shell, meta );

    // Save the value of the changed flag on the meta object. If the user cancels
    // the dialog, it will be restored to this saved value.
    // The "changed" variable is inherited from BaseStepDialog
    changed = meta.hasChanged();

    // Initialize the ColumnStoreDriver
    if(meta.getColumnStoreXML()!=null && !meta.getColumnStoreXML().equals("")) {
      try{
        d = new ColumnStoreDriver(meta.getColumnStoreXML());
      } catch(ColumnStoreException e){
        logError("can't instantiate the ColumnStoreDriver with configuration file: " + meta.getColumnStoreXML(),e);
        d = null;
      }
    } else{
      try{
        d = new ColumnStoreDriver();
      } catch(ColumnStoreException e){
        logError("can't instantiate the default ColumnStoreDriver.", e);
        d = null;
      }
    }

    if(d==null){
      MessageBox mb = new MessageBox(shell, SWT.OK | SWT.ICON_ERROR);
      mb.setMessage(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.XMLConfigurationLoading.Error.DialogMessage"));
      mb.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.XMLConfigurationLoading.Error.DialogTitle"));
      mb.open();
    }

    // Save a local deep copy of the FieldMapping
    itm = new KettleColumnStoreBulkExporterStepMeta.InputTargetMapping(meta.getFieldMapping().getNumberOfEntries());
    for (int i=0; i<meta.getFieldMapping().getNumberOfEntries(); i++){
      itm.setInputFieldMetaData(i, meta.getFieldMapping().getInputStreamField(i));
      itm.setTargetColumnStoreColumn(i, meta.getFieldMapping().getTargetColumnStoreColumn(i));
    }

    // The ModifyListener used on all controls. It will update the meta object to
    // indicate that changes are being made.
    ModifyListener lsMod = new ModifyListener() {
      public void modifyText( ModifyEvent e ) {
        meta.setChanged();
        updateTableView();
      }
    };

    // ------------------------------------------------------- //
    // SWT code for building the actual settings dialog        //
    // ------------------------------------------------------- //
    FormLayout formLayout = new FormLayout();
    formLayout.marginWidth = Const.FORM_MARGIN;
    formLayout.marginHeight = Const.FORM_MARGIN;
    shell.setLayout( formLayout );
    shell.setText( BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.Shell.Title" ) );
    int middle = props.getMiddlePct();
    int margin = Const.MARGIN;

    // Stepname line
    wlStepname = new Label( shell, SWT.RIGHT );
    wlStepname.setText( BaseMessages.getString( PKG, "System.Label.StepName" ) );
    props.setLook( wlStepname );
    fdlStepname = new FormData();
    fdlStepname.left = new FormAttachment( 0, 0 );
    fdlStepname.right = new FormAttachment( middle, -margin );
    fdlStepname.top = new FormAttachment( 0, margin );
    wlStepname.setLayoutData( fdlStepname );

    wStepname = new Text( shell, SWT.SINGLE | SWT.LEFT | SWT.BORDER );
    wStepname.setText( stepname );
    props.setLook( wStepname );
    wStepname.addModifyListener( lsMod );
    fdStepname = new FormData();
    fdStepname.left = new FormAttachment( middle, 0 );
    fdStepname.top = new FormAttachment( 0, margin );
    fdStepname.right = new FormAttachment( 100, 0 );
    wStepname.setLayoutData( fdStepname );

    // Target database line
    wTargetDatabaseFieldName = new LabelText( shell, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.TargetDatabaseField.Label" ), null );
    props.setLook( wTargetDatabaseFieldName );
    wTargetDatabaseFieldName.addModifyListener( lsMod );
    FormData fdValTargetDatabase = new FormData();
    fdValTargetDatabase.left = new FormAttachment( 0, 0 );
    fdValTargetDatabase.right = new FormAttachment( 100, 0 );
    fdValTargetDatabase.top = new FormAttachment( wStepname, margin );
    wTargetDatabaseFieldName.setLayoutData( fdValTargetDatabase );

    // Target table line
    wTargetTableFieldName = new LabelText( shell, BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.TargetTableField.Label" ), null );
    props.setLook( wTargetTableFieldName );
    wTargetTableFieldName.addModifyListener( lsMod );
    FormData fdValTargetTable = new FormData();
    fdValTargetTable.left = new FormAttachment( 0, 0 );
    fdValTargetTable.right = new FormAttachment( 100, 0 );
    fdValTargetTable.top = new FormAttachment( wTargetDatabaseFieldName, margin );
    wTargetTableFieldName.setLayoutData( fdValTargetTable );

    // TabFolder as container for TabItems
    TabFolder tabFolder = new TabFolder( shell, SWT.FILL);
    FormData fTabFolder = new FormData();
    fTabFolder.top = new FormAttachment(wTargetTableFieldName, 18);
    fTabFolder.right = new FormAttachment(wTargetTableFieldName, 0, SWT.RIGHT);
    fTabFolder.bottom = new FormAttachment(100, -37);
    fTabFolder.left = new FormAttachment(0, 5);
    tabFolder.setLayoutData(fTabFolder);

    // TabItem field mapping
    TabItem tabItemFieldMapping = new TabItem(tabFolder, SWT.NONE);
    tabItemFieldMapping.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.FieldMapping.Tab"));

    Composite compositeFieldMapping = new Composite(tabFolder, SWT.NONE);
    tabItemFieldMapping.setControl(compositeFieldMapping);
    GridLayout g = new GridLayout();
    g.numColumns = 2;
    compositeFieldMapping.setLayout(g);

    table = new Table(compositeFieldMapping, SWT.BORDER | SWT.FULL_SELECTION);
    table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
    table.setHeaderVisible(true);
    table.setLinesVisible(true);

    TableColumn tblclmnInputStreamField = new TableColumn(table, SWT.NONE);
    tblclmnInputStreamField.setWidth(210);
    tblclmnInputStreamField.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.InputStreamField.Tabular"));

    TableColumn tblclmnColumnstoreTargetColumn = new TableColumn(table, SWT.NONE);
    tblclmnColumnstoreTargetColumn.setWidth(210);
    tblclmnColumnstoreTargetColumn.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.TargetColumn.Tabular"));

    TableColumn tblclmnCompatible = new TableColumn(table, SWT.NONE);
    tblclmnCompatible.setWidth(127);
    tblclmnCompatible.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.Compatible.Tabular"));

    Group btnGroupMapping = new Group(compositeFieldMapping, SWT.NONE);
    btnGroupMapping.setLayout(new GridLayout(1, false));

    Button btnGetFields = new Button(btnGroupMapping, SWT.NONE);
    btnGetFields.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false, 1, 1));
    btnGetFields.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.Button.MapAllInputs"));

    btnGetFields.addListener(SWT.Selection, new Listener() { public void handleEvent(Event arg0) {mapAllInputs();}});

    Button btnEditMapping = new Button(btnGroupMapping, SWT.NONE);
    btnEditMapping.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false, 1, 1));
    btnEditMapping.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.Button.CustomMapping"));
    btnEditMapping.addListener(SWT.Selection, new Listener() { 	public void handleEvent(Event arg0) { customMapping();}});

    // TabItem settings
    TabItem tabItemSettings = new TabItem(tabFolder, SWT.FILL);
    tabItemSettings.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.Settings.Tab"));

    Composite composite = new Composite(tabFolder, SWT.NONE);
    tabItemSettings.setControl(composite);
    composite.setLayout(new FormLayout());

    // Connection line
    wConnection = addConnectionLine(composite, composite, middle, margin);
    if (meta.getDatabaseMeta()==null && transMeta.nrDatabases()==1) wConnection.select(0);
    wConnection.addModifyListener(lsMod);

    // ColumnStore.xml line
    wlColumnStoreXML = new Label(composite, SWT.RIGHT);
    wlColumnStoreXML.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.Label.ColumnStoreXML"));
    props.setLook(wlColumnStoreXML);
    fdlColumnStoreXML = new FormData();
    fdlColumnStoreXML.left = new FormAttachment(0, 0);
    fdlColumnStoreXML.top = new FormAttachment(wConnection, margin);
    fdlColumnStoreXML.right = new FormAttachment(middle, -margin);
    wlColumnStoreXML.setLayoutData(fdlColumnStoreXML);
    wbColumnStoreXML = new Button(composite, SWT.PUSH | SWT.CENTER);
    props.setLook(wbColumnStoreXML);
    wbColumnStoreXML.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.Button.Browse"));
    fdbColumnStoreXML = new FormData();
    fdbColumnStoreXML.right = new FormAttachment(100, 0);
    fdbColumnStoreXML.top = new FormAttachment(wConnection, margin);
    wbColumnStoreXML.setLayoutData(fdbColumnStoreXML);
    wColumnStoreXML = new TextVar(transMeta, composite, SWT.READ_ONLY | SWT.SINGLE | SWT.LEFT | SWT.BORDER);
    props.setLook(wColumnStoreXML);
    wColumnStoreXML.addModifyListener(lsMod);
    fdColumnStoreXML = new FormData();
    fdColumnStoreXML.left = new FormAttachment(middle, 0);
    fdColumnStoreXML.top = new FormAttachment(wConnection, margin);
    fdColumnStoreXML.right = new FormAttachment(wbColumnStoreXML, -margin);
    wColumnStoreXML.setLayoutData(fdColumnStoreXML);

    wbColumnStoreXML.addSelectionListener(new SelectionAdapter()
    {
      public void widgetSelected(SelectionEvent e)
      {
        FileDialog dialog = new FileDialog(shell, SWT.OPEN);
        dialog.setFilterExtensions(new String[]{"*"});
        if (wColumnStoreXML.getText() != null)
        {
          dialog.setFileName(wColumnStoreXML.getText());
        }
        if (dialog.open() != null) {
          try{
            String path = dialog.getFilterPath() + Const.FILE_SEPARATOR + dialog.getFileName();
            ColumnStoreDriver tmp = new ColumnStoreDriver(path);
            wColumnStoreXML.setText(path);
            updateColumnStoreDriver();
            updateTableView();
          } catch(ColumnStoreException ex){
            MessageBox mb = new MessageBox(shell, SWT.OK | SWT.ICON_ERROR);
            mb.setMessage(ex.getMessage());
            mb.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.XMLConfigurationPicker.Error.DialogTitle"));
            mb.open();
          }
        }
      }
    });

    // OK, cancel and SQL buttons
    wOK = new Button( shell, SWT.PUSH );
    wOK.setText( BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.Button.OK" ) );
    wCancel = new Button( shell, SWT.PUSH );
    wCancel.setText( BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.Button.Cancel" ) );
    wSQL = new Button( shell, SWT.PUSH );
    wSQL.setText( BaseMessages.getString( PKG, "KettleColumnStoreBulkExporterPlugin.Button.SQL" ) );
    setButtonPositions( new Button[] { wOK, wCancel, wSQL }, margin, tabFolder );

    // Add listeners for cancel, OK and SQL
    lsCancel = new Listener() {
      public void handleEvent( Event e ) {
        cancel();
      }
    };
    lsOK = new Listener() {
      public void handleEvent( Event e ) {
        ok();
      }
    };
    lsSQL = new Listener() {
      public void handleEvent( Event e ) {
        sqlBtnHit();
      }
    };
    wCancel.addListener( SWT.Selection, lsCancel );
    wOK.addListener( SWT.Selection, lsOK );
    wSQL.addListener(SWT.Selection, lsSQL);

    // default listener (for hitting "enter")
    lsDef = new SelectionAdapter() {
      public void widgetDefaultSelected( SelectionEvent e ) {
        ok();
      }
    };
    wStepname.addSelectionListener( lsDef );
    wTargetDatabaseFieldName.addSelectionListener( lsDef );
    wTargetTableFieldName.addSelectionListener( lsDef );

    // Detect X or ALT-F4 or something that kills this window and cancel the dialog properly
    shell.addShellListener( new ShellAdapter() {
      public void shellClosed( ShellEvent e ) {
        cancel();
      }
    } );

    // Set/Restore the dialog size based on last position on screen
    // The setSize() method is inherited from BaseStepDialog
    setSize();

    // populate the dialog with the values from the meta object
    populateDialog();

    // restore the changed flag to original value, as the modify listeners fire during dialog population  
    meta.setChanged( changed );

    // open dialog and enter event loop  
    shell.open();
    while ( !shell.isDisposed() ) {
      if ( !display.readAndDispatch() ) {
        display.sleep();
      }
    }

    // at this point the dialog has closed, so either ok() or cancel() have been executed
    // The "stepname" variable is inherited from BaseStepDialog
    return stepname;
  }

  /**
   * Updates the ColumnStoreDriver from wColumnStoreXML
   */
  private void updateColumnStoreDriver(){
    if(wColumnStoreXML.getText() != null && !wColumnStoreXML.getText().equals("")) {
      try{
        d = new ColumnStoreDriver(wColumnStoreXML.getText());
      } catch(ColumnStoreException e){
        logError("can't instantiate the ColumnStoreDriver with configuration file: " + wColumnStoreXML.getText(),e);
        d = null;
      }
    } else{
      try{
        d = new ColumnStoreDriver();
      } catch(ColumnStoreException e){
        logError("can't instantiate the default ColumnStoreDriver.", e);
        d = null;
      }
    }
  }

  /**
   * Function is invoked when button "Map all Inputs" is hit.
   * It maps all input fields to a new ColumnStore columns of adequate type.
   */
  private void mapAllInputs() {

    StepMeta stepMeta = transMeta.findStep(stepname);

    KettleColumnStoreBulkExporterStepMeta.InputTargetMapping oldItm = new KettleColumnStoreBulkExporterStepMeta.InputTargetMapping(itm.getNumberOfEntries());
    for (int i=0; i<itm.getNumberOfEntries(); i++){
      oldItm.setInputFieldMetaData(i,itm.getInputStreamField(i));
      oldItm.setTargetColumnStoreColumn(i,itm.getTargetColumnStoreColumn(i));
    }

    try {
      RowMetaInterface row = transMeta.getPrevStepFields(stepMeta);

      List<ValueMetaInterface> inputValueTypes = row.getValueMetaList();

      itm = new KettleColumnStoreBulkExporterStepMeta.InputTargetMapping(inputValueTypes.size());

      for(int i=0; i< inputValueTypes.size(); i++){
        itm.setInputFieldMetaData(i, inputValueTypes.get(i).getName());
        itm.setTargetColumnStoreColumn(i, inputValueTypes.get(i).getName());
      }
    }catch(KettleException e){
      logError("Can't get fields from previous step.", e);
    }

    boolean changed = false;
    if(itm.getNumberOfEntries() == oldItm.getNumberOfEntries()){
      for(int i=0; i<itm.getNumberOfEntries(); i++){
        if(!itm.getInputStreamField(i).equals(oldItm.getInputStreamField(i)) ||
                !itm.getTargetColumnStoreColumn(i).equals(oldItm.getTargetColumnStoreColumn(i))){
          changed = true;
          break;
        }
      }
    }else{
      changed = true;
    }
    if (changed){
      meta.setChanged();
    }

    updateTableView();
  }

  /**
   * Updates the table based on the current itm
   */
  private void updateTableView(){
    table.removeAll();
    int[] inputTypes = new int[itm.getNumberOfEntries()];
    columnstore_data_types_t[] outputTypes = new columnstore_data_types_t[itm.getNumberOfEntries()];

    //get datatypes of mapped input stream fields
    try{
      RowMetaInterface row = transMeta.getPrevStepFields(stepMeta);

      List<ValueMetaInterface> inputValueTypes = row.getValueMetaList();
      ArrayList<String> inputValueFields = new ArrayList<String>(Arrays.asList(row.getFieldNames()));

      for (int i=0; i<itm.getNumberOfEntries(); i++){
        int field = inputValueFields.indexOf(itm.getInputStreamField(i));
        if(field >= 0) { //field was found
          inputTypes[i] = inputValueTypes.get(field).getType();
        } else{ //input field was not found, set type to -1
          inputTypes[i] = -1;
        }
      }
    }
    catch(KettleException e){
      logError("Can't get fields from previous step", e);
    }

    //get datatypes of mapped output columnstore columns
    if(d != null) {
      try {
        ColumnStoreSystemCatalog c = d.getSystemCatalog();
        ColumnStoreSystemCatalogTable t = c.getTable(wTargetDatabaseFieldName.getText(), wTargetTableFieldName.getText());
        for (int i = 0; i < itm.getNumberOfEntries(); i++) {
          try {
            outputTypes[i] = t.getColumn(itm.getTargetColumnStoreColumn(i).toLowerCase()).getType();
          } catch (ColumnStoreException ex) {
            logDetailed("Can't find column " + itm.getTargetColumnStoreColumn(i) + " in table " + wTargetTableFieldName.getText());
          }
        }
      } catch (ColumnStoreException e) {
        logDetailed("Can't access the ColumnStore table " + wTargetDatabaseFieldName.getText() + " " + wTargetTableFieldName.getText());
      }
    }

    //update the entries in the table
    for (int i=0; i<itm.getNumberOfEntries(); i++){
      TableItem tableItem = new TableItem(table, SWT.NONE);
      if(inputTypes[i] > -1) {
        tableItem.setText(0, itm.getInputStreamField(i) + " <" + typeCodes[inputTypes[i]] + ">");
      } else {
        tableItem.setText(0, itm.getInputStreamField(i) + " <None>");
      }
      if(outputTypes[i] != null) {
        tableItem.setText(1, itm.getTargetColumnStoreColumn(i) + " <" + outputTypes[i].toString().substring(10) + ">");
      }
      else{
        tableItem.setText(1, itm.getTargetColumnStoreColumn(i) + " <None>");
      }
      if(inputTypes[i] > -1 && outputTypes[i] != null && meta.checkCompatibility(inputTypes[i],outputTypes[i])){
        tableItem.setText(2, "yes");
      }else{
        tableItem.setText(2, "no");
      }
    }
    table.update();
  }

  /**
   * Function is invoked when button Custom Mapping is hit
   */
  private void customMapping(){
    logDebug("BUTTON CUSTOM MAPPING HIT");
  }

  /**
   * Function is invoked when the SQL button is hit
   */
  private void sqlBtnHit(){
    try
    {
      //Use a copy of meta here as meta won't be updated until OK is clicked
      KettleColumnStoreBulkExporterStepMeta metaCopy = new KettleColumnStoreBulkExporterStepMeta();
      metaCopy.setDatabaseMeta(transMeta.findDatabase(wConnection.getText()));
      metaCopy.setFieldMapping(itm);
      metaCopy.setTargetDatabase(wTargetDatabaseFieldName.getText());
      metaCopy.setTargetTable(wTargetTableFieldName.getText());

      StepMeta stepMeta = new StepMeta(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.StepMeta.Title"), wStepname.getText(), metaCopy); //$NON-NLS-1$
      RowMetaInterface prev = transMeta.getPrevStepFields(stepname);

      SQLStatement sql = metaCopy.getSQLStatements(transMeta, stepMeta, prev, repository, metaStore);
      if (!sql.hasError())
      {
        if (sql.hasSQL())
        {
          SQLEditorC sqledit = new SQLEditorC(shell, SWT.NONE, metaCopy.getDatabaseMeta(), transMeta.getDbCache(), sql.getSQL());
          sqledit.open();
        }
        else
        {
          MessageBox mb = new MessageBox(shell, SWT.OK | SWT.ICON_INFORMATION);
          mb.setMessage(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.NoSQLNeeds.DialogMessage")); //$NON-NLS-1$
          mb.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.NoSQLNeeds.DialogTitle")); //$NON-NLS-1$
          mb.open();
        }
      }
      else
      {
        MessageBox mb = new MessageBox(shell, SWT.OK | SWT.ICON_ERROR);
        mb.setMessage(sql.getError());
        mb.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.SQLError.DialogTitle")); //$NON-NLS-1$
        mb.open();
      }
    }
    catch (KettleException ke)
    {
      new ErrorDialog(shell, BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.CouldNotBuildSQL.DialogTitle"), //$NON-NLS-1$
              BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.CouldNotBuildSQL.DialogMessage"), ke); //$NON-NLS-1$
    }
  }

  /**
   * This helper method puts the step configuration stored in the meta object
   * and puts it into the dialog controls.
   */
  private void populateDialog() {
    wStepname.selectAll();
    wTargetDatabaseFieldName.setText( meta.getTargetDatabase() );
    wTargetTableFieldName.setText( meta.getTargetTable() );
    if (meta.getDatabaseMeta() != null)
      wConnection.setText(meta.getDatabaseMeta().getName());
    else {
      if (transMeta.nrDatabases() == 1) {
        wConnection.setText(transMeta.getDatabase(0).getName());
      }
    }
    if(meta.getColumnStoreXML()==null){
      wColumnStoreXML.setText("");
    }else{
      wColumnStoreXML.setText( meta.getColumnStoreXML() );
    }
  }

  /**
   * Called when the user cancels the dialog.  
   */
  private void cancel() {
    // The "stepname" variable will be the return value for the open() method.  
    // Setting to null to indicate that dialog was cancelled.
    stepname = null;
    // Restoring original "changed" flag on the meta object
    meta.setChanged( changed );
    // close the SWT dialog window
    dispose();
  }

  /**
   * Called when the user confirms the dialog
   */
  private void ok() {
    // The "stepname" variable will be the return value for the open() method.  
    // Setting to step name from the dialog control
    stepname = wStepname.getText();
    // Setting the  settings to the meta object
    meta.setTargetDatabase( wTargetDatabaseFieldName.getText() );
    meta.setTargetTable( wTargetTableFieldName.getText() );

    // set the jdbc database connection
    meta.setDatabaseMeta(transMeta.findDatabase(wConnection.getText()));

    // set the columnstore xml file location
    meta.setColumnStoreXML( wColumnStoreXML.getText() );

    // Set the field mapping
    meta.setFieldMapping(itm);

    // close the SWT dialog window
    dispose();
  }

  /**
   * Adapted class of SQLEditor to updateTableView after execution
   */
  private class SQLEditorC extends SQLEditor{
    public SQLEditorC(Shell parent, int style, DatabaseMeta ci, DBCache dbc, String sql) {
      super(parent, style, ci, dbc, sql);
    }
    @Override
    protected void refreshExecutionResults() {
      super.refreshExecutionResults();
      updateColumnStoreDriver(); //temporary fix for MCOL-1218
      updateTableView();
    }
  }
}
