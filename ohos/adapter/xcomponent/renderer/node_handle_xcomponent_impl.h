/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef OHOS_ADAPTER_XCOMPONENT_RENDERER_NODE_HANDLE_XCOMPONENT_IMPL_H_
#define OHOS_ADAPTER_XCOMPONENT_RENDERER_NODE_HANDLE_XCOMPONENT_IMPL_H_

#include <ace/xcomponent/native_interface_xcomponent.h>

#include <memory>
#include <string>

#include "ohos/adapter/node_handle/node_handle_impl.h"
#include "ohos/adapter/xcomponent/event/node_handle_input_event_common.h"
#include "ohos/adapter/xcomponent/renderer/xcomponent_base.h"
#include "ohos/adapter/xcomponent/xcomponent_delegate.h"

namespace ohos::adapter::xcomponent {

/*
 * NodeHandleXComponentImpl is class used for handle OH native event
 * Decide the future processing direction of the event
 * Such as pass to delegate to modify window status or record context
 * Or pass to InputEventCallback for chromium UI event processing
 * All of the function in this class MUST run in ArkUI Main Thread
 */
class ADAPTER_EXPORT_API NodeHandleXComponentImpl : public XComponentBase {
 public:
  explicit NodeHandleXComponentImpl(const std::string& id, XComponentType type);
  ~NodeHandleXComponentImpl();

  void RegisterNodeHandleInputEventCallBack(
      std::shared_ptr<NodeHandleInputEventCallBack> callback);
  void InitializeWithNodeHandle(XComponentDelegate* delegate);
  void SetNodeHandle(ArkUI_NodeHandle node_handle);
  ArkUI_NodeHandle GetNodeHandle();
  void SetSurfaceHolder(OH_ArkUI_SurfaceHolder* surface_holder);
  void SetSurfaceCallback(OH_ArkUI_SurfaceCallback* surface_callback);
  bool BindNativeXComponentNode(ArkUI_NodeContentHandle node_content_handle);
  void RegisterCallBack();
  void UnRegisterCallBack();
  void RegisterPinchGestureEvent();
  void RegisterPanGestureEvent();
  void RegisterDoubleTapGestureEvent();
  void UnregisterPinchGestureEvent();
  void UnregisterPanGestureEvent();
  void UnregisterDoubleTapGestureEvent();
  ArkUI_GestureRecognizer* CreatePinchGesture();
  ArkUI_GestureRecognizer* CreatePanGesture();
  ArkUI_GestureRecognizer* CreateDoubleTapGesture();

  void OnTouchEvent(const ArkUI_UIInputEvent* touch_event);
  void OnMouseEvent(const ArkUI_UIInputEvent* mouse_event);
  void OnKeyEvent(const ArkUI_UIInputEvent* key_event);
  void OnHoverEvent(const bool is_hover);
  void OnBlurEvent();
  void OnFocusEvent();
  void OnSurfaceCreated();
  void OnSurfaceChanged();
  void OnSurfaceDestroyed();

  // DragEventCallBack
  void OnDragEnterEvent(const ohos::adapter::OhosDropData& drag_data);
  void OnDragMoveEvent(const float window_x, const float window_y);
  void OnDropEvent(const ohos::adapter::OhosDropData& drop_data);
  void OnDragLeaveEvent();
  void OnDragEndEvent();

  void OnPanEvent(const ArkUI_GestureEventActionType action_type,
                  const NodeHandlePanEvent& event);
  void OnPinchEvent(const ArkUI_GestureEventActionType pinch_step,
                    const NodeHandlePinchEvent& event);

  void SendWindowMouseEventForTabDrag(
      Input_MouseEvent* window_mouse_event) override;
  void SendWindowTouchEventForTabDrag(
      Input_TouchEvent* window_touch_event) override;

 private:
  ArkUI_NodeHandle node_handle_ = nullptr;
  OH_ArkUI_SurfaceHolder* surface_holder_ = nullptr;
  OH_ArkUI_SurfaceCallback* surface_callback_ = nullptr;
  std::shared_ptr<NodeHandleInputEventCallBack> event_callback_ = nullptr;

  void EventReissueAfterRegisterInputEventCallBack();
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_RENDERER_NODE_HANDLE_XCOMPONENT_IMPL_H_
