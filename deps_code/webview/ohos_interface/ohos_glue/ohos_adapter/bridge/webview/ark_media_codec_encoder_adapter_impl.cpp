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

#include "ohos_adapter/bridge/ark_media_codec_encoder_adapter_impl.h"

#include "ohos_adapter/bridge/ark_capability_data_adapter_impl.h"
#include "ohos_adapter/bridge/ark_codec_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_codec_config_para_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_producer_surface_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaCodecEncoderAdapterImpl::ArkMediaCodecEncoderAdapterImpl(std::shared_ptr<OHOS::NWeb::MediaCodecAdapter> ref)
    : real_(ref)
{}

int32_t ArkMediaCodecEncoderAdapterImpl::CreateVideoCodecByMime(const ArkWebString mimetype)
{
    return (int32_t)real_->CreateVideoCodecByMime(ArkWebStringStructToClass(mimetype));
}

int32_t ArkMediaCodecEncoderAdapterImpl::CreateVideoCodecByName(const ArkWebString name)
{
    return (int32_t)real_->CreateVideoCodecByName(ArkWebStringStructToClass(name));
}

int32_t ArkMediaCodecEncoderAdapterImpl::SetCodecCallback(const ArkWebRefPtr<ArkCodecCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return (int32_t)real_->SetCodecCallback(nullptr);
    }

    return (int32_t)real_->SetCodecCallback(std::make_shared<ArkCodecCallbackAdapterWapper>(callback));
}

int32_t ArkMediaCodecEncoderAdapterImpl::Configure(const ArkWebRefPtr<ArkCodecConfigParaAdapter> config)
{
    if (CHECK_REF_PTR_IS_NULL(config)) {
        return (int32_t)real_->Configure(nullptr);
    }
    return (int32_t)real_->Configure(std::make_shared<ArkCodecConfigParaAdapterWrapper>(config));
}

int32_t ArkMediaCodecEncoderAdapterImpl::Prepare()
{
    return (int32_t)real_->Prepare();
}

int32_t ArkMediaCodecEncoderAdapterImpl::Start()
{
    return (int32_t)real_->Start();
}

int32_t ArkMediaCodecEncoderAdapterImpl::Stop()
{
    return (int32_t)real_->Stop();
}

int32_t ArkMediaCodecEncoderAdapterImpl::Reset()
{
    return (int32_t)real_->Reset();
}

int32_t ArkMediaCodecEncoderAdapterImpl::Release()
{
    return (int32_t)real_->Release();
}

ArkWebRefPtr<ArkProducerSurfaceAdapter> ArkMediaCodecEncoderAdapterImpl::CreateInputSurface()
{
    std::shared_ptr<NWeb::ProducerSurfaceAdapter> surface = real_->CreateInputSurface();
    if (CHECK_SHARED_PTR_IS_NULL(surface)) {
        return nullptr;
    }

    return new ArkProducerSurfaceAdapterImpl(surface);
}

int32_t ArkMediaCodecEncoderAdapterImpl::ReleaseOutputBuffer(uint32_t index, bool isRender)
{
    return (int32_t)real_->ReleaseOutputBuffer(index, isRender);
}

int32_t ArkMediaCodecEncoderAdapterImpl::RequestKeyFrameSoon()
{
    return (int32_t)real_->RequestKeyFrameSoon();
}

ArkMediaCodecListAdapterImpl::ArkMediaCodecListAdapterImpl(OHOS::NWeb::MediaCodecListAdapter& ref) : real_(ref) {}

ArkWebRefPtr<ArkCapabilityDataAdapter> ArkMediaCodecListAdapterImpl::GetCodecCapability(
    const ArkWebString mime, const bool isCodec)
{
    std::shared_ptr<NWeb::CapabilityDataAdapter> adapter =
        real_.GetCodecCapability(ArkWebStringStructToClass(mime), isCodec);
    if (CHECK_SHARED_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return new ArkCapabilityDataAdapterImpl(adapter);
}

} // namespace OHOS::ArkWeb
