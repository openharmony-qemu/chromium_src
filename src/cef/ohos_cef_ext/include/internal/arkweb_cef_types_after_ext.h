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

#ifndef ARKWEB_INCLUDE_CEF_TYPES_AFTER_EXT_H_
#define ARKWEB_INCLUDE_CEF_TYPES_AFTER_EXT_H_

#if BUILDFLAG(ARKWEB_NOTIFICATION)
// Permission status query callback.
typedef base::RepeatingCallback<void(int32_t)> cef_permission_status_query_callback_t;
#endif // ARKWEB_NOTIFICATION

#if BUILDFLAG(IS_ARKWEB)
///
// Structure text input state info.
///
typedef struct _cef_text_input_info_t {
  int node_id = 0;
  bool show_keyboard = false;
  cef_text_input_mode_t input_mode = CEF_TEXT_INPUT_MODE_NONE;
  cef_text_input_type_t input_type = CEF_TEXT_INPUT_TYPE_NONE;
  cef_text_input_action_t input_action = CEF_TEXT_INPUT_ACTION_DEFAULT;
  cef_text_input_flags_t input_flags = CEF_TEXT_INPUT_FLAG_NONE;
  bool always_hide_ime = false;
} cef_text_input_info_t;
#endif  // BUILDFLAG(IS_ARKWEB)

#endif  // ARKWEB_INCLUDE_CEF_TYPES_AFTER_EXT_H_
