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

#ifndef ARKWEB_INCLUDE_CEF_TYPES_PRE_EXT_H_
#define ARKWEB_INCLUDE_CEF_TYPES_PRE_EXT_H_

#if BUILDFLAG(IS_ARKWEB)
#include <map>
#include "build/build_config.h"
#include "cef/include/base/cef_callback.h"
#endif  // BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(IS_ARKWEB)
///
// Screen orientation types.
///
typedef enum {
  UNDEFINED,
  PORTRAIT_PRIMARY,
  PORTRAIT_SECONDARY,
  LANDSCAPE_PRIMARY,
  LANDSCAPE_SECONDARY
} cef_screen_orientation_type_t;
#endif  // BUILDFLAG(IS_ARKWEB)
// Permission request callback.
typedef base::RepeatingCallback<void(bool)> cef_permission_callback_t;

// Web snapshot callback.
typedef base::OnceCallback<void(const char*, bool, void*, int, int)>
    cef_web_snapshot_callback_t;

// cef accelerated widget type
typedef uint32_t cef_accelerated_widget_t;

// cef native window type
typedef void* cef_native_window_t;
// cef native window type
typedef void* cef_native_window_t;
///
// Supported text direction. See text_direction.mojom.
///
typedef enum {
  ///
  /// type = unknown direction
  ///
  UNKNOWN,

  ///
  /// type = right to left
  ///
  RTL,

  ///
  /// type = left to right
  ///
  LTR,
} cef_text_direction_t;

///
// Supported <select> item type. See popup_menu.mojom.
///
typedef enum {
  ///
  /// type = kOption
  ///
  OPTION,

  ///
  /// type = kCheckableOption
  ///
  CHECKABLE_OPTION,

  ///
  /// type = kGruop
  ///
  GROUP,

  ///
  /// type = kSeparator
  ///
  SEPARATOR,

  ///
  /// type = kSubMenu
  ///
  SubMenu,
} cef_select_popup_item_type_t;

///
// Supported <select> item.
///
typedef struct _cef_select_popup_item_t {
  ///
  /// label name of item.
  ///
  cef_string_t label;

  ///
  /// tool tip of item.
  ///
  cef_string_t tool_tip;

  ///
  /// type of item.
  ///
  cef_select_popup_item_type_t type;

  ///
  /// action of item.
  ///
  uint32_t action;

  ///
  /// text direction of item.
  ///
  cef_text_direction_t text_direction;

  ///
  /// whether item is enabled.
  ///
  bool enabled;

  ///
  /// whether item has text direction overridel
  ///
  bool has_text_direction_override;

  ///
  /// whether item is checked.
  ///
  bool checked;
} cef_select_popup_item_t;

///
// Type of an <input>.
///
typedef enum {
  CEF_TEXT_INPUT_TYPE_NONE,
  CEF_TEXT_INPUT_TYPE_TEXT,
  CEF_TEXT_INPUT_TYPE_PASSWORD,
  CEF_TEXT_INPUT_TYPE_SEARCH,
  CEF_TEXT_INPUT_TYPE_EMAIL,
  CEF_TEXT_INPUT_TYPE_NUMBER,
  CEF_TEXT_INPUT_TYPE_TELEPHONE,
  CEF_TEXT_INPUT_TYPE_URL,
  CEF_TEXT_INPUT_TYPE_DATE,
  CEF_TEXT_INPUT_TYPE_DATE_TIME,
  CEF_TEXT_INPUT_TYPE_DATE_TIME_LOCAL,
  CEF_TEXT_INPUT_TYPE_MONTH,
  CEF_TEXT_INPUT_TYPE_TIME,
  CEF_TEXT_INPUT_TYPE_WEEK,
  CEF_TEXT_INPUT_TYPE_TEXT_AREA,
  CEF_TEXT_INPUT_TYPE_CONTENT_EDITABLE,
  CEF_TEXT_INPUT_TYPE_DATE_TIME_FIELD,
  CEF_TEXT_INPUT_TYPE_TYPE_NULL,
  CEF_TEXT_INPUT_TYPE_MAX = CEF_TEXT_INPUT_TYPE_TYPE_NULL,
} cef_text_input_type_t;

///
// Supported date time chooser.
///
typedef struct _cef_date_time_chooser_t {
  ///
  /// date time chooser type.
  ///
  cef_text_input_type_t dialog_type;

  ///
  /// select date time.
  ///
  double dialog_value;

  ///
  /// minimum date time.
  ///
  double minimum;

  ///
  /// minimum date time.
  ///
  double maximum;

  ///
  /// select step.
  ///
  double step;
} cef_date_time_chooser_t;

///
// Supported context menu input field types. These constants match their
// equivalents in Chromium's ContextMenuDataInputFieldType and should not be
// renumbered.
///
typedef enum {
  ///
  /// Not an input field.
  ///
  CM_INPUTFIELDTYPE_NONE,

  ///
  /// type = text, search, email, url
  ///
  CM_INPUTFIELDTYPE_PLAINTEXT,

  ///
  /// type = password
  ///
  CM_INPUTFIELDTYPE_PASSWORD,

  ///
  /// type = number
  ///
  CM_INPUTFIELDTYPE_NUMBER,

  ///
  /// type = tel
  ///
  CM_INPUTFIELDTYPE_TELEPHONE,

  ///
  /// type = <etc.>
  ///
  CM_INPUTFIELDTYPE_OTHER,
} cef_context_menu_input_field_type_t;

///
// Supported context menu source types. These constants match their equivalents
// in Chromium's ui::MenuSourceType and should not be renumbered.
///
typedef enum {
  ///
  /// type = none
  ///
  CM_SOURCETYPE_NONE,

  ///
  /// type = mouse
  ///
  CM_SOURCETYPE_MOUSE,

  ///
  /// type = keyboard
  ///
  CM_SOURCETYPE_KEYBOARD,

  ///
  /// type = touch
  ///
  CM_SOURCETYPE_TOUCH,

  ///
  /// type = touch edit menu
  ///
  CM_SOURCETYPE_TOUCH_EDIT_MENU,

  ///
  /// type = long press
  ///
  CM_SOURCETYPE_LONG_PRESS,

  ///
  /// type = long tap
  ///
  CM_SOURCETYPE_LONG_TAP,

  ///
  /// type = number
  ///
  CM_SOURCETYPE_TOUCH_HANDLE,

  ///
  /// type = stylus
  ///
  CM_SOURCETYPE_STYLUS,

  ///
  /// type = adjust selection
  ///
  CM_SOURCETYPE_ADJUST_SELECTION,

  ///
  /// type = selection reset
  ///
  CM_SOURCETYPE_SELECTION_RESET,
} cef_context_menu_source_type_t;


///
// Support date time suggestion.
///
typedef struct _cef_date_time_suggestion_t {
  ///
  /// The date/time value represented as a double.
  ///
  double value;

  ///
  /// The localized value to be shown to the user.
  ///
  cef_string_t localized_value;

  ///
  /// The label for the suggestion.
  ///
  cef_string_t label;
} cef_date_time_suggestion_t;

typedef enum {
  CAPTURE_INVAILD_MODE = -1,
  CAPTURE_HOME_SCREEN_MODE = 0,
  CAPTURE_SPECIFIED_SCREEN_MODE = 1,
  CAPTURE_SPECIFIED_WINDOW_MODE = 2
} cef_screen_capture_mode_t;

///
// Supported autofill item.
///
typedef struct _cef_autofill_popup_item_t {
  ///
  /// value name of item.
  ///
  cef_string_t label;

  ///
  /// sub label value of item.
  ///
  cef_string_t sublabel;

  ///
  /// id of item.
  ///
  uint32_t unique_id;
} cef_autofill_popup_item_t;

///
/// Media playing state.
///
typedef enum {
  ///
  /// media is playing
  ///
  PLAYING,

  ///
  /// media is paused
  ///
  PAUSE,

  ///
  /// media playing reached end of the stream
  ///
  END_OF_STREAM,

  ///
  /// media playing is interrupted because of media player gone
  ///
  PLAYER_GONE,
} cef_media_playing_state_t;

///
/// Media type.
///
typedef enum {
  VIDEO,
  AUDIO,
} cef_media_type_t;

///
// embed life change.
///
typedef enum {
  CREATE = 0,
  UPDATE = 1,
  DESTROY = 2,
  ENTER_BFCACHE = 3,
  LEAVE_BFCACHE = 4,
} cef_embed_life_change_t;

///
// Structure native embed data.
///
typedef struct _cef_native_embed_t {
  std::string id;
  int32_t width;
  int32_t height;
  std::string type;
  std::string src;
  std::string url;
  std::string tag;
  std::map<std::string, std::string> params;
  int32_t x;
  int32_t y;
} cef_native_embed_t;

///
// Structure native embed data.
///
typedef struct _cef_native_embed_data_t {
  cef_embed_life_change_t status;
  std::string surfaceId;
  std::string embedId;
  cef_native_embed_t info;
} cef_native_embed_data_t;

///
// Param status.
///
typedef enum {
  PARAM_ADD = 0,
  PARAM_UPDATE = 1,
  PARAM_DELETE = 2,
} cef_native_param_status_t;

///
// Structure native param item.
///
typedef struct _cef_native_param_item_t {
  cef_native_param_status_t status;
  std::string id;
  std::string name;
  std::string value;
} cef_native_param_item_t;

///
// Structure native param data.
///
typedef struct _cef_native_param_data_t {
  std::string embedId;
  std::string objectAttributeId;
  std::vector<cef_native_param_item_t> paramItems;
} cef_native_param_data_t;

///
// Touch type.
///
typedef enum {
  DOWN,
  UP,
  MOVE,
  CANCEL,
  PULL_DOWN,
  PULL_UP,
  PULL_MOVE,
  PULL_IN_WINDOW,
  PULL_OUT_WINDOW,
  TOUCH_UNKNOWN,
} cef_embed_touch_type_t;

///
// Structure embed touch data.
///
typedef struct _cef_embed_touch_event_t {
  std::string embedId;
  int32_t id = 0;
  float x = 0.0f;
  float y = 0.0f;
  float screenX = 0.0f;
  float screenY = 0.0f;
  cef_embed_touch_type_t type;
  float offsetX = 0.0f;
  float offsetY = 0.0f;
} cef_embed_touch_event_t;

///
// Navigation entry types.
///
typedef enum {
  NAVIGATION_TYPE_UNKNOWN = 0,
  NAVIGATION_TYPE_MAIN_FRAME_NEW_ENTRY = 1,
  NAVIGATION_TYPE_MAIN_FRAME_EXISTING_ENTRY = 2,
  NAVIGATION_TYPE_NEW_SUBFRAME = 4,
  NAVIGATION_TYPE_AUTO_SUBFRAME = 5,
} cef_navigation_entry_type_t;

///
// Type action of an <input>.
///
typedef enum {
  CEF_TEXT_INPUT_ACTION_DEFAULT,
  CEF_TEXT_INPUT_ACTION_ENTER,
  CEF_TEXT_INPUT_ACTION_DONE,
  CEF_TEXT_INPUT_ACTION_GO,
  CEF_TEXT_INPUT_ACTION_NEXT,
  CEF_TEXT_INPUT_ACTION_PREVIOUS,
  CEF_TEXT_INPUT_ACTION_SEARCH,
  CEF_TEXT_INPUT_ACTION_SEND,
  CEF_TEXT_INPUT_ACTION_MAX = CEF_TEXT_INPUT_ACTION_SEND,
} cef_text_input_action_t;

///
// Type flag of an <input>.
///
typedef enum {
  CEF_TEXT_INPUT_FLAG_NONE = 0,
  CEF_TEXT_INPUT_FLAG_AUTOCOMPLETE_ON = 1 << 0,
  CEF_TEXT_INPUT_FLAG_AUTOCOMPLETE_OFF = 1 << 1,
  CEF_TEXT_INPUT_FLAG_AUTOCORRECT_ON = 1 << 2,
  CEF_TEXT_INPUT_FLAG_AUTOCORRECT_OFF = 1 << 3,
  CEF_TEXT_INPUT_FLAG_SPELLCHECK_ON = 1 << 4,
  CEF_TEXT_INPUT_FLAG_SPELLCHECK_OFF = 1 << 5,
  CEF_TEXT_INPUT_FLAG_AUTOCAPITALIZE_NONE = 1 << 6,
  CEF_TEXT_INPUT_FLAG_AUTOCAPITALIZE_CHARACTERS = 1 << 7,
  CEF_TEXT_INPUT_FLAG_AUTOCAPITALIZE_WORDS = 1 << 8,
  CEF_TEXT_INPUT_FLAG_AUTOCAPITALIZE_SENTENCES = 1 << 9,
  CEF_TEXT_INPUT_FLAG_HAVE_NEXT_FOCUSABLE_ELEMENT = 1 << 10,
  CEF_TEXT_INPUT_FLAG_HAVE_PREVIOUS_FOCUSABLE_ELEMENT = 1 << 11,
  CEF_TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD = 1 << 12,
  CEF_TEXT_INPUT_FLAG_VERTICAL = 1 << 13
} cef_text_input_flags_t;

///
// Mouse type.
///
typedef enum {
    MOUSE_NONE,
    MOUSE_PRESS,
    MOUSE_RELEASE,
    MOUSE_MOVE,
    MOUSE_WINDOW_ENTER,
    MOUSE_WINDOW_LEAVE,
    MOUSE_HOVER,
    MOUSE_HOVER_ENTER,
    MOUSE_HOVER_MOVE,
    MOUSE_HOVER_EXIT,
    MOUSE_PULL_DOWN,
    MOUSE_PULL_MOVE,
    MOUSE_PULL_UP,
    MOUSE_CANCEL,
} cef_embed_mouse_type_t;

///
// Mouse Button.
///
typedef enum {
    MOUSE_NONE_BUTTON = 0,
    MOUSE_LEFT_BUTTON = 1,
    MOUSE_RIGHT_BUTTON = 1 << 1,
    MOUSE_MIDDLE_BUTTON = 1 << 2,
    MOUSE_BACK_BUTTON = 1 << 3,
    MOUSE_FORWARD_BUTTON = 1 << 4,
    MOUSE_SIDE_BUTTON = 1 << 5,
    MOUSE_EXTRA_BUTTON = 1 << 6,
    MOUSE_TASK_BUTTON = 1 << 7,
} cef_embed_mouse_button_t;

///
// Structure embed mouse data.
///
typedef struct _cef_embed_mouse_event_t {
    std::string embedId;
    float x = 0.0f;
    float y = 0.0f;
    float screenX = 0.0f;
    float screenY = 0.0f;
    cef_embed_mouse_type_t type;
    cef_embed_mouse_button_t button;
    bool isHitNativeArea = false;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
} cef_embed_mouse_event_t;
#endif  // ARKWEB_INCLUDE_CEF_TYPES_PRE_EXT_H_
