/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _IGNITE_ODBC_META_TABLE_META
#define _IGNITE_ODBC_META_TABLE_META

#include <stdint.h>

#include <string>

#include <ignite/odbc/common/concurrent.h>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "ignite/odbc/impl/binary/binary_reader_impl.h"
#include "ignite/odbc/result_set.h"
#include "ignite/odbc/utility.h"
#include "ignite/odbc/ts_error.h"

#include <aws/timestream-write/TimestreamWriteClient.h>

using Aws::TimestreamWrite::Model::Database;
using Aws::TimestreamWrite::Model::Table;
using ignite::odbc::ResultSet;
using ignite::odbc::common::concurrent::SharedPointer;

namespace ignite {
namespace odbc {
namespace meta {
/**
 * Table metadata.
 */
class TableMeta {
 public:
  /**
   * Default constructor.
   */
  TableMeta() {
    // No-op.
  }

  /**
   * Constructor.
   *
   * @param catalogName Catalog name.
   * @param schemaName Schema name.
   * @param tableName Table name.
   * @param tableType Table type.
   */
  TableMeta(const std::string& catalogName, const std::string& schemaName,
            const std::string& tableName, const std::string& tableType)
      : catalogName(catalogName),
        schemaName(schemaName),
        tableName(tableName),
        tableType(tableType) {
    // No-op.
  }

  /**
   * Destructor.
   */
  ~TableMeta() {
    // No-op.
  }

  /**
   * Copy constructor.
   */
  TableMeta(const TableMeta& other)
      : catalogName(other.catalogName),
        schemaName(other.schemaName),
        tableName(other.tableName),
        tableType(other.tableType),
        remarks(other.remarks) {
    // No-op.
  }

  /**
   * Copy operator.
   */
  TableMeta& operator=(const TableMeta& other) {
    catalogName = other.catalogName;
    schemaName = other.schemaName;
    tableName = other.tableName;
    tableType = other.tableType;
    remarks = other.remarks;

    return *this;
  }

  /**
   * Read table object from AWS SDK.
   * @param tb Table.
   */
  void Read(Table& tb);

  /**
   * Read database object from AWS SDK.
   * @param db Database.
   */
  void Read(Database& db);

  /**
   * Read table type
   * @param tbType string for table type.
   */
  void Read(std::string& tbType);

  /**
   * Get catalog name.
   * @return Catalog name.
   */
  const boost::optional< std::string >& GetCatalogName() const {
    return catalogName;
  }

  /**
   * Get schema name.
   * @return Schema name.
   */
  const boost::optional< std::string >& GetSchemaName() const {
    return schemaName;
  }

  /**
   * Get table name.
   * @return Table name.
   */
  const boost::optional< std::string >& GetTableName() const {
    return tableName;
  }

  /**
   * Get table type.
   * @return Table type.
   */
  const boost::optional< std::string >& GetTableType() const {
    return tableType;
  }

  /**
   * Get the remarks.
   * @return Remarks.
   */
  const boost::optional< std::string >& GetRemarks() const {
    return remarks;
  }

 private:
  /** Catalog name. */
  boost::optional< std::string > catalogName;

  /** Schema name. */
  boost::optional< std::string > schemaName;

  /** Table name. */
  boost::optional< std::string > tableName;

  /** Table type. */
  boost::optional< std::string > tableType;

  /** Remarks */
  boost::optional< std::string > remarks;
};

/** Table metadata vector alias. */
typedef std::vector< TableMeta > TableMetaVector;

/**
 * Read tables metadata collection.
 * @param tableName table name for result set
 * @param tbVector Aws::Vector< Table >.
 * @param meta Collection.
 */
void ReadTableMetaVector(const std::string& tableName,
                         const Aws::Vector< Table >& tbVector,
                         TableMetaVector& meta);

/**
 * Read database metadata collection.
 * Will make meta contain a list of valid schemas for the data source
 * @param dbVector Aws::Vector< Database >.
 * @param meta Collection.
 */
void ReadDatabaseMetaVector(const Aws::Vector< Database >& tbVector,
                            TableMetaVector& meta);
}  // namespace meta
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_META_TABLE_META
