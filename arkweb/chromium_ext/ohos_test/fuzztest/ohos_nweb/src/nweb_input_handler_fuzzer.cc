/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/nweb_input_handler.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/cef_delegate/nweb_delegate.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

class MockNWebTouchPointInfo : public NWebTouchPointInfo {
 public:
  MockNWebTouchPointInfo(int32_t id, double x, double y)
      : id_(id), x_(x), y_(y) {}
  int32_t GetId() override { return id_; }
  double GetX() override { return x_; }
  double GetY() override { return y_; }

 private:
  int32_t id_;
  double x_;
  double y_;
};

class MockNWebKeyboardEvent : public NWebKeyboardEvent {
 public:
  MockNWebKeyboardEvent(int32_t keyCode,
                        int32_t action,
                        int32_t unicode,
                        bool enableCapsLock,
                        const std::vector<int32_t>& pressKeyCodes)
      : keyCode_(keyCode),
        action_(action),
        unicode_(unicode),
        enableCapsLock_(enableCapsLock),
        pressKeyCodes_(pressKeyCodes) {}
  int32_t GetKeyCode() override { return keyCode_; }
  int32_t GetAction() override { return action_; }
  int32_t GetUnicode() override { return unicode_; }
  bool IsEnableCapsLock() override { return enableCapsLock_; }
  std::vector<int32_t> GetPressKeyCodes() override { return pressKeyCodes_; }

 private:
  int32_t keyCode_;
  int32_t action_;
  int32_t unicode_;
  bool enableCapsLock_;
  std::vector<int32_t> pressKeyCodes_;
};

class MockNWebMouseEvent : public NWebMouseEvent {
 public:
  MockNWebMouseEvent(int32_t x,
                     int32_t y,
                     int32_t button,
                     int32_t action,
                     int32_t clickNum,
                     const std::vector<int32_t>& pressKeyCodes,
                     int32_t rawX,
                     int32_t rawY)
      : x_(x),
        y_(y),
        button_(button),
        action_(action),
        clickNum_(clickNum),
        pressKeyCodes_(pressKeyCodes),
        rawX_(rawX),
        rawY_(rawY) {}
  int32_t GetX() override { return x_; }
  int32_t GetY() override { return y_; }
  int32_t GetButton() override { return button_; }
  int32_t GetAction() override { return action_; }
  int32_t GetClickNum() override { return clickNum_; }
  std::vector<int32_t> GetPressKeyCodes() override { return pressKeyCodes_; }
  int32_t GetRawX() override { return rawX_; }
  int32_t GetRawY() override { return rawY_; }

 private:
  int32_t x_;
  int32_t y_;
  int32_t button_;
  int32_t action_;
  int32_t clickNum_;
  std::vector<int32_t> pressKeyCodes_;
  int32_t rawX_;
  int32_t rawY_;
};

void NWebInputDelegateFuzzTestOne(FuzzedDataProvider* fdp, std::shared_ptr<NWebInputHandler> impl) {
  int32_t id = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  double x = fdp->ConsumeFloatingPoint<double>();
  double y = fdp->ConsumeFloatingPoint<double>();
  bool from_overlay = fdp->ConsumeBool();
  std::shared_ptr<NWebTouchPointInfo> touch_point_info =
      std::make_shared<MockNWebTouchPointInfo>(id, x, y);
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  touch_point_infos.push_back(touch_point_info);
  int32_t keyCode = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t keyAction = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  std::vector<int32_t> pressedCodes;
  pressedCodes.push_back(fdp->ConsumeIntegralInRange<int32_t>(0, 100));
  double deltaX = fdp->ConsumeFloatingPoint<double>();
  double deltaY = fdp->ConsumeFloatingPoint<double>();
  int32_t source = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  double vx = fdp->ConsumeFloatingPoint<double>();
  double vy = fdp->ConsumeFloatingPoint<double>();
  int32_t unicode = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  bool enableCapsLock = fdp->ConsumeBool();
  std::shared_ptr<NWebKeyboardEvent> keyboardEvent =
      std::make_shared<MockNWebKeyboardEvent>(keyCode, keyAction, unicode,
                                              enableCapsLock, pressedCodes);

  impl->Init();

  impl->OnTouchPress(id, x, y, from_overlay);

  impl->OnTouchRelease(id, x, y, from_overlay);

  impl->OnTouchMove(id, x, y, from_overlay);

  impl->OnTouchMove(touch_point_infos, from_overlay);

  impl->OnTouchPress(id, x, y, from_overlay);

  impl->OnTouchCancel();

  impl->OnNavigateBack();

  impl->WebSendKeyEvent(keyCode, keyAction, pressedCodes);

  impl->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);

  impl->WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source);

  impl->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);

  impl->SendKeyEvent(keyCode, keyAction);

  impl->SendTouchpadFlingEvent(x, y, vx, vy);

  impl->SendKeyboardEvent(keyboardEvent);

  impl->SendMouseWheelEvent(x, y, deltaX, deltaY);
}

void NWebInputDelegateFuzzTestTwo(FuzzedDataProvider* fdp, std::shared_ptr<NWebInputHandler> impl) {
  int32_t x = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t y = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t button = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t action = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t clickNum = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  std::vector<int32_t> pressedCodes;
  pressedCodes.push_back(fdp->ConsumeIntegralInRange<int32_t>(0, 100));
  int32_t rawX = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  int32_t rawY = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  std::shared_ptr<NWebMouseEvent> mouseEvent =
      std::make_shared<MockNWebMouseEvent>(x, y, button, action, clickNum,
                                           pressedCodes, rawX, rawY);

  impl->WebSendMouseEvent(mouseEvent);

  impl->SendMouseEvent(x, y, button, action, clickNum);
}

void NWebInputDelegateFuzzTest(FuzzedDataProvider* fdp) {
  int argc;
  const char* argv[5];
  std::shared_ptr<NWebDelegateInterface> nweb_delegate =
      std::make_shared<NWebDelegate>(argc, argv);
  std::shared_ptr<NWebInputHandler> impl =
      NWebInputHandler::Create(nweb_delegate);

  NWebInputDelegateFuzzTestOne(fdp, impl);

  NWebInputDelegateFuzzTestTwo(fdp, impl);

  impl->OnDestroy();

  NWebInputDelegateFuzzTestOne(fdp, impl);

  NWebInputDelegateFuzzTestTwo(fdp, impl);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebInputDelegateFuzzTest(&fdp);

  return 0;
}
