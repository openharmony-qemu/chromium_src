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

  void Show(ui::mojom::WindowShowState show_state,
            const gfx::Rect& restore_bounds) override;

  ~DesktopWindowTreeHostOhos() override;
  std::unique_ptr<aura::ScopedEnableUnadjustedMouseEvents>
  RequestUnadjustedMovement() override;
  bool SupportsMouseLock() override;
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
  void OnCloseRequest() override;

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
