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

#include <mock/mock_environment.h>
#include <mock/mock_connection.h>

namespace ignite {
namespace odbc {
MockEnvironment::MockEnvironment()
    : Environment() {
  // No-op.
}

MockEnvironment::~MockEnvironment() {
  // No-op.
}

SqlResult::Type MockEnvironment::InternalCreateConnection(Connection*& connection) {
  connection = new MockConnection(this);

  if (!connection) {
    AddStatusRecord(SqlState::SHY001_MEMORY_ALLOCATION, "Not enough memory.");

    return SqlResult::AI_ERROR;
  }

  connections.insert(connection);
  return SqlResult::AI_SUCCESS;
}
}  // namespace odbc
}  // namespace ignite
