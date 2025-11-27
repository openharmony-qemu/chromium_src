// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_WIDGET_DESKTOP_AURA_DESKTOP_WINDOW_TREE_HOST_OHOS_H_
#define UI_VIEWS_WIDGET_DESKTOP_AURA_DESKTOP_WINDOW_TREE_HOST_OHOS_H_

#include "ui/views/views_export.h"
#include "ui/views/widget/desktop_aura/desktop_window_tree_host_platform.h"

namespace views {

class WindowEventFilterOhos;

class VIEWS_EXPORT DesktopWindowTreeHostOhos
    : public DesktopWindowTreeHostPlatform {
 public:
  DesktopWindowTreeHostOhos(
      internal::NativeWidgetDelegate* native_widget_delegate,
      DesktopNativeWidgetAura* desktop_native_widget_aura);

  DesktopWindowTreeHostOhos(const DesktopWindowTreeHostOhos&) = delete;
  DesktopWindowTreeHostOhos& operator=(const DesktopWindowTreeHostOhos&) =
      delete;

  ~DesktopWindowTreeHostOhos() override;

  void LockMouse(aura::Window* window) override;
  void UnlockMouse(aura::Window* window) override;

 protected:
  // Overridden from DesktopWindowTreeHost:
  void OnNativeWidgetCreated(const Widget::InitParams& params) override;
  Widget::MoveLoopResult RunMoveLoop(
    const gfx::Vector2d& drag_offset,
    Widget::MoveLoopSource source,
    Widget::MoveLoopEscapeBehavior escape_behavior) override;
  // PlatformWindowDelegate:
  void DispatchEvent(ui::Event* event) override;
  void OnClosed() override;

  // DesktopWindowTreeHostPlatform:
  void AddAdditionalInitProperties(
      const Widget::InitParams& params,
      ui::PlatformWindowInitProperties* properties) override;

 private:
  void CreateNonClientEventFilter();
  void DestroyNonClientEventFilter();

  // A handler for events intended for non client area.
  // A posthandler for events intended for non client area. Handles events if no
  // other consumer handled them.
  std::unique_ptr<WindowEventFilterOhos> non_client_window_event_filter_;

  base::WeakPtrFactory<DesktopWindowTreeHostOhos> weak_factory_{this};
};

}  // namespace views

#endif  // UI_VIEWS_WIDGET_DESKTOP_AURA_DESKTOP_WINDOW_TREE_HOST_OHOS_H_