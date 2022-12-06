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

#ifndef _IGNITE_ODBC_MOCK_CONNECTION
#define _IGNITE_ODBC_MOCK_CONNECTION

#include "ignite/odbc/connection.h"
#include "mock/mock_statement.h"

namespace ignite {
namespace odbc {
/**
 * Mock Connection so function level test could be done against Connection.
 */
class MockConnection : public Connection {
 public:
  /**
   * Constructor.
   */
  MockConnection(Environment* env);

  /**
   * Destructor.
   */
  ~MockConnection();

  MockStatement* CreateStatement();

 private:
  /**
   * Create statement associated with the connection.
   * Internal call.
   *
   * @param statement Pointer to valid instance on success or NULL on failure.
   * @return Operation result.
   */
  virtual SqlResult::Type InternalCreateStatement(MockStatement*& statement);

  /**
   * Create MockTimestreamQueryClient object.
   *
   * @param credentials Aws IAM credentials.
   * @param clientCfg Aws client configuration.
   * @param cfg connection configuration.
   * @return a shared_ptr to created MockTimestreamQueryClient object.
   */
  virtual std::shared_ptr< Aws::TimestreamQuery::TimestreamQueryClient >
  CreateTSQueryClient(const Aws::Auth::AWSCredentials& credentials,
                      const Aws::Client::ClientConfiguration& clientCfg);

  /**
   * Create MockTimestreamWriteClient object.
   *
   * @param credentials Aws IAM credentials.
   * @param clientCfg Aws client configuration.
   * @param cfg connection configuration.
   * @return a shared_ptr to created MockTimestreamWriteClient object.
   */
  virtual std::shared_ptr< Aws::TimestreamWrite::TimestreamWriteClient >
  CreateTSWriteClient(const Aws::Auth::AWSCredentials& credentials,
                      const Aws::Client::ClientConfiguration& clientCfg);
};
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_MOCK_CONNECTION
