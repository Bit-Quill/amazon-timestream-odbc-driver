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
      resultMetaAvailable_(false),
      result_(nullptr),
      timeout_(timeout) {
  // No-op.

  LOG_DEBUG_MSG("DataQuery constructor is called, and exiting");
}

DataQuery::~DataQuery() {
  LOG_DEBUG_MSG("~DataQuery is called, and exiting");
}

SqlResult::Type DataQuery::Execute() {
  LOG_DEBUG_MSG("Execute is called");

  if (result_.get())
    InternalClose();

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

  // TODO [AT-1138] complete implementation for get data
  // https://bitquill.atlassian.net/browse/AT-1138
  // purpose of function: put data in columnBindings
  // (see commented out for loop that uses `columnBindings`)

  if (!result_.get()) {
    LOG_ERROR_MSG("result_ object is empty");
    return SqlResult::AI_ERROR;
  }

  std::string nextToken = result_->GetNextToken();

  request_.SetNextToken(nextToken);

  Aws::TimestreamQuery::Model::QueryOutcome outcome =
      connection_.GetQueryClient()->Query(request_);

  if (!outcome.IsSuccess()) {
    auto error = outcome.GetError();
    LOG_DEBUG_MSG("ERROR: " << error.GetExceptionName() << ": "
                            << error.GetMessage());

    diag.AddStatusRecord(SqlState::SHY000_GENERAL_ERROR,
                         "AWS API ERROR: " + error.GetExceptionName() + ": "
                             + error.GetMessage());

    InternalClose();
    return SqlResult::AI_ERROR;
  }
  // outcome is successful, update `result_`
  result_ = std::make_shared< QueryResult >(outcome.GetResult());

  // AT-1138 todo fetch the data using `result_`

  // OLD code
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

  // TODO [AT-1138] implement get column data
  // https://bitquill.atlassian.net/browse/AT-1138
  //
  // Purpose of this function: get current row from results_,
  // then get the column with index `columnIdx`, and
  // put the data inside buffer pointer.
  //
  // Note that re-using DocumentDbColumn would be non-trivial,
  // since it uses bsoncxx::document::element, and we
  // don't have it in Timestream.
  //
  // for timestream implementation reference
  /*
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

  return result;
  */
  buffer.PutNull();

  LOG_DEBUG_MSG("GetColumn exiting");
  LOG_DEBUG_MSG(
      "AI_NO_DATA is returned by default. TODO implement function in AT-1138");

  return SqlResult::AI_NO_DATA;
}

SqlResult::Type DataQuery::Close() {
  LOG_DEBUG_MSG("Close is called, and exiting");

  return InternalClose();
}

SqlResult::Type DataQuery::InternalClose() {
  LOG_DEBUG_MSG("InternalClose is called");

  result_.reset();

  LOG_DEBUG_MSG("InternalClose exiting");

  return SqlResult::AI_SUCCESS;
}

bool DataQuery::DataAvailable() const {
  LOG_DEBUG_MSG("DataAvailable is called, and exiting");
  // TODO [AT-1138] indicate whether there is data available.
  // https://bitquill.atlassian.net/browse/AT-1138

  // not implemented

  // for timestream implementation reference
  // return result_.get() && result_->GetRows();
  return false;
}

int64_t DataQuery::AffectedRows() const {
  // Return zero by default, since number of affected rows is non-zero only
  // if we're executing update statements, which is not supported
  LOG_DEBUG_MSG("AffectedRows is called, and exiting. 0 is returned.");

  return 0;
}

SqlResult::Type DataQuery::NextResultSet() {
  // TODO [AT-1134] Implement SQLMoreResults
  // https://bitquill.atlassian.net/browse/AT-1134
  LOG_DEBUG_MSG("NextResultSet is called");

  InternalClose();

  LOG_DEBUG_MSG("NextResultSet exiting");

  return SqlResult::AI_NO_DATA;
}

SqlResult::Type DataQuery::MakeRequestExecute() {
  // This function is called by Execute() and does the actual querying
  LOG_DEBUG_MSG("MakeRequestExecute is called");

  // Populate `result_` and the fetching is done in MakeRequestFetch.
  LOG_INFO_MSG("sql query: " << sql_);

  request_.SetQueryString(sql_);

  Aws::TimestreamQuery::Model::QueryOutcome outcome =
      connection_.GetQueryClient()->Query(request_);

  if (!outcome.IsSuccess()) {
    auto error = outcome.GetError();

    diag.AddStatusRecord(SqlState::SHY000_GENERAL_ERROR,
                         "AWS API ERROR: " + error.GetExceptionName() + ": "
                             + error.GetMessage());

    InternalClose();

    LOG_ERROR_MSG("MakeRequestExecute exiting with error msg: "
                  << error.GetExceptionName() << ": " << error.GetMessage());
    return SqlResult::AI_ERROR;
  }
  // outcome is successful, update result_
  result_ = std::make_shared< QueryResult >(outcome.GetResult());

  LOG_DEBUG_MSG("MakeRequestExecute exiting");

  return MakeRequestFetch();
}

SqlResult::Type DataQuery::MakeRequestFetch() {
  LOG_DEBUG_MSG("MakeRequestFetch is called");

  // TODO [AT-1138] implement get data
  // https://bitquill.atlassian.net/browse/AT-1138
  // pre-requisite: result_ has been populated

  if (!result_.get()) {
    diag.AddStatusRecord(SqlState::SHY010_SEQUENCE_ERROR,
                         "result_ is a null pointer");
    LOG_DEBUG_MSG("result_ pointer is not populated.");
    return SqlResult::AI_ERROR;
  }

  Aws::Vector< ColumnInfo > columnInfo = result_->GetColumnInfo();

  if (!resultMetaAvailable_) {
    ReadColumnMetadataVector(columnInfo);
  }

  // TODO [AT-1138] fetch the data

  LOG_DEBUG_MSG("MakeRequestFetch exiting");

  return SqlResult::AI_NO_DATA;
}

SqlResult::Type DataQuery::MakeRequestResultsetMeta() {
  LOG_DEBUG_MSG("MakeRequestResultsetMeta is called");

  QueryRequest request;
  request.SetQueryString(sql_);

  Aws::TimestreamQuery::Model::QueryOutcome outcome =
      connection_.GetQueryClient()->Query(request);

  if (!outcome.IsSuccess()) {
    auto const error = outcome.GetError();

    diag.AddStatusRecord(SqlState::SHY000_GENERAL_ERROR,
                         "AWS API ERROR: " + error.GetExceptionName() + ": "
                             + error.GetMessage());

    InternalClose();

    LOG_ERROR_MSG("MakeRequestResultsetMeta exiting with error msg: "
                  << error.GetExceptionName() << ": " << error.GetMessage());
    return SqlResult::AI_ERROR;
  }
  // outcome is successful
  QueryResult result = outcome.GetResult();
  Aws::Vector< ColumnInfo > columnInfo = result.GetColumnInfo();

  ReadColumnMetadataVector(columnInfo);

  LOG_DEBUG_MSG("MakeRequestResultsetMeta exiting");

  return SqlResult::AI_SUCCESS;
}

void DataQuery::ReadColumnMetadataVector(
    const Aws::Vector< ColumnInfo > tsVector) {
  LOG_DEBUG_MSG("ReadColumnMetadataVector is called");

  using ignite::odbc::meta::ColumnMeta;
  resultMeta_.clear();

  if (tsVector.empty()) {
    LOG_ERROR_MSG(
        "ReadColumnMetadataVector exiting without reading Timestream vector");
    LOG_INFO_MSG("reason: tsVector is empty");

    return;
  }

  for (ColumnInfo tsMetadata : tsVector) {
    resultMeta_.emplace_back(ColumnMeta());
    resultMeta_.back().ReadMetadata(tsMetadata);
  }
  resultMetaAvailable_ = true;

  LOG_DEBUG_MSG("ReadColumnMetadataVector exiting");
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

  // assign `resultMeta_` with contents of `value`
  resultMeta_.assign(value.begin(), value.end());
  resultMetaAvailable_ = true;

  // the nested forloops are for logging purposes
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
