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

#include "ui/ozone/platform/ohos/host/ohos_popup.h"

#include "ohos/adapter/task_runner/main_thread_task_runner.h"
#include "ohos/adapter/window/sub_window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

namespace ui {

OhosPopup::OhosPopup(PlatformWindowDelegate* delegate,
                     OhosWindowManager* manager,
                     OhosWindow* parent)
    : OhosWindow(delegate, manager) {
  set_parent_window(parent);
}

OhosPopup::~OhosPopup() = default;

void OhosPopup::Show(bool inactive) {
  DCHECK(parent_window());

  OhosWindow::BindNodeHandle();
  OhosWindow::Show(inactive);

  if (menu_created_) {
    SubWindowAdapter::GetInstance().Show(GetWindowUniqueId());
  }
}

void OhosPopup::Hide() {
  DCHECK(parent_window());

  OhosWindow::Hide();

  if (!menu_created_)
    return;

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

  if (ohos::adapter::nodeHandle::NodeHandleImpl::GetInstance()
          .IsSupportNodeHandle()) {
    OhosWindow::UnBindNodeHandle();
    if (!is_ability_bound_) {
      auto task =
          std::bind(&XComponentManager::RemoveNodeHandleXComponent,
                    XComponentManager::GetInstance(), GetWindowUniqueId());
      ohos::adapter::taskRunner::MainThreadTaskRunner::GetInstance().PostTask(
          task);
    }
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

  WindowRect rect{bounds_in_pixels_.x(),
                  bounds_in_pixels_.y(),
                  bounds_in_pixels_.width(),
                  bounds_in_pixels_.height()};
  SubWindowAdapter::GetInstance().SetBounds(GetWindowUniqueId(), rect);

  Applied(previous_bounds_in_pixels_, bounds_in_pixels_);
}

OhosPopup* OhosPopup::AsOhosPopup() {
  return this;
}

WindowInitParameter OhosPopup::BuildWindowInitParameter() {
  WindowInitParameter parameter;

  WindowRect rect{bounds_in_pixels_.x(),
                  bounds_in_pixels_.y(),
                  bounds_in_pixels_.width(),
                  bounds_in_pixels_.height()};
  // Calculate initial bounds.
  parameter.bounds = rect;
  parameter.type = WindowInitType::kPopup;
  parameter.SetBackground(init_color_arbg_);

  auto root_parent_window = GetRootParentWindow();
  std::string root_window_id = root_parent_window->GetWindowUniqueId();
  parameter.parent_id = root_window_id;
  parameter.window_id = GetWindowUniqueId();
  return parameter;
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

bool OhosPopup::OnCreateWindow(WindowInitParameter param) {
  auto root_parent_window = GetRootParentWindow();
  if (root_parent_window->type() != PlatformWindowType::kWindow) {
    LOG(ERROR) << "Cannot create subwindow from another subwindow";
    return false;
  }

  std::string create_id = XComponentManager::GetInstance()->CreateWindow(param);
  if (create_id.empty()) {
    return false;
  }
  // Enable the sub-window reuse feature.  may return a different widget id 
  // than the original value.  so the widget id needs to be reset
  if (create_id != GetWindowUniqueId()) {
    gfx::AcceleratedWidget new_widget = util::ConvertWindowIdToWidgetId(create_id);
    SetWidget(new_widget);
  }
  menu_created_ = true;
  parent_window()->set_child_window(this);
  return true;
}

void OhosPopup::SetWindowState(PlatformWindowState state) {
  if (state_ != state) {
    VLOG(1) << "OhosPopup::SetWindowState, from: "
            << static_cast<std::underlying_type<PlatformWindowState>::type>(
                    state_)
            << ", to: "
            << static_cast<std::underlying_type<PlatformWindowState>::type>(
                    state);

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
  bounds_in_pixels_ = gfx::Rect{window_event->left,
                                window_event->top,
                                window_event->width,
                                window_event->height};

  // we need to change the window bounds when receiving the event
  Applied(previous_bounds_in_pixels_, bounds_in_pixels_);
}

display::Display OhosPopup::GetCurrentDisplay() {
  OhosWindow* parent_window = GetRootParentWindow();
  return parent_window->GetCurrentDisplay();
}

bool OhosPopup::ShouldWindowContentsBeTransparent() const {
  return true;
}

}  // namespace ui
