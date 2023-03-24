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

#include "timestream/odbc/diagnostic/diagnosable_adapter.h"

#include "timestream/odbc/connection.h"
#include "timestream/odbc/log.h"
#include "ignite/odbc/odbc_error.h"

namespace timestream {
namespace odbc {
namespace diagnostic {
void DiagnosableAdapter::AddStatusRecord(SqlState::Type sqlState,
                                         const std::string& message,
                                         timestream::odbc::LogLevel::Type logLevel,
                                         int32_t rowNum, int32_t columnNum) {
  WRITE_LOG_MSG("Adding new record: " << message << ", rowNum: " << rowNum
                                      << ", columnNum: " << columnNum,
                logLevel);

  if (connection) {
    diagnosticRecords.AddStatusRecord(
        connection->CreateStatusRecord(sqlState, message, rowNum, columnNum));
  } else {
    diagnosticRecords.AddStatusRecord(
        DiagnosticRecord(sqlState, message, "", "", rowNum, columnNum));
  }
}

void DiagnosableAdapter::AddStatusRecord(
    SqlState::Type sqlState, const std::string& message,
    timestream::odbc::LogLevel::Type logLevel) {
  AddStatusRecord(sqlState, message, logLevel, 0, 0);
}

void DiagnosableAdapter::AddStatusRecord(const std::string& message) {
  AddStatusRecord(SqlState::SHY000_GENERAL_ERROR, message);
}

void DiagnosableAdapter::AddStatusRecord(const ignite::odbc::OdbcError& err) {
  AddStatusRecord(err.GetStatus(), err.GetErrorMessage(),
                  timestream::odbc::LogLevel::Type::ERROR_LEVEL, 0, 0);
}

void DiagnosableAdapter::AddStatusRecord(const DiagnosticRecord& rec) {
  diagnosticRecords.AddStatusRecord(rec);
}
}  // namespace diagnostic
}  // namespace odbc
}  // namespace timestream
