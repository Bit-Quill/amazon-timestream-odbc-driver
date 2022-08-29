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
#include "ignite/odbc/diagnostic/diagnosable.h"
#include "ignite/odbc/auth_type.h"
#include "ignite/odbc/odbc_error.h"
#include "ignite/odbc/log_level.h"

#define DEFAULT_DSN "Timestream DSN"
#define DEFAULT_DRIVER "Amazon Timestream ODBC Driver"
#define DEFAULT_ACCESS_KEY_ID ""
#define DEFAULT_SECRET_KEY ""
#define DEFAULT_ACCESS_KEY_ID_FROM_PROFILE ""
#define DEFAULT_SECRET_KEY_FROM_PROFILE ""
#define DEFAULT_PROFILE_IS_PARSED false
#define DEFAULT_SESSION_TOKEN ""

#define DEFAULT_PROFILE_NAME ""
#define DEFAULT_CUS_CRED_FILE ""

#define DEFAULT_REQ_TIMEOUT 3000
#define DEFAULT_CONNECTION_TIMEOUT 1000
#define DEFAULT_MAX_RETRY_COUNT 0
#define DEFAULT_MAX_CONNECTIONS 25

#define DEFAULT_ENDPOINT ""
#define DEFAULT_REGION "us-east-1"

#define DEFAULT_AUTH_TYPE AuthType::Type::AWS_PROFILE
#define DEFAULT_IDP_HOST ""
#define DEFAULT_IDP_USER_NAME ""
#define DEFAULT_IDP_PASSWORD ""
#define DEFAULT_IDP_ARN ""
#define DEFAULT_OKTA_APP_ID ""
#define DEFAULT_ROLE_ARN ""
#define DEFAULT_AAD_APP_ID ""
#define DEFAULT_ACCESS_CLIENT_SECRET ""
#define DEFAULT_AAD_TENANT ""
#define DEFAULT_LOG_LEVEL LogLevel::Type::ERROR_LEVEL

namespace ignite {
namespace odbc {
namespace config {
/**
 * ODBC configuration abstraction.
 */
class IGNITE_IMPORT_EXPORT Configuration {
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

    /** Default value for accessKeyIdFromProfile attribute. */
    static const std::string accessKeyIdFromProfile;

    /** Default value for secretKeyFromProfile attribute. */
    static const std::string secretKeyFromProfile;

    /** Default value for profileIsParsed attribute. */
    static bool profileIsParsed;

    /** Default value for profileName attribute. */
    static const std::string profileName;

    /** Default value for cusCredFile attribute. */
    static const std::string cusCredFile;

    /** Default value for reqTimeout attribute. */
    static const int32_t reqTimeout;

    /** Default value for connectionTimeout attribute. */
    static const int32_t connectionTimeout;

    /** Default value for maxRetryCount attribute. */
    static const int32_t maxRetryCount;

    /** Default value for maxConnections attribute. */
    static const int32_t maxConnections;

    /** Default value for endpoint attribute. */
    static const std::string endpoint;

    /** Default value for region attribute. */
    static const std::string region;

    /** Default value for authType attribute. */
    static const AuthType::Type authType;

    /** Default value for idPHost attribute. */
    static const std::string idPHost;

    /** Default value for idPUserName attribute. */
    static const std::string idPUserName;

    /** Default value for idPPassword attribute. */
    static const std::string idPPassword;

    /** Default value for idPArn attribute. */
    static const std::string idPArn;

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
   * @param secretKey secretKey.
   */
  void SetSecretKey(const std::string& secretKey);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsSecretKeySet() const;

  /**
   * Get accessKeyIdFromProfile.
   *
   * @return accessKeyIdFromProfile.
   */
  const std::string& GetAccessKeyIdFromProfile() const;

  /**
   * Set accessKeyIdFromProfile.
   *
   * @param accessKeyIdFromProfile accessKeyIdFromProfile.
   */
  void SetAccessKeyIdFromProfile(const std::string& accessKeyIdFromProfile);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsAccessKeyIdFromProfileSet() const;

  /**
   * Get secretKeyFromProfile.
   *
   * @return secretKeyFromProfile.
   */
  const std::string& GetSecretKeyFromProfile() const;

  /**
   * Set secretKeyFromProfile.
   *
   * @param secretKeyFromProfile secretKeyFromProfile.
   */
  void SetSecretKeyFromProfile(const std::string& secretKeyFromProfile);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsSecretKeyFromProfileSet() const;

  /**
   * Get profileIsParsed.
   *
   * @return profileIsParsed.
   */
  bool GetProfileIsParsed() const;

  /**
   * Set profileIsParsed.
   *
   * @param profileIsParsed profileIsParsed.
   */
  void SetProfileIsParsed(bool profileIsParsed);

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
   * Get profileName.
   *
   * @return name Profile Name.
   */
  const std::string& GetProfileName() const;

  /**
   * Set profileName.
   *
   * @param name Profile Name.
   */
  void SetProfileName(const std::string& name);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsProfileNameSet() const;

  // TODO remove custom credentials file from DSN
  // https://bitquill.atlassian.net/browse/AT-1079

  /**
   * Get path to Custom Credentials File.
   *
   * @return path to Custom Credentials File.
   */
  const std::string& GetCusCredFile() const;

  /**
   * Set path to Custom Credentials File.
   *
   * @param path Path to Custom Credentials File.
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
   * Get connection timeout in milliseconds.
   *
   * @return ms Connection timeout.
   */
  int32_t GetConnectionTimeout() const;

  /**
   * Set connection timeout in milliseconds.
   *
   * @param ms Connection timeout.
   */
  void SetConnectionTimeout(int32_t ms);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsConnectionTimeoutSet() const;

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
   * Get authType.
   *
   * @return value AuthType.
   */
  AuthType::Type GetAuthType() const;

  /**
   * Set authType.
   *
   * @param value AuthType.
   */
  void SetAuthType(const AuthType::Type value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsAuthTypeSet() const;

  /**
   * Get idPHost.
   *
   * @return value IdPHost.
   */
  const std::string& GetIdPHost() const;

  /**
   * Set idPHost.
   *
   * @param value IdPHost.
   */
  void SetIdPHost(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdPHostSet() const;

  /**
   * Get idPUserName.
   *
   * @return value IdPUserName.
   */
  const std::string& GetIdPUserName() const;

  /**
   * Set idPUserName.
   *
   * @param value IdPUserName.
   */
  void SetIdPUserName(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdPUserNameSet() const;

  /**
   * Get idPPassword.
   *
   * @return value IdPPassword.
   */
  const std::string& GetIdPPassword() const;

  /**
   * Set idPPassword.
   *
   * @param value IdPPassword.
   */
  void SetIdPPassword(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdPPasswordSet() const;

  /**
   * Get idPArn.
   *
   * @return value IdPArn.
   */
  const std::string& GetIdPArn() const;

  /**
   * Set idPArn.
   *
   * @param value IdPArn.
   */
  void SetIdPArn(const std::string& value);

  /**
   * Check if the value set.
   *
   * @return @true if the value set.
   */
  bool IsIdPArnSet() const;

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

  /** Driver Name. */
  SettableValue< std::string > driver = DefaultValue::driver;

  /** Access Key Id. */
  SettableValue< std::string > accessKeyId = DefaultValue::accessKeyId;

  /** Secret Key. */
  SettableValue< std::string > secretKey = DefaultValue::secretKey;

  /** Access Key Id from profile. */
  SettableValue< std::string > accessKeyIdFromProfile = DefaultValue::accessKeyIdFromProfile;

  /** Secret Key from profile. */
  SettableValue< std::string > secretKeyFromProfile = DefaultValue::secretKeyFromProfile;

  /** flag for profile is parsed or not. */
  SettableValue< bool > profileIsParsed = DefaultValue::profileIsParsed;

  /** Session Token. */
  SettableValue< std::string > sessionToken = DefaultValue::sessionToken;

  /** Profile Name. */
  SettableValue< std::string > profileName = DefaultValue::profileName;

  /** Custom Credentials file path. */
  SettableValue< std::string > cusCredFile = DefaultValue::cusCredFile;

  /** Request timeout in milliseconds.  */
  SettableValue< int32_t > reqTimeout = DefaultValue::reqTimeout;

  /** Connection timeout in milliseconds.  */
  SettableValue< int32_t > connectionTimeout = DefaultValue::connectionTimeout;

  /** Max Retry Count.  */
  SettableValue< int32_t > maxRetryCount = DefaultValue::maxRetryCount;

  /** Max Connections.  */
  SettableValue< int32_t > maxConnections = DefaultValue::maxConnections;

  /** Endpoint. */
  SettableValue< std::string > endpoint = DefaultValue::endpoint;

  /** Region. */
  SettableValue< std::string > region = DefaultValue::region;

  /** Auth Type. */
  SettableValue< AuthType::Type > authType = DefaultValue::authType;

  /** IdP Host. */
  SettableValue< std::string > idPHost = DefaultValue::idPHost;

  /** IdP User Name. */
  SettableValue< std::string > idPUserName = DefaultValue::idPUserName;

  /** IdP Password. */
  SettableValue< std::string > idPPassword = DefaultValue::idPPassword;

  /** IdP ARN. */
  SettableValue< std::string > idPArn = DefaultValue::idPArn;

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
void Configuration::AddToMap< AuthType::Type >(
    ArgumentMap& map, const std::string& key,
    const SettableValue< AuthType::Type >& value);

template <>
void Configuration::AddToMap< LogLevel::Type >(
    ArgumentMap& map, const std::string& key,
    const SettableValue< LogLevel::Type >& value);
}  // namespace config
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_CONFIG_CONFIGURATION
