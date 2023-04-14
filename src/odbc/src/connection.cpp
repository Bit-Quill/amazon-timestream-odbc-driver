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
 *
 * Modifications Copyright Amazon.com, Inc. or its affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "timestream/odbc/connection.h"

#include <timestream/odbc/ignite_error.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <sstream>

#include "timestream/odbc/utils.h"
#include "timestream/odbc/config/configuration.h"
#include "timestream/odbc/config/connection_string_parser.h"
#include "timestream/odbc/dsn_config.h"
#include "timestream/odbc/environment.h"
#include "timestream/odbc/log.h"
#include "timestream/odbc/statement.h"
#include "timestream/odbc/system/system_dsn.h"
#include "timestream/odbc/utility.h"

#include "timestream/odbc/authentication/aad.h"
#include "timestream/odbc/authentication/okta.h"

#include <aws/timestream-query/model/QueryRequest.h>
#include <aws/timestream-query/model/QueryResult.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/DefaultRetryStrategy.h>

using namespace timestream::odbc;
using namespace ignite::odbc::common;
using timestream::odbc::IgniteError;

namespace {
#pragma pack(push, 1)
struct OdbcProtocolHeader {
  int32_t len;
};
#pragma pack(pop)
}  // namespace

namespace timestream {
namespace odbc {

std::mutex Connection::mutex_;
bool Connection::awsSDKReady_ = false;
std::atomic< int > Connection::refCount_(0);

Connection::Connection(Environment* env)
    : env_(env), info_(config_), metadataID_(false) {
  LOG_DEBUG_MSG("Connection is called");
  // The AWS SDK for C++ must be initialized by calling Aws::InitAPI.
  // It should only be initialized only once during the application running
  // All Connections in different thread must wait before the InitAPI is
  // finished.
  if (!awsSDKReady_) {
    // Use awsSDKReady_ and mutex_ to guarantee InitAPI is executed before all
    // Connection objects start to run.
    std::lock_guard< std::mutex > lock(mutex_);
    if (!awsSDKReady_) {
      Aws::Utils::Logging::LogLevel awsLogLvl = GetAWSLogLevelFromString(
          ignite::odbc::common::GetEnv("TS_AWS_LOG_LEVEL"));
      options_.loggingOptions.logLevel = awsLogLvl;

      // Aws::Utils::Logging::GetLogLevelName aborts the program with parameter `LogLevel::Off`
      // https://github.com/aws/aws-sdk-cpp/issues/2436 is created to track the AWS SDK issue.
      if (awsLogLvl != Aws::Utils::Logging::LogLevel::Off) {
        LOG_INFO_MSG("AWS SDK log level is set to: "
                     << Aws::Utils::Logging::GetLogLevelName(awsLogLvl));
      } else {
        LOG_INFO_MSG("AWS SDK log level is set to: Off");
      }

      Aws::InitAPI(options_);
      awsSDKReady_ = true;
      LOG_DEBUG_MSG("AWS SDK is Initialized");
    }
  }

  // record the Connection object in an atomic counter
  ++refCount_;
}

Connection::~Connection() {
  Close();

  // Before the application terminates, the SDK must be shut down.
  // It should be shutdown only once by the last Connection
  // destructor during the application running. The atomic counter
  // guarantees this.
  if (0 == --refCount_) {
    Aws::ShutdownAPI(options_);
    awsSDKReady_ = false;
    LOG_DEBUG_MSG("AWS SDK is shut down");
  }
}

const config::ConnectionInfo& Connection::GetInfo() const {
  return info_;
}

void Connection::GetInfo(config::ConnectionInfo::InfoType type, void* buf,
                         short buflen, short* reslen) {
  LOG_INFO_MSG("SQLGetInfo called: "
               << type << " (" << config::ConnectionInfo::InfoTypeToString(type)
               << "), " << std::hex << reinterpret_cast< size_t >(buf) << ", "
               << buflen << ", " << std::hex
               << reinterpret_cast< size_t >(reslen) << std::dec);

  IGNITE_ODBC_API_CALL(InternalGetInfo(type, buf, buflen, reslen));
}

SqlResult::Type Connection::InternalGetInfo(
    config::ConnectionInfo::InfoType type, void* buf, short buflen,
    short* reslen) {
  const config::ConnectionInfo& info = GetInfo();

  SqlResult::Type res = info.GetInfo(type, buf, buflen, reslen);

  if (res != SqlResult::AI_SUCCESS) {
    std::stringstream ss;
    ss << "SQLGetInfo input " << type << " is not implemented.";

    std::string s = ss.str();
    AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED, ss.str(),
                    timestream::odbc::LogLevel::Type::INFO_LEVEL);
  }

  return res;
}

void Connection::Establish(const std::string& connectStr, void* parentWindow) {
  IGNITE_ODBC_API_CALL(InternalEstablish(connectStr, parentWindow));
}

SqlResult::Type Connection::InternalEstablish(const std::string& connectStr,
                                              void* parentWindow) {
  LOG_DEBUG_MSG("InternalEstablish is called");
  config::ConnectionStringParser parser(config_);
  parser.ParseConnectionString(connectStr, &GetDiagnosticRecords());

  if (config_.IsDsnSet()) {
    std::string dsn = config_.GetDsn();
    LOG_DEBUG_MSG("dsn is " << dsn);

    ReadDsnConfiguration(dsn.c_str(), config_, &GetDiagnosticRecords());
  }

#ifdef _WIN32
  if (parentWindow) {
    LOG_DEBUG_MSG("Parent window is passed. Creating configuration window.");
    if (!DisplayConnectionWindow(parentWindow, config_)) {
      AddStatusRecord(odbc::SqlState::SHY008_OPERATION_CANCELED,
                      "Connection canceled by user",
                      timestream::odbc::LogLevel::Type::INFO_LEVEL);

      return SqlResult::AI_ERROR;
    }
  }
#endif  // _WIN32

  return InternalEstablish(config_);
}

void Connection::Establish(const config::Configuration& cfg) {
  IGNITE_ODBC_API_CALL(InternalEstablish(cfg));
}

SqlResult::Type Connection::InternalEstablish(
    const config::Configuration& cfg) {
  LOG_DEBUG_MSG("InternalEstablish is called");
  config_ = cfg;

  if (queryClient_) {
    AddStatusRecord(SqlState::S08002_ALREADY_CONNECTED, "Already connected.",
                    timestream::odbc::LogLevel::Type::INFO_LEVEL);

    return SqlResult::AI_ERROR;
  }

  try {
    config_.Validate();
  } catch (const ignite::odbc::OdbcError& err) {
    AddStatusRecord(err);
    return SqlResult::AI_ERROR;
  }

  IgniteError err;
  bool connected = TryRestoreConnection(cfg, err);

  if (!connected) {
    std::string errMessage = "Failed to establish connection to Timestream.\n";
    errMessage.append(err.GetText());
    AddStatusRecord(SqlState::S08001_CANNOT_CONNECT, errMessage);

    return SqlResult::AI_ERROR;
  }

  bool errors = GetDiagnosticRecords().GetStatusRecordsNumber() > 0;

  LOG_DEBUG_MSG("errors is " << errors);

  return errors ? SqlResult::AI_SUCCESS_WITH_INFO : SqlResult::AI_SUCCESS;
}

void Connection::Release() {
  IGNITE_ODBC_API_CALL(InternalRelease());
}

void Connection::Deregister() {
  env_->DeregisterConnection(this);
}

std::shared_ptr< Aws::TimestreamQuery::TimestreamQueryClient >
Connection::GetQueryClient() const {
  return queryClient_;
}

SqlResult::Type Connection::InternalRelease() {
  LOG_DEBUG_MSG("InternalRelease is called");
  if (!queryClient_) {
    AddStatusRecord(SqlState::S08003_NOT_CONNECTED, "Connection is not open.",
                    timestream::odbc::LogLevel::Type::WARNING_LEVEL);

    Close();

    // It is important to return SUCCESS_WITH_INFO and not ERROR here, as if we
    // return an error, Windows Driver Manager may decide that connection is not
    // valid anymore which results in memory leak.
    return SqlResult::AI_SUCCESS_WITH_INFO;
  }

  Close();

  return SqlResult::AI_SUCCESS;
}

void Connection::Close() {
  if (queryClient_) {
    queryClient_.reset();
  }

  if (samlCredProvider_) {
    samlCredProvider_.reset();
  }
}

Statement* Connection::CreateStatement() {
  Statement* statement;

  IGNITE_ODBC_API_CALL(InternalCreateStatement(statement));

  return statement;
}

SqlResult::Type Connection::InternalCreateStatement(Statement*& statement) {
  LOG_DEBUG_MSG("InternalCreateStatement is called");
  statement = new Statement(*this);

  if (!statement) {
    AddStatusRecord(SqlState::SHY001_MEMORY_ALLOCATION, "Not enough memory.");

    return SqlResult::AI_ERROR;
  }

  return SqlResult::AI_SUCCESS;
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

int32_t Connection::GetEnvODBCVer() {
  SQLINTEGER version;
  SqlLen outResLen;

  app::ApplicationDataBuffer outBuffer(
      type_traits::OdbcNativeType::AI_SIGNED_LONG, &version, 0, &outResLen);
  env_->GetAttribute(SQL_ATTR_ODBC_VERSION, outBuffer);

  return outBuffer.GetInt32();
}

void Connection::GetAttribute(int attr, void* buf, SQLINTEGER bufLen,
                              SQLINTEGER* valueLen) {
  IGNITE_ODBC_API_CALL(InternalGetAttribute(attr, buf, bufLen, valueLen));
}

SqlResult::Type Connection::InternalGetAttribute(int attr, void* buf,
                                                 SQLINTEGER,
                                                 SQLINTEGER* valueLen) {
  LOG_DEBUG_MSG("InternalGetAttribute is called, attr is " << attr);
  if (!buf) {
    AddStatusRecord(SqlState::SHY009_INVALID_USE_OF_NULL_POINTER,
                    "Data buffer is null.");

    return SqlResult::AI_ERROR;
  }

  switch (attr) {
    case SQL_ATTR_CONNECTION_DEAD: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      *val = queryClient_ ? SQL_CD_FALSE : SQL_CD_TRUE;

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;
    }

    case SQL_ATTR_CONNECTION_TIMEOUT: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      // connection timeout is disabled. Same behavior as version 1.0
      *val = 0;

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

    case SQL_ATTR_METADATA_ID: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      *val = metadataID_ ? SQL_TRUE : SQL_FALSE;

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;

      break;
    }

    case SQL_ATTR_AUTO_IPD: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      // could only be false as we do not support SQLPrepare.
      *val = false;

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;

      break;
    }

    case SQL_ATTR_ASYNC_ENABLE: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      // Asynchronous execution is not supported
      *val = SQL_ASYNC_ENABLE_OFF;

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;

      break;
    }

    case SQL_ATTR_TSLOG_DEBUG: {
      SQLUINTEGER* val = reinterpret_cast< SQLUINTEGER* >(buf);

      std::shared_ptr< Logger > logger = Logger::GetLoggerInstance();
      *val = static_cast< SQLUINTEGER >(logger->GetLogLevel());

      if (valueLen)
        *valueLen = SQL_IS_INTEGER;

      break;
    }

    default: {
      AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "Specified attribute is not supported.",
                      timestream::odbc::LogLevel::Type::INFO_LEVEL);

      return SqlResult::AI_ERROR;
    }
  }

  LOG_DEBUG_MSG("buf: " << *reinterpret_cast< SQLUINTEGER* >(buf));

  return SqlResult::AI_SUCCESS;
}

void Connection::SetAttribute(int attr, void* value, SQLINTEGER valueLen) {
  IGNITE_ODBC_API_CALL(InternalSetAttribute(attr, value, valueLen));
}

SqlResult::Type Connection::InternalSetAttribute(int attr, void* value,
                                                 SQLINTEGER) {
  LOG_DEBUG_MSG("InternalSetAttribute is called, attr is " << attr);
  switch (attr) {
    case SQL_ATTR_CONNECTION_DEAD: {
      AddStatusRecord(SqlState::SHY092_OPTION_TYPE_OUT_OF_RANGE,
                      "Attribute is read only.");

      return SqlResult::AI_ERROR;
    }

    case SQL_ATTR_AUTOCOMMIT: {
      SQLUINTEGER mode =
          static_cast< SQLUINTEGER >(reinterpret_cast< ptrdiff_t >(value));

      if (mode != SQL_AUTOCOMMIT_ON && mode != SQL_AUTOCOMMIT_OFF) {
        AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                        "Specified attribute is not supported.",
                        timestream::odbc::LogLevel::Type::INFO_LEVEL);

        return SqlResult::AI_ERROR;
      }

      autoCommit_ = mode == SQL_AUTOCOMMIT_ON;

      break;
    }

    case SQL_ATTR_METADATA_ID: {
      SQLUINTEGER id =
          static_cast< SQLUINTEGER >(reinterpret_cast< ptrdiff_t >(value));

      if (id != SQL_TRUE && id != SQL_FALSE) {
        AddStatusRecord(SqlState::SHY024_INVALID_ATTRIBUTE_VALUE,
                        "Invalid argument value");

        return SqlResult::AI_ERROR;
      }
      LOG_INFO_MSG("SQL_ATTR_METADATA_ID is set to " << id);
      metadataID_ = id;

      break;
    }

    case SQL_ATTR_ANSI_APP: {
      // According to Microsoft, if a driver exhibits the same behavior for both
      // ANSI and Unicode applications, it should return SQL_ERROR for this
      // attribute. Our driver has same behavior for ANSI and Unicode
      // applications.
      AddStatusRecord(SqlState::SHY000_GENERAL_ERROR,
                      "Same behavior for ANSI and Unicode applications.");

      return SqlResult::AI_ERROR;
    }

    case SQL_ATTR_TSLOG_DEBUG: {
      LogLevel::Type type =
          static_cast< LogLevel::Type >(reinterpret_cast< ptrdiff_t >(value));

      std::shared_ptr< Logger > logger = Logger::GetLoggerInstance();
      logger->SetLogLevel(type);
      LOG_INFO_MSG("log level is set to " << static_cast< int >(type));
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

/**
 * Updates connection runtime information used by SQLGetInfo.
 *
 */
void UpdateConnectionRuntimeInfo(const config::Configuration& config,
                                 config::ConnectionInfo& info) {
#ifdef SQL_USER_NAME
  info.SetInfo(SQL_USER_NAME, config.GetDSNUserName());
#endif
#ifdef SQL_DATA_SOURCE_NAME
  info.SetInfo(SQL_DATA_SOURCE_NAME, config.GetDsn());
#endif
}

std::shared_ptr< Aws::Http::HttpClient > Connection::GetHttpClient() {
  return Aws::Http::CreateHttpClient(Aws::Client::ClientConfiguration());
}

Aws::Utils::Logging::LogLevel Connection::GetAWSLogLevelFromString(
    std::string awsLogLvl) {
  std::transform(awsLogLvl.begin(), awsLogLvl.end(), awsLogLvl.begin(),
                 ::toupper);

  if (awsLogLvl == "OFF")
    return Aws::Utils::Logging::LogLevel::Off;

  if (awsLogLvl == "FATAL")
    return Aws::Utils::Logging::LogLevel::Fatal;

  if (awsLogLvl == "ERROR")
    return Aws::Utils::Logging::LogLevel::Error;

  if (awsLogLvl == "WARN")
    return Aws::Utils::Logging::LogLevel::Warn;

  if (awsLogLvl == "INFO")
    return Aws::Utils::Logging::LogLevel::Info;

  if (awsLogLvl == "DEBUG")
    return Aws::Utils::Logging::LogLevel::Debug;

  if (awsLogLvl == "TRACE")
    return Aws::Utils::Logging::LogLevel::Trace;

  // Return default
  return Aws::Utils::Logging::LogLevel::Warn;
}

// TODO: [AT-1256] replace these environment variables with data from
// config::Configuration https://bitquill.atlassian.net/browse/AT-1256
void Connection::SetClientProxy(Aws::Client::ClientConfiguration& clientCfg) {
  LOG_DEBUG_MSG("SetClientProxy is called");
  // proxy host
  std::string proxyHost = utility::Trim(GetEnv("TS_PROXY_HOST"));
  if (!proxyHost.empty()) {
    LOG_DEBUG_MSG("proxy host is " << proxyHost);
    clientCfg.proxyHost = proxyHost;
  }

  // proxy port
  int proxyPort = 0;
  std::string portStr = utility::Trim(GetEnv("TS_PROXY_PORT"));
  if (!portStr.empty()) {
    LOG_DEBUG_MSG("proxy port is " << portStr);
    proxyPort = utility::StringToInt(portStr);
  }
  if (proxyPort > 0) {
    clientCfg.proxyPort = proxyPort;
  }

  // proxy scheme
  std::string proxyScheme = utility::Trim(GetEnv("TS_PROXY_SCHEME"));
  if (!proxyScheme.empty()) {
    LOG_DEBUG_MSG("proxy scheme is " << proxyScheme);
    std::transform(proxyScheme.begin(), proxyScheme.end(), proxyScheme.begin(),
                   ::toupper);
    if (proxyScheme == "HTTPS") {
      clientCfg.proxyScheme = Aws::Http::Scheme::HTTPS;
    } else {
      clientCfg.proxyScheme = Aws::Http::Scheme::HTTP;
    }
  }

  // proxy user name
  std::string proxyUser = utility::Trim(GetEnv("TS_PROXY_USER"));
  if (!proxyUser.empty()) {
    LOG_DEBUG_MSG("proxy username is set");
    clientCfg.proxyUserName = proxyUser;
  }

  // proxy user password
  std::string proxyPassword = utility::Trim(GetEnv("TS_PROXY_PASSWORD"));
  if (!proxyPassword.empty()) {
    LOG_DEBUG_MSG("proxy user password is set");
    clientCfg.proxyPassword = proxyPassword;
  }

  // proxy SSL certificate path
  std::string proxySSLCertPath =
      utility::Trim(GetEnv("TS_PROXY_SSL_CERT_PATH"));
  if (!proxySSLCertPath.empty()) {
    LOG_DEBUG_MSG("proxy SSL certificate path is " << proxySSLCertPath);
    clientCfg.proxySSLCertPath = proxySSLCertPath;
  }

  // proxy SSL certificate type
  std::string proxySSLCertType =
      utility::Trim(GetEnv("TS_PROXY_SSL_CERT_TYPE"));
  if (!proxySSLCertType.empty()) {
    LOG_DEBUG_MSG("proxy SSL certificate type is " << proxySSLCertType);
    clientCfg.proxySSLCertType = proxySSLCertType;
  }

  // proxy SSL key path
  std::string proxySSLKeyPath = utility::Trim(GetEnv("TS_PROXY_SSL_KEY_PATH"));
  if (!proxySSLKeyPath.empty()) {
    LOG_DEBUG_MSG("proxy SSL key path is " << proxySSLKeyPath);
    clientCfg.proxySSLKeyPath = proxySSLKeyPath;
  }

  // proxy SSL key type
  std::string proxySSLKeyType = utility::Trim(GetEnv("TS_PROXY_SSL_KEY_TYPE"));
  if (!proxySSLKeyType.empty()) {
    LOG_DEBUG_MSG("proxy SSL key type is " << proxySSLKeyType);
    clientCfg.proxySSLCertType = proxySSLKeyType;
  }

  // proxy SSL key password
  std::string proxySSLKeyPassword =
      utility::Trim(GetEnv("TS_PROXY_SSL_KEY_PASSWORD"));
  if (!proxySSLKeyPassword.empty()) {
    LOG_DEBUG_MSG("proxy SSL key password is set");
    clientCfg.proxySSLKeyPassword = proxySSLKeyPassword;
  }
}

std::shared_ptr< Aws::STS::STSClient > Connection::GetStsClient() {
  return std::make_shared< Aws::STS::STSClient >();
}

bool Connection::TryRestoreConnection(const config::Configuration& cfg,
                                      IgniteError& err) {
  LOG_DEBUG_MSG("TryRestoreConnection is called");
  Aws::Auth::AWSCredentials credentials;
  std::string errInfo("");

  AuthType::Type authType = cfg.GetAuthType();
  LOG_DEBUG_MSG("auth type is " << static_cast< int >(authType));
  if (authType == AuthType::Type::OKTA) {
    std::shared_ptr< Aws::Http::HttpClient > httpClient = GetHttpClient();
    std::shared_ptr< Aws::STS::STSClient > stsClient = GetStsClient();
    samlCredProvider_ =
        std::make_shared< timestream::odbc::TimestreamOktaCredentialsProvider >(
            cfg, httpClient, stsClient);
    samlCredProvider_->GetAWSCredentials(credentials, errInfo);
  } else if (authType == AuthType::Type::AAD) {
    std::shared_ptr< Aws::Http::HttpClient > httpClient = GetHttpClient();
    std::shared_ptr< Aws::STS::STSClient > stsClient = GetStsClient();
    samlCredProvider_ =
        std::make_shared< timestream::odbc::TimestreamAADCredentialsProvider >(
            cfg, httpClient, stsClient);
    samlCredProvider_->GetAWSCredentials(credentials, errInfo);
  } else if (authType == AuthType::Type::AWS_PROFILE) {
    Aws::Auth::ProfileConfigFileAWSCredentialsProvider credProvider(
        cfg.GetProfileName().data());
    credentials = credProvider.GetAWSCredentials();
    LOG_DEBUG_MSG("profile name is " << cfg.GetProfileName());
  } else if (authType == AuthType::Type::IAM) {
    credentials.SetAWSAccessKeyId(cfg.GetDSNUserName());
    credentials.SetAWSSecretKey(cfg.GetDSNPassword());
    credentials.SetSessionToken(cfg.GetSessionToken());
  } else {
    std::string errMsg =
        "AuthType is not AWS_PROFILE, AAD, IAM or OKTA, but "
        "TryRestoreConnection is "
        "called.";
    LOG_ERROR_MSG(errMsg);
    err = IgniteError(IgniteError::IGNITE_ERR_TS_CONNECT, errMsg.data());

    Close();
    return false;
  }

  if (credentials.IsExpiredOrEmpty()) {
    if (errInfo.empty())
      errInfo += "Empty or expired credentials";

    LOG_ERROR_MSG(errInfo);
    err = IgniteError(IgniteError::IGNITE_ERR_TS_CONNECT, errInfo.data());

    Close();
    return false;
  }

  Aws::Client::ClientConfiguration clientCfg;
  clientCfg.region = cfg.GetRegion();
  clientCfg.enableEndpointDiscovery = true;
  clientCfg.connectTimeoutMs = cfg.GetConnectionTimeout();
  clientCfg.requestTimeoutMs = cfg.GetReqTimeout();
  clientCfg.maxConnections = cfg.GetMaxConnections();
  LOG_DEBUG_MSG("region is "
                << cfg.GetRegion() << ", connection timeout is "
                << clientCfg.connectTimeoutMs << ", request timeout is "
                << clientCfg.requestTimeoutMs << ", max connection is "
                << clientCfg.maxConnections);

  SetClientProxy(clientCfg);

  if (cfg.GetMaxRetryCountClient() > 0) {
    clientCfg.retryStrategy =
        std::make_shared< Aws::Client::DefaultRetryStrategy >(
            cfg.GetMaxRetryCountClient());
    LOG_DEBUG_MSG("max retry count is " << cfg.GetMaxRetryCountClient());
  }

  queryClient_ = CreateTSQueryClient(credentials, clientCfg);

  const std::string& endpoint = cfg.GetEndpoint();
  // endpoint could not be set to empty string
  if (!endpoint.empty()) {
    queryClient_->OverrideEndpoint(endpoint);
    LOG_DEBUG_MSG("endpoint is set to " << endpoint);
  }
  // try a simple query with query client
  Aws::TimestreamQuery::Model::QueryRequest queryRequest;
  queryRequest.SetQueryString("SELECT 1");

  Aws::TimestreamQuery::Model::QueryOutcome outcome =
      queryClient_->Query(queryRequest);
  if (!outcome.IsSuccess()) {
    auto error = outcome.GetError();
    LOG_DEBUG_MSG("ERROR: " << error.GetExceptionName() << ": "
                            << error.GetMessage());

    err = IgniteError(IgniteError::IGNITE_ERR_TS_CONNECT,
                      std::string(error.GetExceptionName())
                          .append(": ")
                          .append(error.GetMessage())
                          .c_str());

    Close();
    return false;
  }

  UpdateConnectionRuntimeInfo(config_, info_);

  return true;
}

std::shared_ptr< Aws::TimestreamQuery::TimestreamQueryClient >
Connection::CreateTSQueryClient(
    const Aws::Auth::AWSCredentials& credentials,
    const Aws::Client::ClientConfiguration& clientCfg) {
  return std::make_shared< Aws::TimestreamQuery::TimestreamQueryClient >(
      credentials, clientCfg);
}
}  // namespace odbc
}  // namespace timestream
