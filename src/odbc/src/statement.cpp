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
 *
 * Modifications Copyright Amazon.com, Inc. or its affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "timestream/odbc/statement.h"

#include <boost/optional.hpp>
#include <limits>

#include "timestream/odbc/connection.h"
#include "timestream/odbc/log.h"
#include "ignite/odbc/odbc_error.h"
#include "timestream/odbc/query/column_metadata_query.h"
#include "timestream/odbc/query/column_privileges_query.h"
#include "timestream/odbc/query/data_query.h"
#include "timestream/odbc/query/foreign_keys_query.h"
#include "timestream/odbc/query/primary_keys_query.h"
#include "timestream/odbc/query/procedure_columns_query.h"
#include "timestream/odbc/query/procedures_query.h"
#include "timestream/odbc/query/statistics_query.h"
#include "timestream/odbc/query/special_columns_query.h"
#include "timestream/odbc/query/table_metadata_query.h"
#include "timestream/odbc/query/table_privileges_query.h"
#include "timestream/odbc/query/type_info_query.h"
#include "timestream/odbc/system/odbc_constants.h"
#include "timestream/odbc/utility.h"

namespace timestream {
namespace odbc {
Statement::Statement(Connection& parent)
    : connection(parent),
      columnBindings(),
      currentQuery(),
      rowsFetched(0),
      rowStatuses(0),
      columnBindOffset(0),
      rowArraySize(1),
      timeout(0) {
}

Statement::~Statement() {
  // No-op.
}

void Statement::BindColumn(uint16_t columnIdx, int16_t targetType,
                           void* targetValue, SqlLen bufferLength,
                           SqlLen* strLengthOrIndicator) {
  IGNITE_ODBC_API_CALL(InternalBindColumn(columnIdx, targetType, targetValue,
                                          bufferLength, strLengthOrIndicator));
}

SqlResult::Type Statement::InternalBindColumn(uint16_t columnIdx,
                                              int16_t targetType,
                                              void* targetValue,
                                              SqlLen bufferLength,
                                              SqlLen* strLengthOrIndicator) {
  LOG_DEBUG_MSG("InternalBindColumn is called with columnIdx "
                << columnIdx << ", targetType " << targetType
                << ", targetValue " << targetValue << ", bufferLength "
                << bufferLength << ", strLengthOrIndicator "
                << strLengthOrIndicator);
  using namespace type_traits;
  OdbcNativeType::Type driverType = ToDriverType(targetType);

  if (driverType == OdbcNativeType::AI_UNSUPPORTED) {
    AddStatusRecord(SqlState::SHY003_INVALID_APPLICATION_BUFFER_TYPE,
                    "The argument TargetType was not a valid data type.");

    return SqlResult::AI_ERROR;
  }

  if (bufferLength < 0) {
    AddStatusRecord(
        SqlState::SHY090_INVALID_STRING_OR_BUFFER_LENGTH,
        "The value specified for the argument BufferLength was less than 0.");

    return SqlResult::AI_ERROR;
  }

  if (targetValue || strLengthOrIndicator) {
    app::ApplicationDataBuffer dataBuffer(driverType, targetValue, bufferLength,
                                          strLengthOrIndicator);

    SafeBindColumn(columnIdx, dataBuffer);
  } else
    SafeUnbindColumn(columnIdx);

  return SqlResult::AI_SUCCESS;
}

void Statement::SafeBindColumn(uint16_t columnIdx,
                               const app::ApplicationDataBuffer& buffer) {
  columnBindings[columnIdx] = buffer;
}

void Statement::SafeUnbindColumn(uint16_t columnIdx) {
  columnBindings.erase(columnIdx);
}

void Statement::SafeUnbindAllColumns() {
  columnBindings.clear();
}

void Statement::SetColumnBindOffsetPtr(int* ptr) {
  columnBindOffset = ptr;
}

int* Statement::GetColumnBindOffsetPtr() {
  return columnBindOffset;
}

int32_t Statement::GetColumnNumber() {
  int32_t res;

  IGNITE_ODBC_API_CALL(InternalGetColumnNumber(res));

  return res;
}

SqlResult::Type Statement::InternalGetColumnNumber(int32_t& res) {
  const meta::ColumnMetaVector* meta = GetMeta();

  if (!meta) {
    LOG_DEBUG_MSG("meta object is not found");
    res = 0;

    return SqlResult::AI_ERROR;
  }

  res = static_cast< int32_t >(meta->size());

  return SqlResult::AI_SUCCESS;
}

void Statement::SetAttribute(int attr, void* value, SQLINTEGER valueLen) {
  IGNITE_ODBC_API_CALL(InternalSetAttribute(attr, value, valueLen));
}

SqlResult::Type Statement::InternalSetAttribute(int attr, void* value,
                                                SQLINTEGER) {
  LOG_DEBUG_MSG("InternalSetAttribute is called with attr " << attr);
  switch (attr) {
    case SQL_ATTR_ROW_ARRAY_SIZE: {
      SqlUlen val = reinterpret_cast< SqlUlen >(value);

      LOG_DEBUG_MSG("SQL_ATTR_ROW_ARRAY_SIZE: " << val);

      if (val != 1) {
        AddStatusRecord(
            SqlState::SIM001_FUNCTION_NOT_SUPPORTED,
            "Array size value cannot be set to a value other than 1");

        return SqlResult::AI_ERROR;
      } else if (rowArraySize != 1) {
        // val is 1
        rowArraySize = 1;
        LOG_DEBUG_MSG(
            "else if (rowArraySize != 1) branch is executed."
            "This branch should not be executed as we currently do not support "
            "rowArraySize to have values other than 1.");
      }

      LOG_DEBUG_MSG("rowArraySize: " << rowArraySize);

      break;
    }

    case SQL_ATTR_ROW_BIND_TYPE: {
      SqlUlen rowBindType = reinterpret_cast< SqlUlen >(value);

      if (rowBindType != SQL_BIND_BY_COLUMN) {
        AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                        "Only binding by column is currently supported");

        return SqlResult::AI_ERROR;
      }

      break;
    }

    case SQL_ATTR_ROWS_FETCHED_PTR: {
      SetRowsFetchedPtr(reinterpret_cast< SQLINTEGER* >(value));

      break;
    }

    case SQL_ATTR_ROW_STATUS_PTR: {
      SetRowStatusesPtr(reinterpret_cast< SQLUSMALLINT* >(value));

      break;
    }

    case SQL_ATTR_PARAM_BIND_TYPE: {
      SqlUlen paramBindType = reinterpret_cast< SqlUlen >(value);

      if (paramBindType != SQL_PARAM_BIND_BY_COLUMN) {
        AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                        "Only binding by column is currently supported");

        return SqlResult::AI_ERROR;
      }

      break;
    }

    case SQL_ATTR_ROW_BIND_OFFSET_PTR: {
      SetColumnBindOffsetPtr(reinterpret_cast< int* >(value));

      break;
    }

    case SQL_ATTR_QUERY_TIMEOUT: {
      SqlUlen uTimeout = reinterpret_cast< SqlUlen >(value);

      if (uTimeout > INT32_MAX) {
        timeout = INT32_MAX;

        std::stringstream ss;

        ss << "Value is too big: " << uTimeout << ", changing to " << timeout
           << ".";
        std::string msg = ss.str();

        AddStatusRecord(SqlState::S01S02_OPTION_VALUE_CHANGED, msg,
                        timestream::odbc::LogLevel::Type::INFO_LEVEL);

        return SqlResult::AI_SUCCESS_WITH_INFO;
      }

      timeout = static_cast< int32_t >(uTimeout);

      break;
    }

    default: {
      AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "Specified attribute is not supported.");

      return SqlResult::AI_ERROR;
    }
  }

  return SqlResult::AI_SUCCESS;
}

void Statement::GetAttribute(int attr, void* buf, SQLINTEGER bufLen,
                             SQLINTEGER* valueLen) {
  IGNITE_ODBC_API_CALL(InternalGetAttribute(attr, buf, bufLen, valueLen));
}

SqlResult::Type Statement::InternalGetAttribute(int attr, void* buf, SQLINTEGER,
                                                SQLINTEGER* valueLen) {
  LOG_DEBUG_MSG("InternalGetAttribute is called with attr " << attr);
  if (!buf) {
    AddStatusRecord("Data buffer is NULL.");

    return SqlResult::AI_ERROR;
  }

  switch (attr) {
    case SQL_ATTR_APP_ROW_DESC:
    case SQL_ATTR_APP_PARAM_DESC:
    case SQL_ATTR_IMP_ROW_DESC:
    case SQL_ATTR_IMP_PARAM_DESC: {
      SQLPOINTER* val = reinterpret_cast< SQLPOINTER* >(buf);

      *val = static_cast< SQLPOINTER >(this);

      if (valueLen)
        *valueLen = SQL_IS_POINTER;

      break;
    }

    case SQL_ATTR_ROW_BIND_TYPE: {
      SqlUlen* val = reinterpret_cast< SqlUlen* >(buf);

      *val = SQL_BIND_BY_COLUMN;

      break;
    }

    case SQL_ATTR_ROW_ARRAY_SIZE: {
      SQLINTEGER* val = reinterpret_cast< SQLINTEGER* >(buf);

      *val = static_cast< SQLINTEGER >(rowArraySize);

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;
      LOG_DEBUG_MSG("*val is " << *val << ", *valueLen is "
                               << (valueLen ? *valueLen : 0));
      break;
    }

    case SQL_ATTR_ROWS_FETCHED_PTR: {
      SqlUlen** val = reinterpret_cast< SqlUlen** >(buf);

      *val = reinterpret_cast< SqlUlen* >(GetRowsFetchedPtr());

      if (valueLen)
        *valueLen = SQL_IS_POINTER;

      LOG_DEBUG_MSG("*val is " << *val << ", *valueLen is "
                               << (valueLen ? *valueLen : 0));
      break;
    }

    case SQL_ATTR_ROW_STATUS_PTR: {
      SQLUSMALLINT** val = reinterpret_cast< SQLUSMALLINT** >(buf);

      *val = reinterpret_cast< SQLUSMALLINT* >(GetRowStatusesPtr());

      if (valueLen)
        *valueLen = SQL_IS_POINTER;

      LOG_DEBUG_MSG("*val is " << *val << ", *valueLen is "
                               << (valueLen ? *valueLen : 0));
      break;
    }

    case SQL_ATTR_PARAM_BIND_TYPE: {
      SqlUlen* val = reinterpret_cast< SqlUlen* >(buf);

      *val = SQL_PARAM_BIND_BY_COLUMN;

      break;
    }

    case SQL_ATTR_ROW_BIND_OFFSET_PTR: {
      SqlUlen** val = reinterpret_cast< SqlUlen** >(buf);

      *val = reinterpret_cast< SqlUlen* >(GetColumnBindOffsetPtr());

      if (valueLen)
        *valueLen = SQL_IS_POINTER;

      LOG_DEBUG_MSG("*val is " << *val << ", *valueLen is "
                               << (valueLen ? *valueLen : 0));
      break;
    }

    case SQL_ATTR_QUERY_TIMEOUT: {
      SqlUlen* uTimeout = reinterpret_cast< SqlUlen* >(buf);

      *uTimeout = static_cast< SqlUlen >(timeout);

      LOG_DEBUG_MSG("*uTimeout is " << *uTimeout);
      break;
    }

    default: {
      AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "Specified attribute is not supported.");

      return SqlResult::AI_ERROR;
    }
  }

  return SqlResult::AI_SUCCESS;
}

void Statement::GetColumnData(uint16_t columnIdx,
                              app::ApplicationDataBuffer& buffer) {
  IGNITE_ODBC_API_CALL(InternalGetColumnData(columnIdx, buffer));
}

SqlResult::Type Statement::InternalGetColumnData(
    uint16_t columnIdx, app::ApplicationDataBuffer& buffer) {
  LOG_DEBUG_MSG("InternalGetColumnData is called");
  if (!currentQuery.get()) {
    std::string errMsg = "Cursor is not in the open state.";
    AddStatusRecord(SqlState::S24000_INVALID_CURSOR_STATE, errMsg);
    return SqlResult::AI_ERROR;
  }

  SqlResult::Type res = currentQuery->GetColumn(columnIdx, buffer);

  return res;
}

void Statement::PrepareSqlQuery(const std::string& query) {
  IGNITE_ODBC_API_CALL(InternalPrepareSqlQuery(query));
}

SqlResult::Type Statement::ProcessInternalCommand(const std::string& query) {
  // not supported
  return SqlResult::AI_ERROR;
}

SqlResult::Type Statement::InternalPrepareSqlQuery(const std::string& query) {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(
      new query::DataQuery(*this, connection, query, timeout));

  return SqlResult::AI_SUCCESS;
}

void Statement::ExecuteSqlQuery(const std::string& query) {
  IGNITE_ODBC_API_CALL(InternalExecuteSqlQuery(query));
}

SqlResult::Type Statement::InternalExecuteSqlQuery(const std::string& query) {
  LOG_DEBUG_MSG("InternalExecuteSqlQuery is called for query " << query);
  SqlResult::Type result = InternalPrepareSqlQuery(query);

  if (result != SqlResult::AI_SUCCESS)
    return result;

  return InternalExecuteSqlQuery();
}

void Statement::ExecuteSqlQuery() {
  IGNITE_ODBC_API_CALL(InternalExecuteSqlQuery());
}

SqlResult::Type Statement::InternalExecuteSqlQuery() {
  LOG_DEBUG_MSG("InternalExecuteSqlQuery is called");
  if (!currentQuery.get()) {
    AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR, "Query is not prepared.");

    return SqlResult::AI_ERROR;
  }

  SqlResult::Type retval = currentQuery->Execute();
  // For SQLExecute() when the query result is empty according to Microsoft document it should be SUCCESS.
  // SQL_NO_DATA is only used for DML statements. The DataQuery::Execute() needs to keep AI_NO_DATA as it is needed by 
  // TableMetadataQuery::getMatchedTables().
  if (retval == SqlResult::AI_NO_DATA) {
    AddStatusRecord(SqlState::S01000_GENERAL_WARNING, "Query result is empty");
    retval = SqlResult::AI_SUCCESS_WITH_INFO;
  }
  return retval;
}

void Statement::CancelSqlQuery() {
  IGNITE_ODBC_API_CALL(InternalCancelSqlQuery());
}

SqlResult::Type Statement::InternalCancelSqlQuery() {
  LOG_DEBUG_MSG("InternalCancelSqlQuery is called");
  if (!currentQuery.get()) {
    AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR, "Query does not exist.");

    return SqlResult::AI_ERROR;
  }

  return currentQuery->Cancel();
}

void Statement::ExecuteGetColumnsMetaQuery(
    const boost::optional< std::string >& catalog,
    const boost::optional< std::string >& schema, 
    const boost::optional< std::string >& table,
    const boost::optional< std::string >& column) {
  IGNITE_ODBC_API_CALL(
      InternalExecuteGetColumnsMetaQuery(catalog, schema, table, column));
}

SqlResult::Type Statement::InternalExecuteGetColumnsMetaQuery(
    const boost::optional< std::string >& catalog,
    const boost::optional< std::string >& schema, 
    const boost::optional< std::string >& table,
    const boost::optional< std::string >& column) {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::ColumnMetadataQuery(*this, connection, catalog,
                                                    schema, table, column));

  return currentQuery->Execute();
}

void Statement::ExecuteGetTablesMetaQuery(
    const boost::optional< std::string >& catalog,
    const boost::optional< std::string >& schema,
    const boost::optional< std::string >& table,
    const boost::optional< std::string >& tableType) {
  IGNITE_ODBC_API_CALL(
      InternalExecuteGetTablesMetaQuery(catalog, schema, table, tableType));
}

SqlResult::Type Statement::InternalExecuteGetTablesMetaQuery(
    const boost::optional< std::string >& catalog,
    const boost::optional< std::string >& schema,
    const boost::optional< std::string >& table,
    const boost::optional< std::string >& tableType) {
  LOG_DEBUG_MSG("InternalExecuteGetTablesMetaQuery is called");
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::TableMetadataQuery(*this, connection, catalog,
                                                   schema, table, tableType));

  return currentQuery->Execute();
}

void Statement::ExecuteGetForeignKeysQuery() {
  IGNITE_ODBC_API_CALL(InternalExecuteGetForeignKeysQuery());
}

SqlResult::Type Statement::InternalExecuteGetForeignKeysQuery() {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::ForeignKeysQuery(
      *this));

  return currentQuery->Execute();
}

void Statement::ExecuteGetPrimaryKeysQuery() {
  IGNITE_ODBC_API_CALL(
      InternalExecuteGetPrimaryKeysQuery());
}

SqlResult::Type Statement::InternalExecuteGetPrimaryKeysQuery() {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(
      new query::PrimaryKeysQuery(*this));

  return currentQuery->Execute();
}

void Statement::ExecuteSpecialColumnsQuery() {
  IGNITE_ODBC_API_CALL(InternalExecuteSpecialColumnsQuery());
}

SqlResult::Type Statement::InternalExecuteSpecialColumnsQuery() {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::SpecialColumnsQuery(
      *this));

  return currentQuery->Execute();
}

void Statement::ExecuteStatisticsQuery() {
  IGNITE_ODBC_API_CALL(InternalExecuteStatisticsQuery());
}

SqlResult::Type Statement::InternalExecuteStatisticsQuery() {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::StatisticsQuery(*this, connection.GetEnvODBCVer()));

  return currentQuery->Execute();
}

void Statement::ExecuteProcedureColumnsQuery() {
  IGNITE_ODBC_API_CALL(InternalExecuteProcedureColumnsQuery());
}

SqlResult::Type Statement::InternalExecuteProcedureColumnsQuery() {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::ProcedureColumnsQuery(*this));

  return currentQuery->Execute();
}

void Statement::ExecuteProceduresQuery() {
  IGNITE_ODBC_API_CALL(InternalExecuteProceduresQuery());
}

SqlResult::Type Statement::InternalExecuteProceduresQuery() {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(
      new query::ProceduresQuery(*this));

  return currentQuery->Execute();
}

void Statement::ExecuteColumnPrivilegesQuery() {
  IGNITE_ODBC_API_CALL(InternalExecuteColumnPrivilegesQuery());
}

SqlResult::Type Statement::InternalExecuteColumnPrivilegesQuery() {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::ColumnPrivilegesQuery(*this));

  return currentQuery->Execute();
}

void Statement::ExecuteTablePrivilegesQuery() {
  IGNITE_ODBC_API_CALL(InternalExecuteTablePrivilegesQuery());
}

SqlResult::Type Statement::InternalExecuteTablePrivilegesQuery() {
  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::TablePrivilegesQuery(*this));

  return currentQuery->Execute();
}

void Statement::ExecuteGetTypeInfoQuery(int16_t sqlType) {
  IGNITE_ODBC_API_CALL(InternalExecuteGetTypeInfoQuery(sqlType));
}

SqlResult::Type Statement::InternalExecuteGetTypeInfoQuery(int16_t sqlType) {
  LOG_DEBUG_MSG("InternalExecuteGetTypeInfoQuery is called with sqlType "
                << sqlType);
  if (sqlType != SQL_ALL_TYPES && !type_traits::IsSqlTypeSupported(sqlType)) {
    std::stringstream builder;
    builder << "Data type is not supported. [typeId=" << sqlType << ']';

    AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                    builder.str());

    return SqlResult::AI_ERROR;
  }

  if (currentQuery.get())
    currentQuery->Close();

  currentQuery.reset(new query::TypeInfoQuery(*this, sqlType));

  return currentQuery->Execute();
}

void Statement::FreeResources(int16_t option) {
  IGNITE_ODBC_API_CALL(InternalFreeResources(option));
}

SqlResult::Type Statement::InternalFreeResources(int16_t option) {
  LOG_DEBUG_MSG("InternalFreeResources is called with option " << option);
  switch (option) {
    case SQL_DROP: {
      AddStatusRecord("Deprecated, call SQLFreeHandle instead");

      return SqlResult::AI_ERROR;
    }

    case SQL_CLOSE: {
      return InternalClose();
    }

    case SQL_UNBIND: {
      SafeUnbindAllColumns();

      break;
    }

    default: {
      AddStatusRecord(
          SqlState::SHY092_OPTION_TYPE_OUT_OF_RANGE,
          "The value specified for the argument Option was invalid");
      return SqlResult::AI_ERROR;
    }
  }
  return SqlResult::AI_SUCCESS;
}

void Statement::Close() {
  IGNITE_ODBC_API_CALL(InternalClose());
}

SqlResult::Type Statement::InternalClose() {
  if (!currentQuery.get())
    return SqlResult::AI_SUCCESS;

  if (!currentQuery->DataAvailable()) {
    AddStatusRecord(SqlState::S24000_INVALID_CURSOR_STATE,
                    "No cursor was open");
    return SqlResult::AI_ERROR;
  }

  SqlResult::Type result = currentQuery->Close();

  return result;
}

void Statement::FetchScroll(int16_t orientation, int64_t offset) {
  IGNITE_ODBC_API_CALL(InternalFetchScroll(orientation, offset));
}

SqlResult::Type Statement::InternalFetchScroll(int16_t orientation,
                                               int64_t offset) {
  LOG_DEBUG_MSG("InternalFetchScroll is called with orientation "
                << orientation);
  UNREFERENCED_PARAMETER(offset);

  if (orientation != SQL_FETCH_NEXT) {
    AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                    "Only SQL_FETCH_NEXT FetchOrientation type is supported");

    return SqlResult::AI_ERROR;
  }

  return InternalFetchRow();
}

void Statement::FetchRow() {
  IGNITE_ODBC_API_CALL(InternalFetchRow());
}

SqlResult::Type Statement::InternalFetchRow() {
  LOG_DEBUG_MSG("InternalFetchRow is called");
  if (rowsFetched)
    *rowsFetched = 0;

  if (!currentQuery.get()) {
    std::string errMsg = "Cursor is not in the open state.";
    AddStatusRecord(SqlState::S24000_INVALID_CURSOR_STATE, errMsg);
    return SqlResult::AI_ERROR;
  }

  if (columnBindOffset) {
    for (app::ColumnBindingMap::iterator it = columnBindings.begin();
         it != columnBindings.end(); ++it)
      it->second.SetByteOffset(*columnBindOffset);
  }

  SQLINTEGER fetched = 0;
  SQLINTEGER errors = 0;

  LOG_DEBUG_MSG("rowArraySize is " << rowArraySize);
  for (SqlUlen i = 0; i < rowArraySize; ++i) {
    for (app::ColumnBindingMap::iterator it = columnBindings.begin();
         it != columnBindings.end(); ++it)
      it->second.SetElementOffset(i);

    SqlResult::Type res = currentQuery->FetchNextRow(columnBindings);

    if (res == SqlResult::AI_SUCCESS || res == SqlResult::AI_SUCCESS_WITH_INFO)
      ++fetched;
    else if (res != SqlResult::AI_NO_DATA)
      ++errors;

    if (rowStatuses)
      rowStatuses[i] = SqlResultToRowResult(res);
  }

  if (rowsFetched)
    *rowsFetched =
        fetched < 0 ? static_cast< SQLINTEGER >(rowArraySize) : fetched;

  if (fetched > 0)
    return errors == 0 ? SqlResult::AI_SUCCESS
                       : SqlResult::AI_SUCCESS_WITH_INFO;

  LOG_DEBUG_MSG("rowsFetched is " << rowsFetched << ", fetched is " << fetched
                                  << ", errors is " << errors);
  return errors == 0 ? SqlResult::AI_NO_DATA : SqlResult::AI_ERROR;
}

const meta::ColumnMetaVector* Statement::GetMeta() {
  LOG_DEBUG_MSG("GetMeta is called");
  if (!currentQuery.get()) {
    AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR, "Query is not executed.");

    return 0;
  }

  return currentQuery->GetMeta();
}

bool Statement::DataAvailable() const {
  return currentQuery.get() && currentQuery->DataAvailable();
}

void Statement::MoreResults() {
  IGNITE_ODBC_API_CALL(InternalMoreResults());
}

SqlResult::Type Statement::InternalMoreResults() {
  LOG_DEBUG_MSG("InternalMoreResults is called");
  if (!currentQuery.get()) {
    AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR, "Query is not executed.");

    return SqlResult::AI_ERROR;
  }

  return currentQuery->NextResultSet();
}

void Statement::GetColumnAttribute(uint16_t colIdx, uint16_t attrId,
                                   SQLWCHAR* strbuf, int16_t buflen,
                                   int16_t* reslen, SqlLen* numbuf) {
  IGNITE_ODBC_API_CALL(InternalGetColumnAttribute(colIdx, attrId, strbuf,
                                                  buflen, reslen, numbuf));
}

SqlResult::Type Statement::InternalGetColumnAttribute(
    uint16_t colIdx, uint16_t attrId, SQLWCHAR* strbuf, int16_t buflen,
    int16_t* reslen, SqlLen* numbuf) {
  LOG_DEBUG_MSG("InternalGetColumnAttribute is called with Column ID: "
                << colIdx << ", Attribute ID: " << attrId << " ("
                << meta::ColumnMeta::AttrIdToString(attrId)
                << "), buflen: " << buflen);
  const meta::ColumnMetaVector* meta = GetMeta();

  if (!meta) {
    LOG_ERROR_MSG(
        "meta object is not found. Returning "
        "SqlResult::AI_ERROR.");
    return SqlResult::AI_ERROR;
  }

  if (colIdx > meta->size() || colIdx < 1) {
    AddStatusRecord(SqlState::SHY000_GENERAL_ERROR,
                    "Column index is out of range.",
                    timestream::odbc::LogLevel::Type::ERROR_LEVEL, 0, colIdx);

    return SqlResult::AI_ERROR;
  }

  const meta::ColumnMeta& columnMeta = meta->at(colIdx - 1);

  bool found = false;

  LOG_DEBUG_MSG("numbuf: " << numbuf);

  // NumericAttributePtr field is used.
  if (numbuf) {
    found = columnMeta.GetAttribute(attrId, *numbuf);
    LOG_DEBUG_MSG("numbuf found: " << numbuf << ", found is " << found);
  }

  // NumericAttributePtr field is unused.
  if (!found) {
    std::string out;

    found = columnMeta.GetAttribute(attrId, out);
    LOG_DEBUG_MSG("out is " << out << ", found is " << found);
    size_t outSize = out.size();

    if (found) {
      bool isTruncated = false;
      if (strbuf) {
        // Length is given in bytes
        outSize =
            utility::CopyStringToBuffer(out, strbuf, buflen, isTruncated, true);
        LOG_DEBUG_MSG("strbuf is " << strbuf << ", out is " << out
                                   << ", outSize is " << outSize
                                   << ", isTruncated is " << isTruncated);
      }
      if (reslen)
        *reslen = static_cast< int16_t >(outSize);
      if (isTruncated)
        return SqlResult::AI_SUCCESS_WITH_INFO;
    }
  }

  if (!found) {
    AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                    "Unknown attribute.");

    return SqlResult::AI_ERROR;
  }

  return SqlResult::AI_SUCCESS;
}

int64_t Statement::AffectedRows() {
  int64_t rowCnt = 0;

  IGNITE_ODBC_API_CALL(InternalAffectedRows(rowCnt));

  return rowCnt;
}

SqlResult::Type Statement::InternalAffectedRows(int64_t& rowCnt) {
  LOG_DEBUG_MSG("InternalAffectedRows is called");
  if (!currentQuery.get()) {
    AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR, "Query is not executed.");

    return SqlResult::AI_ERROR;
  }

  rowCnt = currentQuery->AffectedRows();

  return SqlResult::AI_SUCCESS;
}

void Statement::SetRowsFetchedPtr(SQLINTEGER* ptr) {
  rowsFetched = ptr;
}

SQLINTEGER* Statement::GetRowsFetchedPtr() {
  return rowsFetched;
}

void Statement::SetRowStatusesPtr(SQLUSMALLINT* ptr) {
  rowStatuses = ptr;
}

SQLUSMALLINT* Statement::GetRowStatusesPtr() {
  return rowStatuses;
}

SqlResult::Type Statement::UpdateParamsMeta() {
    // not supported
    return SqlResult::AI_ERROR;
}

uint16_t Statement::SqlResultToRowResult(SqlResult::Type value) {
  LOG_DEBUG_MSG("SqlResultToRowResult is called with value " << value);
  switch (value) {
    case SqlResult::AI_NO_DATA:
      return SQL_ROW_NOROW;

    case SqlResult::AI_SUCCESS:
      return SQL_ROW_SUCCESS;

    case SqlResult::AI_SUCCESS_WITH_INFO:
      return SQL_ROW_SUCCESS_WITH_INFO;

    default:
      return SQL_ROW_ERROR;
  }
}
}  // namespace odbc
}  // namespace timestream
