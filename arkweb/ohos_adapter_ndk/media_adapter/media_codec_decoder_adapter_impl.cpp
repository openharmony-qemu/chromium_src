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

#include "media_codec_decoder_adapter_impl.h"

#include <unordered_map>

#include <multimedia/player_framework/native_avcapability.h>
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>
#include <multimedia/drm_framework/native_drm_err.h>
#include <multimedia/drm_framework/native_mediakeysession.h>
#include <multimedia/player_framework/native_avbuffer.h>
#include <multimedia/player_framework/native_cencinfo.h>

#include "buffer_info_adapter_impl.h"
#include "ohos_buffer_adapter_impl.h"
#include "arkweb/ohos_nweb/src/nweb_hilog.h"

using namespace OHOS::NWeb;

namespace {

const std::unordered_map<OH_AVCodecBufferFlags, BufferFlag> BUFFER_FLAG_MAP = {
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE, BufferFlag::CODEC_BUFFER_FLAG_NONE },
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS, BufferFlag::CODEC_BUFFER_FLAG_EOS },
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_SYNC_FRAME, BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME },
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_INCOMPLETE_FRAME,
        BufferFlag::CODEC_BUFFER_FLAG_PARTIAL_FRAME },
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_CODEC_DATA, BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA }
};

const std::unordered_map<BufferFlag, OH_AVCodecBufferFlags> AV_BUFFER_FLAG_MAP = {
    { BufferFlag::CODEC_BUFFER_FLAG_NONE, OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE },
    { BufferFlag::CODEC_BUFFER_FLAG_EOS, OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS },
    { BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME, OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_SYNC_FRAME },
    { BufferFlag::CODEC_BUFFER_FLAG_PARTIAL_FRAME,
        OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_INCOMPLETE_FRAME },
    { BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA, OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_CODEC_DATA }
};
} // namespace

std::shared_mutex MediaCodecDecoderAdapterImpl::decoderMutex_;

std::map<OH_AVCodec*, OHOS::NWeb::MediaCodecDecoderAdapterImpl*> VideoDecoderCallbackManager::decoders_;

std::shared_mutex VideoDecoderCallbackManager::decodersMapMutex_;

MediaCodecDecoderAdapterImpl::~MediaCodecDecoderAdapterImpl()
{
    std::unique_lock<std::shared_mutex> lock(decoderMutex_);
    WVLOG_D("MediaCodecDecoder ~MediaCodecDecoderAdapterImpl");
    callback_ = nullptr;
    if (decoder_ != nullptr) {
        ReleaseDecoder();
    }
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::CreateVideoDecoderByMime(const std::string& mimetype)
{
    WVLOG_I("MediaCodecDecoder create decoder by mime[%{public}s], isSecure_ = %{public}d",
        mimetype.c_str(), static_cast<int32_t>(isSecure_));
    avCap_ = OH_AVCodec_GetCapability(mimetype.c_str(), false);
    if (avCap_ == nullptr) {
        WVLOG_E("MediaCodecDecoder create failed, get capability error.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    if (isSecure_) {
        return CreateVideoDecoderByName(OH_AVCapability_GetName(avCap_));
    }

    decoder_ = OH_VideoDecoder_CreateByMime(mimetype.c_str());
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder create decoder failed.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    VideoDecoderCallbackManager::AddVideoDecoder(this);
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::CreateVideoDecoderByName(const std::string& name)
{
    WVLOG_I("MediaCodecDecoder create decoder by name[%{public}s], isSecure_ = %{public}d",
        name.c_str(), static_cast<int32_t>(isSecure_));
    std::string decoderName = name;
    avCap_ = OH_AVCodec_GetCapabilityByCategory(name.c_str(), false, HARDWARE);
    if (avCap_ != nullptr) {
        decoderName = std::string(OH_AVCapability_GetName(avCap_));
        if (isSecure_) {
            decoderName += ".secure";
        }
        decoder_ = OH_VideoDecoder_CreateByName(decoderName.c_str());
        if (decoder_ != nullptr) {
            isHardwareDecode_ = true;
            VideoDecoderCallbackManager::AddVideoDecoder(this);
            WVLOG_I("MediaCodecDecoder create hardware decoder.");
            return DecoderAdapterCode::DECODER_OK;
        }
    }

    avCap_ = OH_AVCodec_GetCapabilityByCategory(name.c_str(), false, SOFTWARE);
    if (avCap_ != nullptr) {
        decoderName = std::string(OH_AVCapability_GetName(avCap_));
        if (isSecure_) {
            decoderName += ".secure";
        }
        decoder_ = OH_VideoDecoder_CreateByName(decoderName.c_str());
        if (decoder_ != nullptr) {
            isHardwareDecode_ = false;
            VideoDecoderCallbackManager::AddVideoDecoder(this);
            WVLOG_I("MediaCodecDecoder create software decoder.");
            return DecoderAdapterCode::DECODER_OK;
        }
    }

    WVLOG_E("MediaCodecDecoder create decoder failed.");
    return DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ConfigureDecoder(const std::shared_ptr<DecoderFormatAdapter> format)
{
    WVLOG_D("MediaCodecDecoder ConfigureDecoder");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        WVLOG_E("format is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (avCap_ != nullptr && !OH_AVCapability_IsVideoSizeSupported(avCap_, format->GetWidth(), format->GetHeight())) {
        WVLOG_E("video size not support.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat* codecFormat = OH_AVFormat_Create();
    if (codecFormat == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat_SetIntValue(codecFormat, OH_MD_KEY_WIDTH, format->GetWidth());
    OH_AVFormat_SetIntValue(codecFormat, OH_MD_KEY_HEIGHT, format->GetHeight());

    OH_AVErrCode ret = OH_VideoDecoder_Configure(decoder_, codecFormat);
    OH_AVFormat_Destroy(codecFormat);
    codecFormat = nullptr;
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_Configure fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetParameterDecoder(const std::shared_ptr<DecoderFormatAdapter> format)
{
    WVLOG_D("MediaCodecDecoder SetParameterDecoder");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        WVLOG_E("format is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat* codecFormat = OH_AVFormat_Create();
    if (codecFormat == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat_SetIntValue(codecFormat, OH_MD_KEY_WIDTH, format->GetWidth());
    OH_AVFormat_SetIntValue(codecFormat, OH_MD_KEY_HEIGHT, format->GetHeight());

    OH_AVErrCode ret = OH_VideoDecoder_SetParameter(decoder_, codecFormat);
    OH_AVFormat_Destroy(codecFormat);
    codecFormat = nullptr;
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_SetParameter fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetOutputSurface(void* window)
{
    WVLOG_D("MediaCodecDecoder SetOutputSurface");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (window == nullptr) {
        WVLOG_E("MediaCodecDecoder window is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OHNativeWindow* window_ = reinterpret_cast<OHNativeWindow*>(window);
    if (window_ == nullptr) {
        WVLOG_E("Window is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (isHardwareDecode_) {
        uint64_t usage = NATIVEBUFFER_USAGE_MEM_DMA;
        OH_NativeWindow_NativeWindowHandleOpt(window_, SET_USAGE, usage);
        WVLOG_I("MediaCodecDecoder default to opening Hebc.");
    }
    OH_AVErrCode ret = OH_VideoDecoder_SetSurface(decoder_, window_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_SetSurface fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::PrepareDecoder()
{
    WVLOG_D("MediaCodecDecoder PrepareDecoder");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode ret = OH_VideoDecoder_Prepare(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_Prepare fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::StartDecoder()
{
    WVLOG_D("MediaCodecDecoder StartDecoder");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Start(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_Start fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::StopDecoder()
{
    WVLOG_D("MediaCodecDecoder StopDecoder");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Stop(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_Stop fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }

    std::unique_lock<std::mutex> lock(bufferMutex_);
    bufferMap_.clear();
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::FlushDecoder()
{
    WVLOG_D("MediaCodecDecoder FlushDecoder");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Flush(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_Flush fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    std::unique_lock<std::mutex> lock(bufferMutex_);
    bufferMap_.clear();
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ResetDecoder()
{
    WVLOG_D("MediaCodecDecoder ResetDecoder");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Reset(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("OH_VideoDecoder_Reset fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    std::unique_lock<std::mutex> lock(bufferMutex_);
    bufferMap_.clear();
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ReleaseDecoder()
{
    WVLOG_D("MediaCodecDecoder ReleaseDecoder");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    VideoDecoderCallbackManager::DeleteVideoDecoder(decoder_);
    OH_AVErrCode ret = OH_VideoDecoder_Destroy(decoder_);
    decoder_ = nullptr;
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_Destroy fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    std::unique_lock<std::mutex> lock(bufferMutex_);
    bufferMap_.clear();
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::QueueInputBufferDec(
    uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, BufferFlag flag)
{
    WVLOG_D("MediaCodecDecoder QueueInputBufferDec");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode ret;
    OH_AVCodecBufferAttr attr;
    attr.pts = presentationTimeUs;
    attr.size = size;
    attr.offset = offset;
    attr.flags = MediaCodecDecoderAdapterImpl::GetAVBufferFlag(flag);

    {
        std::unique_lock<std::mutex> lock(bufferMutex_);
        if (bufferMap_.find(index) == bufferMap_.end()) {
            WVLOG_E("MediaCodecDecoder QueueInputBufferDec not find index.");
            return DecoderAdapterCode::DECODER_ERROR;
        }
        ret = OH_AVBuffer_SetBufferAttr(bufferMap_[index], &attr);
    }

    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_AVBuffer_SetBufferAttr fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    ret = OH_VideoDecoder_PushInputBuffer(decoder_, index);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_PushInputBuffer fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::GetOutputFormatDec(std::shared_ptr<DecoderFormatAdapter> format)
{
    WVLOG_D("MediaCodecDecoder GetOutputFormatDec");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        WVLOG_E("format is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat* codecFormat = OH_VideoDecoder_GetOutputDescription(decoder_);
    if (codecFormat == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    int32_t width = 0;
    int32_t height = 0;
    OH_AVFormat_GetIntValue(codecFormat, OH_MD_KEY_VIDEO_PIC_WIDTH, &width);
    OH_AVFormat_GetIntValue(codecFormat, OH_MD_KEY_VIDEO_PIC_HEIGHT, &height);
    format->SetWidth(width);
    format->SetHeight(height);
    OH_AVFormat_Destroy(codecFormat);

    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ReleaseOutputBufferDec(uint32_t index, bool isRender)
{
    WVLOG_D("MediaCodecDecoder ReleaseOutputBufferDec, isRender = %{public}d", isRender);
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode ret;
    if (isRender) {
        // Display the buffer data and release the buffer.
        ret = OH_VideoDecoder_RenderOutputBuffer(decoder_, index);
    } else {
        // Only release the buffer.
        ret = OH_VideoDecoder_FreeOutputBuffer(decoder_, index);
    }
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetCallbackDec(const std::shared_ptr<DecoderCallbackAdapter> callback)
{
    WVLOG_D("MediaCodecDecoder SetCallbackDec");
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    if (callback == nullptr) {
        WVLOG_E("Media Callback is NULL.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    callback_ = callback;
    struct OH_AVCodecCallback cb = {
      &VideoDecoderCallbackManager::OnError,
      &VideoDecoderCallbackManager::OnStreamChanged,
      &VideoDecoderCallbackManager::OnNeedInputBuffer,
      &VideoDecoderCallbackManager::OnNewOutputBuffer
    };

    OH_AVErrCode ret = OH_VideoDecoder_RegisterCallback(decoder_, cb, nullptr);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    return DecoderAdapterCode::DECODER_OK;
}

BufferFlag MediaCodecDecoderAdapterImpl::GetBufferFlag(OH_AVCodecBufferFlags codecBufferFlag)
{
    auto flag = BUFFER_FLAG_MAP.find(codecBufferFlag);
    if (flag == BUFFER_FLAG_MAP.end()) {
        WVLOG_E("buffer flag not found.");
        return BufferFlag::CODEC_BUFFER_FLAG_NONE;
    }
    return flag->second;
}

OH_AVCodecBufferFlags MediaCodecDecoderAdapterImpl::GetAVBufferFlag(BufferFlag bufferFlag)
{
    auto flag = AV_BUFFER_FLAG_MAP.find(bufferFlag);
    if (flag == AV_BUFFER_FLAG_MAP.end()) {
        WVLOG_E("buffer flag not found.");
        return OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE;
    }
    return flag->second;
}

void MediaCodecDecoderAdapterImpl::OnError(int32_t errorCode)
{
    WVLOG_D("MediaCodecDecoder OnError");
    if (!callback_) {
        WVLOG_E("callback is NULL.");
        return;
    }

    callback_->OnError(ErrorType::CODEC_ERROR_INTERNAL, errorCode);
}

void MediaCodecDecoderAdapterImpl::OnOutputFormatChanged(OH_AVFormat* format)
{
    WVLOG_D("MediaCodecDecoder OnOutputFormatChanged");
    if (!callback_) {
        WVLOG_E("callback is NULL.");
        return;
    }

    if (format == nullptr) {
        WVLOG_E("format is NULL.");
        return;
    }

    int32_t width = 0;
    int32_t height = 0;
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_WIDTH, &width);
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_HEIGHT, &height);
    callback_->OnStreamChanged(width, height, 0);
}

void MediaCodecDecoderAdapterImpl::OnInputBufferAvailable(uint32_t index, OH_AVBuffer* buffer)
{
    WVLOG_D("MediaCodecDecoder OnInputBufferAvailable");
    if (!callback_) {
        WVLOG_E("callback is NULL.");
        return;
    }
    if (buffer == nullptr) {
        WVLOG_E("buffer is NULL.");
        return;
    }
    uint8_t *addr = OH_AVBuffer_GetAddr(buffer);
    int32_t bufferSize = OH_AVBuffer_GetCapacity(buffer);
    if (addr == nullptr) {
        WVLOG_E("addr is NULL.");
        return;
    }
    if (bufferSize <= 0) {
        WVLOG_E("bufferSize[%{public}d] error.", bufferSize);
        return;
    }

    std::shared_ptr<OhosBufferAdapterImpl> ohosBuffer = std::make_shared<OhosBufferAdapterImpl>();
    if (ohosBuffer == nullptr) {
        WVLOG_E("new OhosBufferAdapterImpl failed");
        return;
    }

    {
        std::unique_lock<std::mutex> lock(bufferMutex_);
        bufferMap_[index] = buffer;
    }

    ohosBuffer->SetAddr(addr);
    ohosBuffer->SetBufferSize(bufferSize);
    callback_->OnNeedInputData(index, ohosBuffer);
}

void MediaCodecDecoderAdapterImpl::OnOutputBufferAvailable(uint32_t index, OH_AVBuffer* buffer)
{
    WVLOG_D("MediaCodecDecoder OnOutputBufferAvailable");
    if (!callback_) {
        WVLOG_E("callback is NULL.");
        return;
    }

    if (buffer == nullptr) {
        WVLOG_E("buffer is NULL.");
        return; 
    }

    OH_AVCodecBufferAttr attr;
    OH_AVErrCode ret = OH_AVBuffer_GetBufferAttr(buffer, &attr);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return;
    }

    std::shared_ptr<BufferInfoAdapterImpl> info_ = std::make_shared<BufferInfoAdapterImpl>();
    if (!info_) {
        WVLOG_E("new BufferInfoAdapterImpl failed");
        return;
    }

    info_->SetPresentationTimeUs(attr.pts);
    info_->SetSize(attr.size);
    info_->SetOffset(attr.offset);

    BufferFlag bufferFlag_;

    bufferFlag_ = MediaCodecDecoderAdapterImpl::GetBufferFlag(static_cast<OH_AVCodecBufferFlags>(attr.flags));
    callback_->OnNeedOutputData(index, info_, bufferFlag_);
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetAVCencInfoStruct(
    OH_AVCencInfo *avCencInfo, const std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    if (cencInfo->GetClearHeaderLens().size() != cencInfo->GetPayLoadLens().size()) {
        WVLOG_E("MediaCodecDecoder GetClearHeaderLens().size() != GetPayLoadLens().size()");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode errNo = OH_AVCencInfo_SetAlgorithm(avCencInfo, static_cast<DrmCencAlgorithm>(cencInfo->GetAlgo()));
    if (errNo != AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder set AVCencInfo Algorithm fail, errNo = %{public}u", static_cast<uint32_t>(errNo));
        return DecoderAdapterCode::DECODER_ERROR;
    }

    errNo = OH_AVCencInfo_SetKeyIdAndIv(
        avCencInfo, cencInfo->GetKeyId(), cencInfo->GetKeyIdLen(), cencInfo->GetIv(), cencInfo->GetIvLen());
    if (errNo != AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder set AVCencInfo keyid and iv fail, errNo = %{public}u",
            static_cast<uint32_t>(errNo));
        return DecoderAdapterCode::DECODER_ERROR;
    }

    std::vector<DrmSubsample> subSamples(cencInfo->GetClearHeaderLens().size());
    for (uint32_t i = 0; i < cencInfo->GetClearHeaderLens().size(); i++) {
        subSamples[i].clearHeaderLen = cencInfo->GetClearHeaderLens()[i];
        subSamples[i].payLoadLen = cencInfo->GetPayLoadLens()[i];
    }
    errNo = OH_AVCencInfo_SetSubsampleInfo(
        avCencInfo, cencInfo->GetEncryptedBlockCount(), cencInfo->GetSkippedBlockCount(),
        cencInfo->GetFirstEncryptedOffset(), cencInfo->GetClearHeaderLens().size(), subSamples.data());
    if (errNo != AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder set AVCencInfo subsampleInfo fail, errNo = %{public}u",
            static_cast<uint32_t>(errNo));
        return DecoderAdapterCode::DECODER_ERROR;
    }

    errNo = OH_AVCencInfo_SetMode(avCencInfo, DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET);
    if (errNo != AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder set AVCencInfo mode fail, errNo = %{public}u", static_cast<uint32_t>(errNo));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetDecryptionConfig(void *session, bool isSecure)
{
    WVLOG_I("%{public}s, isSecure = %{public}u, session = %{public}d.",
        __FUNCTION__, static_cast<uint32_t>(isSecure), session ? 1 : 0);
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder decoder_ is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    isSecure_ = isSecure;
    if (session == nullptr) {
        WVLOG_E("MediaCodecDecoder session is nullptr.");
        return DecoderAdapterCode::DECODER_OK;
    }
    OH_AVErrCode errCode = OH_VideoDecoder_SetDecryptionConfig(
        decoder_, static_cast<MediaKeySession *>(session), isSecure_);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("MediaCodecDecoder OH_VideoDecoder_SetDecryptionConfig fail, errCode = %{public}u.", uint32_t(errCode));
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetAVCencInfo(
    uint32_t index, const std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    if (cencInfo == nullptr) {
        WVLOG_E("cencInfo is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret;
    OH_AVCencInfo *avCencInfo = OH_AVCencInfo_Create();
    if (avCencInfo == nullptr) {
        WVLOG_E("creat avCencInfo fail.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (SetAVCencInfoStruct(avCencInfo, cencInfo) != DecoderAdapterCode::DECODER_OK) {
        WVLOG_E("OH_AVCencInfo_SetAVCencInfoStruct fail.");
        (void)OH_AVCencInfo_Destroy(avCencInfo);
        avCencInfo = nullptr;
        return DecoderAdapterCode::DECODER_ERROR;
    }

    {
        std::unique_lock<std::mutex> lock(bufferMutex_);
        if (bufferMap_.find(index) == bufferMap_.end()) {
            WVLOG_E("MediaCodecDecoder QueueInputBufferDec not find index.");
            (void)OH_AVCencInfo_Destroy(avCencInfo);
            avCencInfo = nullptr;
            return DecoderAdapterCode::DECODER_ERROR;
        }
        ret = OH_AVCencInfo_SetAVBuffer(avCencInfo, bufferMap_[index]);
    }

    if (ret != AV_ERR_OK) {
        WVLOG_E("OH_AVCencInfo_SetAVBuffer fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        (void)OH_AVCencInfo_Destroy(avCencInfo);
        avCencInfo = nullptr;
        return DecoderAdapterCode::DECODER_ERROR;
    }
    ret = OH_AVCencInfo_Destroy(avCencInfo);
    avCencInfo = nullptr;
    if (ret != AV_ERR_OK) {
        WVLOG_E("OH_AVCencInfo_Destroy fail, ret=%{public}u.", static_cast<uint32_t>(ret));
        return DecoderAdapterCode::DECODER_ERROR;
    }

    return DecoderAdapterCode::DECODER_OK;
}

std::shared_mutex& MediaCodecDecoderAdapterImpl::GetDecoderMutex() {
    return decoderMutex_;
}

void VideoDecoderCallbackManager::OnError(OH_AVCodec *codec, int32_t errorCode, void *userData) {
    (void)userData;
    WVLOG_D("VideoDecoderCallbackManager %{public}s.", __FUNCTION__);
    if (!codec) {
        WVLOG_E("VideoDecoderCallbackManager avcodec is nullptr.");
        return;
    }

    std::shared_lock<std::shared_mutex> lock(MediaCodecDecoderAdapterImpl::GetDecoderMutex());
    OHOS::NWeb::MediaCodecDecoderAdapterImpl* impl = FindVideoDecoder(codec);
    if (!impl) {
        WVLOG_E("VideoDecoderCallbackManager not find decoder.");
        return;
    }

    impl->OnError(errorCode);
}

void VideoDecoderCallbackManager::OnStreamChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
    (void)userData;
    WVLOG_D("VideoDecoderCallbackManager %{public}s.", __FUNCTION__);
    if (!codec) {
        WVLOG_E("VideoDecoderCallbackManager avcodec is nullptr.");
        return;
    }

    std::shared_lock<std::shared_mutex> lock(MediaCodecDecoderAdapterImpl::GetDecoderMutex());
    OHOS::NWeb::MediaCodecDecoderAdapterImpl* impl = FindVideoDecoder(codec);
    if (!impl) {
        WVLOG_E("VideoDecoderCallbackManager not find decoder.");
        return;
    }

    impl->OnOutputFormatChanged(format);
}

void VideoDecoderCallbackManager::OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
    (void)userData;
    WVLOG_D("VideoDecoderCallbackManager %{public}s.", __FUNCTION__);
    if (!codec) {
        WVLOG_E("VideoDecoderCallbackManager avcodec is nullptr.");
        return;
    }

    std::shared_lock<std::shared_mutex> lock(MediaCodecDecoderAdapterImpl::GetDecoderMutex());
    OHOS::NWeb::MediaCodecDecoderAdapterImpl* impl = FindVideoDecoder(codec);
    if (!impl) {
        WVLOG_E("VideoDecoderCallbackManager not find decoder.");
        return;
    }

    impl->OnInputBufferAvailable(index, buffer);
}

void VideoDecoderCallbackManager::OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
    (void)userData;
    WVLOG_D("VideoDecoderCallbackManager %{public}s.", __FUNCTION__);
    if (!codec) {
        WVLOG_E("VideoDecoderCallbackManager avcodec is nullptr.");
        return;
    }

    std::shared_lock<std::shared_mutex> lock(MediaCodecDecoderAdapterImpl::GetDecoderMutex());
    OHOS::NWeb::MediaCodecDecoderAdapterImpl* impl = FindVideoDecoder(codec);
    if (!impl) {
        WVLOG_E("VideoDecoderCallbackManager not find decoder.");
        return;
    }

    impl->OnOutputBufferAvailable(index, buffer);
}

OHOS::NWeb::MediaCodecDecoderAdapterImpl* VideoDecoderCallbackManager::FindVideoDecoder(OH_AVCodec *codec) {
    std::shared_lock<std::shared_mutex> lock(decodersMapMutex_);
    auto it = decoders_.find(codec);
    if (it != decoders_.end()) {
        return it->second;
    }

    return nullptr;
}

void VideoDecoderCallbackManager::DeleteVideoDecoder(OH_AVCodec *codec) {
    if (codec == nullptr) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(decodersMapMutex_);
    decoders_.erase(codec);
}

void VideoDecoderCallbackManager::AddVideoDecoder(OHOS::NWeb::MediaCodecDecoderAdapterImpl *impl) {
    if (impl == nullptr || impl->GetAVDecoder() == nullptr) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(decodersMapMutex_);
    if (decoders_.find(impl->GetAVDecoder()) != decoders_.end()) {
        return;
    }

    decoders_[impl->GetAVDecoder()] = impl;
}