// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_toplevel_window.h"

#include <string>

#include "ohos/adapter/window/app_window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/base/hit_test.h"
#include "ui/base/ui_base_features.h"
#include "ui/display/types/display_constants.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

namespace ui {

using namespace ohos::adapter::window;
using namespace ohos::adapter::xcomponent;

constexpr gfx::AcceleratedWidget kDefaultWidget = 1;

OhosToplevelWindow::OhosToplevelWindow(PlatformWindowDelegate* delegate,
                                       OhosWindowManager* manager)
    : OhosWindow(delegate, manager) {
  // Set a class property key, which allows |this| to be used for interactive
  // events, e.g. move or resize.
  SetWmMoveResizeHandler(this, AsWmMoveResizeHandler());
  state_ = PlatformWindowState::kNormal;
}

OhosToplevelWindow::~OhosToplevelWindow() = default;

void OhosToplevelWindow::Hide() {
  OhosWindow::Hide();

  if (child_window()) {
    child_window()->Hide();
    set_child_window(nullptr);
  }

  if (using_system_floating_window_) {
    SystemFloatingWindowAdapter::GetInstance().Close(GetWidget());
  } else {
    AppWindowAdapter::GetInstance().Close(GetWidget());
  }
}

void OhosToplevelWindow::Close() {
  if (child_window()) {
    child_window()->Hide();
    set_child_window(nullptr);
  }

  if (using_system_floating_window_) {
    SystemFloatingWindowAdapter::GetInstance().Close(GetWidget());
  } else {
    AppWindowAdapter::GetInstance().Close(GetWidget());
  }

  OhosWindow::Close();
}

bool OhosToplevelWindow::IsVisible() const {
  return is_visible_ && PlatformWindowState::kMinimized != state_;
}

void OhosToplevelWindow::SetFullscreen(bool fullscreen,
                                       int64_t target_display_id) {
  DCHECK_EQ(target_display_id, display::kInvalidDisplayId);

  target_display_id_ = target_display_id;

  // We must track the previous state to correctly say our state as long as it
  // can be the maximized instead of normal one.
  PlatformWindowState new_state = PlatformWindowState::kUnknown;
  if (fullscreen) {
    new_state = PlatformWindowState::kFullScreen;
  } else if (previous_state_ == PlatformWindowState::kMinimized) {
    new_state = previous_enter_fullscreen_state_;
  } else {
    new_state = previous_state_;
  }

  SetWindowState(new_state);
}

void OhosToplevelWindow::Maximize() {
  SetWindowState(PlatformWindowState::kMaximized);
}

void OhosToplevelWindow::UnMaximize() {
  SetWindowState(PlatformWindowState::kNormal);
}

void OhosToplevelWindow::Minimize() {
  SetWindowState(PlatformWindowState::kMinimized);
}

void OhosToplevelWindow::Restore() {
  switch (previous_state_) {
    case PlatformWindowState::kFullScreen:
      SetWindowState(PlatformWindowState::kFullScreen, false);
      break;
    case PlatformWindowState::kMaximized:
      SetWindowState(PlatformWindowState::kMaximized, false);
      break;
    case PlatformWindowState::kMinimized:
      Minimize();
      break;
    case PlatformWindowState::kNormal:
      SetWindowState(PlatformWindowState::kNormal, IsMaximized());
      break;
    default:
      LOG(ERROR) << "Restore not handle status:"
                 << static_cast<int32_t>(previous_state_);
  }
}

PlatformWindowState OhosToplevelWindow::GetPlatformWindowState() const {
  return state_;
}

bool OhosToplevelWindow::HasInitDone() {
  if (GetWidget() != kDefaultWidget) {
    return true;
  }
  return WindowAdapter::GetInstance().WindowHasInit(GetWidget());
}

void OhosToplevelWindow::Activate() {
  if (!IsVisible() || !activatable_) {
    return;
  }

  if (activation_state() != ActivationState::kActive) {
    AppWindowAdapter::GetInstance().Activate(GetWidget());
  }
}

void OhosToplevelWindow::SizeConstraintsChanged() {
  UpdateMinAndMaxSize();
}

void OhosToplevelWindow::SetUseNativeFrame(bool use_native_frame) {
  if (use_native_frame_ != use_native_frame) {
    use_native_frame_ = use_native_frame;
  }
}

bool OhosToplevelWindow::ShouldUseNativeFrame() const {
  return use_native_frame_;
}

void OhosToplevelWindow::SetBoundsInPixels(const gfx::Rect& bounds) {
  gfx::Rect new_bounds_in_pixels(bounds);

  const bool size_changed =
      bounds_in_pixels_.size() != new_bounds_in_pixels.size();

  // Ensure we don't go smaller than our minimum bounds.
  if (size_changed) {
    // Update the minimum and maximum sizes in case they have changed.
    UpdateMinAndMaxSize();

    if (new_bounds_in_pixels.width() < min_size_in_pixels_.width() ||
        new_bounds_in_pixels.height() < min_size_in_pixels_.height() ||
        (!max_size_in_pixels_.IsEmpty() &&
         (new_bounds_in_pixels.width() > max_size_in_pixels_.width() ||
          new_bounds_in_pixels.height() > max_size_in_pixels_.height()))) {
      gfx::Size size_in_pixels = new_bounds_in_pixels.size();
      if (!max_size_in_pixels_.IsEmpty()) {
        size_in_pixels.SetToMin(max_size_in_pixels_);
      }
      size_in_pixels.SetToMax(min_size_in_pixels_);
      new_bounds_in_pixels.set_size(size_in_pixels);
    }
  }

  previous_bounds_in_pixels_ = bounds_in_pixels_;
  bounds_in_pixels_ = new_bounds_in_pixels;
  SetWindowState(PlatformWindowState::kNormal, false);

  WindowRect rect{bounds_in_pixels_.x(), bounds_in_pixels_.y(),
                  bounds_in_pixels_.width(), bounds_in_pixels_.height()};
  auto callback = [this]() {
    XComponentManager::GetInstance()->RequestLayout(GetWindowUniqueId());
  };
  if (using_system_floating_window_) {
    SystemFloatingWindowAdapter::GetInstance().SetBounds(GetWidget(), rect,
                                                         callback);
  } else {
    AppWindowAdapter::GetInstance().SetBounds(GetWidget(), rect, callback);
  }
  Applied(previous_bounds_in_pixels_, bounds_in_pixels_);
}

void OhosToplevelWindow::OnInitialize(PlatformWindowInitProperties properties) {
  hide_title_bar_ = properties.remove_standard_frame;
  activatable_ = properties.activatable;
  using_system_floating_window_ = properties.using_system_floating_window;
}

bool OhosToplevelWindow::OnCreateWindow() {
  if (GetWidget() == kDefaultWidget) {
    // First window will lost OH focus event, need trigger manually
    OnFocusEvent();
    return true;
  }

  NewWindowParam param(GetWindowUniqueId(), bounds_in_pixels_.x(),
                       bounds_in_pixels_.y(), bounds_in_pixels_.width(),
                       bounds_in_pixels_.height(), hide_title_bar_,
                       using_system_floating_window_);
  return XComponentManager::GetInstance()->StartNewWindow(param);
}

void OhosToplevelWindow::TriggerStateChanges() {
  if (state_ == PlatformWindowState::kMinimized) {
    AppWindowAdapter::GetInstance().Minimize(GetWidget());
  } else if (state_ == PlatformWindowState::kFullScreen) {
    AppWindowAdapter::GetInstance().SetFullscreen(GetWidget());
  } else if (state_ == PlatformWindowState::kMaximized) {
    AppWindowAdapter::GetInstance().Maximize(GetWidget());
  } else if (state_ == PlatformWindowState::kNormal) {
    AppWindowAdapter::GetInstance().UnMaximize(GetWidget());
  }

  delegate()->OnWindowStateChanged(previous_state_, state_);

  // loses focus when enter or exiting full screen
  if (previous_state_ == PlatformWindowState::kFullScreen ||
      state_ == PlatformWindowState::kFullScreen) {
    AppWindowAdapter::GetInstance().Activate(GetWidget());
  }
}

void OhosToplevelWindow::UpdateStateChanges() {
  delegate()->OnWindowStateChanged(previous_state_, state_);
}

void OhosToplevelWindow::SetWindowState(PlatformWindowState new_state,
                                        bool isTrigger) {
  if (new_state == state_) {
    return;
  }
  VLOG(1) << "SetWindowState from: "
          << static_cast<std::underlying_type<PlatformWindowState>::type>(
                 state_)
          << ", to: "
          << static_cast<std::underlying_type<PlatformWindowState>::type>(
                 new_state);
  previous_state_ = state_;
  state_ = new_state;

  // we need to record the last window state when enter full screen
  if (previous_enter_fullscreen_state_ != previous_state_ &&
      (previous_state_ == PlatformWindowState::kNormal ||
       previous_state_ == PlatformWindowState::kMaximized)) {
    previous_enter_fullscreen_state_ = previous_state_;
  }

  if (isTrigger) {
    TriggerStateChanges();
  } else {
    UpdateStateChanges();
  }
}

WmMoveResizeHandler* OhosToplevelWindow::AsWmMoveResizeHandler() {
  return static_cast<WmMoveResizeHandler*>(this);
}

void OhosToplevelWindow::UpdateMinAndMaxSize() {
  absl::optional<gfx::Size> minimum_in_pixels = GetMinimumSizeForOhosWindow();
  absl::optional<gfx::Size> maximum_in_pixels = GetMaximumSizeForOhosWindow();
  if ((!minimum_in_pixels ||
       min_size_in_pixels_ == minimum_in_pixels.value()) &&
      (!maximum_in_pixels ||
       max_size_in_pixels_ == maximum_in_pixels.value())) {
    return;
  }

  min_size_in_pixels_ = minimum_in_pixels.value();
  max_size_in_pixels_ = maximum_in_pixels.value();
  if (using_system_floating_window_) {
    SystemFloatingWindowAdapter::GetInstance().SetWindowLimits(
        min_size_in_pixels_.width(), min_size_in_pixels_.height(),
        max_size_in_pixels_.width(), max_size_in_pixels_.height(), GetWidget());
  } else {
    AppWindowAdapter::GetInstance().SetWindowLimits(
        min_size_in_pixels_.width(), min_size_in_pixels_.height(),
        max_size_in_pixels_.width(), max_size_in_pixels_.height(), GetWidget());
  }
}

absl::optional<gfx::Size> OhosToplevelWindow::GetMinimumSizeForOhosWindow() {
  if (auto max_size = delegate()->GetMinimumSizeForWindow()) {
    return delegate()->ConvertRectToPixels(gfx::Rect(*max_size)).size();
  }
  return absl::nullopt;
}

absl::optional<gfx::Size> OhosToplevelWindow::GetMaximumSizeForOhosWindow() {
  if (auto max_size = delegate()->GetMaximumSizeForWindow()) {
    return delegate()->ConvertRectToPixels(gfx::Rect(*max_size)).size();
  }
  return absl::nullopt;
}

OhosToplevelWindow* OhosToplevelWindow::AsOhosTopWindow() {
  return this;
}

void OhosToplevelWindow::HandleEvent(std::shared_ptr<XCEvent> event) {
  switch (event->type()) {
    case XCEventType::ET_SURFACE_FOCUS: {
      OnFocusEvent();
      break;
    }
    case XCEventType::ET_SURFACE_BLUR: {
      OnBlurEvent();
      break;
    }
    case XCEventType::ET_WINDOW_SIZE_CHANGE: {
      OnWindowSizeChangeEvent(event);
      break;
    }
    case XCEventType::ET_WINDOW_CHANGE: {
      OnWindowEvent(event);
      break;
    }
    case XCEventType::ET_WINDOW_RECT_CHANGE: {
      OnWindowRectChangeEvent(event);
      break;
    }
    default:
      LOG(ERROR) << "EventType::ET_UNKNOWN not handle:" << int(event->type());
      break;
  }
}

void OhosToplevelWindow::OnFocusEvent() {
  if (!activatable_) {
    return;
  }

  SetFocus(true);
  OnActivateEvent();
}

void OhosToplevelWindow::OnBlurEvent() {
  if (!activatable_) {
    return;
  }

  SetFocus(false);
  auto* event_source =
      reinterpret_cast<OhosEventSource*>(PlatformEventSource::GetInstance());
  if (event_source) {
    event_source->UpdateKeyFlags();
  }
  Deactivate();
}

void OhosToplevelWindow::OnWindowEvent(std::shared_ptr<XCEvent> event) {
  auto window_event = static_pointer_cast<WindowEvent>(event);
  auto window_type = window_event->window_event_type_;

  switch (window_type) {
    case WindowEventType::WINDOW_SHOWN:
      if (state_ == PlatformWindowState::kMinimized) {
        Restore();
      }
      break;
    case WindowEventType::WINDOW_HIDDEN:
      SetWindowState(PlatformWindowState::kMinimized, false);
      break;
    case WindowEventType::WINDOW_OCCLUDED:
      delegate()->OnOcclusionStateChanged(
          ui::PlatformWindowOcclusionState::kOccluded);
      break;
    case WindowEventType::WINDOW_VISIBLE:
      delegate()->OnOcclusionStateChanged(
          ui::PlatformWindowOcclusionState::kVisible);
      break;
    default:
      break;
  }
}

void OhosToplevelWindow::OnWindowSizeChangeEvent(
    std::shared_ptr<XCEvent> event) {
  if (!IsVisible()) {
    return;
  }

  auto window_event = static_pointer_cast<WindowSizeChangeEvent>(event);
  previous_bounds_in_pixels_ = bounds_in_pixels_;
  bounds_in_pixels_ = gfx::Rect{window_event->left, window_event->top,
                                window_event->width, window_event->height};

  // we need to change the window bounds when receiving the event
  Applied(previous_bounds_in_pixels_, bounds_in_pixels_);
}

void OhosToplevelWindow::OnWindowRectChangeEvent(
    std::shared_ptr<XCEvent> event) {
  if (!IsVisible()) {
    return;
  }

  auto window_event = static_pointer_cast<WindowRectChangeEvent>(event);
  auto reason = window_event->reason;
  PlatformWindowState current_state = state_;
  bool handle = false;
  PlatformWindowState new_state;

  // receive event from arkui,we need to deal with it.
  switch (current_state) {
    case PlatformWindowState::kNormal: {
      if (reason == RectChangeReason::MAXIMIZE) {
        handle = true;
        new_state = PlatformWindowState::kMaximized;
        restored_bounds_in_pixels_ = previous_bounds_in_pixels_;
      }
      break;
    }
    case PlatformWindowState::kMaximized:
    case PlatformWindowState::kFullScreen: {
      if (reason == RectChangeReason::RECOVER) {
        handle = true;
        new_state = PlatformWindowState::kNormal;
      }
      break;
    }
    default:
      break;
  }

  if (handle) {
    // This event is from ArkUI, the changes of state is already triggered.
    SetWindowState(new_state, false);
  }
}

void OhosToplevelWindow::DispatchHostWindowDragMovement(
    int hit_test,
    const gfx::Point& pointer_location_in_px) {
  if (hit_test != HitTestCompat::HTCAPTION || bounds_in_pixels_.IsEmpty()) {
    return;
  }
  // Window position is calculated based on the mouse position within the screen
  // minus the mouse position within the window.
  // During a single dragging, the mouse position within the window should not
  // change, the screen one changes when the mouse moves.
  gfx::Point screen_point =
      reinterpret_cast<OhosEventSource*>(PlatformEventSource::GetInstance())
          ->GetCursorScreenPoint();
  gfx::Rect bounds(bounds_in_pixels_);
  if (state_ == PlatformWindowState::kMaximized) {
    // Transfer the mouse position from the maximized window to
    // the normal window;
    // the window position is calculated based on this.
    float relative_x = static_cast<float>(pointer_location_in_px.x()) /
                       bounds_in_pixels_.width();
    float relative_y = static_cast<float>(pointer_location_in_px.y()) /
                       bounds_in_pixels_.height();
    pointer_window_location_in_pixels_.set_x(
        static_cast<int>(relative_x * restored_bounds_in_pixels_.width()));
    pointer_window_location_in_pixels_.set_y(
        static_cast<int>(relative_y * restored_bounds_in_pixels_.height()));
    bounds.set_size(restored_bounds_in_pixels_.size());
    Restore();
  } else if (pointer_location_in_px.x() != -1 &&
             pointer_location_in_px.y() != -1) {
    // Save the mouse position within the window;
    // the window position is calculated based on this.
    pointer_window_location_in_pixels_ = pointer_location_in_px;
  }
  bounds.set_origin(
      {screen_point.x() - pointer_window_location_in_pixels_.x(),
       screen_point.y() - pointer_window_location_in_pixels_.y()});
  SetBoundsInPixels(bounds);
}

}  // namespace ui
