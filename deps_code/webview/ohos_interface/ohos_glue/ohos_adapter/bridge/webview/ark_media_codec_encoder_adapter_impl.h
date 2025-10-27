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

#ifndef ARK_MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H
#define ARK_MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H
#pragma once

#include "media_codec_adapter.h"
#include "ohos_adapter/include/ark_media_codec_adapter.h"

namespace OHOS::ArkWeb {

class ArkMediaCodecEncoderAdapterImpl : public ArkMediaCodecAdapter {
public:
    explicit ArkMediaCodecEncoderAdapterImpl(std::shared_ptr<OHOS::NWeb::MediaCodecAdapter>);

    int32_t CreateVideoCodecByMime(const ArkWebString mimetype) override;

    int32_t CreateVideoCodecByName(const ArkWebString name) override;

    int32_t SetCodecCallback(const ArkWebRefPtr<ArkCodecCallbackAdapter> callback) override;

    int32_t Configure(const ArkWebRefPtr<ArkCodecConfigParaAdapter> config) override;

    int32_t Prepare() override;

    int32_t Start() override;

    int32_t Stop() override;

    int32_t Reset() override;

    int32_t Release() override;

    ArkWebRefPtr<ArkProducerSurfaceAdapter> CreateInputSurface() override;

    int32_t ReleaseOutputBuffer(uint32_t index, bool isRender) override;

    int32_t RequestKeyFrameSoon() override;

private:
    std::shared_ptr<OHOS::NWeb::MediaCodecAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkMediaCodecEncoderAdapterImpl);
};

class ArkMediaCodecListAdapterImpl : public ArkMediaCodecListAdapter {
public:
    ArkMediaCodecListAdapterImpl(OHOS::NWeb::MediaCodecListAdapter&);

    ArkWebRefPtr<ArkCapabilityDataAdapter> GetCodecCapability(const ArkWebString mime, const bool isCodec) override;

private:
    OHOS::NWeb::MediaCodecListAdapter& real_;
    IMPLEMENT_REFCOUNTING(ArkMediaCodecListAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H
