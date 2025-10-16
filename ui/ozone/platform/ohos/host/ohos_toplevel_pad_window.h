/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
 
#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_TOPLEVEL_PAD_WINDOW_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_TOPLEVEL_PAD_WINDOW_H_
 
#include "ui/ozone/platform/ohos/host/ohos_toplevel_window.h"
 
namespace ui {
 
class OhosToplevelPadWindow : public OhosToplevelWindow {
 public:
  OhosToplevelPadWindow(PlatformWindowDelegate* delegate,
                     OhosWindowManager* manager);
  OhosToplevelPadWindow(const OhosToplevelPadWindow&) = delete;
  OhosToplevelPadWindow& operator=(const OhosToplevelPadWindow&) = delete;
  ~OhosToplevelPadWindow() override = default;
 
 private:
  void HandleEvent(std::shared_ptr<XCEvent> event) override;
  void SetWindowState(PlatformWindowState new_state, bool isTrigger) override;
  void OnWindowEvent(std::shared_ptr<XCEvent> event) override;
  
  void OnDeviceModeChanged(std::shared_ptr<XCEvent> event);
  void RecoverStateWhenDeviceModeChanged(PlatformWindowState recover_state);

  bool is_trigger_state_change_required_ = true;
  bool is_exit_fullscreen_required_ = false;
};
 
}  // namespace ui
 
#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_TOPLEVEL_PAD_WINDOW_H_