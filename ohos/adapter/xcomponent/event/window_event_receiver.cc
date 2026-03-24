// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/window/window_common.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

using WindowStatusType = ohos::adapter::window::WindowStatusType;
namespace ohos::adapter::xcomponent {

void OnWindowInitSize(const aki::Value window_rect,
                      const aki::Value drawable_rect,
                      const int64_t display_id) {
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
  WindowAdapter::GetInstance().SetInitialDisplayId(display_id);
}

void OnWindowStatusChange(const std::string xcomponent_id,
                          WindowStatusType status) {
  TRACE_EVENT_1("OnWindowStatusChange", "widget_id", xcomponent_id);
 
  auto event = std::make_shared<WindowStatusChangeEvent>();
  event->status = status;
  LOGI("[window_event_receiver.cc] OnWindowStatusChange: %{public}s", event->ToString().c_str());
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnWindowInitState(const WindowStatusType state) {
  WindowAdapter::GetInstance().SetInitialState(state);
}

void OnWindowRectChange(const std::string& xcomponent_id,
                        const aki::Value window_size,
                        RectChangeReason reason) {
  TRACE_EVENT_1("OnWindowRectChange", "widget_id", xcomponent_id);
  bool handle;

  switch (reason) {
    case RectChangeReason::MAXIMIZE:
    case RectChangeReason::RECOVER:
    // Events for dragging to resize window.
    case RectChangeReason::DRAG:
    case RectChangeReason::DRAG_END:
    // We receive an undefined reason when
    // entering split screen.
    case RectChangeReason::UNDEFINED:
    // Events for dragging to move window.
    case RectChangeReason::MOVE:
      handle = true;
      break;
    default:
      handle = false;
  }

  if (!handle) {
    return;
  }

  auto event = std::make_shared<WindowRectChangeEvent>();
  event->reason = reason;
  event->top = window_size["top"].As<int>();
  event->left = window_size["left"].As<int>();
  event->width = window_size["width"].As<int64_t>();
  event->height = window_size["height"].As<int64_t>();
  if (reason != RectChangeReason::DRAG && reason != RectChangeReason::MOVE) {
      LOGI("[window_event_receiver.cc] OnWindowRectChange: %{public}s", event->ToString().c_str());
  }
  
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
  LOGI("[window_event_receiver.cc] OnWindowSizeChange: %{public}s", event->ToString().c_str());
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnWindowEvent(const std::string& xcomponent_id,
                   WindowEventType type) {
  TRACE_EVENT_0("OnWindowEvent");

  bool handle = false;
  switch (type) {
    case WindowEventType::WINDOW_SHOWN:
    case WindowEventType::WINDOW_HIDDEN:
    case WindowEventType::WINDOW_CLOSE:
      handle = true;
      break;
    default:
      break;
  }

  if (!handle) {
    return;
  }
  auto event = std::make_shared<WindowEvent>(type);
  LOGI("[window_event_receiver.cc] OnWindowEvent: %{public}s", event->ToString().c_str());
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnWindowVisibilityChange(const std::string& xcomponent_id,
                              bool window_visible) {
  if (WindowAdapter::GetInstance().IsDisableOcclusionFeature()) {
    return;
  }
  TRACE_EVENT_1("OnWindowVisibilityChange", "widget_id", xcomponent_id);
  WindowEventType type = window_visible ? WindowEventType::WINDOW_VISIBLE
                                        : WindowEventType::WINDOW_OCCLUDED;
  auto event = std::make_shared<WindowEvent>(type);
  LOGI("[window_event_receiver.cc] OnWindowVisibilityChange: %{public}s", event->ToString().c_str());
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnKeyboardHeightChange(const std::string& xcomponent_id, int32_t height) {
  TRACE_EVENT_1("OnKeyboardHeightChange", "height", height);
  WindowAdapter::GetInstance().NotifyKeyboardHeightEvent(xcomponent_id, height);
}

void OnCaptionButtonRectChange(const std::string& xcomponent_id,
                               const aki::Value caption_button_rect) {
  TRACE_EVENT_1("OnCaptionButtonRectChange", "widget_id", xcomponent_id);
  auto event = std::make_shared<WindowCaptionButtonRectChangeEvent>();
  event->top = caption_button_rect["top"].As<int>();
  event->right = caption_button_rect["right"].As<int>();
  event->width = caption_button_rect["width"].As<int64_t>();
  event->height = caption_button_rect["height"].As<int64_t>();
  LOGI("[window_event_receiver.cc] OnCaptionButtonRectChange: %{public}s", event->ToString().c_str());
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void SetSystemWindowLimits(const aki::Value window_limits) {
  WindowLimits limits;
  limits.max_width = window_limits["max_width"].As<int>();
  limits.max_height = window_limits["max_height"].As<int>();
  limits.min_width = window_limits["min_width"].As<int>();
  limits.min_height = window_limits["min_height"].As<int>();
  WindowAdapter::GetInstance().SetSystemWindowLimits(limits);
}

void OnDeviceModeChange(const std::string& xcomponent_id,
                        ChangeEventType type,
                        WindowStatusType status) {
  TRACE_EVENT_1("OnDeviceInfoChange", "widget_id", xcomponent_id);
 
  auto event = std::make_shared<DeviceInfoChangeEvent>();
  event->change_event_type_ = type;
  event->status_ = status;
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnWindowDisplayIdChange(const std::string& xcomponent_id,
                             const int64_t display_id) {
  TRACE_EVENT_1("OnWindowDisplayIdChange", "widget_id", xcomponent_id);
  auto event = std::make_shared<WindowDisplayIdChangeEvent>();
  event->display_id = display_id;
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

void OnBackToLastPage(const std::string& xcomponent_id) {
  TRACE_EVENT_1("OnBackToLastPage", "widget_id", xcomponent_id);
  auto event = std::make_shared<BackToLastPageEvent>();
  WindowAdapter::GetInstance().NotifyWindowEvent(xcomponent_id, event);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(OnWindowInitSize);
  JSBIND_FUNCTION(OnWindowStatusChange);
  JSBIND_FUNCTION(OnWindowInitState);
  JSBIND_FUNCTION(OnWindowRectChange);
  JSBIND_FUNCTION(OnWindowSizeChange);
  JSBIND_FUNCTION(OnWindowEvent);
  JSBIND_FUNCTION(OnWindowVisibilityChange);
  JSBIND_FUNCTION(OnKeyboardHeightChange);
  JSBIND_FUNCTION(OnCaptionButtonRectChange);
  JSBIND_FUNCTION(SetSystemWindowLimits);
  JSBIND_FUNCTION(OnDeviceModeChange);
  JSBIND_FUNCTION(OnWindowDisplayIdChange);
  JSBIND_FUNCTION(OnBackToLastPage);
}

}  // namespace ohos::adapter::xcomponent
