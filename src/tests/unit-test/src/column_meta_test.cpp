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

#include <sqlext.h>

#include <boost/test/unit_test.hpp>
#include <utility>

#include "ignite/odbc/impl/binary/binary_common.h"
#include "ignite/odbc/meta/column_meta.h"
#include "ignite/odbc/type_traits.h"
#include "odbc_test_suite.h"

using namespace ignite::odbc::impl::binary;
using ignite::odbc::OdbcTestSuite;
using ignite::odbc::meta::ColumnMeta;
using ignite::odbc::meta::Nullability;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(TestGetAttribute) {
  // Only SQL_DESC_* fields are tested in this test.
  // This is because those are the fields that would be passed to
  // SQLColAttribute function.
  using namespace ignite::odbc::type_traits;

  std::string schema("database");
  std::string table("table");
  std::string column("column");

  ColumnMeta columnMeta(schema, table, column,
                        static_cast< int16_t >(ScalarType::VARCHAR),
                        Nullability::NULLABLE);

  SQLLEN intVal;
  std::string resVal;
  bool found;

  // test retrieving std::string value

  // test SQL_DESC_LABEL
  found = columnMeta.GetAttribute(SQL_DESC_LABEL, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, column);

  // test SQL_DESC_BASE_COLUMN_NAME
  found = columnMeta.GetAttribute(SQL_DESC_BASE_COLUMN_NAME, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, column);

  // test SQL_DESC_NAME
  found = columnMeta.GetAttribute(SQL_DESC_NAME, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, column);

  // test SQL_DESC_TABLE_NAME
  found = columnMeta.GetAttribute(SQL_DESC_TABLE_NAME, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, table);

  // test SQL_DESC_BASE_TABLE_NAME
  found = columnMeta.GetAttribute(SQL_DESC_BASE_TABLE_NAME, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, table);

  // test SQL_DESC_SCHEMA_NAME
  found = columnMeta.GetAttribute(SQL_DESC_SCHEMA_NAME, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, schema);

  // test SQL_DESC_CATALOG_NAME
  found = columnMeta.GetAttribute(SQL_DESC_CATALOG_NAME, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, "");

  // test SQL_DESC_LITERAL_PREFIX
  found = columnMeta.GetAttribute(SQL_DESC_LITERAL_PREFIX, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, "'");

  // test SQL_DESC_LITERAL_SUFFIX
  found = columnMeta.GetAttribute(SQL_DESC_LITERAL_SUFFIX, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, "'");

  // test SQL_DESC_TYPE_NAME
  found = columnMeta.GetAttribute(SQL_DESC_TYPE_NAME, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, SqlTypeName::WVARCHAR);

  // test SQL_DESC_LOCAL_TYPE_NAME
  found = columnMeta.GetAttribute(SQL_DESC_LOCAL_TYPE_NAME, resVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(resVal, SqlTypeName::WVARCHAR);

  // fields SQL_COLUMN_PRECISION and SQL_DESC_SCALE are not tested
  // for retrieving string values

  // test retrieving SQLLEN value

  // test SQL_DESC_FIXED_PREC_SCALE
  found = columnMeta.GetAttribute(SQL_DESC_FIXED_PREC_SCALE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_FALSE);

  // test SQL_DESC_AUTO_UNIQUE_VALUE
  found = columnMeta.GetAttribute(SQL_DESC_AUTO_UNIQUE_VALUE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_FALSE);

  // test SQL_DESC_CASE_SENSITIVE
  found = columnMeta.GetAttribute(SQL_DESC_CASE_SENSITIVE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_TRUE);

  // test SQL_DESC_CONCISE_TYPE
  found = columnMeta.GetAttribute(SQL_DESC_CONCISE_TYPE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_WVARCHAR);

  // test SQL_DESC_TYPE
  found = columnMeta.GetAttribute(SQL_DESC_TYPE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_WVARCHAR);

  // test SQL_DESC_DISPLAY_SIZE
  found = columnMeta.GetAttribute(SQL_DESC_DISPLAY_SIZE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NO_TOTAL);

  // test SQL_DESC_LENGTH
  found = columnMeta.GetAttribute(SQL_DESC_LENGTH, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NO_TOTAL);

  // test SQL_DESC_OCTET_LENGTH
  found = columnMeta.GetAttribute(SQL_DESC_OCTET_LENGTH, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NO_TOTAL);

  // test SQL_DESC_NULLABLE
  found = columnMeta.GetAttribute(SQL_DESC_NULLABLE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NULLABLE);

  // test SQL_DESC_NUM_PREC_RADIX
  found = columnMeta.GetAttribute(SQL_DESC_NUM_PREC_RADIX, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, 0);

  // test SQL_DESC_PRECISION
  found = columnMeta.GetAttribute(SQL_DESC_PRECISION, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NO_TOTAL);

  // test SQL_DESC_SCALE
  found = columnMeta.GetAttribute(SQL_DESC_SCALE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, -1);

  // test SQL_DESC_SEARCHABLE
  found = columnMeta.GetAttribute(SQL_DESC_SEARCHABLE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_PRED_BASIC);

  // test SQL_DESC_UNNAMED
  found = columnMeta.GetAttribute(SQL_DESC_UNNAMED, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NAMED);

  // test SQL_DESC_UNSIGNED
  found = columnMeta.GetAttribute(SQL_DESC_UNSIGNED, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_TRUE);

  // test SQL_DESC_UPDATABLE
  found = columnMeta.GetAttribute(SQL_DESC_UPDATABLE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_ATTR_READWRITE_UNKNOWN);
}

BOOST_AUTO_TEST_CASE(TestGetAttributeLiteralPrefix, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, std::string > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving std::string value

    // test SQL_DESC_LITERAL_PREFIX
    found = columnMeta.GetAttribute(SQL_DESC_LITERAL_PREFIX, resVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(resVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeLiteralSuffix, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector<std::pair< int16_t, std::string >> tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving std::string value

    // test SQL_DESC_LITERAL_SUFFIX
    found = columnMeta.GetAttribute(SQL_DESC_LITERAL_SUFFIX, resVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(resVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeLocalTypeName, *disabled()) {
  using namespace ignite::odbc::type_traits;

  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, std::string > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving std::string value

    // test SQL_DESC_LOCAL_TYPE_NAME
    found = columnMeta.GetAttribute(SQL_DESC_LOCAL_TYPE_NAME, resVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(resVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeCaseSensitive, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector<std::pair< int16_t, SQLLEN >> tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_CASE_SENSITIVE
    found = columnMeta.GetAttribute(SQL_DESC_CASE_SENSITIVE, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeConciseTypeAndType, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, SQLLEN > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_CONCISE_TYPE
    found = columnMeta.GetAttribute(SQL_DESC_CONCISE_TYPE, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);

    // test SQL_DESC_TYPE
    found = columnMeta.GetAttribute(SQL_DESC_TYPE, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeDisplaySize, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, SQLLEN > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_DISPLAY_SIZE
    found = columnMeta.GetAttribute(SQL_DESC_DISPLAY_SIZE, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeLength, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, SQLLEN > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_LENGTH
    found = columnMeta.GetAttribute(SQL_DESC_LENGTH, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeOctetLength, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;
  size_t size_of_char = sizeof(char);

  std::vector< std::pair< int16_t, SQLLEN > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_OCTET_LENGTH
    found = columnMeta.GetAttribute(SQL_DESC_OCTET_LENGTH, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeNullable, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;
  ColumnMeta columnMetaNullable(schema, table, column, TS_INVALID_TYPE,
                                Nullability::NULLABLE);

  // test SQL_DESC_NULLABLE
  found = columnMetaNullable.GetAttribute(SQL_DESC_NULLABLE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NULLABLE);

  ColumnMeta columnMetaNoNulls(schema, table, column, TS_INVALID_TYPE,
                               Nullability::NO_NULL);

  // test SQL_DESC_NULLABLE
  found = columnMetaNoNulls.GetAttribute(SQL_DESC_NULLABLE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NO_NULLS);

  ColumnMeta columnMetaUnknown(schema, table, column, TS_INVALID_TYPE,
                               Nullability::NULLABILITY_UNKNOWN);

  // test SQL_DESC_NULLABLE
  found = columnMetaUnknown.GetAttribute(SQL_DESC_NULLABLE, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NULLABLE_UNKNOWN);
}

BOOST_AUTO_TEST_CASE(TestGetAttributeNumPrecRadix, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, SQLLEN > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_NUM_PREC_RADIX
    found = columnMeta.GetAttribute(SQL_DESC_NUM_PREC_RADIX, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributePrecision, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, SQLLEN > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_PRECISION
    found = columnMeta.GetAttribute(SQL_DESC_PRECISION, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeScale, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, SQLLEN > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_SCALE
    found = columnMeta.GetAttribute(SQL_DESC_SCALE, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}

BOOST_AUTO_TEST_CASE(TestGetAttributeUnnamed, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;
  ColumnMeta columnMetaUnnamed(schema, table, std::string(""), TS_INVALID_TYPE,
                               Nullability::NULLABLE);

  // test SQL_DESC_UNNAMED
  found = columnMetaUnnamed.GetAttribute(SQL_DESC_UNNAMED, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_UNNAMED);

  ColumnMeta columnMetaNamed(schema, table, column, TS_INVALID_TYPE,
                             Nullability::NULLABLE);

  // test SQL_DESC_UNNAMED
  found = columnMetaNamed.GetAttribute(SQL_DESC_UNNAMED, intVal);
  BOOST_CHECK(found);
  BOOST_CHECK_EQUAL(intVal, SQL_NAMED);
}

BOOST_AUTO_TEST_CASE(TestGetAttributeUnsigned, *disabled()) {
  std::string schema("database");
  std::string table("table");
  std::string column("column");

  SQLLEN intVal;
  std::string resVal;
  bool found;

  std::vector< std::pair< int16_t, SQLLEN > > tests;

  for (int i = 0; i < tests.size(); i++) {
    ColumnMeta columnMeta(schema, table, column, tests[i].first,
                          Nullability::NULLABLE);
    // test retrieving SQLLEN value

    // test SQL_DESC_UNSIGNED
    found = columnMeta.GetAttribute(SQL_DESC_UNSIGNED, intVal);
    BOOST_CHECK(found);
    BOOST_CHECK_EQUAL(intVal, tests[i].second);
  }
}
