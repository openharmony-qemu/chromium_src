// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/xcomponent/event/window_event_filter_adapter.h"

#include <dlfcn.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/task_runner/main_thread_task_runner.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"

namespace ohos::adapter::window {
using ohos::adapter::device_info::DeviceInfo;

WindowEventFilterAdapter& WindowEventFilterAdapter::GetInstance() {
  static WindowEventFilterAdapter instance;
  return instance;
}
WindowEventFilterAdapter::WindowEventFilterAdapter()
    : tab_dragging_widget_id_(-1),
      window_manager_lib_("native_window_manager") {
  if (!window_manager_lib_.IsLoaded()) {
    LOGE("[OhosTabDrag] native_window_manager library load fail");
    return;
  }
  if (DeviceInfo::SdkApi() >= device_info::SDK_VERSION_15) {
    if (LoadMouseEventFunctions()) {
      can_filter_window_mouse_event_ = true;
    } else {
      LOGE("[OhosTabDrag] LoadMouseEventFunctions fail!");
    }
    if (LoadTouchEventFunctions()) {
      can_filter_window_touch_event_ = true;
    } else {
      LOGE("[OhosTabDrag] LoadTouchEventFunctions fail!");
    }
  }
}

bool WindowEventFilterAdapter::LoadMouseEventFunctions() {
  return window_manager_lib_.LoadFunction(&get_mouse_event_action_func_,
                                          "OH_Input_GetMouseEventAction") &&
         window_manager_lib_.LoadFunction(&get_mouse_event_display_x_func_,
                                          "OH_Input_GetMouseEventDisplayX") &&
         window_manager_lib_.LoadFunction(&get_mouse_event_display_y_func_,
                                          "OH_Input_GetMouseEventDisplayY") &&
         window_manager_lib_.LoadFunction(&get_mouse_event_button_func_,
                                          "OH_Input_GetMouseEventButton") &&
         window_manager_lib_.LoadFunction(&get_mouse_event_action_time_func_,
                                          "OH_Input_GetMouseEventActionTime") &&
         window_manager_lib_.LoadFunction(&get_mouse_event_window_id_func_,
                                          "OH_Input_GetMouseEventWindowId") &&
         window_manager_lib_.LoadFunction(&get_mouse_event_display_id_func_,
                                          "OH_Input_GetMouseEventDisplayId") &&
         window_manager_lib_.LoadFunction(
             &register_mouse_event_filter_func_,
             "OH_NativeWindowManager_RegisterMouseEventFilter") &&
         window_manager_lib_.LoadFunction(
             &un_register_mouse_event_filter_func_,
             "OH_NativeWindowManager_UnregisterMouseEventFilter");
}

bool WindowEventFilterAdapter::LoadTouchEventFunctions() {
  return window_manager_lib_.LoadFunction(&get_touch_event_action_func_,
                                          "OH_Input_GetTouchEventAction") &&
         window_manager_lib_.LoadFunction(&get_touch_event_display_x_func_,
                                          "OH_Input_GetTouchEventDisplayX") &&
         window_manager_lib_.LoadFunction(&get_touch_event_display_y_func_,
                                          "OH_Input_GetTouchEventDisplayY") &&
         window_manager_lib_.LoadFunction(&get_touch_event_action_time_func_,
                                          "OH_Input_GetTouchEventActionTime") &&
         window_manager_lib_.LoadFunction(&get_touch_event_window_id_func_,
                                          "OH_Input_GetTouchEventWindowId") &&
         window_manager_lib_.LoadFunction(&get_touch_event_display_id_func_,
                                          "OH_Input_GetTouchEventDisplayId") &&
         window_manager_lib_.LoadFunction(&get_touch_event_finger_id_func_,
                                          "OH_Input_GetTouchEventFingerId") &&
         window_manager_lib_.LoadFunction(
             &register_touch_event_filter_func_,
             "OH_NativeWindowManager_RegisterTouchEventFilter") &&
         window_manager_lib_.LoadFunction(
             &un_register_touch_event_filter_func_,
             "OH_NativeWindowManager_UnregisterTouchEventFilter");
}

WindowEventFilterAdapter::~WindowEventFilterAdapter() {
  get_mouse_event_action_func_ = nullptr;
  get_mouse_event_display_x_func_ = nullptr;
  get_mouse_event_display_y_func_ = nullptr;
  get_mouse_event_button_func_ = nullptr;
  get_mouse_event_action_time_func_ = nullptr;
  get_mouse_event_window_id_func_ = nullptr;
  register_mouse_event_filter_func_ = nullptr;
  un_register_mouse_event_filter_func_ = nullptr;

  get_touch_event_action_func_ = nullptr;
  get_touch_event_display_x_func_ = nullptr;
  get_touch_event_display_y_func_ = nullptr;
  get_touch_event_action_time_func_ = nullptr;
  get_touch_event_window_id_func_ = nullptr;
  get_touch_event_display_id_func_ = nullptr;
  register_touch_event_filter_func_ = nullptr;
  un_register_touch_event_filter_func_ = nullptr;
}

void WindowEventFilterAdapter::SendMouseEventForTabDrag(
    const int32_t widget_id,
    Input_MouseEvent* window_mouse_event) {
  std::string xcomponent_id =
      xcomponent::WindowAdapter::GetInstance().GetWindowId(widget_id);
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponentBase(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosTabDrag] %{public}s can not get render: %{public}s",
         __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  render->SendWindowMouseEventForTabDrag(window_mouse_event);
}

void WindowEventFilterAdapter::SendMouseEventForTabDragNodeHandle(
    const int32_t widget_id,
    xcomponent::NodeHandleMouseEventData& window_mouse_data) {
  std::string xcomponent_id =
      xcomponent::WindowAdapter::GetInstance().GetWindowId(widget_id);
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponentBase(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosTabDrag] %{public}s can not get render: %{public}s",
         __func__, xcomponent_id.c_str());
    return;
  }
  render->SendWindowMouseEventForTabDragNodeHandle(window_mouse_data);
}

void WindowEventFilterAdapter::SendTouchEventForTabDrag(
    const int32_t widget_id,
    Input_TouchEvent* window_touch_event) {
  std::string xcomponent_id =
      xcomponent::WindowAdapter::GetInstance().GetWindowId(widget_id);
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponentBase(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosTabDrag] %{public}s can not get render: %{public}s",
         __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  render->SendWindowTouchEventForTabDrag(window_touch_event);
}

void WindowEventFilterAdapter::SendTouchEventForTabDragNodeHandle(
    const int32_t widget_id,
    xcomponent::NodeHandleTouchEventData& window_touch_data) {
  std::string xcomponent_id =
      xcomponent::WindowAdapter::GetInstance().GetWindowId(widget_id);
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponentBase(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosTabDrag] %{public}s can not get render: %{public}s",
         __func__, xcomponent_id.c_str());
    return;
  }
  render->SendWindowTouchEventForTabDragNodeHandle(window_touch_data);
}

void WindowEventFilterAdapter::SetDraggingTabWidgetId(int32_t widget_id) {
  LOGI(
      "[OhosTabDrag] WindowEventFilterAdapter::SetDraggingTabWidgetId, "
      "widget_id:%{public}d",
      widget_id);
  tab_dragging_widget_id_.store(widget_id, std::memory_order_release);
}

bool WindowEventFilterAdapter::IsTabDragging() const {
  return GetDraggingTabWidgetId() > 0;
}

int32_t WindowEventFilterAdapter::GetDraggingTabWidgetId() const {
  return tab_dragging_widget_id_.load(std::memory_order_acquire);
}

int32_t WindowEventFilterAdapter::GetTargetWindowIdAfterShiftEvent(
    int32_t origin_window_id) {
  std::shared_lock<std::shared_mutex> lock(window_event_mutext_);
  int32_t target_window_id = -1;
  if (origin_window_id == source_origin_window_id_) {
    target_window_id = source_window_id_;
  } else if (origin_window_id == target_origin_window_id_) {
    target_window_id = tartget_window_id_;
  }
  return target_window_id;
}

void WindowEventFilterAdapter::CacheShiftEventWindowIds(const int32_t source_id,
                                                        const int32_t target_id) {
  std::vector<int32_t> window_ids;
  window_ids.push_back(source_id);
  window_ids.push_back(target_id);
  std::vector<int32_t> origin_window_ids =
      AppWindowAdapter::GetInstance().GetOriginWindowIds(window_ids);
  if (origin_window_ids.size() != window_ids.size()) {
    LOGE(
        "[OhosTabDrag] WindowEventFilterAdapter::CacheShiftEventWindowIds "
        "GetOriginWindowIds fail, "
        "origin_window_ids size:%{public}lu",
        origin_window_ids.size());
    return;
  }
  LOGI(
      "[OhosTabDrag] WindowEventFilterAdapter::CacheShiftEventWindowIds "
      "origin_source-window-id:%{public}d, "
      "origin_target-window-id:%{public}d",
      origin_window_ids[0], origin_window_ids[1]);
  {
    std::unique_lock<std::shared_mutex> lock(window_event_mutext_);
    source_window_id_ = source_id;
    tartget_window_id_ = target_id;
    source_origin_window_id_ = origin_window_ids[0];
    target_origin_window_id_ = origin_window_ids[1];
  }
}

bool WindowEventFilterAdapter::CanShiftTouchEvent() {
  return CanFilterWindowTouchEvent() &&
         DeviceInfo::SdkApi() >= device_info::SDK_VERSION_20;
}

int32_t ConvertInputEventActionFromWindowMouseEvent(
    int32_t window_mouse_action) {
  int32_t mouse_event_action = UI_MOUSE_EVENT_ACTION_UNKNOWN;
  switch (window_mouse_action) {
    case MOUSE_ACTION_MOVE:
      mouse_event_action = UI_MOUSE_EVENT_ACTION_MOVE;
      break;
    case MOUSE_ACTION_BUTTON_DOWN:
      mouse_event_action = UI_MOUSE_EVENT_ACTION_PRESS;
      break;
    case MOUSE_ACTION_BUTTON_UP:
      mouse_event_action = UI_MOUSE_EVENT_ACTION_RELEASE;
      break;
    default:
      LOGI("%{public}s, window_mouse_action is not useful: %{public}d",
           __func__, window_mouse_action);
      break;
  }
  return mouse_event_action;
}

int32_t ConvertInputEventButtonFromWindowMouseEvent(
    int32_t window_mouse_button) {
  int32_t mouse_event_button = UI_MOUSE_EVENT_BUTTON_NONE;
  switch (window_mouse_button) {
    case MOUSE_BUTTON_LEFT:
      mouse_event_button = UI_MOUSE_EVENT_BUTTON_LEFT;
      break;
    case MOUSE_BUTTON_RIGHT:
      mouse_event_button = UI_MOUSE_EVENT_BUTTON_RIGHT;
      break;
    default:
      LOGI("%{public}s, window_mouse_button is not useful: %{public}d",
           __func__, window_mouse_button);
      break;
  }
  return mouse_event_button;
}

void ConvertWindowMouseEventToInputEvent(
    Input_MouseEvent* window_mouse_event,
    xcomponent::NodeHandleMouseEventData& mouse_event_data) {
  WindowEventFilterAdapter& window_event_filter_adapter =
      WindowEventFilterAdapter::GetInstance();
  int32_t window_mouse_action =
      window_event_filter_adapter.GetWindowMouseEventAction(window_mouse_event);
  int32_t display_x = window_event_filter_adapter.GetWindowMouseEventDisplayX(
      window_mouse_event);
  int32_t display_y = window_event_filter_adapter.GetWindowMouseEventDisplayY(
      window_mouse_event);
  int32_t window_mouse_button =
      window_event_filter_adapter.GetWindowMouseEventButton(window_mouse_event);
  int64_t action_time =
      window_event_filter_adapter.GetWindowMouseEventActionTime(
          window_mouse_event);
  int32_t display_id =
      window_event_filter_adapter.GetWindowMouseEventDisplayId(
          window_mouse_event);
  mouse_event_data.screenX = display_x;
  mouse_event_data.screenY = display_y;
  mouse_event_data.button =
      ConvertInputEventButtonFromWindowMouseEvent(window_mouse_button);
  mouse_event_data.action =
      ConvertInputEventActionFromWindowMouseEvent(window_mouse_action);
  // The unit of the mouse event timestamp from the window is millisecond.
  // The unit of the mouse event in the xcomponent is microsecond.
  mouse_event_data.timestamp =
      action_time * WindowEventFilterAdapter::kMicrosecondsUnit;
  mouse_event_data.display_id = display_id;
}

int32_t ConvertTouchEventActionFromWindowTouchEvent(int32_t window_touch_action) {
  int32_t touch_event_action = UI_TOUCH_EVENT_ACTION_CANCEL;
  switch (window_touch_action) {
    case TOUCH_ACTION_MOVE:
      touch_event_action = UI_TOUCH_EVENT_ACTION_MOVE;
      break;
    case TOUCH_ACTION_DOWN:
      touch_event_action = UI_TOUCH_EVENT_ACTION_DOWN;
      break;
    case TOUCH_ACTION_UP:
      touch_event_action = UI_TOUCH_EVENT_ACTION_UP;
      break;
    case TOUCH_ACTION_CANCEL:
      touch_event_action = UI_TOUCH_EVENT_ACTION_CANCEL;
      break;
    default:
      LOGI("%{public}s, window_touch_action is not useful: %{public}d",
           __func__, window_touch_action);
      break;
  }
  return touch_event_action;
}

void ConvertWindowTouchEventToInputEvent(
    Input_TouchEvent* window_touch_event,
    xcomponent::NodeHandleTouchEventData& touch_event_data) {
  WindowEventFilterAdapter& window_event_filter_adapter =
      WindowEventFilterAdapter::GetInstance();
  int32_t window_touch_action =
      window_event_filter_adapter.GetWindowTouchEventAction(window_touch_event);
  int32_t display_x = window_event_filter_adapter.GetWindowTouchEventDisplayX(
      window_touch_event);
  int32_t display_y = window_event_filter_adapter.GetWindowTouchEventDisplayY(
      window_touch_event);
  int32_t window_touch_finger_id =
      window_event_filter_adapter.GetWindowTouchEventFingerId(
          window_touch_event);
  int64_t action_time =
      window_event_filter_adapter.GetWindowTouchEventActionTime(
          window_touch_event);
  int32_t display_id =
      window_event_filter_adapter.GetWindowTouchEventDisplayId(
          window_touch_event);
  touch_event_data.id = window_touch_finger_id;
  touch_event_data.touch_action =
      ConvertTouchEventActionFromWindowTouchEvent(window_touch_action);
  touch_event_data.display_x = display_x;
  touch_event_data.display_y = display_y;
  // The unit of the touch event timestamp from the window is millisecond.
  // The unit of the touch event in the xcomponent is microsecond.
  touch_event_data.timestamp =
      action_time * WindowEventFilterAdapter::kMicrosecondsUnit;
  touch_event_data.display_id = display_id;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
static bool FilterMouseEvent(Input_MouseEvent* mouse_event) {
  WindowEventFilterAdapter& window_event_filter = WindowEventFilterAdapter::GetInstance();
  if (!window_event_filter.CanFilterWindowMouseEvent() ||
      mouse_event == nullptr) {
    return false;
  }
  // When tab dragging, the window mouse event is directly
  // sent to the UI thread of Chromium.
  if (window_event_filter.IsTabDragging()) {
    int32_t origin_window_id =
        window_event_filter.GetWindowMouseEventWindowId(mouse_event);
    int32_t widget_id =
        window_event_filter.GetTargetWindowIdAfterShiftEvent(origin_window_id);
    if (widget_id <= 0) {
      widget_id = window_event_filter.GetDraggingTabWidgetId();
    }
    if (nodeHandle::NodeHandleImpl::GetInstance().IsSupportNodeHandle()) {
      xcomponent::NodeHandleMouseEventData mouse_event_data;
      ConvertWindowMouseEventToInputEvent(mouse_event, mouse_event_data);
      taskRunner::MainThreadTaskRunner::GetInstance().PostTask(
          [widget_id, data = std::move(mouse_event_data)]() mutable {
              auto& window_event_filter = WindowEventFilterAdapter::GetInstance();
              window_event_filter.SendMouseEventForTabDragNodeHandle(
                  widget_id, data);
          }
      );
    } else {
      window_event_filter.SendMouseEventForTabDrag(widget_id, mouse_event);
    }
    return true;
  }
  return false;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
static bool FilterTouchEvent(Input_TouchEvent* touch_event) {
  WindowEventFilterAdapter& window_event_filter = WindowEventFilterAdapter::GetInstance();
  if (!window_event_filter.CanFilterWindowTouchEvent() ||
      touch_event == nullptr) {
    return false;
  }
  // When tab dragging, the window touch event is directly
  // sent to the UI thread of Chromium.
  if (window_event_filter.IsTabDragging()) {
    int32_t origin_window_id =
        window_event_filter.GetWindowTouchEventWindowId(touch_event);
    int32_t widget_id =
        window_event_filter.GetTargetWindowIdAfterShiftEvent(origin_window_id);
    if (widget_id <= 0) {
      widget_id = window_event_filter.GetDraggingTabWidgetId();
    }
    if (nodeHandle::NodeHandleImpl::GetInstance().IsSupportNodeHandle()) {
      xcomponent::NodeHandleTouchEventData touch_event_data;
      ConvertWindowTouchEventToInputEvent(touch_event, touch_event_data);
      taskRunner::MainThreadTaskRunner::GetInstance().PostTask(
          [widget_id, data = std::move(touch_event_data)]() mutable {
              auto& window_event_filter = WindowEventFilterAdapter::GetInstance();
              window_event_filter.SendTouchEventForTabDragNodeHandle(
                  widget_id, data);
          }
      );
    } else {
      window_event_filter.SendTouchEventForTabDrag(widget_id, touch_event);
    }
    return true;
  }
  return false;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void RegisterWindowEventFilter(int32_t origin_window_id) {
  WindowEventFilterAdapter& window_event_filter =
      WindowEventFilterAdapter::GetInstance();
  if (window_event_filter.CanFilterWindowMouseEvent()) {
    window_event_filter.RegisterWindowMouseEventFilterForWindow(
        origin_window_id);
  } else {
    LOGE("[OhosTabDrag] %{public}s, CanFilterWindowMouseEvent false",
         __FUNCTION__);
  }

  if (window_event_filter.CanFilterWindowTouchEvent()) {
    window_event_filter.RegisterWindowTouchEventFilterForWindow(
        origin_window_id);
  } else {
    LOGE("[OhosTabDrag] %{public}s, CanFilterWindowTouchEvent false",
         __FUNCTION__);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void ClearWindowEventFilter(int32_t origin_window_id) {
  WindowEventFilterAdapter& window_event_filter =
      WindowEventFilterAdapter::GetInstance();
  if (window_event_filter.CanFilterWindowMouseEvent()) {
    window_event_filter.UnRegisterWindowMouseEventFilterForWindow(
        origin_window_id);
  } else {
    LOGE("[OhosTabDrag] %{public}s, CanFilterWindowMouseEvent false",
         __FUNCTION__);
  }
  if (window_event_filter.CanFilterWindowTouchEvent()) {
    window_event_filter.UnRegisterWindowTouchEventFilterForWindow(
        origin_window_id);
  } else {
    LOGE("[OhosTabDrag] %{public}s, CanFilterWindowTouchEvent false",
         __FUNCTION__);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventAction(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_action_func_ == nullptr) {
    LOGE("[OhosTabDrag] %{public}s fail, get_mouse_event_action_func_ is null",
         __FUNCTION__);
    return 0;
  }
  return get_mouse_event_action_func_(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventDisplayX(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_display_x_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_mouse_event_display_x_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_mouse_event_display_x_func_(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventDisplayY(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_display_y_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_mouse_event_display_y_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_mouse_event_display_y_func_(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventButton(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_button_func_ == nullptr) {
    LOGE("[OhosTabDrag] %{public}s fail, get_mouse_event_button_func_ is null",
         __FUNCTION__);
    return 0;
  }
  return get_mouse_event_button_func_(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int64_t WindowEventFilterAdapter::GetWindowMouseEventActionTime(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_action_time_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_mouse_event_action_time_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_mouse_event_action_time_func_(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventWindowId(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_window_id_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_mouse_event_window_id_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_mouse_event_window_id_func_(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventDisplayId(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_display_id_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_mouse_event_display_id_func_ is "
        "null", __FUNCTION__);
    return -1;
  }
  return get_mouse_event_display_id_func_(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void WindowEventFilterAdapter::RegisterWindowMouseEventFilterForWindow(
    int32_t origin_window_id) {
  if (!register_mouse_event_filter_func_) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, "
        "register_mouse_event_filter_func_ is null",
        __FUNCTION__);
    return;
  }
  auto result =
      register_mouse_event_filter_func_(origin_window_id, FilterMouseEvent);
  LOGI(
      "[OhosTabDrag] %{public}s fail, "
      "window_id:%{public}d, result:%{public}d",
      __FUNCTION__, origin_window_id, result);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void WindowEventFilterAdapter::UnRegisterWindowMouseEventFilterForWindow(
    int32_t origin_window_id) {
  if (!un_register_mouse_event_filter_func_) {
    LOGE(
        "[OhosTabDrag] %{public}s"
        "un_register_mouse_event_filter_func_ is null",
        __FUNCTION__);
    return;
  }
  auto result = un_register_mouse_event_filter_func_(origin_window_id);
  LOGI(
      "[OhosTabDrag] %{public}s fail, "
      "window_id:%{public}d, result:%{public}d",
      __FUNCTION__, origin_window_id, result);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void WindowEventFilterAdapter::RegisterWindowTouchEventFilterForWindow(
    int32_t origin_window_id) {
  if (!register_touch_event_filter_func_) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, "
        "register_touch_event_filter_func_ is null",
        __FUNCTION__);
    return;
  }
  auto result =
      register_touch_event_filter_func_(origin_window_id, FilterTouchEvent);
  LOGI(
      "[OhosTabDrag] %{public}s fail, "
      "window_id:%{public}d, result:%{public}d",
      __FUNCTION__, origin_window_id, result);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void WindowEventFilterAdapter::UnRegisterWindowTouchEventFilterForWindow(
    int32_t origin_window_id) {
  if (!un_register_touch_event_filter_func_) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, "
        "un_register_touch_event_filter_func_ is null",
        __FUNCTION__);
    return;
  }
  auto result = un_register_touch_event_filter_func_(origin_window_id);
  LOGI(
      "[OhosTabDrag] %{public}s, "
      "window_id:%{public}d, result:%{public}d",
      __FUNCTION__, origin_window_id, result);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowTouchEventAction(
    Input_TouchEvent* window_touch_event) {
  if (get_touch_event_action_func_ == nullptr) {
    LOGE("[OhosTabDrag] %{public}s fail, get_touch_event_action_func_ is null",
         __FUNCTION__);
    return 0;
  }
  return get_touch_event_action_func_(window_touch_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowTouchEventDisplayX(
    Input_TouchEvent* window_touch_event) {
  if (get_touch_event_display_x_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_touch_event_display_x_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_touch_event_display_x_func_(window_touch_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowTouchEventDisplayY(
    Input_TouchEvent* window_touch_event) {
  if (get_touch_event_display_y_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_touch_event_display_y_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_touch_event_display_y_func_(window_touch_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int64_t WindowEventFilterAdapter::GetWindowTouchEventActionTime(
    Input_TouchEvent* window_touch_event) {
  if (get_touch_event_action_time_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_touch_event_action_time_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_touch_event_action_time_func_(window_touch_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowTouchEventWindowId(
    Input_TouchEvent* window_touch_event) {
  if (get_touch_event_window_id_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_touch_event_window_id_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_touch_event_window_id_func_(window_touch_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowTouchEventDisplayId(
    Input_TouchEvent* window_touch_event) {
  if (get_touch_event_display_id_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_touch_event_display_id_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_touch_event_display_id_func_(window_touch_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowTouchEventFingerId(
    Input_TouchEvent* window_touch_event) {
  if (get_touch_event_finger_id_func_ == nullptr) {
    LOGE(
        "[OhosTabDrag] %{public}s fail, get_touch_event_finger_id_func_ is "
        "null", __FUNCTION__);
    return 0;
  }
  return get_touch_event_finger_id_func_(window_touch_event);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(RegisterWindowEventFilter);
  JSBIND_FUNCTION(ClearWindowEventFilter);
}
} // namespace ohos::adapter::window