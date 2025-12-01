// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_ACCESSIBILITY_VIEW_AX_PLATFORM_NODE_DELEGATE_OHOS_H_
#define UI_VIEWS_ACCESSIBILITY_VIEW_AX_PLATFORM_NODE_DELEGATE_OHOS_H_

#include "ui/views/accessibility/view_ax_platform_node_delegate.h"

#include "ohos/adapter/accessibility/accessibility_adapter.h"

namespace views {

using ohos::adapter::accessibility::NativeElementWrapper;

class View;

class ViewAXPlatformNodeDelegateOhos : public ViewAXPlatformNodeDelegate,
                                       public ViewObserver,
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

  // ViewObserver overrides.
  void OnViewAddedToWidget(View* observed_view) override;

 private:
  bool registered_ = false;
};

}  // namespace views

#endif  // UI_VIEWS_ACCESSIBILITY_VIEW_AX_PLATFORM_NODE_DELEGATE_OHOS_H_