// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/tracing/tracing_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::tracing {

namespace {

std::atomic<bool> g_trace_enabled = false;

}  // namespace

TracingAdapter& TracingAdapter::GetInstance() {
  static TracingAdapter helper;
  return helper;
}

void TracingAdapter::RegisterTracingCallback(TraceControllerCallback callback) {
  if (auto func = GetJSFunction("TracingAdapter.RegisterTracingCallback")) {
    func->Invoke<void>(callback);
  }
}

void TracingAdapter::UnregisterTracingCallback() {
  auto func = GetJSFunction("TracingAdapter.UnregisterTracingCallback");
  if (func) {
    func->Invoke<void>();
  }
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

}  // namespace ohos::adapter::tracing
