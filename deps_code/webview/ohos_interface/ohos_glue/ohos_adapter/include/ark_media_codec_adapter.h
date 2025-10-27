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

#ifndef ARK_MEDIA_CODEC_ENCODER_ADAPTER_H
#define ARK_MEDIA_CODEC_ENCODER_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkCapabilityDataAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetMaxWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetMaxHeight() = 0;

    /*--ark web()--*/
    virtual int32_t GetMaxframeRate() = 0;
};

/*--ark web(source=webview)--*/
class ArkCodecFormatAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;
};

/*--ark web(source=webview)--*/
class ArkBufferInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int64_t GetPresentationTimeUs() = 0;

    /*--ark web()--*/
    virtual int32_t GetSize() = 0;

    /*--ark web()--*/
    virtual int32_t GetOffset() = 0;
};

/*--ark web(source=webview)--*/
class ArkOhosBufferAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint8_t* GetAddr() = 0;

    /*--ark web()--*/
    virtual uint32_t GetBufferSize() = 0;
};

/*--ark web(source=webcore)--*/
class ArkCodecConfigParaAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual int64_t GetBitRate() = 0;

    /*--ark web()--*/
    virtual double GetFrameRate() = 0;
};

/*--ark web(source=webcore)--*/
class ArkCodecCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnError(int32_t errorType, int32_t errorCode) = 0;

    /*--ark web()--*/
    virtual void OnStreamChanged(const ArkWebRefPtr<ArkCodecFormatAdapter> format) = 0;

    /*--ark web()--*/
    virtual void OnNeedInputData(uint32_t index, ArkWebRefPtr<ArkOhosBufferAdapter> buffer) = 0;

    /*--ark web()--*/
    virtual void OnNeedOutputData(uint32_t index, ArkWebRefPtr<ArkBufferInfoAdapter> info, int32_t flag,
        ArkWebRefPtr<ArkOhosBufferAdapter> buffer) = 0;
};

/*--ark web(source=webview)--*/
class ArkMediaCodecAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t CreateVideoCodecByMime(const ArkWebString mimetype) = 0;

    /*--ark web()--*/
    virtual int32_t CreateVideoCodecByName(const ArkWebString name) = 0;

    /*--ark web()--*/
    virtual int32_t SetCodecCallback(const ArkWebRefPtr<ArkCodecCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual int32_t Configure(const ArkWebRefPtr<ArkCodecConfigParaAdapter> config) = 0;

    /*--ark web()--*/
    virtual int32_t Prepare() = 0;

    /*--ark web()--*/
    virtual int32_t Start() = 0;

    /*--ark web()--*/
    virtual int32_t Stop() = 0;

    /*--ark web()--*/
    virtual int32_t Reset() = 0;

    /*--ark web()--*/
    virtual int32_t Release() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkProducerSurfaceAdapter> CreateInputSurface() = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseOutputBuffer(uint32_t index, bool isRender) = 0;

    /*--ark web()--*/
    virtual int32_t RequestKeyFrameSoon() = 0;
};

/*--ark web(source=webview)--*/
class ArkMediaCodecListAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkCapabilityDataAdapter> GetCodecCapability(const ArkWebString mime, const bool isCodec) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_CODEC_ENCODER_ADAPTER_H
