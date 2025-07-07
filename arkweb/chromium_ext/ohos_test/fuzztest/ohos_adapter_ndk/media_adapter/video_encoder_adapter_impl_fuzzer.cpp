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

#include "arkweb/ohos_adapter_ndk/media_adapter/include/video_encoder_adapter_impl.h"

#include <cstring>
#include <unordered_map>
#include <vector>
#include <string>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 255;
const std::vector<OH_AVCodecBufferFlags> CODEC_BUFFER_FLAG_VECTOR = {
    OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE,
    OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS,
    OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_SYNC_FRAME,
    OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_INCOMPLETE_FRAME,
    OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_CODEC_DATA
};

class CodecCallbackAdapterMock : public CodecCallbackAdapter {
public:
    CodecCallbackAdapterMock() = default;

    ~CodecCallbackAdapterMock() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override {}

    void OnStreamChanged(const std::shared_ptr<CodecFormatAdapter> format) override {}

    void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) override {}

    void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag,
        std::shared_ptr<OhosBufferAdapter> buffer) override {}
};

class CodecConfigParaAdapterMock : public CodecConfigParaAdapter {
public:
    CodecConfigParaAdapterMock() = default;

    ~CodecConfigParaAdapterMock() override = default;

    int32_t GetWidth() override
    {
        return width_;
    }

    int32_t GetHeight() override
    {
        return height_;
    }

    int64_t GetBitRate() override
    {
        return bitRate_;
    }

    double GetFrameRate() override
    {
        return frameRate_;
    }

    void SetWidth(int32_t width)
    {
        width_ = width;
    }

    void SetHeight(int32_t height)
    {
        height_ = height;
    }

    void SetBitRate(int64_t bitRate)
    {
        bitRate_ = bitRate;
    }

    void SetFrameRate(double frameRate)
    {
        frameRate_ = frameRate;
    }

    int32_t width_ = 0;
    int32_t height_ = 0;
    int64_t bitRate_ = 0;
    double frameRate_ = 0.0;
};

bool VideoEncoderAdapterImplFuzzTest(FuzzedDataProvider* fdp)
{
    VideoEncoderAdapterImpl videoEncoderAdapterImpl;
    std::shared_ptr<CodecCallbackAdapterMock> callbackImpl =
        std::make_shared<CodecCallbackAdapterMock>();
    std::shared_ptr<CodecConfigParaAdapterMock> config =
        std::make_shared<CodecConfigParaAdapterMock>();

    std::string mimeParam = fdp->ConsumeRandomLengthString(MAX_STRING_LENGTH);
    videoEncoderAdapterImpl.CreateVideoCodecByMime(mimeParam);
    videoEncoderAdapterImpl.SetCodecCallback(nullptr);
    videoEncoderAdapterImpl.SetCodecCallback(callbackImpl);
    videoEncoderAdapterImpl.Configure(nullptr);
    videoEncoderAdapterImpl.Configure(config);
    videoEncoderAdapterImpl.Prepare();
    videoEncoderAdapterImpl.Start();
    videoEncoderAdapterImpl.Stop();
    videoEncoderAdapterImpl.Reset();
    videoEncoderAdapterImpl.Release();
    videoEncoderAdapterImpl.CreateInputSurface();
    videoEncoderAdapterImpl.ReleaseOutputBuffer(0, true);
    videoEncoderAdapterImpl.RequestKeyFrameSoon();

    int index = fdp->ConsumeIntegralInRange<int>(0, 4);
    videoEncoderAdapterImpl.GetBufferFlag(CODEC_BUFFER_FLAG_VECTOR[index]);

    std::shared_ptr<CodecCallbackAdapterMock> cb = std::make_shared<CodecCallbackAdapterMock>();
    NWeb::EncoderCallbackImpl encoderCallback(cb);
    int32_t errorCode = fdp->ConsumeIntegralInRange<int32_t>(0, 1);
    encoderCallback.OnError(errorCode);

    OH_AVFormat* format = OH_AVFormat_Create();
    encoderCallback.OnOutputFormatChanged(format);
    OH_AVFormat_Destroy(format);

    int32_t capacity = fdp->ConsumeIntegralInRange<int32_t>(1, 8);
    int32_t bufferIndex = fdp->ConsumeIntegralInRange<int32_t>(1, 100);
    OH_AVBuffer* buffer = OH_AVBuffer_Create(capacity);
    encoderCallback.OnInputBufferAvailable(bufferIndex, buffer);
    encoderCallback.OnOutputBufferAvailable(bufferIndex, buffer);
    OH_AVBuffer_Destroy(buffer);

    return true;
}

}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    /* Run your code on data */
    FuzzedDataProvider fdp(data, size);
    OHOS::VideoEncoderAdapterImplFuzzTest(&fdp);
 
    return 0;
}