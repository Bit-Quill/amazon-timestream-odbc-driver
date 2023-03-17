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

#ifndef _IGNITE_ODBC_DSN_CONFIG
#define _IGNITE_ODBC_DSN_CONFIG

#include "ignite/odbc/config/configuration.h"
#include "ignite/odbc/ignite_error.h"
#include "sqltypes.h"

using namespace ignite::odbc::config;
using namespace ignite::odbc;

namespace ignite {
namespace odbc {
/**
 * Extract last setup error and throw it like IgniteError.
 */
void ThrowLastSetupError();

/**
 * Add new string to the DSN file.
 *
 * @param dsn DSN name.
 * @param key Key.
 * @param value Value.
 * @param error IgniteError
 */
bool WriteDsnString(const char* dsn, const char* key, const char* value,
                    IgniteError& error);

/**
 * Get string from the DSN file.
 *
 * @param dsn DSN name.
 * @param key Key.
 * @param dflt Default value.
 * @return Value.
 */
std::string ReadDsnString(const char* dsn, const char* key, const char* dflt);

/**
 * Read DSN to fill the configuration.
 *
 * @param dsn DSN name.
 * @param config Configuration.
 * @param diag Diagnostic collector.
 */
void ReadDsnConfiguration(const char* dsn, Configuration& config,
                          diagnostic::DiagnosticRecordStorage* diag);

/**
 * Write DSN from the configuration.
 *
 * @param config Configuration.
 * @param diag Diagnostic collector.
 */
bool WriteDsnConfiguration(const Configuration& config, IgniteError& error);

/**
 * Deletes a DSN from the system.
 *
 * @param dsn The DSN name to remove.
 * @param diag Diagnostic collector.
 */
bool DeleteDsnConfiguration(const std::string dsn, IgniteError& error);

/**
 * Register DSN with specified configuration.
 *
 * @param config Configuration.
 * @param driver Driver.
 * @return True on success and false on fail.
 */
bool RegisterDsn(const Configuration& config, const LPCSTR driver,
                 IgniteError& error);

/**
 * Unregister specified DSN.
 *
 * @param dsn DSN name.
 * @return True on success and false on fail.
 */
bool UnregisterDsn(const std::string& dsn, IgniteError& error);
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_DSN_CONFIG
