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

#ifndef INPUT_METHOD_OHOS_ADAPTER_H_
#define INPUT_METHOD_OHOS_ADAPTER_H_

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>

#include "ohos/adapter/export.h"

namespace ohos {
namespace adapter {
enum IMFAdapterTextInputType {
  NONE_INPUT = -1,
  TEXT = 0,
  MULTILINE,
  NUMBER,
  PHONE,
  DATETIME,
  EMAIL_ADDRESS,
  URL,
  VISIBLE_PASSWORD,
};

struct IMFAdapterCursorInfo {
  double left = 0.0;
  double top = 0.0;
  double width = 0.0;
  double height = 0.0;
};

struct IMFAdapterInputAttribute {
  int32_t inputPattern = 0;
  int32_t enterKeyType = 0;
};

struct IMFAdapterSelectionRange {
  int32_t start = -1;
  int32_t end = -1;
};

struct IMFAdapterTextConfig {
  IMFAdapterInputAttribute inputAttribute = {};
  IMFAdapterCursorInfo cursorInfo = {};
  IMFAdapterSelectionRange range = {};
  uint32_t windowId = -1;
};

enum IMFAdapterEnterKeyType {
  UNSPECIFIED = 0,
  NONE_KEY,
  GO,
  SEARCH,
  SEND,
  NEXT,
  DONE,
  PREVIOUS
};

enum IMFAdapterDirection {
  NONE = 0,
  UP = 1,
  DOWN,
  LEFT,
  RIGHT,
};

class ADAPTER_EXPORT_API InputMethodOHOSAdapter {
 public:
  // Handles text insertion, deletion, and cursor movement
  // Processes enter key events and exit full screen
  class Delegate {
    public:
      virtual void InsertText(const std::string& text) = 0;
      virtual void DeleteBackward(int32_t length) = 0;
      virtual void DeleteForward(int32_t length) = 0;
      virtual void SendEnterKeyEvent() = 0;
      virtual void MoveCursor(int direction) = 0;
  };
  static InputMethodOHOSAdapter& GetInstance();
  virtual ~InputMethodOHOSAdapter() = default;
  void AttachTextInput(IMFAdapterTextConfig textConfig,
                       int32_t requestKeyboardReason);
  void DetachTextInput();
  void OffListenIME();
  void UpdateAttribute(IMFAdapterInputAttribute inputAttribute);
  void ShowTextInput(int32_t requestKeyboardReason);
  void NotifyCursorUpdate(const IMFAdapterCursorInfo cursorInfo);
  void InsertTextCallback(const std::string& text);
  void DeleteBackCallback(int32_t length);
  void DeleteForwardCallback(int32_t length);
  void SendEnterKeyEventCallback();
  void MoveCursorCallback(const int direction);
  void Register(Delegate* delegate_);
 private:
  Delegate* delegate_;
};
}  // namespace adapter
}  // namespace ohos
#endif  // INPUT_METHOD_OHOS_ADAPTER_H_
