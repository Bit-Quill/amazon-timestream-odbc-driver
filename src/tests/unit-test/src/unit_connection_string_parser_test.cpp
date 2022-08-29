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

#include <string>

#include <odbc_unit_test_suite.h>
#include <ignite/odbc/auth_type.h>
#include <ignite/odbc/config/connection_string_parser.h>

using namespace ignite::odbc;
using namespace ignite::odbc::config;
using ignite::odbc::OdbcUnitTestSuite;
using namespace boost::unit_test;

/**
 * Test setup fixture.
 */
struct ConnectionStringParserUnitTestSuiteFixture : OdbcUnitTestSuite {
  ConnectionStringParserUnitTestSuiteFixture() : OdbcUnitTestSuite() {
  }

  /**
   * Destructor.
   */
  ~ConnectionStringParserUnitTestSuiteFixture() {
  }
};

const AuthType::Type testAuthType = AuthType::FromString("Aws_Profile");

const std::string rootDir = common::GetEnv("REPOSITORY_ROOT");
#if (defined(__APPLE__) && defined(__MACH__)) || defined(__linux__)
const std::string credentials = rootDir + "/src/tests/input/test_credentials";
const std::string emptyCredentials =
    rootDir + "/src/tests/input/empty_credentials";
const std::string incompleteCredentials =
    rootDir + "/src/tests/input/incomplete_credentials";
const std::string corruptCredentials =
    rootDir + "/src/tests/input/corrupt_credentials";
#elif defined(_WIN32)
const std::string credentials =
    rootDir + "\\src\\tests\\input\\test_credentials";
const std::string emptyCredentials =
    rootDir + "\\src\\tests\\input\\empty_credentials";
const std::string incompleteCredentials =
    rootDir + "\\src\\tests\\input\\incomplete_credentials";
const std::string corruptCredentials =
    rootDir + "\\src\\tests\\input\\corrupt_credentials";
#endif

BOOST_FIXTURE_TEST_SUITE(ConnectionStringParserTestSuite,
                         ConnectionStringParserUnitTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestParsingCredentials) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
            "DRIVER={Amazon Timestream ODBC Driver};"
            "AUTH=" + AuthType::ToString(testAuthType) + ";"
            "CUSTOM_CREDENTIALS_FILE=" + credentials + ";";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

BOOST_AUTO_TEST_CASE(TestParsingNoProvider) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
      "DRIVER={Amazon Timestream ODBC Driver};"
      "CUSTOM_CREDENTIALS_FILE="
      + credentials + ";";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

// TODO modify & enable this test as part of AT-1048
// https://bitquill.atlassian.net/browse/AT-1048
BOOST_AUTO_TEST_CASE(TestParsingNoCredentials, *disabled()) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
      "DRIVER={Amazon Timestream ODBC Driver};"
      "AUTH="
      + AuthType::ToString(testAuthType) + ";";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK_GE(diag.GetStatusRecordsNumber(), 1);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetSqlState(), "08001");
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetMessageText(),
                    "Credentials file is empty");
}

BOOST_AUTO_TEST_CASE(TestParsingNonExistCredentials) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
            "DRIVER={Amazon Timestream ODBC Driver};"
            "AUTH=" + AuthType::ToString(testAuthType) + ";"
            "CUSTOM_CREDENTIALS_FILE=/path/to/nonexist_credentials;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK_GE(diag.GetStatusRecordsNumber(), 1);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetSqlState(), "08001");
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetMessageText(),
                    "Failed to open file /path/to/nonexist_credentials");
}

BOOST_AUTO_TEST_CASE(TestParsingEmptyCredentials) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
            "DRIVER={Amazon Timestream ODBC Driver};"
            "AUTH=" + AuthType::ToString(testAuthType) + ";"
            "CUSTOM_CREDENTIALS_FILE=" + credentials + ";";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

// disabled as it will be modified by AT-1079
// https://bitquill.atlassian.net/browse/AT-1079
BOOST_AUTO_TEST_CASE(TestParsingIncompleteCredentials, *disabled()) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
            "DRIVER={Amazon Timestream ODBC Driver};"
            "AUTH=" + AuthType::ToString(testAuthType) + ";"
            "CUSTOM_CREDENTIALS_FILE=" + incompleteCredentials + ";";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

BOOST_AUTO_TEST_CASE(TestParsingCorruptCredentials1) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
            "DRIVER={Amazon Timestream ODBC Driver};"
            "AUTH=" + AuthType::ToString(testAuthType) + ";"
            "CUSTOM_CREDENTIALS_FILE=" + corruptCredentials + "1;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

BOOST_AUTO_TEST_CASE(TestParsingCorruptCredentials2) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
            "DRIVER={Amazon Timestream ODBC Driver};"
            "AUTH=" + AuthType::ToString(testAuthType) + ";"
            "CUSTOM_CREDENTIALS_FILE=" + corruptCredentials + "2;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK_GE(diag.GetStatusRecordsNumber(), 1);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetSqlState(), "01S02");
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetMessageText(),
                    "Unknown attribute: 'access_key'. Ignoring.");
}

BOOST_AUTO_TEST_CASE(TestParsingInternalKeys) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
      "DRIVER={Amazon Timestream ODBC Driver};"
      "aws_access_key_id=testAccessKey;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK_GE(diag.GetStatusRecordsNumber(), 1);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetSqlState(), "01S02");
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetMessageText(),
                    "Unknown attribute: 'aws_access_key_id'. Ignoring.");

  connectionString =
      "DRIVER={Amazon Timestream ODBC Driver};"
      "aws_secret_access_key=testSecretKey;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK_GE(diag.GetStatusRecordsNumber(), 2);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(2).GetSqlState(), "01S02");
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(2).GetMessageText(),
                    "Unknown attribute: 'aws_secret_access_key'. Ignoring.");
}
BOOST_AUTO_TEST_SUITE_END()
