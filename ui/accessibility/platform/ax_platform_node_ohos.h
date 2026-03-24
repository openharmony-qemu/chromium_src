// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_ACCESSIBILITY_PLATFORM_AX_PLATFORM_NODE_OHOS_H_
#define UI_ACCESSIBILITY_PLATFORM_AX_PLATFORM_NODE_OHOS_H_

#include "ax_platform_node.h"
#include "ax_platform_node_base.h"
#include "ohos/adapter/accessibility/accessibility_adapter.h"

namespace ui {

using ohos::adapter::accessibility::NativeElementWrapper;

class AXPlatformNodeOHOS : public AXPlatformNodeBase,
                           public NativeElementWrapper {
 public:
  ~AXPlatformNodeOHOS() override;
  AXPlatformNodeOHOS(const AXPlatformNodeOHOS&) = delete;
  AXPlatformNodeOHOS& operator=(const AXPlatformNodeOHOS&) = delete;

  // AXPlatformNodeBase.
  void Destroy() override;
  bool IsPlatformCheckable() const override;

  bool IsChecked() const;
  bool IsClickable() const;
  bool IsLongClickable() const;
  bool ISelectionItemProviderIsSelected() const;
  bool IsEnabled() const;
  std::string GetHint() const;
  bool IsHint() const;
  bool IsSelected() const;
  bool IsCheckable() const;
  float GetOpacity() const;
  std::string GetText() const;
  std::string GetComponentType() const;
  std::string GetDescription() const;
  std::string GetRoleString() const;
  std::string GetBackgroundColor() const;
  std::string GetBackgroundImage() const;

  // AXPlatformNode overrides.
  // This has a side effect of creating the AccessibilityNodeInfo if one does
  // not already exist.
  gfx::NativeViewAccessible GetNativeViewAccessible() override;
  // Get or Create AccessibilityNodeInfo. Note that it could return nullptr
  // except ax::mojom::Role::kApplication when the mode is not enabled.
  gfx::NativeViewAccessible GetOrCreateAccessibilityNodeInfo();

 protected:
  AXPlatformNodeOHOS();
  void Init(AXPlatformNodeDelegate& delegate) override;

 private:
  bool ShouldExposeValueAsName() const;

  friend AXPlatformNode::Pointer AXPlatformNode::Create(
      AXPlatformNodeDelegate& delegate);
};

}  // namespace ui

#endif  // UI_ACCESSIBILITY_PLATFORM_AX_PLATFORM_NODE_OHOS_H_