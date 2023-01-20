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

#ifndef _IGNITE_ODBC_MOCK_STSCLIENT
#define _IGNITE_ODBC_MOCK_STSCLIENT

#include "aws/sts/STSClient.h"

using namespace Aws::STS;

namespace ignite {
namespace odbc {
/**
 * Mock Connection so function level test could be done against Connection.
 */
class MockSTSClient : public STSClient {
 public:
  /**
   * Constructor.
   */
  MockSTSClient() = default;

  /**
   * Destructor.
   */
  ~MockSTSClient() = default;

  /**
   * Handle AssumeRoleWithSAMLRequest to generate a mocked outcome with credentials
   *
   * @param request The AssumeRoleWithSAMLRequest.
   * @return Mocked outcome.
   */
  virtual Model::AssumeRoleWithSAMLOutcome AssumeRoleWithSAML(
      const Model::AssumeRoleWithSAMLRequest &request) const;
};
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_MOCK_STSCLIENT
