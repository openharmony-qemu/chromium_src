// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace(
      value);
}

void FinishBytrace() {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishTrace();
}

void StartAsyncBytrace(const std::string& value, int32_t taskId) {
  OhosAdapterHelper::GetInstance()
      .GetHiTraceAdapterInstance()
      .StartAsyncTrace(value, taskId);
}

void FinishAsyncBytrace(const std::string& value, int32_t taskId) {
  OhosAdapterHelper::GetInstance()
      .GetHiTraceAdapterInstance()
      .FinishAsyncTrace(value, taskId);
}

void CountBytrace(const std::string& name, int64_t count) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountTrace(
      name, count);
}

ScopedBytrace::ScopedBytrace(const std::string& proc) : proc_(proc) {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace(
      proc_);
}

void ScopedBytrace::SendTraceEvent(const std::string& data)  {
  OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace(data);
}

ScopedBytrace::ScopedBytrace() {}

ScopedBytrace::~ScopedBytrace() {
  if (IsBytraceEnable()) {
     OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishTrace();
  }
}
