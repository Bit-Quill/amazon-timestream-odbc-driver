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

#include <ignite/odbc/common/utils.h>
#include <ignite/odbc/config/config_tools.h>
#include <ignite/odbc/config/configuration.h>
#include <ignite/odbc/config/connection_string_parser.h>
#include <ignite/odbc/cred_prov_class.h>
#include <ignite/odbc/idp_name.h>
#include <ignite/odbc/log.h>
#include <ignite/odbc/log_level.h>
#include <ignite/odbc/odbc_error.h>

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <set>

#include "ignite/odbc/diagnostic/diagnostic_record_storage.h"

using namespace ignite::odbc;
using namespace ignite::odbc::config;
using ignite::odbc::common::EncodeURIComponent;
using namespace boost::unit_test;

namespace {
const std::string testDriverName = "Test Driver";
const std::string testDsn = "Test DSN";
const std::string testAccessKeyId = "testAccessKeyId";
const std::string testSecretKey = "testSecretKey";
const std::string testSessionToken = "testSessionToken";
const bool testEnableMetadataPreparedStatement = true;
const CredProvClass::Type testCredProvClass = CredProvClass::FromString("PropertiesFileCredentialsProvider");

const std::string rootDir = ignite::odbc::common::GetEnv("REPOSITORY_ROOT");
#if defined(PREDEF_PLATFORM_UNIX_OR_APPLE) || defined(__linux__)
const std::string testCusCredFile = rootDir + "/src/tests/input/test_credentials";
#elif defined(_WIN32)
const std::string testCusCredFile = rootDir + "\\src\\tests\\input\\test_credentials";
#else
const std::string testCusCredFile = "/path/to/credentials";
#endif

const int32_t testReqTimeoutMS = 300000;
const int32_t testSocketTimeoutMS = 200000;
const int32_t testMaxRetryCount = 3;
const int32_t testMaxConnections = 15;
const std::string testEndpoint = "testEndpoint";
const std::string testRegion = "testRegion";
const IdpName::Type testIdpName = IdpName::FromString("azuread");
const std::string testIdpHost = "testIdpHost";
const std::string testIdpUserName = "testIdpUserName";
const std::string testIdpPassword = "testIdpPassword";
const std::string testIdpArn = "testIdpArn";
const std::string testOktaAppId = "testOktaAppId";
const std::string testRoleArn = "testRoleArn";
const std::string testAadAppId = "testAadAppId";
const std::string testAadClientSecret = "testAadClientSecret";
const std::string testAadTenant = "testAadTenant";
const std::string testLogPath = Logger::GetLoggerInstance()->GetLogPath();
const LogLevel::Type testLogLevel = Logger::GetLoggerInstance()->GetLogLevel();
}  // namespace

const char* BoolToStr(bool val, bool lowerCase = true) {
  if (lowerCase)
    return val ? "true" : "false";

  return val ? "TRUE" : "FALSE";
}

void ParseValidDsnString(const std::string& dsnStr, Configuration& cfg) {
  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  BOOST_CHECK_NO_THROW(parser.ParseConfigAttributes(dsnStr.c_str(), &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

void ParseValidConnectString(const std::string& connectStr,
                             Configuration& cfg) {
  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectStr, &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

void ParseConnectStringWithError(const std::string& connectStr,
                                 Configuration& cfg) {
  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectStr, &diag));

  BOOST_CHECK_NE(diag.GetStatusRecordsNumber(), 0);
}

void CheckValidIdpName(const char* connectStr, IdpName::Type idpName) {
  Configuration cfg;

  ParseValidConnectString(connectStr, cfg);

  BOOST_CHECK(cfg.GetIdpName() == idpName);
}

void CheckInvalidIdpName(const char* connectStr) {
  Configuration cfg;

  ParseConnectStringWithError(connectStr, cfg);

  BOOST_CHECK(cfg.GetIdpName() == Configuration::DefaultValue::idpName);
}

void CheckValidLogLevel(const char* connectStr, LogLevel::Type loglevel) {
  Configuration cfg;

  ParseValidConnectString(connectStr, cfg);

  BOOST_CHECK(cfg.GetLogLevel() == loglevel);
}

void CheckInvalidLogLevel(const char* connectStr) {
  Configuration cfg;

  ParseConnectStringWithError(connectStr, cfg);

  BOOST_CHECK(cfg.GetLogLevel() == Configuration::DefaultValue::logLevel);
}

void CheckValidBoolValue(const std::string& connectStr, const std::string& key,
                         bool val) {
  Configuration cfg;

  ParseValidConnectString(connectStr, cfg);

  Configuration::ArgumentMap map;
  cfg.ToMap(map);

  std::string expected = val ? "true" : "false";

  BOOST_CHECK_EQUAL(map[key], expected);
}

void CheckInvalidBoolValue(const std::string& connectStr,
                           const std::string& key) {
  Configuration cfg;

  ParseConnectStringWithError(connectStr, cfg);

  Configuration::ArgumentMap map;
  cfg.ToMap(map);

  BOOST_CHECK(map[key].empty());
}

void CheckConnectionConfig(const Configuration& cfg) {
  BOOST_CHECK_EQUAL(cfg.GetDriver(), testDriverName);
  BOOST_CHECK_EQUAL(cfg.GetAccessKeyId(), testAccessKeyId);
  BOOST_CHECK_EQUAL(cfg.GetSecretKey(), testSecretKey);
  BOOST_CHECK_EQUAL(cfg.GetSessionToken(), testSessionToken);
  BOOST_CHECK_EQUAL(cfg.IsEnableMetadataPreparedStatement(),
                    testEnableMetadataPreparedStatement);
  BOOST_CHECK(cfg.GetCredProvClass() == testCredProvClass);
  BOOST_CHECK_EQUAL(cfg.GetCusCredFile(), testCusCredFile);
  BOOST_CHECK_EQUAL(cfg.GetAccessKeyIdFromProfile(), testAccessKeyId);
  BOOST_CHECK_EQUAL(cfg.GetSecretKeyFromProfile(), testSecretKey);
  BOOST_CHECK_EQUAL(cfg.GetProfileIsParsed(), true);
  BOOST_CHECK_EQUAL(cfg.GetReqTimeout(), testReqTimeoutMS);
  BOOST_CHECK_EQUAL(cfg.GetSocketTimeout(), testSocketTimeoutMS);
  BOOST_CHECK_EQUAL(cfg.GetMaxRetryCount(), testMaxRetryCount);
  BOOST_CHECK_EQUAL(cfg.GetMaxConnections(), testMaxConnections);
  BOOST_CHECK_EQUAL(cfg.GetEndpoint(), testEndpoint);
  BOOST_CHECK_EQUAL(cfg.GetRegion(), testRegion);
  BOOST_CHECK(cfg.GetIdpName() == testIdpName);
  BOOST_CHECK_EQUAL(cfg.GetIdpHost(), testIdpHost);
  BOOST_CHECK_EQUAL(cfg.GetIdpUserName(), testIdpUserName);
  BOOST_CHECK_EQUAL(cfg.GetIdpPassword(), testIdpPassword);
  BOOST_CHECK_EQUAL(cfg.GetIdpArn(), testIdpArn);
  BOOST_CHECK_EQUAL(cfg.GetOktaAppId(), testOktaAppId);
  BOOST_CHECK_EQUAL(cfg.GetRoleArn(), testRoleArn);
  BOOST_CHECK_EQUAL(cfg.GetAadAppId(), testAadAppId);
  BOOST_CHECK_EQUAL(cfg.GetAadClientSecret(), testAadClientSecret);
  BOOST_CHECK_EQUAL(cfg.GetAadTenant(), testAadTenant);
  BOOST_CHECK(cfg.GetLogLevel() == testLogLevel);
  BOOST_CHECK(cfg.GetLogPath() == testLogPath);
  BOOST_CHECK(!cfg.IsDsnSet());

  // the expected string is in alphabetical order
  std::stringstream constructor;
  constructor << "aad_app_id=" << testAadAppId << ';'
              << "aad_client_secret=" << testAadClientSecret << ';'
              << "aad_tenant=" << testAadTenant << ';'
              << "access_key_id=" << testAccessKeyId << ';'
              << "aws_credentials_provider_class="
              << CredProvClass::ToString(testCredProvClass) << ';'
              << "custom_credentials_file=" << testCusCredFile << ';'
              << "driver={" << testDriverName << "};"
              << "enable_metadata_prepared_statement="
              << BoolToStr(testEnableMetadataPreparedStatement) << ';'
              << "endpoint=" << testEndpoint << ';' << "idp_arn=" << testIdpArn
              << ';' << "idp_host=" << testIdpHost << ';'
              << "idp_name=" << IdpName::ToString(testIdpName) << ';'
              << "idp_password=" << testIdpPassword << ';'
              << "idp_user_name=" << testIdpUserName << ';'
              << "log_level=" << LogLevel::ToString(testLogLevel) << ';'
              << "log_path=" << testLogPath << ';'
              << "max_connections=" << testMaxConnections << ';'
              << "max_retry_count=" << testMaxRetryCount << ';'
              << "okta_app_id=" << testOktaAppId << ';'
              << "region=" << testRegion << ';'
              << "request_timeout=" << testReqTimeoutMS << ';'
              << "role_arn=" << testRoleArn << ';'
              << "secret_key=" << testSecretKey << ';'
              << "session_token=" << testSessionToken << ';'
              << "socket_timeout=" << testSocketTimeoutMS << ';';
  const std::string& expectedStr = constructor.str();

  BOOST_CHECK_EQUAL(ignite::odbc::common::ToLower(cfg.ToConnectString()),
                    ignite::odbc::common::ToLower(expectedStr));
}

void CheckDsnConfig(const Configuration& cfg) {
  // since setting logger path/level will change the logger settings,
  // we will not change the logger path/level in configuration_test,
  // which means it is possible for the logger path/level to be equivalent to
  // the default values. Therefore, there will be no boost check for logger
  // path/level.
  BOOST_CHECK_EQUAL(cfg.GetDriver(), testDriverName);
  BOOST_CHECK_EQUAL(cfg.GetDsn(), testDsn);
  BOOST_CHECK_EQUAL(cfg.GetAccessKeyId(),
                    Configuration::DefaultValue::accessKeyId);
  BOOST_CHECK_EQUAL(cfg.GetSecretKey(), Configuration::DefaultValue::secretKey);
  BOOST_CHECK_EQUAL(cfg.GetSessionToken(),
                    Configuration::DefaultValue::sessionToken);
  BOOST_CHECK_EQUAL(
      cfg.IsEnableMetadataPreparedStatement(),
      Configuration::DefaultValue::enableMetadataPreparedStatement);
  BOOST_CHECK(cfg.GetCredProvClass() == Configuration::DefaultValue::credProvClass);
  BOOST_CHECK_EQUAL(cfg.GetCusCredFile(),
                    Configuration::DefaultValue::cusCredFile);
  BOOST_CHECK_EQUAL(cfg.GetReqTimeout(),
                    Configuration::DefaultValue::reqTimeout);
  BOOST_CHECK_EQUAL(cfg.GetSocketTimeout(),
                    Configuration::DefaultValue::socketTimeout);
  BOOST_CHECK_EQUAL(cfg.GetMaxRetryCount(),
                    Configuration::DefaultValue::maxRetryCount);
  BOOST_CHECK_EQUAL(cfg.GetMaxConnections(),
                    Configuration::DefaultValue::maxConnections);
  BOOST_CHECK_EQUAL(cfg.GetEndpoint(), Configuration::DefaultValue::endpoint);
  BOOST_CHECK_EQUAL(cfg.GetRegion(), Configuration::DefaultValue::region);
  BOOST_CHECK(cfg.GetIdpName() == Configuration::DefaultValue::idpName);
  BOOST_CHECK_EQUAL(cfg.GetIdpHost(), Configuration::DefaultValue::idpHost);
  BOOST_CHECK_EQUAL(cfg.GetIdpUserName(),
                    Configuration::DefaultValue::idpUserName);
  BOOST_CHECK_EQUAL(cfg.GetIdpPassword(),
                    Configuration::DefaultValue::idpPassword);
  BOOST_CHECK_EQUAL(cfg.GetIdpArn(), Configuration::DefaultValue::idpArn);
  BOOST_CHECK_EQUAL(cfg.GetOktaAppId(), Configuration::DefaultValue::oktaAppId);
  BOOST_CHECK_EQUAL(cfg.GetRoleArn(), Configuration::DefaultValue::roleArn);
  BOOST_CHECK_EQUAL(cfg.GetAadAppId(), Configuration::DefaultValue::aadAppId);
  BOOST_CHECK_EQUAL(cfg.GetAadClientSecret(),
                    Configuration::DefaultValue::aadClientSecret);
  BOOST_CHECK_EQUAL(cfg.GetAadTenant(), Configuration::DefaultValue::aadTenant);
}

BOOST_AUTO_TEST_SUITE(ConfigurationTestSuite)

BOOST_AUTO_TEST_CASE(CheckTestValuesNotEqualDefault) {
  BOOST_CHECK_NE(testDriverName, Configuration::DefaultValue::driver);
  BOOST_CHECK_NE(testDsn, Configuration::DefaultValue::dsn);
  BOOST_CHECK_NE(testAccessKeyId, Configuration::DefaultValue::accessKeyId);
  BOOST_CHECK_NE(testSecretKey, Configuration::DefaultValue::secretKey);
  BOOST_CHECK_NE(testSessionToken, Configuration::DefaultValue::sessionToken);
  BOOST_CHECK_NE(testEnableMetadataPreparedStatement,
                 Configuration::DefaultValue::enableMetadataPreparedStatement);
  BOOST_CHECK(testCredProvClass != Configuration::DefaultValue::credProvClass);
  BOOST_CHECK_NE(testCusCredFile, Configuration::DefaultValue::cusCredFile);
  BOOST_CHECK_NE(testReqTimeoutMS, Configuration::DefaultValue::reqTimeout);
  BOOST_CHECK_NE(testSocketTimeoutMS,
                 Configuration::DefaultValue::socketTimeout);
  BOOST_CHECK_NE(testMaxRetryCount, Configuration::DefaultValue::maxRetryCount);
  BOOST_CHECK_NE(testMaxConnections,
                 Configuration::DefaultValue::maxConnections);
  BOOST_CHECK_NE(testEndpoint, Configuration::DefaultValue::endpoint);
  BOOST_CHECK_NE(testRegion, Configuration::DefaultValue::region);
  BOOST_CHECK(testIdpName != Configuration::DefaultValue::idpName);
  BOOST_CHECK_NE(testIdpHost, Configuration::DefaultValue::idpHost);
  BOOST_CHECK_NE(testIdpUserName, Configuration::DefaultValue::idpUserName);
  BOOST_CHECK_NE(testIdpPassword, Configuration::DefaultValue::idpPassword);
  BOOST_CHECK_NE(testIdpArn, Configuration::DefaultValue::idpArn);
  BOOST_CHECK_NE(testOktaAppId, Configuration::DefaultValue::oktaAppId);
  BOOST_CHECK_NE(testRoleArn, Configuration::DefaultValue::roleArn);
  BOOST_CHECK_NE(testAadAppId, Configuration::DefaultValue::aadAppId);
  BOOST_CHECK_NE(testAadClientSecret,
                 Configuration::DefaultValue::aadClientSecret);
  BOOST_CHECK_NE(testAadTenant, Configuration::DefaultValue::aadTenant);
}

BOOST_AUTO_TEST_CASE(TestConnectStringUppercase) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << "ACCESS_KEY_ID=" << testAccessKeyId << ';'
              << "SECRET_KEY=" << testSecretKey << ';'
              << "SESSION_TOKEN=" << testSessionToken << ';'
              << "ENABLE_METADATA_PREPARED_STATEMENT="
              << BoolToStr(testEnableMetadataPreparedStatement) << ';'
              << "LOG_LEVEL=" << LogLevel::ToString(testLogLevel) << ';'
              << "LOG_PATH=" << testLogPath << ';'
              << "AWS_CREDENTIALS_PROVIDER_CLASS="
              << CredProvClass::ToString(testCredProvClass) << ';'
              << "CUSTOM_CREDENTIALS_FILE=" << testCusCredFile << ';'
              << "REQUEST_TIMEOUT=" << testReqTimeoutMS << ';'
              << "SOCKET_TIMEOUT=" << testSocketTimeoutMS << ';'
              << "MAX_RETRY_COUNT=" << testMaxRetryCount << ';'
              << "MAX_CONNECTIONS=" << testMaxConnections << ';'
              << "ENDPOINT=" << testEndpoint << ';' << "REGION=" << testRegion
              << ';' << "IDP_NAME=" << IdpName::ToString(testIdpName) << ';'
              << "IDP_HOST=" << testIdpHost << ';'
              << "IDP_USER_NAME=" << testIdpUserName << ';'
              << "IDP_PASSWORD=" << testIdpPassword << ';'
              << "IDP_ARN=" << testIdpArn << ';'
              << "OKTA_APP_ID=" << testOktaAppId << ';'
              << "ROLE_ARN=" << testRoleArn << ';'
              << "AAD_APP_ID=" << testAadAppId << ';'
              << "AAD_CLIENT_SECRET=" << testAadClientSecret << ';'
              << "AAD_TENANT=" << testAadTenant << ';' << "DRIVER={"
              << testDriverName << "};";

  const std::string& connectStr = constructor.str();

  ParseValidConnectString(connectStr, cfg);

  CheckConnectionConfig(cfg);
}

BOOST_AUTO_TEST_CASE(TestConnectStringLowercase) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << "access_key_id=" << testAccessKeyId << ';'
              << "secret_key=" << testSecretKey << ';'
              << "session_token=" << testSessionToken << ';'
              << "enable_metadata_prepared_statement="
              << BoolToStr(testEnableMetadataPreparedStatement) << ';'
              << "log_level=" << LogLevel::ToString(testLogLevel) << ';'
              << "log_path=" << testLogPath << ';'
              << "aws_credentials_provider_class="
              << CredProvClass::ToString(testCredProvClass) << ';'
              << "custom_credentials_file=" << testCusCredFile << ';'
              << "request_timeout=" << testReqTimeoutMS << ';'
              << "socket_timeout=" << testSocketTimeoutMS << ';'
              << "max_retry_count=" << testMaxRetryCount << ';'
              << "max_connections=" << testMaxConnections << ';'
              << "endpoint=" << testEndpoint << ';' << "region=" << testRegion
              << ';' << "idp_name=" << IdpName::ToString(testIdpName) << ';'
              << "idp_host=" << testIdpHost << ';'
              << "idp_user_name=" << testIdpUserName << ';'
              << "idp_password=" << testIdpPassword << ';'
              << "idp_arn=" << testIdpArn << ';'
              << "okta_app_id=" << testOktaAppId << ';'
              << "role_arn=" << testRoleArn << ';'
              << "aad_app_id=" << testAadAppId << ';'
              << "aad_client_secret=" << testAadClientSecret << ';'
              << "aad_tenant=" << testAadTenant << ';' << "driver={"
              << testDriverName << "};";

  const std::string& connectStr = constructor.str();

  ParseValidConnectString(connectStr, cfg);

  CheckConnectionConfig(cfg);
}

BOOST_AUTO_TEST_CASE(TestConnectStringZeroTerminated) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << "access_key_id=" << testAccessKeyId << ';'
              << "secret_key=" << testSecretKey << ';'
              << "session_token=" << testSessionToken << ';'
              << "enable_metadata_prepared_statement="
              << BoolToStr(testEnableMetadataPreparedStatement) << ';'
              << "log_level=" << LogLevel::ToString(testLogLevel) << ';'
              << "log_path=" << testLogPath << ';'
              << "aws_credentials_provider_class="
              << CredProvClass::ToString(testCredProvClass) << ';'
              << "custom_credentials_file=" << testCusCredFile << ';'
              << "request_timeout=" << testReqTimeoutMS << ';'
              << "socket_timeout=" << testSocketTimeoutMS << ';'
              << "max_retry_count=" << testMaxRetryCount << ';'
              << "max_connections=" << testMaxConnections << ';'
              << "endpoint=" << testEndpoint << ';' << "region=" << testRegion
              << ';' << "idp_name=" << IdpName::ToString(testIdpName) << ';'
              << "idp_host=" << testIdpHost << ';'
              << "idp_user_name=" << testIdpUserName << ';'
              << "idp_password=" << testIdpPassword << ';'
              << "idp_arn=" << testIdpArn << ';'
              << "okta_app_id=" << testOktaAppId << ';'
              << "role_arn=" << testRoleArn << ';'
              << "aad_app_id=" << testAadAppId << ';'
              << "aad_client_secret=" << testAadClientSecret << ';'
              << "aad_tenant=" << testAadTenant << ';' << "driver={"
              << testDriverName << "};";

  std::string connectStr = constructor.str();

  connectStr.push_back(0);

  ParseValidConnectString(connectStr, cfg);

  CheckConnectionConfig(cfg);
}

BOOST_AUTO_TEST_CASE(TestConnectStringMixed) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << "Access_Key_Id=" << testAccessKeyId << ';'
              << "Secret_Key=" << testSecretKey << ';'
              << "Session_Token=" << testSessionToken << ';'
              << "Enable_Metadata_Prepared_Statement="
              << BoolToStr(testEnableMetadataPreparedStatement) << ';'
              << "Log_Level=" << LogLevel::ToString(testLogLevel) << ';'
              << "Log_Path=" << testLogPath << ';'
              << "AWS_Credentials_Provider_Class="
              << CredProvClass::ToString(testCredProvClass) << ';'
              << "Custom_Credentials_File=" << testCusCredFile << ';'
              << "Request_Timeout=" << testReqTimeoutMS << ';'
              << "Socket_Timeout=" << testSocketTimeoutMS << ';'
              << "Max_Retry_Count=" << testMaxRetryCount << ';'
              << "Max_Connections=" << testMaxConnections << ';'
              << "Endpoint=" << testEndpoint << ';' << "Region=" << testRegion
              << ';' << "Idp_Name=" << IdpName::ToString(testIdpName) << ';'
              << "Idp_Host=" << testIdpHost << ';'
              << "Idp_User_Name=" << testIdpUserName << ';'
              << "Idp_Password=" << testIdpPassword << ';'
              << "Idp_Arn=" << testIdpArn << ';'
              << "Okta_App_Id=" << testOktaAppId << ';'
              << "Role_Arn=" << testRoleArn << ';'
              << "Aad_App_Id=" << testAadAppId << ';'
              << "Aad_Client_Secret=" << testAadClientSecret << ';'
              << "Aad_Tenant=" << testAadTenant << ';' << "Driver={"
              << testDriverName << "};";

  const std::string& connectStr = constructor.str();

  ParseValidConnectString(connectStr, cfg);

  CheckConnectionConfig(cfg);
}

BOOST_AUTO_TEST_CASE(TestConnectStringWhiteSpaces) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << "ACCESS_KEY_ID =" << testAccessKeyId << ';'
              << "SECRET_KEY=" << testSecretKey << ';'
              << "SESSION_TOKEN=" << testSessionToken << ';'
              << "ENABLE_METADATA_PREPARED_STATEMENT="
              << BoolToStr(testEnableMetadataPreparedStatement) << ";  "
              << "  LOG_LEVEL =" << LogLevel::ToString(testLogLevel) << "  ; "
              << "LOG_PATH=  " << testLogPath << " ;"
              << " AWS_CREDENTIALS_PROVIDER_CLASS="
              << CredProvClass::ToString(testCredProvClass) << " ; "
              << "  CUSTOM_CREDENTIALS_FILE=   " << testCusCredFile << ";  "
              << "  REQUEST_TIMEOUT=" << testReqTimeoutMS << "  ;  "
              << "SOCKET_TIMEOUT=  " << testSocketTimeoutMS << ";  "
              << "MAX_RETRY_COUNT=  " << testMaxRetryCount << " ;"
              << "MAX_CONNECTIONS=  " << testMaxConnections << "  ; "
              << "ENDPOINT=" << testEndpoint << "  ; "
              << "REGION=" << testRegion << "  ; "
              << "IDP_NAME=" << IdpName::ToString(testIdpName) << " ;  "
              << "IDP_HOST=" << testIdpHost << ";  "
              << "IDP_USER_NAME=" << testIdpUserName << ";  "
              << "IDP_PASSWORD=" << testIdpPassword << "  ; "
              << "IDP_ARN=" << testIdpArn << " ;   "
              << "OKTA_APP_ID=" << testOktaAppId << "  ;  "
              << "ROLE_ARN=" << testRoleArn << ";  "
              << "AAD_APP_ID=" << testAadAppId << ";  "
              << "AAD_CLIENT_SECRET=" << testAadClientSecret << "  ; "
              << "AAD_TENANT=" << testAadTenant << "    ;"
              << "DRIVER = {" << testDriverName << "};";

  const std::string& connectStr = constructor.str();

  ParseValidConnectString(connectStr, cfg);

  CheckConnectionConfig(cfg);
}

BOOST_AUTO_TEST_CASE(TestConnectStringInvalidIdpName) {
  CheckInvalidIdpName("idp_name=tableau;");
  CheckInvalidIdpName("idp_name=aat;");
}

BOOST_AUTO_TEST_CASE(TestConnectStringValidIdpName) {
  CheckValidIdpName("idp_name=none;", IdpName::Type::NONE);
  CheckValidIdpName("idp_name=azuread;", IdpName::Type::AAD);
  CheckValidIdpName("idp_name=okta;", IdpName::Type::OKTA);
}

BOOST_AUTO_TEST_CASE(TestConnectStringInvalidLogLevel) {
  CheckInvalidLogLevel("log_level=debug_level;");
  CheckInvalidLogLevel("log_level=off_level;");
}

BOOST_AUTO_TEST_CASE(TestConnectStringValidLogLevel) {
  CheckValidLogLevel("log_level=debug;", LogLevel::Type::DEBUG_LEVEL);
  CheckValidLogLevel("log_level=info;", LogLevel::Type::INFO_LEVEL);
  CheckValidLogLevel("log_level=error;", LogLevel::Type::ERROR_LEVEL);
  CheckValidLogLevel("log_level=off;", LogLevel::Type::OFF);
}

BOOST_AUTO_TEST_CASE(TestConnectStringInvalidBoolKeys) {
  typedef std::set< std::string > Set;

  Set keys;

  keys.emplace("enable_metadata_prepared_statement");

  for (auto it = keys.begin(); it != keys.end(); ++it) {
    const std::string& key = *it;

    CheckInvalidBoolValue(key + "=1;", key);
    CheckInvalidBoolValue(key + "=0;", key);
    CheckInvalidBoolValue(key + "=42;", key);
    CheckInvalidBoolValue(key + "=truee;", key);
    CheckInvalidBoolValue(key + "=flase;", key);
    CheckInvalidBoolValue(key + "=falsee;", key);
    CheckInvalidBoolValue(key + "=yes;", key);
    CheckInvalidBoolValue(key + "=no;", key);
  }
}

BOOST_AUTO_TEST_CASE(TestConnectStringValidBoolKeys) {
  typedef std::set< std::string > Set;

  Set keys;

  keys.emplace("enable_metadata_prepared_statement");

  for (auto it = keys.begin(); it != keys.end(); ++it) {
    const std::string& key = *it;

    CheckValidBoolValue(key + "=true;", key, true);
    CheckValidBoolValue(key + "=True;", key, true);
    CheckValidBoolValue(key + "=TRUE;", key, true);

    CheckValidBoolValue(key + "=false;", key, false);
    CheckValidBoolValue(key + "=False;", key, false);
    CheckValidBoolValue(key + "=FALSE;", key, false);
  }
}

BOOST_AUTO_TEST_CASE(TestDsnStringUppercase) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << "DRIVER=" << testDriverName << '\0' << "DSN={" << testDsn
              << "}" << '\0' << '\0';

  const std::string& configStr = constructor.str();

  ParseValidDsnString(configStr, cfg);

  CheckDsnConfig(cfg);
}

BOOST_AUTO_TEST_CASE(TestDsnStringLowercase) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << "driver=" << testDriverName << '\0' << "dsn={" << testDsn
              << "}" << '\0' << '\0';

  const std::string& configStr = constructor.str();

  ParseValidDsnString(configStr, cfg);

  CheckDsnConfig(cfg);
}

BOOST_AUTO_TEST_CASE(TestDsnStringMixed) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << "Driver=" << testDriverName << '\0' << "Dsn={" << testDsn
              << "}" << '\0' << '\0';

  const std::string& configStr = constructor.str();

  ParseValidDsnString(configStr, cfg);

  CheckDsnConfig(cfg);
}

BOOST_AUTO_TEST_CASE(TestDsnStringWhitespaces) {
  Configuration cfg;

  std::stringstream constructor;

  constructor << " DRIVER =  " << testDriverName << "\r\n"
              << '\0' << "DSN= {" << testDsn << "} \n"
              << '\0' << '\0';

  const std::string& configStr = constructor.str();

  ParseValidDsnString(configStr, cfg);

  CheckDsnConfig(cfg);
}

BOOST_AUTO_TEST_SUITE_END()
