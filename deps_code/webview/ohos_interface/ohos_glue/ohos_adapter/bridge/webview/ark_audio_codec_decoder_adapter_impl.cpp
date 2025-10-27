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

#include "ohos_adapter/bridge/ark_audio_codec_decoder_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_decoder_format_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_decoder_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_cenc_info_adapter_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioCodecDecoderAdapterImpl::ArkAudioCodecDecoderAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AudioCodecDecoderAdapter> ref)
    : real_(ref)
{}

int32_t ArkAudioCodecDecoderAdapterImpl::CreateAudioDecoderByMime(const ArkWebString& mimetype)
{
    return (int32_t)real_->CreateAudioDecoderByMime(ArkWebStringStructToClass(mimetype));
}

int32_t ArkAudioCodecDecoderAdapterImpl::CreateAudioDecoderByName(const ArkWebString& name)
{
    return (int32_t)real_->CreateAudioDecoderByName(ArkWebStringStructToClass(name));
}

int32_t ArkAudioCodecDecoderAdapterImpl::ConfigureDecoder(const ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format)
{
    if (CHECK_REF_PTR_IS_NULL(format)) {
        return (int32_t)real_->ConfigureDecoder(nullptr);
    }
    return (int32_t)real_->ConfigureDecoder(std::make_shared<ArkAudioDecoderFormatAdapterWrapper>(format));
}

int32_t ArkAudioCodecDecoderAdapterImpl::SetParameterDecoder(const ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format)
{
    if (CHECK_REF_PTR_IS_NULL(format)) {
        return (int32_t)real_->SetParameterDecoder(nullptr);
    }
    return (int32_t)real_->SetParameterDecoder(std::make_shared<ArkAudioDecoderFormatAdapterWrapper>(format));
}

int32_t ArkAudioCodecDecoderAdapterImpl::PrepareDecoder()
{
    return (int32_t)real_->PrepareDecoder();
}

int32_t ArkAudioCodecDecoderAdapterImpl::StartDecoder()
{
    return (int32_t)real_->StartDecoder();
}

int32_t ArkAudioCodecDecoderAdapterImpl::StopDecoder()
{
    return (int32_t)real_->StopDecoder();
}

int32_t ArkAudioCodecDecoderAdapterImpl::FlushDecoder()
{
    return (int32_t)real_->FlushDecoder();
}

int32_t ArkAudioCodecDecoderAdapterImpl::ResetDecoder()
{
    return (int32_t)real_->ResetDecoder();
}

int32_t ArkAudioCodecDecoderAdapterImpl::ReleaseDecoder()
{
    return (int32_t)real_->ReleaseDecoder();
}

int32_t ArkAudioCodecDecoderAdapterImpl::QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs,
    uint8_t *bufferData, int32_t bufferSize, const ArkWebRefPtr<ArkAudioCencInfoAdapter> cencInfo, bool isEncrypted,
    uint32_t flag)
{
    return (int32_t)real_->QueueInputBufferDec(index, presentationTimeUs, bufferData, bufferSize,
        std::make_shared<ArkAudioCencInfoAdapterWrapper>(cencInfo), isEncrypted, (NWeb::BufferFlag)flag);
}

int32_t ArkAudioCodecDecoderAdapterImpl::GetOutputFormatDec(ArkWebRefPtr<ArkAudioDecoderFormatAdapter> format)
{
    if (CHECK_REF_PTR_IS_NULL(format)) {
        return (int32_t)real_->GetOutputFormatDec(nullptr);
    }
    return (int32_t)real_->GetOutputFormatDec(std::make_shared<ArkAudioDecoderFormatAdapterWrapper>(format));
}

int32_t ArkAudioCodecDecoderAdapterImpl::ReleaseOutputBufferDec(uint32_t index)
{
    return (int32_t)real_->ReleaseOutputBufferDec(index);
}

int32_t ArkAudioCodecDecoderAdapterImpl::SetCallbackDec(const ArkWebRefPtr<ArkAudioDecoderCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return (int32_t)real_->SetCallbackDec(nullptr);
    }

    return (int32_t)real_->SetCallbackDec(std::make_shared<ArkAudioDecoderCallbackAdapterWrapper>(callback));
}

int32_t ArkAudioCodecDecoderAdapterImpl::SetDecryptionConfig(void *session, bool secureAudio)
{
    return (int32_t)real_->SetDecryptionConfig(session, secureAudio);
}

} // namespace OHOS::ArkWeb
