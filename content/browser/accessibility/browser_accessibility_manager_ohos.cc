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

#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"

#include "base/logging.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#include "ui/gfx/geometry/point_conversions.h"

namespace ui {

// static
BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    const AXTreeUpdate& initial_tree,
    AXNodeIdDelegate& node_id_delegate,
    AXPlatformTreeManagerDelegate* delegate) {
  return new content::BrowserAccessibilityManagerOHOS(initial_tree, node_id_delegate,
                                             delegate);
}

// static
BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    AXNodeIdDelegate& node_id_delegate,
    AXPlatformTreeManagerDelegate* delegate) {
  return new content::BrowserAccessibilityManagerOHOS(
      BrowserAccessibilityManager::GetEmptyDocument(), node_id_delegate,
      delegate);
}

}  // namespace ui

namespace content {
const int64_t kInvalidAccessibilityId = -1;
const int64_t kRootAccessibilityId = 0;
std::function<int64_t()> g_accessibility_id_generator;

BrowserAccessibilityManagerOHOS::BrowserAccessibilityManagerOHOS(
    const ui::AXTreeUpdate& initial_tree,
    ui::AXNodeIdDelegate& node_id_delegate,
    ui::AXPlatformTreeManagerDelegate* delegate)
    : BrowserAccessibilityManager(node_id_delegate, delegate) {
  Initialize(initial_tree);
}

void BrowserAccessibilityManagerOHOS::HandleFocusChanged(
    int64_t accessibility_id) {
  SendAccessibilityEvent(accessibility_id, AccessibilityEventType::FOCUS);
  MoveAccessibilityFocusToId(accessibility_id);
}

void BrowserAccessibilityManagerOHOS::RegisterAccessibilityIdGenerator(
    std::function<int64_t()> accessibility_id_generator) {
  if (g_accessibility_id_generator == nullptr) {
    g_accessibility_id_generator = accessibility_id_generator;
  }
}

int64_t BrowserAccessibilityManagerOHOS::GenerateAccessibilityId() {
  if (g_accessibility_id_generator != nullptr) {
    return g_accessibility_id_generator();
  }
  return kInvalidAccessibilityId;
}

void BrowserAccessibilityManagerOHOS::FireFocusEvent(ui::AXNode* node) {
  ui::AXTreeManager::FireFocusEvent(node);
  BrowserAccessibilityOHOS* node_ohos =
      static_cast<BrowserAccessibilityOHOS*>(GetFromAXNode(node));
  if (!node_ohos) {
    return;
  }
  HandleFocusChanged(TranslateAccessibilityId(node_ohos->GetAccessibilityId()));
}

void BrowserAccessibilityManagerOHOS::FireBlinkEvent(
    ax::mojom::Event event_type,
    ui::BrowserAccessibility* node,
    int action_request_id) {
  BrowserAccessibilityOHOS* node_ohos =
      static_cast<BrowserAccessibilityOHOS*>(node);
  if (!node_ohos) {
    return;
  }
  int64_t accessibility_id =
      TranslateAccessibilityId(node_ohos->GetAccessibilityId());
  switch (event_type) {
    case ax::mojom::Event::kClicked:
      SendAccessibilityEvent(accessibility_id, AccessibilityEventType::CLICK);
      break;
    case ax::mojom::Event::kBlur:
      SendAccessibilityEvent(accessibility_id, AccessibilityEventType::BLUR);
      break;
    case ax::mojom::Event::kHover:
      HandleHover(accessibility_id);
      break;
    default:
      break;
  }
}

void BrowserAccessibilityManagerOHOS::RegisterAccessibilityEventListener(
    std::shared_ptr<NWebAccessibilityEventCallback>
        accessibility_event_listener) {
  accessibility_event_listener_ = accessibility_event_listener;
}

bool BrowserAccessibilityManagerOHOS::MoveAccessibilityFocusToId(
    int64_t new_accessibility_focus_id) {
  if (accessibility_focus_id_ == new_accessibility_focus_id) {
    return false;
  }
  MoveAccessibilityFocus(accessibility_focus_id_, new_accessibility_focus_id);
  SendAccessibilityEvent(accessibility_focus_id_,
      AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
  accessibility_focus_id_ = new_accessibility_focus_id;
  SendAccessibilityEvent(accessibility_focus_id_,
      AccessibilityEventType::ACCESSIBILITY_FOCUSED);
  return true;
}

void BrowserAccessibilityManagerOHOS::MoveAccessibilityFocus(
    int64_t old_id,
    int64_t new_id) const {
  auto old_node = BrowserAccessibilityOHOS::GetFromAccessibilityId(old_id);
  if (old_node && old_node->manager()) {
    old_node->manager()->ClearAccessibilityFocus(*old_node);
  }

  auto node = BrowserAccessibilityOHOS::GetFromAccessibilityId(new_id);
  if (!node) {
    return;
  }
  node->manager()->SetAccessibilityFocus(*node);
}

void BrowserAccessibilityManagerOHOS::SendAccessibilityEvent(
    int64_t accessibility_id,
    AccessibilityEventType event_type) {
  accessibility_id = TranslateAccessibilityId(accessibility_id);
  if (accessibility_event_listener_ != nullptr &&
      event_type != AccessibilityEventType::UNKNOWN &&
      accessibility_id != kInvalidAccessibilityId) {
    accessibility_event_listener_->OnAccessibilityEvent(
        accessibility_id, static_cast<int64_t>(event_type));
  }

  if (event_type == AccessibilityEventType::HOVER_ENTER_EVENT) {
    auto* last_hover_node =
        BrowserAccessibilityOHOS::GetFromAccessibilityId(last_hover_id_);
    if (last_hover_node) {
      SendAccessibilityEvent(last_hover_id_,
          AccessibilityEventType::HOVER_EXIT_EVENT);
    }
    last_hover_id_ = accessibility_id;
  }
}

void BrowserAccessibilityManagerOHOS::HandleHover(int64_t accessibility_id) {
  if (last_hover_id_ == accessibility_id) {
    return;
  }

  SendAccessibilityEvent(accessibility_id,
      AccessibilityEventType::HOVER_ENTER_EVENT);
  MoveAccessibilityFocusToIdAndRefocusIfNeeded(accessibility_id);
}

void BrowserAccessibilityManagerOHOS::HandleEditableTextChanged(
    int64_t accessibility_id) {
  SendAccessibilityEvent(accessibility_id, AccessibilityEventType::TEXT_CHANGE);
}

void BrowserAccessibilityManagerOHOS::
    MoveAccessibilityFocusToIdAndRefocusIfNeeded(
    int64_t new_accessibility_focus_id) {
  if (new_accessibility_focus_id == accessibility_focus_id_) {
    SendAccessibilityEvent(new_accessibility_focus_id,
        AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
    accessibility_focus_id_ = kRootAccessibilityId;
  }
  MoveAccessibilityFocusToId(new_accessibility_focus_id);
}

void BrowserAccessibilityManagerOHOS::OnHoverEvent(const gfx::PointF& point) {
  auto* hover_node = static_cast<BrowserAccessibilityOHOS*>(
      ApproximateHitTest(gfx::ToFlooredPoint(point)));

  if (hover_node) {
    HandleHover(TranslateAccessibilityId(hover_node->GetAccessibilityId()));
  } else {
    HandleHover(kRootAccessibilityId);
  }
}

int64_t BrowserAccessibilityManagerOHOS::TranslateAccessibilityId(
    int64_t accessibility_id) const {
  if (accessibility_id != kRootAccessibilityId) {
    auto root =
        static_cast<BrowserAccessibilityOHOS*>(GetBrowserAccessibilityRoot());
    if (root && accessibility_id == root->GetAccessibilityId()) {
      return kRootAccessibilityId;
    }
  }
  return accessibility_id;
}

void BrowserAccessibilityManagerOHOS::FireGeneratedEvent(
    ui::AXEventGenerator::Event event_type,
    const ui::AXNode* node) {
  ui::AXTreeManager::FireGeneratedEvent(event_type, node);

  ui::BrowserAccessibility* wrapper = GetFromAXNode(node);
  BrowserAccessibilityOHOS* node_ohos =
      static_cast<BrowserAccessibilityOHOS*>(wrapper);
  if (!node_ohos) {
    return;
  }

  int64_t accessibility_id =
      TranslateAccessibilityId(node_ohos->GetAccessibilityId());

  switch (event_type) {
    case ui::AXEventGenerator::Event::VALUE_IN_TEXT_FIELD_CHANGED:
      if (node_ohos->IsTextField() && GetFocus() == wrapper) {
        HandleEditableTextChanged(accessibility_id);
      }
      break;
    default:
      break;
  }
}

}  // namespace content
