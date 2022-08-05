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

const std::string rootDir = ignite::odbc::common::GetEnv("REPOSITORY_ROOT");

// TestConnectionRestoreInternalSSHTunnel has precondition `*precondition(if_integration())`
BOOST_AUTO_TEST_CASE(TestConnection) {
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString);
  Connect(connectionString);
  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingProfile) {
#if defined(PREDEF_PLATFORM_UNIX_OR_APPLE) || defined(__linux__)
  const std::string profile = rootDir + "/src/tests/input/credentials";
#elif defined(_WIN32)
  const std::string profile = rootDir  + "\\src\\tests\\input\\credentials";
#else
  const std::string profile = "";
#endif
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, ignite::odbc::CredProvClass::Type::PROP_FILE_CRED_PROV,
      profile);
  Connect(connectionString);
  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingNonExistProfile) {
#if defined(PREDEF_PLATFORM_UNIX_OR_APPLE) || defined(__linux__)
  const std::string profile = rootDir + "/src/tests/input/nonexist_credentials";
#elif defined(_WIN32)
  const std::string profile = rootDir + "\\src\\tests\\input\\nonexist_credentials";
#else
  const std::string profile = "";
#endif
  std::string connectionString;
  CreateDsnConnectionStringForAWS(
      connectionString, ignite::odbc::CredProvClass::Type::PROP_FILE_CRED_PROV,
      profile);

  ExpectConnectionReject(connectionString,
                         "08001: Failed to open file " + profile);

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingEmptyProfile) {
#if defined(PREDEF_PLATFORM_UNIX_OR_APPLE) || defined(__linux__)
  const std::string profile =
      rootDir + "/src/tests/input/empty_credentials";
#elif defined(_WIN32)
  const std::string profile =
      rootDir + "\\src\\tests\\input\\empty_credentials";
#else
  const std::string profile = "";
#endif

  std::string connectionString;
  CreateDsnConnectionStringForAWS(
      connectionString, ignite::odbc::CredProvClass::Type::PROP_FILE_CRED_PROV,
      profile);

  ExpectConnectionReject(connectionString,
                         "01S00: Any of the following group is required to connect:\n"
                         "ACCESS_KEY_ID and SECRET_KEY or\n"
                         "AWS_CREDENTIALS_PROVIDER_CLASS or\n"
                         "AWS_CREDENTIALS_PROVIDER_CLASS is "
                         "\"PropertiesFileCredentialsProvider\" and "
                         "CUSTOM_CREDENTIALS_FILE which should contain access "
                         "key Id and secret access key");

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionUsingIncompleteProfile) {
#if defined(PREDEF_PLATFORM_UNIX_OR_APPLE) || defined(__linux__)
  const std::string profile =
      rootDir + "/src/tests/input/incomplete_credentials";
#elif defined(_WIN32)
  const std::string profile =
      rootDir + "\\src\\tests\\input\\incomplete_credentials";
#else
  const std::string profile = "";
#endif

  std::string connectionString;
  CreateDsnConnectionStringForAWS(
      connectionString, ignite::odbc::CredProvClass::Type::PROP_FILE_CRED_PROV,
      profile);

  ExpectConnectionReject(connectionString,
                         "01S00: Any of the following group is required to connect:\n"
                         "ACCESS_KEY_ID and SECRET_KEY or\n"
                         "AWS_CREDENTIALS_PROVIDER_CLASS or\n"
                         "AWS_CREDENTIALS_PROVIDER_CLASS is "
                         "\"PropertiesFileCredentialsProvider\" and "
                         "CUSTOM_CREDENTIALS_FILE which should contain access "
                         "key Id and secret access key");

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionConcurrency) {
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
  const std::string miscOptions =
      "APP_NAME=TestAppName;";
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, "", "",
                                          miscOptions);

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

BOOST_AUTO_TEST_CASE(TestConnectionIncompleteBasicProperties) {
  std::string connectionString =
      "DRIVER={Amazon Timestream};"
      "ACCESS_KEY_ID=key;";

  ExpectConnectionReject(
      connectionString,
      "01S00: Any of the following group is required to connect:\n"
      "ACCESS_KEY_ID and SECRET_KEY or\n"
      "AWS_CREDENTIALS_PROVIDER_CLASS or\n"
      "AWS_CREDENTIALS_PROVIDER_CLASS is \"PropertiesFileCredentialsProvider\" and "
      "CUSTOM_CREDENTIALS_FILE which should contain access key Id and secret access key");

  Disconnect();
}

BOOST_AUTO_TEST_CASE(TestConnectionInvalidUser) {
  std::string connectionString;
  CreateDsnConnectionStringForAWS(connectionString, "", "invaliduser");

  ExpectConnectionReject(connectionString,
                         "08001: Failed to establish connection to "
                         "Timestream.\nINVALID_ENDPOINT: Failed to discover");

  Disconnect();
}

BOOST_AUTO_TEST_SUITE_END()
