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

#ifndef NWEB_KEYCODE_INTERFACE_H
#define NWEB_KEYCODE_INTERFACE_H

#include <unordered_map>
#include <vector>
#include "cef/include/internal/cef_types.h"
#include "nweb.h"
#include "nweb_inputevent_handler.h"
#include "ui/events/keycodes/keyboard_codes_posix.h"

namespace OHOS::NWeb {
enum MouseAction { PRESS = 1, RELEASE = 2, MOVE = 3, HOVER_ENTER = 6,HOVER_EXIT = 9 };

class NWebInputDelegate {
 public:
  NWebInputDelegate();
  virtual ~NWebInputDelegate() = default;
  static int CefConverter(const std::string keyValue, int input);
  static int OhosConverter(const std::string keyValue, int input);
  static bool IsMMIKeyEvent(int32_t keyCode);
  static uint32_t GetMouseButtonModifiers(const cef_mouse_button_type_t& button);
  static uint32_t GetWebModifiers(int32_t keyCode, int32_t keyAction,
                                  const std::vector<int32_t>& pressedCodes);
  static uint32_t GetWebModifiersByPressedCode(const std::vector<int32_t>& pressedCodes);
  static uint32_t GetWebMouseModifiersByPressedCode(cef_mouse_button_type_t button,
                                                    const std::vector<int32_t>& pressedCodes);
  static uint32_t GetModifiersByKeyEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent);
  void SetModifiers(int keyCode, int keyAction);
  uint32_t GetModifiers();
  uint32_t GetModifiers(cef_mouse_button_type_t button);
  static inline bool IsMouseDown(int action) {
    return action == MouseAction::PRESS;
  }
  static inline bool IsMouseUp(int action) {
    return action == MouseAction::RELEASE;
  }
  static inline bool IsMouseMove(int action) {
    return action == MouseAction::MOVE;
  }
  static inline bool IsMouseEnter(int action) {
    return action == HOVER_ENTER;
  }
  static inline bool IsMouseLeave(int action) { return action == HOVER_EXIT; }
  void SetMouseWheelRatio(float ratio) { mouseWheelRatio_ = ratio; }
  float GetMouseWheelRatio() { return mouseWheelRatio_; }

 private:
  static bool KeyValueConvert(const std::string keyValue,
                              std::unordered_map<int, int>& map);
  static NWebInputEventHandle<int, int> keyEventHandle_;
  float mouseWheelRatio_ = -12.5;
};
}  // namespace OHOS::NWeb

#endif
