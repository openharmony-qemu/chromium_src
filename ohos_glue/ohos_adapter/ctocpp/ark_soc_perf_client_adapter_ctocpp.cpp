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

#include "ohos_adapter/ctocpp/ark_soc_perf_client_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkSocPerfClientAdapterCToCpp::ApplySocPerfConfigById(int32_t id) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_soc_perf_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, apply_soc_perf_config_by_id, );

  // Execute
  _struct->apply_soc_perf_config_by_id(_struct, id);
}

ARK_WEB_NO_SANITIZE
void ArkSocPerfClientAdapterCToCpp::ApplySocPerfConfigByIdEx(int32_t id,
                                                             bool onOffTag) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_soc_perf_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, apply_soc_perf_config_by_id_ex, );

  // Execute
  _struct->apply_soc_perf_config_by_id_ex(_struct, id, onOffTag);
}

ArkSocPerfClientAdapterCToCpp::ArkSocPerfClientAdapterCToCpp() {
}

ArkSocPerfClientAdapterCToCpp::~ArkSocPerfClientAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkSocPerfClientAdapterCToCpp, ArkSocPerfClientAdapter,
    ark_soc_perf_client_adapter_t>::kBridgeType = ARK_SOC_PERF_CLIENT_ADAPTER;

} // namespace OHOS::ArkWeb
