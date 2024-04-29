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

#include "ohos_adapter/cpptoc/ark_ibuffer_consumer_listener_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/ctocpp/ark_surface_buffer_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_ibuffer_consumer_listener_adapter_on_buffer_available(
    struct _ark_ibuffer_consumer_listener_adapter_t *self,
    ark_surface_buffer_adapter_t *buffer) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkIBufferConsumerListenerAdapterCppToC::Get(self)->OnBufferAvailable(
      ArkSurfaceBufferAdapterCToCpp::Invert(buffer));
}

} // namespace

ArkIBufferConsumerListenerAdapterCppToC::
    ArkIBufferConsumerListenerAdapterCppToC() {
  GetStruct()->on_buffer_available =
      ark_ibuffer_consumer_listener_adapter_on_buffer_available;
}

ArkIBufferConsumerListenerAdapterCppToC::
    ~ArkIBufferConsumerListenerAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkIBufferConsumerListenerAdapterCppToC, ArkIBufferConsumerListenerAdapter,
    ark_ibuffer_consumer_listener_adapter_t>::kBridgeType =
    ARK_IBUFFER_CONSUMER_LISTENER_ADAPTER;

} // namespace OHOS::ArkWeb
