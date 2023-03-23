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

#include "timestream/timestream.h"
#include "timestream/ignition.h"
#include "timestream/odbc/impl/binary/binary_utils.h"
#include "sql_test_suite_fixture.h"
#include "test_type.h"
#include "test_utils.h"

using namespace timestream;
using namespace timestream::cache;
using namespace timestream::cache::query;
using namespace timestream::common;
using namespace timestream_test;

using namespace boost::unit_test;

using timestream::impl::binary::BinaryUtils;

BOOST_FIXTURE_TEST_SUITE(SqlStringFunctionTestSuite,
                         timestream::SqlTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestStringFunctionAscii, *disabled()) {
  TestType in;

  in.strField = "Hi";

  testCache.Put(1, in);

  CheckSingleResult< SQLINTEGER >("SELECT {fn ASCII(strField)} FROM TestType",
                                  static_cast< int32_t >('H'));
}

BOOST_AUTO_TEST_CASE(TestStringFunctionBitLength, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor";

  testCache.Put(1, in);

  CheckSingleResult< SQLBIGINT >(
      "SELECT {fn BIT_LENGTH(strField)} FROM TestType",
      in.strField.size() * 16);
}

BOOST_AUTO_TEST_CASE(TestStringFunctionChar, *disabled()) {
  TestType in;

  in.i32Field = static_cast< int32_t >('H');

  testCache.Put(1, in);

  CheckSingleResult< std::string >("SELECT {fn CHAR(i32Field)} FROM TestType",
                                   "H");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionCharLength, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor";

  testCache.Put(1, in);

  CheckSingleResult< SQLBIGINT >(
      "SELECT {fn CHAR_LENGTH(strField)} FROM TestType", in.strField.size());
}

BOOST_AUTO_TEST_CASE(TestStringFunctionCharacterLength, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor";

  testCache.Put(1, in);

  CheckSingleResult< SQLBIGINT >(
      "SELECT {fn CHARACTER_LENGTH(strField)} FROM TestType",
      in.strField.size());
}

BOOST_AUTO_TEST_CASE(TestStringFunctionConcat, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor sit amet,";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT {fn CONCAT(strField, \' consectetur adipiscing elit\')} FROM "
      "TestType",
      in.strField + " consectetur adipiscing elit");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionDifference, *disabled()) {
  TestType in;
  in.strField = "Hello";

  testCache.Put(1, in);

  CheckSingleResult< SQLINTEGER >(
      "SELECT {fn DIFFERENCE(strField, \'Hola!\')} FROM TestType", 4);
}

BOOST_AUTO_TEST_CASE(TestStringFunctionInsert, *disabled()) {
  TestType in;
  in.strField = "Hello World!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT {fn INSERT(strField, 7, 5, \'Ignite\')} FROM TestType",
      "Hello Ignite!");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionLcase, *disabled()) {
  TestType in;
  in.strField = "Hello World!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >("SELECT {fn LCASE(strField)} FROM TestType",
                                   "hello world!");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionLeft, *disabled()) {
  TestType in;
  in.strField = "Hello World!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT {fn LEFT(strField, 5)} FROM TestType", "Hello");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionLength, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";

  testCache.Put(1, in);

  CheckSingleResult< SQLBIGINT >("SELECT {fn LENGTH(strField)} FROM TestType",
                                 in.strField.size());
}

BOOST_AUTO_TEST_CASE(TestStringFunctionLocate, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";

  testCache.Put(1, in);

  CheckSingleResult< SQLBIGINT >(
      "SELECT {fn LOCATE(\'ip\', strField)} FROM TestType", 7);
}

BOOST_AUTO_TEST_CASE(TestStringFunctionLocate2, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";

  testCache.Put(1, in);

  CheckSingleResult< SQLBIGINT >(
      "SELECT {fn LOCATE(\'ip\', strField, 10)} FROM TestType", 43);
}

BOOST_AUTO_TEST_CASE(TestStringFunctionLtrim, *disabled()) {
  TestType in;
  in.strField = "    Lorem ipsum  ";

  testCache.Put(1, in);

  CheckSingleResult< std::string >("SELECT {fn LTRIM(strField)} FROM TestType",
                                   "Lorem ipsum  ");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionOctetLength, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";

  testCache.Put(1, in);

  CheckSingleResult< SQLBIGINT >(
      "SELECT {fn OCTET_LENGTH(strField)} FROM TestType",
      in.strField.size() * 2);
}

BOOST_AUTO_TEST_CASE(TestStringFunctionPosition, *disabled()) {
  TestType in;
  in.strField = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";

  testCache.Put(1, in);

  CheckSingleResult< SQLBIGINT >(
      "SELECT {fn POSITION(\'sit\', strField)} FROM TestType", 19);
}

BOOST_AUTO_TEST_CASE(TestStringFunctionRepeat, *disabled()) {
  TestType in;
  in.strField = "Test";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT {fn REPEAT(strField,4)} FROM TestType", "TestTestTestTest");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionReplace, *disabled()) {
  TestType in;
  in.strField = "Hello Ignite!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT {fn REPLACE(strField, \'Ignite\', \'World\')} FROM TestType",
      "Hello World!");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionRight, *disabled()) {
  TestType in;
  in.strField = "Hello World!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT {fn RIGHT(strField, 6)} FROM TestType", "World!");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionRtrim, *disabled()) {
  TestType in;
  in.strField = "    Lorem ipsum  ";

  testCache.Put(1, in);

  CheckSingleResult< std::string >("SELECT {fn RTRIM(strField)} FROM TestType",
                                   "    Lorem ipsum");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionSoundex, *disabled()) {
  TestType in;
  in.strField = "Hello Ignite!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT {fn SOUNDEX(strField)} FROM TestType", "H425");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionSpace, *disabled()) {
  CheckSingleResult< std::string >("SELECT {fn SPACE(10)}", "          ");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionSubstring, *disabled()) {
  TestType in;
  in.strField = "Hello Ignite!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT {fn SUBSTRING(strField, 7, 6)} FROM TestType", "Ignite");
}

BOOST_AUTO_TEST_CASE(TestStringFunctionUcase, *disabled()) {
  TestType in;
  in.strField = "Hello World!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >("SELECT {fn UCASE(strField)} FROM TestType",
                                   "HELLO WORLD!");
}

BOOST_AUTO_TEST_CASE(Test92StringFunctionLower, *disabled()) {
  TestType in;
  in.strField = "Hello World!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >("SELECT LOWER(strField) FROM TestType",
                                   "hello world!");
}

BOOST_AUTO_TEST_CASE(Test92StringFunctionUpper, *disabled()) {
  TestType in;
  in.strField = "Hello World!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >("SELECT UPPER(strField) FROM TestType",
                                   "HELLO WORLD!");
}

BOOST_AUTO_TEST_CASE(Test92StringFunctionSubstring, *disabled()) {
  TestType in;
  in.strField = "Hello Ignite!";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT SUBSTRING(strField, 7) FROM TestType", "Ignite!");
  CheckSingleResult< std::string >(
      "SELECT SUBSTRING(strField, 7, 6) FROM TestType", "Ignite");
  CheckSingleResult< std::string >(
      "SELECT SUBSTRING(strField FROM 7) FROM TestType", "Ignite!");
  CheckSingleResult< std::string >(
      "SELECT SUBSTRING(strField FROM 7 FOR 6) FROM TestType", "Ignite");
}

BOOST_AUTO_TEST_CASE(Test92StringFunctionTrimBoth, *disabled()) {
  TestType in;
  in.strField = "    Lorem ipsum  ";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT TRIM(BOTH FROM strField) FROM TestType", "Lorem ipsum");
}

BOOST_AUTO_TEST_CASE(Test92StringFunctionTrimLeading, *disabled()) {
  TestType in;
  in.strField = "    Lorem ipsum  ";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT TRIM(LEADING FROM strField) FROM TestType", "Lorem ipsum  ");
}

BOOST_AUTO_TEST_CASE(Test92StringFunctionTrimTrailing, *disabled()) {
  TestType in;
  in.strField = "    Lorem ipsum  ";

  testCache.Put(1, in);

  CheckSingleResult< std::string >(
      "SELECT TRIM(TRAILING FROM strField) FROM TestType", "    Lorem ipsum");
}

BOOST_AUTO_TEST_SUITE_END()
