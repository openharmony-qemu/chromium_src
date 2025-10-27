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

#include "ohos_adapter/bridge/ark_media_codec_encoder_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_codec_config_para_adapter_impl.h"
#include "ohos_adapter/bridge/ark_encoder_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_producer_surface_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaCodecEncoderAdapterWrapper::ArkMediaCodecEncoderAdapterWrapper(ArkWebRefPtr<ArkMediaCodecAdapter> ref)
    : ctocpp_(ref)
{}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::CreateVideoCodecByMime(const std::string mimetype)
{
    ArkWebString str = ArkWebStringClassToStruct(mimetype);
    int32_t result = ctocpp_->CreateVideoCodecByMime(str);
    ArkWebStringStructRelease(str);
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::CreateVideoCodecByName(const std::string name)
{
    ArkWebString str = ArkWebStringClassToStruct(name);
    int32_t result = ctocpp_->CreateVideoCodecByName(str);
    ArkWebStringStructRelease(str);
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::Configure(
    const std::shared_ptr<OHOS::NWeb::CodecConfigParaAdapter> config)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(config)) {
        result = ctocpp_->Configure(nullptr);
    } else {
        result = ctocpp_->Configure(new ArkCodecConfigParaAdapterImpl(config));
    }
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::Prepare()
{
    int32_t result = ctocpp_->Prepare();
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::Start()
{
    int32_t result = ctocpp_->Start();
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::Stop()
{
    int32_t result = ctocpp_->Stop();
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::Reset()
{
    int32_t result = ctocpp_->Reset();
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::Release()
{
    int32_t result = ctocpp_->Release();
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::SetCodecCallback(
    const std::shared_ptr<OHOS::NWeb::CodecCallbackAdapter> callback)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        result = ctocpp_->SetCodecCallback(nullptr);
    } else {
        result = ctocpp_->SetCodecCallback(new ArkEncoderCallbackAdapterImpl(callback));
    }

    return (OHOS::NWeb::CodecCodeAdapter)result;
}

std::shared_ptr<OHOS::NWeb::ProducerSurfaceAdapter> ArkMediaCodecEncoderAdapterWrapper::CreateInputSurface()
{
    ArkWebRefPtr<ArkProducerSurfaceAdapter> surface = ctocpp_->CreateInputSurface();
    if (CHECK_REF_PTR_IS_NULL(surface)) {
        return nullptr;
    }

    return std::make_shared<ArkProducerSurfaceAdapterWrapper>(surface);
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::ReleaseOutputBuffer(uint32_t index, bool isRender)
{
    int32_t result = ctocpp_->ReleaseOutputBuffer(index, isRender);
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

OHOS::NWeb::CodecCodeAdapter ArkMediaCodecEncoderAdapterWrapper::RequestKeyFrameSoon()
{
    int32_t result = ctocpp_->RequestKeyFrameSoon();
    return (OHOS::NWeb::CodecCodeAdapter)result;
}

} // namespace OHOS::ArkWeb
