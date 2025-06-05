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

#ifndef OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_IMPL_H_
#define OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_IMPL_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <memory>
#include <multimodalinput/oh_input_manager.h>
#include <string>

#include "ohos/adapter/drag_drop/drag_drop_ohos_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_delegate.h"
#include "ohos/adapter/xcomponent/event/input_event_common.h"

namespace ohos::adapter::xcomponent {

class XComponentImpl {
 public:
  explicit XComponentImpl(const std::string& id,
                          const std::string& type);
  ~XComponentImpl();

  std::string GetId() { return id_; }
  XComponentType GetType() { return type_; }
  void SetWidget(int32_t widget_id) { widget_id_ = widget_id; }
  void SetInitialBounds(int32_t initial_width, int32_t initial_height);
  int32_t GetWidget() const { return widget_id_; }
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

  void RequestLayout();
  void SendWindowMouseEventForTabDrag(Input_MouseEvent* window_mouse_event);
 private:
  OH_NativeXComponent* instance_ = nullptr;
  XComponentDelegate* delegate_ = nullptr;
  std::string id_;
  XComponentType type_;
  int32_t widget_id_;
  std::shared_ptr<InputEventCallBack> event_callback_ = nullptr;
  int32_t initial_width_;
  int32_t initial_height_;

  bool is_event_reissuance_required_ = false;
  bool has_hover_event_before_register_ = false;
  bool is_hover_before_register_ = false;

  // XComponent callbacks
  OH_NativeXComponent_Callback surface_callback_;
  OH_NativeXComponent_MouseEvent_Callback mouse_callback_;
  void EventReissueAfterRegisterInputEventCallBack();
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_IMPL_H_
