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

#define BOOST_TEST_MODULE TimestreamUnitTest
#include <string>

#include <odbc_unit_test_suite.h>
#include "ignite/odbc/log.h"
#include "ignite/odbc/log_level.h"
#include <ignite/odbc/common/platform_utils.h>
#include <ignite/odbc/auth_type.h>
#include "mock/mock_timestream_service.h"
#include "ignite/odbc/log.h"

using ignite::odbc::AuthType;
using ignite::odbc::OdbcUnitTestSuite;
using ignite::odbc::MockConnection;
using ignite::odbc::MockTimestreamService;
using ignite::odbc::config::Configuration;
using namespace boost::unit_test;

#ifdef _WIN32
// For Windows only, add static variables definition here in case of 
// "unresolved symbol" error when linking.
using namespace ignite::odbc::config;
using namespace ignite::odbc;

// mirrored from src/odbc/src/config/configuration.cpp
// Connection (Basic Authentication) Settings
const std::string Configuration::DefaultValue::dsn = DEFAULT_DSN;
const std::string Configuration::DefaultValue::driver = DEFAULT_DRIVER;
const std::string Configuration::DefaultValue::uid =
    DEFAULT_UID; 
    const std::string Configuration::DefaultValue::pwd =
    DEFAULT_PWD;
const std::string Configuration::DefaultValue::accessKeyId =
    DEFAULT_ACCESS_KEY_ID;
const std::string Configuration::DefaultValue::secretKey = DEFAULT_SECRET_KEY;
const std::string Configuration::DefaultValue::sessionToken =
    DEFAULT_SESSION_TOKEN;

// Credential Providers Options
const std::string Configuration::DefaultValue::profileName =
    DEFAULT_PROFILE_NAME;

// Connection Options
const int32_t Configuration::DefaultValue::reqTimeout = DEFAULT_REQ_TIMEOUT;
const int32_t Configuration::DefaultValue::connectionTimeout =
    DEFAULT_CONNECTION_TIMEOUT;
const int32_t Configuration::DefaultValue::maxRetryCountClient =
    DEFAULT_MAX_RETRY_COUNT_CLIENT;
const int32_t Configuration::DefaultValue::maxConnections =
    DEFAULT_MAX_CONNECTIONS;

// Endpoint Options
const std::string Configuration::DefaultValue::endpoint = DEFAULT_ENDPOINT;
const std::string Configuration::DefaultValue::region = DEFAULT_REGION;

// Advance Authentication Settings
const AuthType::Type Configuration::DefaultValue::authType = DEFAULT_AUTH_TYPE;
const std::string Configuration::DefaultValue::idPHost = DEFAULT_IDP_HOST;
const std::string Configuration::DefaultValue::idPUserName =
    DEFAULT_IDP_USER_NAME;
const std::string Configuration::DefaultValue::idPPassword =
    DEFAULT_IDP_PASSWORD;
const std::string Configuration::DefaultValue::idPArn = DEFAULT_IDP_ARN;
const std::string Configuration::DefaultValue::oktaAppId = DEFAULT_OKTA_APP_ID;
const std::string Configuration::DefaultValue::roleArn = DEFAULT_ROLE_ARN;
const std::string Configuration::DefaultValue::aadAppId = DEFAULT_AAD_APP_ID;
const std::string Configuration::DefaultValue::aadClientSecret =
    DEFAULT_ACCESS_CLIENT_SECRET;
const std::string Configuration::DefaultValue::aadTenant = DEFAULT_AAD_TENANT;

// Logging Configuration Options
const LogLevel::Type Configuration::DefaultValue::logLevel = DEFAULT_LOG_LEVEL;
const std::string Configuration::DefaultValue::logPath = DEFAULT_LOG_PATH;

// mirrored from src/odbc/src/log.cpp
std::shared_ptr< Logger > Logger::logger_;

// mirrored from src/odbc/src/type_traits.cpp
const std::string type_traits::SqlTypeName::VARCHAR("VARCHAR");
#endif

/**
 * Test setup fixture.
 */
struct ConnectionUnitTestSuiteFixture : OdbcUnitTestSuite {
  ConnectionUnitTestSuiteFixture() : OdbcUnitTestSuite() {
    dbc = static_cast<MockConnection*>(env->CreateConnection());

    // MockTimestreamService is singleton
    MockTimestreamService::CreateMockTimestreamService();

    // setup credentials in MockTimestreamService
    MockTimestreamService::GetInstance()->AddCredential("AwsTSUnitTestKeyId",
                                                        "AwsTSUnitTestSecretKey");
  }

  /**
   * Destructor.
   */
  ~ConnectionUnitTestSuiteFixture() {
    // clear the credentials for this test
    MockTimestreamService::GetInstance()->RemoveCredential("AwsTSUnitTestKeyId");

    // destory the singleton to avoid memory leak
    MockTimestreamService::DestoryMockTimestreamService();
  }

  void getLogOptions(Configuration& config) const {
    using ignite::odbc::common::GetEnv;
    using ignite::odbc::LogLevel;

    std::string logPath = GetEnv("TIMESTREAM_LOG_PATH", "");
    std::string logLevelStr = GetEnv("TIMESTREAM_LOG_LEVEL", "2");

    LogLevel::Type logLevel = LogLevel::FromString(logLevelStr, LogLevel::Type::UNKNOWN);
    config.SetLogLevel(logLevel);
    config.SetLogPath(logPath);
  }

  bool IsSuccessful() {
    if (!dbc) {
      return false;
    }
    return dbc->GetDiagnosticRecords().IsSuccessful();
  }

  int GetReturnCode() {
    if (!dbc) {
      return SQL_ERROR;
    }
    return dbc->GetDiagnosticRecords().GetReturnCode();
  }

  std::string GetSqlState() {
    if (!dbc) {
      return "";
    }
    return dbc->GetDiagnosticRecords()
        .GetStatusRecord(dbc->GetDiagnosticRecords().GetLastNonRetrieved())
        .GetSqlState();
  }
};

BOOST_FIXTURE_TEST_SUITE(ConnectionUnitTestSuite, ConnectionUnitTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestEstablishUsingKey) {
  Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");
  getLogOptions(cfg);

  dbc->Establish(cfg);

  BOOST_CHECK(IsSuccessful());
}

BOOST_AUTO_TEST_CASE(TestEstablishAuthTypeNotSpecified) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "08001");
}

BOOST_AUTO_TEST_CASE(TestEstablishUsingKeyNoSecretKey) {
  Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  getLogOptions(cfg);

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "01S00");
}

BOOST_AUTO_TEST_CASE(TestEstablishUsingKeyInvalidLogin) {
  Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("InvalidLogin");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");
  getLogOptions(cfg);

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(),SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "08001");
}

BOOST_AUTO_TEST_CASE(TestEstablishusingKeyInvalidSecretKey) {
  Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("InvalidSecretKey");
  getLogOptions(cfg);

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "08001");
}

BOOST_AUTO_TEST_CASE(TestEstablishReconnect) {
  Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");
  getLogOptions(cfg);

  dbc->Establish(cfg);

  BOOST_CHECK(IsSuccessful());

  dbc->Establish(cfg);
  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "08002");
}

BOOST_AUTO_TEST_CASE(TestRelease) {
  Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");
  getLogOptions(cfg);

  dbc->Establish(cfg);

  BOOST_CHECK(IsSuccessful());

  dbc->Release();

  BOOST_CHECK(IsSuccessful());
  // release again, error is expected
  dbc->Release();
  BOOST_CHECK_EQUAL(GetSqlState(), "08003");
}

BOOST_AUTO_TEST_CASE(TestDeregister) {
  // This will remove dbc from env, any test that 
  // needs env should be put ahead of this testcase
  dbc->Deregister();
  BOOST_CHECK_EQUAL(env->ConnectionsNum(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
