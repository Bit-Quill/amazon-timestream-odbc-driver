﻿/*
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
#include <Windows.h>
#endif

#include <sql.h>
#include <sqlext.h>

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#include "complex_type.h"
#include "ignite/odbc/common/fixed_size_array.h"
#include "ignite/odbc/impl/binary/binary_utils.h"
#include "ignite/odbc/type_traits.h"
#include "ignite/odbc/utility.h"
#include "odbc_test_suite.h"
#include "test_type.h"
#include "test_utils.h"

using namespace ignite;
using namespace ignite::odbc::common;
using namespace ignite_test;
using namespace ignite::odbc::binary;
using namespace ignite::odbc::impl::binary;
using namespace ignite::odbc::impl::interop;
using ignite::odbc::TestType;
using namespace ignite::odbc::type_traits;

using namespace boost::unit_test;

#ifdef __APPLE__
constexpr auto FUNCTION_SEQUENCE_ERROR_STATE = "S1010";
#endif

constexpr auto INVALID_CURSOR_STATE = "24000";

/**
 * Test setup fixture.
 */
struct MetaQueriesTestSuiteFixture : public odbc::OdbcTestSuite {
  const static SQLLEN C_STR_LEN_DEFAULT = 1024;

  /**
   * Converts SQLCHAR[] to std::string
   *
   * @param strBuf SQLCHAR pointer
   * @return buf std::string
   */
  std::string SqlCharToString(SQLCHAR *strBuf) {
    std::stringstream bufStream;
    bufStream << strBuf;
    std::string buf;
    bufStream >> buf;

    return buf;
  }

  /**
   * Checks single row result set for correct work with SQLGetData.
   *
   * @param stmt Statement.
   */
  void CheckSingleRowResultSetWithGetData(
      SQLHSTMT stmt, SQLUSMALLINT columnIndex = 1,
      const std::string expectedValue = "",
      bool checkOtherValEmpty = false,
#ifdef __APPLE__
      const std::string expectedErrorState = FUNCTION_SEQUENCE_ERROR_STATE) const {
#else
      const std::string expectedErrorState = INVALID_CURSOR_STATE) const {
#endif
    SQLRETURN ret = SQLFetch(stmt);

    if (!SQL_SUCCEEDED(ret)) {
      BOOST_CHECK(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);

      std::string sqlMessage = GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt);
      if (sqlMessage.empty()) {
        sqlMessage.append("SQLFetch returned: " + std::to_string(ret));
      }  
      // Handle the case of ret equals SQL_NO_DATA
      if (ret == SQL_NO_DATA) {
        sqlMessage = "SQL_NO_DATA is returned from SQLFetch. " + sqlMessage;
      }
      BOOST_FAIL(sqlMessage);
    }

    SQLWCHAR buf[1024];
    SQLLEN bufLen = sizeof(buf);

    columnIndex = columnIndex >= 1 ? columnIndex : 1;
    for (int i = 1; i <= columnIndex; i++) {
      ret = SQLGetData(stmt, i, SQL_C_WCHAR, buf, sizeof(buf), &bufLen);

      if (!SQL_SUCCEEDED(ret))
        BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

      std::string actualValueStr = utility::SqlWcharToString(buf, bufLen);
      if (i == columnIndex && !expectedValue.empty()) {
        BOOST_CHECK_EQUAL(expectedValue, actualValueStr);
      } else if (checkOtherValEmpty) {
        // check that values at other column indeces are empty strings
        BOOST_CHECK_EQUAL("", actualValueStr);
      }
    }

    ret = SQLFetch(stmt);

    BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

    ret = SQLGetData(stmt, 1, SQL_C_WCHAR, buf, sizeof(buf), &bufLen);

    BOOST_REQUIRE_EQUAL(ret, SQL_ERROR);
    BOOST_CHECK_EQUAL(GetOdbcErrorState(SQL_HANDLE_STMT, stmt),
                      expectedErrorState);
  }

  /**
   * Check string column.
   *
   * @param stmt Statement.
   * @param colId Column ID to check.
   * @param value Expected value.
   */
  void CheckStringColumn(SQLHSTMT stmt, int colId, const std::string &value) {
    char buf[1024];
    SQLLEN bufLen = sizeof(buf);

    SQLRETURN ret =
        SQLGetData(stmt, colId, SQL_C_CHAR, buf, sizeof(buf), &bufLen);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    if (bufLen <= 0)
      BOOST_CHECK(value.empty());
    else
      BOOST_CHECK_EQUAL(std::string(buf, static_cast< size_t >(bufLen)), value);
  }

  /**
   * Constructor.
   */
  MetaQueriesTestSuiteFixture() = default;

  bool WasNull(const SQLLEN length) const {
    return (length == SQL_NULL_DATA);
  }

  void SQLColumnsBindColumns(
      SQLHSTMT stmt, char table_cat[], SQLLEN &table_cat_len,
      char table_schem[], SQLLEN &table_schem_len, char table_name[],
      SQLLEN &table_name_len, char column_name[], SQLLEN &column_name_len,
      SQLSMALLINT &data_type, SQLLEN &data_type_len, char type_name[],
      SQLLEN &type_name_len, SQLINTEGER &column_size, SQLLEN &column_size_len,
      SQLINTEGER &buffer_length, SQLLEN &buffer_length_len,
      SQLSMALLINT &decimal_digits, SQLLEN &decimal_digits_len,
      SQLSMALLINT &num_prec_radix, SQLLEN &num_prec_radix_len,
      SQLSMALLINT &nullable, SQLLEN &nullable_len, char remarks[],
      SQLLEN &remarks_len, char column_def[], SQLLEN &column_def_len,
      SQLSMALLINT &sql_data_type, SQLLEN &sql_data_type_len,
      SQLSMALLINT &sql_datetime_sub, SQLLEN &sql_datetime_sub_len,
      SQLINTEGER &char_octet_length, SQLLEN &char_octet_length_len,
      SQLINTEGER &ordinal_position, SQLLEN &ordinal_position_len,
      char is_nullable[], SQLLEN &is_nullable_len) {
    SQLRETURN ret = SQL_SUCCESS;
    ret = SQLBindCol(stmt, 1, SQL_C_CHAR, table_cat, C_STR_LEN_DEFAULT,
                     &table_cat_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 2, SQL_C_CHAR, table_schem, C_STR_LEN_DEFAULT,
                     &table_schem_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 3, SQL_C_CHAR, table_name, C_STR_LEN_DEFAULT,
                     &table_name_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 4, SQL_C_CHAR, column_name, C_STR_LEN_DEFAULT,
                     &column_name_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 5, SQL_SMALLINT, &data_type, sizeof(data_type),
                     &data_type_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 6, SQL_C_CHAR, type_name, C_STR_LEN_DEFAULT,
                     &type_name_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 7, SQL_INTEGER, &column_size, sizeof(column_size),
                     &column_size_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 8, SQL_INTEGER, &buffer_length,
                     sizeof(buffer_length), &buffer_length_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 9, SQL_SMALLINT, &decimal_digits,
                     sizeof(decimal_digits), &decimal_digits_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 10, SQL_SMALLINT, &num_prec_radix,
                     sizeof(num_prec_radix), &num_prec_radix_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 11, SQL_SMALLINT, &nullable, sizeof(nullable),
                     &nullable_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 12, SQL_C_CHAR, remarks, C_STR_LEN_DEFAULT,
                     &remarks_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 13, SQL_C_CHAR, column_def, C_STR_LEN_DEFAULT,
                     &column_def_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 14, SQL_SMALLINT, &sql_data_type,
                     sizeof(sql_data_type), &sql_data_type_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 15, SQL_SMALLINT, &sql_datetime_sub,
                     sizeof(sql_datetime_sub), &sql_datetime_sub_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 16, SQL_INTEGER, &char_octet_length,
                     sizeof(char_octet_length), &char_octet_length_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 17, SQL_INTEGER, &ordinal_position,
                     sizeof(ordinal_position), &ordinal_position_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
    ret = SQLBindCol(stmt, 18, SQL_C_CHAR, is_nullable, C_STR_LEN_DEFAULT,
                     &is_nullable_len);
    BOOST_CHECK(SQL_SUCCEEDED(ret));
  }

  /**
   * Check attribute using SQLColAttribute.
   * The value returned from SQLColAttribute should match the expected value.
   *
   * @param stmt Statement.
   * @param query SQL Query.
   * @param fieldId Field Identifier.
   * @param expectedVal Expected string data.
   */
  void callSQLColAttribute(SQLHSTMT stmt, const SQLCHAR *query,
                           SQLSMALLINT fieldId,
                           const std::string &expectedVal) {
    SQLWCHAR strBuf[1024];
    std::vector< SQLWCHAR > wQuery =
        MakeSqlBuffer(reinterpret_cast< const char * >(query));

    SQLRETURN ret = SQLExecDirect(stmt, wQuery.data(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA)
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLColAttribute(stmt, 1, fieldId, strBuf, sizeof(strBuf),
                                    nullptr, nullptr);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    std::string buf = utility::SqlWcharToString(strBuf);

    BOOST_CHECK(expectedVal == buf);
  }

  /**
   * Check attribute using SQLColAttribute.
   * The value returned from SQLColAttribute should match the expected value.
   *
   * @param stmt Statement.
   * @param query SQL Query.
   * @param fieldId Field Identifier.
   * @param expectedVal Expected int data.
   */
  void callSQLColAttribute(SQLHSTMT stmt, const SQLCHAR *query,
                           SQLSMALLINT fieldId, const int &expectedVal) {
    SQLLEN intVal;
    std::vector< SQLWCHAR > wQuery =
        MakeSqlBuffer(reinterpret_cast< const char * >(query));

    SQLRETURN ret = SQLExecDirect(stmt, wQuery.data(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA)
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret =
        SQLColAttribute(stmt, 1, fieldId, nullptr, 0, nullptr, &intVal);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(intVal, expectedVal);
  }

  /**
   * Check result set column metadata using SQLDescribeCol.
   *
   * @param stmt Statement.
   * @param idx Index.
   * @param expName Expected name.
   * @param expDataType Expected data type.
   * @param expSize Expected column size.
   * @param expScale Expected column scale.
   * @param expNullability expected nullability.
   */
  void CheckColumnMetaWithSQLDescribeCol(SQLHSTMT stmt, SQLUSMALLINT idx,
                                         const std::string &expName,
                                         SQLSMALLINT expDataType,
                                         SQLULEN expSize, SQLSMALLINT expScale,
                                         SQLSMALLINT expNullability) const {
    std::vector< SQLWCHAR > name(ODBC_BUFFER_SIZE);
    SQLSMALLINT nameLen = 0;
    SQLSMALLINT dataType = 0;
    SQLULEN size;
    SQLSMALLINT scale;
    SQLSMALLINT nullability;

    SQLRETURN ret = SQLDescribeCol(
        stmt, idx, &name[0], (SQLSMALLINT)name.size(),
        &nameLen, &dataType, &size, &scale, &nullability);
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    BOOST_CHECK_GE(nameLen, 0);
    BOOST_CHECK_LE(nameLen, static_cast< SQLSMALLINT >(ODBC_BUFFER_SIZE));

    BOOST_CHECK_EQUAL(utility::SqlWcharToString(name.data()), expName);
    BOOST_CHECK_EQUAL(dataType, expDataType);
    BOOST_CHECK_EQUAL(size, expSize);
    BOOST_CHECK_EQUAL(scale, expScale);
    BOOST_CHECK_EQUAL(nullability, expNullability);
  }

  /**
   * @param func Function to call before tests. May be PrepareQuery or
   * ExecQuery.
   *
   * 1. Start node.
   * 2. Connect to node using ODBC.
   * 3. Create table with decimal and char columns with specified size and
   * scale.
   * 4. Execute or prepare statement.
   * 5. Check presicion and scale of every column using SQLDescribeCol.
   */
  template < typename F >
  void CheckSQLDescribeColPrecisionAndScale(F func) {
    ConnectToTS();

    SQLRETURN ret = (this->*func)(
        "select meta_queries_test_001__id, fieldDecimal128, fieldDouble, "
        "fieldString, fieldObjectId,"
        "fieldBoolean, fieldDate, fieldInt, fieldBinary from "
        "meta_queries_test_001");
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    SQLSMALLINT columnCount = 0;

    ret = SQLNumResultCols(stmt, &columnCount);
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    BOOST_CHECK_EQUAL(columnCount, 9);

    CheckColumnMetaWithSQLDescribeCol(stmt, 1, "meta_queries_test_001__id",
                                      SQL_WVARCHAR, SQL_NO_TOTAL, -1,
                                      SQL_NO_NULLS);
    CheckColumnMetaWithSQLDescribeCol(stmt, 2, "fieldDecimal128", SQL_DECIMAL,
                                      SQL_NO_TOTAL, -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLDescribeCol(stmt, 3, "fieldDouble", SQL_DOUBLE, 15,
                                      -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLDescribeCol(stmt, 4, "fieldString", SQL_WVARCHAR,
                                      SQL_NO_TOTAL, -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLDescribeCol(stmt, 5, "fieldObjectId", SQL_WVARCHAR,
                                      SQL_NO_TOTAL, -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLDescribeCol(stmt, 6, "fieldBoolean", SQL_BIT, 1, -1,
                                      SQL_NULLABLE);
    CheckColumnMetaWithSQLDescribeCol(stmt, 7, "fieldDate", SQL_TYPE_TIMESTAMP,
                                      19, -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLDescribeCol(stmt, 8, "fieldInt", SQL_INTEGER, 10, 0,
                                      SQL_NULLABLE);
    CheckColumnMetaWithSQLDescribeCol(stmt, 9, "fieldBinary", SQL_VARBINARY,
                                      SQL_NO_TOTAL, -1, SQL_NULLABLE);
  }

  /**
   * Check result set column metadata using SQLColAttribute.
   *
   * @param stmt Statement.
   * @param idx Index.
   * @param expName Expected name.
   * @param expDataType Expected data type.
   * @param expSize Expected column size.
   * @param expScale Expected column scale.
   * @param expNullability expected nullability.
   */
  void CheckColumnMetaWithSQLColAttribute(SQLHSTMT stmt, SQLUSMALLINT idx,
                                          const std::string &expName,
                                          SQLLEN expDataType, SQLULEN expSize,
                                          SQLLEN expScale,
                                          SQLLEN expNullability) {
    std::vector< SQLWCHAR > name(ODBC_BUFFER_SIZE);
    SQLSMALLINT nameLen = 0;
    SQLLEN dataType = 0;
    SQLLEN size;
    SQLLEN scale;
    SQLLEN nullability;

    SQLRETURN ret = SQLColAttribute(stmt, idx, SQL_DESC_NAME, &name[0],
                                    (SQLSMALLINT)name.size() * sizeof(SQLWCHAR),
                                    &nameLen, nullptr);
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    ret = SQLColAttribute(stmt, idx, SQL_DESC_TYPE, 0, 0, 0, &dataType);
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    ret = SQLColAttribute(stmt, idx, SQL_DESC_PRECISION, 0, 0, 0, &size);
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    ret = SQLColAttribute(stmt, idx, SQL_DESC_SCALE, 0, 0, 0, &scale);
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    ret = SQLColAttribute(stmt, idx, SQL_DESC_NULLABLE, 0, 0, 0, &nullability);
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    BOOST_CHECK_GE(nameLen, 0);
    BOOST_CHECK_LE(nameLen, static_cast< SQLSMALLINT >(ODBC_BUFFER_SIZE));

    BOOST_CHECK_EQUAL(utility::SqlWcharToString(name.data()), expName);
    BOOST_CHECK_EQUAL(dataType, expDataType);
    BOOST_CHECK_EQUAL(size, expSize);
    BOOST_CHECK_EQUAL(scale, expScale);
    BOOST_CHECK_EQUAL(nullability, expNullability);
  }

  /**
   * @param func Function to call before tests. May be PrepareQuery or
   * ExecQuery.
   *
   * 1. Start node.
   * 2. Connect to node using ODBC.
   * 3. Create table with decimal and char columns with specified size and
   * scale.
   * 4. Execute or prepare statement.
   * 5. Check presicion and scale of every column using SQLColAttribute.
   */
  template < typename F >
  void CheckSQLColAttributePrecisionAndScale(F func) {
    ConnectToTS();

    SQLRETURN ret = (this->*func)(
        "select meta_queries_test_001__id, fieldDecimal128, fieldDouble, "
        "fieldString, fieldObjectId,"
        "fieldBoolean, fieldDate, fieldInt, fieldBinary from "
        "meta_queries_test_001");
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    SQLSMALLINT columnCount = 0;

    ret = SQLNumResultCols(stmt, &columnCount);
    ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

    BOOST_CHECK_EQUAL(columnCount, 9);

    CheckColumnMetaWithSQLColAttribute(stmt, 1, "meta_queries_test_001__id",
                                       SQL_WVARCHAR, SQL_NO_TOTAL, -1,
                                       SQL_NO_NULLS);
    CheckColumnMetaWithSQLColAttribute(stmt, 2, "fieldDecimal128", SQL_DECIMAL,
                                       SQL_NO_TOTAL, -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLColAttribute(stmt, 3, "fieldDouble", SQL_DOUBLE, 15,
                                       -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLColAttribute(stmt, 4, "fieldString", SQL_WVARCHAR,
                                       SQL_NO_TOTAL, -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLColAttribute(stmt, 5, "fieldObjectId", SQL_WVARCHAR,
                                       SQL_NO_TOTAL, -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLColAttribute(stmt, 6, "fieldBoolean", SQL_BIT, 1, -1,
                                       SQL_NULLABLE);
    CheckColumnMetaWithSQLColAttribute(stmt, 7, "fieldDate", SQL_TYPE_TIMESTAMP,
                                       19, -1, SQL_NULLABLE);
    CheckColumnMetaWithSQLColAttribute(stmt, 8, "fieldInt", SQL_INTEGER, 10, 0,
                                       SQL_NULLABLE);
    CheckColumnMetaWithSQLColAttribute(stmt, 9, "fieldBinary", SQL_VARBINARY,
                                       SQL_NO_TOTAL, -1, SQL_NULLABLE);
  }

  // check SQLGetTypeInfo result 1st and 2nd column
  void CheckSQLGetTypeInfoResult(const std::string& exptectedTypeName, int expectedDataType) {
    SQLRETURN ret = SQLFetch(stmt);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    SQLWCHAR buf[1024];
    SQLLEN bufLen = sizeof(buf);

    ret = SQLGetData(stmt, 1, SQL_C_WCHAR, buf, sizeof(buf), &bufLen);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    std::string actualValueStr = utility::SqlWcharToString(buf, bufLen);
    BOOST_CHECK_EQUAL(exptectedTypeName, actualValueStr);

    SQLSMALLINT dataType;
    SQLLEN dataTypeLen;
    ret = SQLGetData(stmt, 2, SQL_SMALLINT, &dataType, sizeof(dataType),
                     &dataTypeLen);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(expectedDataType, dataType);
  }

  /**
   * Destructor.
   */
  ~MetaQueriesTestSuiteFixture() {
    // No-op.
  }
};

BOOST_FIXTURE_TEST_SUITE(MetaQueriesTestSuite, MetaQueriesTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestGetTypeInfoAllTypes) {
  ConnectToTS();

  SQLRETURN ret = SQLGetTypeInfo(stmt, SQL_ALL_TYPES);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  CheckSQLGetTypeInfoResult("VARCHAR", SQL_VARCHAR);
  CheckSQLGetTypeInfoResult("BIT", SQL_BIT);
  CheckSQLGetTypeInfoResult("BIGINT", SQL_BIGINT);
  CheckSQLGetTypeInfoResult("DOUBLE", SQL_DOUBLE);
  CheckSQLGetTypeInfoResult("TIMESTAMP", SQL_TYPE_TIMESTAMP);
  CheckSQLGetTypeInfoResult("DATE", SQL_TYPE_DATE);
  CheckSQLGetTypeInfoResult("TIME", SQL_TYPE_TIME);
  CheckSQLGetTypeInfoResult("INTERVAL_DAY_TO_SECOND",
                            SQL_INTERVAL_DAY_TO_SECOND);
  CheckSQLGetTypeInfoResult("INTERVAL_YEAR_TO_MONTH",
                            SQL_INTERVAL_YEAR_TO_MONTH);
  CheckSQLGetTypeInfoResult("INTEGER", SQL_INTEGER); 
  CheckSQLGetTypeInfoResult("NOT_SET", SQL_VARCHAR);
  CheckSQLGetTypeInfoResult("UNKNOWN", SQL_VARCHAR);
}

BOOST_AUTO_TEST_CASE(TestDateTypeColumnAttributeLiteral, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select DATE '2020-10-25' from meta_queries_test_001");
  SQLExecDirect(stmt, req.data(), SQL_NTS);

  SQLLEN intVal = 0;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_DESC_TYPE, 0, 0, 0, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, SQL_TYPE_DATE);
}

BOOST_AUTO_TEST_CASE(TestDateTypeColumnAttributeField, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req = MakeSqlBuffer(
      "select CAST (fieldDate as DATE) from meta_queries_test_001");
  SQLExecDirect(stmt, req.data(), SQL_NTS);

  SQLLEN intVal = 0;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_DESC_TYPE, 0, 0, 0, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, SQL_TYPE_DATE);
}

BOOST_AUTO_TEST_CASE(TestTimeTypeColumnAttributeLiteral, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select TIME '12:42:13' from meta_queries_test_001");
  SQLExecDirect(stmt, req.data(), SQL_NTS);

  SQLLEN intVal = 0;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_DESC_TYPE, 0, 0, 0, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, SQL_TYPE_TIME);
}

BOOST_AUTO_TEST_CASE(TestTimeTypeColumnAttributeField, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select fieldDate from meta_queries_test_001");
  SQLExecDirect(stmt, req.data(), SQL_NTS);

  SQLLEN intVal = 0;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_DESC_TYPE, 0, 0, 0, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, SQL_TYPE_TIMESTAMP);
}

BOOST_AUTO_TEST_CASE(TestColAttributesColumnLength, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select fieldInt from meta_queries_test_001");
  SQLExecDirect(stmt, req.data(), SQL_NTS);

  SQLLEN intVal;
  SQLWCHAR strBuf[1024];
  SQLSMALLINT strLen;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_COLUMN_LENGTH, strBuf,
                                  sizeof(strBuf), &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, 4);
}

BOOST_AUTO_TEST_CASE(TestColAttributesColumnPresicion, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select fieldInt from meta_queries_test_001");
  SQLExecDirect(stmt, req.data(), SQL_NTS);

  SQLLEN intVal;
  SQLWCHAR strBuf[1024];
  SQLSMALLINT strLen;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_COLUMN_PRECISION, strBuf,
                                  sizeof(strBuf), &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, 10);
}

BOOST_AUTO_TEST_CASE(TestColAttributeWithOneTable) {
  ConnectToTS();

  std::pair< int16_t, std::string > tests[] = {
      std::make_pair(SQL_VARCHAR, std::string("fleet")),
      std::make_pair(SQL_VARCHAR, std::string("truck_id")),
      std::make_pair(SQL_VARCHAR, std::string("fuel_capacity")),
      std::make_pair(SQL_VARCHAR, std::string("model")),
      std::make_pair(SQL_VARCHAR, std::string("load_capacity")),
      std::make_pair(SQL_VARCHAR, std::string("make")),
      std::make_pair(SQL_VARCHAR, std::string("measure_name")),
      std::make_pair(SQL_TYPE_TIMESTAMP, std::string("time")),
      std::make_pair(SQL_DOUBLE, std::string("load")),
      std::make_pair(SQL_DOUBLE, std::string("fuel-reading")),
      std::make_pair(SQL_VARCHAR, std::string("location")),
      std::make_pair(SQL_DOUBLE, std::string("speed"))};

  int numTests = sizeof(tests) / sizeof(std::pair< int16_t, std::string >);

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("SELECT * FROM meta_queries_test_db.IoTMulti");
  SQLLEN intVal;
  SQLSMALLINT strLen;
  SQLWCHAR strBuf[1024];

  SQLRETURN ret = SQLExecDirect(stmt, req.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA)
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  for (int i = 1; i <= numTests; i++) {
    ret = SQLColAttribute(stmt, SQLUSMALLINT(i), SQL_DESC_TYPE,
                                    nullptr, 0, nullptr, &intVal);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(intVal, tests[i - 1].first);

    ret = SQLColAttribute(stmt, SQLUSMALLINT(i), SQL_DESC_NAME, strBuf,
                          sizeof(strBuf), &strLen, &intVal);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(utility::SqlWcharToString(strBuf), tests[i - 1].second);
  }
}

// [AT-1146] TODO add/enable SQLColAttribute tests
// https://bitquill.atlassian.net/browse/AT-1146
BOOST_AUTO_TEST_CASE(TestColAttributeDataTypesAndColumnNames, *disabled()) {
  ConnectToTS();

  std::pair< int16_t, std::string > tests[] = {
      std::make_pair(SQL_WVARCHAR, std::string("meta_queries_test_002__id")),
      std::make_pair(SQL_DECIMAL, std::string("fieldDecimal128")),
      std::make_pair(SQL_DOUBLE, std::string("fieldFloat")),
      // our ODBC driver identifies float fields as double by default
      std::make_pair(SQL_DOUBLE, std::string("fieldDouble")),
      std::make_pair(SQL_WVARCHAR, std::string("fieldString")),
      std::make_pair(SQL_WVARCHAR, std::string("fieldObjectId")),
      std::make_pair(SQL_BIT, std::string("fieldBoolean")),
      std::make_pair(SQL_TYPE_TIMESTAMP, std::string("fieldDate")),
      std::make_pair(SQL_INTEGER, std::string("fieldInt")),
      std::make_pair(SQL_DOUBLE, std::string("fieldLong")),
      std::make_pair(SQL_WVARCHAR, std::string("fieldMaxKey")),
      std::make_pair(SQL_WVARCHAR, std::string("fieldMinKey")),
      std::make_pair(SQL_TYPE_NULL, std::string("fieldNull")),
      std::make_pair(SQL_VARBINARY, std::string("fieldBinary"))};

  int numTests = sizeof(tests) / sizeof(std::pair< int16_t, std::string >);

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select * from meta_queries_test_002");
  SQLLEN intVal;
  SQLSMALLINT strLen;
  SQLWCHAR strBuf[1024];

  SQLExecDirect(stmt, req.data(), SQL_NTS);

  for (int i = 1; i <= numTests; i++) {
    // TODO remove below if statement when bug from JDBC (AD-765) is fixed.
    // https://bitquill.atlassian.net/browse/AD-766
    // the fieldNull pair is the 13th pair
    if (i == 13)
      continue;

    SQLRETURN ret = SQLColAttribute(stmt, SQLUSMALLINT(i), SQL_DESC_TYPE,
                                    nullptr, 0, nullptr, &intVal);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(intVal, tests[i - 1].first);

    ret = SQLColAttribute(stmt, SQLUSMALLINT(i), SQL_DESC_NAME, strBuf,
                          sizeof(strBuf), &strLen, &intVal);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    BOOST_CHECK_EQUAL(utility::SqlWcharToString(strBuf), tests[i - 1].second);
  }
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescAutoUniqueValue) {
  ConnectToTS();

  const SQLCHAR req[] = "select load from meta_queries_test_db.IoTMulti";

  // only "NO" is returned for IS_AUTOINCREMENT field
  callSQLColAttribute(stmt, req, SQL_DESC_AUTO_UNIQUE_VALUE, SQL_FALSE);
}

// TODO [AT-1146] enable this test on macOS
// https://bitquill.atlassian.net/browse/AT-1146
#if !defined(__APPLE__)
BOOST_AUTO_TEST_CASE(TestColAttributeDescBaseColumnName) {
  ConnectToTS();

  const SQLCHAR req[] = "select \"fuel-reading\" from meta_queries_test_db.IoTMulti";

  callSQLColAttribute(stmt, req, SQL_DESC_BASE_COLUMN_NAME,
                      std::string("fuel-reading"));
}
#endif // #if !defined(__APPLE__)

BOOST_AUTO_TEST_CASE(TestColAttributeDescBaseTableName) {
  ConnectToTS();

  const SQLCHAR req[] = "select time from meta_queries_test_db.IoTMulti";

  // Table names are empty
  callSQLColAttribute(stmt, req, SQL_DESC_BASE_TABLE_NAME,
                      std::string(""));
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescCaseSensitive) {
  ConnectToTS();

  // test that case sensitive returns true for string field.
  const SQLCHAR req1[] = "select location from meta_queries_test_db.IoTMulti";

  callSQLColAttribute(stmt, req1, SQL_DESC_CASE_SENSITIVE, SQL_TRUE);

  // test that case sensitive returns false for int field.
  const SQLCHAR req2[] = "select speed from meta_queries_test_db.IoTMulti";

  callSQLColAttribute(stmt, req2, SQL_DESC_CASE_SENSITIVE, SQL_FALSE);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescCatalogName) {
  ConnectToTS();

  const SQLCHAR req[] = "select time from meta_queries_test_db.IoTMulti";

  // check that catalog should be empty
  callSQLColAttribute(stmt, req, SQL_DESC_CATALOG_NAME, std::string(""));
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescConciseType) {
  ConnectToTS();

  const SQLCHAR req1[] = "select hostname from meta_queries_test_db.DevOpsMulti";

  callSQLColAttribute(stmt, req1, SQL_DESC_CONCISE_TYPE, SQL_VARCHAR);

  const SQLCHAR req2[] = "select time from meta_queries_test_db.DevOpsMulti";

  callSQLColAttribute(stmt, req2, SQL_DESC_CONCISE_TYPE, SQL_TYPE_TIMESTAMP);

  const SQLCHAR req3[] = "select memory_utilization from meta_queries_test_db.DevOpsMulti";

  callSQLColAttribute(stmt, req3, SQL_DESC_CONCISE_TYPE, SQL_DOUBLE);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescCount) {
  ConnectToTS();

  const SQLCHAR req[] = "select hostname from meta_queries_test_db.DevOpsMulti";

  // count should be 1
  callSQLColAttribute(stmt, req, SQL_DESC_COUNT, 1);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescDisplaySize, *disabled()) {
  ConnectToTS();

  const SQLCHAR req1[] = "select fieldBinary from meta_queries_test_001";

  // SQL_VARBINARY should have display size SQL_NO_TOTAL
  callSQLColAttribute(stmt, req1, SQL_DESC_DISPLAY_SIZE, SQL_NO_TOTAL);

  const SQLCHAR req2[] = "select fieldInt from meta_queries_test_001";

  // SQL_INTEGER should have display size 11
  callSQLColAttribute(stmt, req2, SQL_DESC_DISPLAY_SIZE, 11);

  const SQLCHAR req3[] = "select fieldLong from meta_queries_test_001";

  // SQL_BIGINT should have display size 20
  callSQLColAttribute(stmt, req3, SQL_DESC_DISPLAY_SIZE, 20);

  const SQLCHAR req4[] = "select fieldDouble from meta_queries_test_001";

  // SQL_DOUBLE should have display size 24
  callSQLColAttribute(stmt, req4, SQL_DESC_DISPLAY_SIZE, 24);

  const SQLCHAR req5[] = "select fieldDate from meta_queries_test_001";

  // SQL_TYPE_TIMESTAMP should have display size 19
  callSQLColAttribute(stmt, req5, SQL_DESC_DISPLAY_SIZE, 19);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescFixedPrecScale) {
  ConnectToTS();

  const SQLCHAR req[] = "select speed from meta_queries_test_db.IoTMulti";

  // only SQL_FALSE is returned
  callSQLColAttribute(stmt, req, SQL_DESC_FIXED_PREC_SCALE, SQL_FALSE);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescLabel, *disabled()) {
  ConnectToTS();

  const SQLCHAR req[] = "select fieldBoolean from meta_queries_test_002";

  callSQLColAttribute(stmt, req, SQL_DESC_LABEL, std::string("fieldBoolean"));
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescLength, *disabled()) {
  ConnectToTS();

  const SQLCHAR req1[] = "select fieldString from meta_queries_test_002";

  // SQL_WVARCHAR should have length SQL_NO_TOTAL
  callSQLColAttribute(stmt, req1, SQL_DESC_LENGTH, SQL_NO_TOTAL);

  const SQLCHAR req2[] = "select fieldInt from meta_queries_test_002";

  // SQL_INTEGER should have length 4
  callSQLColAttribute(stmt, req2, SQL_DESC_LENGTH, 4);

  const SQLCHAR req3[] = "select fieldLong from meta_queries_test_002";

  // SQL_BIGINT should have length 8
  callSQLColAttribute(stmt, req3, SQL_DESC_LENGTH, 8);

  const SQLCHAR req4[] = "select fieldDouble from meta_queries_test_002";

  // SQL_DOUBLE should have length 8
  callSQLColAttribute(stmt, req4, SQL_DESC_LENGTH, 8);

  const SQLCHAR req5[] = "select fieldDate from meta_queries_test_002";

  // SQL_TYPE_TIMESTAMP should have length 16
  callSQLColAttribute(stmt, req5, SQL_DESC_LENGTH, 16);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescLiteralPrefix, *disabled()) {
  ConnectToTS();

  // test that empty string is returned for non-char and non-binary type
  const SQLCHAR req1[] = "select fieldDouble from meta_queries_test_001";

  callSQLColAttribute(stmt, req1, SQL_DESC_LITERAL_PREFIX, std::string(""));

  // test that "'" is returned for *CHAR type
  const SQLCHAR req2[] = "select fieldString from meta_queries_test_002";

  callSQLColAttribute(stmt, req2, SQL_DESC_LITERAL_PREFIX, std::string("'"));

  // test that "0x" is returned for *CHAR type
  const SQLCHAR req3[] = "select fieldBinary from meta_queries_test_002";

  callSQLColAttribute(stmt, req3, SQL_DESC_LITERAL_PREFIX, std::string("0x"));
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescLiteralSuffix, *disabled()) {
  ConnectToTS();

  // test that empty string is returned for non-char and non-binary type
  const SQLCHAR req1[] = "select fieldBoolean from meta_queries_test_001";

  callSQLColAttribute(stmt, req1, SQL_DESC_LITERAL_SUFFIX, std::string(""));

  // test that "'" is returned for *CHAR type
  const SQLCHAR req2[] = "select fieldString from meta_queries_test_002";

  callSQLColAttribute(stmt, req2, SQL_DESC_LITERAL_SUFFIX, std::string("'"));
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescLocalTypeName, *disabled()) {
  ConnectToTS();

  const SQLCHAR req1[] = "select fieldDouble from meta_queries_test_001";

  // SQL_DOUBLE should have type name SqlTypeName::DOUBLE
  callSQLColAttribute(stmt, req1, SQL_DESC_LOCAL_TYPE_NAME,
                      SqlTypeName::DOUBLE);

  const SQLCHAR req2[] = "select fieldString from meta_queries_test_002";

  // SQL_WVARCHAR should have type name SqlTypeName::VARCHAR
  callSQLColAttribute(stmt, req2, SQL_DESC_LOCAL_TYPE_NAME,
                      SqlTypeName::VARCHAR);

  const SQLCHAR req3[] = "select fieldBinary from meta_queries_test_002";

  // SQL_BINARY should have type name SqlTypeName::VARCHAR
  callSQLColAttribute(stmt, req3, SQL_DESC_LOCAL_TYPE_NAME,
                      SqlTypeName::VARCHAR);

  const SQLCHAR req4[] = "select fieldDate from meta_queries_test_002";

  // SQL_TYPE_TIMESTAMP should have type name SqlTypeName::TIMESTAMP
  callSQLColAttribute(stmt, req4, SQL_DESC_LOCAL_TYPE_NAME,
                      SqlTypeName::TIMESTAMP);

  const SQLCHAR req5[] = "select fieldInt from meta_queries_test_002";

  // SQL_INTEGER should have type name SqlTypeName::INTEGER
  callSQLColAttribute(stmt, req5, SQL_DESC_LOCAL_TYPE_NAME,
                      SqlTypeName::INTEGER);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescName, *disabled()) {
  ConnectToTS();

  const SQLCHAR req[] = "select field from meta_queries_test_002_with_array";

  callSQLColAttribute(stmt, req, SQL_DESC_NAME, std::string("field"));
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescNullable, *disabled()) {
  ConnectToTS();

  // test meta_queries_test_001__id (a primary key) should not be nullable
  const SQLCHAR req1[] =
      "select meta_queries_test_001__id from meta_queries_test_001";

  callSQLColAttribute(stmt, req1, SQL_DESC_NULLABLE, SQL_NO_NULLS);

  // test non-primary key field should be nullable.
  const SQLCHAR req2[] = "select fieldNull from meta_queries_test_001";

  callSQLColAttribute(stmt, req2, SQL_DESC_NULLABLE, SQL_NULLABLE);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescNumPrecRadix, *disabled()) {
  ConnectToTS();

  const SQLCHAR req1[] = "select fieldFloat from meta_queries_test_002";

  // SQL_FLOAT should have precision radix 2
  callSQLColAttribute(stmt, req1, SQL_DESC_NUM_PREC_RADIX, 2);

  const SQLCHAR req2[] = "select fieldInt from meta_queries_test_001";

  // SQL_INT should have precision radix 10
  callSQLColAttribute(stmt, req2, SQL_DESC_NUM_PREC_RADIX, 10);

  const SQLCHAR req3[] = "select fieldString from meta_queries_test_002";

  // SQL_WVARCHAR (non-numeric type) should have precision radix 0
  callSQLColAttribute(stmt, req3, SQL_DESC_NUM_PREC_RADIX, 0);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescOctetLength, *disabled()) {
  ConnectToTS();

  size_t size_of_char = sizeof(char);

  const SQLCHAR req1[] = "select fieldString from meta_queries_test_002";

  // SQL_WVARCHAR should have octet length SQL_NO_TOTAL
  callSQLColAttribute(stmt, req1, SQL_DESC_OCTET_LENGTH, SQL_NO_TOTAL);

  const SQLCHAR req2[] = "select fieldInt from meta_queries_test_002";

  // SQL_INTEGER should have octet length 4 * sizeof(char)
  callSQLColAttribute(stmt, req2, SQL_DESC_OCTET_LENGTH, 4 * size_of_char);

  const SQLCHAR req3[] = "select fieldLong from meta_queries_test_002";

  // SQL_BIGINT should have octet length 8 * sizeof(char)
  callSQLColAttribute(stmt, req3, SQL_DESC_OCTET_LENGTH, 8 * size_of_char);

  const SQLCHAR req4[] = "select fieldDouble from meta_queries_test_002";

  // SQL_DOUBLE should have octet length 8 * sizeof(char)
  callSQLColAttribute(stmt, req4, SQL_DESC_OCTET_LENGTH, 8 * size_of_char);

  const SQLCHAR req5[] = "select fieldDate from meta_queries_test_002";

  // SQL_TYPE_TIMESTAMP should have octet length 16 * sizeof(char)
  callSQLColAttribute(stmt, req5, SQL_DESC_OCTET_LENGTH, 16 * size_of_char);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescPrecision, *disabled()) {
  ConnectToTS();

  const SQLCHAR req1[] = "select fieldString from meta_queries_test_001";

  // SQL_WVARCHAR should have precision SQL_NO_TOTAL
  callSQLColAttribute(stmt, req1, SQL_DESC_PRECISION, SQL_NO_TOTAL);

  const SQLCHAR req2[] = "select fieldInt from meta_queries_test_001";

  // SQL_INTEGER should have precision 10
  callSQLColAttribute(stmt, req2, SQL_DESC_PRECISION, 10);

  const SQLCHAR req3[] = "select fieldLong from meta_queries_test_001";

  // SQL_BIGINT should have precision 19
  callSQLColAttribute(stmt, req3, SQL_DESC_PRECISION, 19);

  const SQLCHAR req4[] = "select fieldDouble from meta_queries_test_001";

  // SQL_DOUBLE should have precision 15
  callSQLColAttribute(stmt, req4, SQL_DESC_PRECISION, 15);

  const SQLCHAR req5[] = "select fieldDate from meta_queries_test_001";

  // SQL_TIMESTAMP should have precision 19
  callSQLColAttribute(stmt, req5, SQL_DESC_PRECISION, 19);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescScale, *disabled()) {
  ConnectToTS();

  // [AT-1146] TODO add/enable SQLColAttribute test
  // For scale to be 0, data type needs to be SQL_INTEGER
  const SQLCHAR req[] = "select fieldLong from meta_queries_test_001";

  // default scale value is 0
  callSQLColAttribute(stmt, req, SQL_DESC_SCALE, 0);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescSchemaName) {
  ConnectToTS();

  const SQLCHAR req[] = "select location from meta_queries_test_db.IoTMulti";

  // schema name is empty
  callSQLColAttribute(stmt, req, SQL_DESC_SCHEMA_NAME,
                      std::string(""));
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescSearchable, *disabled()) {
  ConnectToTS();

  const SQLCHAR req[] = "select fieldString from meta_queries_test_002";

  // only SQL_PRED_BASIC is returned
  callSQLColAttribute(stmt, req, SQL_DESC_SEARCHABLE, SQL_PRED_BASIC);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescTableName, *disabled()) {
  ConnectToTS();

  const SQLCHAR req[] = "select field from meta_queries_test_002_with_array";

  callSQLColAttribute(stmt, req, SQL_DESC_TABLE_NAME,
                      std::string("meta_queries_test_002_with_array"));
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescType, *disabled()) {
  ConnectToTS();

  const SQLCHAR req1[] = "select fieldString from meta_queries_test_001";

  callSQLColAttribute(stmt, req1, SQL_DESC_TYPE, SQL_WVARCHAR);

  const SQLCHAR req2[] = "select fieldInt from meta_queries_test_001";

  callSQLColAttribute(stmt, req2, SQL_DESC_TYPE, SQL_INTEGER);

  const SQLCHAR req3[] = "select fieldBinary from meta_queries_test_001";

  callSQLColAttribute(stmt, req3, SQL_DESC_TYPE, SQL_VARBINARY);

  // TODO re-enable this test when bug from JDBC (AD-765) is fixed.
  // https://bitquill.atlassian.net/browse/AD-766
  // const SQLCHAR req4[] = "select fieldNull from meta_queries_test_001";
  //
  // callSQLColAttribute(stmt, req4, SQL_DESC_TYPE, SQL_TYPE_NULL);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescUnnamed, *disabled()) {
  ConnectToTS();

  const SQLCHAR req[] = "select fieldNull from meta_queries_test_001";

  // all columns should be named bacause they cannot be null
  callSQLColAttribute(stmt, req, SQL_DESC_UNNAMED, SQL_NAMED);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescUnsigned, *disabled()) {
  ConnectToTS();

  const SQLCHAR req1[] = "select fieldInt from meta_queries_test_001";

  // numeric type should be signed
  callSQLColAttribute(stmt, req1, SQL_DESC_UNSIGNED, SQL_FALSE);

  const SQLCHAR req2[] = "select fieldString from meta_queries_test_001";

  // non-numeric types should be unsigned
  callSQLColAttribute(stmt, req2, SQL_DESC_UNSIGNED, SQL_TRUE);
}

BOOST_AUTO_TEST_CASE(TestColAttributeDescUpdatable, *disabled()) {
  ConnectToTS();

  const SQLCHAR req[] = "select fieldMaxKey from meta_queries_test_002";

  // only SQL_ATTR_READWRITE_UNKNOWN is returned
  callSQLColAttribute(stmt, req, SQL_DESC_UPDATABLE,
                      SQL_ATTR_READWRITE_UNKNOWN);
}

BOOST_AUTO_TEST_CASE(TestColAttributesColumnScale, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select fieldDecimal128 from meta_queries_test_001");
  SQLExecDirect(stmt, req.data(), SQL_NTS);

  SQLLEN intVal;
  SQLWCHAR strBuf[1024];
  SQLSMALLINT strLen;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_COLUMN_SCALE, strBuf,
                                  sizeof(strBuf), &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

BOOST_AUTO_TEST_CASE(TestColAttributesColumnLengthPrepare, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select fieldInt from meta_queries_test_001");
  SQLPrepare(stmt, req.data(), SQL_NTS);

  SQLLEN intVal;
  SQLWCHAR strBuf[1024];
  SQLSMALLINT strLen;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_COLUMN_LENGTH, strBuf,
                                  sizeof(strBuf), &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, 4);

  ret = SQLExecute(stmt);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  ret = SQLColAttribute(stmt, 1, SQL_COLUMN_LENGTH, strBuf, sizeof(strBuf),
                        &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, 4);
}

BOOST_AUTO_TEST_CASE(TestColAttributesColumnPresicionPrepare, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select fieldInt from meta_queries_test_001");
  SQLPrepare(stmt, req.data(), SQL_NTS);

  SQLLEN intVal;
  SQLWCHAR strBuf[1024];
  SQLSMALLINT strLen;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_COLUMN_PRECISION, strBuf,
                                  sizeof(strBuf), &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, 10);

  ret = SQLExecute(stmt);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  ret = SQLColAttribute(stmt, 1, SQL_COLUMN_PRECISION, strBuf, sizeof(strBuf),
                        &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL(intVal, 10);
}

BOOST_AUTO_TEST_CASE(TestColAttributesColumnScalePrepare, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > req =
      MakeSqlBuffer("select fieldString from meta_queries_test_001");
  SQLPrepare(stmt, req.data(), SQL_NTS);

  SQLLEN intVal;
  SQLWCHAR strBuf[1024];
  SQLSMALLINT strLen;

  SQLRETURN ret = SQLColAttribute(stmt, 1, SQL_COLUMN_SCALE, strBuf,
                                  sizeof(strBuf), &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLExecute(stmt);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  ret = SQLColAttribute(stmt, 1, SQL_COLUMN_SCALE, strBuf, sizeof(strBuf),
                        &strLen, &intVal);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
}

BOOST_AUTO_TEST_CASE(TestGetDataWithGetTypeInfo) {
  ConnectToTS();

  SQLRETURN ret = SQLGetTypeInfo(stmt, SQL_VARCHAR);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  CheckSQLGetTypeInfoResult("VARCHAR", SQL_VARCHAR);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithColumnsDataTypes) {
  ConnectToTS();

  std::string dbNameStr = "data_queries_test_db";
  std::vector< SQLWCHAR > table = MakeSqlBuffer("TestScalarTypes");
  std::vector< SQLWCHAR > databaseName = MakeSqlBuffer(dbNameStr);
  SQLRETURN ret;

  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, nullptr, 0, databaseName.data(), SQL_NTS,
                     table.data(), SQL_NTS, nullptr, 0);    
  } else {
    ret = SQLColumns(stmt, databaseName.data(), SQL_NTS, nullptr, 0,
                     table.data(), SQL_NTS, nullptr, 0);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  char column_name[C_STR_LEN_DEFAULT]{};
  SQLLEN column_name_len = sizeof(column_name);
  SQLSMALLINT data_type = 0;
  SQLLEN data_type_len = sizeof(data_type);
  char type_name[C_STR_LEN_DEFAULT]{};
  SQLLEN type_name_len = sizeof(type_name);
  SQLSMALLINT nullable = 0;
  SQLLEN nullable_len = sizeof(nullable);

  ret = SQLBindCol(stmt, 4, SQL_C_CHAR, column_name, sizeof(column_name),
                   &column_name_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));
  ret = SQLBindCol(stmt, 5, SQL_SMALLINT, &data_type, sizeof(data_type),
                   &data_type_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));
  ret = SQLBindCol(stmt, 6, SQL_C_CHAR, type_name, sizeof(type_name),
                   &type_name_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));
  ret = SQLBindCol(stmt, 11, SQL_SMALLINT, &nullable, sizeof(nullable),
                   &nullable_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL("device_id", column_name);  // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_VARCHAR, data_type);                   // DATA_TYPE
  BOOST_CHECK_EQUAL(SqlTypeName::VARCHAR, type_name);           // TYPE_NAME
  BOOST_CHECK_EQUAL(SQL_NO_NULLS, nullable);          // TYPE_NAME

  // Currently at 1st column in the table, call SQLFetch(stmt) 4 times to go to 5th column
  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL("measure_name", column_name);   // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_VARCHAR, data_type);           // DATA_TYPE
  BOOST_CHECK_EQUAL(SqlTypeName::VARCHAR, type_name);  // TYPE_NAME
  BOOST_CHECK_EQUAL(SQL_NO_NULLS, nullable);            // TYPE_NAME

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL("time", column_name);      // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_TYPE_TIMESTAMP, data_type);      // DATA_TYPE
  BOOST_CHECK_EQUAL(SqlTypeName::TIMESTAMP, type_name);  // TYPE_NAME
  BOOST_CHECK_EQUAL(SQL_NO_NULLS, nullable);             // TYPE_NAME

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL("flag", column_name);       // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_BIT, data_type);          // DATA_TYPE
  BOOST_CHECK_EQUAL(SqlTypeName::BIT, type_name);  // TYPE_NAME
  BOOST_CHECK_EQUAL(SQL_NULLABLE, nullable);       // TYPE_NAME

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL("rebuffering_ratio", column_name);     // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_DOUBLE, data_type);             // DATA_TYPE
  BOOST_CHECK_EQUAL(SqlTypeName::DOUBLE, type_name);       // TYPE_NAME
  BOOST_CHECK_EQUAL(SQL_NULLABLE, nullable);            // TYPE_NAME

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL("video_startup_time", column_name);  // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_BIGINT, data_type);              // DATA_TYPE
  BOOST_CHECK_EQUAL(SqlTypeName::BIGINT, type_name);     // TYPE_NAME
  BOOST_CHECK_EQUAL(SQL_NULLABLE, nullable);             // TYPE_NAME

  ret = SQLFetch(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithColumnsTableNameOnly) {
  // Test SQLColumns by only passing table names and "%" without specifying database
  // Check that columns from tables with duplicate names are returned 
  // correctly with SQLColumns.
  ConnectToTS();

  std::vector< SQLWCHAR > all = MakeSqlBuffer("%");
  std::vector< SQLWCHAR > table = MakeSqlBuffer("IoTMulti");
  SQLUSMALLINT databaseColumnIndex;
  SQLRETURN ret;

  if (DATABASE_AS_SCHEMA) {
    databaseColumnIndex = 2;
    ret = SQLColumns(stmt, all.data(), SQL_NTS, nullptr, 0,
                               table.data(), SQL_NTS, all.data(), SQL_NTS);
  } else {
    databaseColumnIndex = 1;
    ret = SQLColumns(stmt, nullptr, 0, all.data(), SQL_NTS,
                               table.data(), SQL_NTS, all.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  char database_name[C_STR_LEN_DEFAULT]{};
  SQLLEN database_name_len = sizeof(database_name);
  char column_name[C_STR_LEN_DEFAULT]{};
  SQLLEN column_name_len = sizeof(column_name);
  SQLSMALLINT data_type = 0;
  SQLLEN data_type_len = sizeof(data_type);
  char type_name[C_STR_LEN_DEFAULT]{};
  SQLLEN type_name_len = sizeof(type_name);
  SQLSMALLINT nullable = 0;
  SQLLEN nullable_len = sizeof(nullable);

  // databaseColumnIndex = 1 (TABLE_CAT) if databse is reported as catalog, 
  // 2 (TABLE_SCHEM) if database is reported as schema
  ret = SQLBindCol(stmt, databaseColumnIndex, SQL_C_CHAR, database_name,
    sizeof(database_name), &database_name_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));
  ret = SQLBindCol(stmt, 4, SQL_C_CHAR, column_name, sizeof(column_name),
                   &column_name_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));
  ret = SQLBindCol(stmt, 5, SQL_SMALLINT, &data_type, sizeof(data_type),
                   &data_type_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));
  ret = SQLBindCol(stmt, 6, SQL_C_CHAR, type_name, sizeof(type_name),
                   &type_name_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));
  ret = SQLBindCol(stmt, 11, SQL_SMALLINT, &nullable, sizeof(nullable),
                   &nullable_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL("meta_queries_test_db", database_name); // DATABASE
  BOOST_CHECK_EQUAL("fleet", column_name);         // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_VARCHAR, data_type);           // DATA_TYPE
  BOOST_CHECK_EQUAL(SqlTypeName::VARCHAR, type_name);  // TYPE_NAME
  BOOST_CHECK_EQUAL(SQL_NO_NULLS, nullable);           // TYPE_NAME

  // Currently at 1st column in the table, call SQLFetch(stmt) 11 times to go to 12th column
  for (int i = 0; i < 12; i++) {
    ret = SQLFetch(stmt);
    BOOST_TEST_MESSAGE("i = " + i);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }

  BOOST_CHECK_EQUAL("sampleDB", database_name);  // DATABASE
  BOOST_CHECK_EQUAL("fleet", column_name);                   // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_VARCHAR, data_type);                 // DATA_TYPE
  BOOST_CHECK_EQUAL(SqlTypeName::VARCHAR, type_name);        // TYPE_NAME
  BOOST_CHECK_EQUAL(SQL_NO_NULLS, nullable);                 // TYPE_NAME
}


BOOST_AUTO_TEST_CASE(TestGetDataWithColumnsNull) {
  ConnectToTS();

  // database is empty case
  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("TestColumnsMetadata1");
  std::vector< SQLWCHAR > column = MakeSqlBuffer("device_id");
  SQLRETURN ret;

  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, empty.data(), SQL_NTS, nullptr, 0,
                               table.data(), SQL_NTS, column.data(), SQL_NTS);
  } else {
    ret = SQLColumns(stmt, nullptr, 0, empty.data(), SQL_NTS,
                               table.data(), SQL_NTS, column.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  int count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(--count == 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  // table is nullptr case
  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, empty.data(), SQL_NTS, nullptr, 0, nullptr, 0,
                     column.data(), SQL_NTS);
  } else {
    ret = SQLColumns(stmt, nullptr, 0, empty.data(), SQL_NTS, nullptr, 0,
                     column.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(--count > 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  // column is nullptr case
  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, empty.data(), SQL_NTS, nullptr, 0, nullptr, 0,
                     nullptr, 0);
  } else {
    ret = SQLColumns(stmt, nullptr, 0, empty.data(), SQL_NTS, nullptr, 0,
                     nullptr, 0);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(--count > 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithColumnsEmpty) {
  ConnectToTS();

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > database = MakeSqlBuffer("meta_queries_test_db");
  std::vector< SQLWCHAR > table = MakeSqlBuffer("TestColumnsMetadata1");
  std::vector< SQLWCHAR > column = MakeSqlBuffer("device_id");

  // database is empty case
  SQLRETURN ret = SQLColumns(stmt, empty.data(), SQL_NTS, empty.data(), SQL_NTS,
                             table.data(), SQL_NTS, column.data(), SQL_NTS);

  BOOST_REQUIRE_EQUAL(ret, SQL_SUCCESS_WITH_INFO);
  CheckSQLStatementDiagnosticError("01000");

  // table is empty case
  ret = SQLColumns(stmt, empty.data(), SQL_NTS, database.data(), SQL_NTS,
                   empty.data(), SQL_NTS, column.data(), SQL_NTS);

  BOOST_REQUIRE_EQUAL(ret, SQL_SUCCESS_WITH_INFO);
  CheckSQLStatementDiagnosticError("01000");

  // database and table non-empty case
  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, empty.data(), SQL_NTS, database.data(), SQL_NTS,
                     table.data(), SQL_NTS, empty.data(), 0);
  } else {
    ret = SQLColumns(stmt, database.data(), SQL_NTS, empty.data(), SQL_NTS,
                     table.data(), SQL_NTS, empty.data(), 0);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  int count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(--count > 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithColumnsUnicode) {
  ConnectToTS();
  
  // Timestream only has unicode support for column name, 
  // and database/table name does not have unicode support.
  std::string dbNameStr = "meta_queries_test_db";
  std::vector< SQLWCHAR > table = MakeSqlBuffer("TestColumnsMetadata1");
  std::vector< SQLWCHAR > databaseName = MakeSqlBuffer(dbNameStr);
  std::vector< SQLWCHAR > column = MakeSqlBuffer("地区");
  SQLRETURN ret;

  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, nullptr, 0, databaseName.data(), SQL_NTS,
                              table.data(), SQL_NTS, column.data(), SQL_NTS);
  } else {
    ret = SQLColumns(stmt, databaseName.data(), SQL_NTS, nullptr, 0,
                               table.data(), SQL_NTS, column.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  SQLWCHAR column_name[C_STR_LEN_DEFAULT];
  SQLLEN column_name_len = sizeof(column_name);
  SQLSMALLINT data_type = 0;
  SQLLEN data_type_len = sizeof(data_type);

  ret = SQLBindCol(stmt, 4, SQL_C_WCHAR, column_name, sizeof(column_name),
                   &column_name_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));
  ret = SQLBindCol(stmt, 5, SQL_SMALLINT, &data_type, sizeof(data_type),
                   &data_type_len);
  BOOST_CHECK(SQL_SUCCEEDED(ret));

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  BOOST_CHECK_EQUAL("地区", utility::SqlWcharToString(column_name, column_name_len));          // COLUMN_NAME
  BOOST_CHECK_EQUAL(SQL_VARCHAR, data_type);           // DATA_TYPE
}

BOOST_AUTO_TEST_CASE(TestGetDataWithColumnsSearchPattern) {
  ConnectToTS();

  std::vector< SQLWCHAR > databaseName = MakeSqlBuffer("%");
  std::vector< SQLWCHAR > table = MakeSqlBuffer("%");
  std::vector< SQLWCHAR > column = MakeSqlBuffer("%");
  SQLRETURN ret;

  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, nullptr, 0, databaseName.data(), SQL_NTS,
                               table.data(), SQL_NTS, column.data(), SQL_NTS);
  } else {
    ret = SQLColumns(stmt, databaseName.data(), SQL_NTS, nullptr, 0,
                               table.data(), SQL_NTS, column.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  int count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(--count > 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  databaseName = MakeSqlBuffer("data\\_queries\\_test\\_db");
  table = MakeSqlBuffer("TestScalarTypes");

  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, nullptr, 0, databaseName.data(), SQL_NTS,
                     table.data(), SQL_NTS, column.data(), SQL_NTS);
  } else {
    ret = SQLColumns(stmt, databaseName.data(), SQL_NTS, nullptr, 0,
                     table.data(), SQL_NTS, column.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(--count > 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithColumnsIdentifier) {
  ConnectToTS();

  SQLRETURN ret = SQLSetConnectAttr(dbc, SQL_ATTR_METADATA_ID,
                          reinterpret_cast< SQLPOINTER >(SQL_TRUE), 0);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  std::vector< SQLWCHAR > schemaName = MakeSqlBuffer("%");
  std::vector< SQLWCHAR > table = MakeSqlBuffer("%");
  std::vector< SQLWCHAR > column = MakeSqlBuffer("%");

  ret = SQLColumns(stmt, nullptr, 0, schemaName.data(), SQL_NTS,
                             table.data(), SQL_NTS, column.data(), SQL_NTS);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

#ifndef __linux__
  // Linux unixODBC DM could clear the diagnostic error message when 
  // function return value is not SQL_ERROR
  std::string error = GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt);
  std::string pattern = "Failed to execute query \"describe %.%\"";

  if (error.find(pattern) == std::string::npos)
    BOOST_FAIL("'" + error + "' does not match '" + pattern + "'");
#endif
}

BOOST_AUTO_TEST_CASE(TestGetDataWithColumnsNonExist) {
  // test passing nonexistent database/table/column names to SQLColumns returns no data
  ConnectToTS();

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("nonexistent");
  std::vector< SQLWCHAR > column = MakeSqlBuffer("nonexistent_column");
  SQLRETURN ret;

  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, empty.data(), SQL_NTS, nullptr, 0,
                               table.data(), SQL_NTS, column.data(), SQL_NTS);
  } else {
    ret = SQLColumns(stmt, nullptr, 0, empty.data(), SQL_NTS,
                               table.data(), SQL_NTS, column.data(), SQL_NTS);  
  }

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  std::vector< SQLWCHAR > database = MakeSqlBuffer("nonexistent_database");
  std::vector< SQLWCHAR > correctTable = MakeSqlBuffer("TestColumnsMetadata1");
  std::vector< SQLWCHAR > correctColumn = MakeSqlBuffer("device_id");

  if (DATABASE_AS_SCHEMA) {
    ret = SQLColumns(stmt, empty.data(), SQL_NTS, database.data(), SQL_NTS,
                     table.data(), SQL_NTS, column.data(), SQL_NTS);
  } else {
    ret = SQLColumns(stmt, database.data(), SQL_NTS, empty.data(), SQL_NTS,
                     table.data(), SQL_NTS, column.data(), SQL_NTS);
  }

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  // test passing empty string databaseName to SQLColumns returns no data
  if (DATABASE_AS_SCHEMA) {
    ret =
        SQLColumns(stmt, empty.data(), SQL_NTS, nullptr, 0, correctTable.data(),
                   SQL_NTS, correctColumn.data(), SQL_NTS);
  } else {
    ret =
        SQLColumns(stmt, nullptr, 0, empty.data(), SQL_NTS, correctTable.data(),
                   SQL_NTS, correctColumn.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  int count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(--count == 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithTablesReturnsOne) {
  ConnectToTS();

  // Case 1: provide table name
  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("testTableMeta");
  SQLRETURN ret;

  if (DATABASE_AS_SCHEMA) {
    ret = SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0,
                              table.data(), SQL_NTS, empty.data(), SQL_NTS);
  } else {
    ret = SQLTables(stmt, nullptr, 0, empty.data(), SQL_NTS,
                              table.data(), SQL_NTS, empty.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  CheckSingleRowResultSetWithGetData(stmt, 3,
                                     utility::SqlWcharToString(table.data()));

  BOOST_TEST_CHECKPOINT("case 1 passed");

  // Case 2: provide database name and table name
  // meta_queries_test_db has multiple tables. Check that only 1 table is
  // returned
  std::vector< SQLWCHAR > database = MakeSqlBuffer("meta_queries_test_db");
  std::vector< SQLWCHAR > testTable = MakeSqlBuffer("IoTMulti");

  if (DATABASE_AS_SCHEMA) {
    ret = SQLTables(stmt, empty.data(), SQL_NTS, database.data(), SQL_NTS,
                    testTable.data(), SQL_NTS, empty.data(), SQL_NTS);
  } else {
    ret = SQLTables(stmt, database.data(), SQL_NTS, empty.data(), SQL_NTS,
                    testTable.data(), SQL_NTS, empty.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  CheckSingleRowResultSetWithGetData(
      stmt, 3, utility::SqlWcharToString(testTable.data()));

  // Case 3: provide database name
  // Check that only 1 table is returned
  std::vector< SQLWCHAR > testDatabase = MakeSqlBuffer("sampleDB");

  if (DATABASE_AS_SCHEMA) {
    ret = SQLTables(stmt, empty.data(), SQL_NTS, testDatabase.data(), SQL_NTS,
                    nullptr, 0, empty.data(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    CheckSingleRowResultSetWithGetData(
        stmt, 2, utility::SqlWcharToString(testDatabase.data()));

  } else {
    ret = SQLTables(stmt, testDatabase.data(), SQL_NTS, empty.data(), SQL_NTS,
                    nullptr, 0, empty.data(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    CheckSingleRowResultSetWithGetData(
        stmt, 1, utility::SqlWcharToString(testDatabase.data()));
  }
}

BOOST_AUTO_TEST_CASE(TestGetDataWithTablesReturnsOneWithTableTypes) {
  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("testTableMeta");
  std::vector< SQLWCHAR > tableTypes = MakeSqlBuffer(
      "TABLE,VIEW");  // Test that VIEW type is ignored by ODBC driver
  SQLRETURN ret;

  ConnectToTS();
  
  if (DATABASE_AS_SCHEMA) {
    ret = SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0, table.data(),
                    SQL_NTS, tableTypes.data(), SQL_NTS);
  } else {
    ret = SQLTables(stmt, nullptr, 0, empty.data(), SQL_NTS, table.data(),
                    SQL_NTS, tableTypes.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  CheckSingleRowResultSetWithGetData(stmt, 3,
                                     utility::SqlWcharToString(table.data()));
}

BOOST_AUTO_TEST_CASE(TestGetDataWithTablesReturnsOneForQuotedTypes) {
  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("testTableMeta");
  std::vector< SQLWCHAR > tableTypes =
      MakeSqlBuffer("'TABLE' , 'VIEW'");  // Test that quoted values are handled
  SQLRETURN ret;

  ConnectToTS();

  if (DATABASE_AS_SCHEMA) {
    ret =
        SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0, table.data(),
                  SQL_NTS, tableTypes.data(), SQL_NTS);
  } else {
    ret =
        SQLTables(stmt, nullptr, 0, empty.data(), SQL_NTS, table.data(),
                  SQL_NTS, tableTypes.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  CheckSingleRowResultSetWithGetData(stmt, 3, utility::SqlWcharToString(table.data()));
}

BOOST_AUTO_TEST_CASE(TestGetDataWithTablesReturnsNoneForUnsupportedTableType) {
  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("testTableMeta");
  std::vector< SQLWCHAR > tableTypes = MakeSqlBuffer("VIEW");

  ConnectToTS();

  SQLRETURN ret = SQLTables(stmt, nullptr, 0, nullptr, 0, 
                            table.data(), SQL_NTS, tableTypes.data(), SQL_NTS);


  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  BOOST_CHECK_EQUAL(SQL_NO_DATA, ret);
}

BOOST_AUTO_TEST_CASE(TestGetDatabasesWithSQLTables) {
  // tests special case: get a list of databases with SQLTables
  // To avoid test failures due to unrelated database changes,
  // this test checks the specified three databases only.

  // TODO [AT-1163] Check all databases after we make sure that
  // there will be no more database changes
  // https://bitquill.atlassian.net/browse/AT-1163
  std::vector< SQLWCHAR > empty = {0};
  SQLUSMALLINT columnIndex;
  SQLRETURN ret;

  ConnectToTS();

  // columnIndex 1 and 2 corresponds to CatalogName and SchemaName respectively. 
  if (DATABASE_AS_SCHEMA) {
    columnIndex = 2;
    std::vector< SQLWCHAR > schemas = MakeSqlBuffer(SQL_ALL_SCHEMAS);

    ret =
        SQLTables(stmt, empty.data(), SQL_NTS, schemas.data(), SQL_NTS,
                  empty.data(), SQL_NTS, empty.data(), SQL_NTS);
  } else {
    columnIndex = 1;
    std::vector< SQLWCHAR > catalogs = MakeSqlBuffer(SQL_ALL_CATALOGS);

    ret =
        SQLTables(stmt, catalogs.data(), SQL_NTS, empty.data(), SQL_NTS,
                  empty.data(), SQL_NTS, empty.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  std::map< std::string, bool > databaseMap;
  databaseMap["data_queries_test_db"] = false;
  databaseMap["meta_queries_test_db"] = false;
  databaseMap["sampleDB"] = false;

  // check all databases
  for (;;) {
    ret = SQLFetch(stmt);

    if (ret == SQL_NO_DATA) {
      break;
    } else if (!SQL_SUCCEEDED(ret)) {
      BOOST_CHECK(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);

      std::string sqlMessage = GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt);
      if (sqlMessage.empty()) {
        sqlMessage.append("SQLFetch returned: " + std::to_string(ret));
      }
    }

    SQLWCHAR buf[1024];
    SQLLEN bufLen = sizeof(buf);

    for (int i = 1; i <= columnIndex; i++) {
      ret = SQLGetData(stmt, i, SQL_C_WCHAR, buf, sizeof(buf), &bufLen);

      if (!SQL_SUCCEEDED(ret))
        BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

      std::string actualValueStr = utility::SqlWcharToString(buf, bufLen);
      if (databaseMap.find(actualValueStr) != databaseMap.end()) {
        databaseMap[actualValueStr] = true;
      }
    }
  }

  // check all specified databased be found
  for (auto &itr : databaseMap) {
    if (!itr.second) {
      std::string sqlMessage = "Database " + itr.first + " not found";
      BOOST_FAIL(sqlMessage);
    }
  }
}

BOOST_AUTO_TEST_CASE(TestGetTableTypesWithSQLTables) {
  // tests special case: get a list of valid table types with SQLTables
  ConnectToTS();

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > tableType = MakeSqlBuffer(SQL_ALL_TABLE_TYPES);

  SQLRETURN ret =
      SQLTables(stmt, empty.data(), SQL_NTS, empty.data(), SQL_NTS,
                empty.data(), SQL_NTS, tableType.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  std::string expectedTableType = "TABLE";
  // check that columnIndex 4 (corresponds to TABLE_TYPE) is "TABLE"

  CheckSingleRowResultSetWithGetData(stmt, 4, expectedTableType, true);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithTablesReturnsNone) {
  ConnectToTS();

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("nonexistent");

  SQLRETURN ret = SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0,
                            table.data(), SQL_NTS, empty.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  // test that no data is returned with empty string schema
  std::vector< SQLWCHAR > correctTable = MakeSqlBuffer("testTableMeta");

  ret = SQLTables(stmt, empty.data(), SQL_NTS, empty.data(), SQL_NTS,
                  correctTable.data(), SQL_NTS, empty.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);


  if (DATABASE_AS_SCHEMA) {
    // test that no data is returned for a list of catalog
    // Note that Timestream does not have catalogs
    std::vector< SQLWCHAR > catalog = MakeSqlBuffer(SQL_ALL_CATALOGS);
    ret = SQLTables(stmt, catalog.data(), SQL_NTS, empty.data(), SQL_NTS,
                    empty.data(), SQL_NTS, empty.data(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLFetch(stmt);

    BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
  } else {
    // test that no data is returned for a list of schemas
    // Note that Timestream does not have schemas
    std::vector< SQLWCHAR > schema = MakeSqlBuffer(SQL_ALL_SCHEMAS);
    ret = SQLTables(stmt, empty.data(), SQL_NTS, schema.data(), SQL_NTS,
                    empty.data(), SQL_NTS, empty.data(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret))
      BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

    ret = SQLFetch(stmt);

    BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
  }
}

BOOST_AUTO_TEST_CASE(TestGetDataWithTablesReturnsMany) {
  ConnectToTS();

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("%");
  SQLRETURN ret;

  // test with table passed as "%"
  if (DATABASE_AS_SCHEMA) {
    ret = SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0, table.data(),
                    SQL_NTS, empty.data(), SQL_NTS);
  } else {
    ret = SQLTables(stmt, nullptr, 0, empty.data(), SQL_NTS, table.data(),
                    SQL_NTS, empty.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  int count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(count > 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  // test with table passed as nullptr
  if (DATABASE_AS_SCHEMA) {
    ret = SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0, nullptr, 0,
                    empty.data(), SQL_NTS);
  } else {
    ret = SQLTables(stmt, nullptr, 0, empty.data(), SQL_NTS, nullptr, 0,
                    empty.data(), SQL_NTS);
  }

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  count = 0;
  do {
    ret = SQLFetch(stmt);
    count++;
  } while (SQL_SUCCEEDED(ret));
  BOOST_CHECK(count > 1);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithTablesIdentifier) {
  ConnectToTS();

  SQLRETURN ret = SQLSetConnectAttr(
      dbc, SQL_ATTR_METADATA_ID, reinterpret_cast< SQLPOINTER >(SQL_TRUE), 0);

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("%");

  // test with table passed as "%"
  ret = SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0,
                            table.data(), SQL_NTS, empty.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);
  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithPrimaryKeysReturnsOneFromLocalServer, *disabled()) {
  std::vector< SQLWCHAR > table = MakeSqlBuffer("meta_queries_test_001");

  ConnectToTS();

  SQLRETURN ret =
      SQLPrimaryKeys(stmt, nullptr, 0, nullptr, 0, table.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // check result for COLUMN_NAME
  CheckSingleRowResultSetWithGetData(stmt, 4, "meta_queries_test_001__id");
}

BOOST_AUTO_TEST_CASE(TestGetDataWithPrimaryKeysReturnsNone, *disabled()) {
  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("meta_queries_test_001");

  ConnectToTS();

  SQLRETURN ret = SQLPrimaryKeys(stmt, nullptr, SQL_NTS, nullptr, SQL_NTS,
                                 empty.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  ret = SQLPrimaryKeys(stmt, empty.data(), SQL_NTS, empty.data(), SQL_NTS,
                       empty.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  ret = SQLPrimaryKeys(stmt, empty.data(), SQL_NTS, empty.data(), SQL_NTS,
                       table.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithForeignKeysReturnsOneFromLocalServer, *disabled()) {
  std::vector< SQLWCHAR > table =
      MakeSqlBuffer("meta_queries_test_002_with_array_array");

  ConnectToTS();

  SQLRETURN ret = SQLForeignKeys(stmt, NULL, 0,          /* Primary catalog */
                                 NULL, 0,                /* Primary schema */
                                 NULL, 0,                /* Primary table */
                                 NULL, 0,                /* Foreign catalog */
                                 NULL, 0,                /* Foreign schema */
                                 table.data(), SQL_NTS); /* Foreign table */

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // check result for PKTableName
  CheckSingleRowResultSetWithGetData(stmt, 4,
                                     "meta_queries_test_002_with_array__id");

  ret = SQLCloseCursor(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  std::vector< SQLWCHAR > empty = {0};

  ret = SQLForeignKeys(stmt, NULL, 0,          /* Primary catalog */
                       NULL, 0,                /* Primary schema */
                       NULL, 0,                /* Primary table */
                       empty.data(), SQL_NTS,  /* Foreign catalog */
                       NULL, 0,                /* Foreign schema */
                       table.data(), SQL_NTS); /* Foreign table */

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  // check result for PKTableName
  CheckSingleRowResultSetWithGetData(stmt, 4,
                                     "meta_queries_test_002_with_array__id");
}

BOOST_AUTO_TEST_CASE(TestGetDataWithForeignKeysReturnsNone, *disabled()) {
  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table =
      MakeSqlBuffer("meta_queries_test_002_with_array_array");

  ConnectToTS();

  SQLRETURN ret =
      SQLForeignKeys(stmt, empty.data(), SQL_NTS, /* Primary catalog */
                     empty.data(), SQL_NTS,       /* Primary schema */
                     empty.data(), SQL_NTS,       /* Primary table */
                     empty.data(), SQL_NTS,       /* Foreign catalog */
                     empty.data(), SQL_NTS,       /* Foreign schema */
                     empty.data(), SQL_NTS);      /* Foreign table */

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);

  // when empty strings are passed as catalog/schema, SQL_NO_DATA should be
  // returned
  ret = SQLForeignKeys(stmt, empty.data(), SQL_NTS, /* Primary catalog */
                       empty.data(), SQL_NTS,       /* Primary schema */
                       empty.data(), SQL_NTS,       /* Primary table */
                       empty.data(), SQL_NTS,       /* Foreign catalog */
                       empty.data(), SQL_NTS,       /* Foreign schema */
                       table.data(), SQL_NTS);      /* Foreign table */

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  ret = SQLFetch(stmt);

  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestSQLColumnWithSQLBindCols, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("meta_queries_test_001");
  std::vector< SQLWCHAR > column = MakeSqlBuffer("meta_queries_test_001__id");

  SQLRETURN ret = SQL_SUCCESS;

  char table_cat[C_STR_LEN_DEFAULT]{};
  SQLLEN table_cat_len = sizeof(table_cat);
  char table_schem[C_STR_LEN_DEFAULT]{};
  SQLLEN table_schem_len = sizeof(table_schem);
  char table_name[C_STR_LEN_DEFAULT]{};
  SQLLEN table_name_len = sizeof(table_name);
  char column_name[C_STR_LEN_DEFAULT]{};
  SQLLEN column_name_len = sizeof(column_name);
  SQLSMALLINT data_type = 0;
  SQLLEN data_type_len = sizeof(data_type);
  char type_name[C_STR_LEN_DEFAULT]{};
  SQLLEN type_name_len = sizeof(type_name);
  SQLINTEGER column_size = 0;
  SQLLEN column_size_len = sizeof(column_size);
  SQLINTEGER buffer_length = 0;
  SQLLEN buffer_length_len = sizeof(buffer_length);
  SQLSMALLINT decimal_digits = 0;
  SQLLEN decimal_digits_len = sizeof(decimal_digits);
  SQLSMALLINT num_prec_radix = 0;
  SQLLEN num_prec_radix_len = sizeof(num_prec_radix);
  SQLSMALLINT nullable = 0;
  SQLLEN nullable_len = sizeof(nullable);
  char remarks[C_STR_LEN_DEFAULT]{};
  SQLLEN remarks_len = sizeof(remarks);
  char column_def[C_STR_LEN_DEFAULT]{};
  SQLLEN column_def_len = sizeof(column_def);
  SQLSMALLINT sql_data_type = 0;
  SQLLEN sql_data_type_len = sizeof(sql_data_type);
  SQLSMALLINT sql_datetime_sub = 0;
  SQLLEN sql_datetime_sub_len = sizeof(sql_datetime_sub);
  SQLINTEGER char_octet_length = 0;
  SQLLEN char_octet_length_len = sizeof(char_octet_length);
  SQLINTEGER ordinal_position = 0;
  SQLLEN ordinal_position_len = sizeof(ordinal_position);
  char is_nullable[C_STR_LEN_DEFAULT]{};
  SQLLEN is_nullable_len = sizeof(is_nullable);

  SQLColumnsBindColumns(
      stmt, table_cat, table_cat_len, table_schem, table_schem_len, table_name,
      table_name_len, column_name, column_name_len, data_type, data_type_len,
      type_name, type_name_len, column_size, column_size_len, buffer_length,
      buffer_length_len, decimal_digits, decimal_digits_len, num_prec_radix,
      num_prec_radix_len, nullable, nullable_len, remarks, remarks_len,
      column_def, column_def_len, sql_data_type, sql_data_type_len,
      sql_datetime_sub, sql_datetime_sub_len, char_octet_length,
      char_octet_length_len, ordinal_position, ordinal_position_len,
      is_nullable, is_nullable_len);

  ret = SQLColumns(stmt, nullptr, 0, nullptr, 0, table.data(), SQL_NTS,
                   column.data(), SQL_NTS);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  SQLSMALLINT numResultCols = 0;
  ret = SQLNumResultCols(stmt, &numResultCols);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  BOOST_CHECK_EQUAL(18, numResultCols);

  ret = SQLFetch(stmt);
  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

  bool errorExpected = false;
  BOOST_CHECK_EQUAL(true, WasNull(table_cat_len));
  BOOST_CHECK_EQUAL("", table_cat);  // TABLE_CAT
  BOOST_CHECK_EQUAL(false, WasNull(table_schem_len));
  BOOST_CHECK_EQUAL("odbc-test", table_schem);  // TABLE_SCHEM
  BOOST_CHECK_EQUAL(false, WasNull(table_name_len));
  BOOST_CHECK_EQUAL("meta_queries_test_001", table_name);  // TABLE_NAME
  BOOST_CHECK_EQUAL(false, WasNull(column_name_len));
  BOOST_CHECK_EQUAL("meta_queries_test_001__id", column_name);  // COLUMN_NAME
  BOOST_CHECK_EQUAL(false, WasNull(data_type_len));
  BOOST_CHECK_EQUAL(SQL_WVARCHAR, data_type);  // DATA_TYPE
  BOOST_CHECK_EQUAL(false, WasNull(type_name_len));
  BOOST_CHECK_EQUAL("VARCHAR", type_name);  // TYPE_NAME
  BOOST_CHECK_EQUAL(false, WasNull(column_size_len));
  BOOST_CHECK_EQUAL(SQL_NO_TOTAL, column_size);  // COLUMN_SIZE
  BOOST_CHECK_EQUAL(false, WasNull(buffer_length_len));
  BOOST_CHECK_EQUAL(SQL_NO_TOTAL, buffer_length);  // BUFFER_LENGTH
  BOOST_CHECK_EQUAL(true, WasNull(decimal_digits_len));
  BOOST_CHECK_EQUAL(0, decimal_digits);  // DECIMAL_DIGITS
  BOOST_CHECK_EQUAL(false, WasNull(num_prec_radix_len));
  BOOST_CHECK_EQUAL(0, num_prec_radix);  // NUM_PREC_RADIX
  BOOST_CHECK_EQUAL(false, WasNull(nullable_len));
  BOOST_CHECK_EQUAL(SQL_NO_NULLS, nullable);  // NULLABLE
  BOOST_CHECK_EQUAL(true, WasNull(remarks_len));
  BOOST_CHECK_EQUAL("", remarks);  // REMARKS
  BOOST_CHECK_EQUAL(true, WasNull(column_def_len));
  BOOST_CHECK_EQUAL("", column_def);  // COLUMN_DEF
  BOOST_CHECK_EQUAL(false, WasNull(sql_data_type_len));
  BOOST_CHECK_EQUAL(SQL_WVARCHAR, sql_data_type);  // SQL_DATA_TYPE
  BOOST_CHECK_EQUAL(true, WasNull(sql_datetime_sub_len));
  BOOST_CHECK_EQUAL(0, sql_datetime_sub);  // SQL_DATETIME_SUB
  BOOST_CHECK_EQUAL(false, WasNull(char_octet_length_len));
  BOOST_CHECK_EQUAL(SQL_NO_TOTAL, char_octet_length);  // CHAR_OCTET_LENGTH
  BOOST_CHECK_EQUAL(false, WasNull(ordinal_position_len));
  BOOST_CHECK_EQUAL(1, ordinal_position);  // ORDINAL_POSITION
  BOOST_CHECK_EQUAL(false, WasNull(is_nullable_len));
  BOOST_CHECK_EQUAL("NO", is_nullable);  // IS_NULLABLE

  // Check that we can get an attribute on the columns metadata.
  SQLWCHAR attrColumnName[C_STR_LEN_DEFAULT];
  SQLSMALLINT attrColumnNameLen = 0;
  ret = SQLColAttribute(stmt, 2, SQL_DESC_NAME, attrColumnName,
                        sizeof(attrColumnName), &attrColumnNameLen,
                        nullptr);  // COLUMN_NAME, NOT NULL
  if (!SQL_SUCCEEDED(ret)) {
    BOOST_ERROR(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));
  }
  BOOST_CHECK_EQUAL("TABLE_SCHEM", utility::SqlWcharToString(
                                       attrColumnName, attrColumnNameLen));

  // Test that the next fetch will have no data.
  ret = SQLFetch(stmt);
  BOOST_REQUIRE_EQUAL(ret, SQL_NO_DATA);
}

BOOST_AUTO_TEST_CASE(TestGetDataWithSelectQuery, *disabled()) {
  ConnectToTS();

  std::vector< SQLWCHAR > selectReq =
      MakeSqlBuffer("select fieldInt from meta_queries_test_001");
  SQLRETURN ret = SQLExecDirect(stmt, selectReq.data(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_STMT, stmt));

#ifdef __APPLE__
  CheckSingleRowResultSetWithGetData(stmt, 1, "", false, INVALID_CURSOR_STATE);
#else
  CheckSingleRowResultSetWithGetData(stmt);
#endif
}

BOOST_AUTO_TEST_CASE(TestGetInfoScrollOptions, *disabled()) {
  ConnectToTS();

  SQLUINTEGER val = 0;
  SQLRETURN ret = SQLGetInfo(dbc, SQL_SCROLL_OPTIONS, &val, 0, 0);

  if (!SQL_SUCCEEDED(ret))
    BOOST_FAIL(GetOdbcErrorMessage(SQL_HANDLE_DBC, dbc));

  BOOST_CHECK_NE(val, 0);
}

BOOST_AUTO_TEST_CASE(TestSQLNumResultColsAfterSQLPrepare, *disabled()) {
  ConnectToTS();

  SQLRETURN ret = PrepareQuery("select fieldInt from meta_queries_test_001");
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  SQLSMALLINT columnCount = 0;

  ret = SQLNumResultCols(stmt, &columnCount);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(columnCount, 1);

  ret = SQLExecute(stmt);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  columnCount = 0;

  ret = SQLNumResultCols(stmt, &columnCount);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(columnCount, 1);
}

BOOST_AUTO_TEST_CASE(TestSQLDescribeColSQLTablesODBCVer3) {
  // Check SQLTable metadata when ODBC Ver is set to 3 (default).
  ConnectToTS();

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("%");

  // test with table passed as "%"
  SQLRETURN ret = SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0,
                            table.data(), SQL_NTS, empty.data(), SQL_NTS);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  SQLSMALLINT columnCount = 0;

  ret = SQLNumResultCols(stmt, &columnCount);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(columnCount, 5);

  SQLWCHAR columnName[ODBC_BUFFER_SIZE];
  SQLSMALLINT columnNameLen = 0;
  SQLSMALLINT dataType = 0;
  SQLULEN columnSize = 0;
  SQLSMALLINT decimalDigits = 0;
  SQLSMALLINT nullable = 0;

  // Everything is ok.
  ret = SQLDescribeCol(stmt, 1, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "TABLE_CAT");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);

  ret = SQLDescribeCol(stmt, 2, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "TABLE_SCHEM");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);

  ret = SQLDescribeCol(stmt, 3, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "TABLE_NAME");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);

  ret = SQLDescribeCol(stmt, 4, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "TABLE_TYPE");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);

  ret = SQLDescribeCol(stmt, 5, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "REMARKS");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);
}

BOOST_AUTO_TEST_CASE(TestSQLDescribeColSQLTablesODBCVer2) {
  // Check SQLTable metadata when ODBC Ver is set to 2.
  ConnectToTS(SQL_OV_ODBC2);

  std::vector< SQLWCHAR > empty = {0};
  std::vector< SQLWCHAR > table = MakeSqlBuffer("%");

  // test with table passed as "%"
  SQLRETURN ret = SQLTables(stmt, empty.data(), SQL_NTS, nullptr, 0,
                            table.data(), SQL_NTS, empty.data(), SQL_NTS);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  SQLSMALLINT columnCount = 0;

  ret = SQLNumResultCols(stmt, &columnCount);

  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(columnCount, 5);

  SQLWCHAR columnName[ODBC_BUFFER_SIZE];
  SQLSMALLINT columnNameLen = 0;
  SQLSMALLINT dataType = 0;
  SQLULEN columnSize = 0;
  SQLSMALLINT decimalDigits = 0;
  SQLSMALLINT nullable = 0;

  // Everything is ok.
  ret = SQLDescribeCol(stmt, 1, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "TABLE_QUALIFIER");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);

  ret = SQLDescribeCol(stmt, 2, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "TABLE_OWNER");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);

  ret = SQLDescribeCol(stmt, 3, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "TABLE_NAME");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);

  ret = SQLDescribeCol(stmt, 4, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "TABLE_TYPE");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);

  ret = SQLDescribeCol(stmt, 5, columnName, ODBC_BUFFER_SIZE, &columnNameLen,
                       &dataType, &columnSize, &decimalDigits, &nullable);
  ODBC_FAIL_ON_ERROR(ret, SQL_HANDLE_STMT, stmt);

  BOOST_CHECK_EQUAL(utility::SqlWcharToString(columnName, columnNameLen),
                    "REMARKS");
  BOOST_CHECK_EQUAL(dataType, SQL_VARCHAR);
}

/**
 * Check that SQLDescribeCol return valid scale and precision for columns of
 * different type after Prepare.
 *
 * 1. Start node.
 * 2. Connect to node using ODBC.
 * 3. Create table with decimal and char columns with specified size and scale.
 * 4. Prepare statement.
 * 5. Check precision and scale of every column using SQLDescribeCol.
 */
BOOST_AUTO_TEST_CASE(TestSQLDescribeColPrecisionAndScaleAfterPrepare, *disabled()) {
  CheckSQLDescribeColPrecisionAndScale(&OdbcTestSuite::PrepareQuery);
}

/**
 * Check that SQLDescribeCol return valid scale and precision for columns of
 * different type after Execute.
 *
 * 1. Start node.
 * 2. Connect to node using ODBC.
 * 3. Create table with decimal and char columns with specified size and scale.
 * 4. Execute statement.
 * 5. Check precision and scale of every column using SQLDescribeCol. */
BOOST_AUTO_TEST_CASE(TestSQLDescribeColPrecisionAndScaleAfterExec, *disabled()) {
  CheckSQLDescribeColPrecisionAndScale(&OdbcTestSuite::ExecQuery);
}

/**
 * Check that SQLColAttribute return valid scale and precision for columns of
 * different type after Prepare.
 *
 * 1. Start node.
 * 2. Connect to node using ODBC.
 * 3. Create table with decimal and char columns with specified size and scale.
 * 4. Prepare statement.
 * 5. Check precision and scale of every column using SQLColAttribute.
 */
BOOST_AUTO_TEST_CASE(TestSQLColAttributePrecisionAndScaleAfterPrepare, *disabled()) {
  CheckSQLColAttributePrecisionAndScale(&OdbcTestSuite::PrepareQuery);
}

/**
 * Check that SQLColAttribute return valid scale and precision for columns of
 * different type after Execute.
 *
 * 1. Start node.
 * 2. Connect to node using ODBC.
 * 3. Create table with decimal and char columns with specified size and scale.
 * 4. Execute statement.
 * 5. Check precision and scale of every column using SQLColAttribute. */
BOOST_AUTO_TEST_CASE(TestSQLColAttributePrecisionAndScaleAfterExec, *disabled()) {
  CheckSQLColAttributePrecisionAndScale(&OdbcTestSuite::ExecQuery);
}

BOOST_AUTO_TEST_SUITE_END()
