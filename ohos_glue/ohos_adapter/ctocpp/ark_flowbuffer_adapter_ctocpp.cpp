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

#include "ohos_adapter/ctocpp/ark_flowbuffer_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkFlowbufferAdapterCToCpp::StartPerformanceBoost() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_flowbuffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_performance_boost, );

  // Execute
  _struct->start_performance_boost(_struct);
}

ARK_WEB_NO_SANITIZE
void *ArkFlowbufferAdapterCToCpp::CreateAshmem(size_t size, int mapType,
                                               int &fd) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_flowbuffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_ashmem, nullptr);

  // Execute
  return _struct->create_ashmem(_struct, size, mapType, &fd);
}

ARK_WEB_NO_SANITIZE
void *ArkFlowbufferAdapterCToCpp::CreateAshmemWithFd(const int fd, size_t size,
                                                     int mapType) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_flowbuffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_ashmem_with_fd, nullptr);

  // Execute
  return _struct->create_ashmem_with_fd(_struct, fd, size, mapType);
}

ArkFlowbufferAdapterCToCpp::ArkFlowbufferAdapterCToCpp() {
}

ArkFlowbufferAdapterCToCpp::~ArkFlowbufferAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkFlowbufferAdapterCToCpp, ArkFlowbufferAdapter,
                           ark_flowbuffer_adapter_t>::kBridgeType =
        ARK_FLOWBUFFER_ADAPTER;

} // namespace OHOS::ArkWeb
