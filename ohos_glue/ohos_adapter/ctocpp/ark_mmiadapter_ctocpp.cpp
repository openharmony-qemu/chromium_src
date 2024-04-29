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

#include "ohos_adapter/ctocpp/ark_mmiadapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_mmidevice_info_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_mmiinput_listener_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_mmilistener_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
char *ArkMMIAdapterCToCpp::KeyCodeToString(int32_t keyCode) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_mmiadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, key_code_to_string, nullptr);

  // Execute
  return _struct->key_code_to_string(_struct, keyCode);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMMIAdapterCToCpp::RegisterMMIInputListener(
    ArkWebRefPtr<ArkMMIInputListenerAdapter> eventCallback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_mmiadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_mmiinput_listener, 0);

  // Execute
  return _struct->register_mmiinput_listener(
      _struct, ArkMMIInputListenerAdapterCppToC::Invert(eventCallback));
}

ARK_WEB_NO_SANITIZE
void ArkMMIAdapterCToCpp::UnregisterMMIInputListener(int32_t monitorId) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_mmiadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, unregister_mmiinput_listener, );

  // Execute
  _struct->unregister_mmiinput_listener(_struct, monitorId);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMMIAdapterCToCpp::RegisterDevListener(
    ArkWebString type, ArkWebRefPtr<ArkMMIListenerAdapter> listener) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_mmiadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_dev_listener, 0);

  // Execute
  return _struct->register_dev_listener(
      _struct, type, ArkMMIListenerAdapterCppToC::Invert(listener));
}

ARK_WEB_NO_SANITIZE
int32_t ArkMMIAdapterCToCpp::UnregisterDevListener(ArkWebString type) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_mmiadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, unregister_dev_listener, 0);

  // Execute
  return _struct->unregister_dev_listener(_struct, type);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMMIAdapterCToCpp::GetKeyboardType(int32_t deviceId, int32_t &type) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_mmiadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_keyboard_type, 0);

  // Execute
  return _struct->get_keyboard_type(_struct, deviceId, &type);
}

ARK_WEB_NO_SANITIZE
int32_t ArkMMIAdapterCToCpp::GetDeviceIds(ArkWebInt32Vector &ids) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_mmiadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_device_ids, 0);

  // Execute
  return _struct->get_device_ids(_struct, &ids);
}

ARK_WEB_NO_SANITIZE
int32_t
ArkMMIAdapterCToCpp::GetDeviceInfo(int32_t deviceId,
                                   ArkWebRefPtr<ArkMMIDeviceInfoAdapter> info) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_mmiadapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_device_info, 0);

  // Execute
  return _struct->get_device_info(_struct, deviceId,
                                  ArkMMIDeviceInfoAdapterCppToC::Invert(info));
}

ArkMMIAdapterCToCpp::ArkMMIAdapterCToCpp() {
}

ArkMMIAdapterCToCpp::~ArkMMIAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<ArkMMIAdapterCToCpp, ArkMMIAdapter,
                                        ark_mmiadapter_t>::kBridgeType =
    ARK_MMIADAPTER;

} // namespace OHOS::ArkWeb
