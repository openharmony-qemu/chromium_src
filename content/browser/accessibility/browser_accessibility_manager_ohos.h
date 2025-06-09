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

class NWebAccessibilityEventCallback {
 public:
  NWebAccessibilityEventCallback() = default;

  virtual ~NWebAccessibilityEventCallback() = default;
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
      std::shared_ptr<NWebAccessibilityEventCallback>
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

  std::shared_ptr<NWebAccessibilityEventCallback> accessibility_event_listener_;
};
}  // namespace content

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_MANAGER_OHOS_H_
