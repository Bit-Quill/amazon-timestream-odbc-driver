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

#ifndef _IGNITE_ODBC_CONFIG_CONNECTION_STRING_PARSER
#define _IGNITE_ODBC_CONFIG_CONNECTION_STRING_PARSER

#include <string>

#include "ignite/odbc/config/configuration.h"
#include "ignite/odbc/diagnostic/diagnostic_record_storage.h"

namespace ignite {
namespace odbc {
namespace config {
/**
 * ODBC configuration parser abstraction.
 */
class ConnectionStringParser {
 public:
  /** Connection attribute keywords. */
  struct Key {
    /** Connection attribute keyword for DSN attribute. */
    static const std::string dsn;

    /** Connection attribute keyword for Driver attribute. */
    static const std::string driver;

    /** Connection attribute keyword for username attribute. */
    static const std::string accessKeyId;

    /** Connection attribute keyword for password attribute. */
    static const std::string secretKey;

    /** Connection attribute keyword for sessiontoken attribute. */
    static const std::string sessionToken;

    /** Connection attribute keyword for enableMetadataPreparedStatement attribute. */
    static const std::string enableMetadataPreparedStatement;

    /** Connection attribute keyword for credProvClass attribute. */
    static const std::string credProvClass;

    /** Connection attribute keyword for cusCredFile attribute. */
    static const std::string cusCredFile;

    /** Connection attribute keyword for reqTimeout attribute. */
    static const std::string reqTimeout;

    /** Connection attribute keyword for socketTimeout attribute. */
    static const std::string socketTimeout;

    /** Connection attribute keyword for maxRetryCount attribute. */
    static const std::string maxRetryCount;

    /** Connection attribute keyword for maxConnections attribute. */
    static const std::string maxConnections;

    /** Connection attribute keyword for endpoint attribute. */
    static const std::string endpoint;

    /** Connection attribute keyword for region attribute. */
    static const std::string region;

    /** Connection attribute keyword for idpName attribute. */
    static const std::string idpName;

    /** Connection attribute keyword for idpHost attribute. */
    static const std::string idpHost;

    /** Connection attribute keyword for idpUserName attribute. */
    static const std::string idpUserName;

    /** Connection attribute keyword for idpPassword attribute. */
    static const std::string idpPassword;

    /** Connection attribute keyword for idpArn attribute. */
    static const std::string idpArn;

    /** Connection attribute keyword for oktaAppId attribute. */
    static const std::string oktaAppId;

    /** Connection attribute keyword for roleArn attribute. */
    static const std::string roleArn;

    /** Connection attribute keyword for aadAppId attribute. */
    static const std::string aadAppId;

    /** Connection attribute keyword for aadClientSecret attribute. */
    static const std::string aadClientSecret;

    /** Connection attribute keyword for aadTenant attribute. */
    static const std::string aadTenant;

    /** Connection attribute keyword for log level. */
    static const std::string logLevel;

    /** Connection attribute keyword for log path. */
    static const std::string logPath;
  };

  /**
   * Constructor.
   *
   * @param cfg Configuration.
   */
  ConnectionStringParser(Configuration& cfg);

  /**
   * Destructor.
   */
  ~ConnectionStringParser() = default;

  /**
   * Parse connect string.
   *
   * @param str String to parse.
   * @param len String length.
   * @param delimiter delimiter.
   * @param diag Diagnostics collector.
   */
  void ParseConnectionString(const char* str, size_t len, char delimiter,
                             diagnostic::DiagnosticRecordStorage* diag);

  /**
   * Parse connect string.
   *
   * @param str String to parse.
   * @param diag Diagnostics collector.
   */
  void ParseConnectionString(const std::string& str,
                             diagnostic::DiagnosticRecordStorage* diag);

  /**
   * Parse config attributes.
   *
   * @param str String to parse.
   * @param diag Diagnostics collector.
   */
  void ParseConfigAttributes(const char* str,
                             diagnostic::DiagnosticRecordStorage* diag);

 private:
  /**
   * Result of parsing string value to bool.
   */
  struct BoolParseResult {
    enum class Type { AI_FALSE, AI_TRUE, AI_UNRECOGNIZED };
  };

  /**
   * Handle new attribute pair callback.
   *
   * @param key Key.
   * @param value Value.
   * @param diag Diagnostics collector.
   */
  void HandleAttributePair(const std::string& key, const std::string& value,
                           diagnostic::DiagnosticRecordStorage* diag);

  /**
   * Convert string to boolean value.
   *
   * @param value Value to convert to bool.
   * @return Result.
   */
  static BoolParseResult::Type StringToBool(const std::string& value);

  /**
   * Convert string to boolean value.
   *
   * @param msg Error message.
   * @param key Key.
   * @param value Value.
   * @return Resulting error message.
   */
  static std::string MakeErrorMessage(const std::string& msg,
                                      const std::string& key,
                                      const std::string& value);

  /** Configuration. */
  Configuration& cfg;
};
}  // namespace config

}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_CONFIG_CONNECTION_STRING_PARSER
