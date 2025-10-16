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

#ifndef OHOS_ADAPTER_XCOMPONENT_EVENT_WINDOW_EVENT_FILTER_ADAPTER_H_
#define OHOS_ADAPTER_XCOMPONENT_EVENT_WINDOW_EVENT_FILTER_ADAPTER_H_

#include <cstdint>
#include <map>
#include <shared_mutex>
#include <string>

#include <multimodalinput/oh_input_manager.h>

#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::window {
using MouseEventFilterFunc = bool (*)(Input_MouseEvent*);
using RegisterMouseEventFilterFunc = int32_t(int32_t, MouseEventFilterFunc);
using UnRegisterMouseEventFilterFunc = int32_t(int32_t);
using GetMouseEventActionFunc = int32_t(Input_MouseEvent*);
using GetMouseEventDisplayXFunc = int32_t(Input_MouseEvent*);
using GetMouseEventDisplayYFunc = int32_t(Input_MouseEvent*);
using GetMouseEventButtonFunc = int32_t(Input_MouseEvent*);
using GetMouseEventActionTimeFunc = int64_t(Input_MouseEvent*);
using GetMouseEventWindowIdFunc = int32_t(Input_MouseEvent*);
using GetMouseEventDisplayIdFunc = int32_t(Input_MouseEvent*);

class ADAPTER_EXPORT_API WindowEventFilterAdapter {
 public:
  static WindowEventFilterAdapter& GetInstance();
  bool CanFilterWindowMouseEvent() { return can_filter_window_mouse_event_; }
  void SendMouseEventForTabDrag(const int32_t widget_id,
                                Input_MouseEvent* window_mouse_event);
  void SetDraggingTabWidgetId(int32_t widget_id);
  int32_t GetDraggingTabWidgetId() const;
  bool IsTabDragging() const;
  void CacheShiftEventWindowIds(const int32_t source_id,
                                const int32_t target_id);
  int32_t GetTargetWindowIdAfterShiftEvent(int32_t origin_window_id);
  bool LoadAllEventFunctions();
  int32_t GetWindowMouseEventAction(Input_MouseEvent* window_mouse_event);
  int32_t GetWindowMouseEventDisplayX(Input_MouseEvent* window_mouse_event);
  int32_t GetWindowMouseEventDisplayY(Input_MouseEvent* window_mouse_event);
  int32_t GetWindowMouseEventButton(Input_MouseEvent* window_mouse_event);
  int64_t GetWindowMouseEventActionTime(Input_MouseEvent* window_mouse_event);
  int32_t GetWindowMouseEventWindowId(Input_MouseEvent* window_mouse_event);
  int32_t GetWindowMouseEventDisplayId(Input_MouseEvent* window_mouse_event);
  void RegisterWindowEventFilterForWindow(int32_t origin_window_id);
  void UnRegisterWindowEventFilterForWindow(int32_t origin_window_id);

  static void RegisterWindowEventFilter(int32_t origin_window_id);
  static void ClearWindowEventFilter(int32_t origin_window_id);

  std::atomic<int32_t> tab_dragging_widget_id_;
 private:
  WindowEventFilterAdapter();
  ~WindowEventFilterAdapter();
  // Check whether all functions required for tab dragging are parsed.
  bool can_filter_window_mouse_event_ = false;
  // Saves the window id and origin window id after the event is transferred.
  int32_t source_window_id_;
  int32_t tartget_window_id_;
  int32_t source_origin_window_id_;
  int32_t target_origin_window_id_;
  std::shared_mutex window_event_mutext_;
  common::SharedLibrary window_manager_lib_;
  RegisterMouseEventFilterFunc* register_mouse_event_filter_func_;
  UnRegisterMouseEventFilterFunc* un_register_mouse_event_filter_func_;
  GetMouseEventActionFunc* get_mouse_event_action_func_;
  GetMouseEventDisplayXFunc* get_mouse_event_display_x_func_;
  GetMouseEventDisplayYFunc* get_mouse_event_display_y_func_;
  GetMouseEventButtonFunc* get_mouse_event_button_func_;
  GetMouseEventActionTimeFunc* get_mouse_event_action_time_func_;
  GetMouseEventWindowIdFunc* get_mouse_event_window_id_func_;
  GetMouseEventDisplayIdFunc* get_mouse_event_display_id_func_;
};

}  // namespace ohos::adapter::window
#endif  // OHOS_ADAPTER_XCOMPONENT_EVENT_WINDOW_EVENT_FILTER_ADAPTER_H_
