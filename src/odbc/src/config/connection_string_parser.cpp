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

#include "ignite/odbc/config/connection_string_parser.h"

#include <vector>

#include "ignite/odbc/common/utils.h"
#include "ignite/odbc/config/config_tools.h"
#include "ignite/odbc/utility.h"

namespace ignite {
namespace odbc {
namespace config {
const std::string ConnectionStringParser::Key::dsn = "dsn";
const std::string ConnectionStringParser::Key::driver = "driver";
const std::string ConnectionStringParser::Key::accessKeyId = "access_key_id";
const std::string ConnectionStringParser::Key::secretKey = "secret_key";
const std::string ConnectionStringParser::Key::sessionToken = "session_token";
const std::string ConnectionStringParser::Key::enableMetadataPreparedStatement =
    "enable_metadata_prepared_statement";
const std::string ConnectionStringParser::Key::credProvClass =
    "aws_credentials_provider_class";
const std::string ConnectionStringParser::Key::cusCredFile =
    "custom_credentials_file";
const std::string ConnectionStringParser::Key::reqTimeout = "request_timeout";
const std::string ConnectionStringParser::Key::socketTimeout = "socket_timeout";
const std::string ConnectionStringParser::Key::maxRetryCount =
    "max_retry_count";
const std::string ConnectionStringParser::Key::maxConnections =
    "max_connections";
const std::string ConnectionStringParser::Key::endpoint = "endpoint";
const std::string ConnectionStringParser::Key::region = "region";
const std::string ConnectionStringParser::Key::idpName = "idp_name";
const std::string ConnectionStringParser::Key::idpHost = "idp_host";
const std::string ConnectionStringParser::Key::idpUserName = "idp_user_name";
const std::string ConnectionStringParser::Key::idpPassword = "idp_password";
const std::string ConnectionStringParser::Key::idpArn = "idp_arn";
const std::string ConnectionStringParser::Key::oktaAppId = "okta_app_id";
const std::string ConnectionStringParser::Key::roleArn = "role_arn";
const std::string ConnectionStringParser::Key::aadAppId = "aad_app_id";
const std::string ConnectionStringParser::Key::aadClientSecret =
    "aad_client_secret";
const std::string ConnectionStringParser::Key::aadTenant = "aad_tenant";
const std::string ConnectionStringParser::Key::logLevel = "log_level";
const std::string ConnectionStringParser::Key::logPath = "log_path";

ConnectionStringParser::ConnectionStringParser(Configuration& cfg) : cfg(cfg) {
  // No-op.
}

void ConnectionStringParser::ParseConnectionString(
    const char* str, size_t len, char delimiter,
    diagnostic::DiagnosticRecordStorage* diag) {
  std::string connect_str(str, len);

  while (connect_str.rbegin() != connect_str.rend()
         && *connect_str.rbegin() == 0)
    connect_str.erase(connect_str.size() - 1);

  while (!connect_str.empty()) {
    size_t attr_begin = connect_str.rfind(delimiter);

    if (attr_begin == std::string::npos)
      attr_begin = 0;
    else
      ++attr_begin;

    size_t attr_eq_pos = connect_str.rfind('=');

    if (attr_eq_pos == std::string::npos)
      attr_eq_pos = 0;

    if (attr_begin < attr_eq_pos) {
      const char* key_begin = connect_str.data() + attr_begin;
      const char* key_end = connect_str.data() + attr_eq_pos;

      const char* value_begin = connect_str.data() + attr_eq_pos + 1;
      const char* value_end = connect_str.data() + connect_str.size();

      std::string key = common::StripSurroundingWhitespaces(key_begin, key_end);
      std::string value =
          common::StripSurroundingWhitespaces(value_begin, value_end);

      if (value[0] == '{' && value[value.size() - 1] == '}')
        value = value.substr(1, value.size() - 2);

      HandleAttributePair(key, value, diag);
    }

    if (!attr_begin)
      break;

    connect_str.erase(attr_begin - 1);
  }
}

void ConnectionStringParser::ParseConnectionString(
    const std::string& str, diagnostic::DiagnosticRecordStorage* diag) {
  ParseConnectionString(str.data(), str.size(), ';', diag);
}

void ConnectionStringParser::ParseConfigAttributes(
    const char* str, diagnostic::DiagnosticRecordStorage* diag) {
  size_t len = 0;

  // Getting list length. List is terminated by two '\0'.
  while (str[len] || str[len + 1])
    ++len;

  ++len;

  ParseConnectionString(str, len, '\0', diag);
}

void ConnectionStringParser::HandleAttributePair(
    const std::string& key, const std::string& value,
    diagnostic::DiagnosticRecordStorage* diag) {
  std::string lKey = common::ToLower(key);

  if (lKey == Key::dsn) {
    cfg.SetDsn(value);
  } else if (lKey == Key::enableMetadataPreparedStatement) {
    BoolParseResult::Type res = StringToBool(value);

    if (res == BoolParseResult::Type::AI_UNRECOGNIZED) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Unrecognized bool value. Using default value.",
                             key, value));
      }
      return;
    }

    cfg.SetEnableMetadataPreparedStatement(res
                                           == BoolParseResult::Type::AI_TRUE);
  } else if (lKey == Key::credProvClass) {
    CredProvClass::Type className = CredProvClass::FromString(value);

    if (className == CredProvClass::Type::UNKNOWN) {
      if (diag) {
        diag->AddStatusRecord(SqlState::S01S02_OPTION_VALUE_CHANGED,
                              "Specified credProvClass is not supported. "
                              "Default value used ('none').");
      }
      return;
    }

    cfg.SetCredProvClass(className);
  } else if (lKey == Key::cusCredFile) {
    cfg.SetCusCredFile(value);
  } else if (lKey == Key::reqTimeout) {
    if (value.empty()) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Request Timeout attribute value is empty. Using "
                             "default value.",
                             key, value));
      }
      return;
    }

    if (!common::AllDigits(value)) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Request Timeout attribute value contains "
                             "unexpected characters."
                             " Using default value.",
                             key, value));
      }
      return;
    }

    if (value.size() >= sizeof(std::to_string(UINT32_MAX))) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Request Timeout attribute value is too large. "
                             "Using default value.",
                             key, value));
      }
      return;
    }

    int64_t numValue = 0;
    std::stringstream conv;

    conv << value;
    conv >> numValue;

    if (numValue <= 0 || numValue > UINT32_MAX) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Request Timeout attribute value is out of range. "
                             "Using default value.",
                             key, value));
      }
      return;
    }

    cfg.SetReqTimeout(static_cast< uint32_t >(numValue));
  } else if (lKey == Key::socketTimeout) {
    if (value.empty()) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Socket Timeout attribute value is empty. Using "
                             "default value.",
                             key, value));
      }
      return;
    }

    if (!common::AllDigits(value)) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Socket Timeout attribute value contains "
                             "unexpected characters."
                             " Using default value.",
                             key, value));
      }
      return;
    }

    if (value.size() >= sizeof(std::to_string(UINT32_MAX))) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Socket Timeout attribute value is too large. "
                             "Using default value.",
                             key, value));
      }
      return;
    }

    int64_t numValue = 0;
    std::stringstream conv;

    conv << value;
    conv >> numValue;

    if (numValue <= 0 || numValue > UINT32_MAX) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Socket Timeout attribute value is out of range. "
                             "Using default value.",
                             key, value));
      }
      return;
    }

    cfg.SetSocketTimeout(static_cast< uint32_t >(numValue));
  } else if (lKey == Key::maxRetryCount) {
    if (value.empty()) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Max Retry Count attribute value is empty. Using "
                             "default value.",
                             key, value));
      }
      return;
    }

    if (!common::AllDigits(value)) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Max Retry Count attribute value contains "
                             "unexpected characters."
                             " Using default value.",
                             key, value));
      }
      return;
    }

    if (value.size() >= sizeof(std::to_string(UINT32_MAX))) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Max Retry Count attribute value is too large. "
                             "Using default value.",
                             key, value));
      }
      return;
    }

    int64_t numValue = 0;
    std::stringstream conv;

    conv << value;
    conv >> numValue;

    if (numValue <= 0 || numValue > UINT32_MAX) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Max Retry Count attribute value is out of range. "
                             "Using default value.",
                             key, value));
      }
      return;
    }

    cfg.SetMaxRetryCount(static_cast< uint32_t >(numValue));
  } else if (lKey == Key::maxConnections) {
    if (value.empty()) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Max Connections attribute value is empty. Using "
                             "default value.",
                             key, value));
      }
      return;
    }

    if (!common::AllDigits(value)) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Max Connections attribute value contains "
                             "unexpected characters."
                             " Using default value.",
                             key, value));
      }
      return;
    }

    if (value.size() >= sizeof(std::to_string(UINT32_MAX))) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Max Connections attribute value is too large. "
                             "Using default value.",
                             key, value));
      }
      return;
    }

    int64_t numValue = 0;
    std::stringstream conv;

    conv << value;
    conv >> numValue;

    if (numValue <= 0 || numValue > UINT32_MAX) {
      if (diag) {
        diag->AddStatusRecord(
            SqlState::S01S02_OPTION_VALUE_CHANGED,
            MakeErrorMessage("Max Connections attribute value is out of range. "
                             "Using default value.",
                             key, value));
      }
      return;
    }

    cfg.SetMaxConnections(static_cast< uint32_t >(numValue));
  } else if (lKey == Key::endpoint) {
    cfg.SetEndpoint(value);
  } else if (lKey == Key::region) {
    cfg.SetRegion(value);
  } else if (lKey == Key::idpName) {
    IdpName::Type idpName = IdpName::FromString(value);

    if (idpName == IdpName::Type::UNKNOWN) {
      if (diag) {
        diag->AddStatusRecord(SqlState::S01S02_OPTION_VALUE_CHANGED,
                              "Specified idpName is not supported. "
                              "Default value used ('none').");
      }
      return;
    }

    cfg.SetIdpName(idpName);
  } else if (lKey == Key::idpHost) {
    cfg.SetIdpHost(value);
  } else if (lKey == Key::idpUserName) {
    if (!cfg.GetIdpUserName().empty() && diag) {
      diag->AddStatusRecord(
          SqlState::S01S02_OPTION_VALUE_CHANGED,
          "Re-writing IDP_USER_NAME (have you specified it several times?");
    }

    cfg.SetIdpUserName(value);
  } else if (lKey == Key::idpPassword) {
    if (!cfg.GetIdpPassword().empty() && diag) {
      diag->AddStatusRecord(
          SqlState::S01S02_OPTION_VALUE_CHANGED,
          "Re-writing IDP_PASSWORD (have you specified it several times?");
    }

    cfg.SetIdpPassword(value);
  } else if (lKey == Key::idpArn) {
    cfg.SetIdpArn(value);
  } else if (lKey == Key::oktaAppId) {
    cfg.SetOktaAppId(value);
  } else if (lKey == Key::roleArn) {
    cfg.SetRoleArn(value);
  } else if (lKey == Key::aadAppId) {
    cfg.SetAadAppId(value);
  } else if (lKey == Key::aadClientSecret) {
    if (!cfg.GetAadClientSecret().empty() && diag) {
      diag->AddStatusRecord(
          SqlState::S01S02_OPTION_VALUE_CHANGED,
          "Re-writing AAD_CLIENT_SECRET (have you specified it several times?");
    }

    cfg.SetAadClientSecret(value);
  } else if (lKey == Key::aadTenant) {
    cfg.SetAadTenant(value);
  } else if (lKey == Key::logLevel) {
    LogLevel::Type level = LogLevel::FromString(value);

    if (level == LogLevel::Type::UNKNOWN) {
      if (diag) {
        diag->AddStatusRecord(SqlState::S01S02_OPTION_VALUE_CHANGED,
                              "Specified log level is not supported. "
                              "Default value used ('error').");
      }
      return;
    }

    cfg.SetLogLevel(level);
  } else if (lKey == Key::logPath) {
    cfg.SetLogPath(value);
  } else if (lKey == Key::driver) {
    cfg.SetDriver(value);
  } else if (lKey == Key::accessKeyId) {
    if (!cfg.GetAccessKeyId().empty() && diag) {
      diag->AddStatusRecord(
          SqlState::S01S02_OPTION_VALUE_CHANGED,
          "Re-writing ACCESS_KEY_ID (have you specified it several times?");
    }

    cfg.SetAccessKeyId(value);
  } else if (lKey == Key::secretKey) {
    if (!cfg.GetSecretKey().empty() && diag) {
      diag->AddStatusRecord(
          SqlState::S01S02_OPTION_VALUE_CHANGED,
          "Re-writing SECRET_KEY (have you specified it several times?");
    }

    cfg.SetSecretKey(value);
  } else if (lKey == Key::sessionToken) {
    if (!cfg.GetSessionToken().empty() && diag) {
      diag->AddStatusRecord(
          SqlState::S01S02_OPTION_VALUE_CHANGED,
          "Re-writing SESSION_TOKEN (have you specified it several times?");
    }

    cfg.SetSessionToken(value);
  } else if (diag) {
    std::stringstream stream;

    stream << "Unknown attribute: '" << key << "'. Ignoring.";

    diag->AddStatusRecord(SqlState::S01S02_OPTION_VALUE_CHANGED, stream.str());
  }
}

ConnectionStringParser::BoolParseResult::Type
ConnectionStringParser::StringToBool(const std::string& value) {
  std::string lower = common::ToLower(value);

  if (lower == "true")
    return BoolParseResult::Type::AI_TRUE;

  if (lower == "false")
    return BoolParseResult::Type::AI_FALSE;

  return BoolParseResult::Type::AI_UNRECOGNIZED;
}

std::string ConnectionStringParser::MakeErrorMessage(const std::string& msg,
                                                     const std::string& key,
                                                     const std::string& value) {
  std::stringstream stream;

  stream << msg << " [key='" << key << "', value='" << value << "']";

  return stream.str();
}
}  // namespace config
}  // namespace odbc
}  // namespace ignite
