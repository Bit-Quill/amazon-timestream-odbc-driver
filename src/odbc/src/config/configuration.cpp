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
#include "ignite/odbc/cred_prov_class.h"
#include "ignite/odbc/idp_name.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/utility.h"

using ignite::odbc::common::EncodeURIComponent;

namespace ignite {
namespace odbc {
namespace config {
// Connection (Basic Authentication) Settings
const std::string Configuration::DefaultValue::dsn = "Timestream DSN";
const std::string Configuration::DefaultValue::driver = "Amazon Timestream";
const std::string Configuration::DefaultValue::accessKeyId = "";
const std::string Configuration::DefaultValue::secretKey = "";
const std::string Configuration::DefaultValue::sessionToken = "";
const bool Configuration::DefaultValue::enableMetadataPreparedStatement = false;

// Credential Providers Options
const CredProvClass::Type Configuration::DefaultValue::credProvClass =
    CredProvClass::Type::NONE;
const std::string Configuration::DefaultValue::cusCredFile = "";

// Connection Options
const int32_t Configuration::DefaultValue::reqTimeout = 0;
const int32_t Configuration::DefaultValue::socketTimeout = 50000;
const int32_t Configuration::DefaultValue::maxRetryCount = 0;
const int32_t Configuration::DefaultValue::maxConnections = 50;

// Endpoint Options
const std::string Configuration::DefaultValue::endpoint = "";
const std::string Configuration::DefaultValue::region = "";

// Advance Authentication Settings
const IdpName::Type Configuration::DefaultValue::idpName = IdpName::Type::NONE;
const std::string Configuration::DefaultValue::idpHost = "";
const std::string Configuration::DefaultValue::idpUserName = "";
const std::string Configuration::DefaultValue::idpPassword = "";
const std::string Configuration::DefaultValue::idpArn = "";
const std::string Configuration::DefaultValue::oktaAppId = "";
const std::string Configuration::DefaultValue::roleArn = "";
const std::string Configuration::DefaultValue::aadAppId = "";
const std::string Configuration::DefaultValue::aadClientSecret = "";
const std::string Configuration::DefaultValue::aadTenant = "";

// Logging Configuration Options
const LogLevel::Type Configuration::DefaultValue::logLevel =
    LogLevel::Type::ERROR_LEVEL;
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

const CredProvClass::Type Configuration::GetCredProvClass() const {
  return credProvClass.GetValue();
}

void Configuration::SetCredProvClass(const CredProvClass::Type className) {
  this->credProvClass.SetValue(className);
}

bool Configuration::IsCredProvClassSet() const {
  return credProvClass.IsSet();
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

int32_t Configuration::GetSocketTimeout() const {
  return socketTimeout.GetValue();
}

void Configuration::SetSocketTimeout(int32_t ms) {
  this->socketTimeout.SetValue(ms);
}

bool Configuration::IsSocketTimeoutSet() const {
  return socketTimeout.IsSet();
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

IdpName::Type Configuration::GetIdpName() const {
  return idpName.GetValue();
}

void Configuration::SetIdpName(const IdpName::Type value) {
  this->idpName.SetValue(value);
}

bool Configuration::IsIdpNameSet() const {
  return idpName.IsSet();
}

const std::string& Configuration::GetIdpHost() const {
  return idpHost.GetValue();
}

void Configuration::SetIdpHost(const std::string& value) {
  this->idpHost.SetValue(value);
}

bool Configuration::IsIdpHostSet() const {
  return idpHost.IsSet();
}

const std::string& Configuration::GetIdpUserName() const {
  return idpUserName.GetValue();
}

void Configuration::SetIdpUserName(const std::string& value) {
  this->idpUserName.SetValue(value);
}

bool Configuration::IsIdpUserNameSet() const {
  return idpUserName.IsSet();
}

const std::string& Configuration::GetIdpPassword() const {
  return idpPassword.GetValue();
}

void Configuration::SetIdpPassword(const std::string& value) {
  this->idpPassword.SetValue(value);
}

bool Configuration::IsIdpPasswordSet() const {
  return idpPassword.IsSet();
}

const std::string& Configuration::GetIdpArn() const {
  return idpArn.GetValue();
}

void Configuration::SetIdpArn(const std::string& value) {
  this->idpArn.SetValue(value);
}

bool Configuration::IsIdpArnSet() const {
  return idpArn.IsSet();
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

void Configuration::SetSecretKey(const std::string& pass) {
  this->secretKey.SetValue(pass);
}

bool Configuration::IsSecretKeySet() const {
  return secretKey.IsSet();
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

bool Configuration::IsEnableMetadataPreparedStatement() const {
  return enableMetadataPreparedStatement.GetValue();
}

void Configuration::SetEnableMetadataPreparedStatement(bool val) {
  this->enableMetadataPreparedStatement.SetValue(val);
}

bool Configuration::IsEnableMetadataPreparedStatementSet() const {
  return enableMetadataPreparedStatement.IsSet();
}

void Configuration::ToMap(ArgumentMap& res) const {
  AddToMap(res, ConnectionStringParser::Key::dsn, dsn);
  AddToMap(res, ConnectionStringParser::Key::driver, driver);
  AddToMap(res, ConnectionStringParser::Key::accessKeyId, accessKeyId);
  AddToMap(res, ConnectionStringParser::Key::secretKey, secretKey);
  AddToMap(res, ConnectionStringParser::Key::sessionToken, sessionToken);
  AddToMap(res, ConnectionStringParser::Key::enableMetadataPreparedStatement, enableMetadataPreparedStatement);
  AddToMap(res, ConnectionStringParser::Key::credProvClass, credProvClass);
  AddToMap(res, ConnectionStringParser::Key::cusCredFile, cusCredFile);
  AddToMap(res, ConnectionStringParser::Key::reqTimeout, reqTimeout);
  AddToMap(res, ConnectionStringParser::Key::socketTimeout, socketTimeout);
  AddToMap(res, ConnectionStringParser::Key::maxRetryCount, maxRetryCount);
  AddToMap(res, ConnectionStringParser::Key::maxConnections, maxConnections);
  AddToMap(res, ConnectionStringParser::Key::endpoint, endpoint);
  AddToMap(res, ConnectionStringParser::Key::region, region);
  AddToMap(res, ConnectionStringParser::Key::idpName, idpName);
  AddToMap(res, ConnectionStringParser::Key::idpHost, idpHost);
  AddToMap(res, ConnectionStringParser::Key::idpUserName, idpUserName);
  AddToMap(res, ConnectionStringParser::Key::idpPassword, idpPassword);
  AddToMap(res, ConnectionStringParser::Key::idpArn, idpArn);
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
  if (!IsAccessKeyIdSet() || !IsSecretKeySet()) {
    throw OdbcError(
        SqlState::S01S00_INVALID_CONNECTION_STRING_ATTRIBUTE,
        "ACCESS_KEY_ID and SECRET_KEY are required to connect.");
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
                             const SettableValue< CredProvClass::Type >& value) {
  if (value.IsSet())
    map[key] = CredProvClass::ToString(value.GetValue());
}

template <>
void Configuration::AddToMap(ArgumentMap& map, const std::string& key,
                             const SettableValue< IdpName::Type >& value) {
  if (value.IsSet())
    map[key] = IdpName::ToString(value.GetValue());
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
