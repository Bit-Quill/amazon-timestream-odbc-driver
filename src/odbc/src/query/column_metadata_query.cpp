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

#include "ignite/odbc/query/column_metadata_query.h"

#include <vector>

#include "ignite/odbc/common/concurrent.h"
#include "ignite/odbc/connection.h"
#include "ignite/odbc/ignite_error.h"
#include "ignite/odbc/impl/binary/binary_common.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/message.h"
#include "ignite/odbc/odbc_error.h"
#include "ignite/odbc/type_traits.h"

using ignite::odbc::IgniteError;
using ignite::odbc::common::concurrent::SharedPointer;

namespace {
struct ResultColumn {
  enum Type {
    /** Catalog name. NULL if not applicable to the data source. */
    TABLE_CAT = 1,

    /** Schema name. NULL if not applicable to the data source. */
    TABLE_SCHEM,

    /** Table name. */
    TABLE_NAME,

    /** Column name. */
    COLUMN_NAME,

    /** SQL data type. */
    DATA_TYPE,

    /** Data source-dependent data type name. */
    TYPE_NAME,

    /** Column size. */
    COLUMN_SIZE,

    /** The length in bytes of data transferred on fetch. */
    BUFFER_LENGTH,

    /** The total number of significant digits to the right of the decimal
       point. */
    DECIMAL_DIGITS,

    /** Precision. */
    NUM_PREC_RADIX,

    /** Nullability of the data in column (int). */
    NULLABLE,

    /** A description of the column. */
    REMARKS,

    /** Default value for the column. May be null. */
    COLUMN_DEF,

    /** SQL data type. */
    SQL_DATA_TYPE,

    /** Subtype code for datetime and interval data types. */
    SQL_DATETIME_SUB,

    /** Maximum length in bytes of a character or binary data type column.
       NULL for other data types. */
    CHAR_OCTET_LENGTH,

    /** Index of column in table (starting at 1) */
    ORDINAL_POSITION,

    /** Nullability of data in column (String). */
    IS_NULLABLE
  };
};
}  // namespace
namespace ignite {
namespace odbc {
namespace query {
ColumnMetadataQuery::ColumnMetadataQuery(
    diagnostic::DiagnosableAdapter& diag, Connection& connection,
    const boost::optional< std::string >& catalog,
    const boost::optional< std::string >& schema, const std::string& table,
    const std::string& column)
    : Query(diag, QueryType::COLUMN_METADATA),
      connection(connection),
      catalog(catalog),
      schema(schema),
      table(table),
      column(column),
      executed(false),
      fetched(false),
      meta(),
      columnsMeta() {
  using namespace ignite::odbc::impl::binary;
  using namespace ignite::odbc::type_traits;

  using meta::ColumnMeta;
  using meta::Nullability;

  columnsMeta.reserve(18);

  const std::string sch("");
  const std::string tbl("");

  columnsMeta.push_back(ColumnMeta(sch, tbl, "TABLE_CAT", ScalarType::VARCHAR,
                                   Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "TABLE_SCHEM", ScalarType::VARCHAR,
                                   Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "TABLE_NAME", ScalarType::VARCHAR,
                                   Nullability::NO_NULL));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "COLUMN_NAME", ScalarType::VARCHAR,
                                   Nullability::NO_NULL));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "DATA_TYPE", ScalarType::INTEGER,
                                   Nullability::NO_NULL));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "TYPE_NAME", ScalarType::VARCHAR,
                                   Nullability::NO_NULL));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "COLUMN_SIZE", ScalarType::INTEGER,
                                   Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "BUFFER_LENGTH",
                                   ScalarType::INTEGER, Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "DECIMAL_DIGITS",
                                   ScalarType::INTEGER, Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "NUM_PREC_RADIX",
                                   ScalarType::INTEGER, Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "NULLABLE", ScalarType::INTEGER,
                                   Nullability::NO_NULL));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "REMARKS", ScalarType::VARCHAR,
                                   Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "COLUMN_DEF", ScalarType::VARCHAR,
                                   Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "SQL_DATA_TYPE",
                                   ScalarType::INTEGER, Nullability::NO_NULL));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "SQL_DATETIME_SUB",
                                   ScalarType::INTEGER, Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "CHAR_OCTET_LENGTH",
                                   ScalarType::INTEGER, Nullability::NULLABLE));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "ORDINAL_POSITION",
                                   ScalarType::INTEGER, Nullability::NO_NULL));
  columnsMeta.push_back(ColumnMeta(sch, tbl, "IS_NULLABLE", ScalarType::VARCHAR,
                                   Nullability::NULLABLE));
  tableMetadataQuery_ = std::make_shared< TableMetadataQuery >(
      diag, connection, catalog, schema, table, boost::none);
}

ColumnMetadataQuery::~ColumnMetadataQuery() {
  // No-op.
}

SqlResult::Type ColumnMetadataQuery::Execute() {
  if (executed)
    Close();
  
  // empty schema or table should match nothing
  if ((schema && schema->empty()) || table.empty()) {
    std::string warnMsg = "Schema and table name should not be empty.";
    diag.AddStatusRecord(SqlState::S01000_GENERAL_WARNING, warnMsg);
    LOG_WARNING_MSG(warnMsg);

    return SqlResult::AI_SUCCESS_WITH_INFO;  
  }

  SqlResult::Type result = MakeRequestGetColumnsMeta();

  if (result == SqlResult::AI_SUCCESS) {
    executed = true;
    fetched = false;

    cursor = meta.begin();
  }

  return result;
}

const meta::ColumnMetaVector* ColumnMetadataQuery::GetMeta() {
  return &columnsMeta;
}

SqlResult::Type ColumnMetadataQuery::FetchNextRow(
    app::ColumnBindingMap& columnBindings) {
  if (!executed) {
    diag.AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR,
                         "Query was not executed.");

    return SqlResult::AI_ERROR;
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

SqlResult::Type ColumnMetadataQuery::GetColumn(
    uint16_t columnIdx, app::ApplicationDataBuffer& buffer) {
  if (!executed) {
    diag.AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR,
                         "Query was not executed.");

    return SqlResult::AI_ERROR;
  }

  if (cursor == meta.end()) {
    std::string errMsg = "Cursor has reached end of the result set.";
    diag.AddStatusRecord(SqlState::S24000_INVALID_CURSOR_STATE, errMsg);
    LOG_ERROR_MSG(errMsg);
    return SqlResult::AI_ERROR;
  }

  const meta::ColumnMeta& currentColumn = *cursor;
  boost::optional< int16_t > columnType = currentColumn.GetDataType();

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

    case ResultColumn::COLUMN_NAME: {
      buffer.PutString(currentColumn.GetColumnName());
      break;
    }

    case ResultColumn::DATA_TYPE: {
      buffer.PutInt16(type_traits::BinaryToSqlType(columnType));
      break;
    }

    case ResultColumn::TYPE_NAME: {
      buffer.PutString(type_traits::BinaryTypeToSqlTypeName(columnType));
      break;
    }

    case ResultColumn::COLUMN_SIZE: {
      buffer.PutInt32(type_traits::BinaryTypeColumnSize(columnType));
      break;
    }

    case ResultColumn::BUFFER_LENGTH: {
      buffer.PutInt32(type_traits::BinaryTypeTransferLength(columnType));
      break;
    }

    case ResultColumn::DECIMAL_DIGITS: {
      // todo implement the function for getting the decimal digits:
      // https://bitquill.atlassian.net/browse/AD-615
      boost::optional< int16_t > decDigits =
          type_traits::BinaryTypeDecimalDigits(columnType);
      if (!decDigits || *decDigits < 0)
        buffer.PutNull();
      else
        buffer.PutInt16(*decDigits);
      break;
    }

    case ResultColumn::NUM_PREC_RADIX: {
      boost::optional< int32_t > numPrecRadix =
          type_traits::BinaryTypeNumPrecRadix(columnType);
      if (!numPrecRadix || numPrecRadix < 0)
        buffer.PutNull();
      else
        buffer.PutInt16(static_cast< int16_t >(*numPrecRadix));
      break;
    }

    case ResultColumn::NULLABLE: {
      buffer.PutInt32(currentColumn.GetNullability());
      break;
    }

    case ResultColumn::REMARKS: {
      buffer.PutString(currentColumn.GetRemarks());
      break;
    }

    case ResultColumn::COLUMN_DEF: {
      buffer.PutString(currentColumn.GetColumnDef());
      break;
    }

    case ResultColumn::SQL_DATA_TYPE: {
      buffer.PutInt16(type_traits::BinaryToSqlType(columnType));
      break;
    }

    case ResultColumn::SQL_DATETIME_SUB: {
      buffer.PutNull();
      // todo implement the function for getting the datetime sub code:
      // https://bitquill.atlassian.net/browse/AD-609
      break;
    }

    case ResultColumn::CHAR_OCTET_LENGTH: {
      buffer.PutInt32(type_traits::BinaryTypeCharOctetLength(columnType));
      break;
    }

    case ResultColumn::ORDINAL_POSITION: {
      buffer.PutInt32(currentColumn.GetOrdinalPosition());
      break;
    }

    case ResultColumn::IS_NULLABLE: {
      buffer.PutString(
          type_traits::NullabilityToIsNullable(currentColumn.GetNullability()));
      break;
    }

    default: {
      diag.AddStatusRecord(SqlState::S07009_INVALID_DESCRIPTOR_INDEX,
                           "Invalid index.");
      return SqlResult::AI_ERROR;
      break;
    }
  }

  return SqlResult::AI_SUCCESS;
}

SqlResult::Type ColumnMetadataQuery::Close() {
  meta.clear();

  executed = false;

  return SqlResult::AI_SUCCESS;
}

bool ColumnMetadataQuery::DataAvailable() const {
  return cursor != meta.end();
}

int64_t ColumnMetadataQuery::AffectedRows() const {
  return 0;
}

SqlResult::Type ColumnMetadataQuery::NextResultSet() {
  return SqlResult::AI_NO_DATA;
}


SqlResult::Type ColumnMetadataQuery::MakeRequestGetColumnsMeta() {
  // meta should only be cleared here as MakeRequestGetColumnsMetaPerTable() could be called multiple times
  meta.clear();

  // check if the schema/table has pattern characters in them. If they are
  // different with their pattern string, that means they could contain "\",
  // "_" or "%". This logic should be replaced by checking SQL_ATTR_METADATA_ID
  // value when implement AT-1150.
  std::string schemaPattern = utility::ConvertPatternToRegex(schema.value());
  std::string tablePattern = utility::ConvertPatternToRegex(table);  
  if (schema.value() != schemaPattern || table != tablePattern) {
    SqlResult::Type result = tableMetadataQuery_->Execute();
    if (result != SqlResult::AI_SUCCESS) {
      LOG_WARNING_MSG("Failed to get table metadata for " << (*schema) << "." << table);
      return SqlResult::AI_NO_DATA;
    }

    app::ColumnBindingMap columnBindings;
    SqlLen buflen = STRING_BUFFER_SIZE;
    // According to Timestream database name could only contain
    // letters, digits, dashes, periods or underscores. It could 
    // not be a unicode string.
    char schemaName[STRING_BUFFER_SIZE]{};
    ApplicationDataBuffer buf1(
        ignite::odbc::type_traits::OdbcNativeType::Type::AI_CHAR, schemaName,
        buflen, nullptr);
    columnBindings[TableMetadataQuery::ResultColumn::TABLE_SCHEM] = buf1;

    // According to Timestream, table name could only contain
    // letters, digits, dashes, periods or underscores. It could
    // not be a unicode string.
    char tableName[STRING_BUFFER_SIZE]{};
    ApplicationDataBuffer buf2(
        ignite::odbc::type_traits::OdbcNativeType::Type::AI_CHAR, &tableName,
        buflen, nullptr);
    columnBindings[TableMetadataQuery::ResultColumn::TABLE_NAME] = buf2;

    while (tableMetadataQuery_->FetchNextRow(columnBindings)
           == SqlResult::AI_SUCCESS) {
      result = MakeRequestGetColumnsMetaPerTable(std::string(schemaName),std::string(tableName));
      if (result != SqlResult::AI_SUCCESS) {
        LOG_ERROR_MSG("Failed to get columns for " << schemaName << "." << tableName);
        break;
      }
    }
    return result;
  } else {
    return MakeRequestGetColumnsMetaPerTable(schema.value(), table);
  }
}

SqlResult::Type ColumnMetadataQuery::MakeRequestGetColumnsMetaPerTable(const std::string& schemaName, const std::string& tableName) {
  std::string sql = "describe ";
  sql += schemaName;
  sql += ".";
  sql += tableName;

  app::ParameterSet params;
  int32_t timeout = 60;

  dataQuery_ =
      std::make_shared< DataQuery >(diag, connection, sql, params, timeout);
  SqlResult::Type result = dataQuery_->Execute();
  if (result != SqlResult::AI_SUCCESS) {
    LOG_WARNING_MSG("Failed to execute sql:" << sql);
    return SqlResult::AI_NO_DATA;
  }

  app::ColumnBindingMap columnBindings;
  SqlLen buflen = STRING_BUFFER_SIZE;
  // column name could be a unicode string
  SQLWCHAR columnName[STRING_BUFFER_SIZE];
  ApplicationDataBuffer buf1(
      ignite::odbc::type_traits::OdbcNativeType::Type::AI_WCHAR, columnName, buflen,
      nullptr);
  columnBindings[1] = buf1;

  char dataType[64];
  ApplicationDataBuffer buf2(
      ignite::odbc::type_traits::OdbcNativeType::Type::AI_CHAR, &dataType,
      buflen, nullptr);
  columnBindings[2] = buf2;

  char remarks[64];
  ApplicationDataBuffer buf3(
      ignite::odbc::type_traits::OdbcNativeType::Type::AI_CHAR, &remarks,
      buflen, nullptr);
  columnBindings[3] = buf3;

  int32_t prevPosition = 0;
  while (dataQuery_->FetchNextRow(columnBindings) == SqlResult::AI_SUCCESS) {
    if (column.empty() || column == "%"
            || column == utility::SqlWcharToString(columnName,STRING_BUFFER_SIZE)) {
      meta.emplace_back(ignite::odbc::meta::ColumnMeta(schemaName, tableName));
      meta.back().Read(columnBindings, ++prevPosition);
    }
  }

  return result;
}
}  // namespace query
}  // namespace odbc
}  // namespace ignite
