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

#include "ohos_adapter/ctocpp/ark_ashmem_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/bridge/ark_web_adapter_bridge_helper.h"

namespace OHOS::ArkWeb {

using ArkAshmemAdapterAshmemCreateFunc = int (*)(const char *name, size_t size);
static ArkAshmemAdapterAshmemCreateFunc ark_ashmem_adapter_ashmem_create =
    nullptr;

ARK_WEB_NO_SANITIZE
int ArkAshmemAdapter::AshmemCreate(const char *name, size_t size) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_ashmem_adapter_ashmem_create) {
    ark_ashmem_adapter_ashmem_create =
        reinterpret_cast<ArkAshmemAdapterAshmemCreateFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_ashmem_adapter_ashmem_create_static"));
    if (!ark_ashmem_adapter_ashmem_create) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return 0;
    }
  }

  // Execute
  return ark_ashmem_adapter_ashmem_create(name, size);
}

ArkAshmemAdapterCToCpp::ArkAshmemAdapterCToCpp() {
}

ArkAshmemAdapterCToCpp::~ArkAshmemAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkAshmemAdapterCToCpp, ArkAshmemAdapter,
                           ark_ashmem_adapter_t>::kBridgeType =
        ARK_ASHMEM_ADAPTER;

} // namespace OHOS::ArkWeb
