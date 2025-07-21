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

#include "ohos_nweb/src/cef_delegate/nweb_input_delegate.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

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

void NWebInputDelegateFuzzTest(FuzzedDataProvider* fdp) {
  cef_mouse_button_type_t array[] = {
      MBT_LEFT, MBT_MIDDLE, MBT_RIGHT, MBT_BACK, MBT_FORWARD,
  };
  auto button = fdp->PickValueInArray<cef_mouse_button_type_t>(array);
  int32_t keyCode = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
  int32_t keyAction = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
  int32_t pCode = fdp->ConsumeIntegralInRange<int32_t>(0, 256);
  int32_t unicode = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
  bool enableCapsLock = fdp->ConsumeBool();
  std::vector<int32_t> pressedCodes;
  pressedCodes.push_back(pCode);
  std::shared_ptr<NWebKeyboardEvent> keyboardEvent =
      std::make_shared<MockNWebKeyboardEvent>(keyCode, keyAction, unicode,
                                              enableCapsLock, pressedCodes);
  std::string keyValue = fdp->ConsumeRandomLengthString(256);
  int input = fdp->ConsumeIntegralInRange<int>(0, 256);
  int action = fdp->ConsumeIntegralInRange<int>(0, 256);
  float ratio = fdp->ConsumeFloatingPoint<float>();
  NWebInputDelegate impl;

  impl.GetMouseButtonModifiers(button);

  impl.GetWebModifiers(keyCode, keyAction, pressedCodes);

  impl.GetModifiersByKeyEvent(keyboardEvent);

  impl.GetWebModifiersByPressedCode(pressedCodes);

  impl.CefConverter(keyValue, input);

  impl.OhosConverter(keyValue, input);

  impl.SetModifiers(keyCode, keyAction);

  impl.GetModifiers();

  impl.GetModifiers(button);

  impl.GetWebMouseModifiersByPressedCode(button, pressedCodes);

  impl.IsMMIKeyEvent(keyCode);

  impl.SetMouseWheelRatio(keyCode);

  impl.GetMouseWheelRatio();

  NWebInputDelegate::IsMouseDown(action);

  NWebInputDelegate::IsMouseUp(action);

  NWebInputDelegate::IsMouseMove(action);

  NWebInputDelegate::IsMouseEnter(action);

  NWebInputDelegate::IsMouseLeave(action);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebInputDelegateFuzzTest(&fdp);

  return 0;
}
