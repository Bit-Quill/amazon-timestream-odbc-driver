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

#include "ignite/odbc/connection.h"

#include <ignite/odbc/ignite_error.h>

#include <algorithm>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <cstddef>
#include <cstring>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/uri.hpp>
#include <sstream>

#include "ignite/odbc/driver_instance.h"
#include "ignite/odbc/common/concurrent.h"
#include "ignite/odbc/common/utils.h"
#include "ignite/odbc/config/configuration.h"
#include "ignite/odbc/config/connection_string_parser.h"
#include "ignite/odbc/dsn_config.h"
#include "ignite/odbc/environment.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/message.h"
#include "ignite/odbc/ssl_mode.h"
#include "ignite/odbc/statement.h"
#include "ignite/odbc/system/system_dsn.h"
#include "ignite/odbc/utility.h"

using namespace ignite::odbc::common;
using namespace ignite::odbc::common::concurrent;
using ignite::odbc::IgniteError;


// Uncomment for per-byte debug.
//#define PER_BYTE_DEBUG

namespace {
#pragma pack(push, 1)
struct OdbcProtocolHeader {
  int32_t len;
};
#pragma pack(pop)
}  // namespace

namespace ignite {
namespace odbc {
Connection::Connection(Environment* env) : env_(env), info_(config_) {
  // No-op
}

Connection::~Connection() {
  Close();
}

const config::ConnectionInfo& Connection::GetInfo() const {
  return info_;
}

void Connection::GetInfo(config::ConnectionInfo::InfoType type, void* buf,
                         short buflen, short* reslen) {
  LOG_MSG("SQLGetInfo called: "
          << type << " (" << config::ConnectionInfo::InfoTypeToString(type)
          << "), " << std::hex << reinterpret_cast< size_t >(buf) << ", "
          << buflen << ", " << std::hex << reinterpret_cast< size_t >(reslen)
          << std::dec);

  IGNITE_ODBC_API_CALL(InternalGetInfo(type, buf, buflen, reslen));
}

SqlResult::Type Connection::InternalGetInfo(
    config::ConnectionInfo::InfoType type, void* buf, short buflen,
    short* reslen) {
  const config::ConnectionInfo& info = GetInfo();

  SqlResult::Type res = info.GetInfo(type, buf, buflen, reslen);

  if (res != SqlResult::AI_SUCCESS)
    AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                    "Not implemented.");

  return res;
}

void Connection::Establish(const std::string& connectStr, void* parentWindow) {
  IGNITE_ODBC_API_CALL(InternalEstablish(connectStr, parentWindow));
}

SqlResult::Type Connection::InternalEstablish(const std::string& connectStr,
                                              void* parentWindow) {
  config::Configuration config;
  config::ConnectionStringParser parser(config);
  parser.ParseConnectionString(connectStr, &GetDiagnosticRecords());

  if (config.IsDsnSet()) {
    std::string dsn = config.GetDsn();

    ReadDsnConfiguration(dsn.c_str(), config, &GetDiagnosticRecords());
  }

#ifdef _WIN32
  if (parentWindow) {
    LOG_MSG("Parent window is passed. Creating configuration window.");
    if (!DisplayConnectionWindow(parentWindow, config)) {
      AddStatusRecord(odbc::SqlState::SHY008_OPERATION_CANCELED,
                      "Connection canceled by user");

      return SqlResult::AI_ERROR;
    }
  }
#endif  // _WIN32

  return InternalEstablish(config);
}

void Connection::Establish(const config::Configuration cfg) {
  IGNITE_ODBC_API_CALL(InternalEstablish(cfg));
}

SqlResult::Type Connection::InternalEstablish(
    const config::Configuration& cfg) {
  // not implemented
  return SqlResult::AI_ERROR;
}

void Connection::Release() {
  IGNITE_ODBC_API_CALL(InternalRelease());
}

void Connection::Deregister() {
  env_->DeregisterConnection(this);
}

SqlResult::Type Connection::InternalRelease() {
  return SqlResult::AI_ERROR;
}

void Connection::Close() {
}

Statement* Connection::CreateStatement() {
  Statement* statement;

  IGNITE_ODBC_API_CALL(InternalCreateStatement(statement));

  return statement;
}

SqlResult::Type Connection::InternalCreateStatement(Statement*& statement) {
  statement = new Statement(*this);

  if (!statement) {
    AddStatusRecord(SqlState::SHY001_MEMORY_ALLOCATION, "Not enough memory.");

    return SqlResult::AI_ERROR;
  }

  return SqlResult::AI_SUCCESS;
}

const std::string& Connection::GetSchema() const {
  return config_.GetDatabase();
}

const config::Configuration& Connection::GetConfiguration() const {
  return config_;
}

bool Connection::IsAutoCommit() const {
  return autoCommit_;
}

diagnostic::DiagnosticRecord Connection::CreateStatusRecord(
    SqlState::Type sqlState, const std::string& message, int32_t rowNum,
    int32_t columnNum) {
  return diagnostic::DiagnosticRecord(sqlState, message, "", "", rowNum,
                                      columnNum);
}

void Connection::TransactionCommit() {
  IGNITE_ODBC_API_CALL(InternalTransactionCommit());
}

SqlResult::Type Connection::InternalTransactionCommit() {
  std::string schema = config_.GetDatabase();

  app::ParameterSet empty;

  QueryExecuteRequest req(schema, "COMMIT", empty, timeout_, autoCommit_);
  QueryExecuteResponse rsp;

  try {
    bool sent = SyncMessage(req, rsp, timeout_);

    if (!sent) {
      AddStatusRecord(SqlState::S08S01_LINK_FAILURE,
                      "Failed to send commit request.");

      return SqlResult::AI_ERROR;
    }
  } catch (const OdbcError& err) {
    AddStatusRecord(err);

    return SqlResult::AI_ERROR;
  } catch (const IgniteError& err) {
    AddStatusRecord(err.GetText());

    return SqlResult::AI_ERROR;
  }

  return SqlResult::AI_SUCCESS;
}

void Connection::TransactionRollback() {
  IGNITE_ODBC_API_CALL(InternalTransactionRollback());
}

SqlResult::Type Connection::InternalTransactionRollback() {
  std::string schema = config_.GetDatabase();

  app::ParameterSet empty;

  QueryExecuteRequest req(schema, "ROLLBACK", empty, timeout_, autoCommit_);
  QueryExecuteResponse rsp;

  try {
    bool sent = SyncMessage(req, rsp, timeout_);

    if (!sent) {
      AddStatusRecord(SqlState::S08S01_LINK_FAILURE,
                      "Failed to send rollback request.");

      return SqlResult::AI_ERROR;
    }
  } catch (const OdbcError& err) {
    AddStatusRecord(err);

    return SqlResult::AI_ERROR;
  } catch (const IgniteError& err) {
    AddStatusRecord(err.GetText());

    return SqlResult::AI_ERROR;
  }

  return SqlResult::AI_SUCCESS;
}

void Connection::GetAttribute(int attr, void* buf, SQLINTEGER bufLen,
                              SQLINTEGER* valueLen) {
  IGNITE_ODBC_API_CALL(InternalGetAttribute(attr, buf, bufLen, valueLen));
}

SqlResult::Type Connection::InternalGetAttribute(int attr, void* buf,
                                                 SQLINTEGER,
                                                 SQLINTEGER* valueLen) {
  if (!buf) {
    AddStatusRecord(SqlState::SHY009_INVALID_USE_OF_NULL_POINTER,
                    "Data buffer is null.");

    return SqlResult::AI_ERROR;
  }

  switch (attr) {
    case SQL_ATTR_CONNECTION_DEAD: {
      return SqlResult::AI_ERROR;
    }

    case SQL_ATTR_CONNECTION_TIMEOUT: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      *val = static_cast< SQLUINTEGER >(timeout_);

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;

      break;
    }

    case SQL_ATTR_LOGIN_TIMEOUT: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      *val = static_cast< SQLUINTEGER >(loginTimeout_);

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;

      break;
    }

    case SQL_ATTR_AUTOCOMMIT: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      *val = autoCommit_ ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF;

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;

      break;
    }

    default: {
      AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "Specified attribute is not supported.");

      return SqlResult::AI_ERROR;
    }
  }

  return SqlResult::AI_SUCCESS;
}

void Connection::SetAttribute(int attr, void* value, SQLINTEGER valueLen) {
  IGNITE_ODBC_API_CALL(InternalSetAttribute(attr, value, valueLen));
}

SqlResult::Type Connection::InternalSetAttribute(int attr, void* value,
                                                 SQLINTEGER) {
  switch (attr) {
    case SQL_ATTR_CONNECTION_DEAD: {
      AddStatusRecord(SqlState::SHY092_OPTION_TYPE_OUT_OF_RANGE,
                      "Attribute is read only.");

      return SqlResult::AI_ERROR;
    }

    case SQL_ATTR_CONNECTION_TIMEOUT: {
      timeout_ = RetrieveTimeout(value);

      if (GetDiagnosticRecords().GetStatusRecordsNumber() != 0)
        return SqlResult::AI_SUCCESS_WITH_INFO;

      break;
    }

    case SQL_ATTR_LOGIN_TIMEOUT: {
      loginTimeout_ = RetrieveTimeout(value);

      if (GetDiagnosticRecords().GetStatusRecordsNumber() != 0)
        return SqlResult::AI_SUCCESS_WITH_INFO;

      break;
    }

    case SQL_ATTR_AUTOCOMMIT: {
      SQLUINTEGER mode =
          static_cast< SQLUINTEGER >(reinterpret_cast< ptrdiff_t >(value));

      if (mode != SQL_AUTOCOMMIT_ON && mode != SQL_AUTOCOMMIT_OFF) {
        AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                        "Specified attribute is not supported.");

        return SqlResult::AI_ERROR;
      }

      autoCommit_ = mode == SQL_AUTOCOMMIT_ON;

      break;
    }

    default: {
      AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "Specified attribute is not supported.");

      return SqlResult::AI_ERROR;
    }
  }

  return SqlResult::AI_SUCCESS;
}

SqlResult::Type Connection::MakeRequestHandshake() {
  HandshakeRequest req(config_);
  HandshakeResponse rsp;

  try {
    // Workaround for some Linux systems that report connection on non-blocking
    // sockets as successful but fail to establish real connection.
    bool sent = InternalSyncMessage(req, rsp, loginTimeout_);

    if (!sent) {
      AddStatusRecord(
          SqlState::S08001_CANNOT_CONNECT,
          "Failed to get handshake response (Did you forget to enable SSL?).");

      return SqlResult::AI_ERROR;
    }
  } catch (const OdbcError& err) {
    AddStatusRecord(err);

    return SqlResult::AI_ERROR;
  } catch (const IgniteError& err) {
    AddStatusRecord(SqlState::S08004_CONNECTION_REJECTED, err.GetText());

    return SqlResult::AI_ERROR;
  }

  if (!rsp.IsAccepted()) {
    LOG_MSG("Handshake message has been rejected.");

    std::stringstream constructor;

    constructor << "Node rejected handshake message. ";

    if (!rsp.GetError().empty())
      constructor << "Additional info: " << rsp.GetError() << " ";

    AddStatusRecord(SqlState::S08004_CONNECTION_REJECTED, constructor.str());

    return SqlResult::AI_ERROR;
  }

  return SqlResult::AI_SUCCESS;
}

void Connection::EnsureConnected() {
}

bool Connection::TryRestoreConnection(IgniteError& err) {
  return false;
}

std::string Connection::FormatMongoCppConnectionString(
    int32_t localSSHTunnelPort) const {
  std::string host = "localhost";
  std::string port = std::to_string(localSSHTunnelPort);

  // localSSHTunnelPort == 0 means that internal SSH tunnel option was not set
  if (localSSHTunnelPort == 0) {
    host = config_.GetHostname();
    port = common::LexicalCast< std::string >(config_.GetPort());
  }
  std::string mongoConnectionString;

  mongoConnectionString = "mongodb:";
  mongoConnectionString.append("//" + config_.GetUser());
  mongoConnectionString.append(":" + config_.GetPassword());
  mongoConnectionString.append("@" + host);
  mongoConnectionString.append(":" + port);
  mongoConnectionString.append("/admin");
  mongoConnectionString.append("?authMechanism=SCRAM-SHA-1");
  if (config_.IsTls()) {
    mongoConnectionString.append("&tlsAllowInvalidHostnames=true");
  }

  // tls configuration is handled using tls_options in connectionCPP
  // TODO handle the other DSN configuration
  // https://bitquill.atlassian.net/browse/AD-599

  return mongoConnectionString;
}

int32_t Connection::RetrieveTimeout(void* value) {
  SQLUINTEGER uTimeout =
      static_cast< SQLUINTEGER >(reinterpret_cast< ptrdiff_t >(value));

  if (uTimeout > INT32_MAX) {
    std::stringstream ss;

    ss << "Value is too big: " << uTimeout << ", changing to " << timeout_
       << ".";

    AddStatusRecord(SqlState::S01S02_OPTION_VALUE_CHANGED, ss.str());

    return INT32_MAX;
  }

  return static_cast< int32_t >(uTimeout);
}

bool Connection::ConnectCPPDocumentDB(int32_t localSSHTunnelPort,
                                      odbc::IgniteError& err) {
  using bsoncxx::builder::basic::kvp;
  using bsoncxx::builder::basic::make_document;

  // Make sure that the DriverInstance is initialize
  DriverInstance::getInstance().initialize();
  try {
    std::string mongoCPPConnectionString =
        FormatMongoCppConnectionString(localSSHTunnelPort);
    mongocxx::options::client client_options;
    mongocxx::options::tls tls_options;
    if (config_.IsTls()) {
      // TO-DO Adapt to use certificates
      // https://bitquill.atlassian.net/browse/AD-598
      tls_options.allow_invalid_certificates(true);
      client_options.tls_opts(tls_options);
    }

    mongoClient_ = std::make_shared< mongocxx::client >(
        mongocxx::uri(mongoCPPConnectionString), client_options);
    std::string database = config_.GetDatabase();
    bsoncxx::builder::stream::document ping;
    ping << "ping" << 1;
    auto db = (*mongoClient_.get())[database];
    auto result = db.run_command(ping.view());

    if (result.view()["ok"].get_double() != 1) {
      err = odbc::IgniteError(odbc::IgniteError::IGNITE_ERR_NETWORK_FAILURE,
                              "Unable to ping DocumentDB.");
      return false;
    }

    return true;
  } catch (const mongocxx::exception& xcp) {
    std::stringstream message;
    message << "Unable to establish connection with DocumentDB."
            << " code: " << xcp.code().value()
            << " messagge: " << xcp.code().message()
            << " cause: " << xcp.what();
    err = odbc::IgniteError(
        odbc::IgniteError::IGNITE_ERR_SECURE_CONNECTION_FAILURE,
        message.str().c_str());
    return false;
  }
}
}  // namespace odbc
}  // namespace ignite
