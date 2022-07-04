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

#include "ignite/odbc/query/data_query.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/pipeline.hpp>

#include "ignite/odbc/connection.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/message.h"
#include "ignite/odbc/odbc_error.h"
#include "ignite/odbc/query/batch_query.h"

namespace ignite {
namespace odbc {
namespace query {
DataQuery::DataQuery(diagnostic::DiagnosableAdapter& diag,
                     Connection& connection, const std::string& sql,
                     const app::ParameterSet& params, int32_t& timeout)
    : Query(diag, QueryType::DATA),
      connection_(connection),
      sql_(sql),
      params_(params),
      timeout_(timeout) {
  // No-op.

  LOG_DEBUG_MSG("DataQuery constructor is called, and exiting");
}

DataQuery::~DataQuery() {
  LOG_DEBUG_MSG("~DataQuery is called");

  InternalClose();

  LOG_DEBUG_MSG("~DataQuery exiting");
}

SqlResult::Type DataQuery::Execute() {
  LOG_DEBUG_MSG("Execute is called");

  // not implemented
  /* for timestream implementation reference
  if (cursor_.get())
    InternalClose();
  */

  LOG_DEBUG_MSG("Execute exiting");

  return MakeRequestExecute();
}

const meta::ColumnMetaVector* DataQuery::GetMeta() {
  LOG_DEBUG_MSG("GetMeta is called");

  if (!resultMetaAvailable_) {
    MakeRequestResultsetMeta();

    if (!resultMetaAvailable_) {
      LOG_ERROR_MSG("GetMeta exiting with error. Returning nullptr");
      LOG_DEBUG_MSG("reason: result meta is not available");

      return nullptr;
    }
  }

  LOG_DEBUG_MSG("GetMeta exiting");

  return &resultMeta_;
}

SqlResult::Type DataQuery::FetchNextRow(app::ColumnBindingMap& columnBindings) {
  LOG_DEBUG_MSG("FetchNextRow is called");

  // not implemented
  /* for timestream implementation reference
  if (!cursor_.get()) {
    diag.AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR,
                         "Query was not executed.");

    LOG_ERROR_MSG("FetchNextRow exiting with AI_ERROR");
    LOG_DEBUG_MSG("reason: query was not executed");

    return SqlResult::AI_ERROR;
  }

  if (!cursor_->HasData()) {
    LOG_INFO_MSG("FetchNextRow exiting with AI_NO_DATA");
    LOG_DEBUG_MSG("reason: cursor does not have data");

    return SqlResult::AI_NO_DATA;
  }

  if (!cursor_->Increment()) {
    LOG_INFO_MSG("FetchNextRow exiting with AI_NO_DATA");
    LOG_DEBUG_MSG(
        "reason: cursor cannot be moved to the next row; either data update is "
        "required or there is no more data");

    return SqlResult::AI_NO_DATA;
  }

  DocumentDbRow* row = cursor_->GetRow();

  if (!row) {
    diag.AddStatusRecord("Unknown error.");

    LOG_ERROR_MSG("FetchNextRow exiting with AI_ERROR");
    LOG_DEBUG_MSG("Error unknown. Getting row from cursor failed.");

    return SqlResult::AI_ERROR;
  }

  for (uint32_t i = 1; i < row->GetSize() + 1; ++i) {
    app::ColumnBindingMap::iterator it = columnBindings.find(i);

    if (it == columnBindings.end())
      continue;

    app::ConversionResult::Type convRes =
        row->ReadColumnToBuffer(i, it->second);

    SqlResult::Type result = ProcessConversionResult(convRes, 0, i);

    if (result == SqlResult::AI_ERROR) {
      LOG_ERROR_MSG("FetchNextRow exiting with AI_ERROR");
      LOG_DEBUG_MSG(
          "error occured during column conversion operation, inside the for "
          "loop");

      return SqlResult::AI_ERROR;
    }
  }
  */

  LOG_DEBUG_MSG("FetchNextRow exiting");

  return SqlResult::AI_ERROR;
}

SqlResult::Type DataQuery::GetColumn(uint16_t columnIdx,
                                     app::ApplicationDataBuffer& buffer) {
  LOG_DEBUG_MSG("GetColumn is called");

  // not implemented
  /* for timestream implementation reference
  if (!cursor_.get()) {
    diag.AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR,
                         "Query was not executed.");

    LOG_ERROR_MSG("GetColumn exiting with error: Query was not executed");

    return SqlResult::AI_ERROR;
  }

  DocumentDbRow* row = cursor_->GetRow();

  if (!row) {
    diag.AddStatusRecord(SqlState::S24000_INVALID_CURSOR_STATE,
                         "Cursor has reached end of the result set.");

    LOG_ERROR_MSG(
        "GetColumn exiting with error: Cursor has reached end of the result "
        "set");

    return SqlResult::AI_ERROR;
  }

  app::ConversionResult::Type convRes =
      row->ReadColumnToBuffer(columnIdx, buffer);

  SqlResult::Type result = ProcessConversionResult(convRes, 0, columnIdx);
  */

  LOG_DEBUG_MSG("GetColumn exiting");

  return SqlResult::AI_ERROR;
}

SqlResult::Type DataQuery::Close() {
  LOG_DEBUG_MSG("Close is called, and exiting");

  return InternalClose();
}

SqlResult::Type DataQuery::InternalClose() {
  LOG_DEBUG_MSG("InternalClose is called");

  // not implemented
  /* for timestream implementation reference
  if (!cursor_.get()) {
    LOG_DEBUG_MSG("InternalClose exiting");

    return SqlResult::AI_SUCCESS;
  }

  SqlResult::Type result = MakeRequestClose();
  if (result == SqlResult::AI_SUCCESS) {
    cursor_.reset();
  }
  */

  LOG_DEBUG_MSG("InternalClose exiting");

  return SqlResult::AI_ERROR;
}

bool DataQuery::DataAvailable() const {
  LOG_DEBUG_MSG("DataAvailable is called, and exiting");

  // not implemented
 
  // for timestream implementation reference
  //return cursor_.get() && cursor_->HasData();
  return false;
}

int64_t DataQuery::AffectedRows() const {
  LOG_DEBUG_MSG("AffectedRows is called, and exiting");

  return 0;
}

SqlResult::Type DataQuery::NextResultSet() {
  LOG_DEBUG_MSG("NextResultSet is called");

  InternalClose();

  LOG_DEBUG_MSG("NextResultSet exiting");

  return SqlResult::AI_NO_DATA;
}

SqlResult::Type DataQuery::MakeRequestExecute() {
  LOG_DEBUG_MSG("MakeRequestExecute is called");

  // not implemented
 
  // for timestream implementation reference
  //cursor_.reset(); 

  LOG_DEBUG_MSG("MakeRequestExecute exiting");

  return MakeRequestFetch();
}

SqlResult::Type DataQuery::MakeRequestClose() {
  LOG_DEBUG_MSG("MakeRequestClose is called, and exiting");

  return SqlResult::AI_ERROR;
}

SqlResult::Type DataQuery::MakeRequestFetch() {
  LOG_DEBUG_MSG("MakeRequestFetch is called");

  // not implemented

  LOG_DEBUG_MSG("MakeRequestFetch exiting");

  return SqlResult::AI_ERROR;
}


SqlResult::Type DataQuery::MakeRequestMoreResults() {
  LOG_DEBUG_MSG("MakeRequestMoreResults is called, and exiting");

  return SqlResult::AI_SUCCESS;
}

SqlResult::Type DataQuery::MakeRequestResultsetMeta() {
  LOG_DEBUG_MSG("MakeRequestResultsetMeta is called");

  // not implemented

  LOG_DEBUG_MSG("MakeRequestResultsetMeta exiting");

  return SqlResult::AI_ERROR;
}

SqlResult::Type DataQuery::ProcessConversionResult(
    app::ConversionResult::Type convRes, int32_t rowIdx, int32_t columnIdx) {
  LOG_DEBUG_MSG("ProcessConversionResult is called");

  switch (convRes) {
    case app::ConversionResult::Type::AI_SUCCESS: {
      LOG_DEBUG_MSG("parameter: convRes: AI_SUCCESS");
      LOG_DEBUG_MSG("ProcessConversionResult exiting");

      return SqlResult::AI_SUCCESS;
    }

    case app::ConversionResult::Type::AI_NO_DATA: {
      LOG_DEBUG_MSG("parameter: convRes: AI_NO_DATA");
      LOG_DEBUG_MSG("ProcessConversionResult exiting");

      return SqlResult::AI_NO_DATA;
    }

    case app::ConversionResult::Type::AI_VARLEN_DATA_TRUNCATED: {
      diag.AddStatusRecord(
          SqlState::S01004_DATA_TRUNCATED,
          "Buffer is too small for the column data. Truncated from the right.",
          rowIdx, columnIdx);
      LOG_DEBUG_MSG("parameter: convRes: AI_VARLEN_DATA_TRUNCATED");
      LOG_DEBUG_MSG(
          "ProcessConversionResult exiting with AI_SUCCESS_WITH_INFO: Buffer "
          "is too small for the column data. Truncated from the right.");

      return SqlResult::AI_SUCCESS_WITH_INFO;
    }

    case app::ConversionResult::Type::AI_FRACTIONAL_TRUNCATED: {
      diag.AddStatusRecord(
          SqlState::S01S07_FRACTIONAL_TRUNCATION,
          "Buffer is too small for the column data. Fraction truncated.",
          rowIdx, columnIdx);

      LOG_DEBUG_MSG("parameter: convRes: AI_FRACTIONAL_TRUNCATED");
      LOG_DEBUG_MSG(
          "ProcessConversionResult exiting with AI_SUCCESS_WITH_INFO: Buffer "
          "is too small for the column data. "
          "Fraction truncated.");

      return SqlResult::AI_SUCCESS_WITH_INFO;
    }

    case app::ConversionResult::Type::AI_INDICATOR_NEEDED: {
      diag.AddStatusRecord(
          SqlState::S22002_INDICATOR_NEEDED,
          "Indicator is needed but not suplied for the column buffer.", rowIdx,
          columnIdx);

      LOG_DEBUG_MSG("parameter: convRes: AI_INDICATOR_NEEDED");
      LOG_DEBUG_MSG(
          "ProcessConversionResult exiting with AI_SUCCESS_WITH_INFO: "
          "Indicator is needed but not suplied for the column buffer.");

      return SqlResult::AI_SUCCESS_WITH_INFO;
    }

    case app::ConversionResult::Type::AI_UNSUPPORTED_CONVERSION: {
      diag.AddStatusRecord(SqlState::SHYC00_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                           "Data conversion is not supported.", rowIdx,
                           columnIdx);

      LOG_DEBUG_MSG("parameter: convRes: AI_UNSUPPORTED_CONVERSION");
      LOG_DEBUG_MSG(
          "ProcessConversionResult exiting with AI_SUCCESS_WITH_INFO: Data "
          "conversion is not supported.");

      return SqlResult::AI_SUCCESS_WITH_INFO;
    }

    case app::ConversionResult::Type::AI_FAILURE:
      LOG_DEBUG_MSG("parameter: convRes: AI_FAILURE");
    default: {
      diag.AddStatusRecord(SqlState::S01S01_ERROR_IN_ROW,
                           "Can not retrieve row column.", rowIdx, columnIdx);
      LOG_ERROR_MSG(
          "Default case: ProcessConversionResult exiting. msg: Can not "
          "retrieve row column.");
      break;
    }
  }
  LOG_ERROR_MSG("ProcessConversionResult exiting with error");

  return SqlResult::AI_ERROR;
}

void DataQuery::SetResultsetMeta(const meta::ColumnMetaVector& value) {
  LOG_DEBUG_MSG("SetResultsetMeta is called");

  resultMeta_.assign(value.begin(), value.end());
  resultMetaAvailable_ = true;

  for (size_t i = 0; i < resultMeta_.size(); ++i) {
    meta::ColumnMeta& meta = resultMeta_.at(i);
    if (meta.GetDataType()) {
      LOG_DEBUG_MSG(
          "\n[" << i << "] SchemaName:     "
                << meta.GetSchemaName().get_value_or("") << "\n[" << i
                << "] TypeName:       " << meta.GetTableName().get_value_or("")
                << "\n[" << i
                << "] ColumnName:     " << meta.GetColumnName().get_value_or("")
                << "\n[" << i << "] ColumnType:     "
                << static_cast< int32_t >(*meta.GetDataType()));
    } else {
      LOG_DEBUG_MSG(
          "\n[" << i << "] SchemaName:     "
                << meta.GetSchemaName().get_value_or("") << "\n[" << i
                << "] TypeName:       " << meta.GetTableName().get_value_or("")
                << "\n[" << i
                << "] ColumnName:     " << meta.GetColumnName().get_value_or("")
                << "\n[" << i << "] ColumnType: not available");
    }
  }
  LOG_DEBUG_MSG("SetResultsetMeta exiting");
}
}  // namespace query
}  // namespace odbc
}  // namespace ignite
