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

#include "ohos_adapter/cpptoc/ark_mmilistener_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_mmilistener_adapter_on_device_added(
    struct _ark_mmilistener_adapter_t *self, int32_t deviceId,
    const ArkWebString *type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(type, );

  // Execute
  ArkMMIListenerAdapterCppToC::Get(self)->OnDeviceAdded(deviceId, *type);
}

void ARK_WEB_CALLBACK ark_mmilistener_adapter_on_device_removed(
    struct _ark_mmilistener_adapter_t *self, int32_t deviceId,
    const ArkWebString *type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(type, );

  // Execute
  ArkMMIListenerAdapterCppToC::Get(self)->OnDeviceRemoved(deviceId, *type);
}

} // namespace

ArkMMIListenerAdapterCppToC::ArkMMIListenerAdapterCppToC() {
  GetStruct()->on_device_added = ark_mmilistener_adapter_on_device_added;
  GetStruct()->on_device_removed = ark_mmilistener_adapter_on_device_removed;
}

ArkMMIListenerAdapterCppToC::~ArkMMIListenerAdapterCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkMMIListenerAdapterCppToC, ArkMMIListenerAdapter,
                           ark_mmilistener_adapter_t>::kBridgeType =
        ARK_MMILISTENER_ADAPTER;

} // namespace OHOS::ArkWeb
