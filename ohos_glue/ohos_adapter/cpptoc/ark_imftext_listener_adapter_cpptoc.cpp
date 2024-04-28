/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_adapter/cpptoc/ark_imftext_listener_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/ctocpp/ark_imfadapter_function_key_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_insert_text(
    struct _ark_imftext_listener_adapter_t *self, const ArkWebU16String *text) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(text, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->InsertText(*text);
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_delete_forward(
    struct _ark_imftext_listener_adapter_t *self, int32_t length) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->DeleteForward(length);
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_delete_backward(
    struct _ark_imftext_listener_adapter_t *self, int32_t length) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->DeleteBackward(length);
}

void ARK_WEB_CALLBACK
ark_imftext_listener_adapter_send_key_event_from_input_method(
    struct _ark_imftext_listener_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->SendKeyEventFromInputMethod();
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_send_keyboard_status(
    struct _ark_imftext_listener_adapter_t *self,
    const int32_t *keyboardStatus) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(keyboardStatus, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->SendKeyboardStatus(
      *keyboardStatus);
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_send_function_key(
    struct _ark_imftext_listener_adapter_t *self,
    ark_imfadapter_function_key_adapter_t *functionKey) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->SendFunctionKey(
      ArkIMFAdapterFunctionKeyAdapterCToCpp::Invert(functionKey));
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_set_keyboard_status(
    struct _ark_imftext_listener_adapter_t *self, bool status) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->SetKeyboardStatus(status);
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_move_cursor(
    struct _ark_imftext_listener_adapter_t *self, const uint32_t direction) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->MoveCursor(direction);
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_handle_set_selection(
    struct _ark_imftext_listener_adapter_t *self, int32_t start, int32_t end) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->HandleSetSelection(start, end);
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_handle_extend_action(
    struct _ark_imftext_listener_adapter_t *self, int32_t action) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->HandleExtendAction(action);
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_handle_select(
    struct _ark_imftext_listener_adapter_t *self, int32_t keyCode,
    int32_t cursorMoveSkip) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->HandleSelect(keyCode,
                                                           cursorMoveSkip);
}

int32_t ARK_WEB_CALLBACK ark_imftext_listener_adapter_get_text_index_at_cursor(
    struct _ark_imftext_listener_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkIMFTextListenerAdapterCppToC::Get(self)->GetTextIndexAtCursor();
}

ArkWebU16String ARK_WEB_CALLBACK
ark_imftext_listener_adapter_get_left_text_of_cursor(
    struct _ark_imftext_listener_adapter_t *self, int32_t number) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_u16string_default);

  // Execute
  return ArkIMFTextListenerAdapterCppToC::Get(self)->GetLeftTextOfCursor(
      number);
}

ArkWebU16String ARK_WEB_CALLBACK
ark_imftext_listener_adapter_get_right_text_of_cursor(
    struct _ark_imftext_listener_adapter_t *self, int32_t number) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_u16string_default);

  // Execute
  return ArkIMFTextListenerAdapterCppToC::Get(self)->GetRightTextOfCursor(
      number);
}

int32_t ARK_WEB_CALLBACK ark_imftext_listener_adapter_set_preview_text(
    struct _ark_imftext_listener_adapter_t *self, const ArkWebU16String *text,
    int32_t start, int32_t end) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(text, 0);

  // Execute
  return ArkIMFTextListenerAdapterCppToC::Get(self)->SetPreviewText(*text,
                                                                    start, end);
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_finish_text_preview(
    struct _ark_imftext_listener_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->FinishTextPreview();
}

void ARK_WEB_CALLBACK ark_imftext_listener_adapter_set_need_under_line(
    struct _ark_imftext_listener_adapter_t *self, bool isNeedUnderline) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIMFTextListenerAdapterCppToC::Get(self)->SetNeedUnderLine(isNeedUnderline);
}

} // namespace

ArkIMFTextListenerAdapterCppToC::ArkIMFTextListenerAdapterCppToC() {
  GetStruct()->insert_text = ark_imftext_listener_adapter_insert_text;
  GetStruct()->delete_forward = ark_imftext_listener_adapter_delete_forward;
  GetStruct()->delete_backward = ark_imftext_listener_adapter_delete_backward;
  GetStruct()->send_key_event_from_input_method =
      ark_imftext_listener_adapter_send_key_event_from_input_method;
  GetStruct()->send_keyboard_status =
      ark_imftext_listener_adapter_send_keyboard_status;
  GetStruct()->send_function_key =
      ark_imftext_listener_adapter_send_function_key;
  GetStruct()->set_keyboard_status =
      ark_imftext_listener_adapter_set_keyboard_status;
  GetStruct()->move_cursor = ark_imftext_listener_adapter_move_cursor;
  GetStruct()->handle_set_selection =
      ark_imftext_listener_adapter_handle_set_selection;
  GetStruct()->handle_extend_action =
      ark_imftext_listener_adapter_handle_extend_action;
  GetStruct()->handle_select = ark_imftext_listener_adapter_handle_select;
  GetStruct()->get_text_index_at_cursor =
      ark_imftext_listener_adapter_get_text_index_at_cursor;
  GetStruct()->get_left_text_of_cursor =
      ark_imftext_listener_adapter_get_left_text_of_cursor;
  GetStruct()->get_right_text_of_cursor =
      ark_imftext_listener_adapter_get_right_text_of_cursor;
  GetStruct()->set_preview_text = ark_imftext_listener_adapter_set_preview_text;
  GetStruct()->finish_text_preview =
      ark_imftext_listener_adapter_finish_text_preview;
  GetStruct()->set_need_under_line =
      ark_imftext_listener_adapter_set_need_under_line;
}

ArkIMFTextListenerAdapterCppToC::~ArkIMFTextListenerAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkIMFTextListenerAdapterCppToC, ArkIMFTextListenerAdapter,
    ark_imftext_listener_adapter_t>::kBridgeType = ARK_IMFTEXT_LISTENER_ADAPTER;

} // namespace OHOS::ArkWeb
