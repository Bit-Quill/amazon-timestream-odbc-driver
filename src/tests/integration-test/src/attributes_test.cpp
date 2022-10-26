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

#include "ignite/odbc/connection.h"
#include "odbc_test_suite.h"
#include "test_type.h"
#include "test_utils.h"

using namespace ignite;
using namespace ignite::odbc::common;
using namespace ignite_test;

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

  /**
   * Connect to AWS Timestream database
   */
  void connectToTS() {
    std::string dsnConnectionString;
    CreateDsnConnectionStringForAWS(dsnConnectionString);

    Connect(dsnConnectionString);
  }

};

BOOST_FIXTURE_TEST_SUITE(AttributesTestSuite, AttributesTestSuiteFixture)

BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionDeadGet) {
  connectToTS();

  SQLUINTEGER dead;
  SQLRETURN ret;

  //TODO: Should return SQL_SUCCESS needs to be fixed by https://bitquill.atlassian.net/browse/AT-1150
  ret = SQLGetConnectAttr(dbc, SQL_ATTR_CONNECTION_DEAD, &dead, 0, 0);

  //ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  //BOOST_REQUIRE_EQUAL(dead, SQL_CD_FALSE);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionDeadSet) {
  connectToTS();

  SQLUINTEGER dead = SQL_CD_TRUE;
  SQLRETURN ret;

  ret = SQLSetConnectAttr(dbc, SQL_ATTR_CONNECTION_DEAD, &dead, 0);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);

  // The error code (HY092) is determined according to
  // https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqlsetconnectattr-function#diagnostics
  CheckSQLConnectionDiagnosticError("HY092");
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeDefaultLoginTimeout) {
  Prepare();

  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);

  std::vector< SQLWCHAR > connectStr(dsnConnectionString.begin(),
                                     dsnConnectionString.end());

  SQLWCHAR outstr[ODBC_BUFFER_SIZE];
  SQLSMALLINT outstrlen;

  // Connecting to Timestream database.
  SQLRETURN ret = SQLDriverConnect(
      dbc, NULL, &connectStr[0],
      static_cast< SQLSMALLINT >(connectStr.size()), outstr,
      sizeof(outstr), &outstrlen, SQL_DRIVER_COMPLETE);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  SQLUINTEGER timeout = -1;
  ret =
      SQLGetConnectAttr(dbc, SQL_ATTR_LOGIN_TIMEOUT, &timeout, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  //Needs to check default value
  //https://bitquill.atlassian.net/browse/AT-1150
  //BOOST_REQUIRE_EQUAL(
  //    timeout, (SQLUINTEGER)Configuration::DefaultValue::loginTimeoutSec);
}

// iODBC not working as expected 
// TODO investigate why the setConnectAttr are not being called before
// establishing connection
// enable test after fix
// https://bitquill.atlassian.net/browse/AT-1150
#ifndef __APPLE__
BOOST_AUTO_TEST_CASE(ConnectionAttributeLoginTimeout) {
  Prepare();

  SQLRETURN ret = SQLSetConnectAttr(dbc, SQL_ATTR_LOGIN_TIMEOUT,
                          reinterpret_cast< SQLPOINTER >(42), 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);

  std::vector< SQLWCHAR > connectStr(dsnConnectionString.begin(),
                                     dsnConnectionString.end());

  SQLWCHAR outstr[ODBC_BUFFER_SIZE];
  SQLSMALLINT outstrlen;

  // Connecting to ODBC server.
  ret = SQLDriverConnect(dbc, NULL, &connectStr[0],
                         static_cast< SQLSMALLINT >(connectStr.size()), outstr,
                         sizeof(outstr), &outstrlen, SQL_DRIVER_COMPLETE);

  SQLUINTEGER timeout = -1;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_LOGIN_TIMEOUT, &timeout, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);
  BOOST_REQUIRE_EQUAL(timeout, 42);
}
#endif

BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionTimeoutGet) {
  connectToTS();

  SQLUINTEGER timeout;
  SQLRETURN ret;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_CONNECTION_TIMEOUT, &timeout, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  BOOST_REQUIRE_EQUAL(timeout, 0);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionTimeoutSet) {
  connectToTS();

  SQLRETURN ret;

  ret = SQLSetConnectAttr(dbc, SQL_ATTR_CONNECTION_TIMEOUT,
                          reinterpret_cast< SQLPOINTER >(10), 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  SQLUINTEGER timeout;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_CONNECTION_TIMEOUT, &timeout, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  BOOST_REQUIRE_EQUAL(timeout, 10);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeAutoCommitGet) {
  connectToTS();

  SQLUINTEGER autoCommit;
  SQLRETURN ret;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_AUTOCOMMIT, &autoCommit, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  BOOST_REQUIRE_EQUAL(autoCommit, SQL_AUTOCOMMIT_ON);
}

BOOST_AUTO_TEST_CASE(ConnectionAttributeAutoCommitSet) {
  connectToTS();

  SQLRETURN ret;

  ret = SQLSetConnectAttr(dbc, SQL_ATTR_AUTOCOMMIT,
                          reinterpret_cast< SQLPOINTER >(SQL_AUTOCOMMIT_OFF), 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  SQLUINTEGER autoCommit;

  ret = SQLGetConnectAttr(dbc, SQL_ATTR_CONNECTION_TIMEOUT, &autoCommit, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  BOOST_REQUIRE_EQUAL(autoCommit, SQL_AUTOCOMMIT_OFF);
}

//TODO: enable this test for https://bitquill.atlassian.net/browse/AT-1150
BOOST_AUTO_TEST_CASE(ConnectionAttributeConnectionOption, *disabled()) {
  connectToTS();

  SQLRETURN ret;

  ret = SQLSetConnectOption(dbc, SQL_ATTR_CONNECTION_TIMEOUT, 10);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  SQLUINTEGER timeout;

  ret = SQLGetConnectOption(dbc, SQL_ATTR_CONNECTION_TIMEOUT, &timeout);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_DBC, dbc);

  BOOST_REQUIRE_EQUAL(timeout, 10);
}

BOOST_AUTO_TEST_CASE(StatementAttributeQueryTimeout, *disabled()) {
  connectToTS();

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
 * 1. Start node.
 * 2. Establish connection using ODBC driver.
 * 3. Get current ODBC version from env handle.
 * 4. Check that version is of the expected value.
 */
BOOST_AUTO_TEST_CASE(TestSQLGetEnvAttrDriverVersion) {
  connectToTS();

  SQLINTEGER version;
  SQLRETURN ret = SQLGetEnvAttr(env, SQL_ATTR_ODBC_VERSION, &version, 0, 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_ENV, env);

  BOOST_CHECK_EQUAL(version, SQL_OV_ODBC3);
}

BOOST_AUTO_TEST_SUITE_END()
