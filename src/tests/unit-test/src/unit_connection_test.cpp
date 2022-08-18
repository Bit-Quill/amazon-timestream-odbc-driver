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
#include <ignite/odbc/auth_type.h>
#include "mock/mock_timestream_service.h"

using ignite::odbc::AuthType;
using ignite::odbc::OdbcUnitTestSuite;
using ignite::odbc::MockConnection;
using ignite::odbc::MockTimestreamService;

using namespace boost::unit_test;

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
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");

  dbc->Establish(cfg);

  BOOST_CHECK(IsSuccessful());
}

BOOST_AUTO_TEST_CASE(TestEstablishAuthTypeNotSpecified) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "01S00");
}

BOOST_AUTO_TEST_CASE(TestEstablishUsingKeyNoSecretKey) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "01S00");
}

BOOST_AUTO_TEST_CASE(TestEstablishUsingKeyInvalidLogin) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("InvalidLogin");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(),SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "08001");
}

BOOST_AUTO_TEST_CASE(TestEstablishusingKeyInvalidSecretKey) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("InvalidSecretKey");

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "08001");
}

BOOST_AUTO_TEST_CASE(TestEstablishUsingProfile) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(ignite::odbc::AuthType::Type::AWS_PROFILE);
  cfg.SetAccessKeyIdFromProfile("AwsTSUnitTestKeyId");
  cfg.SetSecretKeyFromProfile("AwsTSUnitTestSecretKey");

  dbc->Establish(cfg);

  BOOST_CHECK(IsSuccessful());
}

BOOST_AUTO_TEST_CASE(TestEstablishUsingProfileNoSecretKey) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(ignite::odbc::AuthType::Type::AWS_PROFILE);
  cfg.SetAccessKeyIdFromProfile("AwsTSUnitTestKeyId");

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "01S00");
}

BOOST_AUTO_TEST_CASE(TestEstablishUsingProfileInvalidLogin) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(ignite::odbc::AuthType::Type::AWS_PROFILE);
  cfg.SetAccessKeyIdFromProfile("InvalidLogin");
  cfg.SetSecretKeyFromProfile("AwsTSUnitTestSecretKey");

  dbc->Establish(cfg);

  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "08001");
}

BOOST_AUTO_TEST_CASE(TestEstablishReconnect) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");

  dbc->Establish(cfg);

  BOOST_CHECK(IsSuccessful());

  dbc->Establish(cfg);
  BOOST_CHECK_EQUAL(GetReturnCode(), SQL_ERROR);
  BOOST_CHECK_EQUAL(GetSqlState(), "08002");
}

BOOST_AUTO_TEST_CASE(TestRelease) {
  ignite::odbc::config::Configuration cfg;
  cfg.SetAuthType(AuthType::Type::IAM);
  cfg.SetAccessKeyId("AwsTSUnitTestKeyId");
  cfg.SetSecretKey("AwsTSUnitTestSecretKey");

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
