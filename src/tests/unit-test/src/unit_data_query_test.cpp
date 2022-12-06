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

#include <string>

#include <odbc_unit_test_suite.h>
#include "ignite/odbc/log.h"
#include "ignite/odbc/log_level.h"
#include <ignite/odbc/common/platform_utils.h>
#include <ignite/odbc/auth_type.h>
#include "ignite/odbc/statement.h"
#include "ignite/odbc/utility.h"

using ignite::odbc::AuthType;
using ignite::odbc::OdbcUnitTestSuite;
using ignite::odbc::MockConnection;
using ignite::odbc::MockTimestreamService;
using ignite::odbc::config::Configuration;
using ignite::odbc::Statement;
using namespace boost::unit_test;

/**
 * Test setup fixture.
 */
struct DataQueryUnitTestSuiteFixture : OdbcUnitTestSuite {
  DataQueryUnitTestSuiteFixture() : OdbcUnitTestSuite() {
    stmt = dbc->CreateStatement();
  }

  /**
   * Destructor.
   */
  ~DataQueryUnitTestSuiteFixture() = default;

  void getLogOptions(Configuration& config) const {
    using ignite::odbc::common::GetEnv;
    using ignite::odbc::LogLevel;

    std::string logPath = GetEnv("TIMESTREAM_LOG_PATH", "");
    std::string logLevelStr = GetEnv("TIMESTREAM_LOG_LEVEL", "2");

    LogLevel::Type logLevel = LogLevel::FromString(logLevelStr);
    config.SetLogLevel(logLevel);
    config.SetLogPath(logPath);
  }

  bool IsSuccessful() {
    if (!stmt) {
      return false;
    }
    return stmt->GetDiagnosticRecords().IsSuccessful();
  }

  int GetReturnCode() {
    if (!stmt) {
      return SQL_ERROR;
    }
    return stmt->GetDiagnosticRecords().GetReturnCode();
  }

  std::string GetSqlState() {
    if (!stmt) {
      return "";
    }
    return stmt->GetDiagnosticRecords()
        .GetStatusRecord(
            stmt->GetDiagnosticRecords().GetLastNonRetrieved())
        .GetSqlState();
  }

  void Connect() {
    Configuration cfg;
    cfg.SetAuthType(AuthType::Type::IAM);
    cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
    cfg.SetSecretKey("AwsTSUnitTestSecretKey");
    getLogOptions(cfg);

    dbc->Establish(cfg);
  }
};

BOOST_FIXTURE_TEST_SUITE(DataQueryUnitTestSuite, DataQueryUnitTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestDataQuery) {
  // Test a normal query which resultset has 3 rows and returns in one page
  Connect();

  std::string sql = "select measure, time from mockDB.mockTable";
  stmt->ExecuteSqlQuery(sql);

  BOOST_CHECK(IsSuccessful());

  const int32_t buf_size = 1024;
  SQLWCHAR measure[buf_size]{};
  SQLLEN measure_len = 0;

  stmt->BindColumn(1, SQL_C_WCHAR, measure, sizeof(measure), &measure_len);

  SQL_TIMESTAMP_STRUCT timestamp;
  SQLLEN timestamp_len = 0;
  stmt->BindColumn(2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);

  stmt->FetchRow();
  BOOST_CHECK(IsSuccessful());

  BOOST_CHECK_EQUAL("cpu_usage", ignite::odbc::utility::SqlWcharToString(measure, measure_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 11);
  BOOST_CHECK_EQUAL(timestamp.day, 9);
  BOOST_CHECK_EQUAL(timestamp.hour, 23);
  BOOST_CHECK_EQUAL(timestamp.minute, 52);
  BOOST_CHECK_EQUAL(timestamp.second, 51);
  BOOST_CHECK_EQUAL(timestamp.fraction, 554000000);

  stmt->FetchRow();
  BOOST_CHECK(IsSuccessful());

  stmt->FetchRow();
  BOOST_CHECK(IsSuccessful());

  stmt->FetchRow();
  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestDataQuery10000Rows) {
  // Test fetching 10000 rows and each page contains 3 rows
  Connect();

  std::string sql = "select measure, time from mockDB.mockTable10000";
  stmt->ExecuteSqlQuery(sql);

  BOOST_CHECK(IsSuccessful());

  for (int i = 0; i < 10000; i++) {
    stmt->FetchRow();
    BOOST_CHECK(IsSuccessful());
  }
}

BOOST_AUTO_TEST_CASE(TestDataQuery10RowWithError) {
  // Test fetching 10 rows and each page contains 3 rows. 
  // When fetch the 10th row, the outcome contains an error. 
  Connect();

  std::string sql = "select measure, time from mockDB.mockTable10Error";
  stmt->ExecuteSqlQuery(sql);

  BOOST_CHECK(IsSuccessful());

  for (int i = 0; i < 9; i++) {
    stmt->FetchRow();
    BOOST_CHECK(IsSuccessful());
  }

  // The 10th row fetching fails due to response outcome containing an error
  stmt->FetchRow();
  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);

  // no data for the following fetching
  stmt->FetchRow();
  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_NO_DATA);
}

BOOST_AUTO_TEST_SUITE_END()
