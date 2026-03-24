// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/accessibility/platform/ax_platform_node_ohos.h"

#include <codecvt>
#include <string>

#include "base/strings/stringprintf.h"
#include "ohos/adapter/accessibility/accessibility_adapter.h"
#include "third_party/skia/include/core/SkColor.h"

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

bool AXPlatformNodeOHOS::IsLongClickable() const {
  return HasAction(ax::mojom::Action::kLongClick);
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

std::string AXPlatformNodeOHOS::GetHint() const {
  std::vector<std::string> strings;

  if (ShouldExposeValueAsName()) {
    std::string name = GetName();
    if (!name.empty()) {
      strings.emplace_back(name);
    }
  }

  if (GetData().GetNameFrom() != ax::mojom::NameFrom::kPlaceholder) {
    std::string placeholder =
        GetStringAttribute(ax::mojom::StringAttribute::kPlaceholder);
    if (!placeholder.empty()) {
      strings.emplace_back(placeholder);
    }
  }

  std::string description =
      GetStringAttribute(ax::mojom::StringAttribute::kDescription);
  if (!description.empty()) {
    strings.emplace_back(description);
  }

  return base::JoinString(strings, " ");
}

bool AXPlatformNodeOHOS::IsHint() const {
  return !GetHint().empty();
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

std::string AXPlatformNodeOHOS::GetRoleString() const {
  return ui::ToString(GetRole());
}

std::string AXPlatformNodeOHOS::GetDescription() const {
  return GetStringAttribute(ax::mojom::StringAttribute::kDescription);
}

std::string AXPlatformNodeOHOS::GetBackgroundColor() const {
  if (HasIntAttribute(ax::mojom::IntAttribute::kBackgroundColor)) {
    return base::StringPrintf(
        "#%X", GetIntAttribute(ax::mojom::IntAttribute::kBackgroundColor));
  }
  return "";
}

std::string AXPlatformNodeOHOS::GetBackgroundImage() const {
  if (GetRole() == ax::mojom::Role::kImage &&
      HasStringAttribute(ax::mojom::StringAttribute::kUrl)) {
    return GetStringAttribute(ax::mojom::StringAttribute::kUrl);
  }
  return "";
}

float AXPlatformNodeOHOS::GetOpacity() const {
  //If the color cannot be obtained, it will be opaque.
  if (!HasIntAttribute(ax::mojom::IntAttribute::kColor)) {
    return 1.0f;
  }

  // Extract alpha channel and convert to float
  int color = GetIntAttribute(ax::mojom::IntAttribute::kColor);
  return SkColor4f::FromColor(color).fA;
}

bool AXPlatformNodeOHOS::ShouldExposeValueAsName() const {
  switch (GetRole()) {
    case ax::mojom::Role::kDate:
    case ax::mojom::Role::kDateTime:
    case ax::mojom::Role::kInputTime:
      return true;
    case ax::mojom::Role::kColorWell:
      return false;
    default:
      break;
  }

  if (GetData().IsRangeValueSupported()) {
    return false;
  }

  if (IsTextField()) {
    return true;
  }

  if (GetRole() == ax::mojom::Role::kPopUpButton &&
      !GetValueForControl().empty()) {
    return true;
  }

  return false;
}

gfx::NativeViewAccessible AXPlatformNodeOHOS::GetNativeViewAccessible() {
  return GetOrCreateAccessibilityNodeInfo();
}

gfx::NativeViewAccessible
AXPlatformNodeOHOS::GetOrCreateAccessibilityNodeInfo() {
  return this;
}

}  // namespace ui