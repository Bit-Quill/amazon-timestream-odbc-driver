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

#define BOOST_TEST_MODULE TimestreamTest
#ifdef _WIN32
#include <Windows.h>
#endif

#include <sql.h>
#include <sqlext.h>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <string>

#include "odbc_test_suite.h"
#include "test_utils.h"
#include <ignite/odbc/common/platform_utils.h>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/bind.hpp>

using boost::unit_test::precondition;
using ignite::odbc::AuthType;
using ignite::odbc::OdbcTestSuite;
using ignite::odbc::common::GetEnv;
using ignite_test::GetOdbcErrorMessage;
using namespace boost::unit_test;

/**
 * Test setup fixture.
 */
struct ConnectionTestSuiteFixture : OdbcTestSuite {
  using OdbcTestSuite::OdbcTestSuite;

  /**
   * Destructor.
   */
  ~ConnectionTestSuiteFixture() override = default;

  void connect() {
    std::string connectionString;
    CreateDsnConnectionStringForAWS(connectionString);
    Connect(connectionString);
    Disconnect();
  }
};

BOOST_FIXTURE_TEST_SUITE(ConnectionTestSuite, ConnectionTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestSQLConnection) {
  std::string dsn = "TestConnectionDSN";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString);

  std::string username;
  std::string password;
  WriteDsnConfiguration(dsn, connectionString, username, password);
  Connect(dsn, username, password);

  Disconnect();

  DeleteDsnConfiguration(dsn);
}

BOOST_AUTO_TEST_CASE(TestDriverConnection) {
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString);

  Connect(connectionString);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestSQLConnectionUsingDupCredString) {
  // Test passing both uid/pwd and accessKeyId/secretKey in the connection
  // string
  std::string dsn = "TestConnectionDSN";
  std::string connectionString;
  std::string accessKeyId;
  std::string secretKey;
  GetIAMCredentials(accessKeyId, secretKey);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM,
                                   accessKeyId, secretKey, true, accessKeyId,
                                   secretKey);

  std::string username;
  std::string password;
  WriteDsnConfiguration(dsn, connectionString, username, password);
  Connect(dsn, username, password);

  Disconnect();

  DeleteDsnConfiguration(dsn);
}

BOOST_AUTO_TEST_CASE(TestDriverConnectionUsingDupCredString) {
  // Test passing both uid/pwd and accessKeyId/secretKey with correct
  // credentials in the connection string

  std::string connectionString;
  std::string accessKeyId;
  std::string secretKey;
  GetIAMCredentials(accessKeyId, secretKey);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM,
                                   accessKeyId, secretKey, true, accessKeyId,
                                   secretKey);

  Connect(connectionString);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(
    TestDriverConnectionUsingDupCredStringWithWrongIAMCredentials) {
  // Test passing uid/pwd with correct credentials and accessKeyId/secretKey
  // with wrong credentials in the connection string. Since uid/pwd take
  // precendence in making a connection, the connection should succeed

  std::string connectionString;
  std::string accessKeyId = "wrongAccessKeyId";
  std::string secretKey = "wrongSecretKey";
  std::string uid;
  std::string pwd;
  GetIAMCredentials(uid, pwd);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid,
                                   pwd, true, accessKeyId,
                                   secretKey);

  Connect(connectionString);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestDriverConnectionUsingDupCredStringWithEmptyUidPwd) {
  // Test passing uid/pwd with empty credentials and accessKeyId/secretKey
  // with correct credentials in the connection string. Since
  // accessKeyId/secretKey should be used if uid/pwd are empty, the connection
  // should succeed

  std::string connectionString;
  std::string uid = "";
  std::string pwd = "";
  std::string accessKeyId;
  std::string secretKey;
  GetIAMCredentials(accessKeyId, secretKey);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid,
                                   pwd, true, accessKeyId,
                                   secretKey);

  Connect(connectionString);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestDriverConnectionUsingDupCredStringWithWrongUidPwd) {
  // Test passing uid/pwd with wrong credentials and accessKeyId/secretKey
  // with correct credentials in the connection string. Since uid/pwd take
  // precendence in making a connection, the connection should fail

  std::string connectionString;
  std::string uid = "wrongUsername";
  std::string pwd = "wrongPassword";
  std::string accessKeyId;
  std::string secretKey;
  GetIAMCredentials(accessKeyId, secretKey);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid,
                                   pwd, true, accessKeyId, secretKey);

  ExpectConnectionReject(
      connectionString, "08001",
      "Failed to establish connection to Timestream.\nINVALID_ENDPOINT: "
      "Failed to discover endpoint");

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestDriverConnectionUsingDupCredStringWithWrongUid) {
  // Test passing uid with wrong value, pwd with correct value, and
  // accessKeyId/secretKey with correct credentials in the connection string.
  // Since uid/pwd take precendence in making a connection, the connection
  // should fail

  std::string connectionString;
  std::string uid = "wrongUsername";
  std::string pwd = GetEnv("AWS_SECRET_ACCESS_KEY");
  std::string accessKeyId;
  std::string secretKey;
  GetIAMCredentials(accessKeyId, secretKey);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid,
                                   pwd, true, accessKeyId, secretKey);

  ExpectConnectionReject(
      connectionString, "08001",
      "Failed to establish connection to Timestream.\nINVALID_ENDPOINT: "
      "Failed to discover endpoint");

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestDriverConnectionUsingDupCredStringWithWrongPwd) {
  // Test passing uid with correct value, pwd with wrong value, and
  // accessKeyId/secretKey with correct credentials in the connection string.
  // Since uid/pwd take precendence in making a connection, the connection
  // should fail

  std::string connectionString;
  std::string uid = GetEnv("AWS_ACCESS_KEY_ID");
  std::string pwd = "wrongPassword";
  std::string accessKeyId;
  std::string secretKey;
  GetIAMCredentials(accessKeyId, secretKey);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid,
                                   pwd, true, accessKeyId, secretKey);

  ExpectConnectionReject(
      connectionString, "08001",
      "Failed to establish connection to Timestream.\nINVALID_ENDPOINT: "
      "Failed to discover endpoint");

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestSQLConnectionUsingGenericIAMString) {
  // Test passing only uid/pwd in the connection string
  std::string dsn = "TestConnectionDSN";
  std::string connectionString;
  std::string uid;
  std::string pwd;
  GetIAMCredentials(uid, pwd);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid,
                                   pwd);

  std::string username;
  std::string password;
  WriteDsnConfiguration(dsn, connectionString, username, password);
  Connect(dsn, username, password);

  Disconnect();

  DeleteDsnConfiguration(dsn);
}

BOOST_AUTO_TEST_CASE(TestDriverConnectionUsingGenericIAMString) {
  // Test passing only uid/pwd in the connection string
  std::string connectionString;
  std::string uid;
  std::string pwd;
  GetIAMCredentials(uid, pwd);

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid,
                                   pwd);

  Connect(connectionString);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestDriverConnectionWithUIDSecretKey) {
  std::string connectionString;
  std::string accessKeyId = GetEnv("AWS_ACCESS_KEY_ID");
  std::string secretKey = GetEnv("AWS_SECRET_ACCESS_KEY");
  std::string sessionToken = GetEnv("AWS_SESSION_TOKEN");
  std::string region = GetEnv("AWS_REGION", "us-west-2");
  std::string logPath = GetEnv("TIMESTREAM_LOG_PATH");
  std::string logLevel = GetEnv("TIMESTREAM_LOG_LEVEL", "2");

  connectionString =
            "driver={Amazon Timestream ODBC Driver};"
            "dsn={" + Configuration::DefaultValue::dsn + "};"
            "auth=" + AuthType::ToString(AuthType::Type::IAM) + ";"
            "secretKey=" + secretKey + ";"
            "uid=" + accessKeyId + ";"
            "sessionToken=" + sessionToken + ";"
            "region=" + region + ";"
            "logOutput=" + logPath + ";"
            "logLevel=" + logLevel + ";";

  Connect(connectionString);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestDriverConnectionWithAccessKeyIdPWD) {
  std::string connectionString;
  std::string accessKeyId = GetEnv("AWS_ACCESS_KEY_ID");
  std::string secretKey = GetEnv("AWS_SECRET_ACCESS_KEY");
  std::string sessionToken = GetEnv("AWS_SESSION_TOKEN");
  std::string region = GetEnv("AWS_REGION", "us-west-2");
  std::string logPath = GetEnv("TIMESTREAM_LOG_PATH");
  std::string logLevel = GetEnv("TIMESTREAM_LOG_LEVEL", "2");

  connectionString =
            "driver={Amazon Timestream ODBC Driver};"
            "dsn={" + Configuration::DefaultValue::dsn + "};"
            "auth=" + AuthType::ToString(AuthType::Type::IAM) + ";"
            "pwd=" + secretKey + ";"
            "accessKeyId=" + accessKeyId + ";"
            "sessionToken=" + sessionToken + ";"
            "region=" + region + ";"
            "logOutput=" + logPath + ";"
            "logLevel=" + logLevel + ";";

  Connect(connectionString);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingProfile) {
  const std::string profile = "test-profile";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, AuthType::Type::AWS_PROFILE,
                                  profile);
  Connect(connectionString);
  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingNonExistProfile) {
  const std::string profile = "nonexist-profile";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, AuthType::Type::AWS_PROFILE,
                                  profile);

  ExpectConnectionReject(
      connectionString, "08001",
      "Failed to establish connection to Timestream.\nNo credentials in "
      "profile nonexist-profile or they are expired");

  Disconnect();
}

// TODO modify & enable this test as part of AT-1048
// https://bitquill.atlassian.net/browse/AT-1048
BOOST_AUTO_TEST_CASE(TestConnectionUsingEmptyProfile, *disabled()) {
  const std::string profile = "";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, AuthType::Type::AWS_PROFILE,
                                  profile);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingIncompleteProfile) {
  const std::string profile = "incomplete-profile";

  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, AuthType::Type::AWS_PROFILE,
                                  profile);

  ExpectConnectionReject(
      connectionString, "08001",
      "Failed to establish connection to Timestream.\nINVALID_ENDPOINT: "
      "Failed to discover endpoint");

  Disconnect();
}

// Disable this multi-thread test now as it causes s2n_init failure
// on github Linux host. The root cause is unclear, use
// https://bitquill.atlassian.net/browse/AT-1087 for investigation
BOOST_AUTO_TEST_CASE(TestConnectionConcurrency, *disabled()) {
  ConnectionTestSuiteFixture testConn[10];

  // Create threads and add them to the thread group
  boost::thread_group threads;

  for (int i = 0; i < 10; i++) {
    boost::thread* t = new boost::thread(
        boost::bind(&ConnectionTestSuiteFixture::connect, testConn[i]));
    threads.add_thread(t);
  }

  // Wait till the threads are finished
  threads.join_all();
}

// TO-DO enable for misc options in future
// https://bitquill.atlassian.net/browse/AT-1056
BOOST_AUTO_TEST_CASE(TestConnectionRestoreMiscOptionsSet, *disabled()) {
  // TODO add misc options
  const std::string miscOptions = "APP_NAME=TestAppName;";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, "", "", miscOptions);

  Connect(connectionString);
  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionOnlyConnect) {
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString);
  Connect(connectionString);
}

BOOST_AUTO_TEST_CASE(TestConnectionOnlyDisconnect) {
  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestSQLConnectionIncompleteBasicProperties) {
  const std::string dsn = "IncompleteBasicProperties";
  std::string connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "auth=IAM;"
      "accessKeyId=key;";

  std::string username;
  std::string password;
  WriteDsnConfiguration(dsn, connectionString, username, password);
  ExpectConnectionReject(dsn, username, password, "01S00",
                         "The following is required to connect:\n"
                         "AUTH is \"IAM\" and "
                         "UID and PWD or "
                         "AccessKeyId and Secretkey");

  Disconnect();

  DeleteDsnConfiguration(dsn);
}

BOOST_AUTO_TEST_CASE(TestSQLDriverConnectionIncompleteBasicProperties) {
  std::string connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "auth=IAM;"
      "accessKeyId=key;";
  ExpectConnectionReject(connectionString, "01S00",
                         "The following is required to connect:\n"
                         "AUTH is \"IAM\" and "
                         "UID and PWD or "
                         "AccessKeyId and Secretkey");

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestSQLConnectionInvalidUser) {
  const std::string dsn = "InvalidUser";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, "", "invaliduser");

  std::string username;
  std::string password;
  WriteDsnConfiguration(dsn, connectionString, username, password);
  ExpectConnectionReject(dsn, username, password, "08001",
                         "Failed to establish connection to "
                         "Timestream.\nINVALID_ENDPOINT: Failed to discover");

  Disconnect();

  DeleteDsnConfiguration(dsn);
}

BOOST_AUTO_TEST_CASE(TestSQLDriverConnectionInvalidUser) {
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, "", "invaliduser");

  ExpectConnectionReject(connectionString, "08001",
                         "Failed to establish connection to "
                         "Timestream.\nINVALID_ENDPOINT: Failed to discover");

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestSQLConnectionInvalidUserUsingGenericIAMString) {
  // Test passing only uid/pwd in the connection string
  const std::string dsn = "InvalidUser";
  std::string connectionString;
  std::string uid = GetEnv("AWS_ACCESS_KEY_ID");

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid, "invaliduser");

  std::string username;
  std::string password;
  WriteDsnConfiguration(dsn, connectionString, username, password);
  ExpectConnectionReject(dsn, username, password, "08001",
                         "Failed to establish connection to "
                         "Timestream.\nINVALID_ENDPOINT: Failed to discover");

  Disconnect();

  DeleteDsnConfiguration(dsn);
}

BOOST_AUTO_TEST_CASE(TestSQLDriverConnectionInvalidUserUsingGenericIAMString) {
  // Test passing only uid/pwd in the connection string
  std::string connectionString;
  std::string uid = GetEnv("AWS_ACCESS_KEY_ID");

  CreateGenericDsnConnectionString(connectionString, AuthType::Type::IAM, uid,
                                   "invaliduser");

  ExpectConnectionReject(connectionString, "08001",
                         "Failed to establish connection to "
                         "Timestream.\nINVALID_ENDPOINT: Failed to discover");

  Disconnect();
}

BOOST_AUTO_TEST_SUITE_END()
