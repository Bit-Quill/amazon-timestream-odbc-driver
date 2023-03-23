/*
 * Copyright <2022> Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 */

#include "timestream/odbc/timestream_cursor.h"

namespace timestream {
namespace odbc {
TimestreamCursor::TimestreamCursor(const Aws::Vector< Row > rowVec,
                                   const meta::ColumnMetaVector& columnMetadataVec)
    : rowVec_(rowVec), 
      iterator_(rowVec_.begin()),
      iteratorEnd_(rowVec_.end()),
      columnMetadataVec_(columnMetadataVec) {
  // No-op.
}

TimestreamCursor::~TimestreamCursor() {
// No-op.
}

bool TimestreamCursor::Increment() {
  LOG_DEBUG_MSG("Increment is called");
  bool hasData = HasData();
  LOG_DEBUG_MSG("hasData is " << hasData);
  if (hasData) {
    if (currentRow_) {
      (*currentRow_).Update(*iterator_);
    } else {
      currentRow_.reset(new TimestreamRow(*iterator_, columnMetadataVec_));
    }
    ++iterator_;
  } else {
    currentRow_.reset();
  }
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
