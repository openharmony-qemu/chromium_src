// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/window/sub_window_adapter.h"

#include <chrono>
#include <cstdint>
#include <future>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"

namespace ohos::adapter::window {

SubWindowAdapter& SubWindowAdapter::GetInstance() {
  static SubWindowAdapter helper;
  return helper;
}

std::string SubWindowAdapter::ReuseSubWindow(const NewWindowParam& param) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.ReuseSubWindow")) {
    return func->Invoke<std::string>(param);
  }
  return std::string();
}
 
void SubWindowAdapter::Create(const NewWindowParam& param) {
  TRACE_EVENT_0("SubWindowAdapter::Create");
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Create")) {
    func->Invoke<void>(param);
    return;
  }
  LOGE("[ohoswindow] SubWindowAdapter::Create error.");
}

void SubWindowAdapter::Cancel(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Cancel")) {
    func->Invoke<void>(id);
  }
}

void SubWindowAdapter::Show(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Show")) {
    func->Invoke<void>(id);
    return;
  }
  LOGW("[ohoswindow] SubWindowAdapter::Show error.");
}

void SubWindowAdapter::Hide(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Hide")) {
    func->Invoke<void>(id);
  }
}

void SubWindowAdapter::SetBounds(const std::string& id, const WindowRect& rect) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.SetBounds")) {
    func->Invoke<void>(id, rect);
  }
}
}  // namespace ohos::adapter::window
