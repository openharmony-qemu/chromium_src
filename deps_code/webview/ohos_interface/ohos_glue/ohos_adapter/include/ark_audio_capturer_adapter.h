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

#ifndef ARK_AUDIO_CAPTURE_ADAPTER_H
#define ARK_AUDIO_CAPTURE_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkAudioCapturerOptionsAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetSamplingRate() = 0;

    /*--ark web()--*/
    virtual int32_t GetEncoding() = 0;

    /*--ark web()--*/
    virtual int32_t GetSampleFormat() = 0;

    /*--ark web()--*/
    virtual int32_t GetChannels() = 0;

    /*--ark web()--*/
    virtual int32_t GetSourceType() = 0;

    /*--ark web()--*/
    virtual int32_t GetCapturerFlags() = 0;
};

/*--ark web(source=webcore)--*/
class ArkBufferDescAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint8_t* GetBuffer() = 0;

    /*--ark web()--*/
    virtual size_t GetBufLength() = 0;

    /*--ark web()--*/
    virtual size_t GetDataLength() = 0;

    /*--ark web()--*/
    virtual void SetBuffer(uint8_t* buffer) = 0;

    /*--ark web()--*/
    virtual void SetBufLength(size_t bufLength) = 0;

    /*--ark web()--*/
    virtual void SetDataLength(size_t dataLength) = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioCapturerReadCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnReadData(size_t length) = 0;
};

/*--ark web(source=webview)--*/
class ArkAudioCapturerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t Create(
        const ArkWebRefPtr<ArkAudioCapturerOptionsAdapter> capturerOptions, ArkWebString& cachePath) = 0;

    /*--ark web()--*/
    virtual bool Start() = 0;

    /*--ark web()--*/
    virtual bool Stop() = 0;

    /*--ark web()--*/
    virtual bool Release2() = 0;

    /*--ark web()--*/
    virtual int32_t SetCapturerReadCallback(ArkWebRefPtr<ArkAudioCapturerReadCallbackAdapter> callbck) = 0;

    /*--ark web()--*/
    virtual int32_t GetBufferDesc(ArkWebRefPtr<ArkBufferDescAdapter> buffferDesc) = 0;

    /*--ark web()--*/
    virtual int32_t Enqueue(const ArkWebRefPtr<ArkBufferDescAdapter> bufferDesc) = 0;

    /*--ark web()--*/
    virtual int32_t GetFrameCount(uint32_t& frameCount) = 0;

    /*--ark web()--*/
    virtual int64_t GetAudioTime() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CAPTURE_ADAPTER_H
