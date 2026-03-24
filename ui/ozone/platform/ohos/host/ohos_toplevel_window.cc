// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_toplevel_window.h"

#include <window_manager/oh_window.h>

#include <string>

#include "base/strings/utf_string_conversions.h"
#include "ohos/adapter/browser/browser_adapter.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/node_handle/node_handle_impl.h"
#include "ohos/adapter/window/app_window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/base/hit_test.h"
#include "ui/base/ui_base_features.h"
#include "ui/display/screen.h"
#include "ui/display/types/display_constants.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source_base.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

namespace ui {

using NodeHandleImpl = ohos::adapter::nodeHandle::NodeHandleImpl;

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

void OhosToplevelWindow::Show(bool inactive) {
  if (NodeHandleImpl::GetInstance().IsSupportNodeHandle() &&
      need_create_ability_) {
    CreateAndShowInternal();
  }
  OhosWindow::BindNodeHandle();
  OhosWindow::Show(inactive);
}

void OhosToplevelWindow::Hide() {
  OhosWindow::Hide();

  if (child_window()) {
    child_window()->Hide();
    set_child_window(nullptr);
  }
  
  CloseInternal();
}

void OhosToplevelWindow::Close() {
  if (child_window()) {
    child_window()->Hide();
    set_child_window(nullptr);
  }

  CloseInternal();

  gfx::AcceleratedWidget widget_id = GetWidget();
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

void OhosToplevelWindow::Minimize() {
  SetWindowState(PlatformWindowState::kMinimized);
}

void OhosToplevelWindow::Restore() {
  SetWindowState(PlatformWindowState::kNormal);
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
  if (!activatable_) {
    return;
  }
  if (!IsVisible()) {
    AppWindowAdapter::GetInstance().Restore(GetWidget());
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
  if (use_native_frame_ != use_native_frame)
    use_native_frame_ = use_native_frame;
}

bool OhosToplevelWindow::ShouldUseNativeFrame() const {
  return use_native_frame_;
}

void OhosToplevelWindow::SetBoundsInPixels(const gfx::Rect& bounds) {
  // Update the minimum and maximum sizes in case they have changed.
  UpdateMinAndMaxSize();

  gfx::Rect new_bounds_in_pixels(bounds);

  const bool size_changed =
      bounds_in_pixels_.size() != new_bounds_in_pixels.size();

  // Ensure we don't go smaller than our minimum bounds.
  if (size_changed) {
    if (new_bounds_in_pixels.width() < min_size_in_pixels_.width() ||
        new_bounds_in_pixels.height() < min_size_in_pixels_.height() ||
        (!max_size_in_pixels_.IsEmpty() &&
         (new_bounds_in_pixels.width() > max_size_in_pixels_.width() ||
          new_bounds_in_pixels.height() > max_size_in_pixels_.height()))) {
      gfx::Size size_in_pixels = new_bounds_in_pixels.size();
      if (!max_size_in_pixels_.IsEmpty())
        size_in_pixels.SetToMin(max_size_in_pixels_);
      size_in_pixels.SetToMax(min_size_in_pixels_);
      new_bounds_in_pixels.set_size(size_in_pixels);
    }
  }

  previous_bounds_in_pixels_ = bounds_in_pixels_;
  bounds_in_pixels_ = new_bounds_in_pixels;

  WindowRect rect{bounds_in_pixels_.x(),
                  bounds_in_pixels_.y(),
                  bounds_in_pixels_.width(),
                  bounds_in_pixels_.height()};
  if (use_floating_window_) {
    SystemFloatingWindowAdapter::GetInstance().SetBounds(GetWidget(), rect);
  } else {
    AppWindowAdapter::GetInstance().SetBounds(GetWidget(), rect);
  }
  Applied(previous_bounds_in_pixels_, bounds_in_pixels_);
}

void OhosToplevelWindow::OnInitialize(
    PlatformWindowInitProperties properties) {
  hide_title_bar_ = properties.remove_standard_frame;
  activatable_ = properties.activatable;
  use_floating_window_ = properties.using_system_floating_window;
  use_dark_mode_ = properties.use_dark_mode;
  caption_button_visible_ = properties.caption_button_visible;
  ability_type_ = properties.ability_type;
  app_id_ = properties.app_id;
}

bool OhosToplevelWindow::OnCreateWindow(WindowInitParameter param) {
  if (NodeHandleImpl::GetInstance().IsSupportNodeHandle()) {
    LOG(INFO) << "[ohoswindow] OhosToplevelWindow::OnCreateWindow, "
            << "type is " << static_cast<int>(param.type);
    std::string ability_id =
        XComponentManager::GetInstance()->GetCreatedAbility();
    if (ability_id.empty()) {
      need_create_ability_ = true;
      XComponentManager::GetInstance()->AddCreatingAbility(param.window_id);
    } else {
      need_create_ability_ = false;
      param.window_id = ability_id;
    }
    init_param_ = param;
  }

  std::string create_id = XComponentManager::GetInstance()->CreateWindow(param);
  if (create_id.empty()) {
    return false;
  }

  // when enable the embedded-window or the first-time launch window.  
  // may return a different widget id than the original value.
  // so the widget id needs to be reset
  if (create_id != GetWindowUniqueId()) {
    gfx::AcceleratedWidget new_widget = util::ConvertWindowIdToWidgetId(create_id);
    SetWidget(new_widget);
  }

  // lost surface focus event in create xcomponent on OH platform, trigger manually
  auto task = base::BindOnce([](base::WeakPtr<OhosWindow> window,
                                gfx::AcceleratedWidget widget) {
        if (window && window->AsOhosToplevelWindow()) {
          window->AsOhosToplevelWindow()->OnFocusEvent();
        }
      },
      AsWeakPtr(), GetWidget());
  ui_task_runner()->PostTask(FROM_HERE, std::move(task));

  return true;
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

void OhosToplevelWindow::UpdateBoundsChanges(int top, int left,
                                             int width, int height) {
  previous_bounds_in_pixels_ = bounds_in_pixels_;
  bounds_in_pixels_ = gfx::Rect{left, top, width, height};
  // we need to change the window bounds when receiving the event of
  // bounds changing.
  Applied(previous_bounds_in_pixels_, bounds_in_pixels_);
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

  if (new_state == PlatformWindowState::kMinimized) {
    // Record the state before minimize.
    previous_enter_minimize_state_ = state_;
  }

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
      (!maximum_in_pixels || max_size_in_pixels_ == maximum_in_pixels.value()))
    return;

  min_size_in_pixels_ = minimum_in_pixels.value();
  max_size_in_pixels_ = maximum_in_pixels.value();
  if (use_floating_window_) {
    SystemFloatingWindowAdapter::GetInstance().SetWindowLimits(
        min_size_in_pixels_.width(),
        min_size_in_pixels_.height(),
        max_size_in_pixels_.width(),
        max_size_in_pixels_.height(),
        GetWidget());
  } else {
    AppWindowAdapter::GetInstance().SetWindowLimits(
        min_size_in_pixels_.width(),
        min_size_in_pixels_.height(),
        max_size_in_pixels_.width(),
        max_size_in_pixels_.height(),
        GetWidget());
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

OhosToplevelWindow* OhosToplevelWindow::AsOhosToplevelWindow() {
  return this;
}

WindowInitParameter OhosToplevelWindow::BuildWindowInitParameter() {
  WindowInitParameter parameter;

  WindowRect rect{bounds_in_pixels_.x(),
                  bounds_in_pixels_.y(),
                  bounds_in_pixels_.width(),
                  bounds_in_pixels_.height()};
  // Calculate initial bounds.
  parameter.bounds = rect;
  parameter.hide_title_bar = hide_title_bar_;
  parameter.use_floating_window = use_floating_window_;
  parameter.window_id = GetWindowUniqueId();
  parameter.use_dark_mode = use_dark_mode_;
  parameter.caption_button_visible = caption_button_visible_;
  parameter.ability_type = ability_type_;
  parameter.app_id = app_id_;
  return parameter;
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
    case XCEventType::ET_WINDOW_CHANGE: {
      OnWindowEvent(event);
      break;
    }
    case XCEventType::ET_WINDOW_RECT_CHANGE: {
      OnWindowRectChangeEvent(event);
      break;
    }
    case XCEventType::ET_WINDOW_STATUS_CHANGE: {
      OnWindowStatusChangeEvent(event);
      break;
    }
    case XCEventType::ET_WINDOW_CAPTION_BUTTON_RECT_CHANGE: {
      OnWindowCaptionButtonRectChangeEvent(event);
      break;
    }
    case XCEventType::ET_WINDOW_DISPLAY_ID_CHANGE: {
      OnWindowDisplayIdChangeEvent(event);
      break;
    }
    case XCEventType::ET_BACK_TO_LAST_PAGE: {
      OnBackToLastPage();
      break;
    }
    default:
      LOG(ERROR) << "EventType::ET_UNKNOWN not handle:" << int(event->type());
      break;
  }
}

bool OhosToplevelWindow::IsHitCaptionButton(const gfx::PointF& point) {
  auto caption_button_bounds_left =
      bounds_in_pixels_.width() - caption_button_rect_in_pixel_.width();
  auto caption_button_bounds =
      gfx::Rect(caption_button_bounds_left, caption_button_rect_in_pixel_.y(),
                caption_button_rect_in_pixel_.width(),
                caption_button_rect_in_pixel_.height());
  return caption_button_bounds.Contains(point.x(), point.y());
}

void OhosToplevelWindow::OnFocusEvent() {
  if (!activatable_) {
    return;
  }
  SetLastActiveWidgetId(GetWidget());
  SetFocus(true);
  OnActivateEvent();
}

void OhosToplevelWindow::OnBlurEvent() {
  if (!activatable_) {
    return;
  }

  SetFocus(false);
  Deactivate();
}

void OhosToplevelWindow::OnWindowEvent(std::shared_ptr<XCEvent> event) {
  auto window_event = static_pointer_cast<WindowEvent>(event);
  auto window_type = window_event->window_event_type_;
 
  switch (window_type) {
    case WindowEventType::WINDOW_SHOWN:
      LOG(INFO) << "[ohoswindow] int OhosToplevelWindow::OnWindowEvent "
                << "event WINDOW_SHOWN received.";
      if (state_ == PlatformWindowState::kMinimized) {
        SetWindowState(previous_enter_minimize_state_, false);
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
    case WindowEventType::WINDOW_CLOSE:
      window_manager()->PrepareCloseWindow(GetWidget(), this);
      delegate()->OnCloseRequest();
      break;
    default:
      break;
  }
}

void OhosToplevelWindow::OnWindowStatusChangeEvent(
    std::shared_ptr<XCEvent> event) {
  auto window_event = static_pointer_cast<WindowStatusChangeEvent>(event);
  is_split_screen_ = window_event->status == WindowStatusType::SPLIT_SCREEN;
}

void OhosToplevelWindow::OnWindowCaptionButtonRectChangeEvent(
    std::shared_ptr<XCEvent> event) {
  auto window_event =
      static_pointer_cast<WindowCaptionButtonRectChangeEvent>(event);
  caption_button_rect_in_pixel_ =
      gfx::Rect(window_event->right, window_event->top, window_event->width,
                window_event->height);
}

void OhosToplevelWindow::OnWindowRectChangeEvent(std::shared_ptr<XCEvent> event) {
  auto window_event = static_pointer_cast<WindowRectChangeEvent>(event);
  auto reason = window_event->reason;
  PlatformWindowState current_state = state_;
  bool handle = false;
  PlatformWindowState new_state;

  UpdateBoundsChanges(window_event->top, window_event->left,
                      window_event->width, window_event->height);

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
      if (reason == RectChangeReason::RECOVER) {
        handle = true;
        new_state = PlatformWindowState::kNormal;
      } else if (reason == RectChangeReason::MAXIMIZE) {
        // Double-click the title bar to enter the branch.
        restored_bounds_in_pixels_ = previous_bounds_in_pixels_;
      }
      break;
    case PlatformWindowState::kFullScreen: {
      if (reason == RectChangeReason::RECOVER) {
        delegate()->OnFullscreenSwitched(false);
      }
      delegate()->OnFullscreenStateChanged();
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

  if (ohos::adapter::device_info::DeviceInfo::SdkApi() >=
      ohos::adapter::device_info::SDK_VERSION_14) {
    if (use_floating_window_) {
      SystemFloatingWindowAdapter::GetInstance().StartWindowMoving(GetWidget());
    } else {
      AppWindowAdapter::GetInstance().StartWindowMoving(GetWidget());
    }
  } else {
    // Window position is calculated based on the mouse position within the screen
    // minus the mouse position within the window.
    // During a single dragging, the mouse position within the window should not
    // change, the screen one changes when the mouse moves.
    gfx::Point screen_point =
        reinterpret_cast<OhosEventSourceBase*>(PlatformEventSource::GetInstance())
            ->GetCursorScreenPoint();
    gfx::Rect bounds(bounds_in_pixels_);
    if (state_ == PlatformWindowState::kMaximized) {
      // Transfer the mouse position from the maximized window to
      // the normal window;
      // the window position is calculated based on this.
      float relative_x =
          static_cast<float>(
              pointer_location_in_px.x()) / bounds_in_pixels_.width();
      float relative_y =
          static_cast<float>(
              pointer_location_in_px.y()) / bounds_in_pixels_.height();
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
    bounds.set_origin({
        screen_point.x() - pointer_window_location_in_pixels_.x(),
        screen_point.y() - pointer_window_location_in_pixels_.y()
    });
    SetBoundsInPixels(bounds);
  }
}

void OhosToplevelWindow::SetTitle(const std::u16string& title) {
  if (window_title_ == title) {
    return;
  }

  window_title_ = title;
  std::string window_title = base::UTF16ToUTF8(title);
  AppWindowAdapter::GetInstance().SetTitle(window_title, GetWidget());
}

void OhosToplevelWindow::CloseInternal() {
  // we must waiting for the browser thread to exit safely
  if (window_manager()->IsWindowAtLast()) {
    LOG(WARNING) << "waiting for the browser thread to exit safely";
    return;
  }

  if (use_floating_window_) {
    SystemFloatingWindowAdapter::GetInstance().Close(GetWidget());
  } else {
    AppWindowAdapter::GetInstance().Close(GetWidget());
  }
}

void OhosToplevelWindow::CreateAndShowInternal() {
  if (state_ == PlatformWindowState::kMaximized) {
    // When bounds is empty, UIAbility will start in OH window saved state.
    init_param_.bounds = {0, 0, 0, 0};
    init_param_.status = WindowStatusType::MAXIMIZE;
  } else {
    init_param_.bounds = {bounds_in_pixels_.x(), bounds_in_pixels_.y(),
                          bounds_in_pixels_.width(),
                          bounds_in_pixels_.height()};
  }
  init_param_.window_limit = {
      max_size_in_pixels_.height(), max_size_in_pixels_.width(),
      min_size_in_pixels_.height(), min_size_in_pixels_.width()};

  XComponentManager::GetInstance()->CreateAndShowAbility(init_param_,
                                                         init_param_.window_id);

  std::string window_title = window_title_.has_value()
                                 ? base::UTF16ToUTF8(window_title_.value())
                                 : std::string();
  AppWindowAdapter::GetInstance().SetTitle(window_title, GetWidget());

  need_create_ability_ = false;
}

void OhosToplevelWindow::StartWindowMovingWithOffset(const float offset_x,
                                                     const float offset_y) {
  AppWindowAdapter::GetInstance().StartWindowMovingWithOffset(
      GetWidget(), offset_x, offset_y);
}

void OhosToplevelWindow::OnWindowDisplayIdChangeEvent(std::shared_ptr<XCEvent> event) {
  auto window_display_id_change_event =
      static_pointer_cast<WindowDisplayIdChangeEvent>(event);
  SetCurrentDisplayId(window_display_id_change_event->display_id);
}

void OhosToplevelWindow::SetLastActiveWidgetId(
    gfx::AcceleratedWidget widget_id) {
  window_manager()->SetLastActiveWidgetId(widget_id);
}

int32_t OhosToplevelWindow::GetOriginWindowId() {
  if (use_floating_window_) {
    return SystemFloatingWindowAdapter::GetInstance().GetOriginWindowId(GetWidget());
  } else {
    return OhosWindow::GetOriginWindowId();
  }
}

display::Display OhosToplevelWindow::GetCurrentDisplay() {
  int64_t display_id = GetCurrentDisplayId();
  if (display_id == display::kInvalidDisplayId) {
    auto oh_window_id = GetOriginWindowId();
    if (oh_window_id > 0) {
      WindowManager_WindowProperties oh_window_prop;
      auto result =
          OH_WindowManager_GetWindowProperties(oh_window_id, &oh_window_prop);
      if (result == WindowManager_ErrorCode::OK) {
        display_id = oh_window_prop.displayId;
        SetCurrentDisplayId(display_id);
      }
    } else {
      LOG(WARNING) << __FUNCTION__ << ", get origin window id fail,"
                   << "widget id:" << GetWidget()
                   << ", oh_window_id:" << oh_window_id;
    }
  }
  display::Display current_display;
  display::Screen* screen = display::Screen::GetScreen();
  screen->GetDisplayWithDisplayId(display_id, &current_display);
  return current_display;
}

void OhosToplevelWindow::OnBackToLastPage() {
  if (!delegate()->OnBackToLastPage()) {
    Minimize();
  }
}

}  // namespace ui
