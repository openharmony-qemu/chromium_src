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

#include "ohos_adapter/cpptoc/ark_net_proxy_event_callback_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_net_proxy_event_callback_adapter_changed(
    struct _ark_net_proxy_event_callback_adapter_t *self,
    const ArkWebString *host, const uint16_t *port, const ArkWebString *pacUrl,
    const ArkWebStringVector *exclusionList) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(host, );

  ARK_WEB_CPPTOC_CHECK_PARAM(port, );

  ARK_WEB_CPPTOC_CHECK_PARAM(pacUrl, );

  ARK_WEB_CPPTOC_CHECK_PARAM(exclusionList, );

  // Execute
  ArkNetProxyEventCallbackAdapterCppToC::Get(self)->Changed(
      *host, *port, *pacUrl, *exclusionList);
}

} // namespace

ArkNetProxyEventCallbackAdapterCppToC::ArkNetProxyEventCallbackAdapterCppToC() {
  GetStruct()->changed = ark_net_proxy_event_callback_adapter_changed;
}

ArkNetProxyEventCallbackAdapterCppToC::
    ~ArkNetProxyEventCallbackAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkNetProxyEventCallbackAdapterCppToC, ArkNetProxyEventCallbackAdapter,
    ark_net_proxy_event_callback_adapter_t>::kBridgeType =
    ARK_NET_PROXY_EVENT_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb
