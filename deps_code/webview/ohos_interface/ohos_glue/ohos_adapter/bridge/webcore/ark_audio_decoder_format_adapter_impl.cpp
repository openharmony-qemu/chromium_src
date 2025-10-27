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

#include "ohos_adapter/bridge/ark_audio_decoder_format_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioDecoderFormatAdapterImpl::ArkAudioDecoderFormatAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AudioDecoderFormatAdapter> ref) : real_(ref)
{}

int32_t ArkAudioDecoderFormatAdapterImpl::GetSampleRate()
{
    return real_->GetSampleRate();
}

int32_t ArkAudioDecoderFormatAdapterImpl::GetChannelCount()
{
    return real_->GetChannelCount();
}

int64_t ArkAudioDecoderFormatAdapterImpl::GetBitRate()
{
    return real_->GetBitRate();
}

int32_t ArkAudioDecoderFormatAdapterImpl::GetMaxInputSize()
{
    return real_->GetMaxInputSize();
}

bool ArkAudioDecoderFormatAdapterImpl::GetAACIsAdts()
{
    return real_->GetAACIsAdts();
}

int32_t ArkAudioDecoderFormatAdapterImpl::GetAudioSampleFormat()
{
    return real_->GetAudioSampleFormat();
}

int32_t ArkAudioDecoderFormatAdapterImpl::GetIdentificationHeader()
{
    return real_->GetIdentificationHeader();
}

int32_t ArkAudioDecoderFormatAdapterImpl::GetSetupHeader()
{
    return real_->GetSetupHeader();
}

uint8_t* ArkAudioDecoderFormatAdapterImpl::GetCodecConfig()
{
    return real_->GetCodecConfig();
}

uint32_t ArkAudioDecoderFormatAdapterImpl::GetCodecConfigSize()
{
    return real_->GetCodecConfigSize();
}

void ArkAudioDecoderFormatAdapterImpl::SetSampleRate(int32_t sampleRate)
{
    real_->SetSampleRate(sampleRate);
}

void ArkAudioDecoderFormatAdapterImpl::SetChannelCount(int32_t channelCount)
{
    real_->SetChannelCount(channelCount);
}

void ArkAudioDecoderFormatAdapterImpl::SetBitRate(int64_t bitRate)
{
    real_->SetBitRate(bitRate);
}

void ArkAudioDecoderFormatAdapterImpl::SetMaxInputSize(int32_t maxInputSize)
{
    real_->SetMaxInputSize(maxInputSize);
}

void ArkAudioDecoderFormatAdapterImpl::SetAACIsAdts(bool isAdts)
{
    real_->SetAACIsAdts(isAdts);
}

void ArkAudioDecoderFormatAdapterImpl::SetAudioSampleFormat(int32_t audioSampleFormat)
{
    real_->SetSetupHeader(audioSampleFormat);
}

void ArkAudioDecoderFormatAdapterImpl::SetSetupHeader(int32_t setupHeader)
{
    real_->SetSetupHeader(setupHeader);
}

void ArkAudioDecoderFormatAdapterImpl::SetIdentificationHeader(int32_t idHeader)
{
    real_->SetIdentificationHeader(idHeader);
}

void ArkAudioDecoderFormatAdapterImpl::SetCodecConfig(uint8_t *codecConfig)
{
    real_->SetCodecConfig(codecConfig);
}

void ArkAudioDecoderFormatAdapterImpl::SetCodecConfigSize(uint32_t size)
{
    real_->SetCodecConfigSize(size);
}

} // namespace OHOS::ArkWeb
