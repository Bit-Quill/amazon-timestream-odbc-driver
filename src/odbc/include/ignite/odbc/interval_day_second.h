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

/**
 * @file
 * Declares ignite::odbc::IntervalDaySecond class.
 */

#ifndef _IGNITE_ODBC_INTERVAL_DAY_SECOND
#define _IGNITE_ODBC_INTERVAL_DAY_SECOND

#include <ignite/odbc/common/common.h>
#include <stdint.h>

namespace ignite {
namespace odbc {
/**
 * IntervalDaySecond type.
 */
class IGNITE_IMPORT_EXPORT IntervalDaySecond {
 public:
  /**
   * Copy constructor.
   *
   * @param another Another instance.
   */
  IntervalDaySecond(const IntervalDaySecond& another);

  /**
   * Constructor.
   *
   * @param day Number of days
   * @param hour Number of hours
   * @param min Number of minutes
   * @param sec Number of seconds
   * @param ns Number of nanoseconds
   */
  IntervalDaySecond(int32_t day, int32_t hour, int32_t min, int32_t sec, int32_t ns);

  /**
   * Copy operator.
   *
   * @param another Another instance.
   * @return This.
   */
  IntervalDaySecond& operator=(const IntervalDaySecond& another);

  /**
   * Returns number of days
   *
   * @return Number of days
   */
  int32_t GetDay() const {
    return day_;
  }
  
  /**
   * Returns number of hours
   *
   * @return Number of hours
   */
  int32_t GetHour() const {
    return hour_;
  }

  /**
   * Returns number of minutes
   *
   * @return Number of minutes
   */
  int32_t GetMinute() const {
    return minute_;
  }

  /**
   * Returns number of seconds
   *
   * @return Number of seconds
   */
  int32_t GetSecond() const {
    return second_;
  }

  /**
   * Returns number of nanoseconds
   *
   * @return Number of nanoseconds
   */
  int32_t GetFraction() const {
    return fractionNs_;
  }

  /**
   * Convert the interval to number of seconds 
   *
   * @return Number of seconds for the interval
   */
  int64_t InSeconds() const;

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if equal.
   */
  friend bool IGNITE_IMPORT_EXPORT operator==(const IntervalDaySecond& val1,
                                              const IntervalDaySecond& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if not equal.
   */
  friend bool IGNITE_IMPORT_EXPORT operator!=(const IntervalDaySecond& val1,
                                              const IntervalDaySecond& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if less.
   */
  friend bool IGNITE_IMPORT_EXPORT operator<(const IntervalDaySecond& val1,
                                             const IntervalDaySecond& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if less or equal.
   */
  friend bool IGNITE_IMPORT_EXPORT operator<=(const IntervalDaySecond& val1,
                                              const IntervalDaySecond& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if gretter.
   */
  friend bool IGNITE_IMPORT_EXPORT operator>(const IntervalDaySecond& val1,
                                             const IntervalDaySecond& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if gretter or equal.
   */
  friend bool IGNITE_IMPORT_EXPORT operator>=(const IntervalDaySecond& val1,
                                              const IntervalDaySecond& val2);

 private:
  /** Number of days. */
  int32_t day_;

  /** Number of hours. */
  int32_t hour_;

  /** Number of minutes. */
  int32_t minute_;

  /** Number of seconds. */
  int32_t second_;

  /** Number of nanoseconds. */
  int32_t fractionNs_;
};
}  // namespace odbc
}  // namespace ignite
#endif  //_IGNITE_ODBC_INTERVAL_DAY_SECOND
