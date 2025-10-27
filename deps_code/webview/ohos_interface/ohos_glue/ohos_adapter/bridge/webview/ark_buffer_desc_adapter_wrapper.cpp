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

#include "ohos_adapter/bridge/ark_buffer_desc_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkBufferDescAdapterWrapper::ArkBufferDescAdapterWrapper(ArkWebRefPtr<ArkBufferDescAdapter> ref) : ctocpp_(ref) {}

uint8_t* ArkBufferDescAdapterWrapper::GetBuffer()
{
    return ctocpp_->GetBuffer();
}

size_t ArkBufferDescAdapterWrapper::GetBufLength()
{
    return ctocpp_->GetBufLength();
}

size_t ArkBufferDescAdapterWrapper::GetDataLength()
{
    return ctocpp_->GetDataLength();
}

void ArkBufferDescAdapterWrapper::SetBuffer(uint8_t* buffer)
{
    ctocpp_->SetBuffer(buffer);
}

void ArkBufferDescAdapterWrapper::SetBufLength(size_t bufLength)
{
    ctocpp_->SetBufLength(bufLength);
}

void ArkBufferDescAdapterWrapper::SetDataLength(size_t dataLength)
{
    ctocpp_->SetDataLength(dataLength);
}

} // namespace OHOS::ArkWeb
