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

#include <mock/mock_connection.h>
#include <mock/mock_statement.h>
#include <mock/mock_timestream_query_client.h>
#include <mock/mock_timestream_write_client.h>

namespace ignite {
namespace odbc {

MockConnection::MockConnection(Environment* env) : Connection(env) {
  // No-op
}

MockConnection::~MockConnection() {
  // No-op
}

SqlResult::Type MockConnection::InternalCreateStatement(MockStatement*& statement) {
  statement = new MockStatement(*this);

  if (!statement) {
    AddStatusRecord(SqlState::SHY001_MEMORY_ALLOCATION, "Not enough memory.");

    return SqlResult::AI_ERROR;
  }

  return SqlResult::AI_SUCCESS;
}

std::shared_ptr< Aws::TimestreamQuery::TimestreamQueryClient >
MockConnection::CreateTSQueryClient(
    const Aws::Auth::AWSCredentials& credentials,
    const Aws::Client::ClientConfiguration& clientCfg) {
  return std::static_pointer_cast<
      Aws::TimestreamQuery::TimestreamQueryClient >(
      std::make_shared< ignite::odbc::MockTimestreamQueryClient >(credentials,
                                                                  clientCfg));
}
std::shared_ptr< Aws::TimestreamWrite::TimestreamWriteClient >
MockConnection::CreateTSWriteClient(
    const Aws::Auth::AWSCredentials& credentials,
    const Aws::Client::ClientConfiguration& clientCfg) {
  return std::static_pointer_cast<
      Aws::TimestreamWrite::TimestreamWriteClient >(
      std::make_shared< ignite::odbc::MockTimestreamWriteClient >(credentials,
                                                                  clientCfg));
}

MockStatement* MockConnection::CreateStatement() {
  MockStatement* statement;

  InternalCreateStatement(statement);

  return statement;
}
}  // namespace odbc
}  // namespace ignite
