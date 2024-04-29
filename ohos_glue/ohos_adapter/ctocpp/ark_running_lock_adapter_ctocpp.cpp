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

#include "ohos_adapter/ctocpp/ark_running_lock_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkRunningLockAdapterCToCpp::IsUsed() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_running_lock_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_used, false);

  // Execute
  return _struct->is_used(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkRunningLockAdapterCToCpp::Lock(uint32_t timeOutMs) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_running_lock_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, lock, 0);

  // Execute
  return _struct->lock(_struct, timeOutMs);
}

ARK_WEB_NO_SANITIZE
int32_t ArkRunningLockAdapterCToCpp::UnLock() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_running_lock_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, un_lock, 0);

  // Execute
  return _struct->un_lock(_struct);
}

ArkRunningLockAdapterCToCpp::ArkRunningLockAdapterCToCpp() {
}

ArkRunningLockAdapterCToCpp::~ArkRunningLockAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkRunningLockAdapterCToCpp, ArkRunningLockAdapter,
                           ark_running_lock_adapter_t>::kBridgeType =
        ARK_RUNNING_LOCK_ADAPTER;

} // namespace OHOS::ArkWeb
