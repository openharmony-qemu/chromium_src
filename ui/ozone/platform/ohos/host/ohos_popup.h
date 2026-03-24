// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_POPUP_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_POPUP_H_

#include "ui/display/display.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"

namespace ui {

class OhosPopup : public OhosWindow {
 public:
  OhosPopup(PlatformWindowDelegate* delegate,
            OhosWindowManager* manager,
            OhosWindow* parent);

  OhosPopup(const OhosPopup&) = delete;
  OhosPopup& operator=(const OhosPopup&) = delete;

  ~OhosPopup() override;

  void OnInitialize(PlatformWindowInitProperties properties) override;

  bool OnCreateWindow(WindowInitParameter param) override;

  // ohos window event overrides
  void HandleEvent(std::shared_ptr<XCEvent> event) override;

  // PlatformWindow
  void Show(bool inactive) override;
  void Hide() override;
  void Close() override;
  bool IsVisible() const override;
  PlatformWindowState GetPlatformWindowState() const override;
  void SetBoundsInPixels(const gfx::Rect& bounds) override;

  OhosPopup* AsOhosPopup() override;
  WindowInitParameter BuildWindowInitParameter() override;
  display::Display GetCurrentDisplay() override;

  bool ShouldWindowContentsBeTransparent() const override;

 private:
  void OnFocusEvent();
  void OnBlurEvent();
  void OnWindowSizeChangeEvent(std::shared_ptr<XCEvent> event);

  void SetWindowState(PlatformWindowState state);

  // Contains the current state of the window.
  PlatformWindowState state_ = PlatformWindowState::kUnknown;

  // Contains the previous state of the window.
  PlatformWindowState previous_state_ = PlatformWindowState::kUnknown;

  SkColor init_color_arbg_{SK_ColorTRANSPARENT};

  bool menu_created_ = false;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_POPUP_H_
