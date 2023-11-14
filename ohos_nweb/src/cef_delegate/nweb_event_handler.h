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

#include "cef/include/cef_client.h"
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
  void OnTouchRelease(int32_t id, double x, double y, bool from_overlay);
  void OnTouchCancel();
  bool SendKeyEventFromAce(int32_t keyCode, int32_t keyAction);
  void SendMouseWheelEvent(double x, double y, double deltaX, double deltaY);
  void SendMouseEvent(int x, int y, int button, int action, int count);

 private:
  void SendKeyEventFromMMI(int32_t keyCode, int32_t keyAction);
  bool SendKeyEvent(int32_t keyCode, int32_t keyAction);
  bool IsCharInputEvent(CefKeyEvent& keyEvent);

 private:
  CefRefPtr<CefBrowser> browser_ = nullptr;
  NWebInputDelegate input_delegate_;
  std::unique_ptr<MMIAdapter> mmi_adapter_ = nullptr;
  int32_t mmi_id_ = -1;
  bool isFocus_ = false;
  double sum_deltaY_ = 0.0;
// last mouse move coordinates
  int last_x_ = -1;
  int last_y_ = -1;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_EVENT_HANDLER_H
