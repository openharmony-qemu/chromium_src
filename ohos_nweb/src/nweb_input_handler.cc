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

#include "nweb_input_handler.h"
#include "nweb_hilog.h"

namespace OHOS::NWeb {
// static
std::shared_ptr<NWebInputHandler> NWebInputHandler::Create(
    std::shared_ptr<NWebDelegateInterface> nweb_delegate) {
  auto input_handler = std::make_shared<NWebInputHandler>(nweb_delegate);
  if (input_handler == nullptr) {
    WVLOG_E("fail to create NWebOutputHandler instance");
    return nullptr;
  }
  input_handler->Init();
  return input_handler;
}

NWebInputHandler::NWebInputHandler(
    std::shared_ptr<NWebDelegateInterface> nweb_delegate)
    : nweb_delegate_(nweb_delegate) {}

bool NWebInputHandler::Init() {
  return true;
}

void NWebInputHandler::OnDestroy() {
  nweb_delegate_ = nullptr;
}

void NWebInputHandler::OnTouchPress(int32_t id,
                                    double x,
                                    double y,
                                    bool from_overlay) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->OnTouchPress(id, x, y, from_overlay);

  touch_press_id_map_[id] = true;
  last_touch_start_x_ = x;
  last_x_ = x;
  last_y_ = y;
}

void NWebInputHandler::OnTouchRelease(int32_t id,
                                      double x,
                                      double y,
                                      bool from_overlay) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  if (x == 0 && y == 0) {
    x = last_x_;
    y = last_y_;
  }
  nweb_delegate_->OnTouchRelease(id, x, y, from_overlay);
  touch_press_id_map_.erase(id);
}

void NWebInputHandler::OnTouchMove(int32_t id,
                                   double x,
                                   double y,
                                   bool from_overlay) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  nweb_delegate_->OnTouchMove(id, x, y, from_overlay);
  last_x_ = x;
  last_y_ = y;
}

void NWebInputHandler::OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                                   bool from_overlay) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  if (touch_point_infos.empty()) {
    return;
  }
  nweb_delegate_->OnTouchMove(touch_point_infos, from_overlay);
  std::shared_ptr<NWebTouchPointInfo> touch_point_info = touch_point_infos.front();
  last_x_ = touch_point_info->GetX();
  last_y_ = touch_point_info->GetY();
}

void NWebInputHandler::OnTouchCancel() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  for (auto it = touch_press_id_map_.begin(); it != touch_press_id_map_.end(); it++) {
    if (it->second) {
      nweb_delegate_->OnTouchCancelById(it->first, 0, 0, false);
    }
  }
  touch_press_id_map_.clear();
}

void NWebInputHandler::OnTouchCancelById(int32_t id,
                                         double x,
                                         double y,
                                         bool from_overlay) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->OnTouchCancelById(id, x, y, from_overlay);
}

void NWebInputHandler::OnNavigateBack() {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  WVLOG_I("receive key navigate back");
  if (nweb_delegate_->IsNavigatebackwardAllowed()) {
    WVLOG_I("do navigate back");
    nweb_delegate_->NavigateBack();
  }
}

#if defined(OHOS_INPUT_EVENTS)
bool NWebInputHandler::WebSendKeyEvent(int32_t keyCode, int32_t keyAction,
                                       const std::vector<int32_t>& pressedCodes) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
}

void NWebInputHandler::WebSendMouseWheelEvent(double x,
                                              double y,
                                              double deltaX,
                                              double deltaY,
                                              const std::vector<int32_t>& pressedCodes) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
}

void NWebInputHandler::WebSendTouchpadFlingEvent(double x,
                                                 double y,
                                                 double vx,
                                                 double vy,
                                                 const std::vector<int32_t>& pressedCodes) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);
}

bool NWebInputHandler::SendKeyboardEvent(
    const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->SendKeyboardEvent(keyboardEvent);
}

void NWebInputHandler::WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->WebSendMouseEvent(mouseEvent);
}
#endif

bool NWebInputHandler::SendKeyEvent(int32_t keyCode, int32_t keyAction) {
  if (nweb_delegate_ == nullptr) {
    return false;
  }
  return nweb_delegate_->SendKeyEvent(keyCode, keyAction);
}

void NWebInputHandler::SendTouchpadFlingEvent(double x, double y, double vx, double vy) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SendTouchpadFlingEvent(x, y, vx, vy);
}

void NWebInputHandler::SendMouseWheelEvent(double x,
                                           double y,
                                           double deltaX,
                                           double deltaY) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SendMouseWheelEvent(x, y, deltaX, deltaY);
}

void NWebInputHandler::SendMouseEvent(int x,
                                      int y,
                                      int button,
                                      int action,
                                      int count) {
  if (nweb_delegate_ == nullptr) {
    return;
  }
  nweb_delegate_->SendMouseEvent(x, y, button, action, count);
}

void NWebInputHandler::CheckSlideNavigation(int16_t start_x, int16_t end_x) {
  if (nweb_delegate_ == nullptr) {
    return;
  }

  constexpr int16_t kTriggerDistance = 100;
  constexpr int16_t kScreenWidth = 2560;
  constexpr int16_t kEdgeWidth = 10;
  if (start_x < kEdgeWidth && (end_x - start_x) > kTriggerDistance) {
    WVLOG_I("receive slide navigate back");
    if (nweb_delegate_->IsNavigatebackwardAllowed()) {
      WVLOG_I("do navigate back");
      nweb_delegate_->NavigateBack();
    }
  } else if (start_x > (kScreenWidth - kEdgeWidth) &&
             (start_x - end_x) > kTriggerDistance) {
    WVLOG_I("receive slide navigate forward");
    if (nweb_delegate_->IsNavigateForwardAllowed()) {
      WVLOG_I("do navigate forward");
      nweb_delegate_->NavigateForward();
    }
  }
}
}  // namespace OHOS::NWeb
