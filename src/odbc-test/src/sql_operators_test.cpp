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

#include <ignite/odbc/common/decimal.h>

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#include "sql_test_suite_fixture.h"
#include "test_type.h"

using namespace ignite;

BOOST_FIXTURE_TEST_SUITE(SqlOperatorTestSuite, ignite::SqlTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestOperatorAddInt, *disabled()) {
  CheckSingleResult< SQLINTEGER >("SELECT 123 + 51", 123 + 51);
}

BOOST_AUTO_TEST_CASE(TestOperatorSubInt, *disabled()) {
  CheckSingleResult< SQLINTEGER >("SELECT 123 - 51", 123 - 51);
}

BOOST_AUTO_TEST_CASE(TestOperatorDivInt, *disabled()) {
  CheckSingleResult< SQLINTEGER >("SELECT 123 / 51", 123 / 51);
}

BOOST_AUTO_TEST_CASE(TestOperatorModInt, *disabled()) {
  CheckSingleResult< SQLINTEGER >("SELECT 123 % 51", 123 % 51);
}

BOOST_AUTO_TEST_CASE(TestOperatorMultInt, *disabled()) {
  CheckSingleResult< SQLINTEGER >("SELECT 123 * 51", 123 * 51);
}

BOOST_AUTO_TEST_CASE(TestOperatorAddDouble, *disabled()) {
  CheckSingleResult< double >("SELECT 123.0 + 51.0", 123.0 + 51.0);
}

BOOST_AUTO_TEST_CASE(TestOperatorSubDouble, *disabled()) {
  CheckSingleResult< double >("SELECT 123.0 - 51.0", 123.0 - 51.0);
}

BOOST_AUTO_TEST_CASE(TestOperatorDivDouble, *disabled()) {
  CheckSingleResult< double >("SELECT 123.0 / 51.0", 123.0 / 51.0);
}

BOOST_AUTO_TEST_CASE(TestOperatorModDouble, *disabled()) {
  CheckSingleResult< double >("SELECT 123.0 % 51.0", 123 % 51);
}

BOOST_AUTO_TEST_CASE(TestOperatorMultDouble, *disabled()) {
  CheckSingleResult< double >("SELECT 123.0 * 51.0", 123.0 * 51.0);
}

BOOST_AUTO_TEST_CASE(TestOperatorConcatString, *disabled()) {
  CheckSingleResult< std::string >("SELECT 'Hello' || ' ' || 'World' || '!'",
                                   "Hello World!");
}

BOOST_AUTO_TEST_CASE(TestOperatorGreaterInt, *disabled()) {
  CheckSingleResult< bool >("SELECT 2 > 3", false);
  CheckSingleResult< bool >("SELECT 3 > 3", false);
  CheckSingleResult< bool >("SELECT 34 > 3", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorLessInt, *disabled()) {
  CheckSingleResult< bool >("SELECT 4 < 4", false);
  CheckSingleResult< bool >("SELECT 4 < 4", false);
  CheckSingleResult< bool >("SELECT 8 < 42", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorGreaterEquealInt, *disabled()) {
  CheckSingleResult< bool >("SELECT 2 >= 3", false);
  CheckSingleResult< bool >("SELECT 3 >= 3", true);
  CheckSingleResult< bool >("SELECT 34 >= 3", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorLessEquealInt, *disabled()) {
  CheckSingleResult< bool >("SELECT 4 <= 3", false);
  CheckSingleResult< bool >("SELECT 4 <= 4", true);
  CheckSingleResult< bool >("SELECT 8 <= 42", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorEquealInt, *disabled()) {
  CheckSingleResult< bool >("SELECT 4 = 3", false);
  CheckSingleResult< bool >("SELECT 4 = 4", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorNotEquealInt, *disabled()) {
  CheckSingleResult< bool >("SELECT 4 != 3", true);
  CheckSingleResult< bool >("SELECT 4 != 4", false);
}

BOOST_AUTO_TEST_CASE(TestOperatorGreaterDouble, *disabled()) {
  CheckSingleResult< bool >("SELECT 2 > 3", false);
  CheckSingleResult< bool >("SELECT 3 > 3", false);
  CheckSingleResult< bool >("SELECT 34 > 3", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorLessDouble, *disabled()) {
  CheckSingleResult< bool >("SELECT 4.0 < 4.0", false);
  CheckSingleResult< bool >("SELECT 4.0 < 4.0", false);
  CheckSingleResult< bool >("SELECT 8.0 < 42.0", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorGreaterEquealDouble, *disabled()) {
  CheckSingleResult< bool >("SELECT 2.0 >= 3.0", false);
  CheckSingleResult< bool >("SELECT 3.0 >= 3.0", true);
  CheckSingleResult< bool >("SELECT 34.0 >= 3.0", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorLessEquealDouble, *disabled()) {
  CheckSingleResult< bool >("SELECT 4.0 <= 3.0", false);
  CheckSingleResult< bool >("SELECT 4.0 <= 4.0", true);
  CheckSingleResult< bool >("SELECT 8.0 <= 42.0", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorEquealDouble, *disabled()) {
  CheckSingleResult< bool >("SELECT 4.0 = 3.0", false);
  CheckSingleResult< bool >("SELECT 4.0 = 4.0", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorNotEquealDouble, *disabled()) {
  CheckSingleResult< bool >("SELECT 4.0 != 3.0", true);
  CheckSingleResult< bool >("SELECT 4.0 != 4.0", false);
}

BOOST_AUTO_TEST_CASE(TestOperatorGreaterString, *disabled()) {
  CheckSingleResult< bool >("SELECT 'abc' > 'bcd'", false);
  CheckSingleResult< bool >("SELECT 'abc' > 'abc'", false);
  CheckSingleResult< bool >("SELECT 'bcd' > 'abc'", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorLessString, *disabled()) {
  CheckSingleResult< bool >("SELECT 'bcd' < 'abc'", false);
  CheckSingleResult< bool >("SELECT 'abc' < 'abc'", false);
  CheckSingleResult< bool >("SELECT 'abc' < 'bcd'", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorGreaterEquealString, *disabled()) {
  CheckSingleResult< bool >("SELECT 'abc' >= 'bcd'", false);
  CheckSingleResult< bool >("SELECT 'abc' >= 'abc'", true);
  CheckSingleResult< bool >("SELECT 'bcd' >= 'abc'", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorLessEquealString, *disabled()) {
  CheckSingleResult< bool >("SELECT 'bcd' <= 'abc'", false);
  CheckSingleResult< bool >("SELECT 'abc' <= 'bcd'", true);
  CheckSingleResult< bool >("SELECT 'abc' <= 'abc'", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorEquealString, *disabled()) {
  CheckSingleResult< bool >("SELECT 'bcd' = 'abc'", false);
  CheckSingleResult< bool >("SELECT 'abc' = 'abc'", true);
}

BOOST_AUTO_TEST_CASE(TestOperatorNotEquealString, *disabled()) {
  CheckSingleResult< bool >("SELECT 'abc' != 'abc'", false);
  CheckSingleResult< bool >("SELECT 'bcd' != 'abc'", true);
}

BOOST_AUTO_TEST_SUITE_END()
