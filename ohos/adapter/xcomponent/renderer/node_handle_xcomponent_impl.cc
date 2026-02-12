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

#include "ohos/adapter/xcomponent/renderer/node_handle_xcomponent_impl.h"

#include <arkui/native_interface.h>
#include <arkui/native_key_event.h>
#include <arkui/native_node.h>
#include <native_window/external_window.h>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/drag_drop/node_handle_drag_drop_ohos_adapter.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/node_handle_util.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"

namespace ohos::adapter::xcomponent {
using ohos::adapter::NodeHandleDragDropOhosAdapter;

namespace {
static constexpr int32_t kPanGestureTriggerMinFingerNum = 1;
static constexpr float kPanGestureTriggerMinDistanceNum = 1;
static constexpr int32_t kPinchGestureTriggerMinFingerNum = 2;
static constexpr float kPinchGestureTriggerMinDistanceNum = 5;
static constexpr int32_t kDoubleTapCountNum = 2;
static constexpr int32_t kDoubleTapFingerNum = 1;

__attribute__((no_sanitize("cfi", "cfi-icall")))
std::string GetRenderId(ArkUI_NodeHandle node_handle) {
  if (!node_handle) {
    LOGE("nodehandle is nullptr");
    return std::string();
  }
  ArkUI_NativeNodeAPI_1* node_api = GetNativeNodeAPI();
  if (node_api == nullptr) {
    LOGE("unable to get nodeAPI");
    return std::string();
  }
  auto node_id_item = node_api->getAttribute(node_handle, NODE_ID);
  if (node_id_item == nullptr) {
    LOGE("unable to get xcomponent id");
    return std::string();
  }
  std::string xcomponent_id(node_id_item->string);
  return xcomponent_id;
}

std::shared_ptr<NodeHandleXComponentImpl> GetXComponent(
    const std::string& render_id) {
  if (render_id.empty()) {
    LOGW("render_id is empty in %{public}s.", __FUNCTION__);
    return nullptr;
  }
  auto manager = XComponentManager::GetInstance();
  if (!manager) {
    LOGW("Failed to get XComponentManager in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return nullptr;
  }
  auto impl = manager->GetNodeHandleXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get GetXComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return nullptr;
  }
  return impl;
}

void OnSurfaceCreatedCB(OH_ArkUI_SurfaceHolder* holder) {
  ArkUI_NodeHandle node_handle = static_cast<ArkUI_NodeHandle>(
      nodeHandle::NodeHandleImpl::GetInstance().GetUserData(holder));
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  OHNativeWindow* native_window =
      nodeHandle::NodeHandleImpl::GetInstance().GetNativeWindow(holder);
  WindowAdapter::GetInstance().AddWindow(render_id, native_window);
  int32_t widget_id = WindowAdapter::GetInstance().GetWidgetId(render_id);
  impl->SetWidget(widget_id);
  impl->OnSurfaceCreated();
}

void OnSurfaceChangedCB(OH_ArkUI_SurfaceHolder* holder,
                        uint64_t width,
                        uint64_t height) {
  ArkUI_NodeHandle node_handle = static_cast<ArkUI_NodeHandle>(
      nodeHandle::NodeHandleImpl::GetInstance().GetUserData(holder));
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  impl->OnSurfaceChanged();
}

void OnSurfaceDestroyedCB(OH_ArkUI_SurfaceHolder* holder) {
  ArkUI_NodeHandle node_handle = static_cast<ArkUI_NodeHandle>(
      nodeHandle::NodeHandleImpl::GetInstance().GetUserData(holder));
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  WindowAdapter::GetInstance().RemoveWindow(render_id);

  impl->OnSurfaceDestroyed();
}

void OnBlurEvent(ArkUI_NodeHandle node_handle) {
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }
  auto event = std::make_shared<SurfaceEvent>(EventType::ET_SURFACE_BLUR);
  WindowAdapter::GetInstance().NotifyWindowEvent(render_id, event);

  impl->OnBlurEvent();
}

void OnFocusEvent(ArkUI_NodeHandle node_handle) {
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }
  auto event = std::make_shared<SurfaceEvent>(EventType::ET_SURFACE_FOCUS);
  WindowAdapter::GetInstance().NotifyWindowEvent(render_id, event);

  impl->OnFocusEvent();
}

void OnKeyEvent(ArkUI_NodeEvent* node_event, ArkUI_NodeHandle node_handle) {
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  ArkUI_UIInputEvent* key_event = OH_ArkUI_NodeEvent_GetInputEvent(node_event);
  if (!key_event) {
    LOGE("get xcomponent key event fail.");
    return;
  }
  impl->OnKeyEvent(key_event);
}

void OnHoverEvent(ArkUI_NodeEvent* node_event, ArkUI_NodeHandle node_handle) {
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  ArkUI_NodeComponentEvent* hover_event =
      OH_ArkUI_NodeEvent_GetNodeComponentEvent(node_event);
  if (hover_event == nullptr) {
    LOGE("get xcomponent hover event fail.");
    return;
  }

  impl->OnHoverEvent(hover_event->data[0].i32);
}

void OnMouseEvent(ArkUI_NodeEvent* node_event, ArkUI_NodeHandle node_handle) {
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  ArkUI_UIInputEvent* mouse_event =
      OH_ArkUI_NodeEvent_GetInputEvent(node_event);
  if (mouse_event == nullptr) {
    LOGE("get xcomponent mouse event fail.");
    return;
  }

  if (NodeHandleDragDropOhosAdapter::GetInstance().IsDraggingStarted()) {
    auto action = OH_ArkUI_UIInputEvent_GetAction(mouse_event);
    // When Chromium is dragging, intercept the mouse events send to Chromium.
    if (action == UI_MOUSE_EVENT_ACTION_RELEASE) {
      LOGW(
          "[OhosDragNodeHandle]Receive mouse release event, drag is ended, "
          "SetDraggingStarted false, "
          "render_id:%{public}s",
          render_id.c_str());
      NodeHandleDragDropOhosAdapter::GetInstance().SetDraggingStarted(false);
      return;
    }
    if (action == UI_MOUSE_EVENT_ACTION_PRESS) {
      LOGW(
          "[OhosDragNodeHandle]Receive mouse press event, "
          "IsDraggingStarted value is invalid, SetDraggingStarted false, "
          "render_id:%{public}s",
          render_id.c_str());
      NodeHandleDragDropOhosAdapter::GetInstance().SetDraggingStarted(false);
    }
  }

  impl->OnMouseEvent(mouse_event);
}

void OnTouchEvent(ArkUI_NodeEvent* node_event, ArkUI_NodeHandle node_handle) {
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  ArkUI_UIInputEvent* touch_event =
      OH_ArkUI_NodeEvent_GetInputEvent(node_event);
  if (touch_event == nullptr) {
    LOGE("get xcomponent touch event fail.");
    return;
  }

  int32_t tool_type = OH_ArkUI_UIInputEvent_GetToolType(touch_event);
  if (tool_type == UI_INPUT_EVENT_TOOL_TYPE_MOUSE ||
      tool_type == UI_INPUT_EVENT_TOOL_TYPE_UNKNOWN) {
    // skip unknown tool type which will cause double click in pc
    return;
  }

  impl->OnTouchEvent(touch_event);
}

void OnNodeEventCB(ArkUI_NodeEvent* event) {
  if (event == nullptr) {
    LOGE("OnNodeEventCB receive node_event input nullptr");
    return;
  }

  ArkUI_NodeHandle node_handle = OH_ArkUI_NodeEvent_GetNodeHandle(event);
  if (node_handle == nullptr) {
    LOGE("OnNodeEventCB retrieve node handle from node event failed");
    return;
  }

  ArkUI_NodeEventType event_type = OH_ArkUI_NodeEvent_GetEventType(event);
  switch (event_type) {
    case NODE_ON_BLUR:
      OnBlurEvent(node_handle);
      break;
    case NODE_ON_FOCUS:
      OnFocusEvent(node_handle);
      break;
    case NODE_TOUCH_EVENT:
      OnTouchEvent(event, node_handle);
      break;
    case NODE_ON_MOUSE:
      OnMouseEvent(event, node_handle);
      break;
    case NODE_ON_HOVER:
      OnHoverEvent(event, node_handle);
      break;
    case NODE_ON_KEY_EVENT:
      OnKeyEvent(event, node_handle);
      break;
    case NODE_ON_DRAG_ENTER: {
      auto render_id = GetRenderId(node_handle);
      NodeHandleDragDropOhosAdapter::GetInstance().OnDragEnterCB(render_id,
                                                                 event);
      break;
    }
    case NODE_ON_DRAG_MOVE: {
      auto render_id = GetRenderId(node_handle);
      NodeHandleDragDropOhosAdapter::GetInstance().OnDragMoveCB(render_id,
                                                                event);
      break;
    }
    case NODE_ON_DRAG_LEAVE: {
      auto render_id = GetRenderId(node_handle);
      NodeHandleDragDropOhosAdapter::GetInstance().OnDragLeaveCB(render_id,
                                                                 event);
      break;
    }
    case NODE_ON_DROP: {
      auto render_id = GetRenderId(node_handle);
      NodeHandleDragDropOhosAdapter::GetInstance().OnDropCB(render_id, event);
      break;
    }
    default:
      LOGE("OnNodeEventCB unknown node event type %{public}d", event_type);
  }
}

void OnPinchEventCB(ArkUI_GestureEvent* event, ArkUI_NodeHandle node_handle) {
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  const ArkUI_UIInputEvent* input_event =
      OH_ArkUI_GestureEvent_GetRawInputEvent(event);
  int32_t tool_type = OH_ArkUI_UIInputEvent_GetToolType(input_event);
  if (tool_type == UI_INPUT_EVENT_TOOL_TYPE_FINGER) {
    return;
  }

  float offset_x = OH_ArkUI_PinchGesture_GetCenterX(event);
  float offset_y = OH_ArkUI_PinchGesture_GetCenterY(event);
  float scale = OH_ArkUI_PinchGesture_GetScale(event);
  auto action_type = OH_ArkUI_GestureEvent_GetActionType(event);
  NodeHandlePinchEvent pinch_event(PINCH_GESTURE, tool_type, offset_x, offset_y, scale);

  impl->OnPinchEvent(action_type, pinch_event);
}

void OnPanEventCB(ArkUI_GestureEvent* event, ArkUI_NodeHandle node_handle) {
  auto render_id = GetRenderId(node_handle);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW(
        "Failed to get NodeHandle XComponent instance in %{public}s. "
        "render_id:%{public}s",
        __FUNCTION__, render_id.c_str());
    return;
  }

  const ArkUI_UIInputEvent* input_event =
      OH_ArkUI_GestureEvent_GetRawInputEvent(event);
  int32_t tool_type = OH_ArkUI_UIInputEvent_GetToolType(input_event);
  int32_t mouse_button = OH_ArkUI_MouseEvent_GetMouseButton(input_event);
  if (tool_type == UI_INPUT_EVENT_TOOL_TYPE_UNKNOWN ||
      tool_type == UI_INPUT_EVENT_TOOL_TYPE_FINGER ||
      (tool_type == UI_INPUT_EVENT_TOOL_TYPE_MOUSE &&
       mouse_button == UI_MOUSE_EVENT_BUTTON_NONE)) {
    // skip unknow source tool and touchscreen slide
    // The pan event will be triggered when the mouse is moved with a long
    // press, when the source tool is mouse and button is none
    return;
  }

  float offset_x = OH_ArkUI_PanGesture_GetOffsetX(event);
  float offset_y = OH_ArkUI_PanGesture_GetOffsetY(event);
  float velocity_x = OH_ArkUI_PanGesture_GetVelocityX(event);
  float velocity_y = OH_ArkUI_PanGesture_GetVelocityY(event);
  NodeHandlePanEvent pan_event(PAN_GESTURE, tool_type, offset_x, offset_y, velocity_x,
                               velocity_y);
  ArkUI_GestureEventActionType action_type =
      OH_ArkUI_GestureEvent_GetActionType(event);

  impl->OnPanEvent(action_type, pan_event);
}

}  // namespace

NodeHandleXComponentImpl::NodeHandleXComponentImpl(const std::string& id,
                                                   XComponentType type)
    : XComponentBase(id) {
  this->type_ = type;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
NodeHandleXComponentImpl::~NodeHandleXComponentImpl() {
  if (surface_holder_) {
    nodeHandle::NodeHandleImpl::GetInstance().SurfaceHolderDispose(
        surface_holder_);
    surface_holder_ = nullptr;
  }
  if (surface_callback_) {
    nodeHandle::NodeHandleImpl::GetInstance().SurfaceCallbackDispose(
        surface_callback_);
    surface_callback_ = nullptr;
  }
  if (node_handle_) {
    UnRegisterCallBack();
    GetNativeNodeAPI()->disposeNode(node_handle_);
    node_handle_ = nullptr;
  }
}

void NodeHandleXComponentImpl::RegisterNodeHandleInputEventCallBack(
    std::shared_ptr<NodeHandleInputEventCallBack> callback) {
  event_callback_ = std::move(callback);
}

void NodeHandleXComponentImpl::SetNodeHandle(ArkUI_NodeHandle node_handle) {
  node_handle_ = node_handle;
}

ArkUI_NodeHandle NodeHandleXComponentImpl::GetNodeHandle() {
  return node_handle_;
}

void NodeHandleXComponentImpl::SetSurfaceHolder(
    OH_ArkUI_SurfaceHolder* surface_holder) {
  surface_holder_ = surface_holder;
}

void NodeHandleXComponentImpl::SetSurfaceCallback(
    OH_ArkUI_SurfaceCallback* surface_callback) {
  surface_callback_ = surface_callback;
}

void NodeHandleXComponentImpl::InitializeWithNodeHandle(
    XComponentDelegate* delegate) {
  delegate_ = delegate;
  RegisterCallBack();
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::RegisterCallBack() {
  nodeHandle::NodeHandleImpl::GetInstance().SetSurfaceCreatedEvent(
      surface_callback_, OnSurfaceCreatedCB);
  nodeHandle::NodeHandleImpl::GetInstance().SetSurfaceChangedEvent(
      surface_callback_, OnSurfaceChangedCB);
  nodeHandle::NodeHandleImpl::GetInstance().SetSurfaceDestroyedEvent(
      surface_callback_, OnSurfaceDestroyedCB);
  nodeHandle::NodeHandleImpl::GetInstance().AddSurfaceCallback(
      surface_holder_, surface_callback_);

  ArkUI_NativeNodeAPI_1* node_api = GetNativeNodeAPI();
  node_api->registerNodeEvent(node_handle_, NODE_TOUCH_EVENT, 0, nullptr);
  node_api->registerNodeEvent(node_handle_, NODE_ON_MOUSE, 0, nullptr);
  node_api->registerNodeEvent(node_handle_, NODE_ON_HOVER, 0, nullptr);
  node_api->registerNodeEvent(node_handle_, NODE_ON_BLUR, 0, nullptr);
  node_api->registerNodeEvent(node_handle_, NODE_ON_FOCUS, 0, nullptr);
  node_api->registerNodeEvent(node_handle_, NODE_ON_KEY_EVENT, 0, nullptr);
  // register drag event callback
  node_api->registerNodeEvent(node_handle_, NODE_ON_DRAG_ENTER, 0, nullptr);
  node_api->registerNodeEvent(node_handle_, NODE_ON_DRAG_MOVE, 0, nullptr);
  node_api->registerNodeEvent(node_handle_, NODE_ON_DRAG_LEAVE, 0, nullptr);
  node_api->registerNodeEvent(node_handle_, NODE_ON_DROP, 0, nullptr);

  node_api->setLengthMetricUnit(node_handle_, ARKUI_LENGTH_METRIC_UNIT_PX);
  node_api->addNodeEventReceiver(node_handle_, OnNodeEventCB);

  RegisterPinchGestureEvent();
  RegisterPanGestureEvent();
  RegisterDoubleTapGestureEvent();
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::UnRegisterCallBack() {
  ArkUI_NativeNodeAPI_1* node_api = GetNativeNodeAPI();
  node_api->unregisterNodeEvent(node_handle_, NODE_TOUCH_EVENT);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_MOUSE);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_HOVER);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_BLUR);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_FOCUS);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_KEY_EVENT);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_DRAG_ENTER);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_DRAG_MOVE);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_DRAG_LEAVE);
  node_api->unregisterNodeEvent(node_handle_, NODE_ON_DROP);
  node_api->removeNodeEventReceiver(node_handle_, OnNodeEventCB);

  UnregisterPinchGestureEvent();
  UnregisterPanGestureEvent();
  UnregisterDoubleTapGestureEvent();
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::RegisterPinchGestureEvent() {
  ArkUI_GestureRecognizer* pinch_gesture = CreatePinchGesture();
  ArkUI_NativeGestureAPI_1* gesture_api = GetGestureAPI();
  if (pinch_gesture && gesture_api) {
    auto pinch_gesture_callback = [](ArkUI_GestureEvent* event,
                                     void* extra_param) {
      ArkUI_NodeHandle node = static_cast<ArkUI_NodeHandle>(extra_param);
      OnPinchEventCB(event, node);
    };
    gesture_api->setGestureEventTarget(pinch_gesture,
                                       GESTURE_EVENT_ACTION_ACCEPT |
                                           GESTURE_EVENT_ACTION_UPDATE |
                                           GESTURE_EVENT_ACTION_END,
                                       node_handle_, pinch_gesture_callback);
    gesture_api->addGestureToNode(node_handle_, pinch_gesture, NORMAL,
                                  NORMAL_GESTURE_MASK);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::RegisterPanGestureEvent() {
  ArkUI_GestureRecognizer* pan_gesture = CreatePanGesture();
  ArkUI_NativeGestureAPI_1* gesture_api = GetGestureAPI();
  if (pan_gesture && gesture_api) {
    auto pan_gesture_callback = [](ArkUI_GestureEvent* event,
                                   void* extra_param) {
      ArkUI_NodeHandle node = static_cast<ArkUI_NodeHandle>(extra_param);
      OnPanEventCB(event, node);
    };
    gesture_api->setGestureEventTarget(pan_gesture,
                                       GESTURE_EVENT_ACTION_ACCEPT |
                                           GESTURE_EVENT_ACTION_UPDATE |
                                           GESTURE_EVENT_ACTION_END,
                                       node_handle_, pan_gesture_callback);
    gesture_api->addGestureToNode(node_handle_, pan_gesture, NORMAL,
                                  NORMAL_GESTURE_MASK);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::RegisterDoubleTapGestureEvent() {
  ArkUI_GestureRecognizer* double_tap_gesture = CreateDoubleTapGesture();
  ArkUI_NativeGestureAPI_1* gesture_api = GetGestureAPI();
  if (double_tap_gesture && gesture_api) {
    auto double_tap_gesture_callback = [](ArkUI_GestureEvent* event,
                                          void* extra_param) {
      // when double tap is triggered, we do nothing, because the event
      // processing flow in Chromium will converts mouse and touch events to
      // double tap events.
    };
    gesture_api->setGestureEventTarget(
        double_tap_gesture,
        GESTURE_EVENT_ACTION_ACCEPT | GESTURE_EVENT_ACTION_UPDATE |
            GESTURE_EVENT_ACTION_END | GESTURE_EVENT_ACTION_CANCEL,
        node_handle_, double_tap_gesture_callback);
    gesture_api->addGestureToNode(node_handle_, double_tap_gesture, NORMAL,
                                  NORMAL_GESTURE_MASK);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::UnregisterPinchGestureEvent() {
  ArkUI_GestureRecognizer* pinch_gesture = CreatePinchGesture();
  ArkUI_NativeGestureAPI_1* gesture_api = GetGestureAPI();
  if (pinch_gesture && gesture_api) {
    gesture_api->removeGestureFromNode(node_handle_, pinch_gesture);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::UnregisterPanGestureEvent() {
  ArkUI_GestureRecognizer* pan_gesture = CreatePanGesture();
  ArkUI_NativeGestureAPI_1* gesture_api = GetGestureAPI();
  if (pan_gesture && gesture_api) {
    gesture_api->removeGestureFromNode(node_handle_, pan_gesture);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::UnregisterDoubleTapGestureEvent() {
  ArkUI_GestureRecognizer* double_tap_gesture = CreateDoubleTapGesture();
  ArkUI_NativeGestureAPI_1* gesture_api = GetGestureAPI();
  if (double_tap_gesture && gesture_api) {
    gesture_api->removeGestureFromNode(node_handle_, double_tap_gesture);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
ArkUI_GestureRecognizer* NodeHandleXComponentImpl::CreatePinchGesture() {
  auto gesture_api = GetGestureAPI();
  ArkUI_GestureRecognizer* pinch_gesture = nullptr;
  if (gesture_api) {
    pinch_gesture = gesture_api->createPinchGesture(
        kPinchGestureTriggerMinFingerNum, kPinchGestureTriggerMinDistanceNum);
  }
  return pinch_gesture;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
ArkUI_GestureRecognizer* NodeHandleXComponentImpl::CreatePanGesture() {
  auto gesture_api = GetGestureAPI();
  ArkUI_GestureRecognizer* pan_gesture = nullptr;
  if (gesture_api) {
    pan_gesture = gesture_api->createPanGesture(
        kPanGestureTriggerMinFingerNum,
        GESTURE_DIRECTION_HORIZONTAL | GESTURE_DIRECTION_VERTICAL,
        kPanGestureTriggerMinDistanceNum);
  }
  return pan_gesture;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
ArkUI_GestureRecognizer* NodeHandleXComponentImpl::CreateDoubleTapGesture() {
  auto gesture_api = GetGestureAPI();
  ArkUI_GestureRecognizer* double_tap_gesture = nullptr;
  if (gesture_api) {
    double_tap_gesture =
        gesture_api->createTapGesture(kDoubleTapCountNum, kDoubleTapFingerNum);
  }
  return double_tap_gesture;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
bool NodeHandleXComponentImpl::BindNativeXComponentNode(
    ArkUI_NodeContentHandle node_content_handle) {
  if (node_handle_ == nullptr) {
    LOGE(
        "NodeHandleXComponentImpl::bindNativeXComponentNode node handle is not "
        "initialized");
    return false;
  }

  auto result = OH_ArkUI_NodeContent_AddNode(node_content_handle, node_handle_);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "NodeHandleXComponentImpl::bindNativeXComponentNode failed error code "
        "is %{public}d",
        result);
    return false;
  }

  ArkUI_NativeNodeAPI_1* node_api = GetNativeNodeAPI();
  if (node_api == nullptr) {
    LOGE("%{public}s get native node api failed", GetId().c_str());
    return false;
  }
  ArkUI_NumberValue node_focus_status_value[] = {{.i32 = 1}};
  ArkUI_AttributeItem node_focus_status_item = {node_focus_status_value, 1};
  result = node_api->setAttribute(node_handle_, NODE_FOCUS_STATUS,
                                  &node_focus_status_item);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s set NODE_FOCUS_STATUS attribute failed, res code is "
        "%{public}d",
        GetId().c_str(), result);
    return false;
  }

  return true;
}

bool NodeHandleXComponentImpl::UnBindNativeXComponentNode(
    ArkUI_NodeContentHandle node_content_handle) {
  if (node_handle_ == nullptr) {
    LOGE(
        "NodeHandleXComponentImpl::UnBindNativeXComponentNode node handle is "
        "not initialized");
    return false;
  }

  auto result =
      OH_ArkUI_NodeContent_RemoveNode(node_content_handle, node_handle_);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "NodeHandleXComponentImpl::UnBindNativeXComponentNode failed error "
        "code is %{public}d",
        result);
    return false;
  }

  return true;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::EventReissueAfterRegisterInputEventCallBack() {
  if (has_hover_event_before_register_) {
    has_hover_event_before_register_ = false;
    event_callback_->mouseHoverEventCallback(GetWidget(),
                                             is_hover_before_register_);
    LOGW(
        "NodeHandleXComponentImpl hover event"
        "was reissued after register event_callback_.");
  }
  is_event_reissuance_required_ = false;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnTouchEvent(
    const ArkUI_UIInputEvent* touch_event) {
  if (event_callback_) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->touchEventCallback(GetWidget(), touch_event);
  } else {
    LOGW(
        "NodeHandleXComponentImpl touch event dropped before register "
        "event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnMouseEvent(
    const ArkUI_UIInputEvent* mouse_event) {
  if (event_callback_) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->mouseEventCallback(GetWidget(), mouse_event);
  } else {
    LOGW(
        "NodeHandleXComponentImpl mouse event dropped before register "
        "event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnKeyEvent(const ArkUI_UIInputEvent* key_event) {
  if (event_callback_) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->keyEventCallback(GetWidget(), key_event);
  } else {
    LOGW(
        "NodeHandleXComponentImpl key event dropped before register "
        "event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnHoverEvent(const bool is_hover) {
  if (event_callback_) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->mouseHoverEventCallback(GetWidget(), is_hover);
  } else {
    is_hover_before_register_ = is_hover;
    has_hover_event_before_register_ = true;
    is_event_reissuance_required_ = true;
    LOGW(
        "NodeHandleXComponentImpl hover event"
        "was saved before register event_callback_.");
  }
}

void NodeHandleXComponentImpl::OnBlurEvent() {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnBlurEvent", "widget_id",
                GetWidget());
  delegate_->OnActivationChanged(GetId(), false);
}

void NodeHandleXComponentImpl::OnFocusEvent() {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnFocusEvent", "widget_id",
                GetWidget());
  delegate_->OnActivationChanged(GetId(), true);
}

void NodeHandleXComponentImpl::OnSurfaceCreated() {
  TRACE_EVENT_0("OnSurfaceCreated");
  delegate_->OnWidgetAvailable(GetId());
}

void NodeHandleXComponentImpl::OnSurfaceChanged() {
  TRACE_EVENT_0("OnSurfaceChanged");
}

void NodeHandleXComponentImpl::OnSurfaceDestroyed() {
  TRACE_EVENT_0("OnSurfaceDestroyed");
  delegate_->OnWidgetDestroyed(GetId());
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnDragEnterEvent(
    const ohos::adapter::OhosDropData& drag_data) {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnDragEnterEvent", "widget_id",
                GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dragEnterEventCallback(GetWidget(), drag_data);
  } else {
    LOGW(
        "NodeHandleXComponentImpl drag enter event"
        "dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnDragMoveEvent(const float window_x,
                                               const float window_y) {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnDragMoveEvent", "widget_id",
                GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dragMoveEventCallback(GetWidget(), window_x, window_y);
  } else {
    LOGW(
        "NodeHandleXComponentImpl drag move event"
        "dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnDropEvent(
    const ohos::adapter::OhosDropData& drop_data) {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnDropEvent", "widget_id",
                GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dropEventCallback(GetWidget(), drop_data);
  } else {
    LOGW(
        "NodeHandleXComponentImpl drop event dropped before register "
        "event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnDragLeaveEvent() {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnDragLeaveEvent", "widget_id",
                GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dragLeaveEventCallback(GetWidget());
  } else {
    LOGW(
        "NodeHandleXComponentImpl drag leave event"
        "dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnDragEndEvent() {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnDragEndEvent", "widget_id",
                GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dragEndEventCallback(GetWidget());
  } else {
    LOGW(
        "NodeHandleXComponentImpl drag end event"
        "dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnPanEvent(
    const ArkUI_GestureEventActionType action_type,
    const NodeHandlePanEvent& event) {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnPanEvent", "widget_id",
                GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->panEventCallback(action_type, GetWidget(), event);
  } else {
    LOGW(
        "NodeHandleXComponentImpl pan event dropped before register "
        "event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::OnPinchEvent(
    const ArkUI_GestureEventActionType action_type,
    const NodeHandlePinchEvent& event) {
  TRACE_EVENT_1("NodeHandleXComponentImpl::OnPinchEvent", "widget_id",
                GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->pinchEventCallback(action_type, GetWidget(), event);
  } else {
    LOGW(
        "NodeHandleXComponentImpl pinch event dropped before register "
        "event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::SendWindowMouseEventForTabDragNodeHandle(
    NodeHandleMouseEventData& window_mouse_data) {
  if (event_callback_ != nullptr) {
    event_callback_->sendWindowMouseEventForTabDragCallback(GetWidget(),
                                                            window_mouse_data);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void NodeHandleXComponentImpl::SendWindowTouchEventForTabDragNodeHandle(
    NodeHandleTouchEventData& window_touch_data) {
  if (event_callback_ != nullptr) {
    event_callback_->sendWindowTouchEventForTabDragCallback(GetWidget(),
                                                            window_touch_data);
  }
}

}  // namespace ohos::adapter::xcomponent
