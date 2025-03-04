// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_FAKE_WINDOW_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_FAKE_WINDOW_H_

#include "base/memory/raw_ptr.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace ui {

class OhosFakeWindow : public OhosWindow {
 public:
  OhosFakeWindow(PlatformWindowDelegate* delegate, OhosWindowManager* manager);
  OhosFakeWindow(const OhosFakeWindow&) = delete;
  OhosFakeWindow& operator=(const OhosFakeWindow&) = delete;

  ~OhosFakeWindow() override;

  bool Initialize(PlatformWindowInitProperties properties) override;

 private:
  // PlatformWindow:
  void Show(bool inactive) override;
  void Hide() override;
  void Close() override;
  bool IsVisible() const override;
  void PrepareForShutdown() override;
  void SetBoundsInPixels(const gfx::Rect& bounds) override;
  gfx::Rect GetBoundsInPixels() const override;
  void SetBoundsInDIP(const gfx::Rect& bounds) override;
  gfx::Rect GetBoundsInDIP() const override;
  void SetTitle(const std::u16string& title) override;
  void SetCapture() override;
  void ReleaseCapture() override;
  bool HasCapture() const override;
  void Activate() override;
  void Deactivate() override;
  void SetCursor(scoped_refptr<PlatformCursor> cursor) override;
  void MoveCursorTo(const gfx::Point& location) override;
  void ConfineCursorToBounds(const gfx::Rect& bounds) override;
  void SetRestoredBoundsInDIP(const gfx::Rect& bounds) override;
  gfx::Rect GetRestoredBoundsInDIP() const override;
  void SetWindowIcons(const gfx::ImageSkia& window_icon,
                      const gfx::ImageSkia& app_icon) override;
  void SizeConstraintsChanged() override;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_FAKE_WINDOW_H_
