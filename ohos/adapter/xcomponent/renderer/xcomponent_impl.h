// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_IMPL_H_
#define OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_IMPL_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <memory>
#include <multimodalinput/oh_input_manager.h>
#include <string>

#include "ohos/adapter/drag_drop/drag_drop_ohos_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_delegate.h"
#include "ohos/adapter/xcomponent/event/input_event_common.h"
#include "ohos/adapter/xcomponent/renderer/xcomponent_base.h"

namespace ohos::adapter::xcomponent {

class XComponentImpl : public XComponentBase {
 public:
  explicit XComponentImpl(const std::string& id,
                          const std::string& type);
  ~XComponentImpl();

  void RegisterInputEventCallBack(std::shared_ptr<InputEventCallBack> callback);
  void Initialize(OH_NativeXComponent* component,
                  XComponentDelegate* delegate);
  OH_NativeXComponent* GetComponent() const { return instance_; }

  // GestureEventCallBack
  void OnPanEvent(const PanAction action,
                  const PanEvent& event);

  void OnPinchEvent(const std::string& pinch_step,
                    const PinchEvent& event);

  // DragEventCallBack
  void OnDragEnterEvent(const ohos::adapter::OhosDropData& drop_data);
  void OnDragMoveEvent(const float window_x,
                       const float window_y);
  void OnDropEvent(const ohos::adapter::OhosDropData& drop_data);
  void OnDragLeaveEvent();
  void OnDragEndEvent();

  // NativeEventListener
  void OnTouchEvent(const OH_NativeXComponent_TouchEvent& touch_event,
                    const OH_NativeXComponent_TouchPointToolType tool_type,
                    const TouchPointCoordinate& coordinate);
  void OnMouseEvent(const OH_NativeXComponent_MouseEvent& mouse_event);
  void OnKeyEvent(OH_NativeXComponent_KeyEvent& key_event);
  void OnHoverEvent(const bool is_hover);
  void OnBlurEvent();
  void OnFocusEvent();
  void OnSurfaceCreated();
  void OnSurfaceChanged();
  void OnSurfaceDestroyed();

  void SendWindowMouseEventForTabDrag(Input_MouseEvent* window_mouse_event) override;
  void SendWindowTouchEventForTabDrag(
      Input_TouchEvent* window_touch_event) override;

 private:
  OH_NativeXComponent* instance_ = nullptr;
  std::shared_ptr<InputEventCallBack> event_callback_ = nullptr;
  int32_t initial_width_;
  int32_t initial_height_;

  // XComponent callbacks
  OH_NativeXComponent_Callback surface_callback_;
  OH_NativeXComponent_MouseEvent_Callback mouse_callback_;
  void EventReissueAfterRegisterInputEventCallBack();
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_IMPL_H_
