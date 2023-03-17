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

#ifndef _IGNITE_ODBC_QUERY_COLUMN_METADATA_QUERY
#define _IGNITE_ODBC_QUERY_COLUMN_METADATA_QUERY

#include "ignite/odbc/query/query.h"
#include "ignite/odbc/query/data_query.h"
#include "ignite/odbc/query/table_metadata_query.h"

namespace ignite {
namespace odbc {
/** Connection forward-declaration. */
class Connection;

namespace query {
/**
 * Query.
 */
class ColumnMetadataQuery : public Query {
 public:
  /**
   * Constructor.
   *
   * @param diag Diagnostics collector.
   * @param connection Associated connection.
   * @param schema Schema search pattern.
   * @param table Table search pattern.
   * @param column Column search pattern.
   */
  ColumnMetadataQuery(diagnostic::DiagnosableAdapter& diag,
                      Connection& connection,
                      const boost::optional< std::string >& catalog,
                      const boost::optional< std::string >& schema,
                      const boost::optional< std::string >& table, 
                      const boost::optional< std::string >& column);

  /**
   * Destructor.
   */
  virtual ~ColumnMetadataQuery();

  /**
   * Execute query.
   *
   * @return True on success.
   */
  virtual SqlResult::Type Execute();

  /**
   * Get column metadata.
   *
   * @return Column metadata.
   */
  virtual const meta::ColumnMetaVector* GetMeta();

  /**
   * Fetch next result row to application buffers.
   *
   * @return Operation result.
   */
  virtual SqlResult::Type FetchNextRow(app::ColumnBindingMap& columnBindings);

  /**
   * Get data of the specified column in the result set.
   *
   * @param columnIdx Column index.
   * @param buffer Buffer to put column data to.
   * @return Operation result.
   */
  virtual SqlResult::Type GetColumn(uint16_t columnIdx,
                                    app::ApplicationDataBuffer& buffer);

  /**
   * Close query.
   *
   * @return True on success.
   */
  virtual SqlResult::Type Close();

  /**
   * Check if data is available.
   *
   * @return True if data is available.
   */
  virtual bool DataAvailable() const;

  /**
   * Get number of rows affected by the statement.
   *
   * @return Number of rows affected by the statement.
   */
  virtual int64_t AffectedRows() const;

  /**
   * Move to the next result set.
   *
   * @return Operation result.
   */
  virtual SqlResult::Type NextResultSet();

 private:
  IGNITE_NO_COPY_ASSIGNMENT(ColumnMetadataQuery);

  /**
   * Get columns metadata with database search pattern
   *
   * @param databasePattern Database pattern
   * @param databaseType ResultColumn Type.
   *
   * @return Operation result.
   */
  SqlResult::Type GetColumnsWithDatabaseSearchPattern(
      const boost::optional< std::string >& databasePattern,
      TableMetadataQuery::ResultColumn::Type databaseType);

  /**
   * Make get columns metadata requets and use response to set internal state.
   *
   * @return Operation result.
   */
  SqlResult::Type MakeRequestGetColumnsMeta();

  /**
   * Make get columns metadata requets and use response to set internal state for a table.
   *
   * @param databaseName Database name
   * @param tableName Table name
   * 
   * @return Operation result.
   */
  SqlResult::Type MakeRequestGetColumnsMetaPerTable(
      const std::string& databaseName, const std::string& tableName);

  /** Connection associated with the statement. */
  Connection& connection;

  /** Catalog search pattern. */
  boost::optional< std::string > catalog;

  /** Schema search pattern. */
  boost::optional< std::string > schema;

  /** Table search pattern. */
  boost::optional< std::string > table;

  /** Column search pattern. */
  boost::optional< std::string > column;

  /** Query executed. */
  bool executed;

  /** Fetched flag. */
  bool fetched;

  /** Fetched metadata. */
  meta::ColumnMetaVector meta;

  /** Metadata cursor. */
  meta::ColumnMetaVector::iterator cursor;

  /** Columns metadata. */
  meta::ColumnMetaVector columnsMeta;

  /** DataQuery pointer for "describe" command to run **/
  std::shared_ptr< DataQuery > dataQuery_;

  /** TableMetadataQuery pointer for fetching table **/
  std::shared_ptr< TableMetadataQuery > tableMetadataQuery_;
};
}  // namespace query
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_QUERY_COLUMN_METADATA_QUERY
