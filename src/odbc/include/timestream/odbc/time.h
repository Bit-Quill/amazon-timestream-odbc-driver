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

/**
 * @file
 * Declares timestream::odbc::Time class.
 */

#ifndef _TIMESTREAM_ODBC_TIME
#define _TIMESTREAM_ODBC_TIME

#include <ignite/common/common.h>
#include <stdint.h>

namespace timestream {
namespace odbc {
/**
 * %Time type.
 */
class IGNITE_IMPORT_EXPORT Time {
 public:
  /**
   * Default constructor.
   */
  Time();

  /**
   * Copy constructor.
   *
   * @param another Another instance.
   */
  Time(const Time& another);

  /**
   * Constructor.
   *
   * @param ms Time in milliseconds since 00:00:00 UTC.
   */
  Time(int64_t ms);

  /**
   * Constructor.
   *
   * @param sec Number of seconds since 00:00:00 UTC.
   * @param ns Fractional second component in nanoseconds.
   *     Must be in range [0..999999999].
   */
  Time(int32_t sec, int32_t ns);

  /**
   * Copy operator.
   *
   * @param another Another instance.
   * @return This.
   */
  Time& operator=(const Time& another);

  /**
   * Returns number of milliseconds since 00:00:00 UTC.
   *
   * @return Number of milliseconds since 00:00:00 UTC.
   */
  int64_t GetMilliseconds() const;

  /**
   * Returns number of seconds since 00:00:00 UTC.
   *
   * @return Number of seconds since 00:00:00 UTC.
   */
  int32_t GetSeconds() const;

  /**
   * Returns number of nanoseconds - fractional seconds component.
   *
   * @return Fractional second component expressed in nanoseconds.
   */
  int32_t GetSecondFraction() const;

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if equal.
   */
  friend bool IGNITE_IMPORT_EXPORT operator==(const Time& val1,
                                              const Time& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if not equal.
   */
  friend bool IGNITE_IMPORT_EXPORT operator!=(const Time& val1,
                                              const Time& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if less.
   */
  friend bool IGNITE_IMPORT_EXPORT operator<(const Time& val1,
                                             const Time& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if less or equal.
   */
  friend bool IGNITE_IMPORT_EXPORT operator<=(const Time& val1,
                                              const Time& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if gretter.
   */
  friend bool IGNITE_IMPORT_EXPORT operator>(const Time& val1,
                                             const Time& val2);

  /**
   * Comparison operator override.
   *
   * @param val1 First value.
   * @param val2 Second value.
   * @return True if gretter or equal.
   */
  friend bool IGNITE_IMPORT_EXPORT operator>=(const Time& val1,
                                              const Time& val2);

 private:
  /** Number of seconds since 00:00:00 UTC. */
  int32_t seconds;

  /** Fractional second component in nanoseconds. */
  int32_t fractionNs;
};
}  // namespace odbc
}  // namespace timestream
#endif  //_TIMESTREAM_ODBC_TIME
