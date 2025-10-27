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

#include "ohos_adapter/bridge/ark_audio_decoder_format_adapter_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioDecoderFormatAdapterWrapper::ArkAudioDecoderFormatAdapterWrapper(
    ArkWebRefPtr<ArkAudioDecoderFormatAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkAudioDecoderFormatAdapterWrapper::GetSampleRate()
{
    return ctocpp_->GetSampleRate();
}

int32_t ArkAudioDecoderFormatAdapterWrapper::GetChannelCount()
{
    return ctocpp_->GetChannelCount();
}

int64_t ArkAudioDecoderFormatAdapterWrapper::GetBitRate()
{
    return ctocpp_->GetBitRate();
}

int32_t ArkAudioDecoderFormatAdapterWrapper::GetMaxInputSize()
{
    return ctocpp_->GetMaxInputSize();
}

bool ArkAudioDecoderFormatAdapterWrapper::GetAACIsAdts()
{
    return ctocpp_->GetAACIsAdts();
}

int32_t ArkAudioDecoderFormatAdapterWrapper::GetAudioSampleFormat()
{
    return ctocpp_->GetAudioSampleFormat();
}

int32_t ArkAudioDecoderFormatAdapterWrapper::GetIdentificationHeader()
{
    return ctocpp_->GetIdentificationHeader();
}

int32_t ArkAudioDecoderFormatAdapterWrapper::GetSetupHeader()
{
    return ctocpp_->GetSetupHeader();
}

uint8_t* ArkAudioDecoderFormatAdapterWrapper::GetCodecConfig()
{
    return ctocpp_->GetCodecConfig();
}

uint32_t ArkAudioDecoderFormatAdapterWrapper::GetCodecConfigSize()
{
    return ctocpp_->GetCodecConfigSize();
}

void ArkAudioDecoderFormatAdapterWrapper::SetSampleRate(int32_t sampleRate)
{
    ctocpp_->SetSampleRate(sampleRate);
}

void ArkAudioDecoderFormatAdapterWrapper::SetChannelCount(int32_t channelCount)
{
    ctocpp_->SetChannelCount(channelCount);
}

void ArkAudioDecoderFormatAdapterWrapper::SetBitRate(int64_t bitRate)
{
    ctocpp_->SetBitRate(bitRate);
}

void ArkAudioDecoderFormatAdapterWrapper::SetMaxInputSize(int32_t maxInputSize)
{
    ctocpp_->SetMaxInputSize(maxInputSize);
}

void ArkAudioDecoderFormatAdapterWrapper::SetAACIsAdts(bool isAdts)
{
    ctocpp_->SetAACIsAdts(isAdts);
}

void ArkAudioDecoderFormatAdapterWrapper::SetAudioSampleFormat(int32_t audioSampleFormat)
{
    ctocpp_->SetAudioSampleFormat(audioSampleFormat);
}

void ArkAudioDecoderFormatAdapterWrapper::SetIdentificationHeader(int32_t idHeader)
{
    ctocpp_->SetIdentificationHeader(idHeader);
}

void ArkAudioDecoderFormatAdapterWrapper::SetSetupHeader(int32_t setupHeader)
{
    ctocpp_->SetSetupHeader(setupHeader);
}

void ArkAudioDecoderFormatAdapterWrapper::SetCodecConfig(uint8_t* codecConfig)
{
    ctocpp_->SetCodecConfig(codecConfig);
}

void ArkAudioDecoderFormatAdapterWrapper::SetCodecConfigSize(uint32_t size)
{
    ctocpp_->SetCodecConfigSize(size);
}

} // namespace OHOS::ArkWeb
