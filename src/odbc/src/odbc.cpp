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

#include "ignite/odbc.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ignite/odbc/config/configuration.h"
#include "ignite/odbc/config/connection_string_parser.h"
#include "ignite/odbc/connection.h"
#include "ignite/odbc/dsn_config.h"
#include "ignite/odbc/environment.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/statement.h"
#include "ignite/odbc/system/odbc_constants.h"
#include "ignite/odbc/system/system_dsn.h"
#include "ignite/odbc/type_traits.h"
#include "ignite/odbc/utility.h"

/**
 * Handle window handle.
 * @param windowHandle Window handle.
 * @param config Configuration.
 * @return @c true on success and @c false otherwise.
 */
bool HandleParentWindow(SQLHWND windowHandle,
                        ignite::odbc::config::Configuration& config) {
#ifdef _WIN32
  if (windowHandle) {
    LOG_INFO_MSG("Parent window is passed. Creating configuration window.");
    return DisplayConnectionWindow(windowHandle, config);
  }
#else
  IGNITE_UNUSED(windowHandle);
  IGNITE_UNUSED(config);
#endif
  return true;
}

using namespace ignite::odbc::utility;

namespace ignite {
SQLRETURN SQLGetInfo(SQLHDBC conn, SQLUSMALLINT infoType, SQLPOINTER infoValue,
                     SQLSMALLINT infoValueMax, SQLSMALLINT* length) {
  using odbc::Connection;
  using odbc::config::ConnectionInfo;

  LOG_DEBUG_MSG("SQLGetInfo called: "
                << infoType << " ("
                << ConnectionInfo::InfoTypeToString(infoType) << "), "
                << std::hex << reinterpret_cast< size_t >(infoValue) << ", "
                << infoValueMax << ", " << std::hex
                << reinterpret_cast< size_t >(length));

  Connection* connection = reinterpret_cast< Connection* >(conn);

  if (!connection)
    return SQL_INVALID_HANDLE;

  connection->GetInfo(infoType, infoValue, infoValueMax, length);

  return connection->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLAllocHandle(SQLSMALLINT type, SQLHANDLE parent,
                         SQLHANDLE* result) {
  LOG_DEBUG_MSG("SQLAllocHandle called with type " << type);
  switch (type) {
    case SQL_HANDLE_ENV:
      return SQLAllocEnv(result);

    case SQL_HANDLE_DBC:
      return SQLAllocConnect(parent, result);

    case SQL_HANDLE_STMT:
      return SQLAllocStmt(parent, result);

    case SQL_HANDLE_DESC: {
      using odbc::Connection;

      Connection* connection = reinterpret_cast< Connection* >(parent);

      if (!connection) {
        LOG_ERROR_MSG("SQLAllocHandle exiting with SQL_INVALID_HANDLE");
        return SQL_INVALID_HANDLE;
      }

      if (result)
        *result = 0;

      connection->GetDiagnosticRecords().Reset();
      connection->AddStatusRecord(
          odbc::SqlState::SIM001_FUNCTION_NOT_SUPPORTED,
          "The HandleType argument was SQL_HANDLE_DESC, and "
          "the driver does not support allocating a descriptor handle");

      return SQL_ERROR;
    }
    default:
      break;
  }

  *result = 0;

  return SQL_ERROR;
}

SQLRETURN SQLAllocEnv(SQLHENV* env) {
  using odbc::Environment;

  LOG_DEBUG_MSG("SQLAllocEnv called");

  *env = reinterpret_cast< SQLHENV >(new Environment());

  return SQL_SUCCESS;
}

SQLRETURN SQLAllocConnect(SQLHENV env, SQLHDBC* conn) {
  using odbc::Connection;
  using odbc::Environment;

  LOG_DEBUG_MSG("SQLAllocConnect called");

  *conn = SQL_NULL_HDBC;

  Environment* environment = reinterpret_cast< Environment* >(env);

  if (!environment) {
    LOG_ERROR_MSG("environment is nullptr");
    return SQL_INVALID_HANDLE;
  }

  Connection* connection = environment->CreateConnection();

  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return environment->GetDiagnosticRecords().GetReturnCode();
  }

  *conn = reinterpret_cast< SQLHDBC >(connection);

  return SQL_SUCCESS;
}

SQLRETURN SQLAllocStmt(SQLHDBC conn, SQLHSTMT* stmt) {
  using odbc::Connection;
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLAllocStmt called");

  *stmt = SQL_NULL_HDBC;

  auto connection = static_cast< Connection* >(conn);

  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return SQL_INVALID_HANDLE;
  }

  Statement* statement = connection->CreateStatement();

  *stmt = reinterpret_cast< SQLHSTMT >(statement);

  return connection->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLFreeHandle(SQLSMALLINT type, SQLHANDLE handle) {
  LOG_DEBUG_MSG("SQLFreeHandle called with type " << type);

  switch (type) {
    case SQL_HANDLE_ENV:
      return SQLFreeEnv(handle);

    case SQL_HANDLE_DBC:
      return SQLFreeConnect(handle);

    case SQL_HANDLE_STMT:
      return SQLFreeStmt(handle, SQL_DROP);

    case SQL_HANDLE_DESC:
    default:
      break;
  }

  return SQL_ERROR;
}

SQLRETURN SQLFreeEnv(SQLHENV env) {
  using odbc::Environment;

  LOG_DEBUG_MSG("SQLFreeEnv called: " << env);

  Environment* environment = reinterpret_cast< Environment* >(env);

  if (!environment) {
    LOG_ERROR_MSG("environment is nullptr");
    return SQL_INVALID_HANDLE;
  }

  delete environment;

  return SQL_SUCCESS;
}

SQLRETURN SQLFreeConnect(SQLHDBC conn) {
  using odbc::Connection;

  LOG_DEBUG_MSG("SQLFreeConnect called");

  Connection* connection = reinterpret_cast< Connection* >(conn);

  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return SQL_INVALID_HANDLE;
  }

  connection->Deregister();

  delete connection;

  return SQL_SUCCESS;
}

SQLRETURN SQLFreeStmt(SQLHSTMT stmt, SQLUSMALLINT option) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLFreeStmt called [option=" << option << ']');

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  if (option == SQL_DROP) {
    delete statement;
    return SQL_SUCCESS;
  }

  statement->FreeResources(option);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLCloseCursor(SQLHSTMT stmt) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLCloseCursor called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  statement->Close();

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLDriverConnect(SQLHDBC conn, SQLHWND windowHandle,
                           SQLWCHAR* inConnectionString,
                           SQLSMALLINT inConnectionStringLen,
                           SQLWCHAR* outConnectionString,
                           SQLSMALLINT outConnectionStringBufferLen,
                           SQLSMALLINT* outConnectionStringLen,
                           SQLUSMALLINT driverCompletion) {
  IGNITE_UNUSED(driverCompletion);

  using odbc::Connection;
  using odbc::diagnostic::DiagnosticRecordStorage;
  using odbc::utility::CopyStringToBuffer;

  LOG_DEBUG_MSG("SQLDriverConnect called");

  Connection* connection = reinterpret_cast< Connection* >(conn);

  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return SQL_INVALID_HANDLE;
  }
  std::string connectStr =
      SqlWcharToString(inConnectionString, inConnectionStringLen);
  connection->Establish(connectStr, windowHandle);

  DiagnosticRecordStorage& diag = connection->GetDiagnosticRecords();
  if (!diag.IsSuccessful()) {
    LOG_INFO_MSG(
        "SQLFreeStmt exiting becase Diagnostic Record Storage shows operation "
        "is not successful");
    return diag.GetReturnCode();
  }

  bool isTruncated = false;

  size_t reslen = CopyStringToBuffer(
      connectStr, outConnectionString,
      static_cast< size_t >(outConnectionStringBufferLen), isTruncated);

  if (outConnectionStringLen)
    *outConnectionStringLen = static_cast< SQLSMALLINT >(reslen);

  return diag.GetReturnCode();
}

SQLRETURN SQLConnect(SQLHDBC conn, SQLWCHAR* serverName,
                     SQLSMALLINT serverNameLen, SQLWCHAR* userName,
                     SQLSMALLINT userNameLen, SQLWCHAR* auth,
                     SQLSMALLINT authLen) {
  using odbc::Connection;
  using odbc::config::Configuration;

  LOG_DEBUG_MSG("SQLConnect called\n");

  Connection* connection = reinterpret_cast< Connection* >(conn);

  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return SQL_INVALID_HANDLE;
  }

  odbc::config::Configuration config;

  std::string dsn = SqlWcharToString(serverName, serverNameLen);

  LOG_INFO_MSG("DSN: " << dsn);

  odbc::ReadDsnConfiguration(dsn.c_str(), config,
                             &connection->GetDiagnosticRecords());
  if (userName) {
    std::string userNameStr = SqlWcharToString(userName, userNameLen);
    config.SetUid(userNameStr);
  }
  if (auth) {
    std::string passwordStr = SqlWcharToString(auth, authLen);
    config.SetPwd(passwordStr);
  }

  connection->Establish(config);

  return connection->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLDisconnect(SQLHDBC conn) {
  using odbc::Connection;

  LOG_DEBUG_MSG("SQLDisconnect called");

  Connection* connection = reinterpret_cast< Connection* >(conn);

  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return SQL_INVALID_HANDLE;
  }

  connection->Release();

  return connection->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLPrepare(SQLHSTMT stmt, SQLWCHAR* query, SQLINTEGER queryLen) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLPrepare called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  std::string sql = SqlWcharToString(query, queryLen);

  LOG_INFO_MSG("SQL: " << sql);

  statement->PrepareSqlQuery(sql);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLExecute(SQLHSTMT stmt) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLExecute called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->ExecuteSqlQuery();

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLExecDirect(SQLHSTMT stmt, SQLWCHAR* query, SQLINTEGER queryLen) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLExecDirect called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  std::string sql = SqlWcharToString(query, queryLen);

  LOG_INFO_MSG("SQL: " << sql);

  statement->ExecuteSqlQuery(sql);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLBindCol(SQLHSTMT stmt, SQLUSMALLINT colNum, SQLSMALLINT targetType,
                     SQLPOINTER targetValue, SQLLEN bufferLength,
                     SQLLEN* strLengthOrIndicator) {
  using namespace odbc::type_traits;

  using odbc::Statement;
  using odbc::app::ApplicationDataBuffer;

  LOG_DEBUG_MSG("SQLBindCol called: index="
                << colNum << ", type=" << targetType
                << ", targetValue=" << reinterpret_cast< size_t >(targetValue)
                << ", bufferLength=" << bufferLength << ", lengthInd="
                << reinterpret_cast< size_t >(strLengthOrIndicator));

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->BindColumn(colNum, targetType, targetValue, bufferLength,
                        strLengthOrIndicator);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLFetch(SQLHSTMT stmt) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLFetch called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->FetchRow();

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLFetchScroll(SQLHSTMT stmt, SQLSMALLINT orientation,
                         SQLLEN offset) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLFetchScroll called with Orientation "
                << orientation << " Offset " << offset);

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->FetchScroll(orientation, offset);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLExtendedFetch(SQLHSTMT stmt, SQLUSMALLINT orientation,
                           SQLLEN offset, SQLULEN* rowCount,
                           SQLUSMALLINT* rowStatusArray) {
  LOG_DEBUG_MSG("SQLExtendedFetch called");

  SQLRETURN res = SQLFetchScroll(stmt, orientation, offset);

  if (res == SQL_SUCCESS) {
    if (rowCount)
      *rowCount = 1;

    if (rowStatusArray)
      rowStatusArray[0] = SQL_ROW_SUCCESS;
  } else if (res == SQL_NO_DATA && rowCount)
    *rowCount = 0;

  LOG_DEBUG_MSG("res is " << res << ", *rowCount is " << *rowCount);

  return res;
}

SQLRETURN SQLNumResultCols(SQLHSTMT stmt, SQLSMALLINT* columnNum) {
  using odbc::Statement;
  using odbc::meta::ColumnMetaVector;

  LOG_DEBUG_MSG("SQLNumResultCols called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  int32_t res = statement->GetColumnNumber();

  if (columnNum) {
    *columnNum = static_cast< SQLSMALLINT >(res);
    LOG_DEBUG_MSG("columnNum: " << *columnNum);
  }

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLTables(SQLHSTMT stmt, SQLWCHAR* catalogName,
                    SQLSMALLINT catalogNameLen, SQLWCHAR* schemaName,
                    SQLSMALLINT schemaNameLen, SQLWCHAR* tableName,
                    SQLSMALLINT tableNameLen, SQLWCHAR* tableType,
                    SQLSMALLINT tableTypeLen) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLTables called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  boost::optional< std::string > catalog =
      SqlWcharToOptString(catalogName, catalogNameLen);
  boost::optional< std::string > schema =
      SqlWcharToOptString(schemaName, schemaNameLen);
  boost::optional< std::string > table =
      SqlWcharToOptString(tableName, tableNameLen);
  boost::optional< std::string > tableTypeStr =
      SqlWcharToOptString(tableType, tableTypeLen);

  LOG_INFO_MSG("catalog: " << catalog << ", schema: " << schema << ", table: "
                           << table << ", tableType: " << tableTypeStr);

  statement->ExecuteGetTablesMetaQuery(catalog, schema, table, tableTypeStr);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLColumns(SQLHSTMT stmt, SQLWCHAR* catalogName,
                     SQLSMALLINT catalogNameLen, SQLWCHAR* schemaName,
                     SQLSMALLINT schemaNameLen, SQLWCHAR* tableName,
                     SQLSMALLINT tableNameLen, SQLWCHAR* columnName,
                     SQLSMALLINT columnNameLen) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLColumns called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  boost::optional< std::string > catalog = SqlWcharToOptString(catalogName, catalogNameLen);
  boost::optional< std::string > schema = SqlWcharToOptString(schemaName, schemaNameLen);
  boost::optional< std::string > table = SqlWcharToOptString(tableName, tableNameLen);
  boost::optional< std::string > column = SqlWcharToOptString(columnName, columnNameLen);


  LOG_INFO_MSG("catalog: " << catalog.get_value_or("")
                           << ", schema: " << schema.get_value_or("")
                           << ", table: " << table << ", column: " << column);

  if (catalog && catalog->empty() && schema && schema->empty()) {
    statement->AddStatusRecord(SqlState::S01000_GENERAL_WARNING,
      "catalogName and schemaName are empty strings.");
    return SQL_SUCCESS_WITH_INFO;
  }

  statement->ExecuteGetColumnsMetaQuery(catalog, schema, table, column);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLMoreResults(SQLHSTMT stmt) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLMoreResults called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->MoreResults();

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLBindParameter(SQLHSTMT stmt, SQLUSMALLINT paramIdx,
                           SQLSMALLINT ioType, SQLSMALLINT bufferType,
                           SQLSMALLINT paramSqlType, SQLULEN columnSize,
                           SQLSMALLINT decDigits, SQLPOINTER buffer,
                           SQLLEN bufferLen, SQLLEN* resLen) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLBindParameter called: " << paramIdx << ", " << bufferType
                                            << ", " << paramSqlType);

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->BindParameter(paramIdx, ioType, bufferType, paramSqlType,
                           columnSize, decDigits, buffer, bufferLen, resLen);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLNativeSql(SQLHDBC conn, SQLWCHAR* inQuery, SQLINTEGER inQueryLen,
                       SQLWCHAR* outQueryBuffer, SQLINTEGER outQueryBufferLen,
                       SQLINTEGER* outQueryLen) {
  using namespace odbc;

  LOG_DEBUG_MSG("SQLNativeSql called");

  Connection* connection = reinterpret_cast< Connection* >(conn);
  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return SQL_INVALID_HANDLE;
  }

  int64_t outQueryLenLocal = 0;
  connection->NativeSql(
      inQuery, static_cast< int64_t >(inQueryLen), outQueryBuffer,
      static_cast< int64_t >(outQueryBufferLen), &outQueryLenLocal);
  if (outQueryLen) {
    *outQueryLen = static_cast< SQLINTEGER >(outQueryLenLocal);
    LOG_DEBUG_MSG("*outQueryLen is " << *outQueryLen);
  }

  return connection->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLColAttribute(SQLHSTMT stmt, SQLUSMALLINT columnNum,
                          SQLUSMALLINT fieldId, SQLPOINTER strAttr,
                          SQLSMALLINT bufferLen, SQLSMALLINT* strAttrLen,
                          SQLLEN* numericAttr) {
  using odbc::Statement;
  using odbc::meta::ColumnMeta;
  using odbc::meta::ColumnMetaVector;

  LOG_DEBUG_MSG("SQLColAttribute called: "
                << fieldId << " (" << ColumnMeta::AttrIdToString(fieldId)
                << ")");

  // TODO adapt SQLColumns
  // https://bitquill.atlassian.net/browse/AT-1032

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  // This is a special case
  if (fieldId == SQL_DESC_COUNT) {
    SQLSMALLINT val = 0;

    SQLRETURN res = SQLNumResultCols(stmt, &val);

    if (numericAttr && res == SQL_SUCCESS)
      *numericAttr = val;

    return res;
  }

  statement->GetColumnAttribute(columnNum, fieldId,
                                reinterpret_cast< SQLWCHAR* >(strAttr),
                                bufferLen, strAttrLen, numericAttr);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLDescribeCol(SQLHSTMT stmt, SQLUSMALLINT columnNum,
                         SQLWCHAR* columnNameBuf, SQLSMALLINT columnNameBufLen,
                         SQLSMALLINT* columnNameLen, SQLSMALLINT* dataType,
                         SQLULEN* columnSize, SQLSMALLINT* decimalDigits,
                         SQLSMALLINT* nullable) {
  using odbc::SqlLen;
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLDescribeCol called with columnNum "
                << columnNum << ", columnNameBuf " << columnNameBuf
                << ", columnNameBufLen" << columnNameBufLen
                << ", columnNameLen " << columnNameLen << ", dataType "
                << dataType << ", columnSize " << columnSize
                << ", decimalDigits " << decimalDigits << ", nullable "
                << nullable);

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  std::vector< SQLRETURN > returnCodes;

  // Convert from length in characters to bytes.
  SQLSMALLINT columnNameLenInBytes = 0;
  statement->GetColumnAttribute(columnNum, SQL_DESC_NAME, columnNameBuf,
                                columnNameBufLen * sizeof(SQLWCHAR),
                                &columnNameLenInBytes, 0);
  // Save status of getting column name.
  returnCodes.push_back(statement->GetDiagnosticRecords().GetReturnCode());
  if (columnNameLen) {
    // Convert from length in bytes to characters.
    *columnNameLen = columnNameLenInBytes / sizeof(SQLWCHAR);
  }

  SqlLen dataTypeRes;
  SqlLen columnSizeRes;
  SqlLen decimalDigitsRes;
  SqlLen nullableRes;

  statement->GetColumnAttribute(columnNum, SQL_DESC_TYPE, 0, 0, 0,
                                &dataTypeRes);
  // Save status of getting column type.
  returnCodes.push_back(statement->GetDiagnosticRecords().GetReturnCode());
  statement->GetColumnAttribute(columnNum, SQL_DESC_PRECISION, 0, 0, 0,
                                &columnSizeRes);
  // Save status of getting column precision.
  returnCodes.push_back(statement->GetDiagnosticRecords().GetReturnCode());
  statement->GetColumnAttribute(columnNum, SQL_DESC_SCALE, 0, 0, 0,
                                &decimalDigitsRes);
  // Save status of getting column scale.
  returnCodes.push_back(statement->GetDiagnosticRecords().GetReturnCode());
  statement->GetColumnAttribute(columnNum, SQL_DESC_NULLABLE, 0, 0, 0,
                                &nullableRes);
  // Save status of getting column nullable.
  returnCodes.push_back(statement->GetDiagnosticRecords().GetReturnCode());

  LOG_INFO_MSG("columnNum: "
               << columnNum << ", dataTypeRes: " << dataTypeRes
               << ", columnSizeRes: " << columnSizeRes
               << ", decimalDigitsRes: " << decimalDigitsRes
               << ", nullableRes: " << nullableRes << ", columnNameBuf: "
               << (columnNameBuf
                       ? reinterpret_cast< const char* >(columnNameBuf)
                       : "<null>")
               << ", columnNameLen: " << (columnNameLen ? *columnNameLen : -1));

  if (dataType)
    *dataType = static_cast< SQLSMALLINT >(dataTypeRes);

  if (columnSize)
    *columnSize = static_cast< SQLULEN >(columnSizeRes);

  if (decimalDigits)
    *decimalDigits = static_cast< SQLSMALLINT >(decimalDigitsRes);

  if (nullable)
    *nullable = static_cast< SQLSMALLINT >(nullableRes);

  // Return error code, if any.
  for (auto returnCode : returnCodes) {
    if (!SQL_SUCCEEDED(returnCode)) {
      LOG_INFO_MSG("returnCode is " << returnCode);
      return returnCode;
    }
  }
  // Return success with info, if any.
  for (auto returnCode : returnCodes) {
    if (returnCode != SQL_SUCCESS) {
      LOG_DEBUG_MSG("returnCode is " << returnCode);
      return returnCode;
    }
  }

  return SQL_SUCCESS;
}

SQLRETURN SQLRowCount(SQLHSTMT stmt, SQLLEN* rowCnt) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLRowCount called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  int64_t res = statement->AffectedRows();

  LOG_DEBUG_MSG("Row count: " << res);

  if (rowCnt)
    *rowCnt = static_cast< SQLLEN >((res > 0 ? res : -1));

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLForeignKeys(
    SQLHSTMT stmt, SQLWCHAR* primaryCatalogName,
    SQLSMALLINT primaryCatalogNameLen, SQLWCHAR* primarySchemaName,
    SQLSMALLINT primarySchemaNameLen, SQLWCHAR* primaryTableName,
    SQLSMALLINT primaryTableNameLen, SQLWCHAR* foreignCatalogName,
    SQLSMALLINT foreignCatalogNameLen, SQLWCHAR* foreignSchemaName,
    SQLSMALLINT foreignSchemaNameLen, SQLWCHAR* foreignTableName,
    SQLSMALLINT foreignTableNameLen) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLForeignKeys called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG(
        "SQLForeignKeys exiting with SQL_INVALID_HANDLE because statement "
        "object is null");
    return SQL_INVALID_HANDLE;
  }

  std::string primaryCatalog =
      SqlWcharToString(primaryCatalogName, primaryCatalogNameLen);
  std::string primarySchema =
      SqlWcharToString(primarySchemaName, primarySchemaNameLen);
  std::string primaryTable =
      SqlWcharToString(primaryTableName, primaryTableNameLen);
  const boost::optional< std::string > foreignCatalog =
      SqlWcharToOptString(foreignCatalogName, foreignCatalogNameLen);
  const boost::optional< std::string > foreignSchema =
      SqlWcharToOptString(foreignSchemaName, foreignSchemaNameLen);
  std::string foreignTable =
      SqlWcharToString(foreignTableName, foreignTableNameLen);

  LOG_INFO_MSG("primaryCatalog: " << primaryCatalog);
  LOG_INFO_MSG("primarySchema: " << primarySchema);
  LOG_INFO_MSG("primaryTable: " << primaryTable);
  LOG_INFO_MSG("foreignCatalog: " << foreignCatalog.get_value_or(""));
  LOG_INFO_MSG("foreignSchema: " << foreignSchema.get_value_or(""));
  LOG_INFO_MSG("foreignTable: " << foreignTable);

  statement->ExecuteGetForeignKeysQuery(primaryCatalog, primarySchema,
                                        primaryTable, foreignCatalog,
                                        foreignSchema, foreignTable);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLGetStmtAttr(SQLHSTMT stmt, SQLINTEGER attr, SQLPOINTER valueBuf,
                         SQLINTEGER valueBufLen, SQLINTEGER* valueResLen) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLGetStmtAttr called");

#ifdef _DEBUG
  using odbc::type_traits::StatementAttrIdToString;

  LOG_DEBUG_MSG("Attr: " << StatementAttrIdToString(attr) << " (" << attr
                         << ")");
#endif  //_DEBUG

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->GetAttribute(attr, valueBuf, valueBufLen, valueResLen);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLSetStmtAttr(SQLHSTMT stmt, SQLINTEGER attr, SQLPOINTER value,
                         SQLINTEGER valueLen) {
  using odbc::Statement;
  LOG_DEBUG_MSG("SQLSetStmtAttr called: " << attr);

#ifdef _DEBUG
  LOG_DEBUG_MSG("Attr: " << odbc::type_traits::StatementAttrIdToString(attr)
                         << " (" << attr
                         << ")");
#endif  //_DEBUG

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->SetAttribute(attr, value, valueLen);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLPrimaryKeys(SQLHSTMT stmt, SQLWCHAR* catalogName,
                         SQLSMALLINT catalogNameLen, SQLWCHAR* schemaName,
                         SQLSMALLINT schemaNameLen, SQLWCHAR* tableName,
                         SQLSMALLINT tableNameLen) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLPrimaryKeys called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG(
        "SQLPrimaryKeys exiting with SQL_INVALID_HANDLE because statement "
        "object is null");
    return SQL_INVALID_HANDLE;
  }

  boost::optional< std::string > catalog =
      SqlWcharToOptString(catalogName, catalogNameLen);
  boost::optional< std::string > schema =
      SqlWcharToOptString(schemaName, schemaNameLen);
  boost::optional< std::string > table =
      SqlWcharToOptString(tableName, tableNameLen);

  LOG_INFO_MSG("catalog: " << catalog.get_value_or(""));
  LOG_INFO_MSG("schema: " << schema.get_value_or(""));
  LOG_INFO_MSG("table: " << table.get_value_or(""));

  statement->ExecuteGetPrimaryKeysQuery(catalog, schema, table);

  LOG_DEBUG_MSG("SQLPrimaryKeys exiting");

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLNumParams(SQLHSTMT stmt, SQLSMALLINT* paramCnt) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLNumParams called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG(
        "SQLNumParams exiting with SQL_INVALID_HANDLE because statement "
        "object is null");
    return SQL_INVALID_HANDLE;
  }

  if (paramCnt) {
    uint16_t paramNum = 0;
    statement->GetParametersNumber(paramNum);

    *paramCnt = static_cast< SQLSMALLINT >(paramNum);
  }

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLGetDiagField(SQLSMALLINT handleType, SQLHANDLE handle,
                          SQLSMALLINT recNum, SQLSMALLINT diagId,
                          SQLPOINTER buffer, SQLSMALLINT bufferLen,
                          SQLSMALLINT* resLen) {
  using namespace odbc;
  using namespace odbc::diagnostic;
  using namespace odbc::type_traits;

  using odbc::app::ApplicationDataBuffer;

  LOG_DEBUG_MSG("SQLGetDiagField called with handleType "
                << handleType << ", recNum " << recNum << ", diagId "
                << diagId);

  SqlLen outResLen;
  ApplicationDataBuffer outBuffer(OdbcNativeType::AI_DEFAULT, buffer, bufferLen,
                                  &outResLen);

  SqlResult::Type result;

  DiagnosticField::Type field = DiagnosticFieldToInternal(diagId);

  switch (handleType) {
    case SQL_HANDLE_ENV:
    case SQL_HANDLE_DBC:
    case SQL_HANDLE_STMT: {
      Diagnosable* diag = reinterpret_cast< Diagnosable* >(handle);

      result = diag->GetDiagnosticRecords().GetField(recNum, field, outBuffer);

      break;
    }

    default: {
      result = SqlResult::AI_NO_DATA;
      break;
    }
  }

  if (resLen && result == SqlResult::AI_SUCCESS)
    *resLen = static_cast< SQLSMALLINT >(outResLen);

  return SqlResultToReturnCode(result);
}

SQLRETURN SQLGetDiagRec(SQLSMALLINT handleType, SQLHANDLE handle,
                        SQLSMALLINT recNum, SQLWCHAR* sqlState,
                        SQLINTEGER* nativeError, SQLWCHAR* msgBuffer,
                        SQLSMALLINT msgBufferLen, SQLSMALLINT* msgLen) {
  using namespace odbc::utility;
  using namespace odbc;
  using namespace odbc::diagnostic;
  using namespace odbc::type_traits;

  using odbc::app::ApplicationDataBuffer;

  LOG_DEBUG_MSG("SQLGetDiagRec called with handleType "
                << handleType << ", handle " << handle << ", recNum " << recNum
                << ", sqlState " << sqlState << ", nativeError " << nativeError
                << ", msgBuffer " << msgBuffer << ", msgBufferLen "
                << msgBufferLen << ", msgLen " << msgLen);

  const DiagnosticRecordStorage* records = 0;
  bool isTruncated = false;

  switch (handleType) {
    case SQL_HANDLE_ENV:
    case SQL_HANDLE_DBC:
    case SQL_HANDLE_STMT: {
      Diagnosable* diag = reinterpret_cast< Diagnosable* >(handle);

      if (!diag) {
        LOG_ERROR_MSG("diag is null");
        return SQL_INVALID_HANDLE;
      }

      records = &diag->GetDiagnosticRecords();

      break;
    }

    default:
      LOG_ERROR_MSG(
          "SQLGetDiagRec exiting with SQL_INVALID_HANDLE on default case");
      return SQL_INVALID_HANDLE;
  }

  if (recNum < 1 || msgBufferLen < 0) {
    LOG_ERROR_MSG("SQLGetDiagRec exiting with SQL_ERROR. recNum: "
                  << recNum << ", msgBufferLen: " << msgBufferLen);
    return SQL_ERROR;
  }

  if (!records || recNum > records->GetStatusRecordsNumber()) {
    if (records) {
      LOG_ERROR_MSG("SQLGetDiagRec exiting with SQL_NO_DATA. recNum: "
                    << recNum << ", records: " << records
                    << ", records->GetStatusRecordsNumber(): "
                    << records->GetStatusRecordsNumber());
    } else {
      LOG_ERROR_MSG(
          "SQLGetDiagRec exiting with SQL_NO_DATA because records variable is "
          "null. recNum: "
          << recNum << ", records: " << records);
    }
    return SQL_NO_DATA;
  }

  const DiagnosticRecord& record = records->GetStatusRecord(recNum);

  if (sqlState)
    CopyStringToBuffer(record.GetSqlState(), sqlState, 6, isTruncated);

  if (nativeError)
    *nativeError = 0;

  const std::string& errMsg = record.GetMessageText();

  if (!msgBuffer
      || msgBufferLen < static_cast< SQLSMALLINT >(errMsg.size() + 1)) {
    if (!msgLen) {
      LOG_ERROR_MSG(
          "SQLGetDiagRec exiting with SQL_ERROR. msgLen must not be NULL.");
      return SQL_ERROR;
    }

    // Length is given in characters
    *msgLen = CopyStringToBuffer(
        errMsg, msgBuffer, static_cast< size_t >(msgBufferLen), isTruncated);

    return SQL_SUCCESS_WITH_INFO;
  }

  // Length is given in characters
  size_t msgLen0 = CopyStringToBuffer(
      errMsg, msgBuffer, static_cast< size_t >(msgBufferLen), isTruncated);

  if (msgLen)
    *msgLen = msgLen0;

  return SQL_SUCCESS;
}

SQLRETURN SQLGetTypeInfo(SQLHSTMT stmt, SQLSMALLINT type) {
  using odbc::Statement;

  LOG_DEBUG_MSG("SQLGetTypeInfo called: [type=" << type << ']');

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  statement->ExecuteGetTypeInfoQuery(static_cast< int16_t >(type));

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLEndTran(SQLSMALLINT handleType, SQLHANDLE handle,
                     SQLSMALLINT completionType) {
  using namespace odbc;

  LOG_DEBUG_MSG("SQLEndTran called with handleType " << handleType);

  SQLRETURN result;

  switch (handleType) {
    case SQL_HANDLE_ENV: {
      Environment* env = reinterpret_cast< Environment* >(handle);

      if (!env) {
        LOG_ERROR_MSG("env is nullptr");
        return SQL_INVALID_HANDLE;
      }

      if (completionType == SQL_COMMIT)
        env->TransactionCommit();
      else
        env->TransactionRollback();

      result = env->GetDiagnosticRecords().GetReturnCode();

      break;
    }

    case SQL_HANDLE_DBC: {
      Connection* conn = reinterpret_cast< Connection* >(handle);

      if (!conn) {
        LOG_ERROR_MSG(
            "SQLEndTran exiting with SQL_INVALID_HANDLE because conn "
            "object is null");
        LOG_DEBUG_MSG("handletype is SQL_HANDLE_DBC");
        return SQL_INVALID_HANDLE;
      }

      if (completionType == SQL_COMMIT)
        conn->TransactionCommit();
      else
        conn->TransactionRollback();

      result = conn->GetDiagnosticRecords().GetReturnCode();

      break;
    }

    default: {
      result = SQL_INVALID_HANDLE;

      break;
    }
  }

  LOG_DEBUG_MSG("SQLEndTran exiting");

  return result;
}

SQLRETURN SQLGetData(SQLHSTMT stmt, SQLUSMALLINT colNum, SQLSMALLINT targetType,
                     SQLPOINTER targetValue, SQLLEN bufferLength,
                     SQLLEN* strLengthOrIndicator) {
  using namespace odbc::type_traits;

  using odbc::Statement;
  using odbc::app::ApplicationDataBuffer;

  LOG_DEBUG_MSG("SQLGetData called with colNum " << colNum << ", targetType "
                                                 << targetType);

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG("statement is nullptr");
    return SQL_INVALID_HANDLE;
  }

  OdbcNativeType::Type driverType = ToDriverType(targetType);

  ApplicationDataBuffer dataBuffer(driverType, targetValue, bufferLength,
                                   strLengthOrIndicator);

  statement->GetColumnData(colNum, dataBuffer);

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLSetEnvAttr(SQLHENV env, SQLINTEGER attr, SQLPOINTER value,
                        SQLINTEGER valueLen) {
  using odbc::Environment;

  LOG_DEBUG_MSG("SQLSetEnvAttr called with Attribute " << attr << ", Value "
                                                       << (size_t)value);

  Environment* environment = reinterpret_cast< Environment* >(env);

  if (!environment) {
    LOG_ERROR_MSG("environment is nullptr");
    return SQL_INVALID_HANDLE;
  }

  environment->SetAttribute(attr, value, valueLen);

  return environment->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLGetEnvAttr(SQLHENV env, SQLINTEGER attr, SQLPOINTER valueBuf,
                        SQLINTEGER valueBufLen, SQLINTEGER* valueResLen) {
  using namespace odbc;
  using namespace type_traits;

  using app::ApplicationDataBuffer;

  LOG_DEBUG_MSG("SQLGetEnvAttr called with attr " << attr);

  Environment* environment = reinterpret_cast< Environment* >(env);

  if (!environment)
    return SQL_INVALID_HANDLE;

  SqlLen outResLen;
  ApplicationDataBuffer outBuffer(OdbcNativeType::AI_SIGNED_LONG, valueBuf,
                                  static_cast< int32_t >(valueBufLen),
                                  &outResLen);

  environment->GetAttribute(attr, outBuffer);

  if (valueResLen)
    *valueResLen = static_cast< SQLSMALLINT >(outResLen);

  return environment->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLSpecialColumns(SQLHSTMT stmt, SQLSMALLINT idType,
                            SQLWCHAR* catalogName, SQLSMALLINT catalogNameLen,
                            SQLWCHAR* schemaName, SQLSMALLINT schemaNameLen,
                            SQLWCHAR* tableName, SQLSMALLINT tableNameLen,
                            SQLSMALLINT scope, SQLSMALLINT nullable) {
  using namespace odbc;

  LOG_DEBUG_MSG("SQLSpecialColumns called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG(
        "SQLSpecialColumns exiting with SQL_INVALID_HANDLE because statement "
        "object is null");
    return SQL_INVALID_HANDLE;
  }

  std::string catalog = SqlWcharToString(catalogName, catalogNameLen);
  std::string schema = SqlWcharToString(schemaName, schemaNameLen);
  std::string table = SqlWcharToString(tableName, tableNameLen);

  LOG_INFO_MSG("catalog: " << catalog);
  LOG_INFO_MSG("schema: " << schema);
  LOG_INFO_MSG("table: " << table);

  statement->ExecuteSpecialColumnsQuery(idType, catalog, schema, table, scope,
                                        nullable);

  LOG_DEBUG_MSG("SQLSpecialColumns exiting");

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLParamData(SQLHSTMT stmt, SQLPOINTER* value) {
  using namespace ignite::odbc;

  LOG_DEBUG_MSG("SQLParamData called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG(
        "SQLParamData exiting with SQL_INVALID_HANDLE because statement "
        "object is null");
    return SQL_INVALID_HANDLE;
  }

  statement->SelectParam(value);

  LOG_DEBUG_MSG("SQLParamData exiting");

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLPutData(SQLHSTMT stmt, SQLPOINTER data,
                     SQLLEN strLengthOrIndicator) {
  using namespace ignite::odbc;

  LOG_DEBUG_MSG("SQLPutData called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG(
        "SQLPutData exiting with SQL_INVALID_HANDLE because statement "
        "object is null");
    return SQL_INVALID_HANDLE;
  }

  statement->PutData(data, strLengthOrIndicator);

  LOG_DEBUG_MSG("SQLPutData exiting");

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLDescribeParam(SQLHSTMT stmt, SQLUSMALLINT paramNum,
                           SQLSMALLINT* dataType, SQLULEN* paramSize,
                           SQLSMALLINT* decimalDigits, SQLSMALLINT* nullable) {
  using namespace ignite::odbc;

  LOG_DEBUG_MSG("SQLDescribeParam called");

  Statement* statement = reinterpret_cast< Statement* >(stmt);

  if (!statement) {
    LOG_ERROR_MSG(
        "SQLDescribeParam exiting with SQL_INVALID_HANDLE because statement "
        "object is null");
    return SQL_INVALID_HANDLE;
  }

  statement->DescribeParam(paramNum, dataType, paramSize, decimalDigits,
                           nullable);

  LOG_DEBUG_MSG("SQLDescribeParam exiting");

  return statement->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQLError(SQLHENV env, SQLHDBC conn, SQLHSTMT stmt, SQLWCHAR* state,
                   SQLINTEGER* error, SQLWCHAR* msgBuf, SQLSMALLINT msgBufLen,
                   SQLSMALLINT* msgResLen) {
  using namespace ignite::odbc::utility;
  using namespace ignite::odbc;
  using namespace ignite::odbc::diagnostic;
  using namespace ignite::odbc::type_traits;

  using ignite::odbc::app::ApplicationDataBuffer;

  LOG_DEBUG_MSG("SQLError is called with env "
                << env << ", conn " << conn << ", stmt " << stmt << ", state "
                << state << ", error " << error << ", msgBuf " << msgBuf
                << ", msgBufLen " << msgBufLen << " msgResLen " << msgResLen);

  SQLHANDLE handle = 0;

  if (env != 0)
    handle = static_cast< SQLHANDLE >(env);
  else if (conn != 0)
    handle = static_cast< SQLHANDLE >(conn);
  else if (stmt != 0)
    handle = static_cast< SQLHANDLE >(stmt);
  else {
    LOG_ERROR_MSG("SQLError exiting with SQL_INVALID_HANDLE");
    return SQL_INVALID_HANDLE;
  }

  Diagnosable* diag = reinterpret_cast< Diagnosable* >(handle);

  DiagnosticRecordStorage& records = diag->GetDiagnosticRecords();

  int32_t recNum = records.GetLastNonRetrieved();
  LOG_DEBUG_MSG("recNum is " << recNum);

  if (recNum < 1 || recNum > records.GetStatusRecordsNumber()) {
    LOG_ERROR_MSG("SQLError exiting with SQL_NO_DATA");
    return SQL_NO_DATA;
  }

  DiagnosticRecord& record = records.GetStatusRecord(recNum);

  record.MarkRetrieved();

  bool isTruncated = false;
  if (state)
    CopyStringToBuffer(record.GetSqlState(), state, 6, isTruncated);

  if (error)
    *error = 0;

  std::string errMsg = record.GetMessageText();
  // NOTE: msgBufLen is in characters.
  size_t outResLen = CopyStringToBuffer(
      errMsg, msgBuf, static_cast< size_t >(msgBufLen), isTruncated);

  if (msgResLen)
    *msgResLen = static_cast< SQLSMALLINT >(outResLen);

  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetConnectAttr(SQLHDBC conn, SQLINTEGER attr,
                                    SQLPOINTER valueBuf, SQLINTEGER valueBufLen,
                                    SQLINTEGER* valueResLen) {
  using namespace odbc;
  using namespace type_traits;

  using app::ApplicationDataBuffer;

  LOG_DEBUG_MSG("SQLGetConnectAttr called with attr " << attr);

  Connection* connection = reinterpret_cast< Connection* >(conn);

  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return SQL_INVALID_HANDLE;
  }

  connection->GetAttribute(attr, valueBuf, valueBufLen, valueResLen);

  return connection->GetDiagnosticRecords().GetReturnCode();
}

SQLRETURN SQL_API SQLSetConnectAttr(SQLHDBC conn, SQLINTEGER attr,
                                    SQLPOINTER value, SQLINTEGER valueLen) {
  using odbc::Connection;

  LOG_DEBUG_MSG("SQLSetConnectAttr called(" << attr << ", " << value << ")");

  Connection* connection = reinterpret_cast< Connection* >(conn);

  if (!connection) {
    LOG_ERROR_MSG("connection is nullptr");
    return SQL_INVALID_HANDLE;
  }

  connection->SetAttribute(attr, value, valueLen);

  return connection->GetDiagnosticRecords().GetReturnCode();
}

}  // namespace ignite
