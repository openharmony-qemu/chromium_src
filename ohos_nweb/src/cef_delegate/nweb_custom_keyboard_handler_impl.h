/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_CUSTOM_KEYBOARD_HANDLER_IMPL_H
#define NWEB_CUSTOM_KEYBOARD_HANDLER_IMPL_H

#include <chrono>
#include <string>

#include "cef/include/cef_browser.h"
#include "nweb_handler.h"

namespace OHOS::NWeb {
enum class WebCustomKeyboardState {
  FROM_ONBLUR = 0,
  FROM_ONPAUSE,
  FROME_ONFOCUS,
  FROME_ONCONTINUE,
};

class NWebCustomKeyboardHandlerImpl : public NWebCustomKeyboardHandler {
public:
  explicit NWebCustomKeyboardHandlerImpl(std::shared_ptr<NWebHandler> handler) : nweb_handler_(handler) {}
  ~NWebCustomKeyboardHandlerImpl() = default;

  void InsertText(const std::string &text) override;

  void DeleteForward(int32_t length) override;

  void DeleteBackward(int32_t length) override;

  void SendFunctionKey(int32_t key) override;

  void Close() override;

  void Attach(CefRefPtr<CefBrowser> browser, bool show_keyboard, int32_t input_flags);

  void CloseFromWebStateChange(WebCustomKeyboardState mode);

  bool AttachFromWebStateChange(WebCustomKeyboardState mode);

  bool IsAttached();

private:
  std::weak_ptr<NWebHandler> nweb_handler_;
  CefRefPtr<CefBrowser> browser_ = nullptr;
  bool isAttached_ = false;
  bool isCloseFromOnblur_ = false;
  bool isCloseFromOnpause_ = false;
  int32_t input_flags_ = 0;
  std::chrono::high_resolution_clock::time_point lastCloseInputMethodTime_;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_CUSTOM_KEYBOARD_HANDLER_IMPL_H
