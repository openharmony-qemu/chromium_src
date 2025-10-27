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

#ifndef MEDIA_CODEC_ADAPTER_H
#define MEDIA_CODEC_ADAPTER_H

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include "graphic_adapter.h"

namespace OHOS::NWeb {

enum class CodecCodeAdapter : int32_t { OK = 0, ERROR = 1, RETRY = 2 };

class CapabilityDataAdapter {
public:
    CapabilityDataAdapter() = default;

    virtual ~CapabilityDataAdapter() = default;

    virtual int32_t GetMaxWidth() = 0;

    virtual int32_t GetMaxHeight() = 0;

    virtual int32_t GetMaxframeRate() = 0;
};

class CodecFormatAdapter {
public:
    CodecFormatAdapter() = default;

    virtual ~CodecFormatAdapter() = default;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;
};

enum class ErrorType : int32_t {
    CODEC_ERROR_INTERNAL,
    CODEC_ERROR_EXTEND_START = 0X10000,
};

class BufferInfoAdapter {
public:
    BufferInfoAdapter() = default;

    virtual ~BufferInfoAdapter() = default;

    virtual int64_t GetPresentationTimeUs() = 0;

    virtual int32_t GetSize() = 0;

    virtual int32_t GetOffset() = 0;
};

enum class BufferFlag : uint32_t {
    CODEC_BUFFER_FLAG_NONE = 0,
    CODEC_BUFFER_FLAG_EOS = 1 << 0,
    CODEC_BUFFER_FLAG_SYNC_FRAME = 1 << 1,
    CODEC_BUFFER_FLAG_PARTIAL_FRAME = 1 << 2,
    CODEC_BUFFER_FLAG_CODEC_DATA = 1 << 3,
};

class OhosBufferAdapter {
public:
    OhosBufferAdapter() = default;

    virtual ~OhosBufferAdapter() = default;

    virtual uint8_t* GetAddr() = 0;

    virtual uint32_t GetBufferSize() = 0;
};

class CodecConfigParaAdapter {
public:
    CodecConfigParaAdapter() = default;

    virtual ~CodecConfigParaAdapter() = default;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual int64_t GetBitRate() = 0;

    virtual double GetFrameRate() = 0;
};

class CodecCallbackAdapter {
public:
    CodecCallbackAdapter() = default;

    virtual ~CodecCallbackAdapter() = default;

    virtual void OnError(ErrorType errorType, int32_t errorCode) = 0;

    virtual void OnStreamChanged(const std::shared_ptr<CodecFormatAdapter> format) = 0;

    virtual void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) = 0;

    virtual void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag,
        std::shared_ptr<OhosBufferAdapter> buffer) = 0;
};

class MediaCodecAdapter {
public:
    MediaCodecAdapter() = default;

    virtual ~MediaCodecAdapter() = default;

    virtual CodecCodeAdapter CreateVideoCodecByMime(const std::string mimetype) = 0;

    virtual CodecCodeAdapter CreateVideoCodecByName(const std::string name) = 0;

    virtual CodecCodeAdapter SetCodecCallback(const std::shared_ptr<CodecCallbackAdapter> callback) = 0;

    virtual CodecCodeAdapter Configure(const std::shared_ptr<CodecConfigParaAdapter> config) = 0;

    virtual CodecCodeAdapter Prepare() = 0;

    virtual CodecCodeAdapter Start() = 0;

    virtual CodecCodeAdapter Stop() = 0;

    virtual CodecCodeAdapter Reset() = 0;

    virtual CodecCodeAdapter Release() = 0;

    virtual std::shared_ptr<ProducerSurfaceAdapter> CreateInputSurface() = 0;

    virtual CodecCodeAdapter ReleaseOutputBuffer(uint32_t index, bool isRender) = 0;

    virtual CodecCodeAdapter RequestKeyFrameSoon() = 0;
};

class MediaCodecListAdapter {
public:
    MediaCodecListAdapter() = default;

    virtual ~MediaCodecListAdapter() = default;

    virtual std::shared_ptr<CapabilityDataAdapter> GetCodecCapability(const std::string& mime, const bool isCodec) = 0;
};

} // namespace OHOS::NWeb

#endif