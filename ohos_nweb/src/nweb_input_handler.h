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

#ifndef NWEB_INPUT_HANDLER_H
#define NWEB_INPUT_HANDLER_H

#include <memory>
#include <unordered_map>
#include "nweb.h"
#include "nweb_delegate_interface.h"

namespace OHOS::NWeb {
class NWebInputHandler {
 public:
  static std::shared_ptr<NWebInputHandler> Create(
      std::shared_ptr<NWebDelegateInterface> nweb_delegate);

  NWebInputHandler(std::shared_ptr<NWebDelegateInterface> nweb_delegate);
  ~NWebInputHandler() = default;

  bool Init();
  void OnDestroy();

  /* event interface */
  void OnTouchPress(int32_t id, double x, double y, bool from_overlay);
  void OnTouchRelease(int32_t id, double x, double y, bool from_overlay);
  void OnTouchMove(int32_t id, double x, double y, bool from_overlay);
  void OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                   bool from_overlay);
  void OnTouchCancel();
  void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay);
  void OnNavigateBack();
  bool SendKeyEvent(int32_t keyCode, int32_t keyAction);
  void SendTouchpadFlingEvent(double x, double y, double vx, double vy);
  void SendMouseWheelEvent(double x, double y, double deltaX, double deltaY);
  void SendMouseEvent(int x, int y, int button, int action, int count);
#if defined(OHOS_INPUT_EVENTS)
  bool WebSendKeyEvent(int32_t keyCode, int32_t keyAction,
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
  void WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent);
#endif
 private:
  void CheckSlideNavigation(int16_t start_x, int16_t end_x);

 private:
  std::shared_ptr<NWebDelegateInterface> nweb_delegate_;
  int32_t last_touch_start_x_ = -1;
  double last_x_ = -1;
  double last_y_ = -1;
  std::unordered_map<uint32_t, bool> touch_press_id_map_;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_INPUT_HANDLER_H
