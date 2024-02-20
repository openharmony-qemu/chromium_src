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

#include "ohos_nweb/ctocpp/ark_web_resource_ready_callback_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkWebResourceReadyCallbackCToCpp::Cancel() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_resource_ready_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, cancel, );

  // Execute
  _struct->cancel(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebResourceReadyCallbackCToCpp::Continue() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_resource_ready_callback_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, continue0, );

  // Execute
  _struct->continue0(_struct);
}

ArkWebResourceReadyCallbackCToCpp::ArkWebResourceReadyCallbackCToCpp() {
}

ArkWebResourceReadyCallbackCToCpp::~ArkWebResourceReadyCallbackCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkWebResourceReadyCallbackCToCpp, ArkWebResourceReadyCallback,
    ark_web_resource_ready_callback_t>::kBridgeType =
    ARK_WEB_RESOURCE_READY_CALLBACK;

} // namespace OHOS::ArkWeb
