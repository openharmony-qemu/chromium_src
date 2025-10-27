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

#include "ohos_adapter/bridge/ark_media_avsession_position_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaAVSessionPositionAdapterWrapper::ArkMediaAVSessionPositionAdapterWrapper(
    ArkWebRefPtr<ArkMediaAVSessionPositionAdapter> ref)
    : ctocpp_(ref)
{}

void ArkMediaAVSessionPositionAdapterWrapper::SetDuration(int64_t duration)
{
    ctocpp_->SetDuration(duration);
}

int64_t ArkMediaAVSessionPositionAdapterWrapper::GetDuration()
{
    return ctocpp_->GetDuration();
}

void ArkMediaAVSessionPositionAdapterWrapper::SetElapsedTime(int64_t elapsedTime)
{
    ctocpp_->SetElapsedTime(elapsedTime);
}

int64_t ArkMediaAVSessionPositionAdapterWrapper::GetElapsedTime()
{
    return ctocpp_->GetElapsedTime();
}

void ArkMediaAVSessionPositionAdapterWrapper::SetUpdateTime(int64_t updateTime)
{
    ctocpp_->SetUpdateTime(updateTime);
}

int64_t ArkMediaAVSessionPositionAdapterWrapper::GetUpdateTime()
{
    return ctocpp_->GetUpdateTime();
}

} // namespace OHOS::ArkWeb
