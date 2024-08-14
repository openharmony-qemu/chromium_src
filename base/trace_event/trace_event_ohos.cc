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

#include "base/trace_event/trace_event_ohos.h"

#include <chrono>
#include <time.h>

#include "base/logging.h"
#include "ohos_adapter_helper.h"

using OHOS::NWeb::OhosAdapterHelper;
constexpr char DEBUG_CATEGORY[] = "disabled-";
static bool traceDebugStatus = false;

class TraceObserver : public OHOS::NWeb::SystemPropertiesObserver {
  public:
    TraceObserver() = default;
    ~TraceObserver() override = default;

    void PropertiesUpdate(const char* value) override {
      traceDebugStatus = OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance()
                         .GetTraceDebugEnable();
    }
};
std::unique_ptr<TraceObserver> traceObserver;
void StartObserveTraceEnable() {
  traceObserver = std::make_unique<TraceObserver>();
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                    .GetSystemPropertiesInstance();
  system_properties_adapter.AttachSysPropObserver(OHOS::NWeb::PropertiesKey::PROP_DEBUG_TRACE,
    traceObserver.get());

  traceDebugStatus = OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance()
                      .GetTraceDebugEnable();
}

bool IsOHOSBytraceEnable() {
  return OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance()
         .IsHiTraceEnable() || OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance()
         .IsACETraceEnable();
}

static bool IsDebugCategory(const char* a, const char* b) {
  for (; *a != '\0' && *b != '-'; ++a, ++b) {
    if (*a != *b) {
      return false;
    }
  }
  return *a == *b;
}

bool IsCategoryEnable(const char *category_group) {
  if (!OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance()
                      .IsHiTraceEnable()) {
    return false;
  }

  if (category_group == nullptr) {
    return false;
  }

  if (traceDebugStatus || !IsDebugCategory(category_group, DEBUG_CATEGORY)) {
    return true;
  }
  return false;
}

BytraceArg GetArg(double i) {
  BytraceArg arg;
  arg.value.as_double = i;
  arg.type = TYPE_NUMBER;
  return arg;
}

BytraceArg GetArg(const char* i) {
  BytraceArg arg;
  arg.value.as_string = i;
  arg.type = TYPE_STRING;
  return arg;
}

std::string GetStringFromArg(const BytraceArg& arg) {
  switch (arg.type) {
    case TYPE_NUMBER: {
      return std::to_string(arg.value.as_double);
    }
    case TYPE_STRING: {
      return arg.value.as_string;
    }
    default: {
      return "UnsupportArgType";
    }
  }
}

std::string GetStringWithArgs(const std::string& name) {
  return name;
}

void StartBytrace(const std::string& value) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace(value);
}

void FinishBytrace() {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishTrace();
}

void StartOHOSBytrace(const std::string& value) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace(value);
}

void FinishOHOSBytrace() {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishOHOSTrace();
}

void StartAsyncBytrace(const std::string& value, int32_t taskId) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance()
    .StartAsyncTrace(value, taskId);
}

void FinishAsyncBytrace(const std::string& value, int32_t taskId) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance()
    .FinishAsyncTrace(value, taskId);
}

void CountBytrace(const std::string& name, int64_t count) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountTrace(name, count);
}

void CountOHOSBytrace(const std::string& name, int64_t count) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountOHOSTrace(name, count);
}

ScopedBytrace::ScopedBytrace(const std::string& proc) : proc_(proc) {
}

void ScopedBytrace::SendTraceEvent(const std::string& data) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace(data);
}

ScopedBytrace::ScopedBytrace() {}

ScopedBytrace::~ScopedBytrace() {
  if (IsCategoryEnable(!proc_.empty() ? proc_.c_str() : nullptr)) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishTrace();
  }
}

ScopedOHOSBytrace::ScopedOHOSBytrace(const std::string& proc) : proc_(proc) {
}

void ScopedOHOSBytrace::SendOHOSTraceEvent(const std::string& data) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace(data);
}

ScopedOHOSBytrace::ScopedOHOSBytrace() {}

ScopedOHOSBytrace::~ScopedOHOSBytrace() {
  if (IsOHOSBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishOHOSTrace();
  }
}
