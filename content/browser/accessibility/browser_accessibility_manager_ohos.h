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

#ifndef CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_
#define CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_

#include "content/browser/accessibility/browser_accessibility_manager.h"
#include "content/browser/accessibility/one_shot_accessibility_tree_search.h"
#include "ohos_nweb/src/cef_delegate/nweb_accessibility_utils.h"

namespace content {

class BrowserAccessibility;
class BrowserAccessibilityManagerOHOS;
class BrowserAccessibilityOHOS;
class WebContentsImpl;

class AccessibilityEventDispatcher {
 public:
  explicit AccessibilityEventDispatcher(
      const std::unordered_map<int32_t, int32_t>& eventThrottleDelays,
      const std::unordered_set<int32_t>& viewIndependentEvents,
      BrowserAccessibilityManagerOHOS* manager)
      : eventThrottleDelays_(std::move(eventThrottleDelays)),
        viewIndependentEvents_(std::move(viewIndependentEvents)),
        manager_(manager) {}
  void EnqueueEvent(int64_t accessibilityId, int32_t eventType);

 private:
  int64_t Uuid(int64_t accessibilityId, int32_t eventType);
  void RunTask(int64_t accessibilityId, int32_t eventType, int64_t uuid);

  std::unordered_map<int32_t, int32_t> eventThrottleDelays_;
  std::unordered_set<int32_t> viewIndependentEvents_;
  BrowserAccessibilityManagerOHOS* manager_;
  std::unordered_map<int64_t, int64_t> eventLastFiredTimes_;
  std::unordered_map<int64_t, base::DelayedTaskHandle> pendingEvents_;
};

// Manages a tree of BrowserAccessibility objects.
class CONTENT_EXPORT BrowserAccessibilityManagerOHOS
    : public BrowserAccessibilityManager {
 public:
  static BrowserAccessibilityManager* Create(
      const ui::AXTreeUpdate& initial_tree,
      WebAXPlatformTreeManagerDelegate* delegate);

  static BrowserAccessibilityManager* Create(
      WebAXPlatformTreeManagerDelegate* delegate);

  BrowserAccessibilityManagerOHOS(const ui::AXTreeUpdate& initial_tree,
                                  WebAXPlatformTreeManagerDelegate* delegate);

  ~BrowserAccessibilityManagerOHOS() = default;

  void FireFocusEvent(ui::AXNode* node) override;

  void FireBlinkEvent(ax::mojom::Event event_type,
                      BrowserAccessibility* node,
                      int action_request_id) override;

  void SetLastHoverId(int64_t id) { lastHoverId_ = id; }

  int64_t GetLastHoverId() const { return lastHoverId_; }

  void SetAccessibilityFocusId(int64_t id) { accessibilityFocusId_ = id; }

  int64_t GetAccessibilityFocusId() const { return accessibilityFocusId_; }

  void MoveAccessibilityFocus(int64_t oldId, int64_t newId) const;

  bool MoveAccessibilityFocusToId(int64_t newAccessibilityFocusId);

  void SendAccessibilityEvent(int64_t accessibilityId,
                              OHOS::NWeb::AccessibilityEventType eventType,
                              const std::string& argument = "");

  bool DispatchEvent(int64_t accessibilityId, int32_t eventType);

  void OnHoverEvent(const gfx::PointF& point);

  void FireGeneratedEvent(ui::AXEventGenerator::Event event_type,
                          const ui::AXNode* node) override;
  void Copy();
  void Paste();
  void Cut();

  int64_t GetRootAccessibilityId() const;

  bool JumpToElementType(int64_t accessibility_id,
                         const std::string& element_type,
                         bool forwards,
                         bool can_wrap);

  ui::AXNode* RetargetForEvents(ui::AXNode* node,
                                RetargetEventType type) const override;

  void ScrollToMakeNodeVisible(int64_t accessibility_id);

  int64_t FindElementType(int64_t start_id,
                          const std::string& element_type,
                          bool forwards,
                          bool can_wrap,
                          bool use_default_predicate);

  void FireLocationChanged(BrowserAccessibility* node);

  void SendLocationChangeEvents(
      const std::vector<blink::mojom::LocationChangesPtr>& changes) override;

 private:
  void HandleHover(int64_t accessibilityId);

  void MoveAccessibilityFocusToIdAndRefocusIfNeeded(
      int64_t newAccessibilityFocusId);

  void HandleFocusChanged(int64_t accessibilityId);

  void HandleEditableTextChanged(int64_t accessibilityId);

  void HandleContentChanged(int64_t accessibilityId);

  void HandleScrollPositionChanged(int64_t accessibilityId);

  void HandleDialogModalOpened(int64_t accessibiltyId);

  void SendDelayedWindowContentChangedEvent();

  void InitSearchKeyToPredicateMapIfNeeded();

  void AddToPredicateMap(const char* search_key_ascii,
                       AccessibilityMatchPredicate predicate);
  
  AccessibilityMatchPredicate PredicateForSearchKey(const std::u16string& element_type);

  static bool AllInterestingNodesPredicate(BrowserAccessibility* start,
    BrowserAccessibility* node);

  // AXTreeObserver overrides.
  void OnAtomicUpdateFinished(
      ui::AXTree* tree,
      bool root_changed,
      const std::vector<ui::AXTreeObserver::Change>& changes) override;

  void HandleNavigate(int64_t newRootId);

  void InitializeAccessibilityEventDispatcher();

  void ClearAccessibilityFocus();

  void DecideAccessibilityFocus(int64_t accessibilityId);

  int64_t lastHoverId_ = -1;

  int64_t accessibilityFocusId_ = -1;

  int32_t content_changed_events_ = 0;

  std::unique_ptr<AccessibilityEventDispatcher> eventDispatcher_ = nullptr;
};
}  // namespace content

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_