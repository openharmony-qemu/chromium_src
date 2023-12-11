// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#include "ui/gfx/geometry/point_conversions.h"

namespace content {
const int32_t kInvalidAccessibilityId = -1;
const int32_t kRootAccessibilityId = 0;
std::function<int32_t()> g_accessibility_id_generator;

BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    const ui::AXTreeUpdate& initial_tree,
    BrowserAccessibilityDelegate* delegate) {
  return new BrowserAccessibilityManagerOHOS(initial_tree, delegate);
}

// static
BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    BrowserAccessibilityDelegate* delegate) {
  return new BrowserAccessibilityManagerOHOS(
      BrowserAccessibilityManager::GetEmptyDocument(), delegate);
}

void BrowserAccessibilityManagerOHOS::HandleFocusChanged(
    int32_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::FOCUS);
  MoveAccessibilityFocusToId(accessibilityId);
}

void BrowserAccessibilityManagerOHOS::RegisterAccessibilityIdGenerator(
    std::function<int32_t()> accessibilityIdGenerator) {
  if (g_accessibility_id_generator == nullptr) {
    g_accessibility_id_generator = accessibilityIdGenerator;
  }
}

int32_t BrowserAccessibilityManagerOHOS::GenerateAccessibilityId() {
  if (g_accessibility_id_generator != nullptr) {
    return g_accessibility_id_generator();
  }
  return kInvalidAccessibilityId;
}

void BrowserAccessibilityManagerOHOS::FireFocusEvent(
    BrowserAccessibility* node) {
  BrowserAccessibilityManager::FireFocusEvent(node);
  BrowserAccessibilityOHOS* nodeOHOS =
      static_cast<BrowserAccessibilityOHOS*>(node);
  if (!nodeOHOS) {
    return;
  }
  HandleFocusChanged(TranslateAccessibilityId(nodeOHOS->GetAccessibilityId()));
}

void BrowserAccessibilityManagerOHOS::FireBlinkEvent(
    ax::mojom::Event event_type,
    BrowserAccessibility* node,
    int action_request_id) {
  BrowserAccessibilityOHOS* nodeOHOS =
      static_cast<BrowserAccessibilityOHOS*>(node);
  if (!nodeOHOS) {
    return;
  }
  switch (event_type) {
    case ax::mojom::Event::kClicked:
      SendAccessibilityEvent(nodeOHOS->GetAccessibilityId(),
                             OHOS::NWeb::AccessibilityEventType::CLICK);
      break;
    case ax::mojom::Event::kHover:
      HandleHover(nodeOHOS->GetAccessibilityId());
      return;
    case ax::mojom::Event::kTextChanged:
      SendAccessibilityEvent(nodeOHOS->GetAccessibilityId(),
                             OHOS::NWeb::AccessibilityEventType::TEXT_CHANGE);
      break;
    default:
      break;
  }
}

void BrowserAccessibilityManagerOHOS::RegisterAccessibilityEventListener(
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback>
        accessibilityEventListener) {
  accessibilityEventListener_ = accessibilityEventListener;
}

bool BrowserAccessibilityManagerOHOS::MoveAccessibilityFocusToId(
    int32_t newAccessibilityFocusId) {
  if (accessibilityFocusId_ == newAccessibilityFocusId) {
    return false;
  }
  MoveAccessibilityFocus(accessibilityFocusId_, newAccessibilityFocusId);
  accessibilityFocusId_ = newAccessibilityFocusId;
  SendAccessibilityEvent(
      accessibilityFocusId_,
      OHOS::NWeb::AccessibilityEventType::ACCESSIBILITY_FOCUSED);
  return true;
}

void BrowserAccessibilityManagerOHOS::MoveAccessibilityFocus(
    int32_t oldId,
    int32_t newId) const {
  auto oldNode = BrowserAccessibilityOHOS::GetFromAccessibilityId(oldId);
  if (oldNode && oldNode->manager())
    oldNode->manager()->ClearAccessibilityFocus(*oldNode);

  auto node = BrowserAccessibilityOHOS::GetFromAccessibilityId(newId);
  if (!node)
    return;
  node->manager()->SetAccessibilityFocus(*node);
}

void BrowserAccessibilityManagerOHOS::SendAccessibilityEvent(
    int32_t accessibilityId,
    OHOS::NWeb::AccessibilityEventType eventType) {
  accessibilityId = TranslateAccessibilityId(accessibilityId);

  if (accessibilityEventListener_ != nullptr &&
      eventType != OHOS::NWeb::AccessibilityEventType::UNKNOWN) {
    accessibilityEventListener_->OnAccessibilityEvent(
        accessibilityId, static_cast<int32_t>(eventType));
  }

  if (eventType == OHOS::NWeb::AccessibilityEventType::HOVER_ENTER_EVENT) {
    auto* lastHoverNode =
        BrowserAccessibilityOHOS::GetFromAccessibilityId(lastHoverId_);
    if (lastHoverNode) {
      SendAccessibilityEvent(
          lastHoverId_, OHOS::NWeb::AccessibilityEventType::HOVER_EXIT_EVENT);
    }
    lastHoverId_ = accessibilityId;
  }
}

void BrowserAccessibilityManagerOHOS::HandleHover(int32_t id) {
  if (lastHoverId_ == id) {
    return;
  }

  SendAccessibilityEvent(id,
                         OHOS::NWeb::AccessibilityEventType::HOVER_ENTER_EVENT);
  MoveAccessibilityFocusToIdAndRefocusIfNeeded(id);
}

void BrowserAccessibilityManagerOHOS::
    MoveAccessibilityFocusToIdAndRefocusIfNeeded(
        int32_t newAccessibilityFocusId) {
  if (newAccessibilityFocusId == accessibilityFocusId_) {
    SendAccessibilityEvent(
        newAccessibilityFocusId,
        OHOS::NWeb::AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
    accessibilityFocusId_ = kRootAccessibilityId;
  }
  MoveAccessibilityFocusToId(newAccessibilityFocusId);
}

void BrowserAccessibilityManagerOHOS::OnHoverEvent(const gfx::PointF& point) {
  auto* hoverNode = static_cast<BrowserAccessibilityOHOS*>(
      ApproximateHitTest(gfx::ToFlooredPoint(point)));

  if (hoverNode) {
    HandleHover(TranslateAccessibilityId(hoverNode->GetAccessibilityId()));
  } else {
    HandleHover(kRootAccessibilityId);
  }
}

int32_t BrowserAccessibilityManagerOHOS::TranslateAccessibilityId(
    int32_t accessibilityId) const {
  if (accessibilityId != kRootAccessibilityId) {
    auto root = static_cast<BrowserAccessibilityOHOS*>(GetRoot());
    if (root && accessibilityId == root->GetAccessibilityId()) {
      return kRootAccessibilityId;
    }
  }
  return accessibilityId;
}
}  // namespace content
