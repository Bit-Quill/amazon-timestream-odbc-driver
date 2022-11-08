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

#include "ignite/odbc/timestream_cursor.h"

namespace ignite {
namespace odbc {
TimestreamCursor::TimestreamCursor(const Aws::Vector< Row >& rowVec,
                                   const meta::ColumnMetaVector& columnMetadataVec)
    : iterator_(rowVec.begin()),
      iteratorEnd_(rowVec.end()),
      columnMetadataVec_(columnMetadataVec) {
  // No-op.
}

TimestreamCursor::~TimestreamCursor() {
// No-op.
}

bool TimestreamCursor::Increment() {
  LOG_DEBUG_MSG("Increment is called");
  bool hasData = HasData();
  if (hasData) {
    if (currentRow_) {
      (*currentRow_).Update(*iterator_);
    } else {
      currentRow_.reset(new TimestreamRow(*iterator_, columnMetadataVec_));
    }
    ++iterator_;
  } else {
    LOG_DEBUG_MSG("No data is found");
    currentRow_.reset();
  }
  LOG_DEBUG_MSG("Increment exiting");
  return hasData;
}

bool TimestreamCursor::HasData() const {
  return iterator_ != iteratorEnd_;
}

TimestreamRow* TimestreamCursor::GetRow() {
  return currentRow_.get();
}
}  // namespace odbc
}  // namespace timestream