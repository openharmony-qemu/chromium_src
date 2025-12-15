// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

  std::string window_id = GetWindowId(window);
  if (window_id == kInvalidWindowId) {
    return;
  }

  auto existing_delegate = ohos::adapter::accessibility::
                               AccessibilityDelegateOhosRegistry::GetInstance()
                                   .GetAccessibilityDelegate(window_id);
  if (existing_delegate == nullptr) {
    auto accessibility_bridge =
        std::make_shared<ui::AccessibilityBridgeOhosImpl>(window);
    ohos::adapter::accessibility::AccessibilityDelegateOhosRegistry::
        GetInstance()
            .RegisterAccessibilityDelegate(window_id, accessibility_bridge);
  }
}

// static
std::unique_ptr<ViewAccessibility>
ViewAXPlatformNodeDelegate::CreatePlatformSpecific(View* view) {
  auto result = std::make_unique<ViewAXPlatformNodeDelegateOhos>(view);
  result->Init();
  return result;
}

ViewAXPlatformNodeDelegateOhos::ViewAXPlatformNodeDelegateOhos(View* view)
    : ViewAXPlatformNodeDelegate(view) {
  if (view) {
    view->AddObserver(this);
  }
}

ViewAXPlatformNodeDelegateOhos::~ViewAXPlatformNodeDelegateOhos() {
  if (view()) {
    view()->RemoveObserver(this);
  }
}

void ViewAXPlatformNodeDelegateOhos::OnViewAddedToWidget(View* observed_view) {
  if (registered_) {
    return;
  }
  RegisterAccessibilityBridge(observed_view->GetWidget());
  registered_ = true;
}

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