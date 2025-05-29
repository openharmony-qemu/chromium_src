// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/window/system_floating_window_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::window {

template <class C>
struct ClassDefinerRect {
  ClassDefinerRect();
  aki::ClassDefiner<WindowRect> definer;
};

static struct ClassDefinerRect<WindowRect> classDefinerRect;

template <class C>
ClassDefinerRect<C>::ClassDefinerRect() : definer("WindowBound") {
  definer.AddProperty("left", &C::left);
  definer.AddProperty("top", &C::top);
  definer.AddProperty("width", &C::width);
  definer.AddProperty("height", &C::height);
}

SystemFloatingWindowAdapter& SystemFloatingWindowAdapter::GetInstance() {
  static SystemFloatingWindowAdapter instance;
  return instance;
}

void SystemFloatingWindowAdapter::Create(const NewWindowParam& param) {
  auto jsFunc =
      ohos::adapter::GetJSFunction("SystemFloatingWindow.CreateWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(param);
  }
}

void SystemFloatingWindowAdapter::Close(int32_t id) {
  auto jsFunc =
      ohos::adapter::GetJSFunction("SystemFloatingWindow.CloseWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void SystemFloatingWindowAdapter::SetBounds(int32_t id,
                                            const WindowRect& rect,
                                            ChangeSizeCallback callback) {
  auto jsFunc = ohos::adapter::GetJSFunction("SystemFloatingWindow.SetBounds");
  if (jsFunc) {
    jsFunc->Invoke<void>(id, rect, callback);
  }
}

void SystemFloatingWindowAdapter::SetWindowLimits(int32_t min_width,
                                                  int32_t min_height,
                                                  int32_t max_width,
                                                  int32_t max_height,
                                                  int32_t id) {
  auto jsFunc =
      ohos::adapter::GetJSFunction("SystemFloatingWindow.SetWindowLimits");
  if (jsFunc) {
    jsFunc->Invoke<void>(min_width, min_height, max_width, max_height, id);
  }
}

}  // namespace ohos::adapter::window
