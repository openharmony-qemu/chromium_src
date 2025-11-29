// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_
#define CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_

#include <string>

#include "content/common/content_export.h"
#include "ui/accessibility/platform/browser_accessibility_manager.h"

namespace content {
// Manages a tree of BrowserAccessibility objects.

enum class AccessibilityEventType : int32_t {
  CLICK = 0x00000001,
  LONG_PRESS = 0x00000002,
  SELECTED = 0x00000004,
  BLUR = 0x00000007,
  FOCUS = 0x00000008,
  TEXT_CHANGE = 0x00000010,
  EJECT_DISMISS = 0x00000020,
  MOUSE = 149,
  KEYBOARD_BACK = 150,
  KEYBOARD_UP = 151,
  KEYBOARD_DOWN = 152,
  KEYBOARD_LEFT = 153,
  KEYBOARD_RIGHT = 154,
  KEYBOARD_CENTER = 155,
  HANDLE_A = 156,
  HANDLE_SELECT = 157,
  KEYBOARD_TAB = 158,
  KEYBOARD_SPACE = 159,
  KEYBOARD_ENTER = 160,
  KEYBOARD_ESCAPE = 161,
  KEYBOARD_NUMBER_ENTER = 162,
  TV_CONTROL_MEDIA_PLAY = 163,
  PAGE_CHANGE = 200,
  HOVER_ENTER_EVENT = 0x0000080,
  HOVER_EXIT_EVENT = 0x0000100,
  TOUCH_START = 0x00000200,
  TOUCH_MOVE = 0x00000201,
  TOUCH_CANCEL = 0x00000202,
  TOUCH_END = 0x00000400,
  CHANGE = 0x00000800,
  SCROLL_END = 0x00001000,
  TEXT_SELECTION_UPDATE = 0x000002000,
  ACCESSIBILITY_FOCUSED = 0x00008000,
  ACCESSIBILITY_FOCUS_CLEARED = 0x00010000,
  TEXT_MOVE_UNIT = 0x00020000,
  SCROLL_START = 0x01000000,
  UNKNOWN,
};

class AccessibilityEventCallback {
 public:
  AccessibilityEventCallback() = default;

  virtual ~AccessibilityEventCallback() = default;
  virtual void OnAccessibilityEvent(int64_t accessibility_id,
                                    uint32_t event_type) = 0;
};

class CONTENT_EXPORT BrowserAccessibilityManagerOHOS
    : public ui::BrowserAccessibilityManager {
 public:
  static BrowserAccessibilityManager* Create(
      const ui::AXTreeUpdate& initial_tree,
    ui::AXNodeIdDelegate& node_id_delegate,
    ui::AXPlatformTreeManagerDelegate* delegate);

  static BrowserAccessibilityManager* Create(
      ui::AXNodeIdDelegate& node_id_delegate,
    ui::AXPlatformTreeManagerDelegate* delegate);

  BrowserAccessibilityManagerOHOS(const ui::AXTreeUpdate& initial_tree,
    ui::AXNodeIdDelegate& node_id_delegate,
    ui::AXPlatformTreeManagerDelegate* delegate);

  ~BrowserAccessibilityManagerOHOS() = default;

  void FireFocusEvent(ui::AXNode* node) override;

  void FireBlinkEvent(ax::mojom::Event event_type,
      ui::BrowserAccessibility* node,
      int action_request_id) override;

  static void RegisterAccessibilityIdGenerator(
      std::function<int64_t()> accessibility_id_generator);

  static int64_t GenerateAccessibilityId();

  void RegisterAccessibilityEventListener(
      std::shared_ptr<AccessibilityEventCallback>
          accessibility_event_listener);

  void SetLastHoverId(int64_t id) { last_hover_id_ = id; }

  int64_t GetLastHoverId() const { return last_hover_id_; }

  void SetAccessibilityFocusId(int64_t id) { accessibility_focus_id_ = id; }

  int64_t GetAccessibilityFocusId() const { return accessibility_focus_id_; }

  void MoveAccessibilityFocus(int64_t old_id, int64_t new_id) const;

  bool MoveAccessibilityFocusToId(int64_t new_accessibility_focus_id);

  void SendAccessibilityEvent(int64_t accessibility_id,
      AccessibilityEventType event_type);

  void OnHoverEvent(const gfx::PointF& point);

  void FireGeneratedEvent(ui::AXEventGenerator::Event event_type,
      const ui::AXNode* node) override;

private:
  void HandleHover(int64_t accessibility_id);

  void MoveAccessibilityFocusToIdAndRefocusIfNeeded(
      int64_t new_accessibility_focus_id);

  void HandleFocusChanged(int64_t accessibility_id);

  void HandleEditableTextChanged(int64_t accessibility_id);

  int64_t TranslateAccessibilityId(int64_t accessibility_id) const;

  int64_t last_hover_id_ = -1;

  int64_t accessibility_focus_id_ = -1;

  std::shared_ptr<AccessibilityEventCallback> accessibility_event_listener_;
};
}  // namespace content

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_
