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

#include "timestream/odbc/timestream_row.h"
#include "timestream/odbc/utility.h"

namespace timestream {
namespace odbc {
TimestreamRow::TimestreamRow(const Row& row,
                             const meta::ColumnMetaVector& columnMetadataVec)
    : columns_(), row_(row), columnMetadataVec_(columnMetadataVec) {
}

void TimestreamRow::Update(const Row& row) {
  LOG_DEBUG_MSG("Update is called");
  row_ = row;
  for (TimestreamColumn& column : columns_) {
    column.Update(row);
  }
}

app::ConversionResult::Type TimestreamRow::ReadColumnToBuffer(
    uint32_t columnIdx, app::ApplicationDataBuffer& dataBuf) {
  LOG_DEBUG_MSG("ReadColumnToBuffer is called");
  if (!EnsureColumnDiscovered(columnIdx)) {
    LOG_ERROR_MSG("columnIdx could not be discovered for index " << columnIdx);
    return app::ConversionResult::Type::AI_FAILURE;
  }

  TimestreamColumn const& column = GetColumn(columnIdx);

  return column.ReadToBuffer(dataBuf);
}

bool TimestreamRow::EnsureColumnDiscovered(uint32_t columnIdx) {
  LOG_DEBUG_MSG("EnsureColumnDiscovered is called for column " << columnIdx);
  if (columnIdx > columnMetadataVec_.size() || columnIdx < 1) {
    LOG_ERROR_MSG("columnIdx out of range for index " << columnIdx);
    return false;
  }

  LOG_DEBUG_MSG("columns_ size is " << columns_.size()
                                    << ", columnMetadataVec_ size is "
                                    << columnMetadataVec_.size());
  if (columns_.size() == columnMetadataVec_.size()) {
    return true;
  }

  uint32_t index = columns_.size();
  while (index < columnIdx) {
    TimestreamColumn newColumn(row_, index, columnMetadataVec_[index]);

    columns_.push_back(newColumn);
    index++;
  }

  return true;
}
}  // namespace odbc
}  // namespace timestream
