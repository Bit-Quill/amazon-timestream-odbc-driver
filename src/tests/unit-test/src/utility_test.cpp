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
 *
 * Modifications Copyright Amazon.com, Inc. or its affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <timestream/odbc/utils.h>
#include <timestream/odbc/utility.h>

#include <boost/test/unit_test.hpp>
#include <chrono>
#include <stdio.h>

using namespace timestream::odbc;
using namespace timestream::odbc::utility;
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(UtilityTestSuite)

BOOST_AUTO_TEST_CASE(TestUtilityRemoveSurroundingSpaces) {
  std::string inStr("   \r \n    \t  some meaningfull data   \n\n   \t  \r  ");
  std::string expectedOutStr("some meaningfull data");

  std::string realOutStr = utility::Trim(inStr);

  BOOST_REQUIRE(expectedOutStr == realOutStr);
}

BOOST_AUTO_TEST_CASE(TestUtilityCopyStringToBuffer) {
  SQLWCHAR buffer[1024];
  std::wstring wstr(L"你好 - Some data. And some more data here.");
  std::string str = ToUtf8(wstr);
  size_t bytesWrittenOrRequired = 0;
  bool isTruncated = false;

  // With length in character mode
  buffer[0] = 0;
  bytesWrittenOrRequired = CopyStringToBuffer(
      str, buffer, sizeof(buffer) / sizeof(SQLWCHAR), isTruncated);
  BOOST_REQUIRE_EQUAL(SqlWcharToString(buffer), str);
  BOOST_CHECK_EQUAL(wstr.size(), bytesWrittenOrRequired);

  // With length in byte mode
  buffer[0] = 0;
  bytesWrittenOrRequired =
      CopyStringToBuffer(str, buffer, sizeof(buffer), isTruncated, true);
  BOOST_REQUIRE_EQUAL(SqlWcharToString(buffer), str);
  BOOST_CHECK_EQUAL(wstr.size() * sizeof(SQLWCHAR), bytesWrittenOrRequired);

  // 10 characters plus 1 for null char.
  buffer[0] = 0;
  bytesWrittenOrRequired =
      CopyStringToBuffer(str, buffer, 11, isTruncated, false);
  BOOST_REQUIRE_EQUAL(SqlWcharToString(buffer), ToUtf8(wstr.substr(0, 10)));
  BOOST_CHECK_EQUAL(10, bytesWrittenOrRequired);

  // 10 characters plus 1 for null char, in bytes
  buffer[0] = 0;
  bytesWrittenOrRequired = CopyStringToBuffer(
      str, buffer, ((10 + 1) * sizeof(SQLWCHAR)), isTruncated, true);
  BOOST_REQUIRE_EQUAL(SqlWcharToString(buffer), ToUtf8(wstr.substr(0, 10)));
  BOOST_CHECK_EQUAL(10 * sizeof(SQLWCHAR), bytesWrittenOrRequired);

  // Zero length buffer in character mode.
  buffer[0] = 0;
  bytesWrittenOrRequired = CopyStringToBuffer(str, buffer, 0, isTruncated);
  BOOST_REQUIRE_EQUAL(SqlWcharToString(buffer), std::string());
  BOOST_CHECK_EQUAL(0, bytesWrittenOrRequired);

  // Zero length buffer in byte mode.
  buffer[0] = 0;
  bytesWrittenOrRequired =
      CopyStringToBuffer(str, buffer, 0, isTruncated, true);
  BOOST_REQUIRE_EQUAL(SqlWcharToString(buffer), std::string());
  BOOST_CHECK_EQUAL(0, bytesWrittenOrRequired);

  // nullptr buffer, zero length, in character mode.
  buffer[0] = 0;
  bytesWrittenOrRequired = CopyStringToBuffer(str, nullptr, 0, isTruncated);
  BOOST_CHECK_EQUAL(wstr.size(), bytesWrittenOrRequired);

  // nullptr buffer, zero length, in byte mode.
  buffer[0] = 0;
  bytesWrittenOrRequired =
      CopyStringToBuffer(str, nullptr, 0, isTruncated, true);
  BOOST_CHECK_EQUAL(wstr.size() * sizeof(SQLWCHAR), bytesWrittenOrRequired);

  // nullptr buffer, non-zero length, in character mode.
  buffer[0] = 0;
  bytesWrittenOrRequired = CopyStringToBuffer(
      str, nullptr, sizeof(buffer) / sizeof(SQLWCHAR), isTruncated);
  BOOST_CHECK_EQUAL(wstr.size(), bytesWrittenOrRequired);

  // nullptr buffer, non-zero length, in byte mode.
  buffer[0] = 0;
  bytesWrittenOrRequired =
      CopyStringToBuffer(str, nullptr, sizeof(buffer), isTruncated, true);
  BOOST_CHECK_EQUAL(wstr.size() * sizeof(SQLWCHAR), bytesWrittenOrRequired);
}

// Enable test to determine efficiency of conversion function.
BOOST_AUTO_TEST_CASE(TestUtilityCopyStringToBufferRepetative, *disabled()) {
  char cch;
  int strLen = 1024 * 1024;
  std::string str;
  std::vector< SQLWCHAR > buffer(strLen + 1);

  for (int i = 0; i < strLen; i++) {
    cch = 'a' + rand() % 26;
    str.push_back(cch);
  }

  auto t1 = std::chrono::high_resolution_clock::now();
  size_t bytesWrittenOrRequired = 0;
  bool isTruncated = false;
  for (int i = 0; i < 500; i++) {
    bytesWrittenOrRequired = CopyStringToBuffer(
        str, buffer.data(), ((strLen + 1) * sizeof(SQLWCHAR)), isTruncated);
    BOOST_CHECK_EQUAL(str.size(), bytesWrittenOrRequired);
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  std::cout << t2.time_since_epoch().count() - t1.time_since_epoch().count()
            << " nanoseconds\n";
}

BOOST_AUTO_TEST_CASE(TestUtilitySqlStringToString) {
  std::string utf8String = u8"你好 - Some data. And some more data here.";
  std::vector< SQLWCHAR > buffer = ToWCHARVector(utf8String);
  std::string utf8StringShortened = u8"你好 - Some da";

  std::string result = SqlWcharToString(buffer.data());
  BOOST_CHECK_EQUAL(utf8String, result);

  result = SqlWcharToString(buffer.data(), buffer.size());
  BOOST_CHECK_EQUAL(utf8String, result);

  result = SqlWcharToString(buffer.data(), buffer.size(), false);
  BOOST_CHECK_EQUAL(utf8String, result);

  result =
      SqlWcharToString(buffer.data(), buffer.size() * sizeof(SQLWCHAR), true);
  BOOST_CHECK_EQUAL(utf8String, result);

  result = SqlWcharToString(nullptr, buffer.size());
  BOOST_CHECK_EQUAL(std::string(), result);

  result = SqlWcharToString(nullptr, buffer.size() * sizeof(SQLWCHAR), true);
  BOOST_CHECK_EQUAL(std::string(), result);

  result = SqlWcharToString(buffer.data(), 0);
  BOOST_CHECK_EQUAL(std::string(), result);

  result = SqlWcharToString(buffer.data(), 0, true);
  BOOST_CHECK_EQUAL(std::string(), result);

  result = SqlWcharToString(buffer.data(), 12);
  BOOST_CHECK_EQUAL(utf8StringShortened, result);

  result = SqlWcharToString(buffer.data(), 12 * sizeof(SQLWCHAR), true);
  BOOST_CHECK_EQUAL(utf8StringShortened, result);
}

BOOST_AUTO_TEST_CASE(TestCopyWcharStringToSqlWcharString) {
    // Normal Copy
    {
        const wchar_t* inBuffer = L"Hello, World!";
        SQLWCHAR outBuffer[50] = {0};
        bool isTruncated = false;
        size_t bytesWritten = CopyWcharStringToSqlWcharString(inBuffer, outBuffer, sizeof(outBuffer), isTruncated);
        BOOST_CHECK(!isTruncated);
        BOOST_CHECK_EQUAL(bytesWritten, wcslen(inBuffer) * sizeof(SQLWCHAR));
        BOOST_CHECK_EQUAL(std::wstring(reinterpret_cast<wchar_t*>(outBuffer)), std::wstring(inBuffer));
    }

    // Exact Fit
    {
        const wchar_t* inBuffer = L"ExactFit";
        size_t inLength = wcslen(inBuffer);
        size_t outBufferLenBytes = (inLength + 1) * sizeof(SQLWCHAR); // Exact fit including null terminator
        SQLWCHAR outBuffer[9] = {0}; // 8 characters + null terminator
        bool isTruncated = false;
        size_t bytesWritten = CopyWcharStringToSqlWcharString(inBuffer, outBuffer, outBufferLenBytes, isTruncated);
        BOOST_CHECK(!isTruncated);
        BOOST_CHECK_EQUAL(bytesWritten, inLength * sizeof(SQLWCHAR));
        BOOST_CHECK_EQUAL(std::wstring(reinterpret_cast<wchar_t*>(outBuffer)), std::wstring(inBuffer));
    }

    // Truncation
    {
        const wchar_t* inBuffer = L"TruncatedString";
        size_t maxChars = 5; // Intentionally small to force truncation
        size_t outBufferLenBytes = (maxChars + 1) * sizeof(SQLWCHAR); // 5 characters + null terminator
        SQLWCHAR outBuffer[6] = {0};
        bool isTruncated = false;
        size_t bytesWritten = CopyWcharStringToSqlWcharString(inBuffer, outBuffer, outBufferLenBytes, isTruncated);
        BOOST_CHECK(isTruncated);
        BOOST_CHECK_EQUAL(bytesWritten, maxChars * sizeof(SQLWCHAR));
        std::wstring expected(inBuffer, maxChars);
        BOOST_CHECK_EQUAL(std::wstring(reinterpret_cast<wchar_t*>(outBuffer)), expected);
    }

    // Null Input Buffer
    {
        SQLWCHAR outBuffer[10] = {0};
        bool isTruncated = false;
        size_t bytesWritten = CopyWcharStringToSqlWcharString(nullptr, outBuffer, sizeof(outBuffer), isTruncated);
        BOOST_CHECK_EQUAL(bytesWritten, 0);
        // Here we check that the first element is untouched (still 0)
        BOOST_CHECK_EQUAL(outBuffer[0], static_cast<SQLWCHAR>(0));
    }

    // Empty Input String
    {
        const wchar_t* inBuffer = L"";
        SQLWCHAR outBuffer[10] = {0};
        bool isTruncated = false;
        size_t bytesWritten = CopyWcharStringToSqlWcharString(inBuffer, outBuffer, sizeof(outBuffer), isTruncated);
        BOOST_CHECK(!isTruncated);
        BOOST_CHECK_EQUAL(bytesWritten, 0);
        BOOST_CHECK_EQUAL(outBuffer[0], static_cast<SQLWCHAR>(0));
    }
}


BOOST_AUTO_TEST_SUITE_END()

