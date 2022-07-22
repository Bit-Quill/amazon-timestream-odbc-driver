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

#ifndef _IGNITE_ODBC_CONFIG_CONFIGURATION
#define _IGNITE_ODBC_CONFIG_CONFIGURATION

#include <stdint.h>

#include <map>
#include <string>

#include "ignite/odbc/config/settable_value.h"
#include "ignite/odbc/cred_prov_class.h"
#include "ignite/odbc/diagnostic/diagnosable.h"
#include "ignite/odbc/idp_name.h"
#include "ignite/odbc/odbc_error.h"
#include "ignite/odbc/log_level.h"

namespace ignite {
namespace odbc {
namespace config {
/**
 * ODBC configuration abstraction.
 */
class Configuration {
 public:
  /** Argument map type. */
  typedef std::map< std::string, std::string > ArgumentMap;

  /** Default values for configuration. */
  struct DefaultValue {
    /** Default value for DSN attribute. */
    static const std::string dsn;

    /** Default value for Driver attribute. */
    static const std::string driver;

    /** Default value for accessKeyId attribute. */
    static const std::string accessKeyId;

    /** Default value for secretKey attribute. */
    static const std::string secretKey;

    /** Default value for sessionToken attribute. */
    static const std::string sessionToken;

    /** Default value for enableMetadataPreparedStatement attribute. */
    static const bool enableMetadataPreparedStatement;

    /** Default value for credProvClass attribute. */
    static const CredProvClass::Type credProvClass;

    /** Default value for cusCredFile attribute. */
    static const std::string cusCredFile;

    /** Default value for reqTimeout attribute. */
    static const int32_t reqTimeout;

    /** Default value for socketTimeout attribute. */
    static const int32_t socketTimeout;

    /** Default value for maxRetryCount attribute. */
    static const int32_t maxRetryCount;

    /** Default value for maxConnections attribute. */
    static const int32_t maxConnections;

    /** Default value for endpoint attribute. */
    static const std::string endpoint;

    /** Default value for region attribute. */
    static const std::string region;

    /** Default value for idpName attribute. */
    static const IdpName::Type idpName;

    /** Default value for idpHost attribute. */
    static const std::string idpHost;

    /** Default value for idpUserName attribute. */
    static const std::string idpUserName;

    /** Default value for idpPassword attribute. */
    static const std::string idpPassword;

    /** Default value for idpArn attribute. */
    static const std::string idpArn;

    /** Default value for oktaAppId attribute. */
    static const std::string oktaAppId;

    /** Default value for roleArn attribute. */
    static const std::string roleArn;

    /** Default value for aadAppId attribute. */
    static const std::string aadAppId;

    /** Default value for aadClientSecret attribute. */
    static const std::string aadClientSecret;

    /** Default value for aadTenant attribute. */
    static const std::string aadTenant;

    /** Default value for logLevel attribute. */
    static const LogLevel::Type logLevel;

    /** Default value for logPath attribute. */
    static const std::string logPath;
  };

  /**
   * Default constructor.
   */
  Configuration() = default;

  /**
   * Destructor.
   */
  ~Configuration() = default;

  /**
   * Convert configure to connect string.
   *
   * @return Connect string.
   */
  std::string ToConnectString() const;

  /**
   * Get DSN.
   *
   * @return Data Source Name.
   */
  const std::string& GetDsn(const std::string& dflt = DefaultValue::dsn) const;

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsDsnSet() const;

  /**
   * Set DSN.
   *
   * @param dsn Data Source Name.
   */
  void SetDsn(const std::string& dsnName);

  /**
   * Get Driver.
   *
   * @return Driver name.
   */
  const std::string& GetDriver() const;

  /**
   * Set driver.
   *
   * @param driver Driver.
   */
  void SetDriver(const std::string& driverName);

  /**
   * Get accessKeyId.
   *
   * @return accessKeyId.
   */
  const std::string& GetAccessKeyId() const;

  /**
   * Set accessKeyId.
   *
   * @param accessKeyId accessKeyId.
   */
  void SetAccessKeyId(const std::string& accessKeyIdValue);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsAccessKeyIdSet() const;

  /**
   * Get secretKey.
   *
   * @return secretKey.
   */
  const std::string& GetSecretKey() const;

  /**
   * Set secretKey.
   *
   * @param pass secretKey.
   */
  void SetSecretKey(const std::string& pass);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsSecretKeySet() const;

  /**
   * Get sessionToken.
   *
   * @return sessionToken.
   */
  const std::string& GetSessionToken() const;

  /**
   * Set sessionToken.
   *
   * @param token sessionToken.
   */
  void SetSessionToken(const std::string& token);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsSessionTokenSet() const;

  /**
   * Get enable metadata prepared statement flag.
   *
   * @return @true if metadata prepared statement enabled.
   */
  bool IsEnableMetadataPreparedStatement() const;

  /**
   * Set enable metadata prepared statement.
   *
   * @param val Value to set.
   */
  void SetEnableMetadataPreparedStatement(bool val);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsEnableMetadataPreparedStatementSet() const;

  /**
   * Get AWS Credentials Provider Class name.
   *
   * @return className AWS Credentials Provider Class name.
   */
  const CredProvClass::Type GetCredProvClass() const;

  /**
   * Set AWS Credentials Provider Class name.
   *
   * @param className AWS Credentials Provider Class name.
   */
  void SetCredProvClass(const CredProvClass::Type className);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsCredProvClassSet() const;

  /**
   * Get path to Custom Credentials File file.
   *
   * @return path to Custom Credentials File file.
   */
  const std::string& GetCusCredFile() const;

  /**
   * Set path to Custom Credentials File file.
   *
   * @param path Path to Custom Credentials File file.
   */
  void SetCusCredFile(const std::string& path);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsCusCredFileSet() const;

  /**
   * Get request timeout in milliseconds.
   *
   * @return ms Request timeout.
   */
  int32_t GetReqTimeout() const;

  /**
   * Set request timeout in milliseconds.
   *
   * @param ms Request timeout.
   */
  void SetReqTimeout(int32_t ms);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsReqTimeoutSet() const;

  /**
   * Get socket timeout in milliseconds.
   *
   * @return ms Socket timeout.
   */
  int32_t GetSocketTimeout() const;

  /**
   * Set socket timeout in milliseconds.
   *
   * @param ms Socket timeout.
   */
  void SetSocketTimeout(int32_t ms);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsSocketTimeoutSet() const;

  /**
   * Get maximum # of retry attempts.
   *
   * @return count Maximum # of retry attempts.
   */
  int32_t GetMaxRetryCount() const;

  /**
   * Set maximum # of retry attempts.
   *
   * @param count Maximum # of retry attempts.
   */
  void SetMaxRetryCount(int32_t count);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsMaxRetryCountSet() const;

  /**
   * Get maximum # of connections.
   *
   * @return count Maximum # of connections.
   */
  int32_t GetMaxConnections() const;

  /**
   * Set maximum # of connections.
   *
   * @param count Maximum # of connections.
   */
  void SetMaxConnections(int32_t count);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsMaxConnectionsSet() const;

  /**
   * Get endpoint.
   *
   * @return value Endpoint.
   */
  const std::string& GetEndpoint() const;

  /**
   * Set endpoint.
   *
   * @param value Endpoint.
   */
  void SetEndpoint(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsEndpointSet() const;

  /**
   * Get region.
   *
   * @return value Region.
   */
  const std::string& GetRegion() const;

  /**
   * Set region.
   *
   * @param value region.
   */
  void SetRegion(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsRegionSet() const;

  /**
   * Get idpName.
   *
   * @return value IdpName.
   */
  IdpName::Type GetIdpName() const;

  /**
   * Set idpName.
   *
   * @param value IdpName.
   */
  void SetIdpName(const IdpName::Type value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdpNameSet() const;

  /**
   * Get idpHost.
   *
   * @return value IdpHost.
   */
  const std::string& GetIdpHost() const;

  /**
   * Set idpHost.
   *
   * @param value IdpHost.
   */
  void SetIdpHost(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdpHostSet() const;

  /**
   * Get idpUserName.
   *
   * @return value IdpUserName.
   */
  const std::string& GetIdpUserName() const;

  /**
   * Set idpUserName.
   *
   * @param value IdpUserName.
   */
  void SetIdpUserName(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdpUserNameSet() const;

  /**
   * Get idpPassword.
   *
   * @return value IdpPassword.
   */
  const std::string& GetIdpPassword() const;

  /**
   * Set idpPassword.
   *
   * @param value IdpPassword.
   */
  void SetIdpPassword(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdpPasswordSet() const;

  /**
   * Get idpArn.
   *
   * @return value IdpArn.
   */
  const std::string& GetIdpArn() const;

  /**
   * Set idpArn.
   *
   * @param value IdpArn.
   */
  void SetIdpArn(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdpArnSet() const;

  /**
   * Get oktaAppId.
   *
   * @return value OktaAppId.
   */
  const std::string& GetOktaAppId() const;

  /**
   * Set oktaAppId.
   *
   * @param value OktaAppId.
   */
  void SetOktaAppId(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsOktaAppIdSet() const;

  /**
   * Get roleArn.
   *
   * @return value RoleArn.
   */
  const std::string& GetRoleArn() const;

  /**
   * Set roleArn.
   *
   * @param value RoleArn.
   */
  void SetRoleArn(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsRoleArnSet() const;

  /**
   * Get aadAppId.
   *
   * @return value AadAppId.
   */
  const std::string& GetAadAppId() const;

  /**
   * Set aadAppId.
   *
   * @param value AadAppId.
   */
  void SetAadAppId(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsAadAppIdSet() const;

  /**
   * Get aadClientSecret.
   *
   * @return value AadClientSecret.
   */
  const std::string& GetAadClientSecret() const;

  /**
   * Set aadClientSecret.
   *
   * @param value AadClientSecret.
   */
  void SetAadClientSecret(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsAadClientSecretSet() const;

  /**
   * Get aadTenant.
   *
   * @return value AadTenant.
   */
  const std::string& GetAadTenant() const;

  /**
   * Set aadTenant.
   *
   * @param value AadTenant.
   */
  void SetAadTenant(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsAadTenantSet() const;

  /**
   * Get log level.
   *
   * @return Log level.
   */
  LogLevel::Type GetLogLevel() const;

  /**
   * Set log level.
   *
   * @param level Log level.
   */
  void SetLogLevel(const LogLevel::Type level);

  /**
   * Check if log level set.
   *
   * @return @true if Log level set.
   */
  bool IsLogLevelSet() const;

  /**
   * Get log path to save.
   *
   * @return Log path.
   */
  const std::string& GetLogPath() const;

  /**
   * Set log path to save.
   *
   * @param path Log path.
   */
  void SetLogPath(const std::string& path);

  /**
   * Check if log path set.
   *
   * @return @true if Log path set.
   */
  bool IsLogPathSet() const;

  /**
   * Get argument map.
   *
   * @param res Resulting argument map.
   */
  void ToMap(ArgumentMap& res) const;

  /**
   * Checks that a valid JDBC connection string with all the required properties
   * can be built from the configuration. Throws error on incomplete properties.
   */
  void Validate() const;

 private:
  /**
   * Add key and value to the argument map.
   *
   * @param map Map.
   * @param key Key.
   * @param value Value.
   */
  template < typename T >
  static void AddToMap(ArgumentMap& map, const std::string& key,
                       const SettableValue< T >& value);

  /**
   * Add key and value to the argument map.
   *
   * @param map Map.
   * @param key Key.
   * @param value Value.
   */
  template < typename T >
  static void AddToMap(ArgumentMap& map, const std::string& key,
                       const SettableValue< T >& value, bool isJdbcFormat);

  /** DSN. */
  SettableValue< std::string > dsn = DefaultValue::dsn;

  /** Driver name. */
  SettableValue< std::string > driver = DefaultValue::driver;

  /** Access Key Id. */
  SettableValue< std::string > accessKeyId = DefaultValue::accessKeyId;

  /** Secret Key. */
  SettableValue< std::string > secretKey = DefaultValue::secretKey;

  /** Session Token. */
  SettableValue< std::string > sessionToken = DefaultValue::sessionToken;

  /** Enable metadata prepared statement flag. */
  SettableValue< bool > enableMetadataPreparedStatement = DefaultValue::enableMetadataPreparedStatement;

  /** AWS Credentials Provider class name. */
  SettableValue< CredProvClass::Type > credProvClass =
      DefaultValue::credProvClass;

  /** Custom Credentials file path. */
  SettableValue< std::string > cusCredFile = DefaultValue::cusCredFile;

  /** Request timeout in milliseconds.  */
  SettableValue< int32_t > reqTimeout = DefaultValue::reqTimeout;

  /** Socket timeout in milliseconds.  */
  SettableValue< int32_t > socketTimeout = DefaultValue::socketTimeout;

  /** Max Retry Count.  */
  SettableValue< int32_t > maxRetryCount = DefaultValue::maxRetryCount;

  /** Max Connections.  */
  SettableValue< int32_t > maxConnections = DefaultValue::maxConnections;

  /** Endpoint. */
  SettableValue< std::string > endpoint = DefaultValue::endpoint;

  /** Region. */
  SettableValue< std::string > region = DefaultValue::region;

  /** Idp Name. */
  SettableValue< IdpName::Type > idpName = DefaultValue::idpName;

  /** Idp Host. */
  SettableValue< std::string > idpHost = DefaultValue::idpHost;

  /** Idp User Name. */
  SettableValue< std::string > idpUserName = DefaultValue::idpUserName;

  /** Idp Password. */
  SettableValue< std::string > idpPassword = DefaultValue::idpPassword;

  /** Idp ARN. */
  SettableValue< std::string > idpArn = DefaultValue::idpArn;

  /** Okta Application ID. */
  SettableValue< std::string > oktaAppId = DefaultValue::oktaAppId;

  /** Role ARN. */
  SettableValue< std::string > roleArn = DefaultValue::roleArn;

  /** Azure AD Application ID. */
  SettableValue< std::string > aadAppId = DefaultValue::aadAppId;

  /** Azure AD Client Secret. */
  SettableValue< std::string > aadClientSecret = DefaultValue::aadClientSecret;

  /** Azure AD Tenant. */
  SettableValue< std::string > aadTenant = DefaultValue::aadTenant;

  /** The log level for the log file. */
  SettableValue< LogLevel::Type > logLevel = DefaultValue::logLevel;

  /** The logging file path. */
  SettableValue< std::string > logPath = DefaultValue::logPath;
};

template <>
void Configuration::AddToMap< std::string >(
    ArgumentMap& map, const std::string& key,
    const SettableValue< std::string >& value);

template <>
void Configuration::AddToMap< uint16_t >(
    ArgumentMap& map, const std::string& key,
    const SettableValue< uint16_t >& value);

template <>
void Configuration::AddToMap< int32_t >(ArgumentMap& map,
                                        const std::string& key,
                                        const SettableValue< int32_t >& value);

template <>
void Configuration::AddToMap< bool >(ArgumentMap& map, const std::string& key,
                                     const SettableValue< bool >& value);

template <>
void Configuration::AddToMap< CredProvClass::Type >(
    ArgumentMap& map, const std::string& key,
    const SettableValue< CredProvClass::Type >& value);

template <>
void Configuration::AddToMap< IdpName::Type >(
    ArgumentMap& map, const std::string& key,
    const SettableValue< IdpName::Type >& value);

template <>
void Configuration::AddToMap< LogLevel::Type >(
    ArgumentMap& map, const std::string& key,
    const SettableValue< LogLevel::Type >& value);
}  // namespace config
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_CONFIG_CONFIGURATION
