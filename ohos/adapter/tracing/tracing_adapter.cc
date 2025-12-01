// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/tracing/tracing_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/tracing/common_event/common_event_wrapper.h"

namespace ohos::adapter::tracing {

namespace {

std::atomic<bool> g_trace_enabled = false;
const std::string kChromiumTracingStart = "chromium_tracing_start";
const std::string kChromiumTracingStop = "chromium_tracing_stop";
const std::string kChromiumUpdateConfig = "chromium_update_config";
const int32_t kChromiumTracingStartCode = 0;
const int32_t kChromiumTracingStopCode = 1;
const int32_t kChromiumUpdateConfigCode = 2;

const std::map<std::string, int32_t> kEventCodeMap = {
    {kChromiumTracingStart, kChromiumTracingStartCode},
    {kChromiumTracingStop, kChromiumTracingStopCode},
    {kChromiumUpdateConfig, kChromiumUpdateConfigCode}
};

}  // namespace

TracingAdapter& TracingAdapter::GetInstance() {
  static TracingAdapter helper;
  return helper;
}

void TracingAdapter::RegisterTracingCallback(TraceControllerCallback callback) {
  LOGI("RegisterTracingCallback ");
  callback_ = callback;
  common_event::Subscriber::Instance().RegisterEventListener(
      [](const common_event::EventData& data) {
        auto callback = TracingAdapter::GetInstance().GetTracingCallback();
        LOGI("recv common_event %{public}s", data.ToString().c_str());
        if (callback) {
          // ignore the event from other app, just handle the event from hdc.
          if (!data.bundle_name.empty()) {
            return;
          }
          if (data.event == kChromiumTracingStart) {
            callback(kEventCodeMap.at(kChromiumTracingStart), data.data_string);
          } else if (data.event == kChromiumTracingStop) {
            callback(kEventCodeMap.at(kChromiumTracingStop), data.data_string);
          } else if (data.event == kChromiumUpdateConfig) {
            callback(kEventCodeMap.at(kChromiumUpdateConfig), data.data_string);
          }
        }
      });
  common_event::Subscriber::Instance().Subscribe(
      {kChromiumTracingStart, kChromiumTracingStop, kChromiumUpdateConfig});
}

void TracingAdapter::UnregisterTracingCallback() {
  common_event::Subscriber::Instance().Unsubscribe();
}

void TracingAdapter::EnableAdapterTrace() {
  g_trace_enabled = true;
}

void TracingAdapter::DisableAdapterTrace() {
  g_trace_enabled = false;
}

bool TracingAdapter::IsAdapterTraceEnabled() {
  return g_trace_enabled;
}

TraceControllerCallback TracingAdapter::GetTracingCallback() {
  return callback_;
}
}  // namespace ohos::adapter::tracing
