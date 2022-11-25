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

#ifndef _IGNITE_ODBC_MULTI_MEASURE_METADATA_CREATER
#define _IGNITE_ODBC_MULTI_MEASURE_METADATA_CREATER

#include <aws/core/Aws.h>
#include <aws/timestream-write/model/Dimension.h>
#include <aws/timestream-write/model/Record.h>
#include <aws/timestream-write/model/MeasureValue.h>


using Aws::TimestreamWrite::Model::Dimension;
using Aws::TimestreamWrite::Model::MeasureValue;
using Aws::TimestreamWrite::Model::Record;

namespace ignite {
namespace odbc {

class MeasureMetadataCreater {
 public:
  MeasureMetadataCreater() = default;

  virtual ~MeasureMetadataCreater() = default;

  virtual void CreateDimensions(Aws::Vector< Dimension >& dimensions) = 0;
  virtual void CreateRecords(
      Aws::Vector< Dimension >& dimensions, Aws::Vector< Record >& values) = 0;
  virtual void CreateMeasureValues(Aws::Vector< MeasureValue >& values)  = 0;
  virtual const char* GetMetricName() = 0;

  typedef void (*RecordValueAssignFunPtr)(Aws::Vector< Record >& values, int index);
  virtual RecordValueAssignFunPtr GetRecordValueAssignFunPtr() = 0;

  typedef void (*MeasureValueAssignFunPtr)(Aws::Vector< MeasureValue >& values,
                                          int index);
  virtual MeasureValueAssignFunPtr GetMeasureValueAssignFunPtr() = 0;
};
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_MULTI_MEASURE_METADATA_CREATER
