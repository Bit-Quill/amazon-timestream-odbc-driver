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
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>

#include "timestream/timestream.h"
#include "odbc_test_suite.h"
#include "test_type.h"
#include "test_utils.h"

using namespace timestream;
using namespace timestream_test;

using namespace boost::unit_test;

/**
 * Test setup fixture.
 */
struct TypesTestSuiteFixture : odbc::OdbcTestSuite {
  /**
   * Constructor.
   */
  TypesTestSuiteFixture() : cache1(0) {
    node = StartPlatformNode("queries-test.xml", "NodeMain");

    cache1 = node.GetCache< int64_t, TestType >("cache");
  }

  /**
   * Destructor.
   */
  virtual ~TypesTestSuiteFixture() {
    // No-op.
  }

  /** Node started during the test. */
  Ignite node;

  /** Cache instance. */
  cache::Cache< int64_t, TestType > cache1;
};

BOOST_FIXTURE_TEST_SUITE(TypesTestSuite, TypesTestSuiteFixture)

BOOST_AUTO_TEST_SUITE_END()
