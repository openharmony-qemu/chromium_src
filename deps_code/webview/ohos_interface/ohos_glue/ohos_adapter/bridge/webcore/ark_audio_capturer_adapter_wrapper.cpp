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

#include "ohos_adapter/bridge/ark_audio_capturer_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_audio_capturer_options_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_capturer_read_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_buffer_desc_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioCapturerAdapterWrapper::ArkAudioCapturerAdapterWrapper(ArkWebRefPtr<ArkAudioCapturerAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkAudioCapturerAdapterWrapper::Create(
    const std::shared_ptr<NWeb::AudioCapturerOptionsAdapter> capturerOptions, std::string cachePath)
{
    ArkWebString str = ArkWebStringClassToStruct(cachePath);
    int result;
    if (CHECK_SHARED_PTR_IS_NULL(capturerOptions)) {
        result = ctocpp_->Create(nullptr, str);
    } else {
        result = ctocpp_->Create(new ArkAudioCapturerOptionsAdapterImpl(capturerOptions), str);
    }

    ArkWebStringStructRelease(str);
    return result;
}

bool ArkAudioCapturerAdapterWrapper::Start()
{
    return ctocpp_->Start();
}

bool ArkAudioCapturerAdapterWrapper::Stop()
{
    return ctocpp_->Stop();
}

bool ArkAudioCapturerAdapterWrapper::Release()
{
    return ctocpp_->Release2();
}

int32_t ArkAudioCapturerAdapterWrapper::SetCapturerReadCallback(
    std::shared_ptr<NWeb::AudioCapturerReadCallbackAdapter> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->SetCapturerReadCallback(nullptr);
    }

    return ctocpp_->SetCapturerReadCallback(new ArkAudioCapturerReadCallbackAdapterImpl(callback));
}

int32_t ArkAudioCapturerAdapterWrapper::GetBufferDesc(std::shared_ptr<NWeb::BufferDescAdapter> bufferDesc)
{
    if (CHECK_SHARED_PTR_IS_NULL(bufferDesc)) {
        return ctocpp_->GetBufferDesc(nullptr);
    }
    return ctocpp_->GetBufferDesc(new ArkBufferDescAdapterImpl(bufferDesc));
}

int32_t ArkAudioCapturerAdapterWrapper::Enqueue(const std::shared_ptr<NWeb::BufferDescAdapter> bufferDesc)
{
    if (CHECK_SHARED_PTR_IS_NULL(bufferDesc)) {
        return ctocpp_->Enqueue(nullptr);
    }
    return ctocpp_->Enqueue(new ArkBufferDescAdapterImpl(bufferDesc));
}

int32_t ArkAudioCapturerAdapterWrapper::GetFrameCount(uint32_t& frameCount)
{
    return ctocpp_->GetFrameCount(frameCount);
}

int64_t ArkAudioCapturerAdapterWrapper::GetAudioTime()
{
    return ctocpp_->GetAudioTime();
}
} // namespace OHOS::ArkWeb
