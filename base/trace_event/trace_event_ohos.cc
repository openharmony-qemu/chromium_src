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

bool IsBytraceEnable() {
  static bool traceStatus = false;
  static time_t lastTime = 0;
  time_t nowTime;
  nowTime = time(0);
  if (nowTime == lastTime) {
    return traceStatus;
  }
  lastTime = nowTime;
  traceStatus = OhosAdapterHelper::GetInstance()
                .GetHiTraceAdapterInstance()
                .IsHiTraceEnable();
  return traceStatus;
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
