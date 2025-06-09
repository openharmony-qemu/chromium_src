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
