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
#include <cstdio>
#include <string>
#include <vector>

#include "timestream/timestream.h"
#include "timestream/ignition.h"
#include "timestream/odbc/impl/binary/binary_utils.h"
#include "odbc_test_suite.h"
#include "test_type.h"
#include "test_utils.h"

using namespace timestream;
using namespace timestream::cache;
using namespace timestream::cache::query;
using namespace timestream::common;
using namespace timestream_test;

using namespace boost::unit_test;

using timestream::impl::binary::BinaryUtils;

/**
 * Test setup fixture.
 */
struct CursorBindingTestSuiteFixture : public odbc::OdbcTestSuite {
  static Ignite StartAdditionalNode(const char* name) {
    return StartPlatformNode("queries-test.xml", name);
  }

  /**
   * Constructor.
   */
  CursorBindingTestSuiteFixture() : testCache(0) {
    grid = StartAdditionalNode("NodeMain");

    testCache = grid.GetCache< int64_t, TestType >("cache");
  }

  /**
   * Destructor.
   */
  virtual ~CursorBindingTestSuiteFixture() {
    // No-op.
  }

  /** Node started during the test. */
  Ignite grid;

  /** Test cache instance. */
  Cache< int64_t, TestType > testCache;
};

BOOST_FIXTURE_TEST_SUITE(CursorBindingTestSuite, CursorBindingTestSuiteFixture)

#define CHECK_TEST_VALUES(idx, testIdx)                                      \
  do {                                                                       \
    BOOST_TEST_CONTEXT("Test idx: " << testIdx) {                            \
      BOOST_CHECK(RowStatus[idx] == SQL_ROW_SUCCESS                          \
                  || RowStatus[idx] == SQL_ROW_SUCCESS_WITH_INFO);           \
                                                                             \
      BOOST_CHECK(i8FieldsInd[idx] != SQL_NULL_DATA);                        \
      BOOST_CHECK(i16FieldsInd[idx] != SQL_NULL_DATA);                       \
      BOOST_CHECK(i32FieldsInd[idx] != SQL_NULL_DATA);                       \
      BOOST_CHECK(strFieldsLen[idx] != SQL_NULL_DATA);                       \
      BOOST_CHECK(floatFields[idx] != SQL_NULL_DATA);                        \
      BOOST_CHECK(doubleFieldsInd[idx] != SQL_NULL_DATA);                    \
      BOOST_CHECK(boolFieldsInd[idx] != SQL_NULL_DATA);                      \
      BOOST_CHECK(dateFieldsInd[idx] != SQL_NULL_DATA);                      \
      BOOST_CHECK(timeFieldsInd[idx] != SQL_NULL_DATA);                      \
      BOOST_CHECK(timestampFieldsInd[idx] != SQL_NULL_DATA);                 \
      BOOST_CHECK(i8ArrayFieldsLen[idx] != SQL_NULL_DATA);                   \
                                                                             \
      int8_t i8Field = static_cast< int8_t >(i8Fields[idx]);                 \
      int16_t i16Field = static_cast< int16_t >(i16Fields[idx]);             \
      int32_t i32Field = static_cast< int32_t >(i32Fields[idx]);             \
      std::string strField(reinterpret_cast< char* >(&strFields[idx][0]),    \
                           static_cast< size_t >(strFieldsLen[idx]));        \
      float floatField = static_cast< float >(floatFields[idx]);             \
      double doubleField = static_cast< double >(doubleFields[idx]);         \
      bool boolField = boolFields[idx] != 0;                                 \
                                                                             \
      CheckTestI8Value(testIdx, i8Field);                                    \
      CheckTestI16Value(testIdx, i16Field);                                  \
      CheckTestI32Value(testIdx, i32Field);                                  \
      CheckTestStringValue(testIdx, strField);                               \
      CheckTestFloatValue(testIdx, floatField);                              \
      CheckTestDoubleValue(testIdx, doubleField);                            \
      CheckTestBoolValue(testIdx, boolField);                                \
      CheckTestDateValue(testIdx, dateFields[idx]);                          \
      CheckTestTimeValue(testIdx, timeFields[idx]);                          \
      CheckTestTimestampValue(testIdx, timestampFields[idx]);                \
      CheckTestI8ArrayValue(testIdx,                                         \
                            reinterpret_cast< int8_t* >(i8ArrayFields[idx]), \
                            static_cast< SQLLEN >(i8ArrayFieldsLen[idx]));   \
    }                                                                        \
  } while (false)

BOOST_AUTO_TEST_CASE(TestCursorBindingRowWise, *disabled()) {
  Connect(
      "DRIVER={Apache "
      "Ignite};ADDRESS=127.0.0.1:11110;SCHEMA=cache;PAGE_SIZE=8");

  SQLRETURN ret = SQLSetStmtAttr(stmt, SQL_ATTR_ROW_BIND_TYPE,
                                 reinterpret_cast< SQLPOINTER* >(42), 0);

  BOOST_CHECK_EQUAL(ret, SQL_ERROR);

  CheckSQLStatementDiagnosticError("HYC00");
}

BOOST_AUTO_TEST_SUITE_END()
