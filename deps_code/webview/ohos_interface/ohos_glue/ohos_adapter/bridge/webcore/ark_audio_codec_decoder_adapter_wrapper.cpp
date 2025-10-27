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

#include "ohos_adapter/bridge/ark_audio_codec_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_decoder_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_decoder_format_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_cenc_info_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioCodecDecoderAdapterWrapper::ArkAudioCodecDecoderAdapterWrapper(ArkWebRefPtr<ArkAudioCodecDecoderAdapter> ref)
    : ctocpp_(ref)
{}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::CreateAudioDecoderByMime(
    const std::string& mimetype)
{
    ArkWebString str = ArkWebStringClassToStruct(mimetype);
    int32_t result = ctocpp_->CreateAudioDecoderByMime(str);
    ArkWebStringStructRelease(str);
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::CreateAudioDecoderByName(
    const std::string& name)
{
    ArkWebString str = ArkWebStringClassToStruct(name);
    int32_t result = ctocpp_->CreateAudioDecoderByName(str);
    ArkWebStringStructRelease(str);
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::ConfigureDecoder(
    const std::shared_ptr<OHOS::NWeb::AudioDecoderFormatAdapter> format)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(format)) {
        result = ctocpp_->ConfigureDecoder(nullptr);
    } else {
        result = ctocpp_->ConfigureDecoder(new ArkAudioDecoderFormatAdapterImpl(format));
    }
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::SetParameterDecoder(
    const std::shared_ptr<OHOS::NWeb::AudioDecoderFormatAdapter> format)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(format)) {
        result = ctocpp_->SetParameterDecoder(nullptr);
    } else {
        result = ctocpp_->SetParameterDecoder(new ArkAudioDecoderFormatAdapterImpl(format));
    }

    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::PrepareDecoder()
{
    int32_t result = ctocpp_->PrepareDecoder();
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::StartDecoder()
{
    int32_t result = ctocpp_->StartDecoder();
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::StopDecoder()
{
    int32_t result = ctocpp_->StopDecoder();
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::FlushDecoder()
{
    int32_t result = ctocpp_->FlushDecoder();
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::ResetDecoder()
{
    int32_t result = ctocpp_->ResetDecoder();
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::ReleaseDecoder()
{
    int32_t result = ctocpp_->ReleaseDecoder();
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::QueueInputBufferDec(uint32_t index,
    int64_t presentationTimeUs, uint8_t *bufferData, int32_t bufferSize,
    const std::shared_ptr<OHOS::NWeb::AudioCencInfoAdapter> cencInfo, bool isEncrypted, NWeb::BufferFlag flag)
{
    int32_t result = ctocpp_->QueueInputBufferDec(index, presentationTimeUs, bufferData, bufferSize,
        new ArkAudioCencInfoAdapterImpl(cencInfo), isEncrypted, (uint32_t)flag);
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::GetOutputFormatDec(
    std::shared_ptr<OHOS::NWeb::AudioDecoderFormatAdapter> format)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(format)) {
        result = ctocpp_->GetOutputFormatDec(nullptr);
    } else {
        result = ctocpp_->GetOutputFormatDec(new ArkAudioDecoderFormatAdapterImpl(format));
    }

    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}


OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::ReleaseOutputBufferDec(uint32_t index)
{
    int32_t result = ctocpp_->ReleaseOutputBufferDec(index);
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::SetCallbackDec(
    const std::shared_ptr<OHOS::NWeb::AudioDecoderCallbackAdapter> callback)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        result = ctocpp_->SetCallbackDec(nullptr);
    } else {
        result = ctocpp_->SetCallbackDec(new ArkAudioDecoderCallbackAdapterImpl(callback));
    }

    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

OHOS::NWeb::AudioDecoderAdapterCode ArkAudioCodecDecoderAdapterWrapper::SetDecryptionConfig(
    void *session, bool secureAudio)
{
    int32_t result = ctocpp_->SetDecryptionConfig(session, secureAudio);
    return (OHOS::NWeb::AudioDecoderAdapterCode)result;
}

} // namespace OHOS::ArkWeb
