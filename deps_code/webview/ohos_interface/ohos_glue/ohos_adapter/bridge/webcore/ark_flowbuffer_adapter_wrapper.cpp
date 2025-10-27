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

#include "ohos_adapter/bridge/ark_flowbuffer_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkFlowbufferAdapterWrapper::ArkFlowbufferAdapterWrapper(ArkWebRefPtr<ArkFlowbufferAdapter> ref) : ctocpp_(ref) {}

void ArkFlowbufferAdapterWrapper::StartPerformanceBoost()
{
    ctocpp_->StartPerformanceBoost();
}

void* ArkFlowbufferAdapterWrapper::CreateAshmem(size_t size, int mapType, int& fd)
{
    return ctocpp_->CreateAshmem(size, mapType, fd);
}

void* ArkFlowbufferAdapterWrapper::CreateAshmemWithFd(const int fd, size_t size, int mapType)
{
    return ctocpp_->CreateAshmemWithFd(fd, size, mapType);
}

} // namespace OHOS::ArkWeb
