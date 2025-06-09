/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/tracing/tracing_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/tracing/common_event/common_event_wrapper.h"

namespace ohos::adapter::tracing {

namespace {

std::atomic<bool> g_trace_enabled = false;
const std::string kChromiumTracingStart = "chromium_tracing_start";
const std::string kChromiumTracingStop = "chromium_tracing_stop";
const int32_t kChromiumTracingStartCode = 0;
const int32_t kChromiumTracingStopCode = 1;

const std::map<std::string, int32_t> kEventCodeMap = {
    {kChromiumTracingStart, kChromiumTracingStartCode},
    {kChromiumTracingStop, kChromiumTracingStopCode},
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
          }
        }
      });
  common_event::Subscriber::Instance().Subscribe(
      {kChromiumTracingStart, kChromiumTracingStop});
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
