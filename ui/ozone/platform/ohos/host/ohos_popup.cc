// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_popup.h"

#include "ohos/adapter/window/sub_window_adapter.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

namespace ui {

using namespace ohos::adapter::window;

OhosPopup::OhosPopup(PlatformWindowDelegate* delegate,
                     OhosWindowManager* manager,
                     OhosWindow* parent)
    : OhosWindow(delegate, manager) {
  set_parent_window(parent);
}

OhosPopup::~OhosPopup() = default;

void OhosPopup::Show(bool inactive) {
  DCHECK(parent_window());

  OhosWindow::Show(inactive);

  if (menu_created_) {
    SubWindowAdapter::GetInstance().Show(GetWindowUniqueId());
  }
}

void OhosPopup::Hide() {
  DCHECK(parent_window());

  OhosWindow::Hide();

  if (!menu_created_) {
    return;
  }

  SetWindowState(PlatformWindowState::kMinimized);
  SubWindowAdapter::GetInstance().Hide(GetWindowUniqueId());

  parent_window()->set_child_window(nullptr);
}

void OhosPopup::Close() {
  DCHECK(parent_window());

  if (menu_created_) {
    parent_window()->set_child_window(nullptr);
    menu_created_ = false;
    SubWindowAdapter::GetInstance().Cancel(GetWindowUniqueId());
  }

  OhosWindow::Close();
}

bool OhosPopup::IsVisible() const {
  return is_visible_;
}

void OhosPopup::SetBoundsInPixels(const gfx::Rect& bounds) {
  previous_bounds_in_pixels_ = bounds_in_pixels_;
  bounds_in_pixels_ = bounds;
  SetWindowState(PlatformWindowState::kNormal);

  WindowRect rect{bounds_in_pixels_.x(), bounds_in_pixels_.y(),
                  bounds_in_pixels_.width(), bounds_in_pixels_.height()};
  SubWindowAdapter::GetInstance().SetBounds(GetWindowUniqueId(), rect);

  Applied(previous_bounds_in_pixels_, bounds_in_pixels_);
}

PlatformWindowState OhosPopup::GetPlatformWindowState() const {
  return state_;
}

void OhosPopup::OnInitialize(PlatformWindowInitProperties properties) {
  DCHECK(parent_window());

  auto background_color = properties.background_color;
  if (background_color.has_value()) {
    init_color_arbg_ = background_color.value();
  }
}

bool OhosPopup::OnCreateWindow() {
  auto root_parent_window = GetRootParentWindow();
  if (root_parent_window->type() != PlatformWindowType::kWindow) {
    LOG(ERROR) << "Cannot create subwindow from another subwindow";
    return false;
  }
  std::string root_window_id = root_parent_window->GetWindowUniqueId();
  WindowRect rect{bounds_in_pixels_.x(), bounds_in_pixels_.y(),
                  bounds_in_pixels_.width(), bounds_in_pixels_.height()};
  std::string create_id;
  if (!SubWindowAdapter::GetInstance().Create(root_window_id,
                                              GetWindowUniqueId(), rect,
                                              init_color_arbg_, create_id)) {
    return false;
  }
  // Enable the sub-window reuse feature.  may return a different widget id
  // than the original value.  so the widget id needs to be reset
  if (create_id != GetWindowUniqueId()) {
    gfx::AcceleratedWidget new_widget =
        util::ConvertWindowIdToWidgetId(create_id);
    SetWidget(new_widget);
  }
  menu_created_ = true;
  parent_window()->set_child_window(this);
  return true;
}

void OhosPopup::SetWindowState(PlatformWindowState state) {
  if (state_ != state) {
    VLOG(1)
        << "OhosPopup::SetWindowState, from: "
        << static_cast<std::underlying_type<PlatformWindowState>::type>(state_)
        << ", to: "
        << static_cast<std::underlying_type<PlatformWindowState>::type>(state);

    previous_state_ = state_;
    state_ = state;

    delegate()->OnWindowStateChanged(previous_state_, state_);
  }
}

void OhosPopup::HandleEvent(std::shared_ptr<XCEvent> event) {
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
    default:
      LOG(ERROR) << "EventType::ET_UNKNOWN not handle:" << int(event->type());
      break;
  }
}

void OhosPopup::OnFocusEvent() {
  SetFocus(true);
  OnActivateEvent();
}

void OhosPopup::OnBlurEvent() {
  SetFocus(false);
  Deactivate();
}

void OhosPopup::OnWindowSizeChangeEvent(std::shared_ptr<XCEvent> event) {
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

}  // namespace ui
