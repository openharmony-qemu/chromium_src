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

#include "ohos_adapter/bridge/ark_audio_capturer_adapter_impl.h"

#include "ohos_adapter/bridge/ark_audio_capturer_options_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_capturer_read_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_buffer_desc_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioCapturerAdapterImpl::ArkAudioCapturerAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioCapturerAdapter> ref)
    : real_(ref)
{}

int32_t ArkAudioCapturerAdapterImpl::Create(
    const ArkWebRefPtr<ArkAudioCapturerOptionsAdapter> capturerOptions, ArkWebString& cachePath)
{
    std::string str = ArkWebStringStructToClass(cachePath);
    if (CHECK_REF_PTR_IS_NULL(capturerOptions)) {
        return real_->Create(nullptr, str);
    }
    return real_->Create(std::make_shared<ArkAudioCapturerOptionsAdapterWrapper>(capturerOptions), str);
}

bool ArkAudioCapturerAdapterImpl::Start()
{
    return real_->Start();
}

bool ArkAudioCapturerAdapterImpl::Stop()
{
    return real_->Stop();
}

bool ArkAudioCapturerAdapterImpl::Release2()
{
    return real_->Release();
}

int32_t ArkAudioCapturerAdapterImpl::SetCapturerReadCallback(
    const ArkWebRefPtr<ArkAudioCapturerReadCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_->SetCapturerReadCallback(nullptr);
    }

    return real_->SetCapturerReadCallback(std::make_shared<ArkAudioCapturerReadCallbackAdapterWrapper>(callback));
}

int32_t ArkAudioCapturerAdapterImpl::GetBufferDesc(ArkWebRefPtr<ArkBufferDescAdapter> bufferDesc)
{
    if (CHECK_REF_PTR_IS_NULL(bufferDesc)) {
        return real_->GetBufferDesc(nullptr);
    }
    return real_->GetBufferDesc(std::make_shared<ArkBufferDescAdapterWrapper>(bufferDesc));
}

int32_t ArkAudioCapturerAdapterImpl::Enqueue(const ArkWebRefPtr<ArkBufferDescAdapter> bufferDesc)
{
    if (CHECK_REF_PTR_IS_NULL(bufferDesc)) {
        return real_->Enqueue(nullptr);
    }
    return real_->Enqueue(std::make_shared<ArkBufferDescAdapterWrapper>(bufferDesc));
}

int32_t ArkAudioCapturerAdapterImpl::GetFrameCount(uint32_t& frameCount)
{
    return real_->GetFrameCount(frameCount);
}

int64_t ArkAudioCapturerAdapterImpl::GetAudioTime()
{
    return real_->GetAudioTime();
}

} // namespace OHOS::ArkWeb
