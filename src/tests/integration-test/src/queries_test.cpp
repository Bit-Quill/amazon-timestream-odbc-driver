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

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <chrono>

#include "ignite/odbc/common/fixed_size_array.h"
#include "ignite/odbc/utility.h"
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
struct QueriesTestSuiteFixture : odbc::OdbcTestSuite {
  /**
   * Constructor.
   */
  QueriesTestSuiteFixture() {
    BigTablePaginationTestIsEnabled =
        GetEnv("BIG_TABLE_PAGINATION_TEST_ENABLE");
    std::transform(BigTablePaginationTestIsEnabled.begin(),
                   BigTablePaginationTestIsEnabled.end(),
                   BigTablePaginationTestIsEnabled.begin(), ::toupper);
  }

  /**
   * Destructor.
   */
  ~QueriesTestSuiteFixture() override = default;

  /**
   * Connect to the local server with the database name
   *
   * @param databaseName Database Name
   */
  void connectToLocalServer(std::string databaseName) {
    std::string dsnConnectionString;
    CreateDsnConnectionStringForAWS(dsnConnectionString);

    Connect(dsnConnectionString);
  }

  template < typename T >
  void CheckTwoRowsInt(SQLSMALLINT type) {
    connectToLocalServer("odbc-test");

    SQLRETURN ret;

    const SQLSMALLINT columnsCnt = 6;

    T columns[columnsCnt];

    std::memset(&columns, 0, sizeof(columns));

    // Binding columns.
    for (SQLSMALLINT i = 0; i < columnsCnt; ++i) {
      ret = SQLBindCol(stmt, i + 1, type, &columns[i], sizeof(columns[i]),
                       nullptr);

      if (!SQL_SUCCEEDED(ret))
        BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
    }

    std::vector< SQLWCHAR > request = MakeSqlBuffer(
        "SELECT fieldInt, fieldLong, fieldDecimal128, fieldDouble,"
        "fieldString, fieldBoolean FROM queries_test_005 order by fieldInt");

    ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLFetch(stmt);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(columns[0], 1);
    BOOST_CHECK_EQUAL(columns[1], 2);
    BOOST_CHECK_EQUAL(columns[2], 3);
    BOOST_CHECK_EQUAL(columns[3], 4);
    BOOST_CHECK_EQUAL(columns[4], 5);
    BOOST_CHECK_EQUAL(columns[5], 1);

    SQLLEN columnLens[columnsCnt];

    // Binding columns.
    for (SQLSMALLINT i = 0; i < columnsCnt; ++i) {
      ret = SQLBindCol(stmt, i + 1, type, &columns[i], sizeof(columns[i]),
                       &columnLens[i]);

      if (!SQL_SUCCEEDED(ret))
        BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
    }

    ret = SQLFetch(stmt);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(columns[0], 6);
    BOOST_CHECK_EQUAL(columns[1], 5);
    BOOST_CHECK_EQUAL(columns[2], 4);
    BOOST_CHECK_EQUAL(columns[3], 3);
    BOOST_CHECK_EQUAL(columns[4], 2);
    BOOST_CHECK_EQUAL(columns[5], 1);

    BOOST_CHECK_EQUAL(columnLens[0], static_cast< SQLLEN >(sizeof(T)));
    BOOST_CHECK_EQUAL(columnLens[1], static_cast< SQLLEN >(sizeof(T)));
    BOOST_CHECK_EQUAL(columnLens[2], static_cast< SQLLEN >(sizeof(T)));
    BOOST_CHECK_EQUAL(columnLens[3], static_cast< SQLLEN >(sizeof(T)));
    BOOST_CHECK_EQUAL(columnLens[4], static_cast< SQLLEN >(sizeof(T)));
    BOOST_CHECK_EQUAL(columnLens[5], static_cast< SQLLEN >(sizeof(T)));

    ret = SQLFetch(stmt);
    BOOST_CHECK(ret == SQL_NO_DATA);
  }

  void CheckParamsNum(const std::string& req, SQLSMALLINT expectedParamsNum) {
    std::vector< SQLWCHAR > req0(req.begin(), req.end());

    SQLRETURN ret =
        SQLPrepare(stmt, &req0[0], static_cast< SQLINTEGER >(req0.size()));

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    SQLSMALLINT paramsNum = -1;

    ret = SQLNumParams(stmt, &paramsNum);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(paramsNum, expectedParamsNum);
  }

  int CountRows(SQLHSTMT stmt) {
    int res = 0;

    SQLRETURN ret = SQL_SUCCESS;

    while (ret == SQL_SUCCESS) {
      ret = SQLFetch(stmt);

      if (ret == SQL_NO_DATA)
        break;

      if (!SQL_SUCCEEDED(ret))
        BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

      ++res;
    }

    return res;
  }

  std::string BigTablePaginationTestIsEnabled;
};

BOOST_FIXTURE_TEST_SUITE(QueriesTestSuite, QueriesTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestSingleResultUsingBindCol) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select device_id, time, flag, rebuffering_ratio, video_startup_time, "
      "date(TIMESTAMP '2022-07-07 17:44:43.771000000'), current_time, interval "
      "'4' year + interval '2' month,"
      "interval '6' day + interval '4' hour, current_timestamp from "
      "data_queries_test_db.TestScalarTypes order by device_id limit 1");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLWCHAR id[buf_size]{};
  SQLLEN id_len = 0;

  ret = SQLBindCol(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT timestamp;
  SQLLEN timestamp_len = 0;
  ret = SQLBindCol(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  bool fieldBoolean = false;
  SQLLEN fieldBoolean_len = 0;
  ret = SQLBindCol(stmt, 3, SQL_C_BIT, &fieldBoolean, sizeof(fieldBoolean),
                   &fieldBoolean_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  double fieldDouble = 0;
  SQLLEN fieldDouble_len = 0;
  ret = SQLBindCol(stmt, 4, SQL_C_DOUBLE, &fieldDouble, sizeof(fieldDouble),
                   &fieldDouble_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLBIGINT fieldLong;
  SQLLEN fieldLong_len = 0;
  ret = SQLBindCol(stmt, 5, SQL_C_SBIGINT, &fieldLong, sizeof(fieldLong),
                   &fieldLong_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  DATE_STRUCT fieldDate{};
  SQLLEN fieldDate_len = 0;
  ret = SQLBindCol(stmt, 6, SQL_C_TYPE_DATE, &fieldDate, sizeof(fieldDate),
                   &fieldDate_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  TIME_STRUCT timeValue;
  SQLLEN timeValue_len = 0;
  ret = SQLBindCol(stmt, 7, SQL_C_TYPE_TIME, &timeValue, sizeof(timeValue),
                   &timeValue_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_YEAR_MONTH_STRUCT yearMonth;
  SQLLEN yearMonth_len = 0;
  ret = SQLBindCol(stmt, 8, SQL_C_INTERVAL_YEAR_TO_MONTH, &yearMonth,
                   sizeof(yearMonth), &yearMonth_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_DAY_SECOND_STRUCT daySecond;
  SQLLEN daySecond_len = 0;
  ret = SQLBindCol(stmt, 9, SQL_C_INTERVAL_DAY_TO_SECOND, &daySecond,
                   sizeof(daySecond), &daySecond_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT current_timestamp;
  SQLLEN current_timestamp_len = 0;
  ret = SQLBindCol(stmt, 10, SQL_C_TYPE_TIMESTAMP, &current_timestamp,
                   sizeof(current_timestamp), &current_timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000001", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 20);
  BOOST_CHECK_EQUAL(true, fieldBoolean);
  BOOST_CHECK_EQUAL(0.1, fieldDouble);
  BOOST_CHECK_EQUAL(1, fieldLong);

  BOOST_CHECK_EQUAL(fieldDate.year, 2022);
  BOOST_CHECK_EQUAL(fieldDate.month, 7);
  BOOST_CHECK_EQUAL(fieldDate.day, 7);

  BOOST_CHECK_EQUAL(timeValue.hour, current_timestamp.hour);
  BOOST_CHECK_EQUAL(timeValue.minute, current_timestamp.minute);
  BOOST_CHECK_EQUAL(timeValue.second, current_timestamp.second);

  BOOST_CHECK_EQUAL(yearMonth.year, 4);
  BOOST_CHECK_EQUAL(yearMonth.month, 2);

  BOOST_CHECK_EQUAL(daySecond.day, 6);
  BOOST_CHECK_EQUAL(daySecond.hour, 4);

  // Fetch 2nd row - not exist
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
}

BOOST_AUTO_TEST_CASE(TestSingleResultUsingGetData) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select device_id, time, flag, rebuffering_ratio, video_startup_time, "
      "date(TIMESTAMP '2022-07-07 17:44:43.771000000'), current_time, interval "
      "'4' year + interval '2' month,"
      "interval '6' day + interval '4' hour, current_timestamp from "
      "data_queries_test_db.TestScalarTypes order by device_id limit 1");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  const int32_t buf_size = 1024;
  SQLWCHAR id[buf_size]{};
  SQLLEN id_len = 0;

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT timestamp;
  SQLLEN timestamp_len = 0;
  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  bool fieldBoolean = false;
  SQLLEN fieldBoolean_len = 0;
  ret = SQLGetData(stmt, 3, SQL_C_BIT, &fieldBoolean, sizeof(fieldBoolean),
                   &fieldBoolean_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  double fieldDouble = 0;
  SQLLEN fieldDouble_len = 0;
  ret = SQLGetData(stmt, 4, SQL_C_DOUBLE, &fieldDouble, sizeof(fieldDouble),
                   &fieldDouble_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLBIGINT fieldLong;
  SQLLEN fieldLong_len = 0;
  ret = SQLGetData(stmt, 5, SQL_C_SBIGINT, &fieldLong, sizeof(fieldLong),
                   &fieldLong_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  DATE_STRUCT fieldDate{};
  SQLLEN fieldDate_len = 0;
  ret = SQLGetData(stmt, 6, SQL_C_TYPE_DATE, &fieldDate, sizeof(fieldDate),
                   &fieldDate_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  TIME_STRUCT timeValue;
  SQLLEN timeValue_len = 0;
  ret = SQLGetData(stmt, 7, SQL_C_TYPE_TIME, &timeValue, sizeof(timeValue),
                   &timeValue_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS_WITH_INFO, ret);

  SQL_YEAR_MONTH_STRUCT yearMonth;
  SQLLEN yearMonth_len = 0;
  ret = SQLGetData(stmt, 8, SQL_C_INTERVAL_YEAR_TO_MONTH, &yearMonth,
                   sizeof(yearMonth), &yearMonth_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_DAY_SECOND_STRUCT daySecond;
  SQLLEN daySecond_len = 0;
  ret = SQLGetData(stmt, 9, SQL_C_INTERVAL_DAY_TO_SECOND, &daySecond,
                   sizeof(daySecond), &daySecond_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT current_timestamp;
  SQLLEN current_timestamp_len = 0;
  ret = SQLGetData(stmt, 10, SQL_C_TYPE_TIMESTAMP, &current_timestamp,
                   sizeof(current_timestamp), &current_timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000001", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 20);
  BOOST_CHECK_EQUAL(true, fieldBoolean);
  BOOST_CHECK_EQUAL(0.1, fieldDouble);
  BOOST_CHECK_EQUAL(1, fieldLong);

  BOOST_CHECK_EQUAL(fieldDate.year, 2022);
  BOOST_CHECK_EQUAL(fieldDate.month, 7);
  BOOST_CHECK_EQUAL(fieldDate.day, 7);

  BOOST_CHECK_EQUAL(timeValue.hour, current_timestamp.hour);
  BOOST_CHECK_EQUAL(timeValue.minute, current_timestamp.minute);
  BOOST_CHECK_EQUAL(timeValue.second, current_timestamp.second);

  BOOST_CHECK_EQUAL(yearMonth.year, 4);
  BOOST_CHECK_EQUAL(yearMonth.month, 2);

  BOOST_CHECK_EQUAL(daySecond.day, 6);
  BOOST_CHECK_EQUAL(daySecond.hour, 4);

  // Fetch 2nd row - not exist
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
}

BOOST_AUTO_TEST_CASE(TestMultiLineResultUsingGetData) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select device_id, time from data_queries_test_db.TestScalarTypes order "
      "by device_id limit 3");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  const int32_t buf_size = 1024;
  SQLWCHAR id[buf_size]{};
  SQLLEN id_len = 0;

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT timestamp;
  SQLLEN timestamp_len = 0;
  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000001", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 20);

  // Fetch 2nd row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000002", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 21);

  // Fetch 3rd row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000003", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 22);

  // Fetch 4th row - not exist
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
}

BOOST_AUTO_TEST_CASE(TestSQLFetchTimeStampAsOtherTypes) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select time as firstTime, time, time, time from "
      "data_queries_test_db.TestScalarTypes order by firstTime");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR timestampChar[buf_size]{};
  SQLLEN timestampChar_len = 0;

  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, timestampChar, sizeof(timestampChar),
                   &timestampChar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR timestampWchar[buf_size]{};
  SQLLEN timestampWchar_len = 0;
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, &timestampWchar,
                   sizeof(timestampWchar), &timestampWchar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  DATE_STRUCT fieldDate{};
  SQLLEN fieldDate_len = 0;
  ret = SQLBindCol(stmt, 3, SQL_C_TYPE_DATE, &fieldDate, sizeof(fieldDate),
                   &fieldDate_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  TIME_STRUCT timeValue;
  SQLLEN timeValue_len = 0;
  ret = SQLBindCol(stmt, 4, SQL_C_TYPE_TIME, &timeValue, sizeof(timeValue),
                   &timeValue_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("2022-10-20 19:01:02.000000000",
                    utility::SqlCharToString(timestampChar, timestampChar_len));
  BOOST_CHECK_EQUAL(
      "2022-10-20 19:01:02.000000000",
      utility::SqlWcharToString(timestampWchar, timestampWchar_len));

  BOOST_CHECK_EQUAL(fieldDate.year, 2022);
  BOOST_CHECK_EQUAL(fieldDate.month, 10);
  BOOST_CHECK_EQUAL(fieldDate.day, 20);

  BOOST_CHECK_EQUAL(timeValue.hour, 19);
  BOOST_CHECK_EQUAL(timeValue.minute, 1);
  BOOST_CHECK_EQUAL(timeValue.second, 2);
}

BOOST_AUTO_TEST_CASE(TestSQLFetchTimeAsOtherTypes) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select current_time, current_time, current_time, current_time");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR timestampChar[buf_size]{};
  SQLLEN timestampChar_len = 0;

  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, timestampChar, sizeof(timestampChar),
                   &timestampChar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR timestampWchar[buf_size]{};
  SQLLEN timestampWchar_len = 0;
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, &timestampWchar,
                   sizeof(timestampWchar), &timestampWchar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT timestamp;
  SQLLEN timestamp_len = 0;
  ret = SQLBindCol(stmt, 3, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  TIME_STRUCT timeValue;
  SQLLEN timeValue_len = 0;
  ret = SQLBindCol(stmt, 4, SQL_C_TYPE_TIME, &timeValue, sizeof(timeValue),
                   &timeValue_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL(
      utility::SqlCharToString(timestampChar, timestampChar_len),
      utility::SqlWcharToString(timestampWchar, timestampWchar_len));

  BOOST_CHECK_EQUAL(timeValue.hour, timestamp.hour);
  BOOST_CHECK_EQUAL(timeValue.minute, timestamp.minute);
  BOOST_CHECK_EQUAL(timeValue.second, timestamp.second);
}

BOOST_AUTO_TEST_CASE(TestSQLFetchDateAsOtherTypes) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select date(TIMESTAMP '2022-07-07 17:44:43.771000000'),"
      "date(TIMESTAMP '2022-07-07 17:44:43.771000000'),"
      "date(TIMESTAMP '2022-07-07 17:44:43.771000000')");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR timestampChar[buf_size]{};
  SQLLEN timestampChar_len = 0;

  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, timestampChar, sizeof(timestampChar),
                   &timestampChar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR timestampWchar[buf_size]{};
  SQLLEN timestampWchar_len = 0;
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, &timestampWchar,
                   sizeof(timestampWchar), &timestampWchar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT timestamp;
  SQLLEN timestamp_len = 0;
  ret = SQLBindCol(stmt, 3, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("2022-07-07",
                    utility::SqlCharToString(timestampChar, timestampChar_len));
  BOOST_CHECK_EQUAL("2022-07-07", utility::SqlWcharToString(
                                      timestampWchar, timestampWchar_len));

  BOOST_CHECK_EQUAL(2022, timestamp.year);
  BOOST_CHECK_EQUAL(7, timestamp.month);
  BOOST_CHECK_EQUAL(7, timestamp.day);
  BOOST_CHECK_EQUAL(0, timestamp.hour);
  BOOST_CHECK_EQUAL(0, timestamp.minute);
  BOOST_CHECK_EQUAL(0, timestamp.second);
  BOOST_CHECK_EQUAL(0, timestamp.fraction);
}

BOOST_AUTO_TEST_CASE(TestSQLFetchIntervalYearMonthAsOtherTypes) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "SELECT interval '3' year + interval '11' month, interval '3' year + "
      "interval '11' month");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR yearMonthChar[buf_size]{};
  SQLLEN yearMonthChar_len = 0;

  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, &yearMonthChar, sizeof(yearMonthChar),
                   &yearMonthChar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR yearMonthWchar[buf_size]{};
  SQLLEN yearMonthWchar_len = 0;
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, &yearMonthWchar,
                   sizeof(yearMonthWchar), &yearMonthWchar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("3-11",
                    utility::SqlCharToString(yearMonthChar, yearMonthChar_len));

  BOOST_CHECK_EQUAL(
      "3-11", utility::SqlWcharToString(yearMonthWchar, yearMonthWchar_len));
}

BOOST_AUTO_TEST_CASE(TestSQLFetchIntervalDayMonthAsOtherTypes) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "SELECT interval '6' day + interval '0' hour, interval '0' day + "
      "interval '4' hour");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR daySecondChar[buf_size]{};
  SQLLEN daySecondChar_len = 0;

  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, &daySecondChar, sizeof(daySecondChar),
                   &daySecondChar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR daySecondWchar[buf_size]{};
  SQLLEN daySecondWchar_len = 0;
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, &daySecondWchar,
                   sizeof(daySecondWchar), &daySecondWchar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("6 00:00:00.000000000",
                    utility::SqlCharToString(daySecondChar, daySecondChar_len));

  BOOST_CHECK_EQUAL(
      "0 04:00:00.000000000",
      utility::SqlWcharToString(daySecondWchar, daySecondWchar_len));
}

BOOST_AUTO_TEST_CASE(TestTimeSeriesSingleResultUsingBindCol) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "SELECT region, az, vpc, instance_id,"
      "CREATE_TIME_SERIES(time, measure_value::double) as cpu_utilization, "
      "CREATE_TIME_SERIES(time, measure_value::double) as cpu_utilization2 "
      "FROM data_queries_test_db.TestComplexTypes WHERE "
      "measure_name='cpu_utilization' "
      "GROUP BY region, az, vpc, instance_id order by instance_id");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR timeSeriesChar[buf_size]{};
  SQLLEN timeSeriesChar_len = 0;

  // fetch result as a string
  ret = SQLBindCol(stmt, 5, SQL_C_CHAR, timeSeriesChar, sizeof(timeSeriesChar),
                   &timeSeriesChar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR timeSeriesWchar[buf_size]{};
  SQLLEN timeSeriesWchar_len = 0;

  // fetch result as a unicode string
  ret = SQLBindCol(stmt, 6, SQL_C_WCHAR, timeSeriesWchar,
                   sizeof(timeSeriesWchar), &timeSeriesWchar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  std::string expected = "[{time: 2019-12-04 19:00:00.000000000, value: 35.2},";
  expected += "{time: 2019-12-04 19:01:00.000000000, value: 38.2},";
  expected += "{time: 2019-12-04 19:02:00.000000000, value: 45.3}]";

  BOOST_CHECK_EQUAL(
      expected, utility::SqlCharToString(timeSeriesChar, timeSeriesChar_len));

  BOOST_CHECK_EQUAL(expected, utility::SqlWcharToString(
                                  timeSeriesWchar, timeSeriesWchar_len, true));
}

BOOST_AUTO_TEST_CASE(TestArraySingleResultUsingBindCol) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request =
      MakeSqlBuffer("select Array[1,2,3], Array[1,2,3], Array[], Array[]");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR arrayChar1[buf_size]{};
  SQLLEN arrayChar1_len = 0;

  // fetch result as a string
  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, arrayChar1, sizeof(arrayChar1),
                   &arrayChar1_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR arrayWchar1[buf_size]{};
  SQLLEN arrayWchar1_len = 0;

  // fetch result as a unicode string
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, arrayWchar1, sizeof(arrayWchar1),
                   &arrayWchar1_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLCHAR arrayChar2[buf_size]{};
  SQLLEN arrayChar2_len = 0;

  // fetch result as a string
  ret = SQLBindCol(stmt, 3, SQL_C_CHAR, arrayChar2, sizeof(arrayChar2),
                   &arrayChar2_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR arrayWchar2[buf_size]{};
  SQLLEN arrayWchar2_len = 0;

  // fetch result as a unicode string
  ret = SQLBindCol(stmt, 4, SQL_C_WCHAR, arrayWchar2, sizeof(arrayWchar2),
                   &arrayWchar2_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("[1,2,3]",
                    utility::SqlCharToString(arrayChar1, arrayChar1_len));
  BOOST_CHECK_EQUAL(
      "[1,2,3]", utility::SqlWcharToString(arrayWchar1, arrayWchar1_len, true));
  BOOST_CHECK_EQUAL("-", utility::SqlCharToString(arrayChar2, arrayChar2_len));
  BOOST_CHECK_EQUAL(
      "-", utility::SqlWcharToString(arrayWchar2, arrayWchar2_len, true));
}

BOOST_AUTO_TEST_CASE(TestRowSingleResultUsingBindCol) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer("SELECT (1,2,3), (1,2,3)");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR rowChar[buf_size]{};
  SQLLEN rowChar_len = 0;

  // fetch result as a string
  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, rowChar, sizeof(rowChar), &rowChar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR rowWchar[buf_size]{};
  SQLLEN rowWchar_len = 0;

  // fetch result as a unicode string
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, rowWchar, sizeof(rowWchar),
                   &rowWchar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("(1,2,3)", utility::SqlCharToString(rowChar, rowChar_len));
  BOOST_CHECK_EQUAL("(1,2,3)",
                    utility::SqlWcharToString(rowWchar, rowWchar_len, true));
}

BOOST_AUTO_TEST_CASE(TestNullSingleResultUsingBindCol) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > request = MakeSqlBuffer("select null, null");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR nullChar[buf_size]{};
  SQLLEN nullChar_len = 0;

  // fetch result as a string
  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, nullChar, sizeof(nullChar),
                   &nullChar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQLWCHAR nullWchar[buf_size]{};
  SQLLEN nullWchar_len = 0;

  // fetch result as a unicode string
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, nullWchar, sizeof(nullWchar),
                   &nullWchar_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("-", utility::SqlCharToString(nullChar, nullChar_len));
  BOOST_CHECK_EQUAL("-",
                    utility::SqlWcharToString(nullWchar, nullWchar_len, true));
}

BOOST_AUTO_TEST_CASE(TestArrayStructJoinUsingGetData, *disabled()) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "SELECT q3.queries_test_003__id, \n"
      "  \"a1\".\"value\", \n"
      "  \"a2\".\"value\", \n"
      "  \"d1\".\"field1\", \n"
      "  \"d2\".\"field2\" \n"
      " FROM queries_test_003 AS q3 \n"
      " JOIN queries_test_003_fieldArrayOfInt AS a1 \n"
      "  ON q3.queries_test_003__id = a1.queries_test_003__id \n"
      " JOIN queries_test_003_fieldArrayOfString AS a2 \n"
      "  ON a1.queries_test_003__id = a2.queries_test_003__id \n"
      " JOIN queries_test_003_fieldDocument AS d1 \n"
      "  ON a2.queries_test_003__id = d1.queries_test_003__id \n"
      " JOIN queries_test_003_fieldDocument_subDoc AS d2 \n"
      "  ON d1.queries_test_003__id = d2.queries_test_003__id");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLWCHAR id[buf_size]{};
  SQLLEN id_len = 0;
  SQLINTEGER a1_value = 0;
  SQLLEN a1_value_len = 0;
  SQLWCHAR a2_value[buf_size]{};
  SQLLEN a2_value_len = 0;
  SQLWCHAR d1_value[buf_size]{};
  SQLLEN d1_value_len = 0;
  SQLWCHAR d2_value[buf_size]{};
  SQLLEN d2_value_len = 0;

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
  ret = SQLGetData(stmt, 2, SQL_C_SLONG, &a1_value, sizeof(a1_value),
                   &a1_value_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
  ret = SQLGetData(stmt, 3, SQL_C_WCHAR, a2_value, sizeof(a2_value),
                   &a2_value_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
  ret = SQLGetData(stmt, 4, SQL_C_WCHAR, d1_value, sizeof(d1_value),
                   &d1_value_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
  ret = SQLGetData(stmt, 5, SQL_C_WCHAR, d2_value, sizeof(d2_value),
                   &d2_value_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  // Check the first row
  BOOST_CHECK_NE(SQL_NULL_DATA, id_len);
  BOOST_CHECK_EQUAL("62196dcc4d91892191475139",
                    utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_NE(SQL_NULL_DATA, a1_value_len);
  BOOST_CHECK_EQUAL(1, a1_value);
  BOOST_CHECK_NE(SQL_NULL_DATA, a2_value_len);
  BOOST_CHECK_EQUAL("value1",
                    utility::SqlWcharToString(a2_value, a2_value_len, true));
  BOOST_CHECK_NE(SQL_NULL_DATA, d1_value_len);
  BOOST_CHECK_EQUAL("field1 Value",
                    utility::SqlWcharToString(d1_value, d1_value_len, true));
  BOOST_CHECK_NE(SQL_NULL_DATA, d2_value_len);
  BOOST_CHECK_EQUAL("field2 Value",
                    utility::SqlWcharToString(d2_value, d2_value_len, true));

  // Count the rows
  int32_t actual_rows = 0;
  while (SQL_SUCCEEDED(ret)) {
    actual_rows++;
    ret = SQLFetch(stmt);
  }
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
  BOOST_CHECK_EQUAL(9, actual_rows);
}

BOOST_AUTO_TEST_CASE(TestSQLFetchBigTablePagination) {
  if (BigTablePaginationTestIsEnabled == "TRUE") {
    // This test verifies big table resultset could be paginated
    // and the returned data is correct
    std::string dsnConnectionString;
    CreateDsnConnectionStringForAWS(dsnConnectionString);
    Connect(dsnConnectionString);
    SQLRETURN ret;
    // data_queries_test_db.TestMultiMeasureBigTable is a big table which has
    // 20,000 records and the resultset will be paginated by default
    std::vector< SQLWCHAR > request = MakeSqlBuffer(
        "select time, index, cpu_utilization from "
        "data_queries_test_db.TestMultiMeasureBigTable order by time");
    ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
    SQL_TIMESTAMP_STRUCT timestamp;
    SQLLEN timestamp_len = 0;
    ret = SQLBindCol(stmt, 1, SQL_C_TYPE_TIMESTAMP, &timestamp,
                     sizeof(timestamp), &timestamp_len);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
    SQLBIGINT fieldLong;
    SQLLEN fieldLong_len = 0;
    ret = SQLBindCol(stmt, 2, SQL_C_SBIGINT, &fieldLong, sizeof(fieldLong),
                     &fieldLong_len);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
    double fieldDouble = 0;
    SQLLEN fieldDouble_len = 0;
    ret = SQLBindCol(stmt, 3, SQL_C_DOUBLE, &fieldDouble, sizeof(fieldDouble),
                     &fieldDouble_len);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

    // Get 1st row of current page
    ret = SQLFetch(stmt);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
    // verify result
    BOOST_CHECK_EQUAL(timestamp.year, 2022);
    BOOST_CHECK_EQUAL(timestamp.month, 11);
    BOOST_CHECK_EQUAL(timestamp.day, 9);
    BOOST_CHECK_EQUAL(timestamp.hour, 23);
    BOOST_CHECK_EQUAL(timestamp.minute, 51);
    BOOST_CHECK_EQUAL(timestamp.second, 56);
    BOOST_CHECK_EQUAL(60.502944999999997, fieldDouble);
    BOOST_CHECK_EQUAL(1, fieldLong);
  } else {
    std::cout << boost::unit_test::framework::current_test_case().p_name
              << " is skipped" << std::endl;
  }
}

BOOST_AUTO_TEST_CASE(TestSQLExecBigTablePagination) {
  if (BigTablePaginationTestIsEnabled == "TRUE") {
    // This test verifies the internal asynchronous thread could be
    // terminated when testcase ends. It also verifies all rows could
    // be fetched including the last page.
    std::string dsnConnectionString;
    CreateDsnConnectionStringForAWS(dsnConnectionString);
    Connect(dsnConnectionString);
    SQLRETURN ret;
    std::vector< SQLWCHAR > request = MakeSqlBuffer(
        "select time, index, cpu_utilization from "
        "data_queries_test_db.TestMultiMeasureBigTable order by time");
    ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

    int count = 0;
    do {
      ret = SQLFetch(stmt);
      if (ret == SQL_NO_DATA) {
        break;
      } else if (!SQL_SUCCEEDED(ret)) {
        BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
      }
      count++;
    } while (true);

    BOOST_CHECK_EQUAL(20000, count);
  } else {
    std::cout << boost::unit_test::framework::current_test_case().p_name
              << " is skipped" << std::endl;
  }
}

BOOST_AUTO_TEST_CASE(TestSQLFetchBigTablePagination1000Rows) {
  if (BigTablePaginationTestIsEnabled == "TRUE") {
    // Fetch 1000 rows and verify the resultset is correct for 1001st record.
    // Each page contains only one row. There will be 1000 internal asynchronous
    // threads created to fetch 1000 pages.
    std::string dsnConnectionString;
    CreateDsnConnectionStringForAWS(dsnConnectionString);
    AddMaxRowPerPage(dsnConnectionString, "1");
    Connect(dsnConnectionString);
    SQLRETURN ret;
    std::vector< SQLWCHAR > request = MakeSqlBuffer(
        "select time, index, cpu_utilization from "
        "data_queries_test_db.TestMultiMeasureBigTable order by time");
    ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

    int count = 0;

    // These time_points could be reopened in case there is
    // a performance check need.
    // std::chrono::steady_clock::time_point time_exec_start =
    //    std::chrono::steady_clock::now();

    // fetch 1000 rows
    do {
      ret = SQLFetch(stmt);
      if (!SQL_SUCCEEDED(ret))
        BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
      count++;
    } while (count < 1000);

    /*
    std::chrono::steady_clock::time_point time_exec_end =
        std::chrono::steady_clock::now();

    std::chrono::steady_clock::duration time_span =
        time_exec_end - time_exec_start;

    double nseconds = double(time_span.count())
                      * std::chrono::steady_clock::period::num
                      / std::chrono::steady_clock::period::den;
    std::cout << "Fetching 1000 rows took " << nseconds << " seconds"
              << std::endl;
    */

    SQL_TIMESTAMP_STRUCT timestamp;
    SQLLEN timestamp_len = 0;
    ret = SQLBindCol(stmt, 1, SQL_C_TYPE_TIMESTAMP, &timestamp,
                     sizeof(timestamp), &timestamp_len);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
    SQLBIGINT fieldLong;
    SQLLEN fieldLong_len = 0;
    ret = SQLBindCol(stmt, 2, SQL_C_SBIGINT, &fieldLong, sizeof(fieldLong),
                     &fieldLong_len);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);
    double fieldDouble = 0;
    SQLLEN fieldDouble_len = 0;
    ret = SQLBindCol(stmt, 3, SQL_C_DOUBLE, &fieldDouble, sizeof(fieldDouble),
                     &fieldDouble_len);
    BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

    // Get 1001st row
    ret = SQLFetch(stmt);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    // verify result
    BOOST_CHECK_EQUAL(timestamp.year, 2022);
    BOOST_CHECK_EQUAL(timestamp.month, 11);
    BOOST_CHECK_EQUAL(timestamp.day, 9);
    BOOST_CHECK_EQUAL(timestamp.hour, 23);
    BOOST_CHECK_EQUAL(timestamp.minute, 52);
    BOOST_CHECK_EQUAL(timestamp.second, 51);
    BOOST_CHECK_EQUAL(71.239357, fieldDouble);
    BOOST_CHECK_EQUAL(1001, fieldLong);
  } else {
    std::cout << boost::unit_test::framework::current_test_case().p_name
              << " is skipped" << std::endl;
  }
}

BOOST_AUTO_TEST_CASE(TestSmallResultPagination) {
  // This test runs a query which returns 3 rows. It sets each page
  // contains 1 row. It verifies the results are correct.
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  AddMaxRowPerPage(dsnConnectionString, "1");
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select device_id, time from data_queries_test_db.TestScalarTypes order "
      "by device_id limit 3");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  // Fetch 1st row
  // These time_points could be reopened in case there is
  // a performance check need.
  // std::chrono::steady_clock::time_point time_exec_start =
  //    std::chrono::steady_clock::now();
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  /*
  std::chrono::steady_clock::time_point time_exec_end =
      std::chrono::steady_clock::now();

  std::chrono::steady_clock::duration time_span =
      time_exec_end - time_exec_start;

  double nseconds = double(time_span.count())
                    * std::chrono::steady_clock::period::num
                    / std::chrono::steady_clock::period::den;
  std::cout << "Fetching 1st row took " << nseconds << " seconds" << std::endl;
  */

  const int32_t buf_size = 1024;
  SQLWCHAR id[buf_size]{};
  SQLLEN id_len = 0;

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT timestamp;
  SQLLEN timestamp_len = 0;
  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000001", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 20);

  // Fetch 2nd row
  // time_exec_start = std::chrono::steady_clock::now();
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  /*
  time_exec_end = std::chrono::steady_clock::now();

  time_span = time_exec_end - time_exec_start;

  nseconds = double(time_span.count()) * std::chrono::steady_clock::period::num
             / std::chrono::steady_clock::period::den;
  std::cout << "Fetching 2nd row took " << nseconds << " seconds" << std::endl;
  */

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000002", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 21);

  // Fetch 3rd row
  // time_exec_start = std::chrono::steady_clock::now();
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  /*
  time_exec_end = std::chrono::steady_clock::now();

  time_span = time_exec_end - time_exec_start;

  nseconds = double(time_span.count()) * std::chrono::steady_clock::period::num
             / std::chrono::steady_clock::period::den;
  std::cout << "Fetching 3rd row took " << nseconds << " seconds" << std::endl;
  */

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000003", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 22);

  // Fetch 4th row - not exist
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
}

BOOST_AUTO_TEST_CASE(TestSmallResultPaginationTermination) {
  // This test runs a query which returns 3 rows. It sets each page
  // contains 1 row. It only fetches the first two rows. It could verify
  // the 3rd row asynchronous thread be terminated without a problem.
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  AddMaxRowPerPage(dsnConnectionString, "1");
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select device_id, time from data_queries_test_db.TestScalarTypes order "
      "by device_id limit 3");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  const int32_t buf_size = 1024;
  SQLWCHAR id[buf_size]{};
  SQLLEN id_len = 0;

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  SQL_TIMESTAMP_STRUCT timestamp;
  SQLLEN timestamp_len = 0;
  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000001", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 20);

  // Fetch 2nd row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, id, sizeof(id), &id_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 2, SQL_C_TYPE_TIMESTAMP, &timestamp, sizeof(timestamp),
                   &timestamp_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_EQUAL("00000002", utility::SqlWcharToString(id, id_len, true));
  BOOST_CHECK_EQUAL(timestamp.year, 2022);
  BOOST_CHECK_EQUAL(timestamp.month, 10);
  BOOST_CHECK_EQUAL(timestamp.day, 21);
}

BOOST_AUTO_TEST_CASE(TestSmallResultPaginationNoFetch) {
  // This test runs a query which returns 3 rows. It sets each page
  // to contain 1 row. It does not fetch any data. It verifies that
  // the asynchronous thread could be terminated without a problem.
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  AddMaxRowPerPage(dsnConnectionString, "1");
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select device_id, time from data_queries_test_db.TestScalarTypes order "
      "by device_id limit 3");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }
}

BOOST_AUTO_TEST_CASE(TestSQLFetchPaginationEmptyTable) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  AddMaxRowPerPage(dsnConnectionString, "1");
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "select measure_name, time from data_queries_test_db.EmptyTable");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);

  ret = SQLFetch(stmt);
#ifdef _WIN32
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
#else
  // unixODBC DM/iODBC DM returns an error
  BOOST_CHECK_EQUAL(SQL_ERROR, ret);
  std::string error = GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt);
#if defined(__APPLE__) || (defined(__linux__) && defined(__i386__))
  std::string pattern = "Function sequence error";
#else
  std::string pattern = "Invalid cursor state";
#endif  //__APPLE__
  if (error.find(pattern) == std::string::npos)
    BOOST_FAIL("'" + error + "' does not match '" + pattern + "'");
#endif  //_WIN32
}

BOOST_AUTO_TEST_CASE(TestSQLRowCountWithNoResults) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;

  std::vector< SQLWCHAR > sql = MakeSqlBuffer(
      "select * from data_queries_test_db.TestScalarTypes limit 20");

  ret = SQLExecDirect(stmt, sql.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  SQLLEN rows = 0;

  ret = SQLRowCount(stmt, &rows);

  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  // SQLRowCount should set rows to -1 as no rows were changed
  BOOST_CHECK_EQUAL(-1, rows);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsInt8, *disabled()) {
  CheckTwoRowsInt< signed char >(SQL_C_STINYINT);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsUint8, *disabled()) {
  CheckTwoRowsInt< unsigned char >(SQL_C_UTINYINT);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsInt16, *disabled()) {
  CheckTwoRowsInt< signed short >(SQL_C_SSHORT);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsUint16, *disabled()) {
  CheckTwoRowsInt< unsigned short >(SQL_C_USHORT);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsInt32, *disabled()) {
  CheckTwoRowsInt< SQLINTEGER >(SQL_C_SLONG);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsUint32, *disabled()) {
  CheckTwoRowsInt< SQLUINTEGER >(SQL_C_ULONG);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsInt64, *disabled()) {
  CheckTwoRowsInt< int64_t >(SQL_C_SBIGINT);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsUint64, *disabled()) {
  CheckTwoRowsInt< uint64_t >(SQL_C_UBIGINT);
}

BOOST_AUTO_TEST_CASE(TestTwoRowsString, *disabled()) {
  connectToLocalServer("odbc-test");

  SQLRETURN ret;

  const SQLSMALLINT columnsCnt = 6;

  SQLWCHAR columns[columnsCnt][ODBC_BUFFER_SIZE];

  // Binding columns.
  for (SQLSMALLINT i = 0; i < columnsCnt; ++i) {
    ret = SQLBindCol(stmt, i + 1, SQL_C_WCHAR, &columns[i],
                     ODBC_BUFFER_SIZE * sizeof(SQLWCHAR), 0);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "SELECT fieldInt, fieldLong, fieldDecimal128, fieldDouble,"
      "fieldString, fieldBoolean FROM queries_test_005 order by fieldInt");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[0], SQL_NTS, true), "1");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[1], SQL_NTS, true), "2");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[2], SQL_NTS, true), "3");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[3], SQL_NTS, true), "4");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[4], SQL_NTS, true), "5");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[5], SQL_NTS, true), "1");

  SQLLEN columnLens[columnsCnt];

  // Binding columns.
  for (SQLSMALLINT i = 0; i < columnsCnt; ++i) {
    ret = SQLBindCol(stmt, i + 1, SQL_C_WCHAR, &columns[i],
                     ODBC_BUFFER_SIZE * sizeof(SQLWCHAR), &columnLens[i]);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[0], SQL_NTS, true), "6");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[1], SQL_NTS, true), "5");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[2], SQL_NTS, true), "4");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[3], SQL_NTS, true), "3");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[4], SQL_NTS, true), "2");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[5], SQL_NTS, true), "1");

#ifdef __APPLE__
  SQLLEN expectedLen = 4;
#else
  SQLLEN expectedLen = 2;
#endif

  BOOST_CHECK_EQUAL(columnLens[0], expectedLen);
  BOOST_CHECK_EQUAL(columnLens[1], expectedLen);
  BOOST_CHECK_EQUAL(columnLens[2], expectedLen);
  BOOST_CHECK_EQUAL(columnLens[3], expectedLen);
  BOOST_CHECK_EQUAL(columnLens[4], expectedLen);
  BOOST_CHECK_EQUAL(columnLens[5], expectedLen);

  ret = SQLFetch(stmt);
  BOOST_CHECK(ret == SQL_NO_DATA);
}

// TODO: Memory leak, traced by https://bitquill.atlassian.net/browse/AD-813
BOOST_AUTO_TEST_CASE(TestOneRowObject, *disabled()) {
  connectToLocalServer("odbc-test");

  SQLRETURN ret;

  int64_t column1 = 0;
  int8_t column2[ODBC_BUFFER_SIZE];
  char column3[ODBC_BUFFER_SIZE];

  SQLLEN column1Len = sizeof(column1);
  SQLLEN column2Len = sizeof(column2);
  SQLLEN column3Len = sizeof(column3);

  // Binding columns.
  ret = SQLBindCol(stmt, 1, SQL_C_SLONG, &column1, column1Len, &column1Len);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLBindCol(stmt, 2, SQL_C_BINARY, &column2, column2Len, &column2Len);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLBindCol(stmt, 3, SQL_C_CHAR, &column3, column3Len, &column3Len);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "SELECT fieldInt, fieldBinary, fieldString FROM queries_test_005 order "
      "by fieldInt");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(column1, 1);
  BOOST_CHECK_EQUAL(column2[0], 0);
  BOOST_CHECK_EQUAL(column2[1], 1);
  BOOST_CHECK_EQUAL(column2[2], 2);
  BOOST_CHECK_EQUAL(column3, "5");
}

// Enable this testcase when AD-549 is finished
BOOST_AUTO_TEST_CASE(TestDataAtExecution, *disabled()) {
  connectToLocalServer("odbc-test");

  SQLRETURN ret;

  const SQLSMALLINT columnsCnt = 6;

  SQLLEN columnLens[columnsCnt];
  SQLWCHAR columns[columnsCnt][ODBC_BUFFER_SIZE];

  // Binding columns.
  for (SQLSMALLINT i = 0; i < columnsCnt; ++i) {
    ret = SQLBindCol(stmt, i + 1, SQL_C_WCHAR, &columns[i],
                     ODBC_BUFFER_SIZE * sizeof(SQLWCHAR), &columnLens[i]);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "SELECT fieldInt, fieldLong, fieldDecimal128, fieldDouble,"
      "fieldString, fieldBoolean FROM queries_test_005 "
      "where fieldInt = ?");

  ret = SQLPrepare(stmt, request.data(), SQL_NTS);

  SQLLEN ind1 = 1;

  SQLLEN len1 = SQL_DATA_AT_EXEC;

  ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                         100, 100, &ind1, sizeof(ind1), &len1);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLExecute(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NEED_DATA);

  void* oind;

  ret = SQLParamData(stmt, &oind);

  BOOST_REQUIRE_EQUAL(ret, SQL_NEED_DATA);

  int value = 1;
  if (oind == &ind1)
    ret = SQLPutData(stmt, &value, 0);
  else
    BOOST_FAIL("Unknown indicator value");

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLParamData(stmt, &oind);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[0], SQL_NTS, true), "1");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[1], SQL_NTS, true), "2");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[2], SQL_NTS, true), "3");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[3], SQL_NTS, true), "4");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[4], SQL_NTS, true), "5");
  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columns[5], SQL_NTS, true), "6");

  BOOST_CHECK_EQUAL(columnLens[0], 1);
  BOOST_CHECK_EQUAL(columnLens[1], 1);
  BOOST_CHECK_EQUAL(columnLens[2], 1);
  BOOST_CHECK_EQUAL(columnLens[3], 1);
  BOOST_CHECK_EQUAL(columnLens[4], 1);
  BOOST_CHECK_EQUAL(columnLens[5], 1);

  ret = SQLFetch(stmt);
  BOOST_CHECK(ret == SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestNullFields, *disabled()) {
  connectToLocalServer("odbc-test");

  SQLRETURN ret;

  const SQLSMALLINT columnsCnt = 7;

  SQLLEN columnLens[columnsCnt];

  int32_t intColumn;
  int64_t longColumn;
  double decimalColumn;
  double doubleColumn;
  char idColumn[ODBC_BUFFER_SIZE];
  char strColumn[ODBC_BUFFER_SIZE];
  bool boolColumn;

  // Binding columns.
  ret = SQLBindCol(stmt, 1, SQL_C_CHAR, &idColumn, ODBC_BUFFER_SIZE,
                   &columnLens[0]);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLBindCol(stmt, 2, SQL_C_SLONG, &intColumn, 0, &columnLens[1]);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLBindCol(stmt, 3, SQL_C_SBIGINT, &longColumn, 0, &columnLens[2]);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLBindCol(stmt, 4, SQL_C_DOUBLE, &decimalColumn, 0, &columnLens[3]);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLBindCol(stmt, 5, SQL_C_DOUBLE, &doubleColumn, 0, &columnLens[4]);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLBindCol(stmt, 6, SQL_C_CHAR, &strColumn, ODBC_BUFFER_SIZE,
                   &columnLens[5]);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLBindCol(stmt, 7, SQL_C_BIT, &boolColumn, 0, &columnLens[6]);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      "SELECT queries_test_002__id, fieldInt, fieldLong, fieldDecimal128, "
      "fieldDouble,"
      "fieldString, fieldBoolean FROM queries_test_002 order by "
      "queries_test_002__id");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // Fetching the first non-null row.
  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // Checking that columns are not null.
  for (SQLSMALLINT i = 0; i < columnsCnt; ++i)
    BOOST_CHECK_NE(columnLens[i], SQL_NULL_DATA);

  // Fetching null row.
  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // Checking that columns are null.
  for (SQLSMALLINT i = 1; i < columnsCnt; ++i)
    BOOST_CHECK_EQUAL(columnLens[i], SQL_NULL_DATA);

  // Fetching the last non-null row.
  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // Checking that columns are not null.
  for (SQLSMALLINT i = 0; i < columnsCnt; ++i)
    BOOST_CHECK_NE(columnLens[i], SQL_NULL_DATA);

  ret = SQLFetch(stmt);
  BOOST_CHECK(ret == SQL_NO_DATA);
}

// Maybe needed for TS
BOOST_AUTO_TEST_CASE(TestParamsNum, *disabled()) {
  connectToLocalServer("odbc-test");

  CheckParamsNum("SELECT * FROM TestType", 0);
  CheckParamsNum("SELECT * FROM TestType WHERE _key=?", 1);
  CheckParamsNum("SELECT * FROM TestType WHERE _key=? AND _val=?", 2);
}

BOOST_AUTO_TEST_CASE(TestExecuteAfterCursorClose, *disabled()) {
  connectToLocalServer("odbc-test");

  int64_t key = 0;
  SQLWCHAR strField[1024];
  SQLLEN strFieldLen = 0;

  // Binding columns.
  SQLRETURN ret = SQLBindCol(stmt, 1, SQL_C_SLONG, &key, 0, 0);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // Binding columns.
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, &strField, sizeof(strField),
                   &strFieldLen);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // Just selecting everything to make sure everything is OK
  std::vector< SQLWCHAR > selectReq = MakeSqlBuffer(
      "SELECT fieldInt, fieldString FROM queries_test_005 where fieldInt=1");

  ret = SQLPrepare(stmt, selectReq.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLExecute(stmt);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFreeStmt(stmt, SQL_CLOSE);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLExecute(stmt);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(key, 1);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(strField, strFieldLen, true),
                    "5");

  ret = SQLFetch(stmt);

  BOOST_CHECK_EQUAL(ret, SQL_NO_DATA);
}

// TODO: Memory leak, traced by https://bitquill.atlassian.net/browse/AD-813
BOOST_AUTO_TEST_CASE(TestCloseNonFullFetch, *disabled()) {
  connectToLocalServer("odbc-test");

  int64_t key = 0;
  SQLWCHAR strField[1024];
  SQLLEN strFieldLen = 0;

  // Binding columns.
  SQLRETURN ret = SQLBindCol(stmt, 1, SQL_C_SLONG, &key, 0, 0);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // Binding columns.
  ret = SQLBindCol(stmt, 2, SQL_C_WCHAR, &strField, sizeof(strField),
                   &strFieldLen);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // Just selecting everything to make sure everything is OK
  std::vector< SQLWCHAR > selectReq = MakeSqlBuffer(
      "SELECT fieldInt, fieldString FROM queries_test_005 where fieldInt=1");

  ret = SQLExecDirect(stmt, selectReq.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(key, 1);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(strField, strFieldLen, true),
                    "5");
  ret = SQLFreeStmt(stmt, SQL_CLOSE);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

BOOST_AUTO_TEST_CASE(TestErrorMessage, *disabled()) {
  connectToLocalServer("odbc-test");

  // Just selecting everything to make sure everything is OK
  std::vector< SQLWCHAR > selectReq = MakeSqlBuffer("SELECT A FROM B");

  SQLRETURN ret = SQLExecDirect(stmt, selectReq.data(), SQL_NTS);

  BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);

  std::string error = GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt);
  std::string pattern = "Object \"B\" not found";

  if (error.find(pattern) == std::string::npos)
    BOOST_FAIL("'" + error + "' does not match '" + pattern + "'");
}

BOOST_AUTO_TEST_CASE(TestQueryTimeoutQuery, *disabled()) {
  connectToLocalServer("odbc-test");

  SQLRETURN ret = SQLSetStmtAttr(stmt, SQL_ATTR_QUERY_TIMEOUT,
                                 reinterpret_cast< SQLPOINTER >(5), 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  std::vector< SQLWCHAR > selectReq =
      MakeSqlBuffer("SELECT * FROM queries_test_005");

  ret = SQLExecDirect(stmt, selectReq.data(), selectReq.size());

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

BOOST_AUTO_TEST_CASE(TestManyCursors, *disabled()) {
  connectToLocalServer("odbc-test");

  for (int32_t i = 0; i < 100; ++i) {
    std::vector< SQLWCHAR > req =
        MakeSqlBuffer("SELECT * FROM queries_test_005");

    SQLRETURN ret = SQLExecDirect(stmt, req.data(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLFreeStmt(stmt, SQL_CLOSE);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }
}

// TODO: Memory leak, traced by https://bitquill.atlassian.net/browse/AD-813
BOOST_AUTO_TEST_CASE(TestManyCursors2, *disabled()) {
  connectToLocalServer("odbc-test");

  SQLRETURN ret = SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  for (int32_t i = 0; i < 1000; ++i) {
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    std::vector< SQLWCHAR > req =
        MakeSqlBuffer("SELECT fieldInt, fieldString FROM queries_test_005");

    ret = SQLExecDirect(stmt, req.data(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    int32_t res = 0;
    SQLLEN resLen = 0;
    ret = SQLBindCol(stmt, 1, SQL_INTEGER, &res, 0, &resLen);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLFetch(stmt);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_REQUIRE_EQUAL(res, 1);

    ret = SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    stmt = NULL;
  }
}

// Needed for TS?
BOOST_AUTO_TEST_CASE(TestManyCursorsTwoSelects1, *disabled()) {
  connectToLocalServer("odbc-test");

  for (int32_t i = 0; i < 1000; ++i) {
    std::vector< SQLWCHAR > req = MakeSqlBuffer(
        "SELECT * FROM queries_test_005; SELECT * FROM queries_test_004");

    SQLRETURN ret = SQLExecDirect(stmt, req.data(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLFreeStmt(stmt, SQL_CLOSE);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }
}

// Needed for TS?
BOOST_AUTO_TEST_CASE(TestManyCursorsTwoSelects2, *disabled()) {
  connectToLocalServer("odbc-test");

  for (int32_t i = 0; i < 1000; ++i) {
    std::vector< SQLWCHAR > req = MakeSqlBuffer(
        "SELECT * FROM queries_test_005; SELECT * FROM queries_test_004");

    SQLRETURN ret = SQLExecDirect(stmt, req.data(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLMoreResults(stmt);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLFreeStmt(stmt, SQL_CLOSE);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }
}

BOOST_AUTO_TEST_CASE(TestSingleResultUsingGetDataWideChar, *disabled()) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request =
      MakeSqlBuffer("SELECT fieldString FROM \"queries_test_004\"");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  SQLWCHAR fieldString[1024]{};
  SQLLEN fieldString_len = 0;

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, fieldString, sizeof(fieldString),
                   &fieldString_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_NE(SQL_NULL_DATA, fieldString_len);
  BOOST_CHECK_EQUAL(
      u8"你好", utility::SqlWcharToString(fieldString, fieldString_len, true));

  // Fetch 2nd row - not exist
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
}

BOOST_AUTO_TEST_CASE(TestSingleResultSelectWideCharUsingGetDataWideChar,
                     *disabled()) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      u8"SELECT fieldString FROM \"queries_test_004\" WHERE fieldString = "
      u8"'你好'");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLWCHAR fieldString[buf_size]{};
  SQLLEN fieldString_len = 0;

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 1, SQL_C_WCHAR, fieldString, sizeof(fieldString),
                   &fieldString_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_NE(SQL_NULL_DATA, fieldString_len);
  BOOST_CHECK_EQUAL(
      u8"你好", utility::SqlWcharToString(fieldString, fieldString_len, true));

  // Fetch 2nd row - does not exist
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
}

BOOST_AUTO_TEST_CASE(TestSingleResultSelectWideCharUsingGetDataNarrowChar,
                     *disabled()) {
  std::string dsnConnectionString;
  CreateDsnConnectionStringForAWS(dsnConnectionString);
  Connect(dsnConnectionString);
  SQLRETURN ret;
  std::vector< SQLWCHAR > request = MakeSqlBuffer(
      u8"SELECT fieldString FROM \"queries_test_004\" WHERE fieldString = "
      u8"'你好'");

  ret = SQLExecDirect(stmt, request.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  const int32_t buf_size = 1024;
  SQLCHAR fieldString[buf_size]{};
  SQLLEN fieldString_len = 0;

  // Fetch 1st row
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  ret = SQLGetData(stmt, 1, SQL_C_CHAR, fieldString, sizeof(fieldString),
                   &fieldString_len);
  BOOST_CHECK_EQUAL(SQL_SUCCESS, ret);

  BOOST_CHECK_NE(SQL_NULL_DATA, fieldString_len);
  BOOST_CHECK_EQUAL(std::string("??"), std::string((char*)fieldString));

  // Fetch 2nd row - does not exist
  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
}

BOOST_AUTO_TEST_SUITE_END()
