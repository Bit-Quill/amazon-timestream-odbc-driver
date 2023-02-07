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
#include "ignite/odbc/query/table_metadata_query.h"

#include <aws/timestream-write/model/ListTablesRequest.h>
#include <aws/timestream-write/model/ListTablesResult.h>
#include <aws/timestream-write/model/ListDatabasesRequest.h>
#include <aws/timestream-write/model/ListDatabasesResult.h>
#include <aws/timestream-write/TimestreamWriteClient.h>
#include <aws/timestream-query/model/ScalarType.h>

#include <vector>

#include "ignite/odbc/common/concurrent.h"
#include "ignite/odbc/connection.h"
#include "ignite/odbc/ignite_error.h"
#include "ignite/odbc/impl/binary/binary_common.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/message.h"
#include "ignite/odbc/odbc_error.h"
#include "ignite/odbc/type_traits.h"

using Aws::TimestreamQuery::Model::ScalarType;
using ignite::odbc::IgniteError;
using ignite::odbc::common::concurrent::SharedPointer;
using Aws::TimestreamWrite::Model::Database;
using Aws::TimestreamWrite::Model::ListDatabasesRequest;
using Aws::TimestreamWrite::Model::ListDatabasesResult;

namespace ignite {
namespace odbc {
namespace query {
TableMetadataQuery::TableMetadataQuery(
    diagnostic::DiagnosableAdapter& diag, Connection& connection,
    const boost::optional< std::string >& catalog,
    const boost::optional< std::string >& schema, const std::string& table,
    const boost::optional< std::string >& tableType)
    : Query(diag, QueryType::TABLE_METADATA),
      connection(connection),
      catalog(catalog),
      schema(schema),
      table(table),
      tableType(tableType),
      executed(false),
      fetched(false),
      all_schemas(false),
      all_catalogs(false),
      all_table_types(false),
      meta(),
      columnsMeta() {
  using namespace ignite::odbc::impl::binary;
  using namespace ignite::odbc::type_traits;

  using meta::ColumnMeta;
  using meta::Nullability;

  columnsMeta.reserve(5);

  const std::string sch("");
  const std::string tbl("");

  if (!connection.GetMetadataID()) {
    all_catalogs = catalog && *catalog == SQL_ALL_CATALOGS && schema
                   && schema->empty() && table.empty();

    all_schemas = schema && *schema == SQL_ALL_SCHEMAS && catalog
                  && catalog->empty() && table.empty();

    all_table_types = tableType && *tableType == SQL_ALL_TABLE_TYPES && catalog
                      && catalog->empty() && schema && schema->empty()
                      && table.empty();
  }
  int32_t odbcVer = connection.GetEnvODBCVer();

// driver needs to have have 2.0 column names for applications (e.g., Excel on macOS) 
// that expect ODBC driver ver 2.0. 
  std::string catalog_meta_name = "TABLE_CAT";
  std::string schema_meta_name = "TABLE_SCHEM";
  if (odbcVer == SQL_OV_ODBC2) {
    // For backwards compatibility with ODBC 2.0
    catalog_meta_name = "TABLE_QUALIFIER";
    schema_meta_name = "TABLE_OWNER";
  }

  if (all_catalogs) {
    /**
     * If CatalogName equals SQL_ALL_CATALOGS, and SchemaName and TableName are
     * empty strings, the result set should contain a list of valid catalogs for
     * the data source. (All columns except the TABLE_CAT column contain NULLs.)
     * If DATABASE_AS_SCHEMA is set to TRUE, an empty result set is returned 
     * since driver does not support catalogs, otherwise, a list of databases is 
     * returned.
     */
    LOG_INFO_MSG(
        "CatalogName equals SQL_ALL_CATALOGS, and SchemaName and TableName are "
        "empty strings."
        "All columns except the TABLE_CAT column contain NULLs.");
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, catalog_meta_name, ScalarType::VARCHAR,
        DATABASE_AS_SCHEMA ? Nullability::NULLABLE : Nullability::NO_NULL));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, schema_meta_name, ScalarType::VARCHAR, Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, "TABLE_NAME", ScalarType::VARCHAR, Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, "TABLE_TYPE", ScalarType::VARCHAR, Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(sch, tbl, "REMARKS", ScalarType::VARCHAR,
                                     Nullability::NULLABLE));
  } else if (all_schemas) {
    /**
     * If SchemaName equals SQL_ALL_SCHEMAS, and CatalogName and TableName are
     * empty strings, the result set should contain a list of valid schemas for
     * the data source. (All columns except the TABLE_SCHEM column contain
     * NULLs.)
     * If DATABASE_AS_SCHEMA is set to TRUE, a list of databases is returned, 
     * otherwise, an empty result set is returned since driver does not
     * support schemas.
     */
    LOG_INFO_MSG(
        "SchemaName equals SQL_ALL_SCHEMAS, and CatalogName and TableName are "
        "empty strings."
        "All columns except the TABLE_SCHEM column contain NULLs.");
    columnsMeta.push_back(ColumnMeta(sch, tbl, catalog_meta_name, ScalarType::VARCHAR,
                                     Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, schema_meta_name, ScalarType::VARCHAR,
        DATABASE_AS_SCHEMA ? Nullability::NO_NULL : Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, "TABLE_NAME", ScalarType::VARCHAR, Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, "TABLE_TYPE", ScalarType::VARCHAR, Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(sch, tbl, "REMARKS", ScalarType::VARCHAR,
                                     Nullability::NULLABLE));
  } else if (all_table_types) {
    /**
     * If TableType equals SQL_ALL_TABLE_TYPES and CatalogName, SchemaName, and
     * TableName are empty strings, the result set contains a list of valid
     * table types for the data source. (All columns except the TABLE_TYPE
     * column contain NULLs.) "TABLE_TYPE" is set to "TABLE" for Timestream.
     */
    LOG_INFO_MSG(
        "TableType equals SQL_ALL_TABLE_TYPES and CatalogName, SchemaName, and "
        "TableName are empty strings. "
        "All columns except the TABLE_TYPE column contain NULLs.");
    columnsMeta.push_back(ColumnMeta(sch, tbl, catalog_meta_name, ScalarType::VARCHAR,
                                     Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, schema_meta_name, ScalarType::VARCHAR, Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, "TABLE_NAME", ScalarType::VARCHAR, Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, "TABLE_TYPE", ScalarType::VARCHAR, Nullability::NO_NULL));
    columnsMeta.push_back(ColumnMeta(sch, tbl, "REMARKS", ScalarType::VARCHAR,
                                     Nullability::NULLABLE));
  } else {
    columnsMeta.push_back(ColumnMeta(sch, tbl, catalog_meta_name, ScalarType::VARCHAR,
                                     Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, schema_meta_name, ScalarType::VARCHAR, Nullability::NULLABLE));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, "TABLE_NAME", ScalarType::VARCHAR, Nullability::NO_NULL));
    columnsMeta.push_back(ColumnMeta(
        sch, tbl, "TABLE_TYPE", ScalarType::VARCHAR, Nullability::NO_NULL));
    columnsMeta.push_back(ColumnMeta(sch, tbl, "REMARKS", ScalarType::VARCHAR,
                                     Nullability::NULLABLE));
  }

  if (DATABASE_AS_SCHEMA) {
    if (!connection.GetMetadataID() && schema && !schema->empty()) {
      schema_regex = utility::ConvertPatternToRegex(schema.value());
    }
  } else {
    if (!connection.GetMetadataID() && catalog && !catalog->empty()) {
      catalog_regex = utility::ConvertPatternToRegex(catalog.value());
    }
  }
}

TableMetadataQuery::~TableMetadataQuery() {
  // No-op.
}

SqlResult::Type TableMetadataQuery::Execute() {
  if (executed)
    Close();

  SqlResult::Type result = MakeRequestGetTablesMeta();

  if (result == SqlResult::AI_SUCCESS
      || result == SqlResult::AI_SUCCESS_WITH_INFO) {
    executed = true;
    fetched = false;

    if (!meta.empty())
      cursor = meta.begin();
  }

  return result;
}

const meta::ColumnMetaVector* TableMetadataQuery::GetMeta() {
  return &columnsMeta;
}

SqlResult::Type TableMetadataQuery::FetchNextRow(
    app::ColumnBindingMap& columnBindings) {
  if (!executed) {
    diag.AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR,
                         "Query was not executed.");

    return SqlResult::AI_ERROR;
  } else if (meta.empty()) {
    LOG_DEBUG_MSG("meta object is empty. Returning AI_NO_DATA.");
    return SqlResult::AI_NO_DATA;
  }

  if (!fetched)
    fetched = true;
  else if (cursor != meta.end())
    ++cursor;
  if (cursor == meta.end())
    return SqlResult::AI_NO_DATA;

  app::ColumnBindingMap::iterator it;

  for (it = columnBindings.begin(); it != columnBindings.end(); ++it)
    GetColumn(it->first, it->second);

  return SqlResult::AI_SUCCESS;
}

SqlResult::Type TableMetadataQuery::GetColumn(
    uint16_t columnIdx, app::ApplicationDataBuffer& buffer) {
  if (!executed) {
    diag.AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR,
                         "Query was not executed.");

    return SqlResult::AI_ERROR;
  } else if (meta.empty()) {
    LOG_DEBUG_MSG("meta object is empty. Returning AI_NO_DATA.");
    return SqlResult::AI_NO_DATA;
  }

  if (cursor == meta.end()) {
    std::string errMsg = "Cursor has reached end of the result set.";
    diag.AddStatusRecord(SqlState::S24000_INVALID_CURSOR_STATE, errMsg);
    LOG_ERROR_MSG(errMsg);
    return SqlResult::AI_ERROR;
  }

  const meta::TableMeta& currentColumn = *cursor;

  LOG_DEBUG_MSG("columnIdx: " << columnIdx);

  switch (columnIdx) {
    case ResultColumn::TABLE_CAT: {
      buffer.PutString(currentColumn.GetCatalogName());
      break;
    }

    case ResultColumn::TABLE_SCHEM: {
      buffer.PutString(currentColumn.GetSchemaName());
      break;
    }

    case ResultColumn::TABLE_NAME: {
      buffer.PutString(currentColumn.GetTableName());
      break;
    }

    case ResultColumn::TABLE_TYPE: {
      buffer.PutString(currentColumn.GetTableType());
      break;
    }

    case ResultColumn::REMARKS: {
      buffer.PutString(currentColumn.GetRemarks());
      break;
    }

    default:
      break;
  }

  LOG_INFO_MSG("buffer: " << buffer.GetString(SQL_NTS));

  return SqlResult::AI_SUCCESS;
}

SqlResult::Type TableMetadataQuery::Close() {
  meta.clear();

  executed = false;

  return SqlResult::AI_SUCCESS;
}

bool TableMetadataQuery::DataAvailable() const {
  return cursor != meta.end();
}

int64_t TableMetadataQuery::AffectedRows() const {
  return 0;
}

SqlResult::Type TableMetadataQuery::NextResultSet() {
  return SqlResult::AI_NO_DATA;
}

// TODO [AT-1154] Send Query to server from driver internally to filter tables
// https://bitquill.atlassian.net/browse/AT-1154
SqlResult::Type TableMetadataQuery::MakeRequestGetTablesMeta() {
  // clear meta object at beginning of function
  meta.clear();

  if (all_table_types) {
    // case for SQL_ALL_TABLE_TYPES is THE same whether databases are reported as schemas or catalogs
    using meta::TableMeta;
    std::string tableType = "TABLE";
    meta.emplace_back(TableMeta());
    meta.back().Read(tableType);

    return SqlResult::AI_SUCCESS;
  }

  // Check for corner cases in tables meta retrieval 
  const boost::optional< SqlResult::Type > sqlResult =
      getTablesMetaCornerCase();
  if (sqlResult)
    return *sqlResult;

  if (tableType) {
    // Parse provided table types
    bool validTableType = false;
    if (tableType->empty() || *tableType == SQL_ALL_TABLE_TYPES) {
      // Table type not specified. "TABLE" table type is accepted
      validTableType = true;
    } else {
      std::stringstream ss(*tableType);
      std::string singleTableType;
      while (std::getline(ss, singleTableType, ',')) {
        if (dequote(utility::Trim(singleTableType)) == "TABLE") {
          validTableType = true;
          break;
        }
      }
    }

    if (!validTableType) {
      // table type(s) provided is not valid for Timestream.
      std::string warnMsg =
          "Empty result set is returned as tableType is set to \"" + *tableType
          + "\" and Timestream only supports \"TABLE\" table type";
      LOG_WARNING_MSG(warnMsg);
      diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);

      return SqlResult::AI_SUCCESS_WITH_INFO;
    }
  }

  // The default case: Get tables and update meta object
  return getTables();
}

 boost::optional< SqlResult::Type > TableMetadataQuery::getTablesMetaCornerCase() {
  if (DATABASE_AS_SCHEMA) {
     // Databases are reported as schemas
     if (catalog && !catalog->empty() && *catalog != SQL_ALL_CATALOGS) {
       // check if catalog is set and does not equal to SQL_ALL_CATALOGS
       std::string warnMsg;
       if (all_catalogs) {
         warnMsg =
             "Empty result set is returned as catalog is set to "
             "SQL_ALL_CATALOGS "
             "and Timestream does not have catalogs";
       } else {
         // catalog has been provided with a non-empty value that isn't
         // SQL_ALL_CATALOGS. Return empty result set by default since
         // Timestream does not have catalogs.
         warnMsg = "Empty result set is returned as catalog is set to \""
                   + *catalog + "\" and Timestream does not have catalogs";
       }
       LOG_WARNING_MSG(warnMsg);
       diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);

       return SqlResult::AI_SUCCESS_WITH_INFO;
     } else if (all_schemas) {
       LOG_DEBUG_MSG("Attempting to retrieve list of all schemas (databases)");
       return getDatabases();
     } else if ((schema && schema->empty()) || table.empty()) {
       // empty schema or empty table should match nothing
       std::string warnMsg = "Schema and table name should not be empty";
       diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
       return SqlResult::AI_SUCCESS_WITH_INFO;
     }
   } else {
     // Databases are reported as catalogs
     if (schema && !schema->empty() && *schema != SQL_ALL_SCHEMAS) {
       // check if schema is set and does not equal to SQL_ALL_SCHEMAS
       std::string warnMsg;
       if (all_schemas) {
         warnMsg =
             "Empty result set is returned as schemas is set to "
             "SQL_ALL_SCHEMAS "
             "and Timestream does not have schemas";
       } else {
         // catalog has been provided with a non-empty value that isn't
         // SQL_ALL_SCHEMAS. Return empty result set by default since Timestream
         // does not have schemas.
         warnMsg = "Empty result set is returned as schema is set to \""
                   + *schema + "\" and Timestream does not have schemas";
       }
       LOG_WARNING_MSG(warnMsg);
       diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);

       return SqlResult::AI_SUCCESS_WITH_INFO;
     } else if (all_catalogs) {
       LOG_DEBUG_MSG("Attempting to retrieve list of all catalogs (databases)");
       return getDatabases();
     } else if ((catalog && catalog->empty()) || table.empty()) {
       // empty catalog or empty table should match nothing
       std::string warnMsg = "Catalog and table name should not be empty";
       diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
       return SqlResult::AI_SUCCESS_WITH_INFO;
     }
   }

  return boost::none;
 }

SqlResult::Type TableMetadataQuery::getDatabases() {
  // Use TimestreamWriteClient API to retrieve databases and tables. 
  ListDatabasesRequest dbRequest;

  std::string nextDatabaseToken("");
  
  LOG_DEBUG_MSG("Entering while loop for using nextDatabaseToken");
  do {
    ListDatabasesOutcome dbOutcome = connection.GetWriteClient()->ListDatabases(dbRequest);

    if (!dbOutcome.IsSuccess()) {
      return checkOutcomeError(dbOutcome);
    }

    ListDatabasesResult dbResult = dbOutcome.GetResult();
    Aws::Vector< Database > dbVector = dbResult.GetDatabases();
  
    meta::ReadDatabaseMetaVector(dbVector, meta);

    nextDatabaseToken = dbResult.GetNextToken();

    dbRequest.SetNextToken(nextDatabaseToken);
  } while (!nextDatabaseToken.empty());

  LOG_DEBUG_MSG(
      "The while loop for using nextDatabaseToken has finished running. "
      "Database retrieval is finished.");

  if (meta.empty()) {
    std::string warnMsg =
        "Empty result set is returned as no databases could be "
        "found.";
    LOG_WARNING_MSG(warnMsg);
    diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
    return SqlResult::AI_SUCCESS_WITH_INFO;
  }

  // Log the results
  if (DATABASE_AS_SCHEMA) {
    for (size_t i = 0; i < meta.size(); ++i) {
      LOG_INFO_MSG("\n[" << i << "] SchemaName:  " << meta[i].GetSchemaName());
    }
  } else {
    for (size_t i = 0; i < meta.size(); ++i) {
      LOG_INFO_MSG("\n[" << i
                         << "] CatalogName:  " << meta[i].GetCatalogName());
    }    
  }

  return SqlResult::AI_SUCCESS;
}

bool TableMetadataQuery::checkIfDatabaseMatch(
    const std::string& databaseName,
    const boost::optional< std::string >& databasePattern,
    const std::string& allDatabasePattern,
    const std::regex db_pattern_regex) {
  if (!databasePattern || *databasePattern == allDatabasePattern) {
    return true;
  }

  bool match;
  if (connection.GetMetadataID()) {
    Aws::String schemaUpper =
        Aws::Utils::StringUtils::ToUpper(databasePattern.value().data());
    Aws::String dbNameUpper =
        Aws::Utils::StringUtils::ToUpper(databaseName.data());
    match = (schemaUpper == dbNameUpper);
  } else {
    match = std::regex_match(databaseName, db_pattern_regex);
  }

  return match;
}

SqlResult::Type TableMetadataQuery::getTables() {
  // Use TimestreamWriteClient API to retrieve databases and tables. 
  ListDatabasesRequest dbRequest;

  std::string nextDatabaseToken("");
  
  LOG_DEBUG_MSG("Entering while loop for using nextDatabaseToken");
  do {
    // retrieve next database result set before proceeding.
    ListDatabasesOutcome dbOutcome = connection.GetWriteClient()->ListDatabases(dbRequest);
      if (!dbOutcome.IsSuccess()) {
        return checkOutcomeError(dbOutcome);
      }

    ListDatabasesResult dbResult = dbOutcome.GetResult();
    Aws::Vector< Database > dbVector = dbResult.GetDatabases();

    for (Database db : dbVector) {
      std::string databaseName = db.GetDatabaseName();

      bool match = true;

      if (DATABASE_AS_SCHEMA) {
        match = checkIfDatabaseMatch(databaseName, schema, SQL_ALL_SCHEMAS,
                                     schema_regex);
      } else {
        match = checkIfDatabaseMatch(databaseName, catalog, SQL_ALL_CATALOGS,
                                     catalog_regex);
      }

      if (!match) {
        // If database name does not equal database search pattern, then do not include the
        // database in the result set
        continue;
      }

      // retrieve tables using database name
      SqlResult::Type res = getTablesWithDBName(databaseName);
      if (res != SqlResult::AI_SUCCESS)
        return res;
    }

    nextDatabaseToken = dbResult.GetNextToken();

    dbRequest.SetNextToken(nextDatabaseToken);
  } while (!nextDatabaseToken.empty());
  LOG_DEBUG_MSG(
      "The while loop for using nextDatabaseToken has finished running. "
      "Database retrieval is finished.");
  
  // Log contents of meta object
  return checkMeta();
}

SqlResult::Type TableMetadataQuery::getTablesWithDBName(std::string& databaseName) {
  using Aws::TimestreamWrite::Model::ListTablesRequest;
  using Aws::TimestreamWrite::Model::ListTablesResult;
  using Aws::TimestreamWrite::Model::Table;

  // List tables
  ListTablesRequest tbRequest;
  tbRequest.SetDatabaseName(databaseName);

  std::string nextTableToken("");

  LOG_DEBUG_MSG("Entering while loop for using nextTableToken. Database name: "
                << databaseName);
  do {
    ListTablesOutcome tbOutcome = connection.GetWriteClient()->ListTables(tbRequest);

    if (!tbOutcome.IsSuccess()) {
      return checkOutcomeError(tbOutcome);
    }

    ListTablesResult tbResult = tbOutcome.GetResult();
    Aws::Vector< Table > tbVector = tbResult.GetTables();

    meta::ReadTableMetaVector(table, connection.GetMetadataID(), tbVector, meta);

    nextTableToken = tbResult.GetNextToken();

    tbRequest.SetNextToken(nextTableToken);
  } while(!nextTableToken.empty());

  LOG_DEBUG_MSG(
      "The while loop for using nextTableToken has finished running. "
      "Database name: "
      << databaseName);

  return SqlResult::AI_SUCCESS;
}

SqlResult::Type TableMetadataQuery::checkOutcomeError(ListDatabasesOutcome const& dbOutcome) {
  auto& error = dbOutcome.GetError();
  LOG_ERROR_MSG("ERROR from ListDatabasesOutcome: "
                << error.GetExceptionName() << ": " << error.GetMessage());

  diag.AddStatusRecord(
      SqlState::SHY000_GENERAL_ERROR,
      "AWS API ERROR: " + error.GetExceptionName() + ": " + error.GetMessage());

  return SqlResult::AI_ERROR;
}

SqlResult::Type TableMetadataQuery::checkOutcomeError(
    ListTablesOutcome const& tbOutcome) {
  auto& error = tbOutcome.GetError();
  LOG_ERROR_MSG("ERROR from ListTablesOutcome: " << error.GetExceptionName()
                                                 << ": " << error.GetMessage());

  diag.AddStatusRecord(
      SqlState::SHY000_GENERAL_ERROR,
      "AWS API ERROR: " + error.GetExceptionName() + ": " + error.GetMessage());

  return SqlResult::AI_ERROR;
}

SqlResult::Type TableMetadataQuery::checkMeta() {
  // execute this function after meta object population is finished.
  if (meta.empty()) {
    std::string warnMsg =
        "Empty result set is returned as we could not find tables with the "
        "table search pattern \""
        + table + "\"";
    if (DATABASE_AS_SCHEMA) {
      if (schema) {
        warnMsg = warnMsg + " and from schema with search pattern \"" + *schema
                  + "\"";
      }
    } else {
      if (catalog) {
        warnMsg = warnMsg + " and from catalog with search pattern \""
                  + *catalog + "\"";
      }
    }
    LOG_WARNING_MSG(warnMsg);
    diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
    return SqlResult::AI_SUCCESS_WITH_INFO;
  }

  for (size_t i = 0; i < meta.size(); ++i) {
    LOG_INFO_MSG("\n[" << i << "] CatalogName: " << meta[i].GetCatalogName()
                       << "\n[" << i << "] SchemaName:  "
                       << meta[i].GetSchemaName() << "\n[" << i
                       << "] TableName:   " << meta[i].GetTableName() << "\n["
                       << i << "] TableType:   " << meta[i].GetTableType());
  }

  return SqlResult::AI_SUCCESS;
}

std::string TableMetadataQuery::dequote(const std::string& s) {
  if (s.size() >= 2
      && ((s.front() == '\'' && s.back() == '\'')
          || (s.front() == '"' && s.back() == '"'))) {
    return s.substr(1, s.size() - 2);
  }
  return s;
}
}  // namespace query
}  // namespace odbc
}  // namespace ignite
