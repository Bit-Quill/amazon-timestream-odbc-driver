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
#include "ignite/odbc/config/configuration.h"

#include <iterator>
#include <sstream>
#include <string>

#include "ignite/odbc/common/utils.h"
#include "ignite/odbc/config/config_tools.h"
#include "ignite/odbc/config/connection_string_parser.h"
#include "ignite/odbc/auth_type.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/utility.h"

using ignite::odbc::common::EncodeURIComponent;

namespace ignite {
namespace odbc {
namespace config {
// Connection (Basic Authentication) Settings
const std::string Configuration::DefaultValue::dsn = "Timestream DSN";
const std::string Configuration::DefaultValue::driver = "Amazon Timestream ODBC Driver";
const std::string Configuration::DefaultValue::accessKeyId = "";
const std::string Configuration::DefaultValue::secretKey = "";
const std::string Configuration::DefaultValue::accessKeyIdFromProfile = "";
const std::string Configuration::DefaultValue::secretKeyFromProfile = "";
bool Configuration::DefaultValue::profileIsParsed = false;
const std::string Configuration::DefaultValue::sessionToken = "";

// Credential Providers Options
const std::string Configuration::DefaultValue::profileName = "";
const std::string Configuration::DefaultValue::cusCredFile = "";

// Connection Options
const int32_t Configuration::DefaultValue::reqTimeout = 3000;
const int32_t Configuration::DefaultValue::connectionTimeout = 1000;
const int32_t Configuration::DefaultValue::maxRetryCount = 0;
const int32_t Configuration::DefaultValue::maxConnections = 25;

// Endpoint Options
const std::string Configuration::DefaultValue::endpoint = "";
const std::string Configuration::DefaultValue::region = "us-east-1";

// Advance Authentication Settings
const AuthType::Type Configuration::DefaultValue::authType = AuthType::Type::AWS_PROFILE;
const std::string Configuration::DefaultValue::idPHost = "";
const std::string Configuration::DefaultValue::idPUserName = "";
const std::string Configuration::DefaultValue::idPPassword = "";
const std::string Configuration::DefaultValue::idPArn = "";
const std::string Configuration::DefaultValue::oktaAppId = "";
const std::string Configuration::DefaultValue::roleArn = "";
const std::string Configuration::DefaultValue::aadAppId = "";
const std::string Configuration::DefaultValue::aadClientSecret = "";
const std::string Configuration::DefaultValue::aadTenant = "";

// Logging Configuration Options
const LogLevel::Type Configuration::DefaultValue::logLevel =
    LogLevel::Type::OFF;
const std::string Configuration::DefaultValue::logPath = DEFAULT_LOG_PATH;

std::string Configuration::ToConnectString() const {
  ArgumentMap arguments;

  ToMap(arguments);

  std::stringstream connect_string_buffer;

  for (ArgumentMap::const_iterator it = arguments.begin();
       it != arguments.end(); ++it) {
    const std::string& key = it->first;
    const std::string& value = it->second;

    if (value.empty())
      continue;

    // If there is space in the value, add brackets around it.
    if (value.find(' ') == std::string::npos)
      connect_string_buffer << key << '=' << value << ';';
    else
      connect_string_buffer << key << "={" << value << "};";
  }

  return connect_string_buffer.str();
}

const std::string& Configuration::GetDsn(const std::string& dflt) const {
  if (!dsn.IsSet())
    return dflt;

  return dsn.GetValue();
}

bool Configuration::IsDsnSet() const {
  return dsn.IsSet();
}

void Configuration::SetDsn(const std::string& dsnName) {
  this->dsn.SetValue(dsnName);
}

const std::string& Configuration::GetDriver() const {
  return driver.GetValue();
}

void Configuration::SetDriver(const std::string& driverName) {
  this->driver.SetValue(driverName);
}

const std::string& Configuration::GetProfileName() const {
  return profileName.GetValue();
}

void Configuration::SetProfileName(const std::string& name) {
  this->profileName.SetValue(name);
}

bool Configuration::IsProfileNameSet() const {
  return profileName.IsSet();
}

const std::string& Configuration::GetCusCredFile() const {
  return cusCredFile.GetValue();
}

void Configuration::SetCusCredFile(const std::string& path) {
  this->cusCredFile.SetValue(path);
}

bool Configuration::IsCusCredFileSet() const {
  return cusCredFile.IsSet();
}

int32_t Configuration::GetReqTimeout() const {
  return reqTimeout.GetValue();
}

void Configuration::SetReqTimeout(int32_t ms) {
  this->reqTimeout.SetValue(ms);
}

bool Configuration::IsReqTimeoutSet() const {
  return reqTimeout.IsSet();
}

int32_t Configuration::GetConnectionTimeout() const {
  return connectionTimeout.GetValue();
}

void Configuration::SetConnectionTimeout(int32_t ms) {
  this->connectionTimeout.SetValue(ms);
}

bool Configuration::IsConnectionTimeoutSet() const {
  return connectionTimeout.IsSet();
}

int32_t Configuration::GetMaxRetryCount() const {
  return maxRetryCount.GetValue();
}

void Configuration::SetMaxRetryCount(int32_t count) {
  this->maxRetryCount.SetValue(count);
}

bool Configuration::IsMaxRetryCountSet() const {
  return maxRetryCount.IsSet();
}

int32_t Configuration::GetMaxConnections() const {
  return maxConnections.GetValue();
}

void Configuration::SetMaxConnections(int32_t count) {
  this->maxConnections.SetValue(count);
}

bool Configuration::IsMaxConnectionsSet() const {
  return maxConnections.IsSet();
}

const std::string& Configuration::GetEndpoint() const {
  return endpoint.GetValue();
}

void Configuration::SetEndpoint(const std::string& value) {
  this->endpoint.SetValue(value);
}

bool Configuration::IsEndpointSet() const {
  return endpoint.IsSet();
}

const std::string& Configuration::GetRegion() const {
  return region.GetValue();
}

void Configuration::SetRegion(const std::string& value) {
  this->region.SetValue(value);
}

bool Configuration::IsRegionSet() const {
  return region.IsSet();
}

AuthType::Type Configuration::GetAuthType() const {
  return authType.GetValue();
}

void Configuration::SetAuthType(const AuthType::Type value) {
  this->authType.SetValue(value);
}

bool Configuration::IsAuthTypeSet() const {
  return authType.IsSet();
}

const std::string& Configuration::GetIdPHost() const {
  return idPHost.GetValue();
}

void Configuration::SetIdPHost(const std::string& value) {
  this->idPHost.SetValue(value);
}

bool Configuration::IsIdPHostSet() const {
  return idPHost.IsSet();
}

const std::string& Configuration::GetIdPUserName() const {
  return idPUserName.GetValue();
}

void Configuration::SetIdPUserName(const std::string& value) {
  this->idPUserName.SetValue(value);
}

bool Configuration::IsIdPUserNameSet() const {
  return idPUserName.IsSet();
}

const std::string& Configuration::GetIdPPassword() const {
  return idPPassword.GetValue();
}

void Configuration::SetIdPPassword(const std::string& value) {
  this->idPPassword.SetValue(value);
}

bool Configuration::IsIdPPasswordSet() const {
  return idPPassword.IsSet();
}

const std::string& Configuration::GetIdPArn() const {
  return idPArn.GetValue();
}

void Configuration::SetIdPArn(const std::string& value) {
  this->idPArn.SetValue(value);
}

bool Configuration::IsIdPArnSet() const {
  return idPArn.IsSet();
}

const std::string& Configuration::GetOktaAppId() const {
  return oktaAppId.GetValue();
}

void Configuration::SetOktaAppId(const std::string& value) {
  this->oktaAppId.SetValue(value);
}

bool Configuration::IsOktaAppIdSet() const {
  return oktaAppId.IsSet();
}

const std::string& Configuration::GetRoleArn() const {
  return roleArn.GetValue();
}

void Configuration::SetRoleArn(const std::string& value) {
  this->roleArn.SetValue(value);
}

bool Configuration::IsRoleArnSet() const {
  return roleArn.IsSet();
}

const std::string& Configuration::GetAadAppId() const {
  return aadAppId.GetValue();
}

void Configuration::SetAadAppId(const std::string& value) {
  this->aadAppId.SetValue(value);
}

bool Configuration::IsAadAppIdSet() const {
  return aadAppId.IsSet();
}

const std::string& Configuration::GetAadClientSecret() const {
  return aadClientSecret.GetValue();
}

void Configuration::SetAadClientSecret(const std::string& value) {
  this->aadClientSecret.SetValue(value);
}

bool Configuration::IsAadClientSecretSet() const {
  return aadClientSecret.IsSet();
}

const std::string& Configuration::GetAadTenant() const {
  return aadTenant.GetValue();
}

void Configuration::SetAadTenant(const std::string& value) {
  this->aadTenant.SetValue(value);
}

bool Configuration::IsAadTenantSet() const {
  return aadTenant.IsSet();
}

LogLevel::Type Configuration::GetLogLevel() const {
  return logLevel.GetValue();
}

void Configuration::SetLogLevel(const LogLevel::Type level) {
  if (level != LogLevel::Type::UNKNOWN) {
    this->logLevel.SetValue(level);
    Logger::GetLoggerInstance()->SetLogLevel(level);
  }
}

bool Configuration::IsLogLevelSet() const {
  return logLevel.IsSet();
}

const std::string& Configuration::GetLogPath() const {
  return logPath.GetValue();
}

void Configuration::SetLogPath(const std::string& path) {
  if (common::IsValidDirectory(path)) {
    this->logPath.SetValue(path);
    Logger::GetLoggerInstance()->SetLogPath(path);
  }
}

bool Configuration::IsLogPathSet() const {
  return logPath.IsSet();
}

const std::string& Configuration::GetAccessKeyId() const {
  return accessKeyId.GetValue();
}

void Configuration::SetAccessKeyId(const std::string& accessKeyIdValue) {
  this->accessKeyId.SetValue(accessKeyIdValue);
}

bool Configuration::IsAccessKeyIdSet() const {
  return accessKeyId.IsSet();
}

const std::string& Configuration::GetSecretKey() const {
  return secretKey.GetValue();
}

void Configuration::SetSecretKey(const std::string& secretKey) {
  this->secretKey.SetValue(secretKey);
}

bool Configuration::IsSecretKeySet() const {
  return secretKey.IsSet();
}

const std::string& Configuration::GetAccessKeyIdFromProfile() const {
  return accessKeyIdFromProfile.GetValue();
}

void Configuration::SetAccessKeyIdFromProfile(const std::string& accessKeyIdValue) {
  this->accessKeyIdFromProfile.SetValue(accessKeyIdValue);
}

bool Configuration::IsAccessKeyIdFromProfileSet() const {
  return accessKeyIdFromProfile.IsSet();
}

const std::string& Configuration::GetSecretKeyFromProfile() const {
  return secretKeyFromProfile.GetValue();
}

void Configuration::SetSecretKeyFromProfile(const std::string& secretKeyFromProfile) {
  this->secretKeyFromProfile.SetValue(secretKeyFromProfile);
}

bool Configuration::IsSecretKeyFromProfileSet() const {
  return secretKeyFromProfile.IsSet();
}

bool Configuration::GetProfileIsParsed() const {
  return profileIsParsed.GetValue();
}

void Configuration::SetProfileIsParsed(bool profileIsParsed) {
  this->profileIsParsed.SetValue(profileIsParsed);
}

const std::string& Configuration::GetSessionToken() const {
  return sessionToken.GetValue();
}

void Configuration::SetSessionToken(const std::string& token) {
  this->sessionToken.SetValue(token);
}

bool Configuration::IsSessionTokenSet() const {
  return sessionToken.IsSet();
}

void Configuration::ToMap(ArgumentMap& res) const {
  AddToMap(res, ConnectionStringParser::Key::dsn, dsn);
  AddToMap(res, ConnectionStringParser::Key::driver, driver);
  AddToMap(res, ConnectionStringParser::Key::accessKeyId, accessKeyId);
  AddToMap(res, ConnectionStringParser::Key::secretKey, secretKey);
  AddToMap(res, ConnectionStringParser::Key::sessionToken, sessionToken);
  AddToMap(res, ConnectionStringParser::Key::profileName, profileName);
  AddToMap(res, ConnectionStringParser::Key::cusCredFile, cusCredFile);
  AddToMap(res, ConnectionStringParser::Key::reqTimeout, reqTimeout);
  AddToMap(res, ConnectionStringParser::Key::connectionTimeout, connectionTimeout);
  AddToMap(res, ConnectionStringParser::Key::maxRetryCount, maxRetryCount);
  AddToMap(res, ConnectionStringParser::Key::maxConnections, maxConnections);
  AddToMap(res, ConnectionStringParser::Key::endpoint, endpoint);
  AddToMap(res, ConnectionStringParser::Key::region, region);
  AddToMap(res, ConnectionStringParser::Key::authType, authType);
  AddToMap(res, ConnectionStringParser::Key::idPHost, idPHost);
  AddToMap(res, ConnectionStringParser::Key::idPUserName, idPUserName);
  AddToMap(res, ConnectionStringParser::Key::idPPassword, idPPassword);
  AddToMap(res, ConnectionStringParser::Key::idPArn, idPArn);
  AddToMap(res, ConnectionStringParser::Key::oktaAppId, oktaAppId);
  AddToMap(res, ConnectionStringParser::Key::roleArn, roleArn);
  AddToMap(res, ConnectionStringParser::Key::aadAppId, aadAppId);
  AddToMap(res, ConnectionStringParser::Key::aadClientSecret, aadClientSecret);
  AddToMap(res, ConnectionStringParser::Key::aadTenant, aadTenant);
  AddToMap(res, ConnectionStringParser::Key::logLevel, logLevel);
  AddToMap(res, ConnectionStringParser::Key::logPath, logPath);
}

void Configuration::Validate() const {
  // Validate minimum required properties.

  // TODO support Okta authentication and remove this check
  // https://bitquill.atlassian.net/browse/AT-1055

  // TODO support AAD authentication and remove this check
  // https://bitquill.atlassian.net/browse/AT-1056
  if ((GetAuthType() == ignite::odbc::AuthType::Type::OKTA)
      || (GetAuthType() == ignite::odbc::AuthType::Type::AAD))
        throw OdbcError(SqlState::S01S00_INVALID_CONNECTION_STRING_ATTRIBUTE, "Unsupported AUTH value");

  if (((GetAuthType() == ignite::odbc::AuthType::Type::IAM) && (!IsAccessKeyIdSet() || !IsSecretKeySet())) || 
      ((GetAuthType()
             == ignite::odbc::AuthType::Type::AWS_PROFILE) && (!IsAccessKeyIdFromProfileSet() || !IsSecretKeyFromProfileSet()))) {
    throw OdbcError(
        SqlState::S01S00_INVALID_CONNECTION_STRING_ATTRIBUTE,
        "Any of the following group is required to connect:\n"
        "AUTH is \"IAM\" and "
        "ACCESS_KEY_ID and SECRET_KEY or\n"
        "AUTH is \"AWS_PROFILE\" and "
        "CUSTOM_CREDENTIALS_FILE which should contain access key Id and secret access key");
  }
}

template <>
void Configuration::AddToMap(ArgumentMap& map, const std::string& key,
                             const SettableValue< uint16_t >& value) {
  if (value.IsSet())
    map[key] = common::LexicalCast< std::string >(value.GetValue());
}

template <>
void Configuration::AddToMap(ArgumentMap& map, const std::string& key,
                             const SettableValue< int32_t >& value) {
  if (value.IsSet())
    map[key] = common::LexicalCast< std::string >(value.GetValue());
}

template <>
void Configuration::AddToMap(ArgumentMap& map, const std::string& key,
                             const SettableValue< std::string >& value) {
  if (value.IsSet())
    map[key] = value.GetValue();
}

template <>
void Configuration::AddToMap(ArgumentMap& map, const std::string& key,
                             const SettableValue< bool >& value) {
  if (value.IsSet())
    map[key] = value.GetValue() ? "true" : "false";
}

template <>
void Configuration::AddToMap(ArgumentMap& map, const std::string& key,
                             const SettableValue< AuthType::Type >& value) {
  if (value.IsSet())
    map[key] = AuthType::ToString(value.GetValue());
}

template <>
void Configuration::AddToMap(ArgumentMap& map, const std::string& key,
                             const SettableValue< LogLevel::Type >& value) {
  if (value.IsSet())
    map[key] = LogLevel::ToString(value.GetValue());
}
}  // namespace config
}  // namespace odbc
}  // namespace ignite
