// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/window/app_window_adapter.h"

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

AppWindowAdapter& AppWindowAdapter::GetInstance() {
  static AppWindowAdapter instance;
  return instance;
}

void AppWindowAdapter::Create(const NewWindowParam& param) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.CreateWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(param);
  }
}

void AppWindowAdapter::Close(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.CloseWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::Show(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.ShowWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::Hide(int32_t id) {}

void AppWindowAdapter::Activate(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.ActivateWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::SetFullscreen(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetFullscreen");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::SetBounds(int32_t id,
                                 const WindowRect& rect,
                                 ChangeSizeCallback callback) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetBounds");
  if (jsFunc) {
    jsFunc->Invoke<void>(id, rect, callback);
  }
}

void AppWindowAdapter::SetEnabled(bool enable, int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetEnabled");
  if (jsFunc) {
    jsFunc->Invoke<void>(enable, id);
  }
}

void AppWindowAdapter::Maximize(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.Maximize");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::UnMaximize(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.UnMaximize");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::Minimize(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.Minimize");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::SetWindowLimits(int32_t min_width,
                                       int32_t min_height,
                                       int32_t max_width,
                                       int32_t max_height,
                                       int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetWindowLimits");
  if (jsFunc) {
    jsFunc->Invoke<void>(min_width, min_height, max_width, max_height, id);
  }
}

void AppWindowAdapter::SetTitle(const std::string& title, int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetTitle");
  if (jsFunc) {
    jsFunc->Invoke<void>(title, id);
  }
}

JSBIND_CLASS(NewWindowParam) {
  JSBIND_PROPERTY(id);
  JSBIND_PROPERTY(left);
  JSBIND_PROPERTY(top);
  JSBIND_PROPERTY(width);
  JSBIND_PROPERTY(height);
  JSBIND_PROPERTY(hide_title_bar);
  JSBIND_PROPERTY(using_system_floating_window);
}

}  // namespace ohos::adapter::window
