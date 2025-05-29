// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser_accessibility_manager_ohos.h"

#include "base/logging.h"
#include "browser_accessibility_ohos.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/browser_thread.h"
#include "ui/accessibility/ax_selection.h"
#include "ui/gfx/geometry/point_conversions.h"

namespace ui {
const int64_t kInvalidAccessibilityId = -1;
constexpr int64_t kDefaultUpdateEventDelayMs = 100;
constexpr int64_t kDefaultFrequentHoverEnterEventDelayMs = 300;

BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    const AXTreeUpdate& initial_tree,
    AXNodeIdDelegate& node_id_delegate,
    AXPlatformTreeManagerDelegate* delegate) {
  return new BrowserAccessibilityManagerOHOS(initial_tree, node_id_delegate,
                                             delegate);
}

// static
BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    AXNodeIdDelegate& node_id_delegate,
    AXPlatformTreeManagerDelegate* delegate) {
  return new BrowserAccessibilityManagerOHOS(
      BrowserAccessibilityManager::GetEmptyDocument(), node_id_delegate,
      delegate);
}

BrowserAccessibilityManagerOHOS::BrowserAccessibilityManagerOHOS(
    const AXTreeUpdate& initial_tree,
    AXNodeIdDelegate& node_id_delegate,
    AXPlatformTreeManagerDelegate* delegate)
    : BrowserAccessibilityManager(node_id_delegate, delegate) {
  Initialize(initial_tree);
}

void BrowserAccessibilityManagerOHOS::HandleFocusChanged(
    int64_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::FOCUS);
  if (accessibilityId != accessibilityFocusId_ &&
      accessibilityId != GetRootAccessibilityId()) {
    SendAccessibilityEvent(accessibilityId,
                           OHOS::NWeb::AccessibilityEventType::REQUEST_FOCUS);
  }
}

void BrowserAccessibilityManagerOHOS::FireFocusEvent(AXNode* node) {
  AXTreeManager::FireFocusEvent(node);
  BrowserAccessibilityOHOS* nodeOHOS =
      static_cast<BrowserAccessibilityOHOS*>(GetFromAXNode(node));
  if (!nodeOHOS) {
    return;
  }
  HandleFocusChanged(nodeOHOS->GetAccessibilityId());
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
  int64_t accessibilityId = nodeOHOS->GetAccessibilityId();
  switch (event_type) {
    case ax::mojom::Event::kClicked:
      SendAccessibilityEvent(accessibilityId,
                             OHOS::NWeb::AccessibilityEventType::CLICK);
      break;
    case ax::mojom::Event::kBlur:
      SendAccessibilityEvent(accessibilityId,
                             OHOS::NWeb::AccessibilityEventType::BLUR);
      break;
    case ax::mojom::Event::kHover:
      HandleHover(accessibilityId);
      break;
    case ax::mojom::Event::kTreeChanged:
    case ax::mojom::Event::kLocationChanged:
    case ax::mojom::Event::kLayoutComplete:
      if (GetRootAccessibilityId() == accessibilityId) {
        SendAccessibilityEvent(accessibilityId,
                               OHOS::NWeb::AccessibilityEventType::PAGE_CHANGE);
      }
      break;
    default:
      break;
  }
}

bool BrowserAccessibilityManagerOHOS::MoveAccessibilityFocusToId(
    int64_t newAccessibilityFocusId) {
  if (accessibilityFocusId_ == newAccessibilityFocusId) {
    return false;
  }
  MoveAccessibilityFocus(accessibilityFocusId_, newAccessibilityFocusId);
  SendAccessibilityEvent(
      accessibilityFocusId_,
      OHOS::NWeb::AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
  accessibilityFocusId_ = newAccessibilityFocusId;
  SendAccessibilityEvent(
      accessibilityFocusId_,
      OHOS::NWeb::AccessibilityEventType::ACCESSIBILITY_FOCUSED);
  return true;
}

void BrowserAccessibilityManagerOHOS::MoveAccessibilityFocus(
    int64_t oldId,
    int64_t newId) const {
  auto oldNode = BrowserAccessibilityOHOS::GetFromAccessibilityId(oldId);
  if (oldNode && oldNode->manager()) {
    oldNode->manager()->ClearAccessibilityFocus(*oldNode);
  }

  auto node = BrowserAccessibilityOHOS::GetFromAccessibilityId(newId);
  if (!node) {
    return;
  }
  node->manager()->SetAccessibilityFocus(*node);

  if (node != node->manager()->GetBrowserAccessibilityRoot()) {
    node->manager()->LoadInlineTextBoxes(*node);
  }
}

bool BrowserAccessibilityManagerOHOS::IsIgnoredEvent(
    std::map<int64_t, int64_t>& lastEventFiredTimes,
    const int64_t& accessibilityId) {
  auto lastEventFireTimeIter = lastEventFiredTimes.find(accessibilityId);
  auto now = std::chrono::system_clock::now();
  auto millis = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  auto timestamp = millis.time_since_epoch().count();
  if (lastEventFiredTimes.end() == lastEventFireTimeIter) {
    lastEventFiredTimes.insert(std::make_pair(accessibilityId, timestamp));
  } else {
    auto interval = std::abs(timestamp - lastEventFireTimeIter->second);
    if (interval <= kDefaultUpdateEventDelayMs) {
      return true;
    }
    lastEventFireTimeIter->second = timestamp;
  }
  return false;
}

void BrowserAccessibilityManagerOHOS::SendAccessibilityEvent(
    int64_t accessibilityId,
    OHOS::NWeb::AccessibilityEventType eventType) {
  if ((OHOS::NWeb::AccessibilityEventType::CHANGE == eventType &&
       IsIgnoredEvent(lastContentUpdateEventFiredTimes_, accessibilityId)) ||
      (OHOS::NWeb::AccessibilityEventType::SCROLL_END == eventType &&
       IsIgnoredEvent(lastScrollEventFiredTimes_, accessibilityId))) {
    return;
  }

  LOG(INFO) << "SendAccessibilityEvent accessibilityId is " << accessibilityId
            << ", eventType is " << static_cast<uint32_t>(eventType);

  if (eventType != OHOS::NWeb::AccessibilityEventType::UNKNOWN &&
      accessibilityId != kInvalidAccessibilityId && delegate_ != nullptr) {
    auto renderFrameHost = delegate_->AccessibilityRenderFrameHost();
    if (renderFrameHost != nullptr) {
      renderFrameHost->SendAccessibilityEvent(accessibilityId,
                                              static_cast<int32_t>(eventType));
    }
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

bool BrowserAccessibilityManagerOHOS::IsFrequentlyEvent(
    std::map<int64_t, int64_t>& lastEventFiredTimes,
    int64_t intervalMs,
    const int64_t& accessibilityId) {
  auto lastEnterTimeIter = lastEventFiredTimes.find(accessibilityId);
  auto now = std::chrono::system_clock::now();
  auto millis = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  auto timestamp = millis.time_since_epoch().count();
  if (lastEventFiredTimes.end() == lastEnterTimeIter) {
    lastEventFiredTimes.insert(std::make_pair(accessibilityId, timestamp));
  } else {
    auto interval = std::abs(timestamp - lastEnterTimeIter->second);
    lastEnterTimeIter->second = timestamp;
    if (interval <= intervalMs) {
      return true;
    }
  }
  return false;
}

void BrowserAccessibilityManagerOHOS::HandleHover(int64_t accessibilityId) {
  // Hover events with intervals less than
  // kDefaultFrequentHoverEnterEventDelayMs milliseconds will be filtered out.
  if (IsFrequentlyEvent(lastHoverEnterEventFiredTimes_,
                        kDefaultFrequentHoverEnterEventDelayMs,
                        accessibilityId)) {
    LOG(INFO) << "skip send hover enter event accessibilityId is "
              << accessibilityId;
    return;
  }
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::HOVER_ENTER_EVENT);
}

void BrowserAccessibilityManagerOHOS::HandleEditableTextChanged(
    int64_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::TEXT_CHANGE);
}

void BrowserAccessibilityManagerOHOS::HandleContentChanged(
    int64_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::CHANGE);
}

void BrowserAccessibilityManagerOHOS::
    MoveAccessibilityFocusToIdAndRefocusIfNeeded(
        int64_t newAccessibilityFocusId) {
  if (newAccessibilityFocusId == accessibilityFocusId_) {
    SendAccessibilityEvent(
        newAccessibilityFocusId,
        OHOS::NWeb::AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
    accessibilityFocusId_ = GetRootAccessibilityId();
  }
  MoveAccessibilityFocusToId(newAccessibilityFocusId);
}

void BrowserAccessibilityManagerOHOS::OnHoverEvent(const gfx::PointF& point) {
  auto* hoverNode = static_cast<BrowserAccessibilityOHOS*>(
      ApproximateHitTest(gfx::ToFlooredPoint(point)));

  if (hoverNode) {
    HandleHover(hoverNode->GetAccessibilityId());
  } else {
    HandleHover(GetRootAccessibilityId());
  }
}

int64_t BrowserAccessibilityManagerOHOS::GetRootAccessibilityId() const {
  auto root =
      static_cast<BrowserAccessibilityOHOS*>(GetBrowserAccessibilityRoot());
  if (root) {
    return root->GetAccessibilityId();
  }
  return kInvalidAccessibilityId;
}

void BrowserAccessibilityManagerOHOS::FireGeneratedEvent(
    AXEventGenerator::Event event_type,
    const AXNode* node) {
  AXTreeManager::FireGeneratedEvent(event_type, node);

  BrowserAccessibility* wrapper = GetFromAXNode(node);
  BrowserAccessibilityOHOS* nodeOHOS =
      static_cast<BrowserAccessibilityOHOS*>(wrapper);
  if (!nodeOHOS) {
    return;
  }

  int64_t accessibilityId = nodeOHOS->GetAccessibilityId();

  if (event_type != AXEventGenerator::Event::SUBTREE_CREATED) {
    HandleContentChanged(GetRootAccessibilityId());
  }
  switch (event_type) {
    case AXEventGenerator::Event::VALUE_IN_TEXT_FIELD_CHANGED:
      if (nodeOHOS->IsTextField() && GetFocus() == wrapper) {
        HandleEditableTextChanged(accessibilityId);
      }
      break;
    case AXEventGenerator::Event::NAME_CHANGED:
      SendAccessibilityEvent(accessibilityId,
                             OHOS::NWeb::AccessibilityEventType::TEXT_CHANGE);
      break;
    case AXEventGenerator::Event::SCROLL_HORIZONTAL_POSITION_CHANGED:
    case AXEventGenerator::Event::SCROLL_VERTICAL_POSITION_CHANGED:
      if (GetRootAccessibilityId() == accessibilityId) {
        SendAccessibilityEvent(accessibilityId,
                               OHOS::NWeb::AccessibilityEventType::PAGE_CHANGE);
      }
      SendAccessibilityEvent(accessibilityId,
                             OHOS::NWeb::AccessibilityEventType::SCROLL_END);
      break;
    case AXEventGenerator::Event::SELECTED_CHANGED:
      if (nodeOHOS->IsSelected()) {
        SendAccessibilityEvent(accessibilityId,
                               OHOS::NWeb::AccessibilityEventType::SELECTED);
        if (accessibilityId != accessibilityFocusId_) {
          SendAccessibilityEvent(
              accessibilityId,
              OHOS::NWeb::AccessibilityEventType::REQUEST_FOCUS);
        }
      }
      break;
    case AXEventGenerator::Event::DOCUMENT_SELECTION_CHANGED: {
      if (ax_tree() == nullptr) {
        break;
      }
      AXNodeID focus_id = ax_tree()->GetUnignoredSelection().focus_object_id;
      BrowserAccessibility* focus_object = GetFromID(focus_id);
      if (focus_object) {
        BrowserAccessibilityOHOS* oh_focus_object =
            static_cast<BrowserAccessibilityOHOS*>(focus_object);
        if (oh_focus_object == nullptr) {
          break;
        }
        SendAccessibilityEvent(
            oh_focus_object->GetAccessibilityId(),
            OHOS::NWeb::AccessibilityEventType::TEXT_SELECTION_UPDATE);
      }
      break;
    }
    case AXEventGenerator::Event::LIVE_REGION_NODE_CHANGED: {
      SendAccessibilityEvent(accessibilityId,
                             OHOS::NWeb::AccessibilityEventType::SELECTED);
      break;
    }
    case AXEventGenerator::Event::SUBTREE_CREATED:
      if (GetRootAccessibilityId() == accessibilityId) {
        SendAccessibilityEvent(accessibilityId,
                               OHOS::NWeb::AccessibilityEventType::PAGE_OPEN);
      }
      break;
    default:
      break;
  }
}

void BrowserAccessibilityManagerOHOS::Copy() {}

void BrowserAccessibilityManagerOHOS::Paste() {}

void BrowserAccessibilityManagerOHOS::Cut() {}

}  // namespace ui
