// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_TOPLEVEL_WINDOW_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_TOPLEVEL_WINDOW_H_

#include "base/memory/raw_ptr.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"
#include "ui/platform_window/wm/wm_move_loop_handler.h"
#include "ui/platform_window/wm/wm_move_resize_handler.h"

namespace ui {

class OhosToplevelWindow : public OhosWindow, public WmMoveResizeHandler {
 public:
  OhosToplevelWindow(PlatformWindowDelegate* delegate,
                     OhosWindowManager* manager);
  OhosToplevelWindow(const OhosToplevelWindow&) = delete;
  OhosToplevelWindow& operator=(const OhosToplevelWindow&) = delete;
  ~OhosToplevelWindow() override;

  void OnInitialize(PlatformWindowInitProperties properties) override;

  // Creates a surface window, which is visible as a main window.
  bool OnCreateWindow() override;

  // Window event overrides
  void HandleEvent(std::shared_ptr<XCEvent> event) override;

  // WmMoveResizeHandler
  void DispatchHostWindowDragMovement(
      int hittest,
      const gfx::Point& pointer_location_in_px) override;

  // PlatformWindow:
  void Hide() override;
  void Close() override;
  bool IsVisible() const override;
  void SetFullscreen(bool fullscreen, int64_t target_display_id) override;
  void Maximize() override;
  void Minimize() override;
  void Restore() override;
  PlatformWindowState GetPlatformWindowState() const override;
  void Activate() override;
  void SizeConstraintsChanged() override;
  void SetUseNativeFrame(bool use_native_frame) override;
  bool ShouldUseNativeFrame() const override;
  void SetBoundsInPixels(const gfx::Rect& bounds) override;

  OhosToplevelWindow* AsOhosTopWindow();

 private:
  void OnFocusEvent();
  void OnBlurEvent();
  void OnWindowEvent(std::shared_ptr<XCEvent> event);
  void OnWindowSizeChangeEvent(std::shared_ptr<XCEvent> event);
  void OnWindowRectChangeEvent(std::shared_ptr<XCEvent> event);

  void UnMaximize();

  void UpdateMinAndMaxSize();
  absl::optional<gfx::Size> GetMinimumSizeForOhosWindow();
  absl::optional<gfx::Size> GetMaximumSizeForOhosWindow();

  bool IsMaximized() { return PlatformWindowState::kMaximized == state_; }
  bool IsMinimized() { return PlatformWindowState::kMinimized == state_; }
  bool IsFullscreen() { return PlatformWindowState::kFullScreen == state_; }
  bool IsNormal() {
    return !IsMaximized() && !IsMinimized() && !IsFullscreen();
  }

  void SetWindowState(PlatformWindowState new_state, bool isTrigger = true);
  void TriggerStateChanges();
  void UpdateStateChanges();

  WmMoveResizeHandler* AsWmMoveResizeHandler();

  bool HasInitDone() override;

  // Contains the previous state of the window.
  PlatformWindowState previous_state_ = PlatformWindowState::kUnknown;

  // Contains the previous state of the window before enter fullscreen.
  PlatformWindowState previous_enter_fullscreen_state_ =
      PlatformWindowState::kUnknown;

  bool use_native_frame_ = false;

  gfx::Rect window_position_size_;

  // window's minimum size.
  gfx::Size min_size_in_pixels_;

  // window's maximum size.
  gfx::Size max_size_in_pixels_;

  int64_t target_display_id_ = -1;
  bool activatable_ = true;
  bool hide_title_bar_;
  bool using_system_floating_window_ = false;
  gfx::Point pointer_window_location_in_pixels_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_TOPLEVEL_WINDOW_H_
