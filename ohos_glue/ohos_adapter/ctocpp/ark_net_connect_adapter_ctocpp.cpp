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

#include "ohos_adapter/ctocpp/ark_net_connect_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_net_conn_callback_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkNetConnectAdapterCToCpp::RegisterNetConnCallback(
    ArkWebRefPtr<ArkNetConnCallback> cb) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_net_connect_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_net_conn_callback, 0);

  // Execute
  return _struct->register_net_conn_callback(
      _struct, ArkNetConnCallbackCppToC::Invert(cb));
}

ARK_WEB_NO_SANITIZE
int32_t ArkNetConnectAdapterCToCpp::UnregisterNetConnCallback(int32_t id) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_net_connect_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, unregister_net_conn_callback, 0);

  // Execute
  return _struct->unregister_net_conn_callback(_struct, id);
}

ARK_WEB_NO_SANITIZE
int32_t
ArkNetConnectAdapterCToCpp::GetDefaultNetConnect(uint32_t &type,
                                                 uint32_t &netConnectSubtype) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_net_connect_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_default_net_connect, 0);

  // Execute
  return _struct->get_default_net_connect(_struct, &type, &netConnectSubtype);
}

ARK_WEB_NO_SANITIZE
ArkWebStringVector ArkNetConnectAdapterCToCpp::GetDnsServers() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_net_connect_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_vector_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_dns_servers,
                                   ark_web_string_vector_default);

  // Execute
  return _struct->get_dns_servers(_struct);
}

ArkNetConnectAdapterCToCpp::ArkNetConnectAdapterCToCpp() {
}

ArkNetConnectAdapterCToCpp::~ArkNetConnectAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkNetConnectAdapterCToCpp, ArkNetConnectAdapter,
                           ark_net_connect_adapter_t>::kBridgeType =
        ARK_NET_CONNECT_ADAPTER;

} // namespace OHOS::ArkWeb
