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

#include "ohos_adapter/cpptoc/ark_net_conn_callback_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK
ark_net_conn_callback_net_available(struct _ark_net_conn_callback_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkNetConnCallbackCppToC::Get(self)->NetAvailable();
}

int32_t ARK_WEB_CALLBACK ark_net_conn_callback_net_capabilities_change(
    struct _ark_net_conn_callback_t *self, const uint32_t *netConnectType,
    const uint32_t *netConnectSubtype) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(netConnectType, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(netConnectSubtype, 0);

  // Execute
  return ArkNetConnCallbackCppToC::Get(self)->NetCapabilitiesChange(
      *netConnectType, *netConnectSubtype);
}

int32_t ARK_WEB_CALLBACK ark_net_conn_callback_net_connection_properties_change(
    struct _ark_net_conn_callback_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkNetConnCallbackCppToC::Get(self)->NetConnectionPropertiesChange();
}

int32_t ARK_WEB_CALLBACK
ark_net_conn_callback_net_unavailable(struct _ark_net_conn_callback_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkNetConnCallbackCppToC::Get(self)->NetUnavailable();
}

} // namespace

ArkNetConnCallbackCppToC::ArkNetConnCallbackCppToC() {
  GetStruct()->net_available = ark_net_conn_callback_net_available;
  GetStruct()->net_capabilities_change =
      ark_net_conn_callback_net_capabilities_change;
  GetStruct()->net_connection_properties_change =
      ark_net_conn_callback_net_connection_properties_change;
  GetStruct()->net_unavailable = ark_net_conn_callback_net_unavailable;
}

ArkNetConnCallbackCppToC::~ArkNetConnCallbackCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkNetConnCallbackCppToC, ArkNetConnCallback,
                           ark_net_conn_callback_t>::kBridgeType =
        ARK_NET_CONN_CALLBACK;

} // namespace OHOS::ArkWeb
