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

#include "ohos_adapter/cpptoc/ark_player_callback_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_player_callback_adapter_on_info(
    struct _ark_player_callback_adapter_t *self, int32_t type, int32_t extra,
    int32_t value) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkPlayerCallbackAdapterCppToC::Get(self)->OnInfo(type, extra, value);
}

void ARK_WEB_CALLBACK ark_player_callback_adapter_on_error(
    struct _ark_player_callback_adapter_t *self, int32_t errorType) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkPlayerCallbackAdapterCppToC::Get(self)->OnError(errorType);
}

} // namespace

ArkPlayerCallbackAdapterCppToC::ArkPlayerCallbackAdapterCppToC() {
  GetStruct()->on_info = ark_player_callback_adapter_on_info;
  GetStruct()->on_error = ark_player_callback_adapter_on_error;
}

ArkPlayerCallbackAdapterCppToC::~ArkPlayerCallbackAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkPlayerCallbackAdapterCppToC, ArkPlayerCallbackAdapter,
    ark_player_callback_adapter_t>::kBridgeType = ARK_PLAYER_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb
