// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_EVENT_NATIVE_EVENT_LISTENER_H_
#define OHOS_ADAPTER_XCOMPONENT_EVENT_NATIVE_EVENT_LISTENER_H_

#include <ace/xcomponent/native_interface_xcomponent.h>

namespace ohos::adapter::xcomponent {

class NativeEventListener {
 public:
  virtual void OnTouchEvent(OH_NativeXComponent* component, void* window) = 0;
  virtual void OnMouseEvent(OH_NativeXComponent* component, void* window) = 0;
  virtual void OnKeyEvent(OH_NativeXComponent* component, void* window) = 0;
  virtual void OnHoverEvent(OH_NativeXComponent* component,
                            const bool is_hover) = 0;
  virtual void OnBlurEvent(OH_NativeXComponent* component, void* window) = 0;
  virtual void OnFocusEvent(OH_NativeXComponent* component, void* window) = 0;
  virtual void OnSurfaceCreated(OH_NativeXComponent* component,
                                void* window) = 0;
  virtual void OnSurfaceChanged(OH_NativeXComponent* component,
                                void* window) = 0;
  virtual void OnSurfaceDestroyed(OH_NativeXComponent* component,
                                  void* window) = 0;
  virtual void OnSurfaceShow(OH_NativeXComponent* component, void* window) = 0;
  virtual void OnSurfaceHide(OH_NativeXComponent* component, void* window) = 0;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_EVENT_NATIVE_EVENT_LISTENER_H_
