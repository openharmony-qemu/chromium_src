/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_EVENT_HANDLER_H
#define NWEB_EVENT_HANDLER_H

#include <list>

#include "cef/include/cef_client.h"
#include "nweb.h"
#include "nweb_input_delegate.h"
#include "nweb_inputmethod_handler.h"
#include "nweb_key_event.h"
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {

class NWebEventHandler {
 public:
  static std::shared_ptr<NWebEventHandler> Create();

  NWebEventHandler();
  ~NWebEventHandler() = default;
  void OnDestroy();

  void SetBrowser(CefRefPtr<CefBrowser> browser);
  void SetIsFocus(bool isFocus) { isFocus_ = isFocus; }
  void OnTouchPress(int32_t id, double x, double y, bool from_overlay);
  void OnTouchMove(int32_t id, double x, double y, bool from_overlay);
  void OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                   bool from_overlay, float virtual_pixel_ratio);
  void OnTouchRelease(int32_t id, double x, double y, bool from_overlay);
  void OnTouchCancel();
  void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay);
  void SendTouchpadFlingEvent(double x, double y, double vx, double vy);
  void SendMouseWheelEvent(double x, double y, double deltaX, double deltaY);
  void SendMouseEvent(int x, int y, int button, int action, int count);
  void NotifyForNextTouchEvent();

#if defined(OHOS_INPUT_EVENTS)
  bool SendKeyEventFromAce(int32_t keyCode, int32_t keyAction);
  bool WebSendKeyEventFromAce(int32_t keyCode, int32_t keyAction,
                              const std::vector<int32_t>& pressedCodes);
  void WebSendMouseWheelEvent(double x,
                              double y,
                              double deltaX,
                              double deltaY,
                              const std::vector<int32_t>& pressedCodes);
  void WebSendTouchpadFlingEvent(double x,
                                 double y,
                                 double vx,
                                 double vy,
                                 const std::vector<int32_t>& pressedCodes);
  void WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent, float ratio);
#endif  // defined(OHOS_INPUT_EVENTS)

 private:
  bool IsCharInputEvent(CefKeyEvent& keyEvent);

#if defined(OHOS_INPUT_EVENTS)
  void SendKeyEventFromMMI(int32_t keyCode, int32_t keyAction);
  bool SendKeyEvent(int32_t keyCode, int32_t keyAction);
  bool WebSendKeyEvent(int32_t keyCode, int32_t keyAction,
                       const std::vector<int32_t>& pressedCodes);
  bool CreateCefKeyEvent(CefKeyEvent& keyEvent,
                         int32_t keyCode, int32_t keyAction, int32_t modifiers);
  void SendCefKeyEvent(CefKeyEvent& keyEvent);
  void SendCefMouseWheelEvent(double x,
                              double y,
                              double deltaX,
                              double deltaY,
                              int32_t modifiers);
  void WebUpdateModifiers(CefMouseEvent& mouseInfo, const cef_mouse_button_type_t& buttonType);
#endif  // defined(OHOS_INPUT_EVENTS)

  CefRefPtr<CefBrowser> browser_ = nullptr;
  NWebInputDelegate input_delegate_;

#if defined(OHOS_INPUT_EVENTS)
  std::unique_ptr<MMIAdapter> mmi_adapter_ = nullptr;
  int32_t mmi_id_ = -1;
  int32_t previous_action_ = 2;
  int32_t previous_button_ = -1;
  bool is_in_web_ = false;
  bool isFocus_ = false;
  double sum_deltaY_ = 0.0;
#endif  // defined(OHOS_INPUT_EVENTS)
// last mouse move coordinates
  int last_mouse_x_ = -1;
  int last_mouse_y_ = -1;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_EVENT_HANDLER_H
