// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_fake_window.h"

#include "base/memory/scoped_refptr.h"
#include "base/notreached.h"
#include "ui/base/cursor/platform_cursor.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace ui {

OhosFakeWindow::OhosFakeWindow(PlatformWindowDelegate* delegate,
                               OhosWindowManager* manager)
    : OhosWindow(delegate, manager) {}

OhosFakeWindow::~OhosFakeWindow() = default;

bool OhosFakeWindow::Initialize(PlatformWindowInitProperties properties) {
  return true;
}

void OhosFakeWindow::Show(bool inactive) {}

void OhosFakeWindow::Hide() {}

void OhosFakeWindow::Close() {}

bool OhosFakeWindow::IsVisible() const {
  return true;
}

void OhosFakeWindow::PrepareForShutdown() {}

void OhosFakeWindow::SetBoundsInPixels(const gfx::Rect& bounds) {}

gfx::Rect OhosFakeWindow::GetBoundsInPixels() const {
  return gfx::Rect();
}

void OhosFakeWindow::SetBoundsInDIP(const gfx::Rect& bounds) {}

gfx::Rect OhosFakeWindow::GetBoundsInDIP() const {
  return gfx::Rect();
}

void OhosFakeWindow::SetTitle(const std::u16string& title) {}

void OhosFakeWindow::SetCapture() {}

void OhosFakeWindow::ReleaseCapture() {}

bool OhosFakeWindow::HasCapture() const {
  return false;
}

void OhosFakeWindow::Activate() {}

void OhosFakeWindow::Deactivate() {}

void OhosFakeWindow::SetCursor(scoped_refptr<PlatformCursor> cursor) {}

void OhosFakeWindow::MoveCursorTo(const gfx::Point& location) {}

void OhosFakeWindow::ConfineCursorToBounds(const gfx::Rect& bounds) {}

void OhosFakeWindow::SetRestoredBoundsInDIP(const gfx::Rect& bounds) {}

gfx::Rect OhosFakeWindow::GetRestoredBoundsInDIP() const {
  return gfx::Rect();
}

void OhosFakeWindow::SetWindowIcons(const gfx::ImageSkia& window_icon,
                                    const gfx::ImageSkia& app_icon) {}

void OhosFakeWindow::SizeConstraintsChanged() {}

}  // namespace ui
