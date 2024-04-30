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
constexpr uint64_t HITRACE_TAG_NWEB = (1ULL << 24); // nweb trace tag
constexpr uint64_t HITRACE_TAG_OHOS = (1ULL << 30); // ohos trace tag
class TraceObserver : public OHOS::NWeb::SystemPropertiesObserver {
  public:
    TraceObserver() = default;
    ~TraceObserver() override = default;

    void PropertiesUpdate(const char* value) override {
      auto status = std::atol(value);
      isHiTraceEnable = status & HITRACE_TAG_NWEB;
      isOHOSHiTraceEnable = status & HITRACE_TAG_OHOS;
    }
};
std::unique_ptr<TraceObserver> traceObserver;
void StartObserveTraceEnable() {
  traceObserver = std::make_unique<TraceObserver>();
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                    .GetSystemPropertiesInstance();
  system_properties_adapter.AttachSysPropObserver(OHOS::NWeb::PropertiesKey::PROP_HITRACE_ENABLEFLAGS,
    traceObserver.get());
}

bool IsBytraceEnable() {
  return isHiTraceEnable;
}

bool IsOHOSBytraceEnable() {
  return isOHOSHiTraceEnable;
}

bool IsCategoryEnable(const char *category_group) {
  bool traceDebugStatus = false;
  traceDebugStatus = OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance()
                                        .GetTraceDebugEnable();
  if (traceDebugStatus) {
    return true;
  }

  if (category_group == nullptr) {
    return false;
  }

  if (strlen(category_group) < strlen("disabled-by-default-")) {
    return true;
  }

  if (!strncmp(category_group, "disabled-by-default-", strlen("disabled-by-default-"))) {
    return false;
  }
  return true;
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
  if (IsBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace(
        value);
  }
}

void FinishBytrace() {
  if (IsBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishTrace();
  }
}

void StartOHOSBytrace(const std::string& value) {
  if (IsOHOSBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace(
        value);
  }
}

void FinishOHOSBytrace() {
  if (IsOHOSBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishOHOSTrace();
  }
}

void StartAsyncBytrace(const std::string& value, int32_t taskId) {
  if (IsBytraceEnable()) {
    OhosAdapterHelper::GetInstance()
        .GetHiTraceAdapterInstance()
        .StartAsyncTrace(value, taskId);
  }
}

void FinishAsyncBytrace(const std::string& value, int32_t taskId) {
  if (IsBytraceEnable()) {
    OhosAdapterHelper::GetInstance()
        .GetHiTraceAdapterInstance()
        .FinishAsyncTrace(value, taskId);
  }
}

void CountBytrace(const std::string& name, int64_t count) {
  if (IsBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountTrace(
        name, count);
  }
}

void CountOHOSBytrace(const std::string& name, int64_t count) {
  if (IsOHOSBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountOHOSTrace(
        name, count);
  }
}

ScopedBytrace::ScopedBytrace(const std::string& proc) : proc_(proc) {
  if (IsBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace(
        proc_);
  }
}

void ScopedBytrace::SendTraceEvent(const std::string& data) {
  if (IsBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace(data);
  }
}

ScopedBytrace::ScopedBytrace() {}

ScopedBytrace::~ScopedBytrace() {
  if (IsBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishTrace();
  }
}

ScopedOHOSBytrace::ScopedOHOSBytrace(const std::string& proc) : proc_(proc) {
  if (IsOHOSBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace(
        proc_);
  }
}

void ScopedOHOSBytrace::SendOHOSTraceEvent(const std::string& data) {
  if (IsOHOSBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace(data);
  }
}

ScopedOHOSBytrace::ScopedOHOSBytrace() {}

ScopedOHOSBytrace::~ScopedOHOSBytrace() {
  if (IsOHOSBytraceEnable()) {
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishOHOSTrace();
  }
}
