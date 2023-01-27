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
#include <ignite/odbc/authentication/auth_type.h>
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
const std::string profileName = "test-profile";

BOOST_FIXTURE_TEST_SUITE(ConnectionStringParserTestSuite,
                         ConnectionStringParserUnitTestSuiteFixture)

BOOST_AUTO_TEST_CASE(TestParsingCredentials) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
            "driver={Amazon Timestream ODBC Driver};"
            "auth=" + AuthType::ToString(testAuthType) + ";"
            "profileName=" + profileName + ";";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

BOOST_AUTO_TEST_CASE(TestParsingNoProvider) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "profileName="
      + profileName + ";";

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
      "driver={Amazon Timestream ODBC Driver};"
      "auth="
      + AuthType::ToString(testAuthType) + ";";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK_GE(diag.GetStatusRecordsNumber(), 1);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetSqlState(), "08001");
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetMessageText(),
                    "Credentials file is empty");
}

BOOST_AUTO_TEST_CASE(TestParsingEmptyCredentials) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
            "driver={Amazon Timestream ODBC Driver};"
            "auth=" + AuthType::ToString(testAuthType) + ";"
            "profileName=" + profileName + ";";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  if (diag.GetStatusRecordsNumber() != 0)
    BOOST_FAIL(diag.GetStatusRecord(1).GetMessageText());
}

BOOST_AUTO_TEST_CASE(TestParsingRequestTimeout) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "RequestTimeout=0;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK(diag.GetStatusRecordsNumber() == 1);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetMessageText(),
                    "Request Timeout attribute value is out of range. Using "
                    "default value. [key='RequestTimeout', value='0']");

  connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "RequestTimeout=-1000;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK(diag.GetStatusRecordsNumber() == 2);
  BOOST_CHECK_EQUAL(
      diag.GetStatusRecord(2).GetMessageText(),
      "Request Timeout attribute value contains unexpected characters. Using "
      "default value. [key='RequestTimeout', value='-1000']");
}

BOOST_AUTO_TEST_CASE(TestParsingConnectionTimeout) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "ConnectionTimeout=0;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK(diag.GetStatusRecordsNumber() == 1);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetMessageText(),
                    "Connection Timeout attribute value is out of range. Using "
                    "default value. [key='ConnectionTimeout', value='0']");

  connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "ConnectionTimeout=-1000;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK(diag.GetStatusRecordsNumber() == 2);
  BOOST_CHECK_EQUAL(
      diag.GetStatusRecord(2).GetMessageText(),
      "Connection Timeout attribute value contains unexpected characters. "
      "Using default value. [key='ConnectionTimeout', value='-1000']");
}

BOOST_AUTO_TEST_CASE(TestParsingMaxRetryCountClient) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "MaxRetryCountClient=-1000;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK(diag.GetStatusRecordsNumber() == 1);
  BOOST_CHECK_EQUAL(
      diag.GetStatusRecord(1).GetMessageText(),
      "Max Retry Count Client attribute value contains unexpected characters. "
      "Using default value. [key='MaxRetryCountClient', value='-1000']");
}

BOOST_AUTO_TEST_CASE(TestParsingMaxConnections) {
  ignite::odbc::config::Configuration cfg;

  ConnectionStringParser parser(cfg);

  diagnostic::DiagnosticRecordStorage diag;

  std::string connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "MaxConnections=0;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK(diag.GetStatusRecordsNumber() == 1);
  BOOST_CHECK_EQUAL(diag.GetStatusRecord(1).GetMessageText(),
                    "Max Connections attribute value is out of range. Using "
                    "default value. [key='MaxConnections', value='0']");

  connectionString =
      "driver={Amazon Timestream ODBC Driver};"
      "MaxConnections=-1000;";

  BOOST_CHECK_NO_THROW(parser.ParseConnectionString(connectionString, &diag));

  BOOST_CHECK(diag.GetStatusRecordsNumber() == 2);
  BOOST_CHECK_EQUAL(
      diag.GetStatusRecord(2).GetMessageText(),
      "Max Connections attribute value contains unexpected characters. Using "
      "default value. [key='MaxConnections', value='-1000']");
}

BOOST_AUTO_TEST_SUITE_END()
