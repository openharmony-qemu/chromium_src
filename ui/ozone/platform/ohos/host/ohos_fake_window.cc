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

#include "ui/ozone/platform/ohos/host/ohos_fake_window.h"

#include "base/memory/scoped_refptr.h"
#include "base/notreached.h"
#include "ui/base/cursor/platform_cursor.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace ui {

OhosFakeWindow::OhosFakeWindow(PlatformWindowDelegate* delegate,
                               OhosWindowManager* manager)
    : OhosWindow(delegate, manager) {
}

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
