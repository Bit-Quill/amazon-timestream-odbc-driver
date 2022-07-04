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

#include "ignite/odbc/log.h"
#include "ignite/odbc/ts_error.h"
#include <utility>

namespace ignite {
namespace odbc {

TSErrorInfo::TSErrorInfo() : code(TSErrorCode::TS_ERR_SUCCESS) {
  // No-op.
}

TSErrorInfo::TSErrorInfo(TSErrorCode code, const char* errCls,
                           const char* errMsg)
    : code(code), errCls(errCls), errMsg(errMsg) {
}

TSErrorInfo::TSErrorInfo(const TSErrorInfo& other) = default;

TSErrorInfo& TSErrorInfo::operator=(const TSErrorInfo& other) {
  LOG_DEBUG_MSG("TSErrorInfo constructor is called");

  if (this != &other) {
    // 1. Create new instance, exception could occur at this point.
    TSErrorInfo tmp(other);

    // 2. Swap with temp.
    std::swap(code, tmp.code);
    std::swap(errCls, tmp.errCls);
    std::swap(errMsg, tmp.errMsg);
  }

  LOG_DEBUG_MSG("TSErrorInfo constructor exiting");

  return *this;
}

}  //  namespace odbc
}  //  namespace ignite
