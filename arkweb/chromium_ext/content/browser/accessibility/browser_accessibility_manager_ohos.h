// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_
#define CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_

#include <unordered_set>

#include "absl/types/optional.h"
#include "browser_accessibility_ohos.h"
#include "content/common/content_export.h"
#include "ohos_nweb/src/cef_delegate/nweb_accessibility_utils.h"
#include "ui/accessibility/platform/ax_platform_tree_manager_delegate.h"
#include "ui/accessibility/platform/browser_accessibility.h"
#include "ui/accessibility/platform/browser_accessibility_manager.h"
#include "ui/accessibility/platform/one_shot_accessibility_tree_search.h"

#if false
#include "content/browser/renderer_host/render_frame_host_impl.h"
#endif
namespace ui {

class BrowserAccessibility;
class BrowserAccessibilityManagerOHOS;
class BrowserAccessibilityOHOS;
class WebContentsImpl;

class AccessibilityEventDispatcher {
public:
    explicit AccessibilityEventDispatcher(const std::unordered_map<int32_t, int32_t> &eventThrottleDelays,
                                          const std::unordered_set<int32_t> &viewIndependentEvents,
                                          BrowserAccessibilityManagerOHOS *manager)
        : eventThrottleDelays_(std::move(eventThrottleDelays)),
          viewIndependentEvents_(std::move(viewIndependentEvents)),
          manager_(manager)
    {
    }
    void EnqueueEvent(int64_t accessibilityId, int32_t eventType);

private:
    int64_t Uuid(int64_t accessibilityId, int32_t eventType);
    void RunTask(int64_t accessibilityId, int32_t eventType, int64_t uuid);

    std::unordered_map<int32_t, int32_t> eventThrottleDelays_;
    std::unordered_set<int32_t> viewIndependentEvents_;
    BrowserAccessibilityManagerOHOS *manager_;
    std::unordered_map<int64_t, int64_t> eventLastFiredTimes_;
    std::unordered_map<int64_t, base::DelayedTaskHandle> pendingEvents_;
};

// Manages a tree of BrowserAccessibility objects.
class CONTENT_EXPORT BrowserAccessibilityManagerOHOS
    : public BrowserAccessibilityManager {
 public:
  static BrowserAccessibilityManager* Create(
      const AXTreeUpdate& initial_tree,
      AXNodeIdDelegate& node_id_delegate,
      AXPlatformTreeManagerDelegate* delegate);

  static BrowserAccessibilityManager* Create(
      AXNodeIdDelegate& node_id_delegate,
      AXPlatformTreeManagerDelegate* delegate);

  BrowserAccessibilityManagerOHOS(const AXTreeUpdate& initial_tree,
                                  AXNodeIdDelegate& node_id_delegate,
                                  AXPlatformTreeManagerDelegate* delegate);

  ~BrowserAccessibilityManagerOHOS() = default;

     void FireAriaNotificationEvent(
      BrowserAccessibility* node,
      const std::string& announcement,
      const std::string& notification_id,
      ax::mojom::AriaNotificationInterrupt interrupt_property,
      ax::mojom::AriaNotificationPriority priority_property) override {}

  void FireFocusEvent(AXNode* node) override;

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

  bool DispatchEvent(int64_t accessibilityId,
                     int32_t eventType,
                     const std::string& argument = "");

  void OnHoverEvent(const gfx::PointF& point);

  void FireGeneratedEvent(AXEventGenerator::Event event_type,
                          const AXNode* node) override;
  void Copy();
  void Paste();
  void Cut();

  int64_t GetRootAccessibilityId() const;

  bool JumpToElementType(int64_t accessibility_id,
                         const std::string& element_type,
                         bool forwards,
                         bool can_wrap);

  AXNode* RetargetForEvents(AXNode* node,
                                RetargetEventType type) const override;

  void ScrollToMakeNodeVisible(int64_t accessibility_id);

  int64_t FindElementType(int64_t start_id,
                          const std::string& element_type,
                          bool forwards,
                          bool can_wrap,
                          bool use_default_predicate);

  void FireLocationChanged(BrowserAccessibility* node);

  void SendLocationChangeEvents(
       const std::vector<AXLocationChange>& changes) override;

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
      AXTree* tree,
      bool root_changed,
      const std::vector<AXTreeObserver::Change>& changes) override;

  void HandleNavigate(int64_t newRootId);

  void InitializeAccessibilityEventDispatcher();

  int64_t lastHoverId_ = -1;

  int64_t accessibilityFocusId_ = -1;

  int32_t content_changed_events_ = 0;

  std::unique_ptr<AccessibilityEventDispatcher> eventDispatcher_ = nullptr;
};
}  // namespace ui

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_
