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

#ifndef _TIMESTREAM_ODBC_SYSTEM_ODBC_CONSTANTS
#define _TIMESTREAM_ODBC_SYSTEM_ODBC_CONSTANTS

#ifdef _WIN32

#define _WINSOCKAPI_
#include <windows.h>

#ifdef min
#undef min
#endif  // min

#endif  //_WIN32

#define ODBCVER 0x0351

#ifndef NAMEDATALEN
#define NAMEDATALEN 64
#endif /* NAMEDATALEN */

#ifndef MAX_CURSOR_LEN
#define MAX_CURSOR_LEN 32
#endif /* MAX_CURSOR_LEN */

// Internal SQL connection attribute to set log level
#define SQL_ATTR_TSLOG_DEBUG 65536

// Internal flag to use database as catalog or schema
// true if databases are reported as catalog, false if databases are reported as
// schema
#define DATABASE_AS_SCHEMA \
  timestream::odbc::utility::CheckEnvVarSetToTrue("DATABASE_AS_SCHEMA")

#include <odbcinst.h>
#include <sqlext.h>

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) (void)(x)
#endif  // UNREFERENCED_PARAMETER

#endif  //_TIMESTREAM_ODBC_SYSTEM_ODBC_CONSTANTS
