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

#ifndef _IGNITE_ODBC_TIMESTREAM_COLUMN
#define _IGNITE_ODBC_TIMESTREAM_COLUMN

#include <stdint.h>
#include <ignite/odbc/app/application_data_buffer.h>
#include "ignite/odbc/meta/column_meta.h"
#include <aws/timestream-query/model/Row.h>

using Aws::TimestreamQuery::Model::Row;
using Aws::TimestreamQuery::Model::Datum;
using ignite::odbc::app::ApplicationDataBuffer;
using ignite::odbc::app::ConversionResult;

namespace ignite {
namespace odbc {
/**
 * Result set column.
 */
class TimestreamColumn {
 public:
  /**
   * Constructor.
   *
   * @param row Aws Row.
   * @param columnIdx The column index, start from 0.
   * @param columnMetadata The column metadata.
   */
  TimestreamColumn(const Row& row,
                   uint32_t columnIdx, const meta::ColumnMeta& columnMetadata);

  /**
   * Copy constructor.
   *
   * @param other Another instance.
   */
  TimestreamColumn(const TimestreamColumn& other) = default;

  /**
   * Copy operator.
   *
   * @param other Another instance.
   * @return This.
   */
  TimestreamColumn& operator=(const TimestreamColumn& other) = delete;

  /**
   * Updates the reference to the current row.
   * @param row The new row that owns this column.
   */
  void Update(const Row& row);

  /**
   * Destructor.
   */
  ~TimestreamColumn() = default;

  /**
   * Read column data and store it in application data buffer.
   *
   * @param dataBuf Application data buffer.
   * @return Operation result.
   */
  ConversionResult::Type ReadToBuffer(ApplicationDataBuffer& dataBuf) const;

 private:
  /**
   * Parse Aws Datum data and save result to dataBuf
   * 
   * @param datum Aws datum which contains the result data
   * @param dataBuf Application data buffer.
   * @return Operation result.
   */
  ConversionResult::Type ParseDatum(const Datum& datum,
                                    ApplicationDataBuffer& dataBuf) const;

  /**
   * Parse scalar data type in datum and save result to dataBuf.
   *
   * @param datum Aws datum which contains the result data
   * @param dataBuf Application data buffer.
   * @return Operation result.
   */
  ConversionResult::Type ParseScalarType(
      const Datum& datum,
      ApplicationDataBuffer& dataBuf) const;

  /**
   * Parse TimeSeries data type in datum and save result to dataBuf.
   *
   * @param datum Aws datum which contains the result data
   * @param dataBuf Application data buffer.
   * @return Operation result.
   */
  ConversionResult::Type ParseTimeSeriesType(
      const Datum& datum,
      ApplicationDataBuffer& dataBuf) const;

  /**
   * Parse Array data type in datum and save result to dataBuf.
   *
   * @param datum Aws datum which contains the result data
   * @param dataBuf Application data buffer.
   * @return Operation result.
   */
  ConversionResult::Type ParseArrayType(
      const Datum& datum,
      ApplicationDataBuffer& dataBuf) const;

  /**
   * Parse Row data type in datum and save result to dataBuf.
   *
   * @param datum Aws datum which contains the result data
   * @param dataBuf Application data buffer.
   * @return Operation result.
   */
  ConversionResult::Type ParseRowType(
      const Datum& datum,
      ApplicationDataBuffer& dataBuf) const;

  /** The row that owns this column */
  Row row_;

  /** The column index */
  uint32_t columnIdx_;

  /** The column metadata */
  const meta::ColumnMeta& columnMeta_;
};
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_TIMESTREAM_COLUMN
