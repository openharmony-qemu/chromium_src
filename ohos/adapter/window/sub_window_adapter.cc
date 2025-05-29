// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/window/sub_window_adapter.h"

#include <chrono>
#include <cstdint>
#include <future>
#include <sstream>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"

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

namespace {
std::string GetRGB(uint32_t argb) {
  auto rgb = argb & 0xFFFFFF;
  std::stringstream tmp;
  tmp << "#" << std::hex << rgb;
  return tmp.str();
}
}  // namespace
SubWindowAdapter& SubWindowAdapter::GetInstance() {
  static SubWindowAdapter helper;
  return helper;
}

bool SubWindowAdapter::Create(const std::string& parent_id,
                              const std::string& id,
                              const WindowRect& rect,
                              const uint32_t color_argb,
                              std::string& create_id) {
  TRACE_EVENT_0("SubWindowAdapter::Create");
  std::promise<bool> promise;
  std::function<void(bool, const std::string&)> callback =
      [&](bool ready, const std::string& ret_id) {
        create_id = ret_id;
        promise.set_value(ready);
      };

  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Create")) {
    func->Invoke<void>(parent_id, id, rect, GetRGB(color_argb), callback);
    auto future = promise.get_future();
    auto status = future.wait_for(std::chrono::seconds(3));
    if (status == std::future_status::timeout) {
      LOGE("SubWindowAdapter::Show timeout");
      return false;
    }
    bool result = future.get();
    return result;
  }
  LOGE("SubWindowAdapter::Show error");
  return false;
}

void SubWindowAdapter::Cancel(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Cancel")) {
    func->Invoke<void>(id);
  }
}

void SubWindowAdapter::Show(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Show")) {
    func->Invoke<void>(id);
  }
}

void SubWindowAdapter::Hide(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Hide")) {
    func->Invoke<void>(id);
  }
}

void SubWindowAdapter::SetBounds(const std::string& id,
                                 const WindowRect& rect) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.SetBounds")) {
    func->Invoke<void>(id, rect);
  }
}
}  // namespace ohos::adapter::window
