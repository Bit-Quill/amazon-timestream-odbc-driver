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

#include "timestream/odbc/timestamp.h"

namespace timestream {
namespace odbc {
Timestamp::Timestamp() : seconds(0), fractionNs(0) {
  // No-op.
}

Timestamp::Timestamp(const Timestamp& another)
    : seconds(another.seconds), fractionNs(another.fractionNs) {
  // No-op.
}

Timestamp::Timestamp(int64_t ms)
    : seconds(ms / 1000), fractionNs((ms % 1000) * 1000000) {
  // No-op.
}

Timestamp::Timestamp(int64_t seconds, int32_t fractionNs)
    : seconds(seconds), fractionNs(fractionNs) {
  // No-op.
}

Timestamp& Timestamp::operator=(const Timestamp& another) {
  seconds = another.seconds;
  fractionNs = another.fractionNs;

  return *this;
}

int64_t Timestamp::GetMilliseconds() const {
  return seconds * 1000 + fractionNs / 1000000;
}

int64_t Timestamp::GetSeconds() const {
  return seconds;
}

int32_t Timestamp::GetSecondFraction() const {
  return fractionNs;
}

Date Timestamp::GetDate() const {
  return Date(GetMilliseconds());
}

bool operator==(const Timestamp& val1, const Timestamp& val2) {
  return val1.seconds == val2.seconds && val1.fractionNs == val2.fractionNs;
}

bool operator!=(const Timestamp& val1, const Timestamp& val2) {
  return val1.seconds != val2.seconds || val1.fractionNs != val2.fractionNs;
}

bool operator<(const Timestamp& val1, const Timestamp& val2) {
  return val1.seconds < val2.seconds
         || (val1.seconds == val2.seconds && val1.fractionNs < val2.fractionNs);
}

bool operator<=(const Timestamp& val1, const Timestamp& val2) {
  return val1.seconds < val2.seconds
         || (val1.seconds == val2.seconds
             && val1.fractionNs <= val2.fractionNs);
}

bool operator>(const Timestamp& val1, const Timestamp& val2) {
  return val1.seconds > val2.seconds
         || (val1.seconds == val2.seconds && val1.fractionNs > val2.fractionNs);
}

bool operator>=(const Timestamp& val1, const Timestamp& val2) {
  return val1.seconds > val2.seconds
         || (val1.seconds == val2.seconds
             && val1.fractionNs >= val2.fractionNs);
}
}  // namespace odbc
}  // namespace timestream
