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

#ifndef VIDEO_ENCODER_IMPL_ADAPTER
#define VIDEO_ENCODER_IMPL_ADAPTER

#include "media_codec_adapter.h"
#include "arkweb/ohos_adapter_ndk/ndk_callback_wrapper/callback_shared_wrapper.h"

#include <multimedia/player_framework/native_avcodec_videoencoder.h>

namespace OHOS::NWeb {

class EncoderCallbackImpl {
public:
    EncoderCallbackImpl(std::shared_ptr<CodecCallbackAdapter> cb);

    ~EncoderCallbackImpl() = default;

    void OnError(int32_t errorCode);

    void OnOutputFormatChanged(OH_AVFormat* format);

    void OnInputBufferAvailable(uint32_t index, OH_AVBuffer* buffer);

    void OnOutputBufferAvailable(uint32_t index, OH_AVBuffer* buffer);
private:
    std::shared_ptr<CodecCallbackAdapter> cb_ = nullptr;
};

class VideoEncoderAdapterImpl : public MediaCodecAdapter {
public:
    VideoEncoderAdapterImpl() = default;

    ~VideoEncoderAdapterImpl();

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

    static BufferFlag GetBufferFlag(OH_AVCodecBufferFlags codecBufferFlag);

    static void OnError(OH_AVCodec *codec, int32_t errorCode, void *userData);

    static void OnStreamChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData);

    static void OnNeedInputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData);

    static void OnNewOutputBuffer(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData);

private:
    OH_AVCodec* encoder_ = nullptr;

    size_t callback_index_ = 0;
    static CallbackSharedWrapper<EncoderCallbackImpl> callback_wrapper_;
};
}

#endif // VIDEO_ENCODER_IMPL_ADAPTER