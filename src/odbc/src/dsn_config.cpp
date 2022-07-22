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

#include "ignite/odbc/dsn_config.h"

#include <ignite/odbc/common/fixed_size_array.h>

#include "ignite/odbc/config/config_tools.h"
#include "ignite/odbc/config/connection_string_parser.h"
#include <ignite/odbc/cred_prov_class.h>
#include <ignite/odbc/idp_name.h>
#include <ignite/odbc/log_level.h>
#include "ignite/odbc/system/odbc_constants.h"
#include "ignite/odbc/utility.h"

using namespace ignite::odbc::config;

#define BUFFER_SIZE (1024 * 1024)
#define CONFIG_FILE u8"ODBC.INI"

namespace ignite {
namespace odbc {
void ThrowLastSetupError() {
  DWORD code;
  common::FixedSizeArray< SQLWCHAR > msg(BUFFER_SIZE);

  SQLInstallerError(1, &code, msg.GetData(), msg.GetSize(), NULL);

  std::stringstream buf;

  buf << "Message: \""
      << utility::SqlWcharToString(msg.GetData(), msg.GetSize())
      << "\", Code: " << code;

  throw IgniteError(IgniteError::IGNITE_ERR_GENERIC, buf.str().c_str());
}

void WriteDsnString(const char* dsn, const char* key, const char* value) {
  if (!SQLWritePrivateProfileString(utility::ToWCHARVector(dsn).data(),
                                    utility::ToWCHARVector(key).data(),
                                    utility::ToWCHARVector(value).data(),
                                    utility::ToWCHARVector(CONFIG_FILE).data()))
    ThrowLastSetupError();
}

SettableValue< std::string > ReadDsnString(const char* dsn,
                                           const std::string& key,
                                           const std::string& dflt = "") {
  static const char* unique = "35a920dd-8837-43d2-a846-e01a2e7b5f84";

  SettableValue< std::string > val(dflt);

  common::FixedSizeArray< SQLWCHAR > buf(BUFFER_SIZE);

  int ret = SQLGetPrivateProfileString(
      utility::ToWCHARVector(dsn).data(), utility::ToWCHARVector(key).data(),
      utility::ToWCHARVector(unique).data(), buf.GetData(), buf.GetSize(),
      utility::ToWCHARVector(CONFIG_FILE).data());

  if (ret > BUFFER_SIZE) {
    buf.Reset(ret + 1);

    ret = SQLGetPrivateProfileString(
        utility::ToWCHARVector(dsn).data(), utility::ToWCHARVector(key).data(),
        utility::ToWCHARVector(unique).data(), buf.GetData(), buf.GetSize(),
        utility::ToWCHARVector(CONFIG_FILE).data());
  }

  std::string res = utility::SqlWcharToString(buf.GetData());

  if (res != unique)
    val.SetValue(res);

  return val;
}

SettableValue< int32_t > ReadDsnInt(const char* dsn, const std::string& key,
                                    int32_t dflt = 0) {
  SettableValue< std::string > str = ReadDsnString(dsn, key, "");

  SettableValue< int32_t > res(dflt);

  if (str.IsSet())
    res.SetValue(common::LexicalCast< int, std::string >(str.GetValue()));

  return res;
}

SettableValue< bool > ReadDsnBool(const char* dsn, const std::string& key,
                                  bool dflt = false) {
  SettableValue< std::string > str = ReadDsnString(dsn, key, "");

  SettableValue< bool > res(dflt);

  if (str.IsSet())
    res.SetValue(str.GetValue() == "true");

  return res;
}

void ReadDsnConfiguration(const char* dsn, Configuration& config,
                          diagnostic::DiagnosticRecordStorage* diag) {
  SettableValue< std::string > accessKeyId =
      ReadDsnString(dsn, ConnectionStringParser::Key::accessKeyId);

  if (accessKeyId.IsSet() && !config.IsAccessKeyIdSet())
    config.SetAccessKeyId(accessKeyId.GetValue());

  SettableValue< std::string > secretKey =
      ReadDsnString(dsn, ConnectionStringParser::Key::secretKey);

  if (secretKey.IsSet() && !config.IsSecretKeySet())
    config.SetSecretKey(secretKey.GetValue());

  SettableValue< std::string > sessionToken =
      ReadDsnString(dsn, ConnectionStringParser::Key::sessionToken);

  if (sessionToken.IsSet() && !config.IsSessionTokenSet())
    config.SetSessionToken(sessionToken.GetValue());

  SettableValue< bool > enableMetadataPreparedStatement = ReadDsnBool(
      dsn, ConnectionStringParser::Key::enableMetadataPreparedStatement);

  if (enableMetadataPreparedStatement.IsSet()
      && !config.IsEnableMetadataPreparedStatementSet())
    config.SetEnableMetadataPreparedStatement(
        enableMetadataPreparedStatement.GetValue());

  SettableValue< std::string > credProvClass =
      ReadDsnString(dsn, ConnectionStringParser::Key::credProvClass);

  if (credProvClass.IsSet() && !config.IsCredProvClassSet()) {
    CredProvClass::Type className = CredProvClass::FromString(
        credProvClass.GetValue(), CredProvClass::Type::NONE);
    config.SetCredProvClass(className);
  }

  SettableValue< std::string > cusCredFile =
      ReadDsnString(dsn, ConnectionStringParser::Key::cusCredFile);

  if (cusCredFile.IsSet() && !config.IsCusCredFileSet())
    config.SetCusCredFile(cusCredFile.GetValue());

  SettableValue< int32_t > reqTimeout =
      ReadDsnInt(dsn, ConnectionStringParser::Key::reqTimeout);

  if (reqTimeout.IsSet() && !config.IsReqTimeoutSet())
    config.SetReqTimeout(reqTimeout.GetValue());

  SettableValue< int32_t > socketTimeout =
      ReadDsnInt(dsn, ConnectionStringParser::Key::socketTimeout);

  if (reqTimeout.IsSet() && !config.IsSocketTimeoutSet())
    config.SetSocketTimeout(socketTimeout.GetValue());

  SettableValue< int32_t > maxRetryCount =
      ReadDsnInt(dsn, ConnectionStringParser::Key::maxRetryCount);

  if (maxRetryCount.IsSet() && !config.IsMaxRetryCountSet())
    config.SetMaxRetryCount(maxRetryCount.GetValue());

  SettableValue< int32_t > maxConnections =
      ReadDsnInt(dsn, ConnectionStringParser::Key::maxConnections);

  if (maxConnections.IsSet() && !config.IsMaxConnectionsSet())
    config.SetMaxConnections(maxConnections.GetValue());

  SettableValue< std::string > endpoint =
      ReadDsnString(dsn, ConnectionStringParser::Key::endpoint);

  if (endpoint.IsSet() && !config.IsEndpointSet())
    config.SetEndpoint(endpoint.GetValue());

  SettableValue< std::string > region =
      ReadDsnString(dsn, ConnectionStringParser::Key::region);

  if (region.IsSet() && !config.IsRegionSet())
    config.SetRegion(region.GetValue());

  SettableValue< std::string > idpName =
      ReadDsnString(dsn, ConnectionStringParser::Key::idpName);

  if (idpName.IsSet() && !config.IsIdpNameSet()) {
    IdpName::Type idpn =
        IdpName::FromString(idpName.GetValue(), IdpName::Type::NONE);
    config.SetIdpName(idpn);
  }

  SettableValue< std::string > idpHost =
      ReadDsnString(dsn, ConnectionStringParser::Key::idpHost);

  if (idpHost.IsSet() && !config.IsIdpHostSet())
    config.SetIdpHost(idpHost.GetValue());

  SettableValue< std::string > idpUserName =
      ReadDsnString(dsn, ConnectionStringParser::Key::idpUserName);

  if (idpUserName.IsSet() && !config.IsIdpUserNameSet())
    config.SetIdpUserName(idpUserName.GetValue());

  SettableValue< std::string > idpPassword =
      ReadDsnString(dsn, ConnectionStringParser::Key::idpPassword);

  if (idpPassword.IsSet() && !config.IsIdpPasswordSet())
    config.SetIdpPassword(idpPassword.GetValue());

  SettableValue< std::string > idpArn =
      ReadDsnString(dsn, ConnectionStringParser::Key::idpArn);

  if (idpArn.IsSet() && !config.IsIdpArnSet())
    config.SetIdpArn(idpArn.GetValue());

  SettableValue< std::string > oktaAppId =
      ReadDsnString(dsn, ConnectionStringParser::Key::oktaAppId);

  if (oktaAppId.IsSet() && !config.IsOktaAppIdSet())
    config.SetOktaAppId(oktaAppId.GetValue());

  SettableValue< std::string > roleArn =
      ReadDsnString(dsn, ConnectionStringParser::Key::roleArn);

  if (roleArn.IsSet() && !config.IsRoleArnSet())
    config.SetRoleArn(roleArn.GetValue());

  SettableValue< std::string > aadAppId =
      ReadDsnString(dsn, ConnectionStringParser::Key::aadAppId);

  if (aadAppId.IsSet() && !config.IsAadAppIdSet())
    config.SetAadAppId(aadAppId.GetValue());

  SettableValue< std::string > aadClientSecret =
      ReadDsnString(dsn, ConnectionStringParser::Key::aadClientSecret);

  if (aadClientSecret.IsSet() && !config.IsAadClientSecretSet())
    config.SetAadClientSecret(aadClientSecret.GetValue());

  SettableValue< std::string > aadTenant =
      ReadDsnString(dsn, ConnectionStringParser::Key::aadTenant);

  if (aadTenant.IsSet() && !config.IsAadTenantSet())
    config.SetAadTenant(aadTenant.GetValue());

  SettableValue< std::string > logLevel =
      ReadDsnString(dsn, ConnectionStringParser::Key::logLevel);

  if (logLevel.IsSet() && !config.IsLogLevelSet()) {
    LogLevel::Type level =
        LogLevel::FromString(logLevel.GetValue(), LogLevel::Type::ERROR_LEVEL);
    config.SetLogLevel(level);
  }

  SettableValue< std::string > logPath =
      ReadDsnString(dsn, ConnectionStringParser::Key::logPath);

  if (logPath.IsSet() && !config.IsLogPathSet())
    config.SetLogPath(logPath.GetValue());
}
}  // namespace odbc
}  // namespace ignite
