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

#include "ohos_adapter/ctocpp/ark_paste_board_client_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_paste_board_observer_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkPasteBoardClientAdapterCToCpp::GetPasteData(
    ArkPasteRecordVector &data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_paste_data, false);

  // Execute
  return _struct->get_paste_data(_struct, &data);
}

ARK_WEB_NO_SANITIZE
void ArkPasteBoardClientAdapterCToCpp::SetPasteData(ArkPasteRecordVector &data,
                                                    int32_t copyOption) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_paste_data, );

  // Execute
  _struct->set_paste_data(_struct, &data, copyOption);
}

ARK_WEB_NO_SANITIZE
bool ArkPasteBoardClientAdapterCToCpp::HasPasteData() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, has_paste_data, false);

  // Execute
  return _struct->has_paste_data(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkPasteBoardClientAdapterCToCpp::Clear() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, clear, );

  // Execute
  _struct->clear(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t
ArkPasteBoardClientAdapterCToCpp::OpenRemoteUri(const ArkWebString &path) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, open_remote_uri, 0);

  // Execute
  return _struct->open_remote_uri(_struct, &path);
}

ARK_WEB_NO_SANITIZE
bool ArkPasteBoardClientAdapterCToCpp::IsLocalPaste() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_local_paste, false);

  // Execute
  return _struct->is_local_paste(_struct);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkPasteBoardClientAdapterCToCpp::GetTokenId() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_token_id, 0);

  // Execute
  return _struct->get_token_id(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPasteBoardClientAdapterCToCpp::AddPasteboardChangedObserver(
    ArkWebRefPtr<ArkPasteBoardObserverAdapter> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, add_pasteboard_changed_observer, 0);

  // Execute
  return _struct->add_pasteboard_changed_observer(
      _struct, ArkPasteBoardObserverAdapterCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkPasteBoardClientAdapterCToCpp::RemovePasteboardChangedObserver(
    int32_t callbackId) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_board_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   remove_pasteboard_changed_observer, );

  // Execute
  _struct->remove_pasteboard_changed_observer(_struct, callbackId);
}

ArkPasteBoardClientAdapterCToCpp::ArkPasteBoardClientAdapterCToCpp() {
}

ArkPasteBoardClientAdapterCToCpp::~ArkPasteBoardClientAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkPasteBoardClientAdapterCToCpp, ArkPasteBoardClientAdapter,
    ark_paste_board_client_adapter_t>::kBridgeType =
    ARK_PASTE_BOARD_CLIENT_ADAPTER;

} // namespace OHOS::ArkWeb
