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
