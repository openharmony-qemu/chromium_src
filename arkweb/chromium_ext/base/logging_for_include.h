/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef LOGGING_FOR_INCLUDE_H_
#define LOGGING_FOR_INCLUDE_H_

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
typedef void (*LoggerCallbackFunction)(int severity,
                                       const std::string& tag,
                                       int policy,
                                       const std::string& str);
BASE_EXPORT void SetLoggerCallbackToBase(LoggerCallbackFunction loggerCallback);

using LogPriority = int;
constexpr LogPriority PRIORITY_INFO = 0;
constexpr LogPriority PRIORITY_WARNING = 1;
constexpr LogPriority PRIORITY_ERROR = 2;
constexpr LogPriority PRIORITY_FATAL = 3;
constexpr LogPriority PRIORITY_DEBUG = 4;
constexpr LogSeverity LOGGING_FEEDBACK = 6;
constexpr LogSeverity LOGGING_URL = 7;
constexpr LogSeverity LOGGING_MAX = 9;  // 后续新增POLICY时需要同时将该变量+1

BASE_EXPORT extern const char kNetwork[];
BASE_EXPORT extern const char kNavigation[];
BASE_EXPORT extern const char kLoadTracker[];
BASE_EXPORT extern const char kHttpsUpgrades[];
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)

#define COMPACT_ARKWEB_LOG_EX_DEBUG(ClassName, ...)                  \
  ::logging::ClassName(LOGGING_TAG __FILE__, __LINE__, ::logging::LOGGING_DEBUG, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_INFO(ClassName, ...)                  \
  ::logging::ClassName(LOGGING_TAG __FILE__, __LINE__, ::logging::LOGGING_INFO, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_WARNING(ClassName, ...)                  \
  ::logging::ClassName(LOGGING_TAG __FILE__, __LINE__, ::logging::LOGGING_WARNING, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_ERROR(ClassName, ...)                  \
  ::logging::ClassName(LOGGING_TAG __FILE__, __LINE__, ::logging::LOGGING_ERROR, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_FATAL(ClassName, ...)                  \
  ::logging::ClassName(LOGGING_TAG __FILE__, __LINE__, ::logging::LOGGING_FATAL, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_DFATAL(ClassName, ...)                  \
  ::logging::ClassName(LOGGING_TAG __FILE__, __LINE__, ::logging::LOGGING_DFATAL, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_DCHECK(ClassName, ...)                  \
  ::logging::ClassName(LOGGING_TAG __FILE__, __LINE__, ::logging::LOGGING_DCHECK, \
                       ##__VA_ARGS__)

#define COMPACT_ARKWEB_LOG_DEBUG   COMPACT_ARKWEB_LOG_EX_DEBUG(LogMessage)
#define COMPACT_ARKWEB_LOG_INFO    COMPACT_ARKWEB_LOG_EX_INFO(LogMessage)
#define COMPACT_ARKWEB_LOG_WARNING COMPACT_ARKWEB_LOG_EX_WARNING(LogMessage)
#define COMPACT_ARKWEB_LOG_ERROR   COMPACT_ARKWEB_LOG_EX_ERROR(LogMessage)
#define COMPACT_ARKWEB_LOG_FATAL   COMPACT_ARKWEB_LOG_EX_FATAL(LogMessage)
#define COMPACT_ARKWEB_LOG_DFATAL  COMPACT_ARKWEB_LOG_EX_DFATAL(LogMessage)
#define COMPACT_ARKWEB_LOG_DCHECK  COMPACT_ARKWEB_LOG_EX_DCHECK(LogMessage)

#define COMPACT_ARKWEB_LOG_EX_RENDERER(ClassName, ...)                  \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOGGING_RENDERER, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_FEEDBACK(ClassName, ...)                  \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOGGING_FEEDBACK, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_URL(ClassName, ...)                  \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOGGING_URL, \
                       ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_FEEDBACK_INFO(ClassName, ...)             \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOGGING_FEEDBACK, \
                       ::logging::PRIORITY_INFO, ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_FEEDBACK_DEBUG(ClassName, ...)            \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOGGING_FEEDBACK, \
                       ::logging::PRIORITY_DEBUG, ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_FEEDBACK_WARNING(ClassName, ...)          \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOGGING_FEEDBACK, \
                       ::logging::PRIORITY_WARNING, ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_FEEDBACK_ERROR(ClassName, ...)            \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOGGING_FEEDBACK, \
                       ::logging::PRIORITY_ERROR, ##__VA_ARGS__)
#define COMPACT_ARKWEB_LOG_EX_FEEDBACK_FATAL(ClassName, ...)            \
  ::logging::ClassName(__FILE__, __LINE__, ::logging::LOGGING_FEEDBACK, \
                       ::logging::PRIORITY_FATAL, ##__VA_ARGS__)
#endif  // ARKWEB_LOGGER_REPORT

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#define COMPACT_ARKWEB_LOG_RENDERER COMPACT_ARKWEB_LOG_EX_RENDERER(LogMessage)
#define COMPACT_ARKWEB_LOG_FEEDBACK COMPACT_ARKWEB_LOG_EX_FEEDBACK(LogMessage)
#define COMPACT_ARKWEB_LOG_URL COMPACT_ARKWEB_LOG_EX_URL(LogMessage)
#define COMPACT_ARKWEB_LOG_FEEDBACK_INFO \
  COMPACT_ARKWEB_LOG_EX_FEEDBACK_INFO(LogMessage)
#define COMPACT_ARKWEB_LOG_FEEDBACK_WARNING \
  COMPACT_ARKWEB_LOG_EX_FEEDBACK_WARNING(LogMessage)
#define COMPACT_ARKWEB_LOG_FEEDBACK_ERROR \
  COMPACT_ARKWEB_LOG_EX_FEEDBACK_ERROR(LogMessage)
#define COMPACT_ARKWEB_LOG_FEEDBACK_FATAL \
  COMPACT_ARKWEB_LOG_EX_FEEDBACK_FATAL(LogMessage)
#define COMPACT_ARKWEB_LOG_FEEDBACK_DEBUG \
  COMPACT_ARKWEB_LOG_EX_FEEDBACK_DEBUG(LogMessage)
#endif  // ARKWEB_LOGGER_REPORT

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#define BASE_LOG_FEEDBACK(level) \
  LAZY_STREAM(LOG_FEEDBACK_STREAM(level), LOG_IS_ON(level))
#define LOG_FEEDBACK_STREAM(level) COMPACT_ARKWEB_LOG_FEEDBACK_##level.stream()

#define GET_LOG_MACRO(_1, _2, NAME, ...) NAME
#define LOG_FEEDBACK(...) \
  GET_LOG_MACRO(__VA_ARGS__, LOG_FEEDBACK2, LOG_FEEDBACK1)(__VA_ARGS__)
#define LOG_FEEDBACK1(level) BASE_LOG_FEEDBACK(level)
#define LOG_FEEDBACK2(level, module) \
  BASE_LOG_FEEDBACK(level) << ::logging::module << " "
#endif

#endif
