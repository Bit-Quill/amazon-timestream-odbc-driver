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

#ifndef _IGNITE_ODBC_TIMESTREAM_WRITER
#define _IGNITE_ODBC_TIMESTREAM_WRITER

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/timestream-write/TimestreamWriteClient.h>

#include "metadata-creator/measure_metadata_creater.h"

namespace ignite {
namespace odbc {

class TimestreamWriter {
 public:
  TimestreamWriter(Aws::Auth::AWSCredentials& credentials,
                   Aws::Client::ClientConfiguration& clientCfg) {
    client_ = std::make_shared< Aws::TimestreamWrite::TimestreamWriteClient >(
        credentials, clientCfg);
  }

  ~TimestreamWriter() = default;

  bool WriteSingleValueRecords(const Aws::String& tableMeta,
                              const Aws::String& database,
                              const Aws::String& table, int loop);

  bool WriteMultiValueRecords(const Aws::String& tableMeta, const Aws::String& database,
                              const Aws::String& table, int loop);

 private:
  std::shared_ptr< MeasureMetadataCreater > CreateMetadataCreater(
      Aws::String tableType);

  std::shared_ptr< Aws::TimestreamWrite::TimestreamWriteClient > client_;
};
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_TIMESTREAM_WRITER
