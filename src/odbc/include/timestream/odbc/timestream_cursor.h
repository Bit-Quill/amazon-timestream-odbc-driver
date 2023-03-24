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

#ifndef _TIMESTREAM_ODBC_IGNITE_CURSOR
#define _TIMESTREAM_ODBC_IGNITE_CURSOR

#include <stdint.h>

#include <map>
#include <memory>

#include "timestream/odbc/common_types.h"
#include "timestream/odbc/timestream_row.h"
#include "timestream/odbc/meta/column_meta.h"

#include <aws/core/utils/memory/stl/AWSVector.h>
#include <aws/timestream-query/model/Row.h>

using Aws::TimestreamQuery::Model::Row;

namespace timestream {
namespace odbc {
/**
 * Query result cursor.
 */
class TimestreamCursor {
 public:
  /**
   * Constructor.
   * @param rowVec Aws Row vector.
   * @param columnMetadataVec Column metadata vector.
   */
  TimestreamCursor(const Aws::Vector< Row > rowVec,
                   const meta::ColumnMetaVector& columnMetadataVec);

  /**
   * Destructor.
   */
  ~TimestreamCursor();

  /**
   * Move cursor to the next result row.
   *
   * @return False if data update required or no more data.
   */
  bool Increment();

  /**
   * Check if the cursor has data.
   *
   * @return True if the cursor has data.
   */
  bool HasData() const;

  /**
   * Get current row.
   *
   * @return Current row. Returns zero if cursor needs data update or has no
   * more data.
   */
  TimestreamRow* GetRow();

 private:
  IGNITE_NO_COPY_ASSIGNMENT(TimestreamCursor);

  /** Resultset rows */
  const Aws::Vector< Row > rowVec_;

  /** The iterator to beginning of cursor */
  Aws::Vector< Row >::const_iterator iterator_;

  /** The iterator to end of cursor */
  const Aws::Vector< Row >::const_iterator iteratorEnd_;

  /** The column metadata vector*/
  const meta::ColumnMetaVector& columnMetadataVec_;

  /** The current row */
  std::unique_ptr< TimestreamRow > currentRow_;
};
}  // namespace odbc
}  // namespace timestream

#endif  //_TIMESTREAM_ODBC_IGNITE_CURSOR
