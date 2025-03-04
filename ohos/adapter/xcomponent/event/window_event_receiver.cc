// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <js_native_api.h>
#include <js_native_api_types.h>

#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/window/window_common.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

namespace ohos::adapter::xcomponent {

void OnWindowInitSize(const aki::Value window_rect,
                      const aki::Value drawable_rect) {
  WindowRect rect;
  rect.top = window_rect["top"].As<int>();
  rect.left = window_rect["left"].As<int>();
  rect.width = window_rect["width"].As<int64_t>();
  rect.height = window_rect["height"].As<int64_t>();

  WindowRect content_rect;
  content_rect.top = drawable_rect["top"].As<int>();
  content_rect.left = drawable_rect["left"].As<int>();
  content_rect.width = drawable_rect["width"].As<int64_t>();
  content_rect.height = drawable_rect["height"].As<int64_t>();

  WindowAdapter::GetInstance().SetInitialBounds(rect, content_rect);
}

void OnWindowRectChange(const std::string& xcomponent_id,
                        const aki::Value window_size,
                        RectChangeReason reason) {
  TRACE_EVENT_1("OnWindowRectChange", "widget_id", xcomponent_id);

  bool handle = false;
  switch (reason) {
    case RectChangeReason::MAXIMIZE:
    case RectChangeReason::RECOVER:
      handle = true;
      break;
    default:
      break;
  }

  if (!handle) {
    return;
  }

  auto event = std::make_shared<WindowRectChangeEvent>();
  event->reason = reason;
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnWindowSizeChange(const std::string& xcomponent_id,
                        const aki::Value window_size) {
  TRACE_EVENT_1("OnWindowSizeChange", "widget_id", xcomponent_id);
  auto event = std::make_shared<WindowSizeChangeEvent>();
  event->top = window_size["top"].As<int>();
  event->left = window_size["left"].As<int>();
  event->width = window_size["width"].As<int64_t>();
  event->height = window_size["height"].As<int64_t>();
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnWindowEvent(const std::string& xcomponent_id, WindowEventType type) {
  TRACE_EVENT_0("OnWindowEvent");

  bool handle = false;
  switch (type) {
    case WindowEventType::WINDOW_SHOWN:
    case WindowEventType::WINDOW_HIDDEN:
      handle = true;
      break;
    default:
      break;
  }

  if (!handle) {
    return;
  }
  auto event = std::make_shared<WindowEvent>(type);
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnRequestCloseWindow(const std::string& xcomponent_id) {
  TRACE_EVENT_0("OnRequestCloseWindow");
  WindowAdapter::GetInstance().RequestCloseWindow(xcomponent_id);
}

void OnWindowVisibilityChange(const std::string& xcomponent_id,
                              bool window_visible) {
  TRACE_EVENT_1("OnWindowVisibilityChange", "widget_id", xcomponent_id);
  WindowEventType type = window_visible ? WindowEventType::WINDOW_VISIBLE
                                        : WindowEventType::WINDOW_OCCLUDED;
  auto event = std::make_shared<WindowEvent>(type);
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(OnWindowInitSize);
  JSBIND_FUNCTION(OnWindowRectChange);
  JSBIND_FUNCTION(OnWindowSizeChange);
  JSBIND_FUNCTION(OnWindowEvent);
  JSBIND_FUNCTION(OnRequestCloseWindow);
  JSBIND_FUNCTION(OnWindowVisibilityChange);
}

}  // namespace ohos::adapter::xcomponent
