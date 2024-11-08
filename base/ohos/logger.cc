

#include <stddef.h>

#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>

#include "logger.h"
#include "base/base_export.h"
#include "ohos_nweb/src/capi/nweb_logger_callback.h"
#include "base/debug/task_trace.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"

namespace ohos{

namespace logger {
enum  LogLevel {
    LEVEL_DEBUG = 0,
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR,
    LEVEL_FATAL,
};

using LogType = int;
constexpr LogType LOGTYPE_VERBOSE = -1;
constexpr LogType LOGTYPE_FEEDBACK = 0;
constexpr LogType LOGTYPE_URL = 1;

bool g_is_main_process_mode = false;

scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

BASE_EXPORT void SetMainProcessMode(bool is_main_process) {
  g_is_main_process_mode = is_main_process;
}

static std::shared_ptr<NWebLoggerCallback> logger_callback_ = nullptr; 

static const std::string TAG_PREFIX = "arkweb_";
static const std::string DEPRECATED_TAG_PREFIX = "cr.";

bool StartWith(const std::string str1, const std::string str2) {
  return (str1.compare(0, str2.size(), str2) == 0);
}

std::string normalizeTag(const std::string& tag) {
  if (StartWith(tag, TAG_PREFIX)) {
    return tag;
  }
  
  int unprefixedTagStart = 0;
  if (StartWith(tag, DEPRECATED_TAG_PREFIX)) {
    unprefixedTagStart = DEPRECATED_TAG_PREFIX.size();
  }

  return TAG_PREFIX + tag.substr(unprefixedTagStart);
}

void LogFeedbackWrite(const std::string& tag, LogLevel level, const std::string& message) {
  if (!logger_callback_ || !logger_callback_->logFeedback) {
    return;
  }
  if (g_is_main_process_mode) {
    if (logger_callback_ != NULL) {
    //   try 
      std::string normalizetag = normalizeTag(tag);
      logger_callback_->logFeedback(normalizetag.c_str(), level, message.c_str());
    }
  } else {
    if (logger_callback_ != NULL) {
      std::string subprocesstag = TAG_PREFIX + tag;
      logger_callback_->logFeedback(subprocesstag.c_str(), level, message.c_str());
    }
  }
}

void LogUrlWrite(const std::string& url) {
  if (!logger_callback_ || !logger_callback_->logUrl) {
    return;
  }
  logger_callback_->logUrl(url.c_str());
}

void LoggerCallbackFunction(int severity,
    const std::string& tag, int policy, const std::string& str) {
  if (task_runner_ == nullptr) {
    return;
  }

  if (!task_runner_->BelongsToCurrentThread()) {
    task_runner_->PostTask(FROM_HERE, base::BindOnce(
                           &LoggerCallbackFunction,
                           severity, tag, policy, str));
    return;
  }
  LogType logtype = LOGTYPE_VERBOSE;
  LogLevel loglevel = LEVEL_DEBUG;
  switch (policy) {
    case logging::LOGGING_FEEDBACK:
      logtype = LOGTYPE_FEEDBACK;
      break;
    case logging::LOGGING_URL:
      logtype = LOGTYPE_URL;
      break;
  }
  if (logtype == LOGTYPE_FEEDBACK) {
    switch (severity) {
      case logging::PRIORITY_DEBUG:
        loglevel = LEVEL_DEBUG;
        break;
      case logging::PRIORITY_ERROR:
        loglevel = LEVEL_ERROR;
        break;
      case logging::PRIORITY_INFO:
        loglevel = LEVEL_INFO;
        break;
      case logging::PRIORITY_WARNING:
        loglevel = LEVEL_WARN;
        break;
      case logging::PRIORITY_FATAL:
        loglevel = LEVEL_FATAL;
        break;
    }
    LogFeedbackWrite(tag, loglevel, str);
  } else if (logtype == LOGTYPE_URL) {
    LogUrlWrite(str);
  }
}

void SetLoggerCallback(std::shared_ptr<NWebLoggerCallback> loggerCallback) {
  task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  logger_callback_ = loggerCallback;
  logging::SetLoggerCallbackToBase(&LoggerCallbackFunction);
}

// static
// report renderer log.
void ReportRendererLog(int policy, const std::string& msg) {
  LoggerCallbackFunction(logging::PRIORITY_INFO, "subprocess", policy, msg);
}

}
}
