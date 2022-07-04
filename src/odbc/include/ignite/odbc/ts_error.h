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

#ifndef _IGNITE_ODBC_TS_ERROR
#define _IGNITE_ODBC_TS_ERROR

#include <ignite/odbc/common/common.h>
#include <string>

namespace ignite {
namespace odbc {
enum class TSErrorCode {
  TS_ERR_SUCCESS = 0,
  TS_ERR_GENERIC = 1
};


/**
 * TimeStream error information.
 */
struct IGNITE_IMPORT_EXPORT TSErrorInfo {
  TSErrorCode code;
  std::string errCls;
  std::string errMsg;

  /**
   * Default constructor. Creates empty error info.
   */
  TSErrorInfo();

  /**
   * Constructor.
   *
   * @param code Code.
   * @param errCls TsError class.
   * @param errMsg TsError message.
   */
  TSErrorInfo(TSErrorCode code, const char* errCls, const char* errMsg);

  /**
   * Copy constructor.
   *
   * @param other Other instance.
   */
  TSErrorInfo(const TSErrorInfo& other);

  /**
   * Assignment operator overload.
   *
   * @param other Other instance.
   * @return This instance.
   */
  TSErrorInfo& operator=(const TSErrorInfo& other);

  /**
   * Destructor.
   */
  ~TSErrorInfo() = default;
};
}  // namespace odbc
}  // namespace ignite
#endif  //_IGNITE_ODBC_TS_ERROR
