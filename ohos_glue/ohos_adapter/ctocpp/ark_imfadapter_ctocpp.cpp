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

#include "ohos_adapter/ctocpp/ark_imfadapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_imfcursor_info_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_imftext_config_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_imftext_listener_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkIMFAdapterCToCpp::Attach(
    ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_imfadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, attach1, false);

  // Execute
  return _struct->attach1(_struct,
                          ArkIMFTextListenerAdapterCppToC::Invert(listener),
                          isShowKeyboard);
}

ARK_WEB_NO_SANITIZE
bool ArkIMFAdapterCToCpp::Attach(
    ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard,
    ArkWebRefPtr<ArkIMFTextConfigAdapter> config) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_imfadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, attach2, false);

  // Execute
  return _struct->attach2(
      _struct, ArkIMFTextListenerAdapterCppToC::Invert(listener),
      isShowKeyboard, ArkIMFTextConfigAdapterCppToC::Invert(config));
}

ARK_WEB_NO_SANITIZE
void ArkIMFAdapterCToCpp::ShowCurrentInput(const int32_t &inputType) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_imfadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, show_current_input, );

  // Execute
  _struct->show_current_input(_struct, &inputType);
}

ARK_WEB_NO_SANITIZE
void ArkIMFAdapterCToCpp::HideTextInput() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_imfadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, hide_text_input, );

  // Execute
  _struct->hide_text_input(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkIMFAdapterCToCpp::Close() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_imfadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, close, );

  // Execute
  _struct->close(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkIMFAdapterCToCpp::OnCursorUpdate(
    ArkWebRefPtr<ArkIMFCursorInfoAdapter> cursorInfo) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_imfadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_cursor_update, );

  // Execute
  _struct->on_cursor_update(_struct,
                            ArkIMFCursorInfoAdapterCppToC::Invert(cursorInfo));
}

ARK_WEB_NO_SANITIZE
void ArkIMFAdapterCToCpp::OnSelectionChange(ArkWebU16String &text, int start,
                                            int end) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_imfadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_selection_change, );

  // Execute
  _struct->on_selection_change(_struct, &text, start, end);
}

ArkIMFAdapterCToCpp::ArkIMFAdapterCToCpp() {
}

ArkIMFAdapterCToCpp::~ArkIMFAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<ArkIMFAdapterCToCpp, ArkIMFAdapter,
                                        ark_imfadapter_t>::kBridgeType =
    ARK_IMFADAPTER;

} // namespace OHOS::ArkWeb
