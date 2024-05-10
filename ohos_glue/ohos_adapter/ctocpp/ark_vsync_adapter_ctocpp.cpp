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

#include "ohos_adapter/ctocpp/ark_vsync_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
uint32_t ArkVSyncAdapterCToCpp::RequestVsync(void *data, void *cb) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_vsync_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, request_vsync, 0);

  // Execute
  return _struct->request_vsync(_struct, data, cb);
}

ARK_WEB_NO_SANITIZE
int64_t ArkVSyncAdapterCToCpp::GetVSyncPeriod() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_vsync_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_vsync_period, 0);

  // Execute
  return _struct->get_vsync_period(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkVSyncAdapterCToCpp::SetFrameRateLinkerEnable(bool enabled) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_vsync_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_frame_rate_linker_enable, );

  // Execute
  _struct->set_frame_rate_linker_enable(_struct, enabled);
}

ARK_WEB_NO_SANITIZE
void ArkVSyncAdapterCToCpp::SetFramePreferredRate(int32_t preferredRate) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_vsync_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_frame_preferred_rate, );

  // Execute
  _struct->set_frame_preferred_rate(_struct, preferredRate);
}

ARK_WEB_NO_SANITIZE
void ArkVSyncAdapterCToCpp::SetOnVsyncCallback(void (*callback)()) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_vsync_adapter_t* _struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_on_vsync_callback, );

  // Execute
  return _struct->set_on_vsync_callback(_struct, callback);
}

ArkVSyncAdapterCToCpp::ArkVSyncAdapterCToCpp() {
}

ArkVSyncAdapterCToCpp::~ArkVSyncAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<ArkVSyncAdapterCToCpp, ArkVSyncAdapter,
                                        ark_vsync_adapter_t>::kBridgeType =
    ARK_VSYNC_ADAPTER;

} // namespace OHOS::ArkWeb
