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

import com.mariadb.columnstore.api.ColumnStoreBulkInsert;
import com.mariadb.columnstore.api.ColumnStoreDriver;
import com.mariadb.columnstore.api.ColumnStoreSystemCatalog;
import com.mariadb.columnstore.api.ColumnStoreSystemCatalogTable;
import org.pentaho.di.core.row.RowMetaInterface;
import org.pentaho.di.core.row.ValueMetaInterface;
import org.pentaho.di.trans.step.BaseStepData;
import org.pentaho.di.trans.step.StepDataInterface;

import java.util.List;

/**
 *
 * This class is the implementation of StepDataInterface.
 *   
 * Implementing classes inherit from BaseStepData, which implements the entire
 * interface completely. 
 * 
 * In addition classes implementing this interface usually keep track of
 * per-thread resources during step execution. Typical examples are:
 * result sets, temporary data, caching indexes, etc.
 *   
 * This implementation stores information about the output row structure,
 * the target mapping, and ColumnStoreDriver to execute the
 * KettleColumnStoreBulkExporterStep.
 *   
 */
public class KettleColumnStoreBulkExporterStepData extends BaseStepData implements StepDataInterface {

  RowMetaInterface rowMeta;
  List<ValueMetaInterface> rowValueTypes;

  ColumnStoreDriver d;
  ColumnStoreBulkInsert b;
  ColumnStoreSystemCatalog catalog;
  ColumnStoreSystemCatalogTable table;
  int targetColumnCount;

  int[] targetInputMapping;

  public KettleColumnStoreBulkExporterStepData() {
    super();
  }
}

