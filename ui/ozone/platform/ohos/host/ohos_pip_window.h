/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_PIP_WINDOW_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_PIP_WINDOW_H_

#include <future>

#include "base/memory/raw_ptr.h"
#include "content/public/browser/video_picture_in_picture_window_controller.h"
#include "ohos/adapter/window/pip_window_adapter.h"
#include "ui/display/display.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"

namespace ui {

using namespace ohos::adapter::window;

class OhosPipWindow : public OhosWindow {
 public:
  OhosPipWindow(PlatformWindowDelegate* delegate, OhosWindowManager* manager);
  OhosPipWindow(const OhosPipWindow&) = delete;
  OhosPipWindow& operator=(const OhosPipWindow&) = delete;
  ~OhosPipWindow() override;

  void OnInitialize(PlatformWindowInitProperties properties) override;

  // Creates a surface window, which is visible as a main window.
  bool OnCreateWindow(WindowInitParameter param) override;

  // PlatformWindow
  void Hide() override;
  void Close() override;
  bool IsVisible() const override;
  void SetBoundsInPixels(const gfx::Rect& bounds) override;
  WindowInitParameter BuildWindowInitParameter() override;

  void CreatePipWindow();
  void CreateAndRegisterNativeWindow(uint64_t surface_id);
  void SetPipState(PictureInPicture_PipState pip_state);

  void OnPipControlEventCallback(PictureInPicture_PipControlType control_type,
                                 PictureInPicture_PipControlStatus status);
  void OnPipResizeCallback(uint32_t width, uint32_t height, double scale);
  void OnPipLifecycleCallback(PictureInPicture_PipState state,
                              int32_t err_code);

  void OnPipControlClose();
  void NotifyPipWindowCreated();
  base::WeakPtr<OhosPipWindow> GetWeakPtr();
  display::Display GetCurrentDisplay() override;

 private:
  PictureInPicture_PipConfig ConfigurePipWindow();
  void RegisterCallbacks();
  bool StopPipController();
  void CloseInternal();

  void RegisterControlCallbacks();

  std::promise<bool> pip_window_created_promise_;

  uint32_t controller_id_;

  double scale_;

  gfx::AcceleratedWidget parent_widget_;

  uint64_t surface_id_;

  PictureInPicture_PipState pip_state_;

  raw_ptr<content::VideoPictureInPictureWindowController> pip_controller_;

  base::WeakPtrFactory<OhosPipWindow> weak_ohos_pip_window_factory_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_PIP_WINDOW_H_
