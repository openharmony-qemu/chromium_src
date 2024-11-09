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
#include "ohos_nweb/include/nweb_accessibility_event_callback.h"
#include "ohos_nweb/src/cef_delegate/nweb_accessibility_utils.h"

namespace content {
// Manages a tree of BrowserAccessibility objects.
class WebContentsImpl;
class BrowserAccessibilityOHOS;
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

  void RegisterAccessibilityEventListener(
      std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback>
          accessibility_event_listener);

  std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback>
      GetAccessibilityEventListener() const;

  void SetLastHoverId(int64_t id) { lastHoverId_ = id; }

  int64_t GetLastHoverId() const { return lastHoverId_; }

  void SetAccessibilityFocusId(int64_t id) { accessibilityFocusId_ = id; }

  int64_t GetAccessibilityFocusId() const { return accessibilityFocusId_; }

  void MoveAccessibilityFocus(int64_t oldId, int64_t newId) const;

  bool MoveAccessibilityFocusToId(int64_t newAccessibilityFocusId);

  void SendAccessibilityEvent(int64_t accessibilityId,
                              OHOS::NWeb::AccessibilityEventType eventType);

  void OnHoverEvent(const gfx::PointF& point);

  void FireGeneratedEvent(ui::AXEventGenerator::Event event_type,
                          const ui::AXNode* node) override;
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
                         int64_t intervalMs, const int64_t& accessibilityId);

  int64_t lastHoverId_ = -1;

  int64_t accessibilityFocusId_ = -1;

  std::map<int64_t, int64_t> lastScrollEventFiredTimes_;
  std::map<int64_t, int64_t> lastStateUpdateEventFiredTimes_;
  std::map<int64_t, int64_t> lastContentUpdateEventFiredTimes_;
  std::map<int64_t, int64_t> lastHoverEnterEventFiredTimes_;

  std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback>
      accessibilityEventListener_;
};
}  // namespace content

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_