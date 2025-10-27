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

#include "ohos_adapter/bridge/ark_flowbuffer_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkFlowbufferAdapterImpl::ArkFlowbufferAdapterImpl(std::shared_ptr<OHOS::NWeb::FlowbufferAdapter> ref) : real_(ref) {}

void ArkFlowbufferAdapterImpl::StartPerformanceBoost()
{
    real_->StartPerformanceBoost();
}

void* ArkFlowbufferAdapterImpl::CreateAshmem(size_t size, int mapType, int& fd)
{
    return real_->CreateAshmem(size, mapType, fd);
}

void* ArkFlowbufferAdapterImpl::CreateAshmemWithFd(const int fd, size_t size, int mapType)
{
    return real_->CreateAshmemWithFd(fd, size, mapType);
}

} // namespace OHOS::ArkWeb
