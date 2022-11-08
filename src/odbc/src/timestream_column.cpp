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

#include <chrono>
#include <ctime>
#include <time.h>
#include "ignite/odbc/timestream_column.h"
#include "ignite/odbc/utility.h"
#include "ignite/odbc/interval_year_month.h"
#include "ignite/odbc/interval_day_second.h"
#include <aws/timestream-query/model/ColumnInfo.h>

using Aws::TimestreamQuery::Model::ScalarType;

namespace ignite {
namespace odbc {

void TimestreamColumn::Update(const Row& row) {
  row_ = row;
}

TimestreamColumn::TimestreamColumn(const Row& row,
                                   uint32_t columnIdx,
                                   const meta::ColumnMeta& columnMeta)
    : row_(row),
      columnIdx_(columnIdx), 
      columnMeta_(columnMeta) {
}

ConversionResult::Type TimestreamColumn::ReadToBuffer(ApplicationDataBuffer& dataBuf) const {
  LOG_DEBUG_MSG("ReadToBuffer is called");
  const boost::optional< Aws::TimestreamQuery::Model::ColumnInfo >& columnInfo = columnMeta_.GetColumnInfo();

  if (!columnInfo || !columnInfo->TypeHasBeenSet()) {
    LOG_ERROR_MSG("ColumnInfo is not found or type is not set");
    return ConversionResult::Type::AI_FAILURE;
  }

  Datum datum = row_.GetData()[columnIdx_];
  ConversionResult::Type retval = ParseDatum(datum, dataBuf);

  LOG_DEBUG_MSG("ReadToBuffer exiting");
  return retval;
}

ConversionResult::Type TimestreamColumn::ParseDatum(Datum& datum, ApplicationDataBuffer& dataBuf) const {
  LOG_DEBUG_MSG("ParseDatum is called");

  ConversionResult::Type retval;
  if (datum.ScalarValueHasBeenSet()) {
    retval = ParseScalarType(datum, dataBuf);
  } else {
    //TODO: handle complex type here 
    //https://bitquill.atlassian.net/browse/AT-1159
    LOG_ERROR_MSG("Non scalar value is found");
    retval = ConversionResult::Type::AI_FAILURE;
  }

  LOG_DEBUG_MSG("ParseDatum exiting");
  return retval;
}

ConversionResult::Type TimestreamColumn::ParseScalarType(Datum& datum, ApplicationDataBuffer& dataBuf) const {
  LOG_DEBUG_MSG("ParseScalarType is called");
  Aws::String value = datum.GetScalarValue();
  LOG_DEBUG_MSG("value is "<< value);

  ConversionResult::Type convRes = ConversionResult::Type::AI_SUCCESS;

  switch (columnMeta_.GetScalarType()) {
    case ScalarType::VARCHAR:
      convRes = dataBuf.PutString(value);
      break;
    case ScalarType::DOUBLE:
      convRes = dataBuf.PutDouble(std::stod(value));
      break;
    case ScalarType::BOOLEAN:
      convRes = dataBuf.PutInt8(value == "true" ? 1 : 0);
      break;
    case ScalarType::INTEGER:
      convRes = dataBuf.PutInt32(std::stoi(value));
      break;
    case ScalarType::BIGINT:
      convRes = dataBuf.PutInt64(std::stol(value));
      break;
    case ScalarType::NOT_SET:
    case ScalarType::UNKNOWN:
      convRes = dataBuf.PutNull();
      break;
    case ScalarType::TIMESTAMP: 
    {
      tm tmTime;
      memset(&tmTime, 0, sizeof(tm));
      int32_t fractionNs;
      std::sscanf(value.c_str(), "%4d-%2d-%2d %2d:%2d:%2d.%9d", &tmTime.tm_year,
                  &tmTime.tm_mon, &tmTime.tm_mday, &tmTime.tm_hour,
                  &tmTime.tm_min, &tmTime.tm_sec, &fractionNs);
      tmTime.tm_year -= 1900;
      tmTime.tm_mon--;
#ifdef _WIN32
      int64_t seconds = _mkgmtime(&tmTime);
#else
      int64_t seconds = timegm(&tmTime);
#endif     
      LOG_DEBUG_MSG("timestamp is " << tmTime.tm_year << " " << tmTime.tm_mon
                                    << " " << tmTime.tm_mday << " "
                                    << tmTime.tm_hour << ":" << tmTime.tm_min
                                    << ":" << tmTime.tm_sec << "."
                                    << fractionNs);

      LOG_DEBUG_MSG("seconds is " << seconds);
      convRes = dataBuf.PutTimestamp(Timestamp(seconds, fractionNs));
      break;
    }
    case ScalarType::DATE: 
    {
      tm tmTime;
      memset(&tmTime, 0 , sizeof(tm));
      int32_t fractionNs;
      std::sscanf(value.c_str(), "%4d-%2d-%2d", &tmTime.tm_year,
                  &tmTime.tm_mon, &tmTime.tm_mday);
      tmTime.tm_year -= 1900;
      tmTime.tm_mon--;
#ifdef _WIN32
      int64_t milliSecond = _mkgmtime(&tmTime)*1000;
#else
      int64_t milliSecond = timegm(&tmTime)*1000;
#endif     
      convRes = dataBuf.PutDate(Date(milliSecond));
      break;
    }
    case ScalarType::TIME: 
    {
      uint32_t hour; 
      uint32_t minute;
      uint32_t second;
      int32_t fractionNs;
      std::sscanf(value.c_str(), "%2d:%2d:%2d.%9d", &hour,
                  &minute, &second, &fractionNs);
      int32_t secondValue = (hour * 60 + minute) * 60 + second;
      convRes = dataBuf.PutTime(Time(secondValue, fractionNs));
      break;
    }
    case ScalarType::INTERVAL_YEAR_TO_MONTH: 
    {
      int32_t year, month;
      std::sscanf(value.c_str(), "%d-%d", &year, &month);
      convRes = dataBuf.PutInterval(IntervalYearMonth(year, month));
      break;
    }
    case ScalarType::INTERVAL_DAY_TO_SECOND: 
    {
      int32_t day, hour, minute, second, fraction;
      std::sscanf(value.c_str(), "%d %2d:%2d:%2d.%9d", &day,
                  &hour, &minute, &second, &fraction);
      convRes = dataBuf.PutInterval(IntervalDaySecond(day, hour, minute, second, fraction));
      break;
    }
    default:
      return ConversionResult::Type::AI_UNSUPPORTED_CONVERSION;
  }

  LOG_DEBUG_MSG("ParseScalarType exiting");
  return convRes;
}
}  // namespace odbc
}  // namespace ignite
