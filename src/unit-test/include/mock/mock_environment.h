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

#ifndef _IGNITE_ODBC_MOCK_ENVIRONMENT
#define _IGNITE_ODBC_MOCK_ENVIRONMENT

#include "ignite/odbc/environment.h"

namespace ignite {
namespace odbc {
/**
 * Mock Environment so function level test could be done against Environment.
 */
class MockEnvironment : public Environment {
 public:
  /**
   * Constructor.
   */
  MockEnvironment();

  /**
   * Destructor.
   */
  ~MockEnvironment();

  /**
   * Get the number of connections created.
   */
  size_t ConnectionsNum() {
    return connections.size();
  }

 private:
  /**
   * Create connection associated with the environment.
   * Internal call.
   *
   * @return Pointer to valid instance on success or NULL on failure.
   * @return Operation result.
   */
  virtual SqlResult::Type InternalCreateConnection(Connection*& connection);
};
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_MOCK_ENVIRONMENT
