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

#include "ohos_adapter/ctocpp/ark_ohos_init_web_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void *ArkOhosInitWebAdapterCToCpp::GetRunWebInitedCallback() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_init_web_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_run_web_inited_callback,
                                   nullptr);

  // Execute
  return _struct->get_run_web_inited_callback(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkOhosInitWebAdapterCToCpp::SetRunWebInitedCallback(void *callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_init_web_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_run_web_inited_callback, );

  // Execute
  _struct->set_run_web_inited_callback(_struct, callback);
}

ArkOhosInitWebAdapterCToCpp::ArkOhosInitWebAdapterCToCpp() {
}

ArkOhosInitWebAdapterCToCpp::~ArkOhosInitWebAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkOhosInitWebAdapterCToCpp, ArkOhosInitWebAdapter,
                           ark_ohos_init_web_adapter_t>::kBridgeType =
        ARK_OHOS_INIT_WEB_ADAPTER;

} // namespace OHOS::ArkWeb
