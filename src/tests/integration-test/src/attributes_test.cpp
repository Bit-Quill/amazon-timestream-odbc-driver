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

#ifdef _WIN32
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#include "timestream/odbc/connection.h"
#include "odbc_test_suite.h"
#include "test_type.h"
#include "test_utils.h"

using namespace timestream;
using namespace timestream_test;

using namespace boost::unit_test;


/**
 * Test setup fixture.
 */
struct AttributesTestSuiteFixture : odbc::OdbcTestSuite {
  /**
   * Constructor.
   */
  AttributesTestSuiteFixture() {
    // No-op
  }

  /**
   * Destructor.
   */
  ~AttributesTestSuiteFixture() override = default;
};

BOOST_FIXTURE_TEST_SUITE(AttributesTestSuite, AttributesTestSuiteFixture)

BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionDeadGet) {
  ConnectToTS();

  SQLUINTEGER dead;
  SQLRETURN ret;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_CONNECTION_DEAD, &dead, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  BOOST_REQUIRE_EQUAL(dead, SQL_CD_FALSE);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionDeadSet) {
  ConnectToTS();

  SQLUINTEGER dead = SQL_CD_TRUE;
  SQLRETURN ret;

  ret = SQLSetConnectAttr(dbc, SQL_ATTR_CONNECTION_DEAD, &dead, 0);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);

  // The error code (HY092) is determined according to
  // https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqlsetconnectattr-function#diagnostics
  CheckSQLConnectionDiagnosticError("HY092");
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionTimeoutGet) {
  ConnectToTS();

  SQLUINTEGER timeout;
  SQLRETURN ret;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_CONNECTION_TIMEOUT, &timeout, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  BOOST_REQUIRE_EQUAL(timeout, 0);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionTimeoutSet) {
  ConnectToTS();

  SQLRETURN ret;

  ret = SQLSetConnectAttr(dbc, SQL_ATTR_CONNECTION_TIMEOUT,
                          reinterpret_cast< SQLPOINTER >(10), 0);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  std::string error = GetOdbcErrorMessage(SQL_HANDLE_DBC, dbc);
  std::string pattern = "Specified attribute is not supported.";
  if (error.find(pattern) == std::string::npos)
      BOOST_FAIL("'" + error + "' does not match '" + pattern + "'");
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeAutoCommit) {
  ConnectToTS();

  SQLUINTEGER autoCommit;
  SQLRETURN ret;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_AUTOCOMMIT, &autoCommit, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  BOOST_REQUIRE_EQUAL(autoCommit, SQL_AUTOCOMMIT_ON);

  ret = SQLSetConnectAttr(dbc, SQL_ATTR_AUTOCOMMIT,
                          reinterpret_cast< SQLPOINTER >(SQL_AUTOCOMMIT_OFF), 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_AUTOCOMMIT, &autoCommit, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  BOOST_REQUIRE_EQUAL(autoCommit, SQL_AUTOCOMMIT_OFF);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeMetadataId) {
  ConnectToTS();

  SQLUINTEGER id = -1;
  SQLRETURN ret = SQLGetConnectAttr(dbc, SQL_ATTR_METADATA_ID, &id, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  BOOST_REQUIRE_EQUAL(id, SQL_FALSE);

  ret = SQLSetConnectAttr(dbc, SQL_ATTR_METADATA_ID,
                       reinterpret_cast< SQLPOINTER >(SQL_TRUE), 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  id = -1;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_METADATA_ID, &id, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  BOOST_REQUIRE_EQUAL(id, SQL_TRUE);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeAutoIPD) {
  ConnectToTS();

  SQLUINTEGER id = -1;
  SQLRETURN ret = SQLGetConnectAttr(dbc, SQL_ATTR_AUTO_IPD, &id, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  BOOST_REQUIRE_EQUAL(id, SQL_FALSE);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeAsyncEnable) {
  ConnectToTS();

  SQLUINTEGER id = -1;
  SQLRETURN ret = SQLGetConnectAttr(dbc, SQL_ATTR_ASYNC_ENABLE, &id, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  BOOST_REQUIRE_EQUAL(id, SQL_ASYNC_ENABLE_OFF);

  ret =
      SQLSetConnectAttr(dbc, SQL_ATTR_ASYNC_ENABLE,
                          reinterpret_cast< SQLPOINTER >(SQL_ASYNC_ENABLE_ON),
                          0);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
  std::string error = GetOdbcErrorMessage(SQL_HANDLE_DBC, dbc);
  std::string pattern = "Specified attribute is not supported.";
  if (error.find(pattern) == std::string::npos)
    BOOST_FAIL("'" + error + "' does not match '" + pattern + "'");
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeTSLogDebug) {
  ConnectToTS();

  SQLUINTEGER id = -1;
  SQLRETURN ret =
      SQLSetConnectAttr(dbc, SQL_ATTR_TSLOG_DEBUG,
      reinterpret_cast< SQLPOINTER >(LogLevel::Type::DEBUG_LEVEL), 0);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_TSLOG_DEBUG, &id, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  BOOST_REQUIRE_EQUAL(id, static_cast<SQLUINTEGER>(LogLevel::Type::DEBUG_LEVEL));
}

// Test SQLGetConnectOption/SQLSetConnectOption
// TODO [AT-1224] enable this test on macOS
// https://bitquill.atlassian.net/browse/AT-1224
#if !defined(__APPLE__)
BOOST_AUTO_TEST_CASE(ConnectionSetAndGetConnectOption) {
  ConnectToTS();

  SQLRETURN ret = SQLSetConnectOption(dbc, SQL_ATTR_METADATA_ID, SQL_TRUE);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  SQLUINTEGER id = 0;
  ret = SQLGetConnectOption(dbc, SQL_ATTR_METADATA_ID, &id);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  BOOST_REQUIRE_EQUAL(id, SQL_TRUE);
}
#endif // #if !defined(__APPLE__)

BOOST_AUTO_TEST_CASE(StatementAttributeQueryTimeout, *disabled()) {
  ConnectToTS();

  SQLULEN timeout = -1;
  SQLRETURN ret = SQLGetStmtAttr(stmt, SQL_ATTR_QUERY_TIMEOUT, &timeout, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);
  BOOST_REQUIRE_EQUAL(timeout, 0);

  ret = SQLSetStmtAttr(stmt, SQL_ATTR_QUERY_TIMEOUT,
                       reinterpret_cast< SQLPOINTER >(7), 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  timeout = -1;

  ret = SQLGetStmtAttr(stmt, SQL_ATTR_QUERY_TIMEOUT, &timeout, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);
  BOOST_REQUIRE_EQUAL(timeout, 7);
}

/**
 * Check that environment returns expected version of ODBC standard.
 *
 * 1. Establish connection using ODBC driver.
 * 2. Get current ODBC version from env handle.
 * 3. Check that version is of the expected value.
 */
BOOST_AUTO_TEST_CASE(TestSQLGetEnvAttrDriverVersion) {
  ConnectToTS();

  SQLINTEGER version;
  SQLRETURN ret = SQLGetEnvAttr(env, SQL_ATTR_ODBC_VERSION, &version, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_ENV, env);

  BOOST_CHECK_EQUAL(version, SQL_OV_ODBC3);
}

BOOST_AUTO_TEST_CASE(TestSQLGetEnvAttrDriverVersionODBC2) {
  // set ODBC version to 2 and check the value is correct.
  ConnectToTS(SQL_OV_ODBC2);

  SQLINTEGER version;
  SQLRETURN ret = SQLGetEnvAttr(env, SQL_ATTR_ODBC_VERSION, &version, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_ENV, env);

  BOOST_CHECK_EQUAL(version, SQL_OV_ODBC2);
}

BOOST_AUTO_TEST_SUITE_END()
