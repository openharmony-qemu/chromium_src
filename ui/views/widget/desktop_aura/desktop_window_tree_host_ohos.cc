// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/widget/desktop_aura/desktop_window_tree_host_ohos.h"

#include "base/logging.h"
#include "ui/aura/window_delegate.h"
#include "ui/platform_window/platform_window_init_properties.h"
#include "ui/platform_window/wm/wm_move_resize_handler.h"
#include "ui/views/widget/desktop_aura/window_event_filter_ohos.h"

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

// static
DesktopWindowTreeHost* DesktopWindowTreeHost::Create(
    internal::NativeWidgetDelegate* native_widget_delegate,
    DesktopNativeWidgetAura* desktop_native_widget_aura) {
  return new DesktopWindowTreeHostOhos(native_widget_delegate,
                                       desktop_native_widget_aura);
}

}  // namespace views
