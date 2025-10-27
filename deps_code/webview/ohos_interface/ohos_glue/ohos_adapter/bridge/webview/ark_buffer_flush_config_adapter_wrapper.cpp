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

#include "ohos_adapter/bridge/ark_buffer_flush_config_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkBufferFlushConfigAdapterWrapper::ArkBufferFlushConfigAdapterWrapper(ArkWebRefPtr<ArkBufferFlushConfigAdapter> ref)
    : ctocpp_(ref)
{}

int32_t ArkBufferFlushConfigAdapterWrapper::GetX()
{
    return ctocpp_->GetX();
}

int32_t ArkBufferFlushConfigAdapterWrapper::GetY()
{
    return ctocpp_->GetY();
}

int32_t ArkBufferFlushConfigAdapterWrapper::GetW()
{
    return ctocpp_->GetW();
}

int32_t ArkBufferFlushConfigAdapterWrapper::GetH()
{
    return ctocpp_->GetH();
}

int64_t ArkBufferFlushConfigAdapterWrapper::GetTimestamp()
{
    return ctocpp_->GetTimestamp();
}

} // namespace OHOS::ArkWeb
