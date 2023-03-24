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

#ifndef _TIMESTREAM_ODBC_SYSTEM_SYSTEM_DSN
#define _TIMESTREAM_ODBC_SYSTEM_SYSTEM_DSN

#include <timestream/odbc/system/odbc_constants.h>

namespace timestream {
namespace odbc {
namespace config {
class Configuration;
}
}  // namespace odbc
}  // namespace timestream

#ifdef _WIN32
/**
 * Display connection window for user to configure connection parameters.
 *
 * @param windowParent Parent window handle.
 * @param config Output configuration.
 * @return True on success and false on fail.
 */
bool DisplayConnectionWindow(void* windowParent,
                             timestream::odbc::config::Configuration& config);
#endif

#endif  //_TIMESTREAM_ODBC_SYSTEM_SYSTEM_DSN
