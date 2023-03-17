/*
 * Copyright <2022> Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 */

#include "ignite/odbc/time.h"

namespace ignite {
namespace odbc {
Time::Time() : seconds(0), fractionNs(0) {
  // No-op.
}

Time::Time(const Time& another) : seconds(another.seconds), fractionNs(another.fractionNs) {
  // No-op.
}

Time::Time(int64_t ms) : seconds(ms / 1000), fractionNs((ms%1000)*100000) {
  // No-op.
}

Time::Time(int32_t sec, int32_t ns) : seconds(sec), fractionNs(ns) {
  // No-op.
}

Time& Time::operator=(const Time& another) {
  seconds = another.seconds;
  fractionNs = another.fractionNs;

  return *this;
}

int64_t Time::GetMilliseconds() const {
  return seconds * 1000 + fractionNs/1000;
}

int32_t Time::GetSeconds() const {
  return seconds;
}

int32_t Time::GetSecondFraction() const {
  return fractionNs;
}

bool operator==(const Time& val1, const Time& val2) {
  return val1.seconds == val2.seconds
         && val1.fractionNs == val2.fractionNs;
}

bool operator!=(const Time& val1, const Time& val2) {
  return val1.seconds != val2.seconds || val1.fractionNs != val2.fractionNs;
}

bool operator<(const Time& val1, const Time& val2) {
  return val1.seconds < val2.seconds
         || (val1.seconds == val2.seconds && val1.fractionNs < val2.fractionNs);
}

bool operator<=(const Time& val1, const Time& val2) {
  return val1.seconds < val2.seconds
         || (val1.seconds == val2.seconds
             && val1.fractionNs <= val2.fractionNs);
}

bool operator>(const Time& val1, const Time& val2) {
  return val1.seconds > val2.seconds
         || (val1.seconds == val2.seconds && val1.fractionNs > val2.fractionNs);
}

bool operator>=(const Time& val1, const Time& val2) {
  return val1.seconds > val2.seconds
         || (val1.seconds == val2.seconds
             && val1.fractionNs >= val2.fractionNs);
}
}  // namespace odbc
}  // namespace ignite
