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

#include "ui/views/widget/desktop_aura/desktop_window_tree_host_ohos.h"

#include "base/logging.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ui/aura/window_delegate.h"
#include "ui/display/screen_ohos.h"
#include "ui/platform_window/platform_window.h"
#include "ui/platform_window/platform_window_init_properties.h"
#include "ui/platform_window/wm/wm_move_resize_handler.h"
#include "ui/views/widget/desktop_aura/window_event_filter_ohos.h"

using WindowAdapter = ohos::adapter::xcomponent::WindowAdapter;

namespace views {

DesktopWindowTreeHostOhos::DesktopWindowTreeHostOhos(
    internal::NativeWidgetDelegate* native_widget_delegate,
    DesktopNativeWidgetAura* desktop_native_widget_aura)
    : DesktopWindowTreeHostPlatform(native_widget_delegate,
                                    desktop_native_widget_aura) {}

DesktopWindowTreeHostOhos::~DesktopWindowTreeHostOhos() = default;

void DesktopWindowTreeHostOhos::OnNativeWidgetCreated(
    const Widget::InitParams& params) {
  CreateNonClientEventFilter();
  DesktopWindowTreeHostPlatform::OnNativeWidgetCreated(params);
}

void DesktopWindowTreeHostOhos::OnClosed() {
  DestroyNonClientEventFilter();
  DesktopWindowTreeHostPlatform::OnClosed();
}

void DesktopWindowTreeHostOhos::Show(ui::mojom::WindowShowState show_state,
                                     const gfx::Rect& restore_bounds) {
  if (show_state == ui::mojom::WindowShowState::kMaximized) {
    auto window_rect = WindowAdapter::GetInstance().GetInitialBounds();
    gfx::Rect rect_in_pixels = {window_rect.left, window_rect.top,
                                window_rect.width, window_rect.height};
    platform_window()->SetBoundsInDIP(
        display::ohos::ScreenOhos::ConvertPixelToDIP(rect_in_pixels));
  }
  DesktopWindowTreeHostPlatform::Show(show_state, restore_bounds);
}

void DesktopWindowTreeHostOhos::AddAdditionalInitProperties(
    const Widget::InitParams& params,
    ui::PlatformWindowInitProperties* properties) {
  // Set the background color on create XComponent Window, before chromium
  // widget is mapped and show.
  // If possible, we use the content window's background color,
  // otherwise we fallback to white.
  ui::ColorId target_color;
  switch (properties->type) {
    case ui::PlatformWindowType::kBubble:
      target_color = ui::kColorBubbleBackground;
      break;
    case ui::PlatformWindowType::kTooltip:
      target_color = ui::kColorTooltipBackground;
      break;
    default:
      target_color = ui::kColorWindowBackground;
      break;
  }
  properties->background_color =
      GetWidget()->GetColorProvider()->GetColor(target_color);
}

void DesktopWindowTreeHostOhos::DispatchEvent(ui::Event* event) {
  if (!event) {
    LOG(ERROR) << "DispatchEvent called with null event";
    return;
  }
  if (non_client_window_event_filter_ == nullptr) {
    CreateNonClientEventFilter();
  }
  int hit_test_code = HTNOWHERE;
  if (event->IsMouseEvent() || event->IsTouchEvent()) {
    ui::LocatedEvent* located_event = event->AsLocatedEvent();
    if (GetContentWindow() && GetContentWindow()->delegate()) {
      int flags = located_event->flags();
      gfx::PointF location = located_event->location_f();
      gfx::PointF location_in_dip =
          GetRootTransform().InverseMapPoint(location).value_or(location);
      hit_test_code = GetContentWindow()->delegate()->GetNonClientComponent(
          gfx::ToRoundedPoint(location_in_dip));
      if (hit_test_code != HTCLIENT && hit_test_code != HTNOWHERE) {
        flags |= ui::EF_IS_NON_CLIENT;
      }
      located_event->SetFlags(flags);
    }

    // While we unset the urgency hint when we gain focus, we also must remove
    // it on mouse clicks because we can call FlashFrame() on an active window.
    if (located_event->IsMouseEvent() &&
        (located_event->AsMouseEvent()->IsAnyButton() ||
         located_event->IsMouseWheelEvent())) {
      FlashFrame(false);
    }
  }

  // Prehandle the event as long as as we are not able to track if it is handled
  // or not as SendEventToSink results in copying the event and our copy of the
  // event will not set to handled unless a dispatcher or a target are
  // destroyed.
  if ((event->IsMouseEvent() || event->IsTouchEvent()) &&
      non_client_window_event_filter_) {
    non_client_window_event_filter_->HandleLocatedEventWithHitTest(
        hit_test_code, event->AsLocatedEvent());
  }

  if (!event->handled()) {
    WindowTreeHostPlatform::DispatchEvent(event);
  }
}

void DesktopWindowTreeHostOhos::CreateNonClientEventFilter() {
  DCHECK(!non_client_window_event_filter_);
  non_client_window_event_filter_ = std::make_unique<WindowEventFilterOhos>(
      this, GetWmMoveResizeHandler(*platform_window()));
}

void DesktopWindowTreeHostOhos::DestroyNonClientEventFilter() {
  non_client_window_event_filter_.reset();
}

Widget::MoveLoopResult DesktopWindowTreeHostOhos::RunMoveLoop(
    const gfx::Vector2d& drag_offset,
    Widget::MoveLoopSource source,
    Widget::MoveLoopEscapeBehavior escape_behavior) {
  auto weak_this = weak_factory_.GetWeakPtr();

  Widget::MoveLoopResult result = DesktopWindowTreeHostPlatform::RunMoveLoop(
      drag_offset, source, escape_behavior);
  if (weak_this.get()) {
    GetContentWindow()->ReleaseCapture();
  }
  return result;
}

// static
DesktopWindowTreeHost* DesktopWindowTreeHost::Create(
    internal::NativeWidgetDelegate* native_widget_delegate,
    DesktopNativeWidgetAura* desktop_native_widget_aura) {
  return new DesktopWindowTreeHostOhos(native_widget_delegate,
                                       desktop_native_widget_aura);
}

void DesktopWindowTreeHostOhos::SetVisiblebyOcclusionState(
    aura::Window::OcclusionState state) {
  if (occlusion_state_ == state) {
    return;
  }
  occlusion_state_ = state;
  // Notify egl surface.
  if (state == aura::Window::OcclusionState::OCCLUDED ||
      state == aura::Window::OcclusionState::HIDDEN) {
    SetVisibleOHOS(false);
  } else {
    SetVisibleOHOS(true);
  }
}

void DesktopWindowTreeHostOhos::OnOcclusionStateChanged(
    ui::PlatformWindowOcclusionState occlusion_state) {
  WindowTreeHostPlatform::OnOcclusionStateChanged(occlusion_state);
  SetVisiblebyOcclusionState(GetNativeWindowOcclusionState());
}

}  // namespace views
