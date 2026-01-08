/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
LoggerCallbackFunction g_logger_callback = nullptr;
void SetLoggerCallbackToBase(LoggerCallbackFunction loggerCallback) {
  g_logger_callback = loggerCallback;
}

const char kNetwork[] = "[WebNetwork]";
const char kNavigation[] = "[WebNavigation]";
const char kLoadTracker[] = "[WebLoadTracker]";
const char kHttpsUpgrades[] = "[WebHttpsUpgrades]";

bool IsEnableLoggerReport() {
  static bool is_enable = false;
  if (is_enable) {
    return true;
  }
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line == nullptr) {
    return false;
  }
  if (command_line->HasSwitch(switches::kEnableLoggerReport)) {
    is_enable = true;
    return true;
  }
  return false;
}

LogMessage::LogMessage(const char* file,
                       int line,
                       LogSeverity severity,
                       LogPriority priority)
    : severity_(severity), file_(file), line_(line), priority_(priority) {
  Init(file, line);
}

void LogMessage::ArkWebLoggingSeverity(const std::string& str_newline) {
  LogSeverity policy = LOGGING_VERBOSE;
  switch (severity_) {
    case LOGGING_FEEDBACK:
      policy = LOGGING_FEEDBACK;
      break;
    case LOGGING_URL:
      policy = LOGGING_URL;
      priority_ = PRIORITY_INFO;
      break;
  }
  g_logger_callback(priority_, ohos_tag_, policy, str_newline);
}
#endif
