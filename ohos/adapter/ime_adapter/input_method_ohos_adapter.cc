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

#include "input_method_ohos_adapter.h"
#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos {
namespace adapter {
InputMethodOHOSAdapter& InputMethodOHOSAdapter::GetInstance() {
  static InputMethodOHOSAdapter helper;
  return helper;
}

void InputMethodOHOSAdapter::AttachTextInput(IMFAdapterTextConfig textConfig,
                                             int32_t requestKeyboardReason) {
  if (auto insert = ohos::adapter::GetJSFunction("IMFAdapter.AttachTextInput")) {
    auto insert_promise = std::make_shared<std::promise<bool>>();
    auto future = insert_promise->get_future();
    std::function<void(bool)> callback = [insert_promise](bool successful) {
      insert_promise->set_value(successful);
    };
    insert->Invoke<void>(textConfig, requestKeyboardReason, callback);
    auto status = future.wait_for(std::chrono::seconds(3));
    if (status == std::future_status::timeout) {
      LOGE("IMFAdapter.AttachTextInput wait timeout");
      return;
    }
    bool result = future.get();
    if (!result) {
      LOGE("IMFAdapter.AttachTextInput run fail");
    }
  }
}

void InputMethodOHOSAdapter::DetachTextInput() {
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.DetachTextInput")) {
    std::promise<bool> detach_promise;
    std::function<void(bool)> callback = [&detach_promise](bool successful) {
      detach_promise.set_value(successful);
    };
    func->Invoke<void>(callback);
    detach_promise.get_future().get();
  }
}
void InputMethodOHOSAdapter::OffListenIME() {
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.OffListenIME")) {
    func->Invoke<void>();
  }
}

void InputMethodOHOSAdapter::ShowTextInput(int32_t requestKeyboardReason) {
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.ShowTextInput")) {
    func->Invoke<void>(requestKeyboardReason);
  }
}

void InputMethodOHOSAdapter::UpdateAttribute(
    IMFAdapterInputAttribute inputAttribute) {
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.UpdateAttribute")) {
    func->Invoke<void>(inputAttribute);
  }
}

void InputMethodOHOSAdapter::NotifyCursorUpdate(
    const IMFAdapterCursorInfo cursorInfo) {
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.CursorUpdate")) {
    func->Invoke<void>(cursorInfo);
  }
}

void InputMethodOHOSAdapter::InsertTextCallback(const std::string& text) {
  if (delegate_ != nullptr) {
    delegate_->InsertText(text);
  }
}

void InsertTextCallback(const std::string& text) {
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().InsertTextCallback(text);
}

void InputMethodOHOSAdapter::DeleteBackCallback(int32_t length) {
  if (delegate_ != nullptr) {
    delegate_->DeleteBackward(length);
  }
}

void DeleteBackCallback(int32_t length) {
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().DeleteBackCallback(length);
}

void InputMethodOHOSAdapter::DeleteForwardCallback(int32_t length) {
  if (delegate_ != nullptr) {
   delegate_->DeleteForward(length);
  }
}

void DeleteForwardCallback(int32_t length) {
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().DeleteForwardCallback(length);
}

void InputMethodOHOSAdapter::SendEnterKeyEventCallback() {
  if (delegate_ != nullptr) {
    delegate_->SendEnterKeyEvent();
  }
}

void SendEnterKeyEventCallback() {
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().SendEnterKeyEventCallback();
}

void InputMethodOHOSAdapter::MoveCursorCallback(const int direction) {
  if (delegate_ != nullptr) {
    delegate_->MoveCursor(direction);
  }
}

void MoveCursorCallback(const aki::Value direction) {
  int input_direction = direction.As<int>();
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().MoveCursorCallback(input_direction);
}

void InputMethodOHOSAdapter::Register(Delegate* delegate) {
  delegate_ = delegate;
}

JSBIND_CLASS(IMFAdapterTextConfig) {
  JSBIND_PROPERTY(inputAttribute);
  JSBIND_PROPERTY(cursorInfo);
}

JSBIND_CLASS(IMFAdapterInputAttribute) {
  JSBIND_PROPERTY(inputPattern);
  JSBIND_PROPERTY(enterKeyType);
}

JSBIND_CLASS(IMFAdapterCursorInfo) {
  JSBIND_PROPERTY(left);
  JSBIND_PROPERTY(top);
  JSBIND_PROPERTY(width);
  JSBIND_PROPERTY(height);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(InsertTextCallback);
  JSBIND_FUNCTION(DeleteBackCallback);
  JSBIND_FUNCTION(DeleteForwardCallback);
  JSBIND_FUNCTION(SendEnterKeyEventCallback);
  JSBIND_FUNCTION(MoveCursorCallback);
}
}  // namespace adapter
}  // namespace ohos
