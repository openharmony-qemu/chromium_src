/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_buffer_desc_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkBufferDescAdapterImpl::ArkBufferDescAdapterImpl(std::shared_ptr<OHOS::NWeb::BufferDescAdapter> ref) : real_(ref) {}

uint8_t* ArkBufferDescAdapterImpl::GetBuffer()
{
    return real_->GetBuffer();
}

size_t ArkBufferDescAdapterImpl::GetBufLength()
{
    return real_->GetBufLength();
}

size_t ArkBufferDescAdapterImpl::GetDataLength()
{
    return real_->GetDataLength();
}

void ArkBufferDescAdapterImpl::SetBuffer(uint8_t* buffer)
{
    return real_->SetBuffer(buffer);
}

void ArkBufferDescAdapterImpl::SetBufLength(size_t bufLength)
{
    return real_->SetBufLength(bufLength);
}

void ArkBufferDescAdapterImpl::SetDataLength(size_t dataLength)
{
    return real_->SetDataLength(dataLength);
}

} // namespace OHOS::ArkWeb
