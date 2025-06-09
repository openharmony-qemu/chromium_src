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

#include "ui/ozone/platform/ohos/host/ohos_window.h"

#include <string>

#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "content/public/browser/browser_thread.h"
#include "ohos/adapter/accessibility/accessibility_delegate_ohos_registry.h"
#include "ohos/adapter/cursor/cursor.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ui/base/cursor/platform_cursor.h"
#include "ui/base/hit_test.h"
#include "ui/events/event.h"
#include "ui/events/event_target_iterator.h"
#include "ui/events/event_utils.h"
#include "ui/events/ozone/events_ozone.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/geometry/vector2d.h"
#include "ui/ozone/common/bitmap_cursor.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_cursor_utils.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

namespace ui {

OhosWindow::OhosWindow(PlatformWindowDelegate* delegate,
                       OhosWindowManager* manager)
    : delegate_(delegate),
      window_manager_(manager),
      drag_manager_(std::make_unique<OhosDragManager>(delegate, this)),
      ui_task_runner_(base::TaskRunnerOHOS::GetUIThreadTaskRunner()) {
  DCHECK(delegate);

  drag_manager_->SetDelegate(this);
  widget_ = util::GetNextWindowWidgetId();
  window_unique_id_ = util::ConvertWidgetIdToWindowId(GetWidget());
}

OhosWindow::~OhosWindow() {
  CHECK(ui_task_runner_->BelongsToCurrentThread());

  window_manager()->RemoveWindow(GetWidget(), this);
  PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
  UnRegistWindowEvent();
}

bool OhosWindow::Initialize(PlatformWindowInitProperties properties) {
  TRACE_EVENT0("gpu", "OhosWindow::Initialize");

  type_ = properties.type;
  bounds_in_pixels_ = delegate()->ConvertRectToPixels(properties.bounds);

  OnInitialize(std::move(properties));

  // create window
  WindowInitParameter parameter = BuildWindowInitParameter();
  if (!OnCreateWindow(std::move(parameter))) {
    return false;
  }

  RegistWindowEvent();

  window_manager()->AddWindow(GetWidget(), this);

  PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
  delegate()->OnAcceleratedWidgetAvailable(GetWidget());
  return true;
}

void OhosWindow::SetWidget(gfx::AcceleratedWidget new_widget) {
  DCHECK(new_widget);

  widget_ = new_widget;
  window_unique_id_ = util::ConvertWidgetIdToWindowId(new_widget);
}

gfx::AcceleratedWidget OhosWindow::GetWidget() const {
  return widget_;
}

std::string OhosWindow::GetWindowUniqueId() const {
  return window_unique_id_;
}

void OhosWindow::RegistWindowEvent() {
  WindowAdapter::GetInstance().RegistWindowEvent(
      GetWidget(), [this](gfx::AcceleratedWidget widget, std::shared_ptr<XCEvent> event) {
        CHECK(event);

        if (widget != GetWidget()) {
          LOG(ERROR) << "handle event error no matched widget: " << (int)widget;
          return;
        }

        // Handle arkui events on the browser UI thread
        auto task = base::BindOnce([](base::WeakPtr<OhosWindow> window,
                                      std::shared_ptr<XCEvent> event) {
              if (window) {
                window->HandleEvent(event);
              }
            },
            AsWeakPtr(), std::move(event));
        ui_task_runner_->PostTask(FROM_HERE, std::move(task));
      });
}

void OhosWindow::UnRegistWindowEvent() {
  WindowAdapter::GetInstance().UnregistWindowEvent(GetWidget());
}

void OhosWindow::OnSurfaceCreated() {
  auto task = base::BindOnce([](base::WeakPtr<OhosWindow> window,
                                gfx::AcceleratedWidget widget) {
        if (window) {
          window->delegate()->OnAcceleratedWidgetAvailable(widget);
        }
      },
      AsWeakPtr(), GetWidget());
  ui_task_runner_->PostTask(FROM_HERE, std::move(task));
}

void OhosWindow::OnSurfaceDestoryed() {
  auto task = base::BindOnce([](base::WeakPtr<OhosWindow> window) {
        if (window) {
          window->delegate()->OnCloseRequest();
        }
      },
      AsWeakPtr());
  ui_task_runner_->PostTask(FROM_HERE, std::move(task));
}

void OhosWindow::Applied(const gfx::Rect& origin_bounds,
                         const gfx::Rect& new_bounds,
                         bool force) {
  bool origin_changed =
      origin_bounds.origin() != new_bounds.origin() || force;
  PlatformWindowDelegate::BoundsChange bounds(origin_changed);
  delegate()->OnBoundsChanged(bounds);
}

void OhosWindow::Show(bool inactive) {
  if (is_visible_) {
    return;
  }
  is_visible_ = true;
}

void OhosWindow::Hide() {
  if (!is_visible_) {
    return;
  }
  is_visible_ = false;
}

void OhosWindow::Close() {
  std::string id = GetWindowUniqueId();
  ohos::adapter::accessibility::AccessibilityDelegateOhosRegistry::GetInstance()
      .UnregisterAccessibilityDelegate(id);
  delegate()->OnClosed();
}

bool OhosWindow::IsVisible() const {
  NOTREACHED();
  return false;
}

void OhosWindow::PrepareForShutdown() {}

void OhosWindow::SetBoundsInPixels(const gfx::Rect& bounds) {
  NOTIMPLEMENTED_LOG_ONCE();
}

gfx::Rect OhosWindow::GetBoundsInPixels() const {
  return bounds_in_pixels_;
}

void OhosWindow::SetBoundsInDIP(const gfx::Rect& bounds) {
  SetBoundsInPixels(delegate()->ConvertRectToPixels(bounds));
}

gfx::Rect OhosWindow::GetBoundsInDIP() const {
  return delegate()->ConvertRectToDIP(bounds_in_pixels_);
}

void OhosWindow::SetTitle(const std::u16string& title) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void OhosWindow::SetCapture() {
  if (!HasCapture()) {
    window_manager()->GrabLocatedEvents(this);
  }
}

void OhosWindow::ReleaseCapture() {
  if (HasCapture()) {
    window_manager()->UngrabLocatedEvents(this);
  }
}

bool OhosWindow::HasCapture() const {
  return window_manager()->located_events_grabber() == this;
}

void OhosWindow::SetFullscreen(bool fullscreen, int64_t target_display_id) {}

void OhosWindow::Maximize() {}

void OhosWindow::Minimize() {}

void OhosWindow::Restore() {}

PlatformWindowState OhosWindow::GetPlatformWindowState() const {
  // Remove normal state for all the other types of windows as it's only the
  // OhosToplevelWindow that supports state changes.
  return PlatformWindowState::kNormal;
}

bool OhosWindow::HasInitDone() {
  return false;
}

void OhosWindow::Activate() {}

void OhosWindow::OnActivateEvent() {
  if (activation_state_ != ActivationState::kActive) {
    activation_state_ = ActivationState::kActive;
    delegate()->OnActivationChanged(/*active=*/true);
  }
}

void OhosWindow::Deactivate() {
  if (activation_state_ != ActivationState::kInactive) {
    activation_state_ = ActivationState::kInactive;
    delegate()->OnActivationChanged(/*active=*/false);
  }
}

void OhosWindow::SetSurfaceId(uint64_t surface_id) {
  delegate()->SetSurfaceId(surface_id);
}

void OhosWindow::SetUseNativeFrame(bool use_native_frame) {}

bool OhosWindow::ShouldUseNativeFrame() const {
  NOTIMPLEMENTED_LOG_ONCE();
  return false;
}

void OhosWindow::SetCursor(scoped_refptr<PlatformCursor> platform_cursor) {
  if (!has_pointer_focus_) {
    return;
  }
  if (cursor_ == platform_cursor) {
    return;
  }
  UpdateCursorShape(BitmapCursor::FromPlatformCursor(platform_cursor));
}

void OhosWindow::UpdateCursorShape(
    scoped_refptr<BitmapCursor> platform_cursor) {
  if (platform_cursor == nullptr) {
    LOG(WARNING) << "platform cursor null";
    return;
  }
  if (platform_cursor->type() == mojom::CursorType::kCustom) {
    auto bitmap = platform_cursor->bitmap();
    auto pixmap = bitmap.pixmap();
    size_t buff_size = bitmap.computeByteSize();
    std::shared_ptr<char[]> buff = std::make_shared<char[]>(buff_size);
    size_t row_bytes = bitmap.rowBytes();
    if (pixmap.readPixels(SkImageInfo::MakeN32Premul(bitmap.width(), bitmap.height()),
                          buff.get(), row_bytes, 0, 0)) {
      auto hotspot = platform_cursor->hotspot();
      ohos::adapter::CustomCusorInfo cursor_info{util::ConvertWidgetIdToWindowId(GetWidget()),
                                                 bitmap.width(),
                                                 bitmap.height(),
                                                 hotspot.x(),
                                                 hotspot.y(),
                                                 buff};
      ohos::adapter::SetCustomCursor(cursor_info);
    } else {
      LOG(ERROR) << "read pixels failed";
    }
  } else {
    auto ohos_cursor_type = ConvertToOhosCursorType(platform_cursor->type());
    ohos::adapter::SetCursor(GetWidget(), ohos_cursor_type);
  }
  cursor_ = platform_cursor;
}

void OhosWindow::MoveCursorTo(const gfx::Point& location) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void OhosWindow::ConfineCursorToBounds(const gfx::Rect& bounds) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void OhosWindow::SetRestoredBoundsInDIP(const gfx::Rect& bounds) {
  restored_bounds_in_pixels_ = delegate()->ConvertRectToPixels(bounds);
}

gfx::Rect OhosWindow::GetRestoredBoundsInDIP() const {
  if (state_ == PlatformWindowState::kNormal) {
    // Restoring a normal window has no effect.
    return gfx::Rect();
  }
  return delegate()->ConvertRectToDIP(restored_bounds_in_pixels_);
}

void OhosWindow::SetWindowIcons(const gfx::ImageSkia& window_icon,
                                const gfx::ImageSkia& app_icon) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void OhosWindow::SizeConstraintsChanged() {
  NOTIMPLEMENTED_LOG_ONCE();
}

// PlatformEventDispatcher
bool OhosWindow::CanDispatchEvent(const PlatformEvent& event) {
  return CanAcceptEvent(*event);
}

uint32_t OhosWindow::DispatchEvent(const PlatformEvent& native_event) {
  Event* event = static_cast<Event*>(native_event);
  if (event->IsLocatedEvent()) {
    auto* event_grabber = window_manager()->located_events_grabber();
    auto* root_parent_window = GetRootParentWindow();
    bool send_to_grabber =
        event_grabber &&
        root_parent_window == event_grabber->GetRootParentWindow();
    auto location = event->AsLocatedEvent()->location_f();
    if (send_to_grabber) {
      gfx::Vector2d diff =
          static_cast<OhosWindow*>(event->target())->bounds_in_pixels_.origin() - 
          event_grabber->bounds_in_pixels_.origin();
      event->AsLocatedEvent()->set_location_f(location + diff);
      Event::DispatcherApi(event).set_target(event_grabber);
      event_grabber->DispatchEventToDelegate(event);
      return POST_DISPATCH_STOP_PROPAGATION;
    }
    if (IsHitCaptionButton(location)) {
      LOG(INFO) << __FUNCTION__ << " Hit caption button, stop propagation";
      return POST_DISPATCH_STOP_PROPAGATION;
    }
  }
  // Dispatch all keyboard events to the root window.
  if (event->IsKeyEvent()) {
    auto* root_parent_window = GetRootParentWindow();
    if (root_parent_window) {
      return root_parent_window->DispatchEventToDelegate(event);
    }
  }
  return DispatchEventToDelegate(event);
}

uint32_t OhosWindow::DispatchEventToDelegate(const PlatformEvent& native_event) {
  bool handled = DispatchEventFromNativeUiEvent(
      native_event, base::BindOnce(&PlatformWindowDelegate::DispatchEvent,
                                   base::Unretained(delegate())));
  return handled ? POST_DISPATCH_STOP_PROPAGATION : POST_DISPATCH_NONE;
}

// EventTarget:
bool OhosWindow::CanAcceptEvent(const Event& event) {
  return this == event.target();
}

EventTarget* OhosWindow::GetParentTarget() {
  return nullptr;
}

std::unique_ptr<EventTargetIterator> OhosWindow::GetChildIterator() const {
  return nullptr;
}

EventTargeter* OhosWindow::GetEventTargeter() {
  return nullptr;
}

void OhosWindow::OnWindowLostCapture() {
  if (delegate()) {
    delegate()->OnLostCapture();
  }
}

OhosWindow* OhosWindow::GetRootParentWindow() {
  return parent_window_ ? parent_window_->GetRootParentWindow() : this;
}

void OhosWindow::OnPointerFocusChanged(const bool focused) {
  has_pointer_focus_ = focused;
  if (cursor_ == nullptr) {
    return;
  }
  if (focused) {
    UpdateCursorShape(cursor_);
  } else if (cursor_->type() == mojom::CursorType::kNone) {
    // invisible cursor blur need to set visible
    ohos::adapter::SetCursorVisible(true);
  }
}

OhosPopup* OhosWindow::AsOhosPopup() {
  return nullptr;
}

OhosToplevelWindow* OhosWindow::AsOhosToplevelWindow() {
  return nullptr;
}

WindowInitParameter OhosWindow::BuildWindowInitParameter() {
  return WindowInitParameter();
}

void OhosWindow::SetDragSourceWidget() {
  window_manager()->SetDragSourceWindow(AsWeakPtr());
}

void OhosWindow::ClearDragSourceWidget() {
  window_manager()->SetDragSourceWindow(nullptr);
}

void OhosWindow::EndDrag() {
  auto drag_source_window = window_manager()->GetDragSourceWindow();
  if (drag_source_window && drag_source_window->GetWidget() != GetWidget()) {
    drag_source_window->GetDragManager()->DragEnd();
  }
  drag_manager_->DragEnd();
}

}  // namespace ui
