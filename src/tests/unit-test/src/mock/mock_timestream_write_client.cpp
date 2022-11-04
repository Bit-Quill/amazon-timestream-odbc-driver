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

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <mock/mock_timestream_write_client.h>
#include <mock/mock_timestream_service.h>

namespace ignite {
namespace odbc {
// All the working logic is done by the singleton MockTimestreamService object.
Aws::TimestreamWrite::Model::ListDatabasesOutcome
MockTimestreamWriteClient::ListDatabases(
    const Aws::TimestreamWrite::Model::ListDatabasesRequest &request) const {
  // authenticate first
  if (!MockTimestreamService::GetInstance()->Authenticate(
          credentials_.GetAWSAccessKeyId(), credentials_.GetAWSSecretKey())) {
    Aws::TimestreamWrite::TimestreamWriteError error(
        Aws::Client::AWSError< Aws::Client::CoreErrors >(
            Aws::Client::CoreErrors::INVALID_ACCESS_KEY_ID, false));

    return Aws::TimestreamWrite::Model::ListDatabasesOutcome(error);
  }

  return MockTimestreamService::GetInstance()->HandleDatabasesReq(request);
}

}  // namespace odbc
}  // namespace ignite
