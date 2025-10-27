// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/frame/desktop_browser_frame_aura_ohos.h"

#include "base/functional/bind.h"
#include "chrome/browser/ui/views/frame/browser_desktop_window_tree_host_ohos.h"
#include "chrome/browser/ui/views/frame/browser_frame.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/native_browser_frame_factory.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"
#include "ui/gfx/geometry/transform.h"
#include "ui/views/widget/widget.h"

constexpr gfx::AcceleratedWidget kDefaultWidget = 1;

namespace {

gfx::Transform GetTransform() {
  display::Display display = display::Screen::GetScreen()->GetPrimaryDisplay();
  gfx::Transform transform;
  float scale = display.device_scale_factor();
  transform.Scale(scale, scale);
  return transform;
}

gfx::Rect ConvertRectToDIP(const gfx::Rect& rect_in_pixels) {
  return GetTransform().InverseMapRect(rect_in_pixels).value_or(rect_in_pixels);
}

bool HasBrowserWindowInit() {
  return false;
}

gfx::Rect GetInitialBounds() {
  gfx::Rect test;
  return test;
}

}  // namespace

DesktopBrowserFrameAuraOhos::DesktopBrowserFrameAuraOhos(
    BrowserFrame* browser_frame,
    BrowserView* browser_view)
    : DesktopBrowserFrameAura(browser_frame, browser_view) {
  // Register to track when the widget has initialized. If a delegate is not
  // set, then the widget creator is responsible for calling
  // OnWidgetInitialized.
  views::WidgetDelegate* delegate =
      static_cast<views::WidgetDelegate*>(browser_view);
  if (delegate) {
    delegate->RegisterWidgetInitializedCallback(
        base::BindOnce(&DesktopBrowserFrameAuraOhos::OnWidgetInitialized,
                       base::Unretained(this)));
  }
}

DesktopBrowserFrameAuraOhos::~DesktopBrowserFrameAuraOhos() = default;

views::Widget::InitParams DesktopBrowserFrameAuraOhos::GetWidgetParams() {
  views::Widget::InitParams params(
      views::Widget::InitParams::NATIVE_WIDGET_OWNS_WIDGET);
  params.native_widget = this;

  Browser& browser = *browser_view()->browser();
  if (browser.is_type_devtools() || browser.is_type_popup()) {
    params.remove_standard_frame = false;
  } else {
    params.remove_standard_frame = !UseCustomFrame();
  }

  // Override browser bounds when start the first window
  if (!HasBrowserWindowInit()) {
    params.bounds = GetInitialBounds();
    if (!params.bounds.IsEmpty()) {
      browser.set_override_bounds(params.bounds);
    }
  }

  return params;
}

bool DesktopBrowserFrameAuraOhos::UseCustomFrame() const {
  return false;
}

void DesktopBrowserFrameAuraOhos::OnWidgetInitialized() {
  gfx::AcceleratedWidget widget = host_->GetAcceleratedWidget();
}

NativeBrowserFrame* NativeBrowserFrameFactory::Create(
    BrowserFrame* browser_frame,
    BrowserView* browser_view) {
  return new DesktopBrowserFrameAuraOhos(browser_frame, browser_view);
}
