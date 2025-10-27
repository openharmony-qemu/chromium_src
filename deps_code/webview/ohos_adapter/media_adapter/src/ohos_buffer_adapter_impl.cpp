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

#include "ohos_buffer_adapter_impl.h"

namespace OHOS::NWeb {

uint8_t* OhosBufferAdapterImpl::GetAddr()
{
    return addr_;
}

uint32_t OhosBufferAdapterImpl::GetBufferSize()
{
    return bufferSize_;
}

void OhosBufferAdapterImpl::SetAddr(uint8_t* addr)
{
    addr_ = addr;
}

void OhosBufferAdapterImpl::SetBufferSize(uint32_t size)
{
    bufferSize_ = size;
}

} // namespace OHOS::NWeb
