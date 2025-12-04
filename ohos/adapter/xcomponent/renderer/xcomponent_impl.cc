// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/xcomponent/renderer/xcomponent_impl.h"

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"

namespace ohos::adapter::xcomponent {

namespace {

std::string GetRenderId(OH_NativeXComponent* component) {
  char id_str[OH_XCOMPONENT_ID_LEN_MAX + 1] = {0};
  uint64_t id_size = OH_XCOMPONENT_ID_LEN_MAX + 1;
  int32_t ret =
      OH_NativeXComponent_GetXComponentId(component, id_str, &id_size);
  if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE("Unable to get xcomponent id for id_str:%{public}s", id_str);
    return std::string();
  }
  std::string xcomponent_id(id_str);
  std::size_t pos = xcomponent_id.find(":");
  if (pos == std::string::npos) {
    LOGE("xcomponent invalid id: %{public}s", id_str);
    return std::string();
  }
  return xcomponent_id.substr(0, pos);
}

XComponentType ConvertXComponentType(const std::string& type) {
  if (type == "xcomponent") {
    return XComponentType::kWindow;
  } else if (type == "subXcomponent") {
    return XComponentType::kSubWindow;
  } else {
    return XComponentType::kUnknown;
  }
}

std::shared_ptr<XComponentImpl> GetXComponent(const std::string& render_id) {
  if (render_id.empty()) {
    LOGW("render_id is empty in %{public}s.",
         __FUNCTION__);
    return nullptr;
  }
  auto manager = XComponentManager::GetInstance();
  if (!manager) {
    LOGW("Failed to get XComponentManager in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return nullptr;
  }
  auto impl = manager->GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get GetXComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return nullptr;
  }
  return impl;
}

}

void OnSurfaceCreatedCB(OH_NativeXComponent* component, void* window) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  if (render_id != impl->GetId()) {
    LOGE("xcomponent not matched, receive id %{public}s, id %{public}s",
         render_id.c_str(), impl->GetId().c_str());
    return;
  }

  uint64_t width;
  uint64_t height;
  int32_t ret =
      OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
  if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE("Get xcomponent surface size error:%{public}lu %{public}lu, render_id:%{public}s",
         width, height, render_id.c_str());
    return;
  }
  
  LOGI("Get xcomponent surface size success:%{public}lu %{public}lu, render_id:%{public}s",
       width, height, render_id.c_str());
  WindowAdapter::GetInstance().AddWindow(render_id, window);
  int32_t widget_id = WindowAdapter::GetInstance().GetWidgetId(render_id);
  impl->SetWidget(widget_id);
  impl->OnSurfaceCreated();
}

void OnSurfaceChangedCB(OH_NativeXComponent* component, void* window) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  impl->OnSurfaceChanged();
}

void OnSurfaceDestroyedCB(OH_NativeXComponent* component, void* window) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  if (render_id != impl->GetId()) {
    LOGE("xcomponent not matched, receive id %{public}s, id %{public}s",
         render_id.c_str(), impl->GetId().c_str());
    return;
  }
  LOGI("Destroy XComponent, render_id:%{public}s", render_id.c_str());
  WindowAdapter::GetInstance().RemoveWindow(render_id);

  impl->OnSurfaceDestroyed();
}

void OnBlurEventCB(OH_NativeXComponent* component, void* window) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  auto event = std::make_shared<SurfaceEvent>(EventType::ET_SURFACE_BLUR);
  WindowAdapter::GetInstance().NotifyWindowEvent(render_id, event);

  impl->OnBlurEvent();
}

void OnFocusEventCB(OH_NativeXComponent* component, void* window) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  auto event = std::make_shared<SurfaceEvent>(EventType::ET_SURFACE_FOCUS);
  WindowAdapter::GetInstance().NotifyWindowEvent(render_id, event);

  impl->OnFocusEvent();
}

void OnTouchEventCB(OH_NativeXComponent* component, void* window) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  OH_NativeXComponent_TouchEvent touch_event;
  int32_t ret =
      OH_NativeXComponent_GetTouchEvent(component, window, &touch_event);
  if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE("Get xcomponent touch event fail. render_id:%{public}s", render_id.c_str());
    return;
  }

  OH_NativeXComponent_TouchPointToolType tool_type =
      OH_NativeXComponent_TouchPointToolType::
          OH_NATIVEXCOMPONENT_TOOL_TYPE_UNKNOWN;
  OH_NativeXComponent_GetTouchPointToolType(component, 0, &tool_type);
  if (tool_type == OH_NATIVEXCOMPONENT_TOOL_TYPE_UNKNOWN) {
    // skip unknown tool type which will cause double click in pc
    return;
  }
  float tilt_x = 0.0f;
  float tilt_y = 0.0f;
  OH_NativeXComponent_GetTouchPointTiltX(component, 0, &tilt_x);
  OH_NativeXComponent_GetTouchPointTiltY(component, 0, &tilt_y);

  float display_x = 0.0f;
  float display_y = 0.0f;
  OH_NativeXComponent_GetTouchPointDisplayX(component, 0, &display_x);
  OH_NativeXComponent_GetTouchPointDisplayY(component, 0, &display_y);
  TouchPointCoordinate coordinate;
  coordinate.tilt_x = tilt_x;
  coordinate.tilt_y = tilt_y;
  coordinate.display_x = display_x;
  coordinate.display_y = display_y;

  impl->OnTouchEvent(touch_event, tool_type, std::move(coordinate));
}

void OnMouseEventCB(OH_NativeXComponent* component, void* window) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  OH_NativeXComponent_MouseEvent mouse_event;
  int32_t ret =
      OH_NativeXComponent_GetMouseEvent(component, window, &mouse_event);
  if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE("Get xcomponent mouse event fail. render_id:%{public}s", render_id.c_str());
    return;
  }

  if (mouse_event.action != OH_NATIVEXCOMPONENT_MOUSE_MOVE) {
    LOGI(
        "%{public}s: xcomponent mouse event x:%{public}f y:%{public}f "
        "screenX:%{public}f screenY:%{public}f timestamp:%{public}ld "
        ",action: %{public}d, "
        "button: %{public}d",
        __FUNCTION__, mouse_event.x, mouse_event.y, mouse_event.screenX,
        mouse_event.screenY, mouse_event.timestamp, mouse_event.action,
        mouse_event.button);
  }
  
  impl->OnMouseEvent(mouse_event);
}

void OnKeyEventCB(OH_NativeXComponent* component, void* window) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  OH_NativeXComponent_KeyEvent* key_event;
  int32_t ret = OH_NativeXComponent_GetKeyEvent(component, &key_event);
  if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE("Get xcomponent key event fail. render_id:%{public}s", render_id.c_str());
    return;
  }

  impl->OnKeyEvent(*key_event);
}

void OnHoverEventCB(OH_NativeXComponent* component, bool is_hover) {
  std::string render_id = GetRenderId(component);
  auto impl = GetXComponent(render_id);
  if (!impl) {
    LOGW("Failed to get XComponent instance in %{public}s. render_id:%{public}s",
         __FUNCTION__, render_id.c_str());
    return;
  }
  impl->OnHoverEvent(is_hover);
}

XComponentImpl::XComponentImpl(const std::string& id,
                               const std::string& type)
                               : XComponentBase(id) {
  this->type_ = ConvertXComponentType(type);

  surface_callback_ = {
    .OnSurfaceCreated = OnSurfaceCreatedCB,
    .OnSurfaceChanged = OnSurfaceChangedCB,
    .OnSurfaceDestroyed = OnSurfaceDestroyedCB,
    .DispatchTouchEvent = OnTouchEventCB
  };

  mouse_callback_ = {
    .DispatchMouseEvent = OnMouseEventCB,
    .DispatchHoverEvent = OnHoverEventCB
  };
}

XComponentImpl::~XComponentImpl() {
  LOGI("~XComponentImpl destructive XComponentImpl instance.");
  if (instance_) {
    instance_ = nullptr;
  }
}

void XComponentImpl::Initialize(OH_NativeXComponent* component,
                                XComponentDelegate* delegate) {
  LOGI("XComponentImpl initialize instance.");
  if (instance_ != nullptr &&
      instance_ != component) {
    LOGW("XComponentImpl RegisterCallback have already set xcomponent!");
  }
  instance_ = component;
  delegate_ = delegate;

  OH_NativeXComponent_RegisterCallback(instance_, &surface_callback_);
  OH_NativeXComponent_RegisterMouseEventCallback(instance_, &mouse_callback_);
  OH_NativeXComponent_RegisterKeyEventCallback(instance_, OnKeyEventCB);
  OH_NativeXComponent_RegisterBlurEventCallback(instance_, OnBlurEventCB);
  OH_NativeXComponent_RegisterFocusEventCallback(instance_, OnFocusEventCB);
}

void XComponentImpl::RegisterInputEventCallBack(
    std::shared_ptr<InputEventCallBack> callback) {
  event_callback_ = std::move(callback);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::EventReissueAfterRegisterInputEventCallBack() {
  if (has_hover_event_before_register_) {
    has_hover_event_before_register_ = false;
    event_callback_->mouse_hover_event_callback(GetWidget(),
                                                is_hover_before_register_);
    LOGW("XComponentImpl hover event"
         "was reissued after register event_callback_.");
  }
  is_event_reissuance_required_ = false;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnPanEvent(const PanAction action,
                                const PanEvent& event) {
  TRACE_EVENT_1("XComponentImpl::OnPanEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->pan_event_callback(action, GetWidget(), event);
  } else {
    LOGW("XComponentImpl pan event dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnPinchEvent(const std::string& pinch_step,
                                  const PinchEvent& event) {
  TRACE_EVENT_1("XComponentImpl::OnPinchEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->pinch_event_callback(pinch_step, GetWidget(), event);
  } else {
    LOGW("XComponentImpl pinch event dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnTouchEvent(
    const OH_NativeXComponent_TouchEvent& touch_event,
    const OH_NativeXComponent_TouchPointToolType tool_type,
    const TouchPointCoordinate& coordinate) {
  TRACE_EVENT_1("XComponentImpl::OnTouchEvent", "widget_id", GetWidget());
  if (event_callback_) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->touchEventCallback(GetWidget(),
                                        touch_event,
                                        tool_type,
                                        coordinate);
  } else {
    LOGW("XComponentImpl touch event dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnMouseEvent(
    const OH_NativeXComponent_MouseEvent& mouse_event) {
  if (event_callback_) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->mouseEventCallback(GetWidget(), mouse_event);
  } else {
    LOGW("XComponentImpl mouse event dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnKeyEvent(OH_NativeXComponent_KeyEvent& key_event) {
  TRACE_EVENT_1("XComponentImpl::OnKeyEvent", "widget_id", GetWidget());
  if (event_callback_) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->keyEventCallback(GetWidget(), key_event);
  } else {
    LOGW("XComponentImpl key event dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnHoverEvent(const bool is_hover) {
  if (event_callback_) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->mouse_hover_event_callback(GetWidget(), is_hover);
  } else {
    is_hover_before_register_ = is_hover;
    has_hover_event_before_register_ = true;
    is_event_reissuance_required_ = true;
    LOGW("XComponentImpl hover event"
         "was saved before register event_callback_.");
  }
}

void XComponentImpl::OnBlurEvent() {
  TRACE_EVENT_1("XComponentImpl::OnBlurEvent", "widget_id", GetWidget());
  delegate_->OnActivationChanged(GetId(), false);
}

void XComponentImpl::OnFocusEvent() {
  TRACE_EVENT_1("XComponentImpl::OnFocusEvent", "widget_id", GetWidget());
  delegate_->OnActivationChanged(GetId(), true);
}

void XComponentImpl::OnSurfaceCreated() {
  TRACE_EVENT_0("OnSurfaceCreated");
  delegate_->OnWidgetAvailable(GetId());
}

void XComponentImpl::OnSurfaceChanged() {
  TRACE_EVENT_0("OnSurfaceChanged");
}

void XComponentImpl::OnSurfaceDestroyed() {
  TRACE_EVENT_0("OnSurfaceDestroyed");
  delegate_->OnWidgetDestroyed(GetId());
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnDragEnterEvent(
    const ohos::adapter::OhosDropData& drop_data) {
  TRACE_EVENT_1("XComponentImpl::OnDragEnterEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dragEnterEventCallback(GetWidget(), drop_data);
  } else {
    LOGW("XComponentImpl drag enter event"
         "dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnDragMoveEvent(const float window_x,
                                     const float window_y) {
  TRACE_EVENT_1("XComponentImpl::OnDragMoveEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dragMoveEventCallback(GetWidget(), window_x, window_y);
  } else {
    LOGW("XComponentImpl drag move event"
         "dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnDropEvent(const ohos::adapter::OhosDropData& drop_data) {
  TRACE_EVENT_1("XComponentImpl::OnDropEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dropEventCallback(GetWidget(), drop_data);
  } else {
    LOGW("XComponentImpl drop event dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnDragLeaveEvent() {
  TRACE_EVENT_1("XComponentImpl::OnDragLeaveEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dragLeaveEventCallback(GetWidget());
  } else {
    LOGW("XComponentImpl drag leave event"
         "dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::OnDragEndEvent() {
  TRACE_EVENT_1("XComponentImpl::OnDragEndEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    if (is_event_reissuance_required_) {
      EventReissueAfterRegisterInputEventCallBack();
    }
    event_callback_->dragEndEventCallback(GetWidget());
  } else {
    LOGW("XComponentImpl drag end event"
         "dropped before register event_callback_.");
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::SendWindowMouseEventForTabDrag(
    Input_MouseEvent* window_mouse_event) {
  if (event_callback_ != nullptr && window_mouse_event != nullptr) {
    event_callback_->sendWindowMouseEventForTabDragCallback(GetWidget(),
                                                            window_mouse_event);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void XComponentImpl::SendWindowTouchEventForTabDrag(
    Input_TouchEvent* window_touch_event) {
  if (event_callback_ != nullptr && window_touch_event != nullptr) {
    event_callback_->sendWindowTouchEventForTabDragCallback(GetWidget(),
                                                            window_touch_event);
  }
}

}  // namespace ohos::adapter::xcomponent
