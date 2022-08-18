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

#ifdef _WIN32
#include "ignite/odbc/system/ui/dsn_configuration_window.h"
#endif

#include <ignite/odbc/common/utils.h>
#include <ignite/odbc/config/config_tools.h>
#include <ignite/odbc/config/configuration.h>
#include <ignite/odbc/config/connection_string_parser.h>
#include <ignite/odbc/auth_type.h>
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
const std::string testProfileName = "testProfileName";
const std::string rootDir = ignite::odbc::common::GetEnv("REPOSITORY_ROOT");
#if (defined(__APPLE__) && defined(__MACH__)) || defined(__linux__)
const std::string testCusCredFile =
    rootDir + "/src/tests/input/test_credentials";
#elif defined(_WIN32)
const std::string testCusCredFile =
    rootDir + "\\src\\tests\\input\\test_credentials";
#else
const std::string testCusCredFile = "/path/to/credentials";
#endif

const int32_t testReqTimeoutMS = 300;
const int32_t testConnectionTimeoutMS = 500;
const int32_t testMaxRetryCount = 3;
const int32_t testMaxConnections = 15;
const std::string testEndpoint = "testEndpoint";
const std::string testRegion = "testRegion";
const AuthType::Type testAuthType = AuthType::FromString("aws_profile");
const std::string testIdPHost = "testIdPHost";
const std::string testIdPUserName = "testIdPUserName";
const std::string testIdPPassword = "testIdPPassword";
const std::string testIdPArn = "testIdPArn";
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

void CheckValidAuthType(const char* connectStr, AuthType::Type authType) {
  Configuration cfg;

  ParseValidConnectString(connectStr, cfg);

  BOOST_CHECK(cfg.GetAuthType() == authType);
}

void CheckInvalidAuthType(const char* connectStr) {
  Configuration cfg;

  ParseConnectStringWithError(connectStr, cfg);

  BOOST_CHECK(cfg.GetAuthType() == Configuration::DefaultValue::authType);
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
  BOOST_CHECK_EQUAL(cfg.GetProfileName(), testProfileName);
  BOOST_CHECK_EQUAL(cfg.GetCusCredFile(), testCusCredFile);
  BOOST_CHECK_EQUAL(cfg.GetAccessKeyIdFromProfile(), testAccessKeyId);
  BOOST_CHECK_EQUAL(cfg.GetSecretKeyFromProfile(), testSecretKey);
  BOOST_CHECK_EQUAL(cfg.GetProfileIsParsed(), true);
  BOOST_CHECK_EQUAL(cfg.GetReqTimeout(), testReqTimeoutMS);
  BOOST_CHECK_EQUAL(cfg.GetConnectionTimeout(), testConnectionTimeoutMS);
  BOOST_CHECK_EQUAL(cfg.GetMaxRetryCount(), testMaxRetryCount);
  BOOST_CHECK_EQUAL(cfg.GetMaxConnections(), testMaxConnections);
  BOOST_CHECK_EQUAL(cfg.GetEndpoint(), testEndpoint);
  BOOST_CHECK_EQUAL(cfg.GetRegion(), testRegion);
  BOOST_CHECK(cfg.GetAuthType() == testAuthType);
  BOOST_CHECK_EQUAL(cfg.GetIdPHost(), testIdPHost);
  BOOST_CHECK_EQUAL(cfg.GetIdPUserName(), testIdPUserName);
  BOOST_CHECK_EQUAL(cfg.GetIdPPassword(), testIdPPassword);
  BOOST_CHECK_EQUAL(cfg.GetIdPArn(), testIdPArn);
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
              << "auth=" << AuthType::ToString(testAuthType) << ';'
              << "connection_timeout=" << testConnectionTimeoutMS << ';'
              << ((testCusCredFile.find(' ') == std::string::npos)
                      ? ("custom_credentials_file=" + testCusCredFile + ";")
                      : ("custom_credentials_file={" + testCusCredFile + "};"))
              << "driver={" << testDriverName << "};"
              << "endpoint=" << testEndpoint << ';' << "idp_arn=" << testIdPArn
              << ';' << "idp_host=" << testIdPHost << ';'
              << "idp_password=" << testIdPPassword << ';'
              << "idp_user_name=" << testIdPUserName << ';'
              << "log_level=" << LogLevel::ToString(testLogLevel) << ';'
              << "log_path=" << testLogPath << ';'
              << "max_connections=" << testMaxConnections << ';'
              << "max_retry_count=" << testMaxRetryCount << ';'
              << "okta_app_id=" << testOktaAppId << ';'
              << "profile_name=" << testProfileName << ';'
              << "region=" << testRegion << ';'
              << "request_timeout=" << testReqTimeoutMS << ';'
              << "role_arn=" << testRoleArn << ';'
              << "secret_key=" << testSecretKey << ';'
              << "session_token=" << testSessionToken << ';';
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
  BOOST_CHECK_EQUAL(cfg.GetProfileName(),
                    Configuration::DefaultValue::profileName);
  BOOST_CHECK_EQUAL(cfg.GetCusCredFile(),
                    Configuration::DefaultValue::cusCredFile);
  BOOST_CHECK_EQUAL(cfg.GetReqTimeout(),
                    Configuration::DefaultValue::reqTimeout);
  BOOST_CHECK_EQUAL(cfg.GetConnectionTimeout(),
                    Configuration::DefaultValue::connectionTimeout);
  BOOST_CHECK_EQUAL(cfg.GetMaxRetryCount(),
                    Configuration::DefaultValue::maxRetryCount);
  BOOST_CHECK_EQUAL(cfg.GetMaxConnections(),
                    Configuration::DefaultValue::maxConnections);
  BOOST_CHECK_EQUAL(cfg.GetEndpoint(), Configuration::DefaultValue::endpoint);
  BOOST_CHECK_EQUAL(cfg.GetRegion(), Configuration::DefaultValue::region);
  BOOST_CHECK(cfg.GetAuthType() == Configuration::DefaultValue::authType);
  BOOST_CHECK_EQUAL(cfg.GetIdPHost(), Configuration::DefaultValue::idPHost);
  BOOST_CHECK_EQUAL(cfg.GetIdPUserName(),
                    Configuration::DefaultValue::idPUserName);
  BOOST_CHECK_EQUAL(cfg.GetIdPPassword(),
                    Configuration::DefaultValue::idPPassword);
  BOOST_CHECK_EQUAL(cfg.GetIdPArn(), Configuration::DefaultValue::idPArn);
  BOOST_CHECK_EQUAL(cfg.GetOktaAppId(), Configuration::DefaultValue::oktaAppId);
  BOOST_CHECK_EQUAL(cfg.GetRoleArn(), Configuration::DefaultValue::roleArn);
  BOOST_CHECK_EQUAL(cfg.GetAadAppId(), Configuration::DefaultValue::aadAppId);
  BOOST_CHECK_EQUAL(cfg.GetAadClientSecret(),
                    Configuration::DefaultValue::aadClientSecret);
  BOOST_CHECK_EQUAL(cfg.GetAadTenant(), Configuration::DefaultValue::aadTenant);
}

BOOST_AUTO_TEST_SUITE(ConfigurationTestSuite)

BOOST_AUTO_TEST_CASE(CheckTestValuesNotEqualDefault) {
  // authType value is the default value (aws_profile) so custom credentials
  // file will be parsed. Therefore there is no check for authType value in this
  // test case.
  BOOST_CHECK_NE(testDriverName, Configuration::DefaultValue::driver);
  BOOST_CHECK_NE(testDsn, Configuration::DefaultValue::dsn);
  BOOST_CHECK_NE(testAccessKeyId, Configuration::DefaultValue::accessKeyId);
  BOOST_CHECK_NE(testSecretKey, Configuration::DefaultValue::secretKey);
  BOOST_CHECK_NE(testSessionToken, Configuration::DefaultValue::sessionToken);
  BOOST_CHECK_NE(testProfileName, Configuration::DefaultValue::profileName);
  BOOST_CHECK_NE(testCusCredFile, Configuration::DefaultValue::cusCredFile);
  BOOST_CHECK_NE(testReqTimeoutMS, Configuration::DefaultValue::reqTimeout);
  BOOST_CHECK_NE(testConnectionTimeoutMS,
                 Configuration::DefaultValue::connectionTimeout);
  BOOST_CHECK_NE(testMaxRetryCount, Configuration::DefaultValue::maxRetryCount);
  BOOST_CHECK_NE(testMaxConnections,
                 Configuration::DefaultValue::maxConnections);
  BOOST_CHECK_NE(testEndpoint, Configuration::DefaultValue::endpoint);
  BOOST_CHECK_NE(testRegion, Configuration::DefaultValue::region);
  BOOST_CHECK_NE(testIdPHost, Configuration::DefaultValue::idPHost);
  BOOST_CHECK_NE(testIdPUserName, Configuration::DefaultValue::idPUserName);
  BOOST_CHECK_NE(testIdPPassword, Configuration::DefaultValue::idPPassword);
  BOOST_CHECK_NE(testIdPArn, Configuration::DefaultValue::idPArn);
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
              << "LOG_LEVEL=" << LogLevel::ToString(testLogLevel) << ';'
              << "LOG_PATH=" << testLogPath << ';'
              << "PROFILE_NAME=" << testProfileName << ';'
              << "CUSTOM_CREDENTIALS_FILE=" << testCusCredFile << ';'
              << "REQUEST_TIMEOUT=" << testReqTimeoutMS << ';'
              << "CONNECTION_TIMEOUT=" << testConnectionTimeoutMS << ';'
              << "MAX_RETRY_COUNT=" << testMaxRetryCount << ';'
              << "MAX_CONNECTIONS=" << testMaxConnections << ';'
              << "ENDPOINT=" << testEndpoint << ';' << "REGION=" << testRegion
              << ';' << "AUTH=" << AuthType::ToString(testAuthType) << ';'
              << "IDP_HOST=" << testIdPHost << ';'
              << "IDP_USER_NAME=" << testIdPUserName << ';'
              << "IDP_PASSWORD=" << testIdPPassword << ';'
              << "IDP_ARN=" << testIdPArn << ';'
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
              << "log_level=" << LogLevel::ToString(testLogLevel) << ';'
              << "log_path=" << testLogPath << ';'
              << "profile_name=" << testProfileName << ';'
              << "custom_credentials_file=" << testCusCredFile << ';'
              << "request_timeout=" << testReqTimeoutMS << ';'
              << "connection_timeout=" << testConnectionTimeoutMS << ';'
              << "max_retry_count=" << testMaxRetryCount << ';'
              << "max_connections=" << testMaxConnections << ';'
              << "endpoint=" << testEndpoint << ';' << "region=" << testRegion
              << ';' << "auth=" << AuthType::ToString(testAuthType) << ';'
              << "idp_host=" << testIdPHost << ';'
              << "idp_user_name=" << testIdPUserName << ';'
              << "idp_password=" << testIdPPassword << ';'
              << "idp_arn=" << testIdPArn << ';'
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
              << "log_level=" << LogLevel::ToString(testLogLevel) << ';'
              << "log_path=" << testLogPath << ';'
              << "profile_name=" << testProfileName << ';'
              << "custom_credentials_file=" << testCusCredFile << ';'
              << "request_timeout=" << testReqTimeoutMS << ';'
              << "connection_timeout=" << testConnectionTimeoutMS << ';'
              << "max_retry_count=" << testMaxRetryCount << ';'
              << "max_connections=" << testMaxConnections << ';'
              << "endpoint=" << testEndpoint << ';' << "region=" << testRegion
              << ';' << "auth=" << AuthType::ToString(testAuthType) << ';'
              << "idp_host=" << testIdPHost << ';'
              << "idp_user_name=" << testIdPUserName << ';'
              << "idp_password=" << testIdPPassword << ';'
              << "idp_arn=" << testIdPArn << ';'
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
              << "Log_Level=" << LogLevel::ToString(testLogLevel) << ';'
              << "Log_Path=" << testLogPath << ';'
              << "Profile_Name=" << testProfileName << ';'
              << "Custom_Credentials_File=" << testCusCredFile << ';'
              << "Request_Timeout=" << testReqTimeoutMS << ';'
              << "Connection_Timeout=" << testConnectionTimeoutMS << ';'
              << "Max_Retry_Count=" << testMaxRetryCount << ';'
              << "Max_Connections=" << testMaxConnections << ';'
              << "Endpoint=" << testEndpoint << ';' << "Region=" << testRegion
              << ';' << "Auth=" << AuthType::ToString(testAuthType) << ';'
              << "IdP_Host=" << testIdPHost << ';'
              << "IdP_User_Name=" << testIdPUserName << ';'
              << "IdP_Password=" << testIdPPassword << ';'
              << "IdP_Arn=" << testIdPArn << ';'
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
              << "  LOG_LEVEL =" << LogLevel::ToString(testLogLevel) << "  ; "
              << "LOG_PATH=  " << testLogPath << " ;"
              << "     PROFILE_NAME  = " << testProfileName << "    ; "
              << "  CUSTOM_CREDENTIALS_FILE=   " << testCusCredFile << ";  "
              << "  REQUEST_TIMEOUT=" << testReqTimeoutMS << "  ;  "
              << "CONNECTION_TIMEOUT=  " << testConnectionTimeoutMS << ";  "
              << "MAX_RETRY_COUNT=  " << testMaxRetryCount << " ;"
              << "MAX_CONNECTIONS=  " << testMaxConnections << "  ; "
              << "ENDPOINT=" << testEndpoint << "  ; "
              << "REGION=" << testRegion << "  ; "
              << "AUTH=" << AuthType::ToString(testAuthType) << " ;  "
              << "IDP_HOST=" << testIdPHost << ";  "
              << "IDP_USER_NAME=" << testIdPUserName << ";  "
              << "IDP_PASSWORD=" << testIdPPassword << "  ; "
              << "IDP_ARN=" << testIdPArn << " ;   "
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

BOOST_AUTO_TEST_CASE(TestConnectStringInvalidAuthType) {
  CheckInvalidAuthType("auth=tableau;");
  CheckInvalidAuthType("auth=aat;");
}

BOOST_AUTO_TEST_CASE(TestConnectStringValidAuthType) {
  CheckValidAuthType("auth=aws_profile;", AuthType::Type::AWS_PROFILE);
  CheckValidAuthType("auth=iam;", AuthType::Type::IAM);
  CheckValidAuthType("auth=aad;", AuthType::Type::AAD);
  CheckValidAuthType("auth=okta;", AuthType::Type::OKTA);
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

#ifdef _WIN32
BOOST_AUTO_TEST_CASE(TestParseDriverVersion) {
  using ignite::odbc::system::ui::DsnConfigurationWindow;

  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.00.0000"), L"V.2.0.0");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.01.0000"), L"V.2.1.0");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.10.0000"),
      L"V.2.10.0");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("12.00.0000"),
      L"V.12.0.0");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.01.1000"),
      L"V.2.1.1000");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.01.0100"),
      L"V.2.1.100");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.10.0010"),
      L"V.2.10.10");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.01.0200"),
      L"V.2.1.200");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.01.0201"),
      L"V.2.1.201");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("02.10.1001"),
      L"V.2.10.1001");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("12.10.0001"),
      L"V.12.10.1");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("08.01.0001"), L"V.8.1.1");
  BOOST_CHECK_EQUAL(
      DsnConfigurationWindow::GetParsedDriverVersion("88.88.8888"), L"V.88.88.8888");
}
#endif

BOOST_AUTO_TEST_SUITE_END()
