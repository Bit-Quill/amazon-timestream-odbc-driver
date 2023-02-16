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

#include <aws/timestream-query/model/ScalarType.h>

#include <vector>

#include "ignite/odbc/connection.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/type_traits.h"

using Aws::TimestreamQuery::Model::ScalarType;
using ignite::odbc::common::concurrent::SharedPointer;

namespace ignite {
namespace odbc {
namespace query {
using ignite::odbc::type_traits::OdbcNativeType;

TableMetadataQuery::TableMetadataQuery(
    diagnostic::DiagnosableAdapter& diag, Connection& connection,
    const boost::optional< std::string >& catalog,
    const boost::optional< std::string >& schema,
    const boost::optional< std::string >& table,
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
  using meta::ColumnMeta;
  using meta::Nullability;

  columnsMeta.reserve(5);

  const std::string sch("");
  const std::string tbl("");

  if (!connection.GetMetadataID()) {
    all_catalogs = catalog && *catalog == SQL_ALL_CATALOGS && schema
                   && schema->empty() && table && table->empty();

    all_schemas = schema && *schema == SQL_ALL_SCHEMAS && catalog
                  && catalog->empty() && table && table->empty();
  }

  // TableType is a value list argument, regardless of the setting of SQL_ATTR_METADATA_ID.
  all_table_types = tableType && *tableType == SQL_ALL_TABLE_TYPES && catalog
                    && catalog->empty() && schema && schema->empty() && table
                    && table->empty();

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

SqlResult::Type TableMetadataQuery::MakeRequestGetTablesMeta() {
  // clear meta object at beginning of function
  meta.clear();

  if (all_table_types) {
    // case for SQL_ALL_TABLE_TYPES is the same whether databases are reported as schemas or catalogs
    using meta::TableMeta;
    std::string tableType = "TABLE";
    meta.emplace_back(TableMeta());
    meta.back().Read(tableType);

    return SqlResult::AI_SUCCESS;
  }

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

  // Check for corner cases and handle database search patterns / identifiers in tables meta retrieval
  return getTables();
}

SqlResult::Type TableMetadataQuery::getTables() {
  if (DATABASE_AS_SCHEMA) {
    // Databases are reported as schemas
    if (connection.GetMetadataID()) {
      // Parameters are case-insensitive identifiers
      if (!schema || !table) {
        // catalogs not supported, only check if schema/table names are
        // nullptrs
        diag.AddStatusRecord(SqlState::SHY009_INVALID_USE_OF_NULL_POINTER,
                             "The SQL_ATTR_METADATA_ID statement attribute "
                             "is set to SQL_TRUE, "
                             "and SchemaName or "
                             "the TableName argument was a null pointer.");
        return SqlResult::AI_ERROR;
      }
      return getTablesWithIdentifier(*schema);
    } else {
      // Parameters are case-sensitive search patterns
      if (all_schemas) {
        LOG_DEBUG_MSG("Attempting to retrieve list of all schemas (databases)");
        return getAllDatabases();
      } else if (catalog && !catalog->empty() && *catalog != SQL_ALL_CATALOGS) {
        // catalog has been provided with a non-empty value that isn't
        // SQL_ALL_CATALOGS. Return empty result set by default since
        // Timestream does not have catalogs.
        std::string warnMsg =
            "Empty result set is returned as catalog is set to \"" + *catalog
            + "\" and Timestream does not have catalogs";

        LOG_WARNING_MSG(warnMsg);
        diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);

        return SqlResult::AI_SUCCESS_WITH_INFO;
      } else if (all_catalogs) {
        std::string warnMsg =
            "Empty result set is returned for a list of catalogs "
            "because Timestream does not have catalogs";
        LOG_WARNING_MSG(warnMsg);
        diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);

        return SqlResult::AI_SUCCESS_WITH_INFO;
      } else if ((schema && schema->empty()) || (table && table->empty())) {
        // empty schema or empty table should match nothing
        std::string warnMsg = "Schema and table name should not be empty";
        diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
        return SqlResult::AI_SUCCESS_WITH_INFO;
      }
      // Timestream does not support catalogs, so catalog name field would be
      // empty string. If catalog variable is "%" (SQL_ALL_CATALOGS), it is
      // ignored for two reasons:
      // 1. The percent sign (%) search pattern represents any sequence of
      // **zero** or more characters and it would match empty string catalog
      // names.
      // 2. Sometimes BI tools can pass SQLTables(catalogname - %, schemaname -
      // %, tablename - %,...) to get all tables, and we want to support this
      // case.
      return getTablesWithSearchPattern(schema);
    }
  } else {
    // Databases are reported as catalogs
    if (connection.GetMetadataID()) {
      // Parameters are case-insensitive identifiers
      if (!catalog || !table) {
        // schemas not supported, only check if catalog/table names are
        // nullptrs
        diag.AddStatusRecord(SqlState::SHY009_INVALID_USE_OF_NULL_POINTER,
                             "The SQL_ATTR_METADATA_ID statement attribute "
                             "is set to SQL_TRUE, "
                             "and CatalogName or "
                             "the TableName argument was a null pointer.");
        return SqlResult::AI_ERROR;
      }
      return getTablesWithIdentifier(*catalog);
    } else {
      // Parameters are case-sensitive search patterns
      if (all_catalogs) {
        LOG_DEBUG_MSG(
            "Attempting to retrieve list of all catalogs (databases)");
        return getAllDatabases();
      } else if (schema && !schema->empty() && *schema != SQL_ALL_SCHEMAS) {
        // catalog has been provided with a non-empty value that isn't
        // SQL_ALL_SCHEMAS. Return empty result set by default since
        // Timestream does not have schemas.
        std::string warnMsg =
            "Empty result set is returned as schema is set to \"" + *schema
            + "\" and Timestream does not have schemas";

        LOG_WARNING_MSG(warnMsg);
        diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);

        return SqlResult::AI_SUCCESS_WITH_INFO;
      } else if (all_schemas) {
        std::string warnMsg =
            "Empty result set is returned for a list of schemas "
            "because Timestream does not have schemas";
        LOG_WARNING_MSG(warnMsg);
        diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);

        return SqlResult::AI_SUCCESS_WITH_INFO;
      } else if ((catalog && catalog->empty()) || (table && table->empty())) {
        // empty catalog or empty table should match nothing
        std::string warnMsg = "Catalog and table name should not be empty";
        diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
        return SqlResult::AI_SUCCESS_WITH_INFO;
      }
      // Timestream does not support schemas, so schema name field would be
      // empty string. If schema variable is "%" (SQL_ALL_SCHEMAS), it is
      // ignored for two reasons:
      // 1. The percent sign (%) search pattern represents any sequence of
      // **zero** or more characters and it would match empty string schema
      // names.
      // 2. Sometimes BI tools can pass SQLTables(catalogname - %, schemaname -
      // %, tablename - %,...) to get all tables, and we want to support this
      // case.
      return getTablesWithSearchPattern(catalog);
    }
  }
}

SqlResult::Type TableMetadataQuery::getMatchedDatabases(
     const std::string& databasePattern, std::vector< std::string >& databaseNames) {
   std::string sql = "SHOW DATABASES LIKE \'" + databasePattern + "\'";

   app::ParameterSet params;
   int32_t timeout = 60;

   dataQuery_ =
       std::make_shared< DataQuery >(diag, connection, sql, params, timeout);
   SqlResult::Type result = dataQuery_->Execute();

   if (result == SqlResult::AI_NO_DATA) {
     std::string warnMsg =
         "No database is found with pattern \'" + databasePattern + "\'";
     LOG_WARNING_MSG(warnMsg);
     diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
     return SqlResult::AI_SUCCESS_WITH_INFO;
   } else if (result != SqlResult::AI_SUCCESS) {
     LOG_ERROR_MSG("Failed to execute sql:" << sql);
     return result;
   }

   app::ColumnBindingMap columnBindings;
   SqlLen buflen = STRING_BUFFER_SIZE;
   // According to Timestream, table name could only contain
   // letters, digits, dashes, periods or underscores. It could
   // not be a unicode string.
   char databaseName[STRING_BUFFER_SIZE]{};
   ApplicationDataBuffer buf(OdbcNativeType::Type::AI_CHAR, &databaseName,
                              buflen, nullptr);
   columnBindings[1] = buf;

   while (dataQuery_->FetchNextRow(columnBindings) == SqlResult::AI_SUCCESS) {
     databaseNames.emplace_back(std::string(databaseName));
   }

   return SqlResult::AI_SUCCESS;
}

SqlResult::Type TableMetadataQuery::getMatchedTables(
    const std::string& databaseName, const std::string& tablePattern,
    std::vector< std::string >& tableNames) {
  std::string sql = "SHOW TABLES FROM \"" + databaseName + "\" LIKE \'" + tablePattern + "\'";

  app::ParameterSet params;
  int32_t timeout = 60;

  dataQuery_ =
      std::make_shared< DataQuery >(diag, connection, sql, params, timeout);
  SqlResult::Type result = dataQuery_->Execute();

  if (result == SqlResult::AI_NO_DATA) {
    std::string warnMsg = "No table is found with pattern \'" + tablePattern
                          + "\' from database (" + databaseName + ")";
    LOG_WARNING_MSG(warnMsg);
    diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
    return SqlResult::AI_SUCCESS_WITH_INFO;
    // DataQuery::Execute() does not return SUCCESS_WITH_INFO
  } else if (result != SqlResult::AI_SUCCESS) {
    LOG_ERROR_MSG("Failed to execute sql:" << sql);
    return result;
  }

  app::ColumnBindingMap columnBindings;
  SqlLen buflen = STRING_BUFFER_SIZE;
  // According to Timestream, table name could only contain
  // letters, digits, dashes, periods or underscores. It could
  // not be a unicode string.
  char tableName[STRING_BUFFER_SIZE]{};
  ApplicationDataBuffer buf(OdbcNativeType::Type::AI_CHAR, &tableName, buflen,
                             nullptr);
  columnBindings[1] = buf;

  while (dataQuery_->FetchNextRow(columnBindings) == SqlResult::AI_SUCCESS) {
    tableNames.emplace_back(std::string(tableName));
  }

  return SqlResult::AI_SUCCESS;
}

SqlResult::Type TableMetadataQuery::getAllDatabases() {
  LOG_DEBUG_MSG("entering getAllDatabases");

  std::vector< std::string > databaseNames;
  SqlResult::Type result = getMatchedDatabases("%", databaseNames);

  if (result != SqlResult::AI_SUCCESS) {
    LOG_DEBUG_MSG("getAllDatabases early exisitng with result: " << result);
    return result;
  }

  int numDatabases = databaseNames.size();

  for (int i = 0; i < numDatabases; i++) {
    using meta::TableMeta;

    std::string databaseName = databaseNames.at(i);
    if (DATABASE_AS_SCHEMA) {
      meta.emplace_back(TableMeta(std::string(""), databaseName,
                                  std::string(""), std::string("TABLE")));
    } else {
      meta.emplace_back(TableMeta(databaseName, std::string(""),
                                  std::string(""), std::string("TABLE")));
    }
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

  LOG_DEBUG_MSG("exiting getAllDatabases");
  return SqlResult::AI_SUCCESS;
}

SqlResult::Type TableMetadataQuery::getTablesWithIdentifier(
   const std::string& databaseIdentifier) {
  LOG_DEBUG_MSG("Entering getTablesWithIdentifier");

  std::vector< std::string > databaseNames;
  SqlResult::Type result = getMatchedDatabases("%", databaseNames);

  if (result != SqlResult::AI_SUCCESS) {
    LOG_DEBUG_MSG("getTablesWithIdentifier early exisitng with result: " << result);
    return result;
  }

  // get all database names, then do filtering based on database name identifier
  int numDatabases = databaseNames.size();
  for (int i = 0; i < numDatabases; i++) {
    std::string databaseName = databaseNames.at(i);

    bool match = false;
    Aws::String databaseUpper =
        Aws::Utils::StringUtils::ToUpper(databaseIdentifier.data());
    Aws::String dbNameUpper =
        Aws::Utils::StringUtils::ToUpper(databaseName.data());
    match = (databaseUpper == dbNameUpper);

    if (!match) {
      // If database name does not equal database case-insensitive identifier, then do not
      // include the database in the result set
      continue;
    } else {
      LOG_DEBUG_MSG(
          "database ("
          << databaseName << ") has exact match with provided database case-insensitive identifier ("
          << databaseIdentifier
          << "), so the database will be included in the result set.");
    }


    // retrieve tables using database name
    std::vector< std::string > tableNames;
    SqlResult::Type res =
        getMatchedTables(databaseName, "%", tableNames);

    if (res != SqlResult::AI_SUCCESS
        && res != SqlResult::AI_SUCCESS_WITH_INFO) {
      LOG_DEBUG_MSG(
          "getTablesWithIdentifier early exisitng with result: " << res);
      return res;
    }

    // get all database names, then do filtering based on database name
    // identifier
    int numTables = tableNames.size();
    for (int j = 0; j < numTables; j++) {
      using meta::TableMeta;

      // Check exact match for table name case-insensitive identifier
      std::string foundTableName = tableNames.at(j);
      Aws::String tableUpper =
          Aws::Utils::StringUtils::ToUpper(table.get().data());
      Aws::String tbNameUpper =
          Aws::Utils::StringUtils::ToUpper(foundTableName.data());
      match = (tableUpper == tbNameUpper);

      if (match) {
        if (DATABASE_AS_SCHEMA) {
          meta.emplace_back(TableMeta(std::string(""), databaseName,
                                      std::string(foundTableName),
                                      std::string("TABLE")));
        } else {
          meta.emplace_back(TableMeta(databaseName, std::string(""),
                                      std::string(foundTableName),
                                      std::string("TABLE")));
        }
        LOG_DEBUG_MSG(
            "current table name ("
                      << foundTableName
                      << ") matches the provided table name identifier ("
                      << *table
                      << "), so it will be included in the resultset.");
      }
    }
  }

  LOG_DEBUG_MSG("Exiting getTablesWithIdentifier");
  // Log contents of meta object
  return checkMeta();
}

SqlResult::Type TableMetadataQuery::getTablesWithSearchPattern(const boost::optional < std::string > &databasePattern) {
  std::vector< std::string > databaseNames;
  SqlResult::Type result =
      getMatchedDatabases(databasePattern.get_value_or("%"), databaseNames);

  if (result != SqlResult::AI_SUCCESS) {
    LOG_DEBUG_MSG("getAllDatabases early exisitng with result: " << result);
    return result;
  }

  int numDatabases = databaseNames.size();

  for (int i = 0; i < numDatabases; i++) {
    std::string databaseName = databaseNames.at(i);
   
    // retrieve tables using database name
    std::vector< std::string > tableNames;
    SqlResult::Type res =
        getMatchedTables(databaseName, table.get_value_or("%"), tableNames);

    if (res != SqlResult::AI_SUCCESS && res != SqlResult::AI_SUCCESS_WITH_INFO)
      return res;

    int numTables = tableNames.size();
    for (int j = 0; j < numTables; j++) {
      using meta::TableMeta;

      std::string foundTableName = tableNames.at(j);
      if (DATABASE_AS_SCHEMA) {
        meta.emplace_back(TableMeta(std::string(""), databaseName,
                                    std::string(foundTableName),
                                    std::string("TABLE")));
      } else {
        meta.emplace_back(TableMeta(databaseName, std::string(""),
                                    std::string(foundTableName),
                                    std::string("TABLE")));
      }
    }
  }

  // Log contents of meta object
  return checkMeta();
}

SqlResult::Type TableMetadataQuery::checkMeta() {
  // execute this function after meta object population is finished.
  if (meta.empty()) {
    std::string warnMsg =
        "Empty result set is returned as we could not find tables with the "
        "tableName \""
        + table.get_value_or("(tableName was nullptr)") + "\"";
    if (DATABASE_AS_SCHEMA) {
      warnMsg = warnMsg + " and from schema with schemaName \""
                + schema.get_value_or("(schemaName was nullptr)") + "\"";

    } else {
      warnMsg = warnMsg + " and from catalog with catalogName \""
                + catalog.get_value_or("(catalogName was nullptr)") + "\"";
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
