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

#include "ohos_adapter/cpptoc/ark_event_handler_fdlistener_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_event_handler_fdlistener_adapter_on_readable(
    struct _ark_event_handler_fdlistener_adapter_t *self,
    int32_t fileDescriptor) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkEventHandlerFDListenerAdapterCppToC::Get(self)->OnReadable(fileDescriptor);
}

} // namespace

ArkEventHandlerFDListenerAdapterCppToC::
    ArkEventHandlerFDListenerAdapterCppToC() {
  GetStruct()->on_readable = ark_event_handler_fdlistener_adapter_on_readable;
}

ArkEventHandlerFDListenerAdapterCppToC::
    ~ArkEventHandlerFDListenerAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkEventHandlerFDListenerAdapterCppToC, ArkEventHandlerFDListenerAdapter,
    ark_event_handler_fdlistener_adapter_t>::kBridgeType =
    ARK_EVENT_HANDLER_FDLISTENER_ADAPTER;

} // namespace OHOS::ArkWeb
