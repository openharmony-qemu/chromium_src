// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "input_method_ohos_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos {
namespace adapter {
InputMethodOHOSAdapter& InputMethodOHOSAdapter::GetInstance() {
  static InputMethodOHOSAdapter helper;
  return helper;
}

void InputMethodOHOSAdapter::AttachTextInput(IMFAdapterTextConfig textConfig) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (auto insert =
          ohos::adapter::GetJSFunction("IMFAdapter.AttachTextInput")) {
    std::promise<bool> insert_promise;
    std::function<void(bool)> callback = [&insert_promise](bool successful) {
      insert_promise.set_value(successful);
    };
    insert->Invoke<void>(textConfig, callback);
    insert_promise.get_future().get();
  }
}

void InputMethodOHOSAdapter::DetachTextInput() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.DetachTextInput")) {
    std::promise<bool> detach_promise;
    std::function<void(bool)> callback = [&detach_promise](bool successful) {
      detach_promise.set_value(successful);
    };
    func->Invoke<void>(callback);
    detach_promise.get_future().get();
  }
}

void InputMethodOHOSAdapter::ShowTextInput() {
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.ShowTextInput")) {
    func->Invoke<void>();
  }
}

void InputMethodOHOSAdapter::UpdateAttribute(
    IMFAdapterInputAttribute inputAttribute) {
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.UpdateAttribute")) {
    func->Invoke<void>(inputAttribute);
  }
}

void InputMethodOHOSAdapter::RegisterInsertTextCallback(
    InsertTextCallback callback) {
  if (callback != nullptr) {
    insertTextCallback_ = callback;
  }
}

void InputMethodOHOSAdapter::RegisterDeleteBackwardCallback(
    DeleteBackCallback callback) {
  if (callback != nullptr) {
    deleteBackCallback_ = callback;
  }
}

void InputMethodOHOSAdapter::RegisterDeleteForwardCallback(
    DeleteForwardCallback callback) {
  if (callback != nullptr) {
    deleteForwardCallback_ = callback;
  }
}

void InputMethodOHOSAdapter::RegisterSendEnterKeyEventCallback(
    SendEnterKeyEventCallback callback) {
  if (callback != nullptr) {
    sendEnterKeyEventCallback_ = callback;
  }
}

void InputMethodOHOSAdapter::RegisterMoveCursorCallback(
    MoveCursorCallback callback) {
  if (callback != nullptr) {
    moveCursorCallback_ = callback;
  }
}

void InputMethodOHOSAdapter::NotifyCursorUpdate(
    const IMFAdapterCursorInfo cursorInfo) {
  if (auto func = ohos::adapter::GetJSFunction("IMFAdapter.CursorUpdate")) {
    func->Invoke<void>(cursorInfo);
  }
}

void InsertTextCallback(const std::string& text) {
  if (InputMethodOHOSAdapter::GetInstance().GetInsertTextCallbcak() !=
      nullptr) {
    InputMethodOHOSAdapter::GetInstance().GetInsertTextCallbcak()(text);
  }
}

void DeleteBackCallback(int32_t length) {
  if (InputMethodOHOSAdapter::GetInstance().GetDeleteBackCallbcak() !=
      nullptr) {
    InputMethodOHOSAdapter::GetInstance().GetDeleteBackCallbcak()(length);
  }
}

void DeleteForwardCallback(int32_t length) {
  if (InputMethodOHOSAdapter::GetInstance().GetDeleteForwardCallbcak() !=
      nullptr) {
    InputMethodOHOSAdapter::GetInstance().GetDeleteForwardCallbcak()(length);
  }
}

void SendEnterKeyEventCallback() {
  if (InputMethodOHOSAdapter::GetInstance().GetSendEnterKeyEventCallbcak() !=
      nullptr) {
    InputMethodOHOSAdapter::GetInstance().GetSendEnterKeyEventCallbcak()();
  }
}

void MoveCursorCallback(const aki::Value direction) {
  int inputDirection = direction.As<int>();
  if (InputMethodOHOSAdapter::GetInstance().GetMoveCursorCallbcak() !=
      nullptr) {
    InputMethodOHOSAdapter::GetInstance().GetMoveCursorCallbcak()(
        inputDirection);
  }
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
