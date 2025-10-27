/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_audio_decoder_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_decoder_format_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioDecoderCallbackAdapterImpl::ArkAudioDecoderCallbackAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AudioDecoderCallbackAdapter> ref)
    : real_(ref)
{}

void ArkAudioDecoderCallbackAdapterImpl::OnError(int32_t errorCode)
{
    real_->OnError(errorCode);
}

void ArkAudioDecoderCallbackAdapterImpl::OnOutputFormatChanged()
{
    real_->OnOutputFormatChanged();
}

void ArkAudioDecoderCallbackAdapterImpl::OnInputBufferAvailable(uint32_t index)
{
    real_->OnInputBufferAvailable(index);
}

void ArkAudioDecoderCallbackAdapterImpl::OnOutputBufferAvailable(
    uint32_t index, uint8_t *bufferData, int32_t size, int64_t pts, int32_t offset, uint32_t flags)
{
    real_->OnOutputBufferAvailable(index, bufferData, size, pts, offset, flags);
}

} // namespace OHOS::ArkWeb