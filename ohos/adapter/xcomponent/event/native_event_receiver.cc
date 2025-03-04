// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/xcomponent/event/native_event_receiver.h"

#include <string>

namespace ohos::adapter::xcomponent {

void OnSurfaceCreatedCB(OH_NativeXComponent* component, void* window) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnSurfaceCreated(component, window);
  }
}

void OnSurfaceChangedCB(OH_NativeXComponent* component, void* window) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnSurfaceChanged(component, window);
  }
}

void OnSurfaceDestroyedCB(OH_NativeXComponent* component, void* window) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnSurfaceDestroyed(component, window);
  }
}

void OnBlurEventCB(OH_NativeXComponent* component, void* window) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnBlurEvent(component, window);
  }
}

void OnFocusEventCB(OH_NativeXComponent* component, void* window) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnFocusEvent(component, window);
  }
}

void OnTouchEventCB(OH_NativeXComponent* component, void* window) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnTouchEvent(component, window);
  }
}

void OnMouseEventCB(OH_NativeXComponent* component, void* window) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnMouseEvent(component, window);
  }
}

void OnKeyEventCB(OH_NativeXComponent* component, void* window) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnKeyEvent(component, window);
  }
}

void OnHoverEventCB(OH_NativeXComponent* component, bool is_hover) {
  auto callback = NativeEventReceiver::GetInstance().GetCallBack(component);
  if (callback) {
    callback->OnHoverEvent(component, is_hover);
  }
}

NativeEventReceiver& NativeEventReceiver::GetInstance() {
  static NativeEventReceiver instance;
  return instance;
}

NativeEventReceiver::NativeEventReceiver() {
  surface_callback_ = {.OnSurfaceCreated = OnSurfaceCreatedCB,
                       .OnSurfaceChanged = OnSurfaceChangedCB,
                       .OnSurfaceDestroyed = OnSurfaceDestroyedCB,
                       .DispatchTouchEvent = OnTouchEventCB};

  mouse_callback_ = {.DispatchMouseEvent = OnMouseEventCB,
                     .DispatchHoverEvent = OnHoverEventCB};
}

NativeEventReceiver::~NativeEventReceiver() {
  callback_map_.clear();
}

NativeEventListener* NativeEventReceiver::GetCallBack(
    OH_NativeXComponent* component) {
  std::lock_guard<std::mutex> lock_protect(mutex_);
  if (callback_map_.find(component) != callback_map_.end()) {
    return callback_map_[component].get();
  }

  return nullptr;
}

void NativeEventReceiver::RegisterCallBack(
    OH_NativeXComponent* component,
    std::shared_ptr<NativeEventListener> callback) {
  std::lock_guard<std::mutex> lock_protect(mutex_);
  if (callback_map_.count(component)) {
    return;
  }

  OH_NativeXComponent_RegisterCallback(component, &surface_callback_);
  OH_NativeXComponent_RegisterMouseEventCallback(component, &mouse_callback_);
  OH_NativeXComponent_RegisterKeyEventCallback(component, OnKeyEventCB);
  OH_NativeXComponent_RegisterBlurEventCallback(component, OnBlurEventCB);
  OH_NativeXComponent_RegisterFocusEventCallback(component, OnFocusEventCB);

  callback_map_[component] = std::move(callback);
}

void NativeEventReceiver::UnRegisterCallBack(OH_NativeXComponent* component) {
  std::lock_guard<std::mutex> lock_protect(mutex_);
  if (!callback_map_.count(component)) {
    return;
  }
  callback_map_.erase(component);
}

}  // namespace ohos::adapter::xcomponent
