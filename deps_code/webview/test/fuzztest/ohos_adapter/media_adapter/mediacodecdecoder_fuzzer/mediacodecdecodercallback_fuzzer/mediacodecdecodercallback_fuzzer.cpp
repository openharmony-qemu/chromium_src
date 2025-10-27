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

#include "mediacodecdecodercallback_fuzzer.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

#include "avsharedmemory.h"
#include "avsharedmemorybase.h"
#include "native_avformat.h"
#include "native_avbuffer.h"
#define private public
#include "media_codec_decoder_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 255;

class DecoderCallbackAdapterMock : public DecoderCallbackAdapter {
public:
    DecoderCallbackAdapterMock() = default;

    ~DecoderCallbackAdapterMock() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override {}

    void OnStreamChanged(int32_t width, int32_t height, double frameRate) override {}

    void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) override {}

    void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag) override {}
};

class DecoderFormatAdapterMock : public DecoderFormatAdapter {
public:
    DecoderFormatAdapterMock() = default;

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    double GetFrameRate() override
    {
        return frameRate;
    }

    void SetWidth(int32_t w) override
    {
        width = w;
    }

    void SetHeight(int32_t h) override
    {
        height = h;
    }

    void SetFrameRate(double fr) override
    {
        frameRate = fr;
    }

    int32_t width;
    int32_t height;
    double frameRate;
};

class AudioCencInfoAdapterMock : public AudioCencInfoAdapter {
public:
    AudioCencInfoAdapterMock() = default;

    uint8_t* GetKeyId() { return nullptr; }

    uint32_t GetKeyIdLen() { return 0; }

    uint8_t* GetIv() { return nullptr; }

    uint32_t GetIvLen() { return 0; }

    uint32_t GetEncryptedBlockCount() { return 0; }

    uint32_t GetAlgo() { return 0; }

    uint32_t GetSkippedBlockCount() { return 0; }

    uint32_t GetFirstEncryptedOffset() { return 0; }

    std::vector<uint32_t> GetClearHeaderLens() { return std::vector<uint32_t>();  }

    std::vector<uint32_t> GetPayLoadLens() { return std::vector<uint32_t>();  }

    uint32_t GetMode() { return 0; }

    void SetKeyId(uint8_t* keyId) {}

    void SetKeyIdLen(uint32_t keyIdLen) {}

    void SetIv(uint8_t* iv) {}

    void SetIvLen(uint32_t ivLen) {}

    void SetAlgo(uint32_t algo) {}

    void SetEncryptedBlockCount(uint32_t encryptedBlockCount) {}

    void SetSkippedBlockCount(uint32_t skippedBlockCount) {}

    void SetFirstEncryptedOffset(uint32_t firstEncryptedOffset) {}

    void SetClearHeaderLens(const std::vector<uint32_t>& clearHeaderLens) {}

    void SetPayLoadLens(const std::vector<uint32_t>& payLoadLens) {}

    void SetMode(uint32_t mode) {}
};

bool MediaCodecDecoderCallBackFuzzTest(const uint8_t* data, size_t size)
{
    NWeb::MediaCodecDecoderAdapterImpl mediaCodecDecoderAdapterImpl;
    NWeb::DecoderAdapterCode code = mediaCodecDecoderAdapterImpl.CreateVideoDecoderByMime("testmimeType");
    std::shared_ptr<NWeb::DecoderFormatAdapter> format = std::make_unique<DecoderFormatAdapterMock>();
    FuzzedDataProvider dataProvider(data, size);
    std::string stringParam = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int32_t intParam = dataProvider.ConsumeIntegralInRange<int32_t>(0, 10000);
    uint32_t uintParam = dataProvider.ConsumeIntegralInRange<uint32_t>(0, 10000);
    auto callback = std::make_shared<DecoderCallbackAdapterMock>();
    mediaCodecDecoderAdapterImpl.SetCallbackDec(callback);
    code = mediaCodecDecoderAdapterImpl.CreateVideoDecoderByName(stringParam);
    code = mediaCodecDecoderAdapterImpl.ConfigureDecoder(format);
    code = mediaCodecDecoderAdapterImpl.SetParameterDecoder(format);

    void* window = nullptr;
    code = mediaCodecDecoderAdapterImpl.SetOutputSurface(window);
    code = mediaCodecDecoderAdapterImpl.PrepareDecoder();
    code = mediaCodecDecoderAdapterImpl.StartDecoder();
    code = mediaCodecDecoderAdapterImpl.StopDecoder();
    code = mediaCodecDecoderAdapterImpl.FlushDecoder();
    code = mediaCodecDecoderAdapterImpl.ResetDecoder();
    code = mediaCodecDecoderAdapterImpl.ReleaseDecoder();
    code = mediaCodecDecoderAdapterImpl.SetAVCencInfo(uintParam, nullptr);
    code = mediaCodecDecoderAdapterImpl.SetDecryptionConfig(nullptr, true);
    code = mediaCodecDecoderAdapterImpl.SetDecryptionConfig(nullptr, false);

    code = mediaCodecDecoderAdapterImpl.QueueInputBufferDec(
        uintParam, 0, intParam, intParam, BufferFlag::CODEC_BUFFER_FLAG_NONE);
    code = mediaCodecDecoderAdapterImpl.GetOutputFormatDec(format);
    code = mediaCodecDecoderAdapterImpl.ReleaseOutputBufferDec(uintParam, true);

    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVFormat* codecFormat = OH_AVFormat_Create();
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
  
    mediaCodecDecoderAdapterImpl.OnError(uintParam);
    mediaCodecDecoderAdapterImpl.OnOutputFormatChanged(nullptr);
    mediaCodecDecoderAdapterImpl.OnInputBufferAvailable(uintParam, buffer);
    mediaCodecDecoderAdapterImpl.OnOutputBufferAvailable(uintParam, buffer);

    mediaCodecDecoderAdapterImpl.OnOutputFormatChanged(codecFormat);
    mediaCodecDecoderAdapterImpl.OnInputBufferAvailable(uintParam, buffer);
    mediaCodecDecoderAdapterImpl.OnOutputBufferAvailable(uintParam, buffer);

    auto cencinfo = std::make_shared<AudioCencInfoAdapterMock>();
    mediaCodecDecoderAdapterImpl.SetAVCencInfo(intParam, cencinfo);
    OH_AVFormat_Destroy(codecFormat);
    OH_AVBuffer_Destroy(buffer);
    codecFormat = nullptr;
    buffer = nullptr;
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::MediaCodecDecoderCallBackFuzzTest(data, size);
    return 0;
}