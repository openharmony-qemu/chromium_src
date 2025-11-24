/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BASE_TRACE_EVENT_TRACE_EVENT_OHOS_H
#define BASE_TRACE_EVENT_TRACE_EVENT_OHOS_H

#include <string>
#include <utility>

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_DFX_TRACING)
bool IsOHOSBytraceEnable();
#endif

static bool isHiTraceEnable{false};
static bool isACETraceEnable{false};
void StartObserveTraceEnable();
bool IsBytraceEnable();
bool IsCategoryEnable(const char* category_group);
void StartBytrace(const std::string& value);
void FinishBytrace();
void StartAsyncBytrace(const std::string& value, int32_t taskId);
void FinishAsyncBytrace(const std::string& value, int32_t taskId);
void CountBytrace(const std::string& name, int64_t count);

#if BUILDFLAG(ARKWEB_DFX_TRACING)
void StartOHOSBytrace(const std::string& value);
void FinishOHOSBytrace();
void CountOHOSBytrace(const std::string& name, int64_t count);
#endif

class ScopedBytrace {
 public:
  ScopedBytrace(const std::string& proc);
  ScopedBytrace();
  ~ScopedBytrace();

  static void SendTraceEvent(const std::string& data);

 private:
  std::string proc_;
};

#if BUILDFLAG(ARKWEB_DFX_TRACING)
class ScopedOHOSBytrace {
 public:
  ScopedOHOSBytrace(const std::string& proc);
  ScopedOHOSBytrace();
  ~ScopedOHOSBytrace();

  static void SendOHOSTraceEvent(const std::string& data);

 private:
  std::string proc_;
};
#endif  // BUILDFLAG(ARKWEB_DFX_TRACING)

#define OHOS_BY_TRACE_CONNENCT(a, b) a##b
#define OHOS_BY_TRACE_NAME2(a, b) OHOS_BY_TRACE_CONNENCT(a, b)
#define OHOS_BY_TRACE_NAME(a) OHOS_BY_TRACE_NAME2(a, __LINE__)

#define BYTRACE_SCOPED_INIT(category_group)  \
  ScopedBytrace OHOS_BY_TRACE_NAME(bytrace)( \
      category_group != nullptr ? category_group : "disable-")

#define BYTRACE_SCOPED_TRACE_EVENT(name) ScopedBytrace::SendTraceEvent(name)

#if BUILDFLAG(ARKWEB_DFX_TRACING)
#define OHOS_BYTRACE_SCOPED_INIT() ScopedOHOSBytrace OHOS_BY_TRACE_NAME(bytrace)

#define OHOS_BYTRACE_SCOPED_TRACE_EVENT(name) \
  ScopedOHOSBytrace::SendOHOSTraceEvent(name)
#endif  // BUILDFLAG(ARKWEB_DFX_TRACING)

#endif  // BASE_TRACE_EVENT_TRACE_EVENT_OHOS_H
