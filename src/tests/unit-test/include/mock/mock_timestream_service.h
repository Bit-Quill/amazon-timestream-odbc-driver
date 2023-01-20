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

#ifndef _MOCK_TIMESTREAM_SERVICE
#define _MOCK_TIMESTREAM_SERVICE

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/timestream-query/TimestreamQueryClient.h>
#include <aws/timestream-query/model/QueryRequest.h>
#include <aws/timestream-write/TimestreamWriteClient.h>
#include <aws/timestream-write/model/ListDatabasesRequest.h>

namespace ignite {
namespace odbc {
/**
 * Mock Timestream service for unit test
 */
class MockTimestreamService {
 public:
  /**
   * Create the singleton object.
   */
  static void CreateMockTimestreamService();

  /**
   * Destory the singleton object.
   */
  static void DestoryMockTimestreamService();
  
  /**
   * Get the singleton object pointer.
   */
  static MockTimestreamService* GetInstance() {
    return instance_;
  }

  /**
   * Destructor.
   */
  ~MockTimestreamService();

  /**
   * Add credentials configured by user before testcase starts
   *
   * @param keyId Access key Id
   * @param secretKey Secret access key 
   */
  void AddCredential(const Aws::String& keyId, const Aws::String& secretKey);

  /**
   * Remove credentials configured by user
   *
   * @param keyId Access key Id
   */
  void RemoveCredential(const Aws::String& keyId);

  /**
   * Get credential map
   *
   * @return The credential map
   */
  std::map< Aws::String, Aws::String > GetCredentialMap() {
    return credMap_;
  }

  /**
   * Verify credentials provided by user
   *
   * @param keyId Access key Id
   * @param secretKey Secret access key
   */
  bool Authenticate(const Aws::String& keyId, const Aws::String& secretKey);

  /**
   * Handle query request from query client
   *
   * @param request Query request
   */
  Aws::TimestreamQuery::Model::QueryOutcome HandleQueryReq(
      const Aws::TimestreamQuery::Model::QueryRequest &request);

  /**
   * Handle list databases request from write client
   *
   * @param request ListDatabases request
   */
  Aws::TimestreamWrite::Model::ListDatabasesOutcome HandleDatabasesReq(
      const Aws::TimestreamWrite::Model::ListDatabasesRequest& request) const;

 private:
  /**
   * Constructor.
   */
  MockTimestreamService() {
  }

  void SetupResultForMockTable(Aws::TimestreamQuery::Model::QueryResult& result);

  static std::mutex mutex_;
  static MockTimestreamService* instance_;
  std::map< Aws::String, Aws::String > credMap_;  // credentials configured by user
  static int token;
  static int errorToken;
};
}  // namespace odbc
}  // namespace ignite

#endif
