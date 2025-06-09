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

#include "ohos/adapter/xcomponent/event/window_event_filter_adapter.h"

#include <dlfcn.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"

namespace ohos::adapter::window {
common::SharedLibrary WindowEventFilterAdapter::window_manager_lib("native_window_manager");
GetMouseEventActionFunc* WindowEventFilterAdapter::get_mouse_event_action_func = nullptr;
GetMouseEventDisplayXFunc* WindowEventFilterAdapter::get_mouse_event_display_x_func = nullptr;
GetMouseEventDisplayYFunc* WindowEventFilterAdapter::get_mouse_event_display_y_func = nullptr;
GetMouseEventButtonFunc* WindowEventFilterAdapter::get_mouse_event_button_func = nullptr;
GetMouseEventActionTimeFunc* WindowEventFilterAdapter::get_mouse_event_action_time_func = nullptr;
GetMouseEventWindowIdFunc* WindowEventFilterAdapter::get_mouse_event_window_id_func = nullptr;
GetMouseEventDisplayIdFunc* WindowEventFilterAdapter::get_mouse_event_display_id_func = nullptr;

WindowEventFilterAdapter& WindowEventFilterAdapter::GetInstance() {
  static WindowEventFilterAdapter instance;
  return instance;
}
WindowEventFilterAdapter::WindowEventFilterAdapter(): tab_dragging_widget_id_(-1) {
  if (!can_filter_window_mouse_event_ && window_manager_lib.IsLoaded() &&
      device_info::DeviceInfo::SdkApi() >= device_info::SDK_VERSION_15) {
    InitMouseEventFunction();
  }
}

void WindowEventFilterAdapter::InitMouseEventFunction() {
    get_mouse_event_action_func =
        window_manager_lib.GetFunction<GetMouseEventActionFunc>("OH_Input_GetMouseEventAction");
    if (!get_mouse_event_action_func) {
      LOGW("WindowEventFilterAdapter::WindowEventFilterAdapter get_mouse_event_action_func fail");
      return;
    }
    get_mouse_event_display_x_func =
        window_manager_lib.GetFunction<GetMouseEventDisplayXFunc>("OH_Input_GetMouseEventDisplayX");
    if (!get_mouse_event_display_x_func) {
      LOGW("WindowEventFilterAdapter::WindowEventFilterAdapter get_mouse_event_display_x_func fail");
      return;
    }
    get_mouse_event_display_y_func =
        window_manager_lib.GetFunction<GetMouseEventDisplayYFunc>("OH_Input_GetMouseEventDisplayY");
    if (!get_mouse_event_display_y_func) {
      LOGW("WindowEventFilterAdapter::WindowEventFilterAdapter get_mouse_event_display_y_func fail");
      return;
    }
    get_mouse_event_button_func =
        window_manager_lib.GetFunction<GetMouseEventButtonFunc>("OH_Input_GetMouseEventButton");
    if (!get_mouse_event_button_func) {
      LOGW("WindowEventFilterAdapter::WindowEventFilterAdapter get_mouse_event_button_func fail");
      return;
    }
    get_mouse_event_action_time_func =
        window_manager_lib.GetFunction<GetMouseEventActionTimeFunc>("OH_Input_GetMouseEventActionTime");
    if (!get_mouse_event_action_time_func) {
      LOGW("WindowEventFilterAdapter::WindowEventFilterAdapter get_mouse_event_action_time_func fail");
      return;
    }
    get_mouse_event_window_id_func =
        window_manager_lib.GetFunction<GetMouseEventWindowIdFunc>("OH_Input_GetMouseEventWindowId");
    if (!get_mouse_event_window_id_func) {
      LOGW("WindowEventFilterAdapter::WindowEventFilterAdapter get_mouse_event_window_id_func fail");
      return;
    }
    get_mouse_event_display_id_func =
        window_manager_lib.GetFunction<GetMouseEventDisplayIdFunc>("OH_Input_GetMouseEventDisplayId");
    if (!get_mouse_event_display_id_func) {
      LOGW("WindowEventFilter::WindowEventFilter get_mouse_event_display_id_func fail");
      return;
    }
    can_filter_window_mouse_event_ = true;
}

WindowEventFilterAdapter::~WindowEventFilterAdapter() {
  WindowEventFilterAdapter::get_mouse_event_action_func = nullptr;
  WindowEventFilterAdapter::get_mouse_event_display_x_func = nullptr;
  WindowEventFilterAdapter::get_mouse_event_display_y_func = nullptr;
  WindowEventFilterAdapter::get_mouse_event_button_func = nullptr;
  WindowEventFilterAdapter::get_mouse_event_action_time_func = nullptr;
  WindowEventFilterAdapter::get_mouse_event_window_id_func = nullptr;
}

void WindowEventFilterAdapter::SendMouseEventForTabDrag(
    const int32_t widget_id,
    Input_MouseEvent* window_mouse_event) {
  std::string xcomponent_id =
      xcomponent::WindowAdapter::GetInstance().GetWindowId(widget_id);
  auto render = xcomponent::XComponentManager::GetInstance()->GetXComponent(
      xcomponent_id);
  if (render == nullptr) {
    LOGE("[OhosTabDrag]can not get render: %{public}s", xcomponent_id.c_str());
    return;
  }
  render->SendWindowMouseEventForTabDrag(window_mouse_event);
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
        WindowEventFilterAdapter::GetWindowMouseEventWindowId(mouse_event);
    int32_t widget_id =
        window_event_filter.GetTargetWindowIdAfterShiftEvent(
            origin_window_id);
    if (widget_id <= 0) {
      widget_id = window_event_filter.GetDraggingTabWidgetId();
    }
    window_event_filter.SendMouseEventForTabDrag(widget_id, mouse_event);
    return true;
  }
  return false;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void RegisterWindowEventFilter(int32_t origin_window_id) {
  if (!WindowEventFilterAdapter::window_manager_lib.IsLoaded()) {
    LOGE(
        "[OhosTabDrag]WindowEventFilterAdapter::RegisterWindowEventFilter"
        " window_manager_lib is not loaded.");
    return;
  }
  auto register_mouse_event_filter_func =
      WindowEventFilterAdapter::window_manager_lib
          .GetFunction<RegisterMouseEventFilterFunc>(
              "OH_NativeWindowManager_RegisterMouseEventFilter");
  if (!register_mouse_event_filter_func) {
    LOGW(
        "[OhosTabDrag]WindowEventFilterAdapter::RegisterWindowEventFilter get "
        "register_mouse_event_filter_func fail");
    return;
  }
  auto result =
      register_mouse_event_filter_func(origin_window_id, FilterMouseEvent);
  LOGI(
      "[OhosTabDrag]WindowEventFilterAdapter::RegisterWindowEventFilter, "
      "window_id:%{public}d, "
      "result:%{public}d",
      origin_window_id, result);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void ClearWindowEventFilter(int32_t origin_window_id) {
  if (!WindowEventFilterAdapter::window_manager_lib.IsLoaded()) {
    LOGE(
        "[OhosTabDrag]WindowEventFilterAdapter::ClearWindowEventFilter "
        " window_manager_lib is not loaded.");
    return;
  }
  auto un_register_mouse_event_filter_func =
      WindowEventFilterAdapter::window_manager_lib
          .GetFunction<UnRegisterMouseEventFilterFunc>(
              "OH_NativeWindowManager_UnregisterMouseEventFilter");
  if (!un_register_mouse_event_filter_func) {
    LOGW(
        "[OhosTabDrag]WindowEventFilterAdapter::ClearWindowEventFilter get "
        "un_register_mouse_event_filter_func fail");
    return;
  }
  auto result = un_register_mouse_event_filter_func(origin_window_id);
  LOGI(
      "[OhosTabDrag]WindowEventFilterAdapter::ClearWindowEventFilter, "
      "window_id:%{public}d, "
      "result:%{public}d",
      origin_window_id, result);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventAction(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_action_func == nullptr) {
    LOGE(
        "[OhosTabDrag]WindowEventFilterAdapter::GetWindowMouseEventAction fail");
    return 0;
  }
  return get_mouse_event_action_func(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventDisplayX(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_display_x_func == nullptr) {
    LOGE(
        "[OhosTabDrag]WindowEventFilterAdapter::GetWindowMouseEventDisplayX "
        "fail");
    return 0;
  }
  return get_mouse_event_display_x_func(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventDisplayY(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_display_y_func == nullptr) {
    LOGE(
        "[OhosTabDrag]WindowEventFilterAdapter::GetWindowMouseEventDisplayY fail");
    return 0;
  }
  return get_mouse_event_display_y_func(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventButton(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_button_func == nullptr) {
    LOGE(
        "[OhosTabDrag]WindowEventFilterAdapter::GetWindowMouseEventButton fail");
    return 0;
  }
  return get_mouse_event_button_func(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int64_t WindowEventFilterAdapter::GetWindowMouseEventActionTime(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_action_time_func == nullptr) {
    LOGE(
        "[OhosTabDrag]WindowEventFilterAdapter::GetWindowMouseEventActionTime fail");
    return 0;
  }
  return get_mouse_event_action_time_func(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventWindowId(
    Input_MouseEvent* window_mouse_event) {
  if (get_mouse_event_window_id_func == nullptr) {
    LOGE(
        "[OhosTabDrag]WindowEventFilterAdapter::GetWindowMouseEventWindowId fail");
    return 0;
  }
  return get_mouse_event_window_id_func(window_mouse_event);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t WindowEventFilterAdapter::GetWindowMouseEventDisplayId(
    Input_MouseEvent* window_mouse_event) {
  return get_mouse_event_display_id_func(window_mouse_event);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(RegisterWindowEventFilter);
  JSBIND_FUNCTION(ClearWindowEventFilter);
}
} // namespace ohos::adapter::window
