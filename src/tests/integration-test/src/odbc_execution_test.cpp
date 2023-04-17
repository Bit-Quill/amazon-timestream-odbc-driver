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
 * Modifications Copyright Amazon.com, Inc. or its affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef _WIN32
#include <windows.h>
#endif

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <string>
#include <vector>

#include "odbc_test_suite.h"
#include "test_type.h"
#include "test_utils.h"

using namespace timestream;
using namespace timestream_test;

using namespace boost::unit_test;

/**
 * Test setup fixture.
 */
struct ODBCExecutionTestSuiteFixture : public odbc::OdbcTestSuite {
  /**
   * Constructor.
   */
  ODBCExecutionTestSuiteFixture() {
    // No-op
  }

  /**
   * Destructor.
   */
  virtual ~ODBCExecutionTestSuiteFixture() {
    // No-op.
  }
};

BOOST_FIXTURE_TEST_SUITE(ODBCExecutionTestSuite, ODBCExecutionTestSuiteFixture)

// Test unsupported functions
BOOST_AUTO_TEST_CASE(TestSQLEndTran) {
  ConnectToTS();

  SQLRETURN ret =
      SQLSetConnectAttr(dbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  ret = SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_COMMIT);
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLConnectionDiagnosticError("HYC00");
  BOOST_REQUIRE_EQUAL("HYC00: SQLEndTran is not supported.",
                      GetOdbcErrorMessage(SQL_HANDLE_DBC, dbc));
}

BOOST_AUTO_TEST_CASE(TestSQLBrowseConnect) {
  Prepare();

  SQLWCHAR InConnectionString[ODBC_BUFFER_SIZE];
  SQLWCHAR OutConnectionString[ODBC_BUFFER_SIZE];
  SQLSMALLINT reslen;

  SQLRETURN ret =
      SQLBrowseConnect(dbc, InConnectionString, 0, InConnectionString, 0, &reslen);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLConnectionDiagnosticError("IM002");
}

#if (ODBCVER >= 0x0380)
BOOST_AUTO_TEST_CASE(TestSQLCancelHandle) {
  ConnectToTS();

  SQLRETURN ret = SQLCancelHandle(SQL_HANDLE_STMT, stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLStatementDiagnosticError("HY010");
  BOOST_REQUIRE_EQUAL("HY010: Query does not exist.",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}
#endif

BOOST_AUTO_TEST_CASE(TestSQLTransact) {
  // SQLTransact is deprecated function and will be mapped to SQLEndTran 
  // by driver manager.
  ConnectToTS();

  SQLRETURN ret =
      SQLSetConnectAttr(dbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  
  ret = SQLTransact(env, dbc, SQL_COMMIT);
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLConnectionDiagnosticError("HYC00");
  BOOST_REQUIRE_EQUAL("HYC00: SQLEndTran is not supported.",
                      GetOdbcErrorMessage(SQL_HANDLE_DBC, dbc));
}

BOOST_AUTO_TEST_CASE(TestSQLDescribeParam) {
  ConnectToTS();

  SQLSMALLINT sqlType;
  SQLULEN paramNum;
  SQLSMALLINT scale;
  SQLSMALLINT nullable;
  std::vector< SQLWCHAR > request = MakeSqlBuffer("SELECT 1");

  SQLRETURN ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));


  ret = SQLDescribeParam(stmt, 1, &sqlType, &paramNum, &scale, &nullable);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLStatementDiagnosticError("HYC00");
  BOOST_REQUIRE_EQUAL("HYC00: SQLDescribeParam is not supported.",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

BOOST_AUTO_TEST_CASE(TestSQLParamData) {
  ConnectToTS();

  void* val;
  std::vector< SQLWCHAR > request = MakeSqlBuffer("SELECT 1");

  SQLRETURN ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLParamData(stmt, &val);
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);

  // "Function sequence error" from the driver manager (DM) is returned.
  // This is because SQL_NEED_DATA (unsupported state) is not returned by
  // SQLExecDirect prior to calling SQLParamData.
  #ifdef __linux__
    BOOST_REQUIRE_EQUAL(
        "HY010: [unixODBC][Driver Manager]Function sequence error",
        GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  #elif defined(__APPLE__)
    BOOST_REQUIRE_EQUAL("S1010: [iODBC][Driver Manager]Function sequence error",
                        GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  #else
    BOOST_REQUIRE_EQUAL(
        "HY010: [Microsoft][ODBC Driver Manager] Function sequence error",
        GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  #endif
}

BOOST_AUTO_TEST_CASE(TestSQLNumParams) {
  ConnectToTS();

  SQLSMALLINT num = 0;
  std::vector< SQLWCHAR > request = MakeSqlBuffer("SELECT 1");

  SQLRETURN ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  
  ret = SQLNumParams(stmt, &num);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLStatementDiagnosticError("HYC00");
  BOOST_REQUIRE_EQUAL("HYC00: SQLNumParams is not supported.",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

BOOST_AUTO_TEST_CASE(TestSQLPutData) {
  ConnectToTS();

  int value = 1;

  SQLRETURN ret = SQLPutData(stmt, &value, 0);
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);

  // "Function sequence error" from the driver manager (DM) is returned.
  // This is because the previous function call must be a successful call to
  // SQLParamData(), and SQL_NEED_DATA (unsupported state) needs to be returned.
  #ifdef __linux__
   BOOST_REQUIRE_EQUAL("HY010: [unixODBC][Driver Manager]Function sequence error",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  #elif defined(__APPLE__)
   BOOST_REQUIRE_EQUAL("S1010: [iODBC][Driver Manager]Function sequence error",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  #else
  BOOST_REQUIRE_EQUAL("HY010: [Microsoft][ODBC Driver Manager] Function sequence error",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  #endif
}

BOOST_AUTO_TEST_CASE(TestSQLBindParameter) {
  ConnectToTS();

  SQLINTEGER int1;
  SQLLEN len1 = SQL_DATA_AT_EXEC;

  SQLRETURN ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,
                                   SQL_INTEGER, 0, 0, &int1, 0, &len1);

  BOOST_REQUIRE(!SQL_SUCCEEDED(ret));

  #ifdef __APPLE__
  // SQLBindParameter is not a supported function. 
  // On GitHub runner, iODBC returns SQL_INVALID_HANDLE when SQLBindParameter is
  // called after a connection is made. This behavior is outside of driver's
  // control.

  // TODO [AT-1354] Check if making SQLGetFunctions return TRUE for SQLBindParameter will resolve
  // the issue of SQL_INVALID_HANDLE being returned
  // https://bitquill.atlassian.net/browse/AT-1354
  if (ret != SQL_ERROR)
    BOOST_REQUIRE_EQUAL(ret, SQL_INVALID_HANDLE);
  else {
    // On Ventura (macOS 13), iODBC calls SQLBindParameter normally and returns SQL_ERROR
    // as expected.
    CheckSQLStatementDiagnosticError("HYC00");
    BOOST_REQUIRE_EQUAL("HYC00: SQLBindParameter is not supported.",
                        GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }
  #else
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLStatementDiagnosticError("HYC00");
  BOOST_REQUIRE_EQUAL("HYC00: SQLBindParameter is not supported.",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  #endif //__APPLE__
}

BOOST_AUTO_TEST_CASE(TestSQLSetParam) {
  // SQLSetParam is deprecated function and will be mapped to SQLBindParameter by driver manager.
  ConnectToTS();

  SQLINTEGER int1;
  SQLLEN len = 0;

  SQLRETURN ret =
      SQLSetParam(stmt, 1, SQL_PARAM_INPUT, SQL_INTEGER, len, 100, &int1, &len);
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
}

// TODO [AT-1344] Check error message from SQLBulkOperations is "HYC00: SQLBulkOperations is not supported."
// https://bitquill.atlassian.net/browse/AT-1344
BOOST_AUTO_TEST_CASE(TestSQLBulkOperations) {
  ConnectToTS();

  SQLRETURN ret = SQLBulkOperations(stmt, SQL_ADD);

  BOOST_REQUIRE(!SQL_SUCCEEDED(ret));

  #ifdef __APPLE__
  // SQLBulkOperations is not a supported function, this is indicated in SQLGetInfo.
  // iODBC returns SQL_INVALID_HANDLE when SQLBulkOperations is called after a connection is made. 
  // This behavior is outside of driver's control.
  BOOST_REQUIRE_EQUAL(ret, SQL_INVALID_HANDLE);
  #else
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  #endif // __APPLE__
}

BOOST_AUTO_TEST_CASE(TestSQLSetPos) {
  ConnectToTS();

  std::vector< SQLWCHAR > request =
      MakeSqlBuffer("SELECT 1");

  SQLRETURN ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetchScroll(stmt, SQL_FETCH_NEXT, 0);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLSetPos(stmt, 0, SQL_POSITION, SQL_LOCK_NO_CHANGE);
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLStatementDiagnosticError("HYC00");
  BOOST_REQUIRE_EQUAL(
      "HYC00: SQLSetPos is not supported.",
      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

// TODO enable descriptor tests as part of AT-1217
// after SQL_ATTR_APP_ROW_DESC is supported
// https://bitquill.atlassian.net/browse/AT-1217
BOOST_AUTO_TEST_CASE(TestSQLSetDescRec, *disabled()) {
  SQLHDESC desc;
  SQLINTEGER data = 10;
  SQLRETURN ret =
      SQLGetStmtAttr(stmt, SQL_ATTR_APP_ROW_DESC, &desc, 0, NULL);
    
  ret = SQLSetDescRec(NULL, 2, SQL_INTEGER, 0, 0, 0, 0, (SQLPOINTER)&data,
                      NULL, NULL);
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLStatementDiagnosticError("HYC00");
  BOOST_REQUIRE_EQUAL("HYC00: SQLSetDescRec is not supported.",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

BOOST_AUTO_TEST_CASE(TestSQLGetDescRec, *disabled()) {
  SQLHDESC desc;
  SQLINTEGER data = 10;
  SQLRETURN ret = SQLGetStmtAttr(stmt, SQL_ATTR_APP_ROW_DESC, &desc, 0, NULL);

  std::vector< SQLWCHAR > column = MakeSqlBuffer("Region");
  ret = SQLGetDescRec(desc, 1, column.data(), SQL_NTS, NULL, NULL, NULL, NULL,
                      NULL, NULL, NULL);
  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  CheckSQLStatementDiagnosticError("HYC00");
  BOOST_REQUIRE_EQUAL("HYC00: SQLGetDescRec is not supported.",
                      GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

BOOST_AUTO_TEST_SUITE_END()
