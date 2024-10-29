/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "base/logging.h"
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/browser_thread.h"
#include "ui/accessibility/ax_selection.h"
#include "ui/gfx/geometry/point_conversions.h"

namespace content {
const int64_t kInvalidAccessibilityId = -1;
constexpr int64_t kDefaultUpdateEventDelayMs = 100;

BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    const ui::AXTreeUpdate& initial_tree,
    WebAXPlatformTreeManagerDelegate* delegate) {
  return new BrowserAccessibilityManagerOHOS(initial_tree, delegate);
}

// static
BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    WebAXPlatformTreeManagerDelegate* delegate) {
  return new BrowserAccessibilityManagerOHOS(
      BrowserAccessibilityManager::GetEmptyDocument(), delegate);
}

BrowserAccessibilityManagerOHOS::BrowserAccessibilityManagerOHOS(
    const ui::AXTreeUpdate& initial_tree,
    WebAXPlatformTreeManagerDelegate* delegate)
    : BrowserAccessibilityManager(delegate) {
  Initialize(initial_tree);
}

void BrowserAccessibilityManagerOHOS::HandleFocusChanged(
    int64_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::FOCUS);
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::REQUEST_FOCUS);
}

std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback>
    BrowserAccessibilityManagerOHOS::GetAccessibilityEventListener() const {
    return accessibilityEventListener_;
}

void BrowserAccessibilityManagerOHOS::FireFocusEvent(
    ui::AXNode* node) {
  ui::AXTreeManager::FireFocusEvent(node);
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
  if (oldNode && oldNode->manager())
    oldNode->manager()->ClearAccessibilityFocus(*oldNode);

  auto node = BrowserAccessibilityOHOS::GetFromAccessibilityId(newId);
  if (!node)
    return;
  node->manager()->SetAccessibilityFocus(*node);

  if (node != node->manager()->GetBrowserAccessibilityRoot())
    node->manager()->LoadInlineTextBoxes(*node);
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
      (OHOS::NWeb::AccessibilityEventType::PAGE_CHANGE == eventType &&
       IsIgnoredEvent(lastStateUpdateEventFiredTimes_, accessibilityId)) ||
      (OHOS::NWeb::AccessibilityEventType::SCROLL_END == eventType &&
       IsIgnoredEvent(lastScrollEventFiredTimes_, accessibilityId))) {
    return;
  }
 
  if (accessibilityEventListener_ == nullptr) {
    auto rootManager =
        static_cast<BrowserAccessibilityManagerOHOS*>(GetManagerForRootFrame());
    if (rootManager != nullptr) {
      accessibilityEventListener_ =
          rootManager->GetAccessibilityEventListener();
    }
  }

  LOG(INFO) << "SendAccessibilityEvent accessibilityId is " << accessibilityId
            << ", eventType is " << static_cast<uint32_t>(eventType)
            << ", listener_ is " << (accessibilityEventListener_ != nullptr);

  if (accessibilityEventListener_ != nullptr &&
      eventType != OHOS::NWeb::AccessibilityEventType::UNKNOWN &&
      accessibilityId != kInvalidAccessibilityId) {
    accessibilityEventListener_->OnAccessibilityEvent(
        accessibilityId, static_cast<uint32_t>(eventType));
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

void BrowserAccessibilityManagerOHOS::HandleHover(int64_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::HOVER_ENTER_EVENT);
}

void BrowserAccessibilityManagerOHOS::HandleEditableTextChanged(int64_t accessibilityId)
{
  SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::TEXT_CHANGE);
}

void BrowserAccessibilityManagerOHOS::HandleContentChanged(int64_t accessibilityId)
{
  SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::CHANGE);
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
  auto root = static_cast<BrowserAccessibilityOHOS*>(GetBrowserAccessibilityRoot());
  if (root) {
    return root->GetAccessibilityId();
  }
  return kInvalidAccessibilityId;
}

void BrowserAccessibilityManagerOHOS::FireGeneratedEvent(
    ui::AXEventGenerator::Event event_type,
    const ui::AXNode* node) {
  ui::AXTreeManager::FireGeneratedEvent(event_type, node);

  BrowserAccessibility* wrapper = GetFromAXNode(node);
  BrowserAccessibilityOHOS* nodeOHOS =
      static_cast<BrowserAccessibilityOHOS*>(wrapper);
  if (!nodeOHOS) {
    return;
  }

  int64_t accessibilityId = nodeOHOS->GetAccessibilityId();

  if (event_type != ui::AXEventGenerator::Event::SUBTREE_CREATED) {
    HandleContentChanged(GetRootAccessibilityId());
  }
  switch (event_type) {
    case ui::AXEventGenerator::Event::VALUE_IN_TEXT_FIELD_CHANGED:
      if (nodeOHOS->IsTextField() && GetFocus() == wrapper) {
        HandleEditableTextChanged(accessibilityId);
      }
      break;
    case ui::AXEventGenerator::Event::NAME_CHANGED:
      SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::TEXT_CHANGE);
      break;
    case ui::AXEventGenerator::Event::SCROLL_HORIZONTAL_POSITION_CHANGED:
    case ui::AXEventGenerator::Event::SCROLL_VERTICAL_POSITION_CHANGED:
      if (GetRootAccessibilityId() == accessibilityId) {
        SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::PAGE_CHANGE);
      } else {
        SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::SCROLL_END);
      }
      break;
    case ui::AXEventGenerator::Event::SELECTED_CHANGED:
      SendAccessibilityEvent(accessibilityId, 
          OHOS::NWeb::AccessibilityEventType::SELECTED);
      break;
    case ui::AXEventGenerator::Event::DOCUMENT_SELECTION_CHANGED: {
      if (ax_tree() == nullptr) {
        break;
      }
      ui::AXNodeID focus_id =
          ax_tree()->GetUnignoredSelection().focus_object_id;
      BrowserAccessibility* focus_object = GetFromID(focus_id);
      if (focus_object) {
        BrowserAccessibilityOHOS* oh_focus_object =
            static_cast<BrowserAccessibilityOHOS*>(focus_object);
        if (oh_focus_object == nullptr) {
          break;
        }
        SendAccessibilityEvent(oh_focus_object->GetAccessibilityId(),
            OHOS::NWeb::AccessibilityEventType::TEXT_SELECTION_UPDATE);
      }
      break;
    }
    default:
      break;
  }
}

void BrowserAccessibilityManagerOHOS::Copy()
{
  content::WebContentsImpl* web_contents_impl = 
      static_cast<content::WebContentsImpl*>(web_contents());
  if (web_contents_impl == nullptr) {
    return;
  }
  web_contents_impl->Copy();
}

void BrowserAccessibilityManagerOHOS::Paste()
{
  content::WebContentsImpl* web_contents_impl = 
      static_cast<content::WebContentsImpl*>(web_contents());
  if (web_contents_impl == nullptr) {
    return;
  }
  web_contents_impl->Paste();
}

void BrowserAccessibilityManagerOHOS::Cut()
{
  content::WebContentsImpl* web_contents_impl = 
      static_cast<content::WebContentsImpl*>(web_contents());
  if (web_contents_impl == nullptr) {
    return;
  }
  web_contents_impl->Cut();
}

}  // namespace content
