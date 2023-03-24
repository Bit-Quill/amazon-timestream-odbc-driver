/*
 * Copyright <2022> Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 */

#ifndef _TIMESTREAM_ODBC_META_TABLE_META
#define _TIMESTREAM_ODBC_META_TABLE_META

#include <string>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "timestream/odbc/utility.h"

namespace timestream {
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
}  // namespace meta
}  // namespace odbc
}  // namespace timestream

#endif  //_TIMESTREAM_ODBC_META_TABLE_META
