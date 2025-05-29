// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/xcomponent/renderer/xcomponent_impl.h"

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/event/native_event_receiver.h"

namespace ohos::adapter::xcomponent {

namespace {

std::string GetRenderId(OH_NativeXComponent* component) {
  char id_str[OH_XCOMPONENT_ID_LEN_MAX + 1] = {0};
  uint64_t id_size = OH_XCOMPONENT_ID_LEN_MAX + 1;
  int32_t ret =
      OH_NativeXComponent_GetXComponentId(component, id_str, &id_size);
  if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE("unable to get xcomponent id");
    return std::string();
  }
  return std::string(id_str);
}

}  // namespace

class NativeEventCallBack final : public NativeEventListener {
 public:
  explicit NativeEventCallBack(XComponentImpl* impl) { impl_ = impl; }

  NativeEventCallBack(const NativeEventCallBack&) = delete;
  NativeEventCallBack& operator=(const NativeEventCallBack&) = delete;

  ~NativeEventCallBack() = default;

  void OnTouchEvent(OH_NativeXComponent* component, void* window) override {
    OH_NativeXComponent_TouchEvent touch_event;
    int32_t ret =
        OH_NativeXComponent_GetTouchEvent(component, window, &touch_event);
    if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
      LOGE("get xcomponent touch event fail.");
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

    impl_->OnTouchEvent(touch_event, tool_type, tilt_x, tilt_y);
  }

  void OnMouseEvent(OH_NativeXComponent* component, void* window) override {
    OH_NativeXComponent_MouseEvent mouse_event;
    int32_t ret =
        OH_NativeXComponent_GetMouseEvent(component, window, &mouse_event);
    if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
      LOGE("get xcomponent mouse event fail.");
      return;
    }

    impl_->OnMouseEvent(mouse_event);
  }

  void OnKeyEvent(OH_NativeXComponent* component, void* window) override {
    OH_NativeXComponent_KeyEvent* key_event;
    int32_t ret = OH_NativeXComponent_GetKeyEvent(component, &key_event);
    if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
      LOGE("get xcomponent key event fail.");
      return;
    }

    impl_->OnKeyEvent(*key_event);
  }

  void OnHoverEvent(OH_NativeXComponent* component,
                    const bool is_hover) override {
    impl_->OnHoverEvent(is_hover);
  }

  void OnBlurEvent(OH_NativeXComponent* component, void* window) override {
    auto event = std::make_shared<SurfaceEvent>(EventType::ET_SURFACE_BLUR);
    WindowAdapter::GetInstance().NotifyWindowEvent(window, event);

    impl_->OnBlurEvent();
  }

  void OnFocusEvent(OH_NativeXComponent* component, void* window) override {
    auto event = std::make_shared<SurfaceEvent>(EventType::ET_SURFACE_FOCUS);
    WindowAdapter::GetInstance().NotifyWindowEvent(window, event);

    impl_->OnFocusEvent();
  }

  void OnSurfaceCreated(OH_NativeXComponent* component, void* window) override {
    auto renderId = GetRenderId(component);
    if (renderId.empty()) {
      return;
    }
    if (renderId != impl_->GetId()) {
      LOGE("xcomponent not matched, receive id %{public}s, id %{public}s",
           renderId.c_str(), impl_->GetId().c_str());
      return;
    }

    uint64_t width;
    uint64_t height;
    int32_t ret = OH_NativeXComponent_GetXComponentSize(component, window,
                                                        &width, &height);
    if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
      LOGE("get xcomponent surface size error:%{public}lu %{public}lu", width,
           height);
      return;
    }

    WindowAdapter::GetInstance().AddWindow(renderId, window);
    int32_t widget_id = WindowAdapter::GetInstance().GetWidgetId(renderId);
    impl_->SetWidget(widget_id);
    impl_->SetInitialBounds(width, height);
    impl_->OnSurfaceCreated();

    // OH may lost surface focus event in create xcomponent, trigger manually
    this->OnFocusEvent(component, window);
  }

  void OnSurfaceChanged(OH_NativeXComponent* component, void* window) override {
    impl_->OnSurfaceChanged();
  }

  void OnSurfaceDestroyed(OH_NativeXComponent* component,
                          void* window) override {
    auto renderId = GetRenderId(component);
    if (renderId.empty()) {
      return;
    }
    if (renderId != impl_->GetId()) {
      LOGE("xcomponent not matched, receive id %{public}s, id %{public}s",
           renderId.c_str(), impl_->GetId().c_str());
      return;
    }
    WindowAdapter::GetInstance().RemoveWindow(renderId);

    impl_->OnSurfaceDestroyed();
  }

  void OnSurfaceShow(OH_NativeXComponent* component, void* window) override {
    LOGI("OnSurfaceShow");
  }

  void OnSurfaceHide(OH_NativeXComponent* component, void* window) override {
    LOGI("OnSurfaceHide");
  }

 private:
  XComponentImpl* impl_;
};

XComponentImpl::XComponentImpl(const std::string& id) {
  this->id_ = id;
}

XComponentImpl::~XComponentImpl() {
  if (instance_) {
    NativeEventReceiver::GetInstance().UnRegisterCallBack(instance_);
    instance_ = nullptr;
  }
}

void XComponentImpl::Initialize(OH_NativeXComponent* component,
                                XComponentDelegate* delegate) {
  if (instance_ != nullptr && instance_ != component) {
    LOGW("RegisterCallback have already set xcomponent!");
  }
  instance_ = component;
  delegate_ = delegate;
  auto callback = std::make_shared<NativeEventCallBack>(this);
  NativeEventReceiver::GetInstance().RegisterCallBack(instance_, callback);
}

void XComponentImpl::SetInitialBounds(int32_t initial_width,
                                      int32_t initial_height) {
  initial_width_ = initial_width;
  initial_height_ = initial_height;
}

void XComponentImpl::RegisterInputEventCallBack(
    std::shared_ptr<InputEventCallBack> callback) {
  event_callback_ = std::move(callback);
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnPanEvent(const PanAction action, const PanEvent& event) {
  TRACE_EVENT_1("XComponentImpl::OnPanEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    event_callback_->pan_event_callback(action, GetWidget(), event);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnPinchEvent(const std::string& pinch_step,
                             const PinchEvent& event) {
  TRACE_EVENT_1("XComponentImpl::OnPinchEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    event_callback_->pinch_event_callback(pinch_step, GetWidget(), event);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnTouchEvent(
    const OH_NativeXComponent_TouchEvent& touch_event,
    const OH_NativeXComponent_TouchPointToolType tool_type,
    const float tilt_x,
    const float tilt_y) {
  TRACE_EVENT_1("XComponentImpl::OnTouchEvent", "widget_id", GetWidget());
  if (event_callback_) {
    event_callback_->touchEventCallback(GetWidget(), touch_event, tool_type,
                                        tilt_x, tilt_y);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnMouseEvent(
    const OH_NativeXComponent_MouseEvent& mouse_event) {
  if (event_callback_) {
    event_callback_->mouseEventCallback(GetWidget(), mouse_event);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnKeyEvent(OH_NativeXComponent_KeyEvent& key_event) {
  TRACE_EVENT_1("XComponentImpl::OnKeyEvent", "widget_id", GetWidget());
  if (event_callback_) {
    event_callback_->keyEventCallback(GetWidget(), key_event);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnHoverEvent(const bool is_hover) {
  if (event_callback_) {
    event_callback_->mouse_hover_event_callback(GetWidget(), is_hover);
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

void XComponentImpl::RequestLayout() {
  auto event = std::make_shared<SurfaceEvent>(EventType::ET_SURFACE_CHANGE);
  event->width = initial_width_;
  event->height = initial_height_;
  WindowAdapter::GetInstance().NotifyWindowEvent(GetWidget(), event);
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnDragEnterEvent(const ohos::adapter::OhosDragInfo& drag_info) {
  TRACE_EVENT_1("XComponentImpl::OnDragEnterEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    event_callback_->dragEnterEventCallback(GetWidget(), drag_info);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnDragMoveEvent(const float window_x, const float window_y) {
  TRACE_EVENT_1("XComponentImpl::OnDragMoveEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    event_callback_->dragMoveEventCallback(GetWidget(), window_x, window_y);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnDropEvent(const ohos::adapter::OhosDragInfo& drag_info) {
  TRACE_EVENT_1("XComponentImpl::OnDropEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    event_callback_->dropEventCallback(GetWidget(), drag_info);
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnDragLeaveEvent() {
  TRACE_EVENT_1("XComponentImpl::OnDragLeaveEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    event_callback_->dragLeaveEventCallback(GetWidget());
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void
XComponentImpl::OnDragEndEvent() {
  TRACE_EVENT_1("XComponentImpl::OnDragEndEvent", "widget_id", GetWidget());
  if (event_callback_ != nullptr) {
    event_callback_->dragEndEventCallback(GetWidget());
  }
}

}  // namespace ohos::adapter::xcomponent
