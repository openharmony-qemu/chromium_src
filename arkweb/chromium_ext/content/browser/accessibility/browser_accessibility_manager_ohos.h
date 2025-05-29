// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_
#define CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_

#include "absl/types/optional.h"
#include "browser_accessibility_ohos.h"
#include "content/common/content_export.h"
#include "ohos_nweb/src/cef_delegate/nweb_accessibility_utils.h"
#include "ui/accessibility/platform/ax_platform_tree_manager_delegate.h"
#include "ui/accessibility/platform/browser_accessibility.h"
#include "ui/accessibility/platform/browser_accessibility_manager.h"
#if false
#include "content/browser/renderer_host/render_frame_host_impl.h"
#endif

namespace ui {
// Manages a tree of BrowserAccessibility objects.
class WebContentsImpl;
class BrowserAccessibilityOHOS;
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
                              OHOS::NWeb::AccessibilityEventType eventType);

  void OnHoverEvent(const gfx::PointF& point);

  void FireGeneratedEvent(AXEventGenerator::Event event_type,
                          const AXNode* node) override;

  void Copy();
  void Paste();
  void Cut();

 private:
  void HandleHover(int64_t accessibilityId);

  void MoveAccessibilityFocusToIdAndRefocusIfNeeded(
      int64_t newAccessibilityFocusId);

  void HandleFocusChanged(int64_t accessibilityId);

  void HandleEditableTextChanged(int64_t accessibilityId);

  void HandleContentChanged(int64_t accessibilityId);

  int64_t GetRootAccessibilityId() const;

  bool IsIgnoredEvent(std::map<int64_t, int64_t>& lastEventFiredTimes,
                      const int64_t& accessibilityId);

  bool IsFrequentlyEvent(std::map<int64_t, int64_t>& lastEventFiredTimes,
                         int64_t intervalMs,
                         const int64_t& accessibilityId);

  int64_t lastHoverId_ = -1;

  int64_t accessibilityFocusId_ = -1;

  std::map<int64_t, int64_t> lastScrollEventFiredTimes_;
  std::map<int64_t, int64_t> lastContentUpdateEventFiredTimes_;
  std::map<int64_t, int64_t> lastHoverEnterEventFiredTimes_;
};
}  // namespace ui

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_
