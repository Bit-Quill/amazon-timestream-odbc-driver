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

#include "ignite/odbc/interval_year_month.h"

namespace ignite {
namespace odbc {
IntervalYearMonth::IntervalYearMonth(const IntervalYearMonth& another)
    : year_(another.year_), month_(another.month_) {
  // No-op.
}

IntervalYearMonth::IntervalYearMonth(int32_t year, int32_t month)
    : year_(year), month_(month) {
  // No-op.
}

IntervalYearMonth& IntervalYearMonth::operator=(const IntervalYearMonth& another) {
  year_ = another.year_;
  month_ = another.month_;

  return *this;
}

bool operator==(const IntervalYearMonth& val1, const IntervalYearMonth& val2) {
  return (val1.year_ == val2.year_) && (val1.month_ == val2.month_);
}

bool operator!=(const IntervalYearMonth& val1, const IntervalYearMonth& val2) {
  return !(val1 == val2);
}

bool operator<(const IntervalYearMonth& val1, const IntervalYearMonth& val2) {
  return (val1.year_ < val2.year_) || ((val1.year_ == val2.year_) && (val1.month_ < val2.month_));
}

bool operator<=(const IntervalYearMonth& val1, const IntervalYearMonth& val2) {
  return (val1 < val2) || (val1 == val2);
}

bool operator>(const IntervalYearMonth& val1, const IntervalYearMonth& val2) {
  return !(val1 <= val2);
}

bool operator>=(const IntervalYearMonth& val1, const IntervalYearMonth& val2) {
  return !(val1 < val2);
}
}  // namespace odbc
}  // namespace ignite
