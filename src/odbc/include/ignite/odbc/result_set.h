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

#ifndef _IGNITE_ODBC_RESULT_SET
#define _IGNITE_ODBC_RESULT_SET


#include "ignite/odbc/ts_error.h"

#include <string>
#include <boost/optional.hpp>

namespace ignite {
namespace odbc {
/**
 * A wrapper class for the ResultSet java class.
 */
class ResultSet {

  /**
   * Constructs a new instancee of ResultSet.
   */
  ResultSet() {}

 public:
  /**
   * Destructs the current object.
   */
  ~ResultSet() {}

  /**
   * Gets an indicator of whether the ResultSet is open.
   *
   * @return true if open, false, otherwise.
   */
  bool IsOpen() {
    // not implemented
    return false;
  }

  /**
   * Attempts to position the cursor to the next record in the result set.
   * If the cursor moves, the hasNext variable will be true, false, otherwise.
   *
   * @return a TSErrorCode indicating success or failure.
   */
  TSErrorCode Next(bool& hasNext, TSErrorInfo& errInfo) {
    // not implemented
    return TSErrorCode::TS_ERR_GENERIC;
  }

  /**
   * Gets a value on the current row of the result set for the
   * given columnIndex (1-indexed). If a value exists, the value
   * is set. If the value is null, the wasNull will be set to
   * true, false, otherwise.
   *
   * @return a TSErrorCode indicating success or failure.
   */
  TSErrorCode GetString(const int columnIndex,
                         boost::optional< std::string >& value,
                      TSErrorInfo& errInfo) {
    // not implemented
    return TSErrorCode::TS_ERR_GENERIC;
  }

  /**
   * Gets a value on the current row of the result set for the
   * given columnName. If a value exists, the value
   * is set. If the value is null, the wasNull will be set to
   * true, false, otherwise.
   *
   * @return a TSErrorCode indicating success or failure.
   */
  TSErrorCode GetString(const std::string& columnName,
                         boost::optional< std::string >& value,
                      TSErrorInfo& errInfo) {
    // not implemented
    return TSErrorCode::TS_ERR_GENERIC;
  }

  /**
   * Gets a value on the current row of the result set for the
   * given columnIndex (1-indexed). If a value exists, the value
   * is set. If the value is null, the wasNull will be set to
   * true, false, otherwise.
   *
   * @return a TSErrorCode indicating success or failure.
   */
  TSErrorCode GetInt(const int columnIndex, boost::optional< int >& value,
                   TSErrorInfo& errInfo) {
    // not implemented
    return TSErrorCode::TS_ERR_GENERIC;
  }

  /**
   * Gets a value on the current row of the result set for the
   * given columnName. If a value exists, the value
   * is set. If the value is null, the wasNull will be set to
   * true, false, otherwise.
   *
   * @return a TSErrorCode indicating success or failure.
   */
  TSErrorCode GetInt(const std::string& columnName, boost::optional< int >& value,
                   TSErrorInfo& errInfo) {
    // not implemented
    return TSErrorCode::TS_ERR_GENERIC;
  }

  /**
   * Gets a value on the current row of the result set for the
   * given columnIndex (1-indexed). If a value exists, the value
   * is set. If the value is null, the wasNull will be set to
   * true, false, otherwise.
   *
   * @return a TSErrorCode indicating success or failure.
   */
  TSErrorCode GetSmallInt(const int columnIndex,
                           boost::optional< int16_t >& value, TSErrorInfo& errInfo) {
    // not implemented
    return TSErrorCode::TS_ERR_GENERIC;
  }

  /**
   * Gets a value on the current row of the result set for the
   * given columnName. If a value exists, the value
   * is set. If the value is null, the wasNull will be set to
   * true, false, otherwise.
   *
   * @return a TSErrorCode indicating success or failure.
   */
  TSErrorCode GetSmallInt(const std::string& columnName,
                           boost::optional< int16_t >& value, TSErrorInfo& errInfo) {
    // not implemented
    return TSErrorCode::TS_ERR_GENERIC;
  }
};
}  // namespace odbc
}  // namespace ignite

#endif  // _IGNITE_ODBC_RESULT_SET
