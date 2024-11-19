/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_input_delegate.h"
#include "cef/include/base/cef_logging.h"
#include "cef/include/internal/cef_types_wrappers.h"
#include "mmi_keyevent_adapter.h"
#include "nweb_inputmethod_handler.h"

using namespace OHOS::NWeb;
using OHOS::NWeb::MMIAdapter::KeyEvent;

namespace OHOS::NWeb {
NWebInputEventHandle<int, int> NWebInputDelegate::keyEventHandle_;
}

const int32_t KEY_DOWN = 0;
const int32_t KEY_UP = 1;
const int32_t LEFT_BUTTON = 1;
const int32_t RIGHT_BUTTON = 2;
const int32_t MIDDLE_BUTTON = 4;
const int32_t BACK_BUTTON = 8;
const int32_t FORWARD_BUTTON = 16;

const std::vector<unsigned int> mmiKeyEvent {};

const std::unordered_map<int, int> keycodeConverter = {
    {KeyEvent::KEYCODE_0, ui::VKEY_0},
    {KeyEvent::KEYCODE_1, ui::VKEY_1},
    {KeyEvent::KEYCODE_2, ui::VKEY_2},
    {KeyEvent::KEYCODE_3, ui::VKEY_3},
    {KeyEvent::KEYCODE_4, ui::VKEY_4},
    {KeyEvent::KEYCODE_5, ui::VKEY_5},
    {KeyEvent::KEYCODE_6, ui::VKEY_6},
    {KeyEvent::KEYCODE_7, ui::VKEY_7},
    {KeyEvent::KEYCODE_8, ui::VKEY_8},
    {KeyEvent::KEYCODE_9, ui::VKEY_9},
    {KeyEvent::KEYCODE_A, ui::VKEY_A},
    {KeyEvent::KEYCODE_B, ui::VKEY_B},
    {KeyEvent::KEYCODE_C, ui::VKEY_C},
    {KeyEvent::KEYCODE_D, ui::VKEY_D},
    {KeyEvent::KEYCODE_E, ui::VKEY_E},
    {KeyEvent::KEYCODE_F, ui::VKEY_F},
    {KeyEvent::KEYCODE_G, ui::VKEY_G},
    {KeyEvent::KEYCODE_H, ui::VKEY_H},
    {KeyEvent::KEYCODE_I, ui::VKEY_I},
    {KeyEvent::KEYCODE_J, ui::VKEY_J},
    {KeyEvent::KEYCODE_K, ui::VKEY_K},
    {KeyEvent::KEYCODE_L, ui::VKEY_L},
    {KeyEvent::KEYCODE_M, ui::VKEY_M},
    {KeyEvent::KEYCODE_N, ui::VKEY_N},
    {KeyEvent::KEYCODE_O, ui::VKEY_O},
    {KeyEvent::KEYCODE_P, ui::VKEY_P},
    {KeyEvent::KEYCODE_Q, ui::VKEY_Q},
    {KeyEvent::KEYCODE_R, ui::VKEY_R},
    {KeyEvent::KEYCODE_S, ui::VKEY_S},
    {KeyEvent::KEYCODE_T, ui::VKEY_T},
    {KeyEvent::KEYCODE_U, ui::VKEY_U},
    {KeyEvent::KEYCODE_V, ui::VKEY_V},
    {KeyEvent::KEYCODE_W, ui::VKEY_W},
    {KeyEvent::KEYCODE_X, ui::VKEY_X},
    {KeyEvent::KEYCODE_Y, ui::VKEY_Y},
    {KeyEvent::KEYCODE_Z, ui::VKEY_Z},
    {KeyEvent::KEYCODE_SHIFT_LEFT, ui::VKEY_SHIFT},
    {KeyEvent::KEYCODE_SHIFT_RIGHT, ui::VKEY_SHIFT},
    {KeyEvent::KEYCODE_TAB, ui::VKEY_TAB},
    {KeyEvent::KEYCODE_SPACE, ui::VKEY_SPACE},
    {KeyEvent::KEYCODE_ENTER, ui::VKEY_RETURN},
    {KeyEvent::KEYCODE_NUMPAD_ENTER, ui::VKEY_RETURN},
    {KeyEvent::KEYCODE_DEL, ui::VKEY_BACK},
    {KeyEvent::KEYCODE_FORWARD_DEL, ui::VKEY_DELETE},
    {KeyEvent::KEYCODE_CTRL_LEFT, ui::VKEY_CONTROL},
    {KeyEvent::KEYCODE_CTRL_RIGHT, ui::VKEY_CONTROL},
    {KeyEvent::KEYCODE_ALT_LEFT, ui::VKEY_MENU},
    {KeyEvent::KEYCODE_ALT_RIGHT, ui::VKEY_MENU},
    {KeyEvent::KEYCODE_ESCAPE, ui::VKEY_ESCAPE},
    {KeyEvent::KEYCODE_F1, ui::VKEY_F1},
    {KeyEvent::KEYCODE_F2, ui::VKEY_F2},
    {KeyEvent::KEYCODE_F3, ui::VKEY_F3},
    {KeyEvent::KEYCODE_F4, ui::VKEY_F4},
    {KeyEvent::KEYCODE_F5, ui::VKEY_F5},
    {KeyEvent::KEYCODE_F6, ui::VKEY_F6},
    {KeyEvent::KEYCODE_F7, ui::VKEY_F7},
    {KeyEvent::KEYCODE_F8, ui::VKEY_F8},
    {KeyEvent::KEYCODE_F9, ui::VKEY_F9},
    {KeyEvent::KEYCODE_F10, ui::VKEY_F10},
    {KeyEvent::KEYCODE_F11, ui::VKEY_F11},
    {KeyEvent::KEYCODE_F12, ui::VKEY_F12},
    {KeyEvent::KEYCODE_PAGE_UP, ui::VKEY_PRIOR},
    {KeyEvent::KEYCODE_PAGE_DOWN, ui::VKEY_NEXT},
    {KeyEvent::KEYCODE_MOVE_HOME, ui::VKEY_HOME},
    {KeyEvent::KEYCODE_MOVE_END, ui::VKEY_END},
    {KeyEvent::KEYCODE_MINUS, ui::VKEY_OEM_MINUS},
    {KeyEvent::KEYCODE_EQUALS, ui::VKEY_OEM_PLUS},
    {KeyEvent::KEYCODE_LEFT_BRACKET, ui::VKEY_OEM_4},
    {KeyEvent::KEYCODE_RIGHT_BRACKET, ui::VKEY_OEM_6},
    {KeyEvent::KEYCODE_SEMICOLON, ui::VKEY_OEM_1},
    {KeyEvent::KEYCODE_APOSTROPHE, ui::VKEY_OEM_7},
    {KeyEvent::KEYCODE_GRAVE, ui::VKEY_OEM_3},
    {KeyEvent::KEYCODE_BACKSLASH, ui::VKEY_OEM_5},
    {KeyEvent::KEYCODE_COMMA, ui::VKEY_OEM_COMMA},
    {KeyEvent::KEYCODE_PERIOD, ui::VKEY_OEM_PERIOD},
    {KeyEvent::KEYCODE_SLASH, ui::VKEY_OEM_2},
    {KeyEvent::KEYCODE_NUMPAD_MULTIPLY, ui::VKEY_MULTIPLY},
    {KeyEvent::KEYCODE_CAPS_LOCK, ui::VKEY_CAPITAL},
    {KeyEvent::KEYCODE_NUM_LOCK, ui::VKEY_NUMLOCK},
    {KeyEvent::KEYCODE_SCROLL_LOCK, ui::VKEY_SCROLL},
    {KeyEvent::KEYCODE_NUMPAD_0, ui::VKEY_NUMPAD0},
    {KeyEvent::KEYCODE_NUMPAD_1, ui::VKEY_NUMPAD1},
    {KeyEvent::KEYCODE_NUMPAD_2, ui::VKEY_NUMPAD2},
    {KeyEvent::KEYCODE_NUMPAD_3, ui::VKEY_NUMPAD3},
    {KeyEvent::KEYCODE_NUMPAD_4, ui::VKEY_NUMPAD4},
    {KeyEvent::KEYCODE_NUMPAD_5, ui::VKEY_NUMPAD5},
    {KeyEvent::KEYCODE_NUMPAD_6, ui::VKEY_NUMPAD6},
    {KeyEvent::KEYCODE_NUMPAD_7, ui::VKEY_NUMPAD7},
    {KeyEvent::KEYCODE_NUMPAD_8, ui::VKEY_NUMPAD8},
    {KeyEvent::KEYCODE_NUMPAD_9, ui::VKEY_NUMPAD9},
    {KeyEvent::KEYCODE_NUMPAD_SUBTRACT, ui::VKEY_SUBTRACT},
    {KeyEvent::KEYCODE_NUMPAD_ADD, ui::VKEY_ADD},
    {KeyEvent::KEYCODE_NUMPAD_DOT, ui::VKEY_DECIMAL},
    {KeyEvent::KEYCODE_NUMPAD_DIVIDE, ui::VKEY_DIVIDE},
    {KeyEvent::KEYCODE_SYSRQ, ui::VKEY_SNAPSHOT},
    {KeyEvent::KEYCODE_INSERT, ui::VKEY_INSERT},
    {KeyEvent::KEYCODE_BREAK, ui::VKEY_PAUSE},
    {KeyEvent::KEYCODE_META_LEFT, ui::VKEY_LWIN},
    {KeyEvent::KEYCODE_META_RIGHT, ui::VKEY_RWIN},
    {KeyEvent::KEYCODE_DPAD_UP, ui::VKEY_UP},
    {KeyEvent::KEYCODE_DPAD_DOWN, ui::VKEY_DOWN},
    {KeyEvent::KEYCODE_DPAD_LEFT, ui::VKEY_LEFT},
    {KeyEvent::KEYCODE_DPAD_RIGHT, ui::VKEY_RIGHT},
};

const std::unordered_map<int, int> keyactionConverter = {
    {KEY_UP, KEYEVENT_KEYUP},
    {KEY_DOWN, KEYEVENT_RAWKEYDOWN},
};

const std::unordered_map<int, int> ohosKeyCodeConverter = {
    {ui::VKEY_0, KeyEvent::KEYCODE_0},
    {ui::VKEY_1, KeyEvent::KEYCODE_1},
    {ui::VKEY_2, KeyEvent::KEYCODE_2},
    {ui::VKEY_3, KeyEvent::KEYCODE_3},
    {ui::VKEY_4, KeyEvent::KEYCODE_4},
    {ui::VKEY_5, KeyEvent::KEYCODE_5},
    {ui::VKEY_6, KeyEvent::KEYCODE_6},
    {ui::VKEY_7, KeyEvent::KEYCODE_7},
    {ui::VKEY_8, KeyEvent::KEYCODE_8},
    {ui::VKEY_9, KeyEvent::KEYCODE_9},
    {ui::VKEY_A, KeyEvent::KEYCODE_A},
    {ui::VKEY_B, KeyEvent::KEYCODE_B},
    {ui::VKEY_C, KeyEvent::KEYCODE_C},
    {ui::VKEY_D, KeyEvent::KEYCODE_D},
    {ui::VKEY_E, KeyEvent::KEYCODE_E},
    {ui::VKEY_F, KeyEvent::KEYCODE_F},
    {ui::VKEY_G, KeyEvent::KEYCODE_G},
    {ui::VKEY_H, KeyEvent::KEYCODE_H},
    {ui::VKEY_I, KeyEvent::KEYCODE_I},
    {ui::VKEY_J, KeyEvent::KEYCODE_J},
    {ui::VKEY_K, KeyEvent::KEYCODE_K},
    {ui::VKEY_L, KeyEvent::KEYCODE_L},
    {ui::VKEY_M, KeyEvent::KEYCODE_M},
    {ui::VKEY_N, KeyEvent::KEYCODE_N},
    {ui::VKEY_O, KeyEvent::KEYCODE_O},
    {ui::VKEY_P, KeyEvent::KEYCODE_P},
    {ui::VKEY_Q, KeyEvent::KEYCODE_Q},
    {ui::VKEY_R, KeyEvent::KEYCODE_R},
    {ui::VKEY_S, KeyEvent::KEYCODE_S},
    {ui::VKEY_T, KeyEvent::KEYCODE_T},
    {ui::VKEY_U, KeyEvent::KEYCODE_U},
    {ui::VKEY_V, KeyEvent::KEYCODE_V},
    {ui::VKEY_W, KeyEvent::KEYCODE_W},
    {ui::VKEY_X, KeyEvent::KEYCODE_X},
    {ui::VKEY_Y, KeyEvent::KEYCODE_Y},
    {ui::VKEY_Z, KeyEvent::KEYCODE_Z},
    {ui::VKEY_SHIFT, KeyEvent::KEYCODE_SHIFT_LEFT},
    {ui::VKEY_LSHIFT, KeyEvent::KEYCODE_SHIFT_LEFT},
    {ui::VKEY_RSHIFT, KeyEvent::KEYCODE_SHIFT_RIGHT},
    {ui::VKEY_TAB, KeyEvent::KEYCODE_TAB},
    {ui::VKEY_SPACE, KeyEvent::KEYCODE_SPACE},
    {ui::VKEY_RETURN, KeyEvent::KEYCODE_ENTER},
    {ui::VKEY_RETURN, KeyEvent::KEYCODE_NUMPAD_ENTER},
    {ui::VKEY_BACK, KeyEvent::KEYCODE_DEL},
    {ui::VKEY_DELETE, KeyEvent::KEYCODE_FORWARD_DEL},
    {ui::VKEY_CONTROL, KeyEvent::KEYCODE_CTRL_LEFT},
    {ui::VKEY_LCONTROL, KeyEvent::KEYCODE_CTRL_LEFT},
    {ui::VKEY_RCONTROL, KeyEvent::KEYCODE_CTRL_RIGHT},
    {ui::VKEY_MENU, KeyEvent::KEYCODE_ALT_LEFT},
    {ui::VKEY_ESCAPE, KeyEvent::KEYCODE_ESCAPE},
    {ui::VKEY_F1, KeyEvent::KEYCODE_F1},
    {ui::VKEY_F2, KeyEvent::KEYCODE_F2},
    {ui::VKEY_F3, KeyEvent::KEYCODE_F3},
    {ui::VKEY_F4, KeyEvent::KEYCODE_F4},
    {ui::VKEY_F5, KeyEvent::KEYCODE_F5},
    {ui::VKEY_F6, KeyEvent::KEYCODE_F6},
    {ui::VKEY_F7, KeyEvent::KEYCODE_F7},
    {ui::VKEY_F8, KeyEvent::KEYCODE_F8},
    {ui::VKEY_F9, KeyEvent::KEYCODE_F9},
    {ui::VKEY_F10, KeyEvent::KEYCODE_F10},
    {ui::VKEY_F11, KeyEvent::KEYCODE_F11},
    {ui::VKEY_F12, KeyEvent::KEYCODE_F12},
    {ui::VKEY_PRIOR, KeyEvent::KEYCODE_PAGE_UP},
    {ui::VKEY_NEXT, KeyEvent::KEYCODE_PAGE_DOWN},
    {ui::VKEY_HOME, KeyEvent::KEYCODE_MOVE_HOME},
    {ui::VKEY_END, KeyEvent::KEYCODE_MOVE_END},
    {ui::VKEY_OEM_MINUS, KeyEvent::KEYCODE_MINUS},
    {ui::VKEY_OEM_PLUS, KeyEvent::KEYCODE_EQUALS},
    {ui::VKEY_OEM_4, KeyEvent::KEYCODE_LEFT_BRACKET},
    {ui::VKEY_OEM_6, KeyEvent::KEYCODE_RIGHT_BRACKET},
    {ui::VKEY_OEM_1, KeyEvent::KEYCODE_SEMICOLON},
    {ui::VKEY_OEM_7, KeyEvent::KEYCODE_APOSTROPHE},
    {ui::VKEY_OEM_3, KeyEvent::KEYCODE_GRAVE},
    {ui::VKEY_OEM_5, KeyEvent::KEYCODE_BACKSLASH},
    {ui::VKEY_OEM_COMMA, KeyEvent::KEYCODE_COMMA},
    {ui::VKEY_OEM_PERIOD, KeyEvent::KEYCODE_PERIOD},
    {ui::VKEY_OEM_2, KeyEvent::KEYCODE_SLASH},
    {ui::VKEY_MULTIPLY, KeyEvent::KEYCODE_NUMPAD_MULTIPLY},
    {ui::VKEY_CAPITAL, KeyEvent::KEYCODE_CAPS_LOCK},
    {ui::VKEY_NUMLOCK, KeyEvent::KEYCODE_NUM_LOCK},
    {ui::VKEY_SCROLL, KeyEvent::KEYCODE_SCROLL_LOCK},
    {ui::VKEY_NUMPAD0, KeyEvent::KEYCODE_NUMPAD_0},
    {ui::VKEY_NUMPAD1, KeyEvent::KEYCODE_NUMPAD_1},
    {ui::VKEY_NUMPAD2, KeyEvent::KEYCODE_NUMPAD_2},
    {ui::VKEY_NUMPAD3, KeyEvent::KEYCODE_NUMPAD_3},
    {ui::VKEY_NUMPAD4, KeyEvent::KEYCODE_NUMPAD_4},
    {ui::VKEY_NUMPAD5, KeyEvent::KEYCODE_NUMPAD_5},
    {ui::VKEY_NUMPAD6, KeyEvent::KEYCODE_NUMPAD_6},
    {ui::VKEY_NUMPAD7, KeyEvent::KEYCODE_NUMPAD_7},
    {ui::VKEY_NUMPAD8, KeyEvent::KEYCODE_NUMPAD_8},
    {ui::VKEY_NUMPAD9, KeyEvent::KEYCODE_NUMPAD_9},
    {ui::VKEY_SUBTRACT, KeyEvent::KEYCODE_NUMPAD_SUBTRACT},
    {ui::VKEY_ADD, KeyEvent::KEYCODE_NUMPAD_ADD},
    {ui::VKEY_DECIMAL, KeyEvent::KEYCODE_NUMPAD_DOT},
    {ui::VKEY_DIVIDE, KeyEvent::KEYCODE_NUMPAD_DIVIDE},
    {ui::VKEY_SNAPSHOT, KeyEvent::KEYCODE_SYSRQ},
    {ui::VKEY_INSERT, KeyEvent::KEYCODE_INSERT},
    {ui::VKEY_PAUSE, KeyEvent::KEYCODE_BREAK},
    {ui::VKEY_LWIN, KeyEvent::KEYCODE_META_LEFT},
    {ui::VKEY_RWIN, KeyEvent::KEYCODE_META_RIGHT},
    {ui::VKEY_UP, KeyEvent::KEYCODE_DPAD_UP},
    {ui::VKEY_DOWN, KeyEvent::KEYCODE_DPAD_DOWN},
    {ui::VKEY_LEFT, KeyEvent::KEYCODE_DPAD_LEFT},
    {ui::VKEY_RIGHT, KeyEvent::KEYCODE_DPAD_RIGHT},
};

const std::unordered_map<int, int> ohosKeyActionConverter = {
    {KEYEVENT_KEYUP, KEY_UP},
    {KEYEVENT_RAWKEYDOWN, KEY_DOWN},
};

const std::unordered_map<int, int> mousebuttonConverter = {
    {LEFT_BUTTON, MBT_LEFT},       {RIGHT_BUTTON, MBT_RIGHT},
    {MIDDLE_BUTTON, MBT_MIDDLE},   {BACK_BUTTON, MBT_BACK},
    {FORWARD_BUTTON, MBT_FORWARD},
};

const std::unordered_map<int, int> keyScanCodeConverter = {
    {KeyEvent::KEYCODE_ESCAPE, ScanKeyCode::ESCAPE_SCAN_CODE},
    {KeyEvent::KEYCODE_1, ScanKeyCode::DIGIT1_SCAN_CODE},
    {KeyEvent::KEYCODE_2, ScanKeyCode::DIGIT2_SCAN_CODE},
    {KeyEvent::KEYCODE_3, ScanKeyCode::DIGIT3_SCAN_CODE},
    {KeyEvent::KEYCODE_4, ScanKeyCode::DIGIT4_SCAN_CODE},
    {KeyEvent::KEYCODE_5, ScanKeyCode::DIGIT5_SCAN_CODE},
    {KeyEvent::KEYCODE_6, ScanKeyCode::DIGIT6_SCAN_CODE},
    {KeyEvent::KEYCODE_7, ScanKeyCode::DIGIT7_SCAN_CODE},
    {KeyEvent::KEYCODE_8, ScanKeyCode::DIGIT8_SCAN_CODE},
    {KeyEvent::KEYCODE_9, ScanKeyCode::DIGIT9_SCAN_CODE},
    {KeyEvent::KEYCODE_0, ScanKeyCode::DIGIT0_SCAN_CODE},
    {KeyEvent::KEYCODE_MINUS, ScanKeyCode::MINUS_SCAN_CODE},
    {KeyEvent::KEYCODE_EQUALS, ScanKeyCode::EQUAL_SCAN_CODE},
    {KeyEvent::KEYCODE_DEL, ScanKeyCode::BACKSPACE_SCAN_CODE},
    {KeyEvent::KEYCODE_TAB, ScanKeyCode::TAB_SCAN_CODE},
    {KeyEvent::KEYCODE_W, ScanKeyCode::KEYW_SCAN_CODE},
    {KeyEvent::KEYCODE_Q, ScanKeyCode::KEYQ_SCAN_CODE},
    {KeyEvent::KEYCODE_E, ScanKeyCode::KEYE_SCAN_CODE},
    {KeyEvent::KEYCODE_R, ScanKeyCode::KEYR_SCAN_CODE},
    {KeyEvent::KEYCODE_T, ScanKeyCode::KEYT_SCAN_CODE},
    {KeyEvent::KEYCODE_Y, ScanKeyCode::KEYY_SCAN_CODE},
    {KeyEvent::KEYCODE_U, ScanKeyCode::KEYU_SCAN_CODE},
    {KeyEvent::KEYCODE_I, ScanKeyCode::KEYI_SCAN_CODE},
    {KeyEvent::KEYCODE_O, ScanKeyCode::KEYO_SCAN_CODE},
    {KeyEvent::KEYCODE_P, ScanKeyCode::KEYP_SCAN_CODE},
    {KeyEvent::KEYCODE_LEFT_BRACKET, ScanKeyCode::BRACKETLEFT_SCAN_CODE},
    {KeyEvent::KEYCODE_RIGHT_BRACKET, ScanKeyCode::BRACKETRIGHT_SCAN_CODE},
    {KeyEvent::KEYCODE_ENTER, ScanKeyCode::ENTER_SCAN_CODE},
    {KeyEvent::KEYCODE_CTRL_LEFT, ScanKeyCode::CONTROLLEFT_SCAN_CODE},
    {KeyEvent::KEYCODE_A, ScanKeyCode::KEYA_SCAN_CODE},
    {KeyEvent::KEYCODE_S, ScanKeyCode::KEYS_SCAN_CODE},
    {KeyEvent::KEYCODE_D, ScanKeyCode::KEYD_SCAN_CODE},
    {KeyEvent::KEYCODE_F, ScanKeyCode::KEYF_SCAN_CODE},
    {KeyEvent::KEYCODE_G, ScanKeyCode::KEYG_SCAN_CODE},
    {KeyEvent::KEYCODE_H, ScanKeyCode::KEYH_SCAN_CODE},
    {KeyEvent::KEYCODE_J, ScanKeyCode::KEYJ_SCAN_CODE},
    {KeyEvent::KEYCODE_K, ScanKeyCode::KEYK_SCAN_CODE},
    {KeyEvent::KEYCODE_L, ScanKeyCode::KEYL_SCAN_CODE},
    {KeyEvent::KEYCODE_SEMICOLON, ScanKeyCode::SEMICOLON_SCAN_CODE},
    {KeyEvent::KEYCODE_APOSTROPHE, ScanKeyCode::QUOTE_SCAN_CODE},
    {KeyEvent::KEYCODE_GRAVE, ScanKeyCode::BACKQUOTE_SCAN_CODE},
    {KeyEvent::KEYCODE_SHIFT_LEFT, ScanKeyCode::SHIFTLEFT_SCAN_CODE},
    {KeyEvent::KEYCODE_BACKSLASH, ScanKeyCode::BACKSLASH_SCAN_CODE},
    {KeyEvent::KEYCODE_Z, ScanKeyCode::KEYZ_SCAN_CODE},
    {KeyEvent::KEYCODE_X, ScanKeyCode::KEYX_SCAN_CODE},
    {KeyEvent::KEYCODE_C, ScanKeyCode::KEYC_SCAN_CODE},
    {KeyEvent::KEYCODE_V, ScanKeyCode::KEYV_SCAN_CODE},
    {KeyEvent::KEYCODE_B, ScanKeyCode::KEYB_SCAN_CODE},
    {KeyEvent::KEYCODE_N, ScanKeyCode::KEYN_SCAN_CODE},
    {KeyEvent::KEYCODE_M, ScanKeyCode::KEYM_SCAN_CODE},
    {KeyEvent::KEYCODE_COMMA, ScanKeyCode::COMMA_SCAN_CODE},
    {KeyEvent::KEYCODE_PERIOD, ScanKeyCode::PERIOD_SCAN_CODE},
    {KeyEvent::KEYCODE_SLASH, ScanKeyCode::SLASH_SCAN_CODE},
    {KeyEvent::KEYCODE_SHIFT_RIGHT, ScanKeyCode::SHIFTRIGHT_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_MULTIPLY, ScanKeyCode::NUMPADMULTIPLY_SCAN_CODE},
    {KeyEvent::KEYCODE_ALT_LEFT, ScanKeyCode::ALTLEFT_SCAN_CODE},
    {KeyEvent::KEYCODE_SPACE, ScanKeyCode::SPACE_SCAN_CODE},
    {KeyEvent::KEYCODE_CAPS_LOCK, ScanKeyCode::CAPSLOCK_SCAN_CODE},
    {KeyEvent::KEYCODE_F1, ScanKeyCode::F1_SCAN_CODE},
    {KeyEvent::KEYCODE_F2, ScanKeyCode::F2_SCAN_CODE},
    {KeyEvent::KEYCODE_F3, ScanKeyCode::F3_SCAN_CODE},
    {KeyEvent::KEYCODE_F4, ScanKeyCode::F4_SCAN_CODE},
    {KeyEvent::KEYCODE_F5, ScanKeyCode::F5_SCAN_CODE},
    {KeyEvent::KEYCODE_F6, ScanKeyCode::F6_SCAN_CODE},
    {KeyEvent::KEYCODE_F7, ScanKeyCode::F7_SCAN_CODE},
    {KeyEvent::KEYCODE_F8, ScanKeyCode::F8_SCAN_CODE},
    {KeyEvent::KEYCODE_F9, ScanKeyCode::F9_SCAN_CODE},
    {KeyEvent::KEYCODE_F10, ScanKeyCode::F10_SCAN_CODE},
    {KeyEvent::KEYCODE_NUM_LOCK, ScanKeyCode::NUMLOCK_SCAN_CODE},
    {KeyEvent::KEYCODE_SCROLL_LOCK, ScanKeyCode::SCROLLLOCK_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_7, ScanKeyCode::NUMPAD7_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_8, ScanKeyCode::NUMPAD8_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_9, ScanKeyCode::NUMPAD9_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_SUBTRACT, ScanKeyCode::NUMPADSUBTRACT_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_4, ScanKeyCode::NUMPAD4_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_5, ScanKeyCode::NUMPAD5_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_6, ScanKeyCode::NUMPAD6_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_ADD, ScanKeyCode::NUMPADADD_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_1, ScanKeyCode::NUMPAD1_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_2, ScanKeyCode::NUMPAD2_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_3, ScanKeyCode::NUMPAD3_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_0, ScanKeyCode::NUMPAD0_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_DOT, ScanKeyCode::NUMPADDECIMAL_SCAN_CODE},
    {KeyEvent::KEYCODE_F11, ScanKeyCode::F11_SCAN_CODE},
    {KeyEvent::KEYCODE_F12, ScanKeyCode::F12_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_ENTER, ScanKeyCode::NUMPADENTER_SCAN_CODE},
    {KeyEvent::KEYCODE_CTRL_RIGHT, ScanKeyCode::CONTROLRIGHT_SCAN_CODE},
    {KeyEvent::KEYCODE_NUMPAD_DIVIDE, ScanKeyCode::NUMPADDIVIDE_SCAN_CODE},
    {KeyEvent::KEYCODE_SYSRQ, ScanKeyCode::PRINTSCREEN_SCAN_CODE},
    {KeyEvent::KEYCODE_ALT_RIGHT, ScanKeyCode::ALTRIGHT_SCAN_CODE},
    {KeyEvent::KEYCODE_DPAD_UP, ScanKeyCode::UP_SCAN_CODE},
    {KeyEvent::KEYCODE_DPAD_LEFT, ScanKeyCode::LEFT_SCAN_CODE},
    {KeyEvent::KEYCODE_DPAD_RIGHT, ScanKeyCode::RIGHT_SCAN_CODE},
    {KeyEvent::KEYCODE_DPAD_DOWN, ScanKeyCode::DOWN_SCAN_CODE},
    {KeyEvent::KEYCODE_PAGE_UP, ScanKeyCode::PAGE_UP_SCAN_CODE},
    {KeyEvent::KEYCODE_PAGE_DOWN, ScanKeyCode::PAGE_DOWN_SCAN_CODE},
    {KeyEvent::KEYCODE_MOVE_HOME, ScanKeyCode::HOME_SCAN_CODE},
    {KeyEvent::KEYCODE_MOVE_END, ScanKeyCode::END_SCAN_CODE},
    {KeyEvent::KEYCODE_INSERT, ScanKeyCode::INSERT_SCAN_CODE},
    {KeyEvent::KEYCODE_FORWARD_DEL, ScanKeyCode::DELETE_SCAN_CODE},
    {KeyEvent::KEYCODE_BREAK, ScanKeyCode::PAUSE_SCAN_CODE},
    {KeyEvent::KEYCODE_META_LEFT, ScanKeyCode::METALEFT_SCAN_CODE},
    {KeyEvent::KEYCODE_META_RIGHT, ScanKeyCode::METARIGHT_SCAN_CODE},
};

const std::unordered_map<std::string, std::unordered_map<int, int>>
    keyValueConverter = {
        {"keycode", keycodeConverter},
        {"keyaction", keyactionConverter},
        {"mousebutton", mousebuttonConverter},
        {"keyscancode", keyScanCodeConverter},
        {"ohoskeycode", ohosKeyCodeConverter},
        {"ohoskeyaction", ohosKeyActionConverter},
};

uint32_t NWebInputDelegate::GetMouseButtonModifiers(const cef_mouse_button_type_t& button) {
  switch (button) {
    case MBT_LEFT:
      return EVENTFLAG_LEFT_MOUSE_BUTTON;
    case MBT_MIDDLE:
      return EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    case MBT_RIGHT:
      return EVENTFLAG_RIGHT_MOUSE_BUTTON;
    case MBT_BACK:
      return EVENTFLAG_BACK_MOUSE_BUTTON;
    case MBT_FORWARD:
      return EVENTFLAG_FORWARD_MOUSE_BUTTON;
    default:
      break;
  }
  return 0;
}

uint32_t NWebInputDelegate::GetWebModifiers(int32_t keyCode, int32_t keyAction,
                                            const std::vector<int32_t>& pressedCodes) {
  uint32_t result = 0;
  if (!keyAction) {
    if (keyCode == static_cast<int32_t>(KeyEvent::KEYCODE_CTRL_LEFT) ||
        keyCode == static_cast<int32_t>(KeyEvent::KEYCODE_CTRL_RIGHT)) {
      result |= EVENTFLAG_CONTROL_DOWN;
    }
    if (keyCode == static_cast<int32_t>(KeyEvent::KEYCODE_SHIFT_LEFT) ||
        keyCode == static_cast<int32_t>(KeyEvent::KEYCODE_SHIFT_RIGHT)) {
      result |= EVENTFLAG_SHIFT_DOWN;
    }
    if (keyCode == static_cast<int32_t>(KeyEvent::KEYCODE_ALT_LEFT) ||
        keyCode == static_cast<int32_t>(KeyEvent::KEYCODE_ALT_RIGHT)) {
      result |= EVENTFLAG_ALT_DOWN;
    }
  }

  for (auto pCode : pressedCodes) {
      if (pCode == static_cast<int32_t>(KeyEvent::KEYCODE_CTRL_LEFT) ||
          pCode == static_cast<int32_t>(KeyEvent::KEYCODE_CTRL_RIGHT)) {
        result |= EVENTFLAG_CONTROL_DOWN;
      }
      if (pCode == static_cast<int32_t>(KeyEvent::KEYCODE_SHIFT_LEFT) ||
          pCode == static_cast<int32_t>(KeyEvent::KEYCODE_SHIFT_RIGHT)) {
        result |= EVENTFLAG_SHIFT_DOWN;
      }
      if (pCode == static_cast<int32_t>(KeyEvent::KEYCODE_ALT_LEFT) ||
          pCode == static_cast<int32_t>(KeyEvent::KEYCODE_ALT_RIGHT)) {
        result |= EVENTFLAG_ALT_DOWN;
      }
  }
  return result;
}

uint32_t NWebInputDelegate::GetWebModifiersByPressedCode(
  const std::vector<int32_t>& pressedCodes) {
  uint32_t result = 0;
  for (auto pCode : pressedCodes) {
      if (pCode == static_cast<int32_t>(KeyEvent::KEYCODE_CTRL_LEFT) ||
          pCode == static_cast<int32_t>(KeyEvent::KEYCODE_CTRL_RIGHT)) {
        result |= EVENTFLAG_CONTROL_DOWN;
      }
      if (pCode == static_cast<int32_t>(KeyEvent::KEYCODE_SHIFT_LEFT) ||
          pCode == static_cast<int32_t>(KeyEvent::KEYCODE_SHIFT_RIGHT)) {
        result |= EVENTFLAG_SHIFT_DOWN;
      }
      if (pCode == static_cast<int32_t>(KeyEvent::KEYCODE_ALT_LEFT) ||
          pCode == static_cast<int32_t>(KeyEvent::KEYCODE_ALT_RIGHT)) {
        result |= EVENTFLAG_ALT_DOWN;
      }
  }
  return result;
}

NWebInputDelegate::NWebInputDelegate() {
  keyEventHandle_.RegistInputEvent(KeyEvent::KEYCODE_CTRL_LEFT, KEY_DOWN,
                                   false);
  keyEventHandle_.RegistInputEvent(KeyEvent::KEYCODE_CTRL_RIGHT, KEY_DOWN,
                                   false);
  keyEventHandle_.RegistInputEvent(KeyEvent::KEYCODE_SHIFT_LEFT, KEY_DOWN,
                                   false);
  keyEventHandle_.RegistInputEvent(KeyEvent::KEYCODE_SHIFT_RIGHT, KEY_DOWN,
                                   false);
  keyEventHandle_.RegistInputEvent(KeyEvent::KEYCODE_ALT_LEFT, KEY_DOWN, false);
  keyEventHandle_.RegistInputEvent(KeyEvent::KEYCODE_ALT_RIGHT, KEY_DOWN,
                                   false);
  keyEventHandle_.ClearInputEvent();
}

bool NWebInputDelegate::KeyValueConvert(const std::string keyValue,
                                        std::unordered_map<int, int>& map) {
  auto itKeyValue = keyValueConverter.find(keyValue);
  if (itKeyValue == keyValueConverter.end()) {
    return false;
  }
  map = itKeyValue->second;
  return true;
}

int NWebInputDelegate::CefConverter(const std::string keyValue, int input) {
  std::unordered_map<int, int> itConverter;
  if (KeyValueConvert(keyValue, itConverter) == false) {
    return -1;
  }
  auto item = itConverter.find(input);
  if (item == itConverter.end()) {
    return -1;
  }
  return item->second;
}

int NWebInputDelegate::OhosConverter(const std::string keyValue, int input) {
  std::unordered_map<int, int> itConverter;
  if (KeyValueConvert(keyValue, itConverter) == false) {
    return -1;
  }
  auto item = itConverter.find(input);
  if (item == itConverter.end()) {
    return -1;
  }
  return item->first;
}

void NWebInputDelegate::SetModifiers(int keyCode, int keyAction) {
  keyEventHandle_.SetInputEvent(keyCode, keyAction);
}

uint32_t NWebInputDelegate::GetModifiers() {
  uint32_t result = 0;
  if (keyEventHandle_.GetInputEvent(KeyEvent::KEYCODE_CTRL_LEFT) ||
      keyEventHandle_.GetInputEvent(KeyEvent::KEYCODE_CTRL_RIGHT)) {
    result |= EVENTFLAG_CONTROL_DOWN;
  }
  if (keyEventHandle_.GetInputEvent(KeyEvent::KEYCODE_SHIFT_LEFT) ||
      keyEventHandle_.GetInputEvent(KeyEvent::KEYCODE_SHIFT_RIGHT)) {
    result |= EVENTFLAG_SHIFT_DOWN;
  }
  if (keyEventHandle_.GetInputEvent(KeyEvent::KEYCODE_ALT_LEFT) ||
      keyEventHandle_.GetInputEvent(KeyEvent::KEYCODE_ALT_RIGHT)) {
    result |= EVENTFLAG_ALT_DOWN;
  }
  return result;
}

uint32_t NWebInputDelegate::GetModifiers(cef_mouse_button_type_t button) {
  return (GetModifiers() | GetMouseButtonModifiers(button));
}

uint32_t NWebInputDelegate::GetWebMouseModifiersByPressedCode(cef_mouse_button_type_t button,
                                                              const std::vector<int32_t>& pressedCodes) {
  return (GetWebModifiersByPressedCode(pressedCodes) | GetMouseButtonModifiers(button));
}

bool NWebInputDelegate::IsMMIKeyEvent(int32_t keyCode) {
  auto item = find(mmiKeyEvent.begin(), mmiKeyEvent.end(), keyCode);
  if (item == mmiKeyEvent.end()) {
    return false;
  }
  return true;
}
