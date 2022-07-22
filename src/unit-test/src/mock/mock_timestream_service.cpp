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
#include <aws/timestream-query/model/QueryResult.h>
#include <aws/timestream-query/TimestreamQueryErrors.h>
#include <aws/core/utils/Outcome.h>
#include <aws/timestream-query/model/Row.h>
#include <aws/timestream-query/model/Datum.h>

#include <mock/mock_timestream_service.h>

namespace ignite {
namespace odbc {

std::mutex MockTimestreamService::mutex_;
MockTimestreamService* MockTimestreamService::instance_ = nullptr;

void MockTimestreamService::CreateMockTimestreamService() {
  if (!instance_) {
    std::lock_guard< std::mutex > lock(mutex_);
    if (!instance_) {
      instance_ = new MockTimestreamService; 
    }
  }
}

void MockTimestreamService::DestoryMockTimestreamService() {
  if (instance_) {
    std::lock_guard< std::mutex > lock(mutex_);
    if (instance_) {
      delete instance_;
      instance_ = nullptr;
    }
  }
}

MockTimestreamService ::~MockTimestreamService() {
  // No-op
}

void MockTimestreamService::AddCredential(const Aws::String& keyId,
                                    const Aws::String& secretKey) {
  credMap_[keyId] = secretKey;
}

void MockTimestreamService::RemoveCredential(const Aws::String& keyId) {
  credMap_.erase(keyId);
}


bool MockTimestreamService::Authenticate(const Aws::String& keyId,
                                         const Aws::String& secretKey) {
  auto itr = credMap_.find(keyId);
  if (itr == credMap_.end() || itr->second != secretKey) {
    return false;
  }
  return true;
}

// This function simulates AWS Timestream service. It provides
// simple result without the need of parsing the query. Update
// this function if new query needs to be handled.
Aws::TimestreamQuery::Model::QueryOutcome MockTimestreamService::HandleQueryReq(
    const Aws::TimestreamQuery::Model::QueryRequest& request) const {
  if (request.GetQueryString() == "SELECT 1") {
    // set up QueryResult
    Aws::TimestreamQuery::Model::QueryResult result;
    Aws::TimestreamQuery::Model::Datum datum;
    datum.SetScalarValue("1");

    Aws::TimestreamQuery::Model::Row row;
    row.AddData(datum);

    result.AddRows(row);
    return Aws::TimestreamQuery::Model::QueryOutcome(result);
  } else {
    Aws::TimestreamQuery::TimestreamQueryError error(
        Aws::Client::AWSError< Aws::Client::CoreErrors >(
            Aws::Client::CoreErrors::UNKNOWN, false));

    return Aws::TimestreamQuery::Model::QueryOutcome(error);
  }
}

}  // namespace odbc
}  // namespace ignite
