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

#ifndef _IGNITE_ODBC_QUERY_TABLE_METADATA_QUERY
#define _IGNITE_ODBC_QUERY_TABLE_METADATA_QUERY

#include "ignite/odbc/meta/table_meta.h"
#include "ignite/odbc/query/query.h"
#include "ignite/odbc/statement.h"

#include <regex>

using Aws::TimestreamWrite::Model::ListDatabasesOutcome;
using Aws::TimestreamWrite::Model::ListTablesOutcome;

namespace ignite {
namespace odbc {
/** Connection forward-declaration. */
class Connection;

namespace query {
/**
 * Query.
 */
class TableMetadataQuery : public Query {
 public:

  struct ResultColumn {
    enum Type {
      /** Catalog name. NULL if not applicable to the data source. */
      TABLE_CAT = 1,

      /** Schema name. NULL if not applicable to the data source. */
      TABLE_SCHEM,

      /** Table name. */
      TABLE_NAME,

      /** Table type. */
      TABLE_TYPE,

      /** A description of the column. */
      REMARKS
    };
  };

  /**
   * Constructor.
   *
   * @param diag Diagnostics collector.
   * @param connection Associated connection.
   * @param catalog Catalog search pattern.
   * @param schema Schema search pattern.
   * @param table Table search pattern.
   * @param tableType Table type search pattern.
   */
  TableMetadataQuery(diagnostic::DiagnosableAdapter& diag,
                     Connection& connection,
                     const boost::optional< std::string >& catalog,
                     const boost::optional< std::string >& schema,
                     const std::string& table,
                     const boost::optional< std::string >& tableType);

  /**
   * Destructor.
   */
  virtual ~TableMetadataQuery();

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
  IGNITE_NO_COPY_ASSIGNMENT(TableMetadataQuery);

  /**
   * Make get columns metadata requets and use response to set internal state.
   *
   * @return True on success.
   */
  SqlResult::Type MakeRequestGetTablesMeta();

  /**
   * Get the list of all schemas (databases).
   * 
   * @return Operation result
   */
  SqlResult::Type getSchemas();

  /**
   * Get the list of all tables.
   * 
   * @return Operation result
   */
  SqlResult::Type getTables();

  /**
   * Update metadata by adding tables from database named databaseName
   *
   * @param database name
   * @return Operation result
   */
  SqlResult::Type getTablesWithDBName(std::string& databaseName);

  /**
   * Check and log error from ListDatabasesOutcome
   * Only run this function if dbOutcome has error
   * 
   * @param dbOutcome ListDatabasesOutcome
   */
  SqlResult::Type checkOutcomeError(ListDatabasesOutcome const& dbOutcome);


  /**
   * Check and log error from ListTablesOutcome
   * Only run this function if tbOutcome has error
   *
   * @param dbOutcome ListTablesOutcome
   */
  SqlResult::Type checkOutcomeError(ListTablesOutcome const& tbOutcome);

  /**
   * Check if meta object is empty and print logs of it.
   */
  SqlResult::Type checkMeta();

  /**
   * Remove outer matching quotes from a string. They can be either single (')
   * or double (") quotes. They must be the left- and right-most characters in
   * the string.
   *
   * @return the string with matched quotes removed.
   */
  std::string dequote(const std::string& s);

  /** Connection associated with the statement. */
  Connection& connection;

  /** Catalog search pattern. */
  boost::optional< std::string > catalog;

  /** Schema search pattern. */
  boost::optional< std::string > schema;

  /** Table search pattern. */
  std::string table;

  /** Table type search pattern. */
  boost::optional< std::string > tableType;

  /** Query executed. */
  bool executed;

  /** Fetched flag. */
  bool fetched;

  /** Return a list of catalogs flag. */
  bool all_catalogs;

  /** Return a list of schemas flag. */
  bool all_schemas;

  /** Return a list of supported table types flag. */
  bool all_table_types;
  
  /** Fetched metadata. */
  meta::TableMetaVector meta;

  /** Metadata cursor. */
  meta::TableMetaVector::iterator cursor;

  /** Columns metadata. */
  meta::ColumnMetaVector columnsMeta;

  std::regex schema_regex;
};
}  // namespace query
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_QUERY_TABLE_METADATA_QUERY
