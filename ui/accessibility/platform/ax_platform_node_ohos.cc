// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/accessibility/platform/ax_platform_node_ohos.h"

#include <codecvt>
#include <string>

#include "ohos/adapter/accessibility/accessibility_adapter.h"

namespace ui {

// static
AXPlatformNode::Pointer AXPlatformNode::Create(AXPlatformNodeDelegate& delegate) {
  AXPlatformNode* node = new AXPlatformNodeOHOS();
  node->Init(delegate);
  return Pointer(node);
}

// static
AXPlatformNode* AXPlatformNode::FromNativeViewAccessible(
    gfx::NativeViewAccessible accessible) {
  return static_cast<AXPlatformNodeOHOS*>(accessible);
}

AXPlatformNodeOHOS::AXPlatformNodeOHOS() = default;

AXPlatformNodeOHOS::~AXPlatformNodeOHOS() = default;

void AXPlatformNodeOHOS::Init(AXPlatformNodeDelegate& delegate) {
  AXPlatformNodeBase::Init(delegate);
  
  if (GetRole() != ax::mojom::Role::kUnknown) {
    GetOrCreateAccessibilityNodeInfo();
  }
}

void AXPlatformNodeOHOS::Destroy() {
  AXPlatformNodeBase::Destroy();
}

bool AXPlatformNodeOHOS::IsPlatformCheckable() const {
  if (GetRole() == ax::mojom::Role::kToggleButton) {
    return false;
  }
  return AXPlatformNodeBase::IsPlatformCheckable();
}

bool AXPlatformNodeOHOS::IsChecked() const {
  return GetData().GetCheckedState() == ax::mojom::CheckedState::kTrue;
}

bool AXPlatformNodeOHOS::IsClickable() const {
  return GetData().IsClickable();
}

bool AXPlatformNodeOHOS::ISelectionItemProviderIsSelected() const {
  // https://www.w3.org/TR/core-aam-1.1/#mapping_state-property_table
  // SelectionItem.IsSelected is set according to the True or False value of
  // aria-checked for 'radio' and 'menuitemradio' roles.
  if (GetRole() == ax::mojom::Role::kRadioButton ||
      GetRole() == ax::mojom::Role::kMenuItemRadio) {
    return GetData().GetCheckedState() == ax::mojom::CheckedState::kTrue;
  }

  // https://www.w3.org/TR/wai-aria-1.1/#aria-selected
  // SelectionItem.IsSelected is set according to the True or False value of
  // aria-selected.
  return GetBoolAttribute(ax::mojom::BoolAttribute::kSelected);
}

bool AXPlatformNodeOHOS::IsEnabled() const {
  switch (GetData().GetRestriction()) {
    case ax::mojom::Restriction::kNone:
      return true;
    case ax::mojom::Restriction::kReadOnly:
    case ax::mojom::Restriction::kDisabled:
      return false;
    default:
      NOTREACHED();
  }
  
  return true;
}

std::string AXPlatformNodeOHOS::GetText() const {
  if (IsTextField()) {
    return GetStringAttribute(ax::mojom::StringAttribute::kValue);
  }
  return GetName();
}

std::string AXPlatformNodeOHOS::GetComponentType() const {
  return GetStringAttribute(ax::mojom::StringAttribute::kClassName);
}

gfx::NativeViewAccessible AXPlatformNodeOHOS::GetNativeViewAccessible() {
  return GetOrCreateAccessibilityNodeInfo();
}

gfx::NativeViewAccessible
AXPlatformNodeOHOS::GetOrCreateAccessibilityNodeInfo() {
  return this;
}

}  // namespace ui