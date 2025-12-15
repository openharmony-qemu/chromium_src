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

#include "ui/accessibility/platform/ax_platform_node_ohos.h"

#include <codecvt>
#include <string>

#include "ohos/adapter/accessibility/accessibility_adapter.h"
#include "third_party/skia/include/core/SkColor.h"

namespace ui {

// static
AXPlatformNode* AXPlatformNode::Create(AXPlatformNodeDelegate* delegate) {
  AXPlatformNode* node = new AXPlatformNodeOHOS();
  node->Init(delegate);
  return node;
}

// static
AXPlatformNode* AXPlatformNode::FromNativeViewAccessible(
    gfx::NativeViewAccessible accessible) {
  return static_cast<AXPlatformNodeOHOS*>(accessible);
}

AXPlatformNodeOHOS::AXPlatformNodeOHOS() = default;

AXPlatformNodeOHOS::~AXPlatformNodeOHOS() = default;

void AXPlatformNodeOHOS::Init(AXPlatformNodeDelegate* delegate) {
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
