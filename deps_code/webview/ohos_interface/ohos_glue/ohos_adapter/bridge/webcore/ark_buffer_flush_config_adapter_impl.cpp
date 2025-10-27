/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_buffer_flush_config_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkBufferFlushConfigAdapterImpl::ArkBufferFlushConfigAdapterImpl(
    std::shared_ptr<OHOS::NWeb::BufferFlushConfigAdapter> ref)
    : real_(ref)
{}

int32_t ArkBufferFlushConfigAdapterImpl::GetX()
{
    return real_->GetX();
}

int32_t ArkBufferFlushConfigAdapterImpl::GetY()
{
    return real_->GetY();
}

int32_t ArkBufferFlushConfigAdapterImpl::GetW()
{
    return real_->GetW();
}

int32_t ArkBufferFlushConfigAdapterImpl::GetH()
{
    return real_->GetH();
}

int64_t ArkBufferFlushConfigAdapterImpl::GetTimestamp()
{
    return real_->GetTimestamp();
}
} // namespace OHOS::ArkWeb
