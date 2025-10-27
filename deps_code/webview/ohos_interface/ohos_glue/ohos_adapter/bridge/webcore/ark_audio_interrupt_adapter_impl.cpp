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

#include "ohos_adapter/bridge/ark_audio_interrupt_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkAudioInterruptAdapterImpl::ArkAudioInterruptAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioInterruptAdapter> ref)
    : real_(ref)
{}

int32_t ArkAudioInterruptAdapterImpl::GetStreamUsage()
{
    return (int32_t)real_->GetStreamUsage();
}

int32_t ArkAudioInterruptAdapterImpl::GetContentType()
{
    return (int32_t)real_->GetContentType();
}

int32_t ArkAudioInterruptAdapterImpl::GetStreamType()
{
    return (int32_t)real_->GetStreamType();
}

uint32_t ArkAudioInterruptAdapterImpl::GetSessionID()
{
    return real_->GetSessionID();
}

bool ArkAudioInterruptAdapterImpl::GetPauseWhenDucked()
{
    return real_->GetPauseWhenDucked();
}

} // namespace OHOS::ArkWeb
