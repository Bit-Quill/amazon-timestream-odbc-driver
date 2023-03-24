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

#ifndef _TIMESTREAM_ODBC_IGNITE_ROW
#define _TIMESTREAM_ODBC_IGNITE_ROW

#include <stdint.h>

#include <vector>

#include "timestream/odbc/app/application_data_buffer.h"
#include "timestream/odbc/meta/column_meta.h"
#include "timestream/odbc/timestream_column.h"

#include <aws/timestream-query/model/Row.h>

using Aws::TimestreamQuery::Model::Row;

namespace timestream {
namespace odbc {
/**
 * Query result row.
 */
class TimestreamRow {
 public:
  /**
   * Constructor.
   * @param row Aws Row data.
   * @param columnMetadataVec Column metadata vector.
   * @param pageData Page data.
   */
  TimestreamRow(const Row& row,
                const meta::ColumnMetaVector& columnMetadataVec);

  /**
   * Destructor.
   */
  ~TimestreamRow() = default;

  /**
   * Get column number in a row.
   *
   * @return number of columns.
   */
  int32_t GetColumnSize() const {
    return columnMetadataVec_.size();
  }

  /**
   * Read column data and store it in application data buffer.
   *
   * @param columnIdx Column index.
   * @param dataBuf Application data buffer.
   * @return Conversion result.
   */
  app::ConversionResult::Type ReadColumnToBuffer(
      uint32_t columnIdx, app::ApplicationDataBuffer& dataBuf);

  /**
   * Updates the row and columns with a new row.
   *
   * @param row New row.
   * @return Conversion result.
   */
  void Update(const Row& row);

 private:
  IGNITE_NO_COPY_ASSIGNMENT(TimestreamRow);

  /**
   * Get columns by its index.
   *
   * Column indexing starts at 1.
   *
   * @note This operation is private because it's unsafe to use:
   *       It is neccessary to ensure that column is discovered prior
   *       to calling this method using EnsureColumnDiscovered().
   *
   * @param columnIdx Column index.
   * @return Reference to specified column.
   */
  TimestreamColumn& GetColumn(uint32_t columnIdx) {
    return columns_[columnIdx - 1];
  }

  /**
   * Ensure that column data is discovered.
   *
   * @param columnIdx Column index.
   * @return True if the column is discovered and false if it can not
   * be discovered.
   */
  bool EnsureColumnDiscovered(uint32_t columnIdx);

  /** Columns. */
  std::vector< TimestreamColumn > columns_;

  /** The current row */
  Row row_;

  /** The column metadata */
  const meta::ColumnMetaVector& columnMetadataVec_;
};
}  // namespace odbc
}  // namespace timestream

#endif  //_TIMESTREAM_ODBC_IGNITE_ROW
