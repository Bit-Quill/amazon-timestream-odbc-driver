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

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/bind.hpp>

using boost::unit_test::precondition;
using ignite::odbc::OdbcTestSuite;
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
  std::string dsn = "TestConnectionDSN";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString);

  Connect(connectionString);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingProfile) {
  const std::string profile = "test-profile";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(
      connectionString, ignite::odbc::AuthType::Type::AWS_PROFILE, profile);
  Connect(connectionString);
  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingNonExistProfile) {
  const std::string profile = "nonexist-profile";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(
      connectionString, ignite::odbc::AuthType::Type::AWS_PROFILE, profile);

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
  CreateDsnConnectionStringForAWS(
      connectionString, ignite::odbc::AuthType::Type::AWS_PROFILE, profile);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingIncompleteProfile) {
  const std::string profile = "incomplete-profile";

  std::string connectionString;
  CreateDsnConnectionStringForAWS(
      connectionString, ignite::odbc::AuthType::Type::AWS_PROFILE, profile);

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
      "DRIVER={Amazon Timestream ODBC Driver};"
      "AUTH=IAM;"
      "ACCESS_KEY_ID=key;";

  std::string username;
  std::string password;
  WriteDsnConfiguration(dsn, connectionString, username, password);
  ExpectConnectionReject(dsn, username, password, "01S00",
                         "The following is required to connect:\n"
                         "AUTH is \"IAM\" and "
                         "ACCESS_KEY_ID and SECRET_KEY");

  Disconnect();

  DeleteDsnConfiguration(dsn);
}

BOOST_AUTO_TEST_CASE(TestSQLDriverConnectionIncompleteBasicProperties) {
  const std::string dsn = "IncompleteBasicProperties";
  std::string connectionString =
      "DRIVER={Amazon Timestream ODBC Driver};"
      "AUTH=IAM;"
      "ACCESS_KEY_ID=key;";
  ExpectConnectionReject(connectionString, "01S00",
                         "The following is required to connect:\n"
                         "AUTH is \"IAM\" and "
                         "ACCESS_KEY_ID and SECRET_KEY");

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
  const std::string dsn = "InvalidUser";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, "", "invaliduser");

  ExpectConnectionReject(connectionString, "08001",
                         "Failed to establish connection to "
                         "Timestream.\nINVALID_ENDPOINT: Failed to discover");

  Disconnect();
}

BOOST_AUTO_TEST_SUITE_END()
