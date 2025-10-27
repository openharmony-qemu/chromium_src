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

#ifndef MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H
#define MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H

#include "avcodec_errors.h"
#include "avcodec_list.h"
#include "avcodec_video_encoder.h"
#include "media_codec_adapter.h"
#include "media_description.h"
#include "surface_adapter_impl.h"

namespace OHOS::NWeb {
using namespace OHOS::MediaAVCodec;

class EncoderCallbackImpl : public MediaAVCodec::AVCodecCallback {
public:
    EncoderCallbackImpl(std::shared_ptr<CodecCallbackAdapter> cb);
    ~EncoderCallbackImpl() override = default;
    void OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode) override;
    void OnOutputFormatChanged(const MediaAVCodec::Format& format) override;
    void OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer) override;
    void OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
        MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer) override;

private:
    std::shared_ptr<CodecCallbackAdapter> cb_ = nullptr;
};

class MediaCodecEncoderAdapterImpl : public MediaCodecAdapter {
public:
    MediaCodecEncoderAdapterImpl() = default;

    ~MediaCodecEncoderAdapterImpl() override = default;

    CodecCodeAdapter CreateVideoCodecByMime(const std::string mimetype) override;
    CodecCodeAdapter CreateVideoCodecByName(const std::string name) override;
    CodecCodeAdapter SetCodecCallback(const std::shared_ptr<CodecCallbackAdapter> callback) override;
    CodecCodeAdapter Configure(const std::shared_ptr<CodecConfigParaAdapter> config) override;
    CodecCodeAdapter Prepare() override;
    CodecCodeAdapter Start() override;
    CodecCodeAdapter Stop() override;
    CodecCodeAdapter Reset() override;
    CodecCodeAdapter Release() override;
    std::shared_ptr<ProducerSurfaceAdapter> CreateInputSurface() override;
    CodecCodeAdapter ReleaseOutputBuffer(uint32_t index, bool isRender) override;
    CodecCodeAdapter RequestKeyFrameSoon() override;
    static ErrorType GetErrorType(MediaAVCodec::AVCodecErrorType codecErrorType);
    static BufferFlag GetBufferFlag(MediaAVCodec::AVCodecBufferFlag codecBufferFlag);

private:
    std::shared_ptr<OHOS::MediaAVCodec::AVCodecVideoEncoder> encoder_ = nullptr;
    std::shared_ptr<EncoderCallbackImpl> callback_ = nullptr;
    sptr<Surface> avCodecEncoderSurface_ = nullptr;
};
} // namespace OHOS::NWeb
#endif // MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H
