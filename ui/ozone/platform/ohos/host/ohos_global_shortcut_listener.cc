/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include "ui/ozone/platform/ohos/host/ohos_global_shortcut_listener.h"

#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <multimodalinput/oh_input_manager.h>

#include "base/functional/bind.h"
#include "base/logging.h"


namespace ui {

namespace {

using UIKeyCodeMap = std::unordered_map<KeyboardCode, Input_KeyCode>;

const UIKeyCodeMap g_ui_key_code_map(
    {UIKeyCodeMap::value_type(VKEY_BACK, KEYCODE_DEL),
     UIKeyCodeMap::value_type(VKEY_TAB, KEYCODE_TAB),
     UIKeyCodeMap::value_type(VKEY_RETURN, KEYCODE_ENTER),
     UIKeyCodeMap::value_type(VKEY_CAPITAL, KEYCODE_CAPS_LOCK),
     UIKeyCodeMap::value_type(VKEY_ESCAPE, KEYCODE_ESCAPE),
     UIKeyCodeMap::value_type(VKEY_SPACE, KEYCODE_SPACE),
     UIKeyCodeMap::value_type(VKEY_PRIOR, KEYCODE_PAGE_UP),
     UIKeyCodeMap::value_type(VKEY_NEXT, KEYCODE_PAGE_DOWN),
     UIKeyCodeMap::value_type(VKEY_END, KEYCODE_MOVE_END),
     UIKeyCodeMap::value_type(VKEY_HOME, KEYCODE_MOVE_HOME),
     UIKeyCodeMap::value_type(VKEY_LEFT, KEYCODE_DPAD_LEFT),
     UIKeyCodeMap::value_type(VKEY_UP, KEYCODE_DPAD_UP),
     UIKeyCodeMap::value_type(VKEY_RIGHT, KEYCODE_DPAD_RIGHT),
     UIKeyCodeMap::value_type(VKEY_DOWN, KEYCODE_DPAD_DOWN),
     UIKeyCodeMap::value_type(VKEY_SNAPSHOT, KEYCODE_SYSRQ),
     UIKeyCodeMap::value_type(VKEY_INSERT, KEYCODE_INSERT),
     UIKeyCodeMap::value_type(VKEY_DELETE, KEYCODE_FORWARD_DEL),
     UIKeyCodeMap::value_type(VKEY_0, KEYCODE_0),
     UIKeyCodeMap::value_type(VKEY_1, KEYCODE_1),
     UIKeyCodeMap::value_type(VKEY_2, KEYCODE_2),
     UIKeyCodeMap::value_type(VKEY_3, KEYCODE_3),
     UIKeyCodeMap::value_type(VKEY_4, KEYCODE_4),
     UIKeyCodeMap::value_type(VKEY_5, KEYCODE_5),
     UIKeyCodeMap::value_type(VKEY_6, KEYCODE_6),
     UIKeyCodeMap::value_type(VKEY_7, KEYCODE_7),
     UIKeyCodeMap::value_type(VKEY_8, KEYCODE_8),
     UIKeyCodeMap::value_type(VKEY_9, KEYCODE_9),
     UIKeyCodeMap::value_type(VKEY_A, KEYCODE_A),
     UIKeyCodeMap::value_type(VKEY_B, KEYCODE_B),
     UIKeyCodeMap::value_type(VKEY_C, KEYCODE_C),
     UIKeyCodeMap::value_type(VKEY_D, KEYCODE_D),
     UIKeyCodeMap::value_type(VKEY_E, KEYCODE_E),
     UIKeyCodeMap::value_type(VKEY_F, KEYCODE_F),
     UIKeyCodeMap::value_type(VKEY_G, KEYCODE_G),
     UIKeyCodeMap::value_type(VKEY_H, KEYCODE_H),
     UIKeyCodeMap::value_type(VKEY_I, KEYCODE_I),
     UIKeyCodeMap::value_type(VKEY_J, KEYCODE_J),
     UIKeyCodeMap::value_type(VKEY_K, KEYCODE_K),
     UIKeyCodeMap::value_type(VKEY_L, KEYCODE_L),
     UIKeyCodeMap::value_type(VKEY_M, KEYCODE_M),
     UIKeyCodeMap::value_type(VKEY_N, KEYCODE_N),
     UIKeyCodeMap::value_type(VKEY_O, KEYCODE_O),
     UIKeyCodeMap::value_type(VKEY_P, KEYCODE_P),
     UIKeyCodeMap::value_type(VKEY_Q, KEYCODE_Q),
     UIKeyCodeMap::value_type(VKEY_R, KEYCODE_R),
     UIKeyCodeMap::value_type(VKEY_S, KEYCODE_S),
     UIKeyCodeMap::value_type(VKEY_T, KEYCODE_T),
     UIKeyCodeMap::value_type(VKEY_U, KEYCODE_U),
     UIKeyCodeMap::value_type(VKEY_V, KEYCODE_V),
     UIKeyCodeMap::value_type(VKEY_W, KEYCODE_W),
     UIKeyCodeMap::value_type(VKEY_X, KEYCODE_X),
     UIKeyCodeMap::value_type(VKEY_Y, KEYCODE_Y),
     UIKeyCodeMap::value_type(VKEY_Z, KEYCODE_Z),
     UIKeyCodeMap::value_type(VKEY_NUMPAD0, KEYCODE_NUMPAD_0),
     UIKeyCodeMap::value_type(VKEY_NUMPAD1, KEYCODE_NUMPAD_1),
     UIKeyCodeMap::value_type(VKEY_NUMPAD2, KEYCODE_NUMPAD_2),
     UIKeyCodeMap::value_type(VKEY_NUMPAD3, KEYCODE_NUMPAD_3),
     UIKeyCodeMap::value_type(VKEY_NUMPAD4, KEYCODE_NUMPAD_4),
     UIKeyCodeMap::value_type(VKEY_NUMPAD5, KEYCODE_NUMPAD_5),
     UIKeyCodeMap::value_type(VKEY_NUMPAD6, KEYCODE_NUMPAD_6),
     UIKeyCodeMap::value_type(VKEY_NUMPAD7, KEYCODE_NUMPAD_7),
     UIKeyCodeMap::value_type(VKEY_NUMPAD8, KEYCODE_NUMPAD_8),
     UIKeyCodeMap::value_type(VKEY_NUMPAD9, KEYCODE_NUMPAD_9),
     UIKeyCodeMap::value_type(VKEY_MULTIPLY, KEYCODE_NUMPAD_MULTIPLY),
     UIKeyCodeMap::value_type(VKEY_ADD, KEYCODE_NUMPAD_ADD),
     UIKeyCodeMap::value_type(VKEY_SUBTRACT, KEYCODE_NUMPAD_SUBTRACT),
     UIKeyCodeMap::value_type(VKEY_DECIMAL, KEYCODE_NUMPAD_DOT),
     UIKeyCodeMap::value_type(VKEY_DIVIDE, KEYCODE_NUMPAD_DIVIDE),
     UIKeyCodeMap::value_type(VKEY_F1, KEYCODE_F1),
     UIKeyCodeMap::value_type(VKEY_F2, KEYCODE_F2),
     UIKeyCodeMap::value_type(VKEY_F3, KEYCODE_F3),
     UIKeyCodeMap::value_type(VKEY_F4, KEYCODE_F4),
     UIKeyCodeMap::value_type(VKEY_F5, KEYCODE_F5),
     UIKeyCodeMap::value_type(VKEY_F6, KEYCODE_F6),
     UIKeyCodeMap::value_type(VKEY_F7, KEYCODE_F7),
     UIKeyCodeMap::value_type(VKEY_F8, KEYCODE_F8),
     UIKeyCodeMap::value_type(VKEY_F9, KEYCODE_F9),
     UIKeyCodeMap::value_type(VKEY_F10, KEYCODE_F10),
     UIKeyCodeMap::value_type(VKEY_F11, KEYCODE_F11),
     UIKeyCodeMap::value_type(VKEY_F12, KEYCODE_F12),
     UIKeyCodeMap::value_type(VKEY_NUMLOCK, KEYCODE_NUM_LOCK),
     UIKeyCodeMap::value_type(VKEY_SCROLL, KEYCODE_SCROLL_LOCK),
     UIKeyCodeMap::value_type(VKEY_VOLUME_MUTE, KEYCODE_VOLUME_MUTE),
     UIKeyCodeMap::value_type(VKEY_VOLUME_DOWN, KEYCODE_VOLUME_DOWN),
     UIKeyCodeMap::value_type(VKEY_VOLUME_UP, KEYCODE_VOLUME_UP),
     UIKeyCodeMap::value_type(VKEY_OEM_1, KEYCODE_SEMICOLON),
     UIKeyCodeMap::value_type(VKEY_OEM_PLUS, KEYCODE_EQUALS),
     UIKeyCodeMap::value_type(VKEY_OEM_COMMA, KEYCODE_COMMA),
     UIKeyCodeMap::value_type(VKEY_OEM_MINUS, KEYCODE_MINUS),
     UIKeyCodeMap::value_type(VKEY_OEM_PERIOD, KEYCODE_PERIOD),
     UIKeyCodeMap::value_type(VKEY_OEM_2, KEYCODE_SLASH),
     UIKeyCodeMap::value_type(VKEY_OEM_3, KEYCODE_GRAVE),
     UIKeyCodeMap::value_type(VKEY_OEM_4, KEYCODE_LEFT_BRACKET),
     UIKeyCodeMap::value_type(VKEY_OEM_5, KEYCODE_BACKSLASH),
     UIKeyCodeMap::value_type(VKEY_OEM_6, KEYCODE_RIGHT_BRACKET),
     UIKeyCodeMap::value_type(VKEY_OEM_7, KEYCODE_APOSTROPHE)});

using OHKeyCodeMap = std::unordered_map<Input_KeyCode, KeyboardCode>;

OHKeyCodeMap CreateOHKeyCodeMap() {
  OHKeyCodeMap oh_key_code_map;
  for (auto [ui_key_code, oh_key_code] : g_ui_key_code_map) {
    oh_key_code_map.insert({oh_key_code, ui_key_code});
  }
  return oh_key_code_map;
}

const OHKeyCodeMap g_oh_key_code_map = CreateOHKeyCodeMap();

std::optional<Input_KeyCode> ToInputKeyCode(KeyboardCode ui_key_code) {
  auto it_key_code = g_ui_key_code_map.find(ui_key_code);
  if (it_key_code == g_ui_key_code_map.end()) {
    return std::nullopt;
  }
  return std::make_optional<Input_KeyCode>(it_key_code->second);
}

std::optional<KeyboardCode> ToKeyboardCode(Input_KeyCode oh_key_code) {
  auto it_key_code = g_oh_key_code_map.find(oh_key_code);
  if (it_key_code == g_oh_key_code_map.end()) {
    return std::nullopt;
  }
  return std::make_optional<KeyboardCode>(it_key_code->second);
}

using Accelerator = std::tuple<KeyboardCode, bool, bool, bool>;

std::optional<Accelerator> ToAccelerator(Input_Hotkey* hotkey) {
  if (!hotkey) {
    LOG(ERROR) << "[GlobalShortcut] ToAccelerator: hotkey is null";
    return std::nullopt;
  }

  int32_t modifiers[2] = {KEYCODE_UNKNOWN, KEYCODE_UNKNOWN};
  int32_t* modifiers_buf[2] = {modifiers, modifiers + 1};
  int32_t modifiers_size = 0;
  Input_Result result =
      OH_Input_GetPreKeys(hotkey, modifiers_buf, &modifiers_size);
  if (result != INPUT_SUCCESS) {
    LOG(ERROR) << "[GlobalShortcut] ToAccelerator: OH_Input_GetPreKeys failed, result: " << result;
    return std::nullopt;
  }

  bool is_alt_down = false;
  bool is_ctrl_down = false;
  bool is_shift_down = false;
  for (int32_t i = 0; i < modifiers_size; ++i) {
    if (modifiers[i] == KEYCODE_ALT_LEFT || modifiers[i] == KEYCODE_ALT_RIGHT) {
      is_alt_down = true;
    } else if (modifiers[i] == KEYCODE_CTRL_LEFT ||
               modifiers[i] == KEYCODE_CTRL_RIGHT) {
      is_ctrl_down = true;
    } else if (modifiers[i] == KEYCODE_SHIFT_LEFT ||
               modifiers[i] == KEYCODE_SHIFT_RIGHT) {
      is_shift_down = true;
    }
  }

  int32_t oh_key_code = KEYCODE_UNKNOWN;
  result = OH_Input_GetFinalKey(hotkey, &oh_key_code);
  if (result != INPUT_SUCCESS) {
    LOG(ERROR) << "[GlobalShortcut] ToAccelerator: OH_Input_GetFinalKey failed, result: " << result;
    return std::nullopt;
  }

  auto ui_key_code = ToKeyboardCode(static_cast<Input_KeyCode>(oh_key_code));
  if (!ui_key_code) {
    LOG(ERROR) << "[GlobalShortcut] ToAccelerator: ToKeyboardCode failed, oh_key_code: " << oh_key_code;
    return std::nullopt;
  }

  return std::make_optional<Accelerator>(*ui_key_code, is_alt_down,
                                         is_ctrl_down, is_shift_down);
}

// OhosGlobalShortcutListener instance for Input_Hotkey Callback
OhosGlobalShortcutListener* g_listener = nullptr;

// Input_Hotkey Callback
void OnHotkeyPressed(Input_Hotkey* hotkey) {
  if (!g_listener) {
    LOG(ERROR) << "[GlobalShortcut] OnHotkeyPressed: Global shortcut listener not start";
    return;
  }

  auto accelerator = ToAccelerator(hotkey);
  if (!accelerator) {
    return;
  }
  auto [ui_key_code, is_alt_down, is_ctrl_down, is_shift_down] =
      *accelerator;
  auto task = base::BindOnce(
      &OhosGlobalShortcutListener::OnKeyPressed, base::Unretained(g_listener),
      ui_key_code, is_alt_down, is_ctrl_down, is_shift_down);
  g_listener->ui_task_runner_->PostTask(FROM_HERE, std::move(task));
}

// deleter for Input_Hotkey
struct HotkeyDeleter {
  void operator()(Input_Hotkey* hotkey) const {
    OH_Input_DestroyHotkey(&hotkey);
  }
};
// std::unique_ptr for Input_Hotkey
using ScopedHotkeyPtr = std::unique_ptr<Input_Hotkey, HotkeyDeleter>;

std::vector<ScopedHotkeyPtr> ToHotkeys(
    Input_KeyCode key_code,
    const std::vector<Input_KeyCode>& modifiers) {
  std::vector<ScopedHotkeyPtr> hotkeys;
  for (auto modifier : modifiers) {
    ScopedHotkeyPtr hotkey(OH_Input_CreateHotkey());
    if (!hotkey) {
      LOG(ERROR) << "[GlobalShortcut] ToHotkeys: OH_Input_CreateHotkey failed, modifier: " << modifier;
      return {};
    }

    int32_t modifiers_buf[1] = {modifier};
    OH_Input_SetPreKeys(hotkey.get(), modifiers_buf, 1);
    OH_Input_SetFinalKey(hotkey.get(), key_code);
    OH_Input_SetRepeat(hotkey.get(), false);
    hotkeys.push_back(std::move(hotkey));
  }
  return hotkeys;
}

std::vector<ScopedHotkeyPtr> ToHotkeys(
    Input_KeyCode key_code,
    const std::vector<Input_KeyCode>& modifiers_1,
    const std::vector<Input_KeyCode>& modifiers_2) {
  std::vector<ScopedHotkeyPtr> hotkeys;
  for (auto modifier_1 : modifiers_1) {
    for (auto modifier_2 : modifiers_2) {
      ScopedHotkeyPtr hotkey(OH_Input_CreateHotkey());
      if (!hotkey) {
        LOG(ERROR) << "[GlobalShortcut] ToHotkeys: OH_Input_CreateHotkey failed, modifier_1: "
                   << modifier_1 << ", modifier_2: " << modifier_2;
        return {};
      }

      int32_t modifiers_buf[2] = {modifier_1, modifier_2};
      OH_Input_SetPreKeys(hotkey.get(), modifiers_buf, 2);
      OH_Input_SetFinalKey(hotkey.get(), key_code);
      OH_Input_SetRepeat(hotkey.get(), false);
      hotkeys.push_back(std::move(hotkey));
    }
  }
  return hotkeys;
}

std::vector<ScopedHotkeyPtr> ToHotkeys(KeyboardCode key_code,
                                       bool is_alt_down,
                                       bool is_ctrl_down,
                                       bool is_shift_down) {
  std::vector<std::vector<Input_KeyCode>> modifiers;
  if (is_alt_down) {
    modifiers.push_back({KEYCODE_ALT_LEFT, KEYCODE_ALT_RIGHT});
  }
  if (is_ctrl_down) {
    modifiers.push_back({KEYCODE_CTRL_LEFT, KEYCODE_CTRL_RIGHT});
  }
  if (is_shift_down) {
    modifiers.push_back({KEYCODE_SHIFT_LEFT, KEYCODE_SHIFT_RIGHT});
  }

  auto oh_key_code = ToInputKeyCode(key_code);
  if (!oh_key_code) {
    LOG(ERROR) << "[GlobalShortcut] ToHotkeys: ToInputKeyCode failed, key_code: " << key_code;
    return {};
  }

  if (modifiers.size() == 1U) {
    return ToHotkeys(*oh_key_code, modifiers[0U]);
  } else if (modifiers.size() == 2U) {
    return ToHotkeys(*oh_key_code, modifiers[0U], modifiers[1U]);
  }

  return {};
}

void UnregisterHotKeys(const std::vector<ScopedHotkeyPtr>& hotkeys) {
  for (auto& hotkey : hotkeys) {
    OH_Input_RemoveHotkeyMonitor(hotkey.get(), OnHotkeyPressed);
  }
}

bool RegisterHotKeys(std::vector<ScopedHotkeyPtr>& hotkeys) {
  std::vector<ScopedHotkeyPtr> registered_hotkeys;
  for (auto& hotkey : hotkeys) {
    Input_Result result =
        OH_Input_AddHotkeyMonitor(hotkey.get(), OnHotkeyPressed);
    if (result != INPUT_SUCCESS) {
      LOG(ERROR) << "[GlobalShortcut] RegisterHotKeys: OH_Input_AddHotkeyMonitor failed, result: " << result;
      UnregisterHotKeys(registered_hotkeys);
      return false;
    }
    registered_hotkeys.push_back(std::move(hotkey));
  }
  return true;
}

}  // namespace

OhosGlobalShortcutListener::OhosGlobalShortcutListener(
    PlatformGlobalShortcutListenerDelegate* delegate)
    : PlatformGlobalShortcutListener(delegate),
      ui_task_runner_(base::TaskRunnerOHOS::GetUIThreadTaskRunner()) {}

OhosGlobalShortcutListener::~OhosGlobalShortcutListener() {
  if (delegate())
    delegate()->OnPlatformListenerDestroyed();
}

void OhosGlobalShortcutListener::OnKeyPressed(KeyboardCode key_code,
                                              bool is_alt_down,
                                              bool is_ctrl_down,
                                              bool is_shift_down) {
  if (delegate()) {
    delegate()->OnKeyPressed(key_code, is_alt_down, is_ctrl_down, is_shift_down);
  }
}

void OhosGlobalShortcutListener::StartListening() {
  g_listener = this;
}

void OhosGlobalShortcutListener::StopListening() {
  g_listener = nullptr;
}

bool OhosGlobalShortcutListener::RegisterAccelerator(KeyboardCode key_code,
                                                     bool is_alt_down,
                                                     bool is_ctrl_down,
                                                     bool is_shift_down) {
  auto hotkeys = ToHotkeys(key_code, is_alt_down, is_ctrl_down, is_shift_down);
  if (hotkeys.empty()) {
    return false;
  }

  return RegisterHotKeys(hotkeys);
}

void OhosGlobalShortcutListener::UnregisterAccelerator(KeyboardCode key_code,
                                                       bool is_alt_down,
                                                       bool is_ctrl_down,
                                                       bool is_shift_down) {
  auto hotkeys = ToHotkeys(key_code, is_alt_down, is_ctrl_down, is_shift_down);
  if (hotkeys.empty()) {
    return;
  }

  UnregisterHotKeys(hotkeys);
}

}  // namespace ui
