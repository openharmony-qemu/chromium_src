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

#include "ohos_adapter/ctocpp/ark_display_manager_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_display_listener_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_display_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
uint64_t ArkDisplayManagerAdapterCToCpp::GetDefaultDisplayId() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_default_display_id, 0);

  // Execute
  return _struct->get_default_display_id(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkDisplayAdapter>
ArkDisplayManagerAdapterCToCpp::GetDefaultDisplay() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_default_display, nullptr);

  // Execute
  ark_display_adapter_t *_retval = _struct->get_default_display(_struct);

  // Return type: refptr_same
  return ArkDisplayAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkDisplayManagerAdapterCToCpp::RegisterDisplayListener(
    ArkWebRefPtr<ArkDisplayListenerAdapter> listener) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_display_listener, 0);

  // Execute
  return _struct->register_display_listener(
      _struct, ArkDisplayListenerAdapterCppToC::Invert(listener));
}

ARK_WEB_NO_SANITIZE
bool ArkDisplayManagerAdapterCToCpp::UnregisterDisplayListener(uint32_t id) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, unregister_display_listener, false);

  // Execute
  return _struct->unregister_display_listener(_struct, id);
}

ARK_WEB_NO_SANITIZE
bool ArkDisplayManagerAdapterCToCpp::IsDefaultPortrait() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_display_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_default_portrait, false);

  // Execute
  return _struct->is_default_portrait(_struct);
}

ArkDisplayManagerAdapterCToCpp::ArkDisplayManagerAdapterCToCpp() {
}

ArkDisplayManagerAdapterCToCpp::~ArkDisplayManagerAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkDisplayManagerAdapterCToCpp, ArkDisplayManagerAdapter,
    ark_display_manager_adapter_t>::kBridgeType = ARK_DISPLAY_MANAGER_ADAPTER;

} // namespace OHOS::ArkWeb
