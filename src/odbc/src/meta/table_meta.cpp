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

#include "ignite/odbc/meta/table_meta.h"
#include "ignite/odbc/log.h"

#include <regex>

namespace ignite {
namespace odbc {
namespace meta {
const std::string TABLE_CAT = "TABLE_CAT";
const std::string TABLE_SCHEM = "TABLE_SCHEM";
const std::string TABLE_NAME = "TABLE_NAME";
const std::string TABLE_TYPE = "TABLE_TYPE";
const std::string REMARKS = "REMARKS";

void TableMeta::Read(Table& tb) {
  tableName = tb.GetTableName();
  schemaName = tb.GetDatabaseName();
  // Timestream only has table type "TABLE"
  tableType = "TABLE";
  // Timestream does not have catalog or table remarks, so those
  // values are kept as boost::none to indicate that they are empty
}
void TableMeta::Read(Database& db) {
  schemaName = db.GetDatabaseName();
}

void TableMeta::Read(std::string& tbType) {
  tableType = tbType;
}

void ReadTableMetaVector(const std::string& tableName,  bool isIdentifier,
                         const Aws::Vector< Table >& tbVector,
                         TableMetaVector& meta) {
  std::regex tablePattern(utility::ConvertPatternToRegex(tableName));

  // Parse through Table vector
  for (Table tb : tbVector) {
    std::string curTableName = tb.GetTableName();

    // TODO [AT-1154] Make server handle search patterns
    // and remove the code for search pattern here
    // https://bitquill.atlassian.net/browse/AT-1154
    if (tableName.empty() || (isIdentifier && curTableName == tableName) 
        || (!isIdentifier && std::regex_match(curTableName, tablePattern))) {
      meta.emplace_back(TableMeta());
      meta.back().Read(tb);
    } else {
      LOG_DEBUG_MSG("current table name ("
                    << curTableName << ") does not match the provided table name ("
                    << tableName
                    << "), so it will not be included in the resultset.");
    }
  }
}

void ReadDatabaseMetaVector(const Aws::Vector< Database >& dbVector,
                         TableMetaVector& meta) {
  // Parse through Database vector
  for (Database db : dbVector) {
    meta.emplace_back(TableMeta());
    meta.back().Read(db);
  }
}
}  // namespace meta
}  // namespace odbc
}  // namespace ignite
