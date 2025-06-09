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

#include "ui/views/accessibility/view_ax_platform_node_delegate_ohos.h"

#include <memory>
#include <set>
#include <vector>

#include "base/memory/singleton.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/strings/utf_string_conversions.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_accessibility_state.h"
#include "ohos/adapter/accessibility/accessibility_delegate_ohos_registry.h"
#include "ui/accessibility/accessibility_switches.h"
#include "ui/accessibility/ax_node_data.h"
#include "ui/accessibility/ax_text_utils.h"
#include "ui/accessibility/platform/ax_platform_node_ohos.h"
#include "ui/accessibility/platform/ohos/accessibility_bridge_ohos_impl.h"
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"
#include "ui/aura/window_tree_host_platform.h"
#include "ui/base/layout.h"
#include "ui/display/screen.h"
#include "ui/platform_window/platform_window.h"
#include "ui/views/accessibility/views_utilities_aura.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget.h"

namespace views {
namespace {

gfx::Rect DIPtoPhysicalPixel(View* view) {
  if (view->GetWidget()) {
    gfx::NativeView native_view = view->GetWidget()->GetNativeView();
    if (native_view) {
      return display::Screen::GetScreen()->DIPToScreenRectInWindow(
          native_view, view->GetBoundsInScreen());
    }
  }
  return gfx::Rect();
}

}  // namespace

// Invalid or not found window ID
const std::string kInvalidWindowId = "";

// Return the widget of any parent window of |widget|, first checking for
// transient parent windows.
Widget* GetWidgetOfParentWindowIncludingTransient(Widget* widget) {
  if (!widget) {
    return nullptr;
  }

  aura::Window* window = widget->GetNativeWindow();
  if (!window) {
    return nullptr;
  }

  // Look for an ancestor window with a Widget, and if found, return
  // the NativeViewAccessible for its RootView.
  aura::Window* ancestor_window = GetWindowParentIncludingTransient(window);
  if (!ancestor_window) {
    return nullptr;
  }
  return Widget::GetWidgetForNativeView(ancestor_window);
}

// Return the toplevel widget ancestor of |widget|, including widgets of
// parents of transient windows.
Widget* GetToplevelWidgetIncludingTransientWindows(Widget* widget) {
  widget = widget->GetTopLevelWidget();
  if (Widget* parent_widget =
          GetWidgetOfParentWindowIncludingTransient(widget)) {
    return GetToplevelWidgetIncludingTransientWindows(parent_widget);
  }
  return widget;
}

std::string GetWindowId(aura::Window* window) {
  if (window->GetHost()) {
    aura::WindowTreeHostPlatform* host =
        static_cast<aura::WindowTreeHostPlatform*>(window->GetHost());
    if (host && host->platform_window()) {
      return host->platform_window()->GetWindowUniqueId();
    }
  }
  return kInvalidWindowId;
}

void RegisterAccessibilityBridge(Widget* widget) {
  if (!widget) {
    return;
  }

  widget = GetToplevelWidgetIncludingTransientWindows(widget);
  if (!widget) {
    return;
  }

  aura::Window* window = widget->GetNativeWindow();
  if (!window) {
    return;
  }

  auto accessibility_bridge =
      std::make_shared<ui::AccessibilityBridgeOhosImpl>(window);

  std::string window_id = GetWindowId(window);
  if (window_id == kInvalidWindowId) {
    return;
  }

  ohos::adapter::accessibility::AccessibilityDelegateOhosRegistry::GetInstance()
      .RegisterAccessibilityDelegate(window_id, accessibility_bridge);
}

// static
std::unique_ptr<ViewAccessibility> ViewAccessibility::Create(View* view) {
  RegisterAccessibilityBridge(view->GetWidget());

  auto result = std::make_unique<ViewAXPlatformNodeDelegateOhos>(view);
  result->Init();
  return result;
}

ViewAXPlatformNodeDelegateOhos::ViewAXPlatformNodeDelegateOhos(View* view)
    : ViewAXPlatformNodeDelegate(view) {}

ViewAXPlatformNodeDelegateOhos::~ViewAXPlatformNodeDelegateOhos() = default;

gfx::NativeViewAccessible ViewAXPlatformNodeDelegateOhos::GetParent() const {
  if (gfx::NativeViewAccessible parent =
          ViewAXPlatformNodeDelegate::GetParent()) {
    return parent;
  }

  Widget* parent_widget =
      GetWidgetOfParentWindowIncludingTransient(view()->GetWidget());
  if (parent_widget)
    return parent_widget->GetRootView()->GetNativeViewAccessible();

  return nullptr;
}

gfx::Rect ViewAXPlatformNodeDelegateOhos::GetBoundsRect(
    const ui::AXCoordinateSystem coordinate_system,
    const ui::AXClippingBehavior clipping_behavior,
    ui::AXOffscreenResult* offscreen_result) const {
  switch (coordinate_system) {
    case ui::AXCoordinateSystem::kScreenPhysicalPixels:
      return DIPtoPhysicalPixel(view());
    case ui::AXCoordinateSystem::kScreenDIPs:
      // We could optionally add clipping here if ever needed.
      return view()->GetBoundsInScreen();
    case ui::AXCoordinateSystem::kRootFrame:
    case ui::AXCoordinateSystem::kFrame:
      NOTIMPLEMENTED();
      return gfx::Rect();
    default:
      NOTREACHED() << "Unexpected coordinate system: "
                   << static_cast<int>(coordinate_system);
      return gfx::Rect();
  }
}

}  // namespace views
