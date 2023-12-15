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

  static void RegisterAccessibilityIdGenerator(
      std::function<int32_t()> accessibilityIdGenerator);

  static int32_t GenerateAccessibilityId();

  void RegisterAccessibilityEventListener(
      std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback>
          accessibility_event_listener);

  void SetLastHoverId(int32_t id) { lastHoverId_ = id; }

  int32_t GetLastHoverId() const { return lastHoverId_; }

  void SetAccessibilityFocusId(int32_t id) { accessibilityFocusId_ = id; }

  int32_t GetAccessibilityFocusId() const { return accessibilityFocusId_; }

  void MoveAccessibilityFocus(int32_t oldId, int32_t newId) const;

  bool MoveAccessibilityFocusToId(int32_t newAccessibilityFocusId);

  void SendAccessibilityEvent(int32_t accessibilityId,
                              OHOS::NWeb::AccessibilityEventType eventType);

  void OnHoverEvent(const gfx::PointF& point);

  void FireGeneratedEvent(ui::AXEventGenerator::Event event_type,
                          const ui::AXNode* node) override;

 private:
  void HandleHover(int accessibilityId);

  void MoveAccessibilityFocusToIdAndRefocusIfNeeded(
      int newAccessibilityFocusId);

  void HandleFocusChanged(int32_t accessibilityId);

  void HandleEditableTextChanged(int32_t accessibilityId);

  int32_t TranslateAccessibilityId(int32_t accessibilityId) const;

  int32_t lastHoverId_ = -1;

  int32_t accessibilityFocusId_ = -1;

  std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback>
      accessibilityEventListener_;
};
}  // namespace content

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_