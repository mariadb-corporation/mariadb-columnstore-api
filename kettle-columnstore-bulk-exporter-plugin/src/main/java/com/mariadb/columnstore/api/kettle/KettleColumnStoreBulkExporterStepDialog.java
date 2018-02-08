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

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.ShellAdapter;
import org.eclipse.swt.events.ShellEvent;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;
import org.pentaho.di.core.Const;
import org.pentaho.di.i18n.BaseMessages;
import org.pentaho.di.trans.TransMeta;
import org.pentaho.di.ui.core.widget.LabelText;
import org.pentaho.di.ui.trans.step.BaseStepDialog;
import org.pentaho.di.trans.step.BaseStepMeta;
import org.pentaho.di.trans.step.StepDialogInterface;

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

  private Table table;

  private Text tJdbcConnection;

  private Text tLogFile;

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

    // The ModifyListener used on all controls. It will update the meta object to
    // indicate that changes are being made.
    ModifyListener lsMod = new ModifyListener() {
      public void modifyText( ModifyEvent e ) {
        meta.setChanged();
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
    tabItemFieldMapping.setText(BaseMessages.getString(PKG, "KettleColumnStoreBulkExporterPlugin.FieldMapping.Label"));

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
    tblclmnInputStreamField.setText("Input Stream Field");

    TableColumn tblclmnColumnstoreTargetColumn = new TableColumn(table, SWT.NONE);
    tblclmnColumnstoreTargetColumn.setWidth(210);
    tblclmnColumnstoreTargetColumn.setText("ColumnStore Target Column");

    TableColumn tblclmnCompatible = new TableColumn(table, SWT.NONE);
    tblclmnCompatible.setWidth(127);
    tblclmnCompatible.setText("Compatible");

    TableItem tableItem = new TableItem(table, SWT.NONE);
    tableItem.setText("New TableItem");

    TableItem tableItem_1 = new TableItem(table, SWT.NONE);
    tableItem_1.setText("New TableItem");

    Group btnGroupMapping = new Group(compositeFieldMapping, SWT.NONE);
    btnGroupMapping.setLayout(new GridLayout(1, false));

    Button btnGetFields = new Button(btnGroupMapping, SWT.NONE);
    btnGetFields.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false, 1, 1));
    btnGetFields.setText("Get Fields");

    Button btnEditMapping = new Button(btnGroupMapping, SWT.NONE);
    btnEditMapping.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false, 1, 1));
    btnEditMapping.setText("Edit Mapping");

    // TabItem settings
    TabItem tabItemSettings = new TabItem(tabFolder, SWT.FILL);
    tabItemSettings.setText("Settings");

    Composite composite = new Composite(tabFolder, SWT.NONE);
    tabItemSettings.setControl(composite);
    composite.setLayout(new GridLayout(3, false));

    Label lblJdbcConnection = new Label(composite, SWT.NONE);
    lblJdbcConnection.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false, 1, 1));
    lblJdbcConnection.setText("JDBC Connection");

    tJdbcConnection = new Text(composite, SWT.BORDER);
    tJdbcConnection.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));

    Group btnGroupJdbc = new Group(composite, SWT.NONE);
    btnGroupJdbc.setLayout(new FillLayout(SWT.VERTICAL));
    btnGroupJdbc.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, 1, 1));
    btnGroupJdbc.setText("");
    btnGroupJdbc.setBounds(0, 0, 70, 82);

    Button btnEditConnection = new Button(btnGroupJdbc, SWT.NONE);
    btnEditConnection.setText("edit");

    Button btnNewConnection = new Button(btnGroupJdbc, SWT.NONE);
    btnNewConnection.setText("new");

    Label lblLogFile = new Label(composite, SWT.NONE);
    lblLogFile.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false, 1, 1));
    lblLogFile.setText("Log File");

    tLogFile = new Text(composite, SWT.BORDER);
    tLogFile.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));

    Button btnBrowseLogfile = new Button(composite, SWT.NONE);
    btnBrowseLogfile.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false, 1, 1));
    btnBrowseLogfile.setText("browse");

    // OK, cancel and SQL buttons
    wOK = new Button(shell, SWT.NONE);
    FormData fd_btnOk = new FormData();
    wOK.setLayoutData(fd_btnOk);
    wOK.setText("OK");

    wCancel = new Button(shell, SWT.NONE);
    fd_btnOk.top = new FormAttachment(wCancel, 0, SWT.TOP);
    fd_btnOk.right = new FormAttachment(wCancel, -3);
    FormData fd_btnCancel = new FormData();
    wCancel.setLayoutData(fd_btnCancel);
    wCancel.setText("Cancel");

    wSQL = new Button(shell, SWT.NONE);
    fd_btnCancel.top = new FormAttachment(wSQL, 0, SWT.TOP);
    fd_btnCancel.right = new FormAttachment(wSQL, -6);
    FormData fd_btnSql = new FormData();
    fd_btnSql.left = new FormAttachment(0, 359);
    fd_btnSql.bottom = new FormAttachment(100);
    wSQL.setLayoutData(fd_btnSql);
    wSQL.setText("SQL");

    // Add listeners for cancel and OK
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
    wCancel.addListener( SWT.Selection, lsCancel );
    wOK.addListener( SWT.Selection, lsOK );

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
   * This helper method puts the step configuration stored in the meta object
   * and puts it into the dialog controls.
   */
  private void populateDialog() {
    wStepname.selectAll();
    wTargetDatabaseFieldName.setText( meta.getTargetDatabase() );
    wTargetTableFieldName.setText( meta.getTargetTable() );
  }

  /**
   * Called when the user cancels the dialog.  
   */
  private void cancel() {
    // The "stepname" variable will be the return value for the open() method.  
    // Setting to null to indicate that dialog was cancelled.
    stepname = null;
    // Restoring original "changed" flag on the met aobject
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
    // close the SWT dialog window
    dispose();
  }
}

