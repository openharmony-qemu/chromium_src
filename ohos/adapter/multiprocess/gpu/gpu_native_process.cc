// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu_native_process.h"

#include <native_window/external_window.h>
#include <unistd.h>

#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

using namespace ohos::adapter::xcomponent;

namespace ohos::adapter::multiprocess {

GpuNativeProcess& GpuNativeProcess::GetInstance() {
  static GpuNativeProcess instance;
  return instance;
}

int GpuNativeProcess::GetPid() {
  return getpid();
}

void GpuNativeProcess::AddWindow(const std::string& window_id, void* window) {
  WindowAdapter::GetInstance().AddWindow(window_id, window);
}

void GpuNativeProcess::RemoveWindow(const std::string& window_id) {
  auto window = WindowAdapter::GetInstance().GetWindow(window_id);
  WindowAdapter::GetInstance().RemoveWindow(window_id);
  if (window) {
    // The native window added by the gpu process needs to be released.
    OH_NativeWindow_DestroyNativeWindow((OHNativeWindow*)window);
  }
}

void GpuNativeProcess::SetWindowWidget(void* window, int widget_id) {
  WindowAdapter::GetInstance().SetWindowWidget(window, widget_id);
}

void GpuNativeProcess::SetInitializeResult(bool result) {
  initialize_success_.set_value(result);
}

bool GpuNativeProcess::GetInitializeResult() {
  return initialize_success_.get_future().get();
}
}  // namespace ohos::adapter::multiprocess
