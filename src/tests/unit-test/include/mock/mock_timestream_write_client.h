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

#ifndef _MOCK_TIMESTREAM_WRITE_CLIENT
#define _MOCK_TIMESTREAM_WRITE_CLIENT

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/timestream-write/TimestreamWriteClient.h>

namespace ignite {
namespace odbc {
/**
 * Mock TimestreamWriteClient so its behavior could be controlled by us.
 * All interfaces should be kept same as TimestreamWriteClient.
 */
class MockTimestreamWriteClient
    : public Aws::TimestreamWrite::TimestreamWriteClient {
 public:
  /**
   * Constructor.
   */
  MockTimestreamWriteClient(
      const Aws::Auth::AWSCredentials &credentials,
      const Aws::Client::ClientConfiguration &clientConfiguration = Aws::Client::ClientConfiguration())
      : Aws::TimestreamWrite::TimestreamWriteClient(credentials, clientConfiguration), 
        credentials_(credentials),
        clientConfiguration_(clientConfiguration) {
  }

  /**
   * Destructor.
   */
  ~MockTimestreamWriteClient() {
  }

  /**
   * Run a list databases request.
   * 
   * @param request Aws ListDatabasesRequest .
   * @return Operation ListDatabasesOutcome.
   */
  virtual Aws::TimestreamWrite::Model::ListDatabasesOutcome ListDatabases(
      const Aws::TimestreamWrite::Model::ListDatabasesRequest &request) const;

 private:
  Aws::Auth::AWSCredentials credentials_;
  Aws::Client::ClientConfiguration clientConfiguration_;
};
}  // namespace odbc
}  // namespace ignite

#endif //_MOCK_TIMESTREAM_WRITE_CLIENT
