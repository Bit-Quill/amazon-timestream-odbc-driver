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
#if defined(__unix__) || defined(__unix) \
    || (defined(__APPLE__) && defined(__MACH__))
#define PREDEF_PLATFORM_UNIX_OR_APPLE 1
#endif

#ifdef PREDEF_PLATFORM_UNIX_OR_APPLE
#include <pwd.h>
#include <unistd.h>
#endif

#include <cstdlib>

#include "ignite/odbc/config/configuration.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/log_level.h"

using ignite::odbc::Logger;
using ignite::odbc::common::concurrent::CsLockGuard;
using ignite::odbc::config::Configuration;

// logger_ pointer will  initialized in first call to GetLoggerInstance
std::shared_ptr< Logger > Logger::logger_;

namespace ignite {
namespace odbc {
LogStream::LogStream(Logger* parent)
    : std::basic_ostream< char >(0), strbuf(), logger(parent) {
  init(&strbuf);
}

bool LogStream::operator()() const {
  return logger != nullptr;
}

LogStream::~LogStream() {
  if (logger) {
    logger->WriteMessage(strbuf.str());
  }
}

std::string Logger::GetDefaultLogPath() {
  std::string defPath;
#if defined(PREDEF_PLATFORM_UNIX_OR_APPLE)
  // try the $HOME environment variable (note: $HOME is not defined in OS X)
  defPath = common::GetEnv("HOME");
  if (defPath.empty()) {
    struct passwd* pwd = getpwuid(getuid());
    if (pwd)
      defPath = pwd->pw_dir;
  }
#elif defined(_WIN32)
  defPath = common::GetEnv("USERPROFILE");
  if (defPath.empty()) {
    const std::string homeDirectory = common::GetEnv("HOMEDRIVE");
    const std::string homepath = common::GetEnv("HOMEPATH");
    defPath = homeDirectory + homepath;
  }
#endif

  if (defPath.empty()) {
    // couldn't find home directory, fall back to current working directory
    std::cout << "warning: couldn't find home directory, the default log path "
                 "is set as the current working directory"
              << '\n';
    defPath = ".";
  }

  return defPath;
}

std::string Logger::CreateFileName() const {
  char tStr[1000];
  time_t curTime = time(nullptr);
  struct tm* locTime = localtime(&curTime);
  strftime(tStr, 1000, "%Y%m%d", locTime);
  std::string dateTime(tStr, std::find(tStr, tStr + 1000, '\0'));
  std::string fileName("timestream_odbc_" + dateTime + ".log");
  return fileName;
}

void Logger::SetLogPath(const std::string& path) {
  if (logPath == path) {
    LOG_DEBUG_MSG(
        "WARNING: SetLogPath is called with the existing path string. "
        "SetLogPath should only be called once in normal circumstances aside "
        "from testing.");
    return;
  }
  if (!common::IsValidDirectory(path)) {
    // if path is not a valid directory, ignore passed path.
    std::cerr << "Error during setting log path: \"" << path
              << "\" is not a valid directory. Log path is not updated" << '\n';
    std::cout << "Current Log Path: \"" << logPath << "\" \n";
    return;
  }
  std::string oldLogFilePath = logFilePath;
  logPath = path;
  if (IsEnabled() && logLevel != LogLevel::Type::OFF) {
    LOG_INFO_MSG("Reset log path: Log path is changed to " + logPath
                 + ". Log file is in format timestream_odbc_YYYYMMDD.log");

    // close file stream and erase log file name to allow new log file path
    fileStream.close();
    logFileName.erase();

    LOG_INFO_MSG("Previously logged information is stored in log file "
                 + oldLogFilePath);
  }
  SetLogStream(&fileStream);
}

void Logger::SetLogStream(std::ostream* logStream) {
  stream = logStream;
}

void Logger::SetLogLevel(LogLevel::Type level) {
  logLevel = level;
}

bool Logger::IsFileStreamOpen() const {
  return fileStream.is_open();
}

bool Logger::IsEnabled() const {
  return stream != nullptr && (stream != &fileStream || IsFileStreamOpen());
}

bool Logger::EnableLog() {
  // if stream is not set, set the stream as filestream
  if (stream == nullptr)
    SetLogStream(&fileStream);

  if (!IsEnabled() && logLevel != LogLevel::Type::OFF
      && stream == &fileStream) {
    if (logFileName.empty()) {
      logFileName = CreateFileName();
      std::stringstream tmpStream;
      tmpStream << logPath << ignite::odbc::common::Fs << logFileName;
      logFilePath = tmpStream.str();
      if (common::FileExists(logFilePath)) {
        std::cout << "log file at \"" << logFilePath
                  << "\" already exists. Appending logs to the log file."
                  << '\n';
      }
      std::cout << "logFilePath: " << logFilePath << '\n';
    }
    fileStream.open(logFilePath, std::ios_base::app);
  }
  return IsEnabled();
}

void Logger::WriteMessage(std::string const& message) {
  if (IsEnabled()) {
    CsLockGuard guard(mutex);
    *stream << message << std::endl;
  }
}

LogLevel::Type Logger::GetLogLevel() const {
  return logLevel;
}

std::string& Logger::GetLogPath() {
  return logPath;
}
}  // namespace odbc
}  // namespace ignite
