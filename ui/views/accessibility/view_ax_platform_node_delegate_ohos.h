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

#ifndef UI_VIEWS_ACCESSIBILITY_VIEW_AX_PLATFORM_NODE_DELEGATE_OHOS_H_
#define UI_VIEWS_ACCESSIBILITY_VIEW_AX_PLATFORM_NODE_DELEGATE_OHOS_H_

#include "ui/views/accessibility/view_ax_platform_node_delegate.h"

#include "ohos/adapter/accessibility/accessibility_adapter.h"

namespace views {

using ohos::adapter::accessibility::NativeElementWrapper;

class View;

class ViewAXPlatformNodeDelegateOhos : public ViewAXPlatformNodeDelegate,
                                       public NativeElementWrapper {
 public:
  explicit ViewAXPlatformNodeDelegateOhos(View* view);
  ViewAXPlatformNodeDelegateOhos(const ViewAXPlatformNodeDelegateOhos&) =
      delete;
  ViewAXPlatformNodeDelegateOhos& operator=(
      const ViewAXPlatformNodeDelegateOhos&) = delete;
  ~ViewAXPlatformNodeDelegateOhos() override;

  // ViewAXPlatformNodeDelegate overrides.
  gfx::NativeViewAccessible GetParent() const override;
  gfx::Rect GetBoundsRect(
      const ui::AXCoordinateSystem coordinate_system,
      const ui::AXClippingBehavior clipping_behavior,
      ui::AXOffscreenResult* offscreen_result) const override;
};

}  // namespace views

#endif  // UI_VIEWS_ACCESSIBILITY_VIEW_AX_PLATFORM_NODE_DELEGATE_OHOS_H_
