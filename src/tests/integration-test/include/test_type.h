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

#ifndef _IGNITE_ODBC_TEST_TEST_TYPE
#define _IGNITE_ODBC_TEST_TEST_TYPE

#include <string>
#include <ignite/odbc/date.h>

using namespace ignite::odbc;

namespace ignite {
namespace odbc {
struct TestType {
  TestType()
      : allNulls(false),
        i8Field(0),
        i16Field(0),
        i32Field(0),
        i64Field(0),
        floatField(0.0f),
        doubleField(0.0),
        boolField(false),
        dateField(),
        timeField(),
        timestampField() {
    // No-op.
  }

  TestType(int8_t i8Field, int16_t i16Field, int32_t i32Field, int64_t i64Field,
           const std::string& strField, float floatField, double doubleField,
           bool boolField, const Date& dateField,
           const Time& timeField, const Timestamp& timestampField)
      : allNulls(false),
        i8Field(i8Field),
        i16Field(i16Field),
        i32Field(i32Field),
        i64Field(i64Field),
        strField(strField),
        floatField(floatField),
        doubleField(doubleField),
        boolField(boolField),
        dateField(dateField),
        timeField(timeField),
        timestampField(timestampField) {
    // No-op.
  }

  friend bool operator==(const TestType& one, const TestType& two) {
    return one.allNulls == two.allNulls && one.i8Field == two.i8Field
           && one.i16Field == two.i16Field && one.i32Field == two.i32Field
           && one.i64Field == two.i64Field && one.strField == two.strField
           && one.floatField == two.floatField
           && one.doubleField == two.doubleField
           && one.boolField == two.boolField
           && one.dateField == two.dateField && one.timeField == two.timeField
           && one.timestampField == two.timestampField
           && one.i8ArrayField == two.i8ArrayField;
  }

  bool allNulls;
  int8_t i8Field;
  int16_t i16Field;
  int32_t i32Field;
  int64_t i64Field;
  std::string strField;
  float floatField;
  double doubleField;
  bool boolField;
  Date dateField;
  Time timeField;
  Timestamp timestampField;
  std::vector< int8_t > i8ArrayField;
};

}  // namespace odbc
}  // namespace ignite

#endif  // _IGNITE_ODBC_TEST_TEST_TYPE
