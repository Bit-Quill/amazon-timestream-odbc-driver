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

#include <ignite/common/common.h>

#include "timestream/odbc.h"
#include "timestream/odbc/log.h"
#include "timestream/odbc/utility.h"

SQLRETURN SQL_API SQLGetInfo(SQLHDBC conn, SQLUSMALLINT infoType,
                             SQLPOINTER infoValue, SQLSMALLINT infoValueMax,
                             SQLSMALLINT* length) {
  return timestream::SQLGetInfo(conn, infoType, infoValue, infoValueMax, length);
}

SQLRETURN SQL_API SQLAllocHandle(SQLSMALLINT type, SQLHANDLE parent,
                                 SQLHANDLE* result) {
  return timestream::SQLAllocHandle(type, parent, result);
}

SQLRETURN SQL_API SQLAllocEnv(SQLHENV* env) {
  return timestream::SQLAllocEnv(env);
}

SQLRETURN SQL_API SQLAllocConnect(SQLHENV env, SQLHDBC* conn) {
  return timestream::SQLAllocConnect(env, conn);
}

SQLRETURN SQL_API SQLAllocStmt(SQLHDBC conn, SQLHSTMT* stmt) {
  return timestream::SQLAllocStmt(conn, stmt);
}

SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT type, SQLHANDLE handle) {
  return timestream::SQLFreeHandle(type, handle);
}

SQLRETURN SQL_API SQLFreeEnv(SQLHENV env) {
  return timestream::SQLFreeEnv(env);
}

SQLRETURN SQL_API SQLFreeConnect(SQLHDBC conn) {
  return timestream::SQLFreeConnect(conn);
}

SQLRETURN SQL_API SQLFreeStmt(SQLHSTMT stmt, SQLUSMALLINT option) {
  return timestream::SQLFreeStmt(stmt, option);
}

SQLRETURN SQL_API SQLCloseCursor(SQLHSTMT stmt) {
  return timestream::SQLCloseCursor(stmt);
}

SQLRETURN SQL_API SQLDriverConnect(SQLHDBC conn, SQLHWND windowHandle,
                                   SQLWCHAR* inConnectionString,
                                   SQLSMALLINT inConnectionStringLen,
                                   SQLWCHAR* outConnectionString,
                                   SQLSMALLINT outConnectionStringBufferLen,
                                   SQLSMALLINT* outConnectionStringLen,
                                   SQLUSMALLINT driverCompletion) {
  return timestream::SQLDriverConnect(conn, windowHandle, inConnectionString,
                                  inConnectionStringLen, outConnectionString,
                                  outConnectionStringBufferLen,
                                  outConnectionStringLen, driverCompletion);
}

SQLRETURN SQL_API SQLConnect(SQLHDBC conn, SQLWCHAR* serverName,
                             SQLSMALLINT serverNameLen, SQLWCHAR* userName,
                             SQLSMALLINT userNameLen, SQLWCHAR* auth,
                             SQLSMALLINT authLen) {
  return timestream::SQLConnect(conn, serverName, serverNameLen, userName,
                            userNameLen, auth, authLen);
}

SQLRETURN SQL_API SQLDisconnect(SQLHDBC conn) {
  return timestream::SQLDisconnect(conn);
}

SQLRETURN SQL_API SQLPrepare(SQLHSTMT stmt, SQLWCHAR* query,
                             SQLINTEGER queryLen) {
  return timestream::SQLPrepare(stmt, query, queryLen);
}

SQLRETURN SQL_API SQLExecute(SQLHSTMT stmt) {
  return timestream::SQLExecute(stmt);
}

SQLRETURN SQL_API SQLExecDirect(SQLHSTMT stmt, SQLWCHAR* query,
                                SQLINTEGER queryLen) {
  return timestream::SQLExecDirect(stmt, query, queryLen);
}

SQLRETURN SQL_API SQLBindCol(SQLHSTMT stmt, SQLUSMALLINT colNum,
                             SQLSMALLINT targetType, SQLPOINTER targetValue,
                             SQLLEN bufferLength,
                             SQLLEN* strLengthOrIndicator) {
  return timestream::SQLBindCol(stmt, colNum, targetType, targetValue, bufferLength,
                            strLengthOrIndicator);
}

SQLRETURN SQL_API SQLFetch(SQLHSTMT stmt) {
  return timestream::SQLFetch(stmt);
}

SQLRETURN SQL_API SQLFetchScroll(SQLHSTMT stmt, SQLSMALLINT orientation,
                                 SQLLEN offset) {
  return timestream::SQLFetchScroll(stmt, orientation, offset);
}

SQLRETURN SQL_API SQLExtendedFetch(SQLHSTMT stmt, SQLUSMALLINT orientation,
                                   SQLLEN offset, SQLULEN* rowCount,
                                   SQLUSMALLINT* rowStatusArray) {
  return timestream::SQLExtendedFetch(stmt, orientation, offset, rowCount,
                                  rowStatusArray);
}

SQLRETURN SQL_API SQLNumResultCols(SQLHSTMT stmt, SQLSMALLINT* columnNum) {
  return timestream::SQLNumResultCols(stmt, columnNum);
}

SQLRETURN SQL_API SQLTables(SQLHSTMT stmt, SQLWCHAR* catalogName,
                            SQLSMALLINT catalogNameLen, SQLWCHAR* schemaName,
                            SQLSMALLINT schemaNameLen, SQLWCHAR* tableName,
                            SQLSMALLINT tableNameLen, SQLWCHAR* tableType,
                            SQLSMALLINT tableTypeLen) {
  return timestream::SQLTables(stmt, catalogName, catalogNameLen, schemaName,
                           schemaNameLen, tableName, tableNameLen, tableType,
                           tableTypeLen);
}

SQLRETURN SQL_API SQLColumns(SQLHSTMT stmt, SQLWCHAR* catalogName,
                             SQLSMALLINT catalogNameLen, SQLWCHAR* schemaName,
                             SQLSMALLINT schemaNameLen, SQLWCHAR* tableName,
                             SQLSMALLINT tableNameLen, SQLWCHAR* columnName,
                             SQLSMALLINT columnNameLen) {
  return timestream::SQLColumns(stmt, catalogName, catalogNameLen, schemaName,
                            schemaNameLen, tableName, tableNameLen, columnName,
                            columnNameLen);
}

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT stmt) {
  return timestream::SQLMoreResults(stmt);
}

SQLRETURN SQL_API SQLBindParameter(SQLHSTMT stmt, SQLUSMALLINT paramIdx,
                                   SQLSMALLINT ioType, SQLSMALLINT bufferType,
                                   SQLSMALLINT paramSqlType, SQLULEN columnSize,
                                   SQLSMALLINT decDigits, SQLPOINTER buffer,
                                   SQLLEN bufferLen, SQLLEN* resLen) {
  return timestream::SQLBindParameter(stmt, paramIdx, ioType, bufferType,
                                  paramSqlType, columnSize, decDigits, buffer,
                                  bufferLen, resLen);
}

SQLRETURN SQL_API SQLNativeSql(SQLHDBC conn, SQLWCHAR* inQuery,
                               SQLINTEGER inQueryLen, SQLWCHAR* outQueryBuffer,
                               SQLINTEGER outQueryBufferLen,
                               SQLINTEGER* outQueryLen) {
  return timestream::SQLNativeSql(conn, inQuery, inQueryLen, outQueryBuffer,
                              outQueryBufferLen, outQueryLen);
}

#if defined _WIN64 || !defined _WIN32
SQLRETURN SQL_API SQLColAttribute(SQLHSTMT stmt, SQLUSMALLINT columnNum,
                                  SQLUSMALLINT fieldId, SQLPOINTER strAttr,
                                  SQLSMALLINT bufferLen,
                                  SQLSMALLINT* strAttrLen, SQLLEN* numericAttr)
#else
SQLRETURN SQL_API SQLColAttribute(SQLHSTMT stmt, SQLUSMALLINT columnNum,
                                  SQLUSMALLINT fieldId, SQLPOINTER strAttr,
                                  SQLSMALLINT bufferLen,
                                  SQLSMALLINT* strAttrLen,
                                  SQLPOINTER numericAttr)
#endif
{
  return timestream::SQLColAttribute(stmt, columnNum, fieldId, strAttr, bufferLen,
                                 strAttrLen, (SQLLEN*)numericAttr);
}

SQLRETURN SQL_API SQLDescribeCol(SQLHSTMT stmt, SQLUSMALLINT columnNum,
                                 SQLWCHAR* columnNameBuf,
                                 SQLSMALLINT columnNameBufLen,
                                 SQLSMALLINT* columnNameLen,
                                 SQLSMALLINT* dataType, SQLULEN* columnSize,
                                 SQLSMALLINT* decimalDigits,
                                 SQLSMALLINT* nullable) {
  return timestream::SQLDescribeCol(stmt, columnNum, columnNameBuf,
                                columnNameBufLen, columnNameLen, dataType,
                                columnSize, decimalDigits, nullable);
}

SQLRETURN SQL_API SQLRowCount(SQLHSTMT stmt, SQLLEN* rowCnt) {
  return timestream::SQLRowCount(stmt, rowCnt);
}

SQLRETURN SQL_API
SQLForeignKeys(SQLHSTMT stmt, SQLWCHAR* primaryCatalogName,
               SQLSMALLINT primaryCatalogNameLen, SQLWCHAR* primarySchemaName,
               SQLSMALLINT primarySchemaNameLen, SQLWCHAR* primaryTableName,
               SQLSMALLINT primaryTableNameLen, SQLWCHAR* foreignCatalogName,
               SQLSMALLINT foreignCatalogNameLen, SQLWCHAR* foreignSchemaName,
               SQLSMALLINT foreignSchemaNameLen, SQLWCHAR* foreignTableName,
               SQLSMALLINT foreignTableNameLen) {
  return timestream::SQLForeignKeys(
      stmt, primaryCatalogName, primaryCatalogNameLen, primarySchemaName,
      primarySchemaNameLen, primaryTableName, primaryTableNameLen,
      foreignCatalogName, foreignCatalogNameLen, foreignSchemaName,
      foreignSchemaNameLen, foreignTableName, foreignTableNameLen);
}

SQLRETURN SQL_API SQLGetStmtAttr(SQLHSTMT stmt, SQLINTEGER attr,
                                 SQLPOINTER valueBuf, SQLINTEGER valueBufLen,
                                 SQLINTEGER* valueResLen) {
  return timestream::SQLGetStmtAttr(stmt, attr, valueBuf, valueBufLen, valueResLen);
}

SQLRETURN SQL_API SQLSetStmtAttr(SQLHSTMT stmt, SQLINTEGER attr,
                                 SQLPOINTER value, SQLINTEGER valueLen) {
  return timestream::SQLSetStmtAttr(stmt, attr, value, valueLen);
}

SQLRETURN SQL_API SQLPrimaryKeys(SQLHSTMT stmt, SQLWCHAR* catalogName,
                                 SQLSMALLINT catalogNameLen,
                                 SQLWCHAR* schemaName,
                                 SQLSMALLINT schemaNameLen, SQLWCHAR* tableName,
                                 SQLSMALLINT tableNameLen) {
  return timestream::SQLPrimaryKeys(stmt, catalogName, catalogNameLen, schemaName,
                                schemaNameLen, tableName, tableNameLen);
}

SQLRETURN SQL_API SQLNumParams(SQLHSTMT stmt, SQLSMALLINT* paramCnt) {
  return timestream::SQLNumParams(stmt, paramCnt);
}

SQLRETURN SQL_API SQLGetDiagField(SQLSMALLINT handleType, SQLHANDLE handle,
                                  SQLSMALLINT recNum, SQLSMALLINT diagId,
                                  SQLPOINTER buffer, SQLSMALLINT bufferLen,
                                  SQLSMALLINT* resLen) {
  return timestream::SQLGetDiagField(handleType, handle, recNum, diagId, buffer,
                                 bufferLen, resLen);
}

SQLRETURN SQL_API SQLGetDiagRec(SQLSMALLINT handleType, SQLHANDLE handle,
                                SQLSMALLINT recNum, SQLWCHAR* sqlState,
                                SQLINTEGER* nativeError, SQLWCHAR* msgBuffer,
                                SQLSMALLINT msgBufferLen, SQLSMALLINT* msgLen) {
  return timestream::SQLGetDiagRec(handleType, handle, recNum, sqlState,
                               nativeError, msgBuffer, msgBufferLen, msgLen);
}

SQLRETURN SQL_API SQLGetTypeInfo(SQLHSTMT stmt, SQLSMALLINT type) {
  return timestream::SQLGetTypeInfo(stmt, type);
}

SQLRETURN SQL_API SQLEndTran(SQLSMALLINT handleType, SQLHANDLE handle,
                             SQLSMALLINT completionType) {
  return timestream::SQLEndTran(handleType, handle, completionType);
}

SQLRETURN SQL_API SQLGetData(SQLHSTMT stmt, SQLUSMALLINT colNum,
                             SQLSMALLINT targetType, SQLPOINTER targetValue,
                             SQLLEN bufferLength,
                             SQLLEN* strLengthOrIndicator) {
  return timestream::SQLGetData(stmt, colNum, targetType, targetValue, bufferLength,
                            strLengthOrIndicator);
}

SQLRETURN SQL_API SQLSetEnvAttr(SQLHENV env, SQLINTEGER attr, SQLPOINTER value,
                                SQLINTEGER valueLen) {
  return timestream::SQLSetEnvAttr(env, attr, value, valueLen);
}

SQLRETURN SQL_API SQLGetEnvAttr(SQLHENV env, SQLINTEGER attr,
                                SQLPOINTER valueBuf, SQLINTEGER valueBufLen,
                                SQLINTEGER* valueResLen) {
  return timestream::SQLGetEnvAttr(env, attr, valueBuf, valueBufLen, valueResLen);
}

SQLRETURN SQL_API SQLSpecialColumns(
    SQLHSTMT stmt, SQLUSMALLINT idType, SQLWCHAR* catalogName,
    SQLSMALLINT catalogNameLen, SQLWCHAR* schemaName, SQLSMALLINT schemaNameLen,
    SQLWCHAR* tableName, SQLSMALLINT tableNameLen, SQLUSMALLINT scope,
    SQLUSMALLINT nullable) {
  return timestream::SQLSpecialColumns(stmt, idType, catalogName, catalogNameLen,
                                   schemaName, schemaNameLen, tableName,
                                   tableNameLen, scope, nullable);
}

SQLRETURN SQL_API SQLParamData(SQLHSTMT stmt, SQLPOINTER* value) {
  return timestream::SQLParamData(stmt, value);
}

SQLRETURN SQL_API SQLPutData(SQLHSTMT stmt, SQLPOINTER data,
                             SQLLEN strLengthOrIndicator) {
  return timestream::SQLPutData(stmt, data, strLengthOrIndicator);
}

SQLRETURN SQL_API SQLDescribeParam(SQLHSTMT stmt, SQLUSMALLINT paramNum,
                                   SQLSMALLINT* dataType, SQLULEN* paramSize,
                                   SQLSMALLINT* decimalDigits,
                                   SQLSMALLINT* nullable) {
  return timestream::SQLDescribeParam(stmt, paramNum, dataType, paramSize,
                                  decimalDigits, nullable);
}

SQLRETURN SQL_API SQLError(SQLHENV env, SQLHDBC conn, SQLHSTMT stmt,
                           SQLWCHAR* state, SQLINTEGER* error, SQLWCHAR* msgBuf,
                           SQLSMALLINT msgBufLen, SQLSMALLINT* msgResLen) {
  return timestream::SQLError(env, conn, stmt, state, error, msgBuf, msgBufLen,
                          msgResLen);
}

SQLRETURN SQL_API SQLGetConnectAttr(SQLHDBC conn, SQLINTEGER attr,
                                    SQLPOINTER valueBuf, SQLINTEGER valueBufLen,
                                    SQLINTEGER* valueResLen) {
  return timestream::SQLGetConnectAttr(conn, attr, valueBuf, valueBufLen,
                                   valueResLen);
}

SQLRETURN SQL_API SQLSetConnectAttr(SQLHDBC conn, SQLINTEGER attr,
                                    SQLPOINTER value, SQLINTEGER valueLen) {
  return timestream::SQLSetConnectAttr(conn, attr, value, valueLen);
}

//
// ==== Not implemented ====
//

SQLRETURN SQL_API SQLCancel(SQLHSTMT stmt) {
  IGNITE_UNUSED(stmt);

  LOG_MSG("SQLCancel called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLColAttributes(SQLHSTMT stmt, SQLUSMALLINT colNum,
                                   SQLUSMALLINT fieldId, SQLPOINTER strAttrBuf,
                                   SQLSMALLINT strAttrBufLen,
                                   SQLSMALLINT* strAttrResLen,
                                   SQLLEN* numAttrBuf) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(colNum);
  IGNITE_UNUSED(fieldId);
  IGNITE_UNUSED(strAttrBuf);
  IGNITE_UNUSED(strAttrBufLen);
  IGNITE_UNUSED(strAttrResLen);
  IGNITE_UNUSED(numAttrBuf);

  LOG_MSG("SQLColAttributes called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetCursorName(SQLHSTMT stmt, SQLWCHAR* nameBuf,
                                   SQLSMALLINT nameBufLen,
                                   SQLSMALLINT* nameResLen) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(nameBuf);
  IGNITE_UNUSED(nameBufLen);
  IGNITE_UNUSED(nameResLen);

  LOG_MSG("SQLGetCursorName called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetCursorName(SQLHSTMT stmt, SQLWCHAR* name,
                                   SQLSMALLINT nameLen) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(name);
  IGNITE_UNUSED(nameLen);

  LOG_MSG("SQLSetCursorName called");
  return SQL_SUCCESS;
}

#if defined(__APPLE__) // TODO remove this #if line in AT-1224
// SQLSetConnectOption is called by Excel 16 on macOS to change the login timeout,
// but Timestream does not support login timeout. Therefore, the function is 
// defined but not implemented. 
// TODO [AT-1224] implement SQLSetConnectOption
// https://bitquill.atlassian.net/browse/AT-1224
SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC conn, SQLUSMALLINT option,
                                      SQLULEN value) {
  IGNITE_UNUSED(conn);
  IGNITE_UNUSED(option);
  IGNITE_UNUSED(value);

  LOG_INFO_MSG("SQLSetConnectOption called");
  return SQL_SUCCESS;
}
#endif //defined(__APPLE__)

SQLRETURN SQL_API SQLGetStmtOption(SQLHSTMT stmt, SQLUSMALLINT option,
                                   SQLPOINTER value) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(option);
  IGNITE_UNUSED(value);

  LOG_MSG("SQLGetStmtOption called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetStmtOption(SQLHSTMT stmt, SQLUSMALLINT option,
                                   SQLULEN value) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(option);
  IGNITE_UNUSED(value);

  LOG_MSG("SQLSetStmtOption called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLStatistics(SQLHSTMT stmt, SQLWCHAR* catalogName,
                                SQLSMALLINT catalogNameLen,
                                SQLWCHAR* schemaName, SQLSMALLINT schemaNameLen,
                                SQLWCHAR* tableName, SQLSMALLINT tableNameLen,
                                SQLUSMALLINT unique, SQLUSMALLINT reserved) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(catalogName);
  IGNITE_UNUSED(catalogNameLen);
  IGNITE_UNUSED(schemaName);
  IGNITE_UNUSED(schemaNameLen);
  IGNITE_UNUSED(tableName);
  IGNITE_UNUSED(tableNameLen);
  IGNITE_UNUSED(unique);
  IGNITE_UNUSED(reserved);

  LOG_MSG("SQLStatistics called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLBrowseConnect(SQLHDBC conn, SQLWCHAR* inConnectionStr,
                                   SQLSMALLINT inConnectionStrLen,
                                   SQLWCHAR* outConnectionStrBuf,
                                   SQLSMALLINT outConnectionStrBufLen,
                                   SQLSMALLINT* outConnectionStrResLen) {
  IGNITE_UNUSED(conn);
  IGNITE_UNUSED(inConnectionStr);
  IGNITE_UNUSED(inConnectionStrLen);
  IGNITE_UNUSED(outConnectionStrBuf);
  IGNITE_UNUSED(outConnectionStrBufLen);
  IGNITE_UNUSED(outConnectionStrResLen);

  LOG_MSG("SQLBrowseConnect called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLProcedureColumns(
    SQLHSTMT stmt, SQLWCHAR* catalogName, SQLSMALLINT catalogNameLen,
    SQLWCHAR* schemaName, SQLSMALLINT schemaNameLen, SQLWCHAR* procName,
    SQLSMALLINT procNameLen, SQLWCHAR* columnName, SQLSMALLINT columnNameLen) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(catalogName);
  IGNITE_UNUSED(catalogNameLen);
  IGNITE_UNUSED(schemaName);
  IGNITE_UNUSED(schemaNameLen);
  IGNITE_UNUSED(procName);
  IGNITE_UNUSED(procNameLen);
  IGNITE_UNUSED(columnName);
  IGNITE_UNUSED(columnNameLen);

  LOG_MSG("SQLProcedureColumns called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetPos(SQLHSTMT stmt, SQLSETPOSIROW rowNum,
                            SQLUSMALLINT operation, SQLUSMALLINT lockType) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(rowNum);
  IGNITE_UNUSED(operation);
  IGNITE_UNUSED(lockType);

  LOG_MSG("SQLSetPos called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetScrollOptions(SQLHSTMT stmt, SQLUSMALLINT concurrency,
                                      SQLLEN crowKeyset,
                                      SQLUSMALLINT crowRowset) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(concurrency);
  IGNITE_UNUSED(crowKeyset);
  IGNITE_UNUSED(crowRowset);

  LOG_MSG("SQLSetScrollOptions called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT stmt, SQLUSMALLINT operation) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(operation);

  LOG_MSG("SQLBulkOperations called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLTablePrivileges(SQLHSTMT stmt, SQLWCHAR* catalogName,
                                     SQLSMALLINT catalogNameLen,
                                     SQLWCHAR* schemaName,
                                     SQLSMALLINT schemaNameLen,
                                     SQLWCHAR* tableName,
                                     SQLSMALLINT tableNameLen) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(catalogName);
  IGNITE_UNUSED(catalogNameLen);
  IGNITE_UNUSED(schemaName);
  IGNITE_UNUSED(schemaNameLen);
  IGNITE_UNUSED(tableName);
  IGNITE_UNUSED(tableNameLen);

  LOG_MSG("SQLTablePrivileges called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLCopyDesc(SQLHDESC src, SQLHDESC dst) {
  IGNITE_UNUSED(src);
  IGNITE_UNUSED(dst);

  LOG_MSG("SQLCopyDesc called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetDescField(SQLHDESC descr, SQLSMALLINT recNum,
                                  SQLSMALLINT fieldId, SQLPOINTER buffer,
                                  SQLINTEGER bufferLen, SQLINTEGER* resLen) {
  IGNITE_UNUSED(descr);
  IGNITE_UNUSED(recNum);
  IGNITE_UNUSED(fieldId);
  IGNITE_UNUSED(buffer);
  IGNITE_UNUSED(bufferLen);
  IGNITE_UNUSED(resLen);

  LOG_MSG("SQLGetDescField called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLGetDescRec(SQLHDESC DescriptorHandle,
                                SQLSMALLINT RecNumber, SQLWCHAR* nameBuffer,
                                SQLSMALLINT nameBufferLen, SQLSMALLINT* strLen,
                                SQLSMALLINT* type, SQLSMALLINT* subType,
                                SQLLEN* len, SQLSMALLINT* precision,
                                SQLSMALLINT* scale, SQLSMALLINT* nullable) {
  IGNITE_UNUSED(DescriptorHandle);
  IGNITE_UNUSED(RecNumber);
  IGNITE_UNUSED(nameBuffer);
  IGNITE_UNUSED(nameBufferLen);
  IGNITE_UNUSED(strLen);
  IGNITE_UNUSED(type);
  IGNITE_UNUSED(subType);
  IGNITE_UNUSED(len);
  IGNITE_UNUSED(precision);
  IGNITE_UNUSED(scale);
  IGNITE_UNUSED(nullable);

  LOG_MSG("SQLGetDescRec called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetDescField(SQLHDESC descr, SQLSMALLINT recNum,
                                  SQLSMALLINT fieldId, SQLPOINTER buffer,
                                  SQLINTEGER bufferLen) {
  IGNITE_UNUSED(descr);
  IGNITE_UNUSED(recNum);
  IGNITE_UNUSED(fieldId);
  IGNITE_UNUSED(buffer);
  IGNITE_UNUSED(bufferLen);

  LOG_MSG("SQLSetDescField called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetDescRec(SQLHDESC descr, SQLSMALLINT recNum,
                                SQLSMALLINT type, SQLSMALLINT subType,
                                SQLLEN len, SQLSMALLINT precision,
                                SQLSMALLINT scale, SQLPOINTER buffer,
                                SQLLEN* resLen, SQLLEN* id) {
  IGNITE_UNUSED(descr);
  IGNITE_UNUSED(recNum);
  IGNITE_UNUSED(type);
  IGNITE_UNUSED(subType);
  IGNITE_UNUSED(len);
  IGNITE_UNUSED(precision);
  IGNITE_UNUSED(scale);
  IGNITE_UNUSED(buffer);
  IGNITE_UNUSED(resLen);
  IGNITE_UNUSED(id);

  LOG_MSG("SQLSetDescRec called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLColumnPrivileges(
    SQLHSTMT stmt, SQLWCHAR* catalogName, SQLSMALLINT catalogNameLen,
    SQLWCHAR* schemaName, SQLSMALLINT schemaNameLen, SQLWCHAR* tableName,
    SQLSMALLINT tableNameLen, SQLWCHAR* columnName, SQLSMALLINT columnNameLen) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(catalogName);
  IGNITE_UNUSED(catalogNameLen);
  IGNITE_UNUSED(schemaName);
  IGNITE_UNUSED(schemaNameLen);
  IGNITE_UNUSED(tableName);
  IGNITE_UNUSED(tableNameLen);
  IGNITE_UNUSED(columnName);
  IGNITE_UNUSED(columnNameLen);

  LOG_MSG("SQLColumnPrivileges called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLParamOptions(SQLHSTMT stmt, SQLULEN paramSetSize,
                                  SQLULEN* paramsProcessed) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(paramSetSize);
  IGNITE_UNUSED(paramsProcessed);

  LOG_MSG("SQLParamOptions called");
  return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLProcedures(SQLHSTMT stmt, SQLWCHAR* catalogName,
                                SQLSMALLINT catalogNameLen,
                                SQLWCHAR* schemaName, SQLSMALLINT schemaNameLen,
                                SQLWCHAR* tableName, SQLSMALLINT tableNameLen) {
  IGNITE_UNUSED(stmt);
  IGNITE_UNUSED(catalogName);
  IGNITE_UNUSED(catalogNameLen);
  IGNITE_UNUSED(schemaName);
  IGNITE_UNUSED(schemaNameLen);
  IGNITE_UNUSED(tableName);
  IGNITE_UNUSED(tableNameLen);

  LOG_MSG("SQLProcedures called");
  return SQL_SUCCESS;
}
