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

#include "ohos_adapter/bridge/ark_encoder_callback_adapter_impl.h"

#include "ohos_adapter/bridge/ark_buffer_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_codec_format_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_buffer_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkEncoderCallbackAdapterImpl::ArkEncoderCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::CodecCallbackAdapter> ref)
    : real_(ref)
{}

void ArkEncoderCallbackAdapterImpl::OnError(int32_t errorType, int32_t errorCode)
{
    real_->OnError((OHOS::NWeb::ErrorType)errorType, errorCode);
}

void ArkEncoderCallbackAdapterImpl::OnStreamChanged(const ArkWebRefPtr<ArkCodecFormatAdapter> format)
{
    if (CHECK_REF_PTR_IS_NULL(format)) {
        return real_->OnStreamChanged(nullptr);
    }

    real_->OnStreamChanged(std::make_shared<ArkCodecFormatAdapterWrapper>(format));
}

void ArkEncoderCallbackAdapterImpl::OnNeedInputData(uint32_t index, ArkWebRefPtr<ArkOhosBufferAdapter> buffer)
{
    if (CHECK_REF_PTR_IS_NULL(buffer)) {
        return real_->OnNeedInputData(index, nullptr);
    }

    real_->OnNeedInputData(index, std::make_shared<ArkOhosBufferAdapterWrapper>(buffer));
}

void ArkEncoderCallbackAdapterImpl::OnNeedOutputData(
    uint32_t index, ArkWebRefPtr<ArkBufferInfoAdapter> info, int32_t flag, ArkWebRefPtr<ArkOhosBufferAdapter> buffer)
{
    if (CHECK_REF_PTR_IS_NULL(info) && CHECK_REF_PTR_IS_NULL(buffer)) {
        real_->OnNeedOutputData(index, nullptr, (OHOS::NWeb::BufferFlag)flag, nullptr);
    } else if (!CHECK_REF_PTR_IS_NULL(info) && !CHECK_REF_PTR_IS_NULL(buffer)) {
        real_->OnNeedOutputData(index, std::make_shared<ArkBufferInfoAdapterWrapper>(info),
            (OHOS::NWeb::BufferFlag)flag, std::make_shared<ArkOhosBufferAdapterWrapper>(buffer));
    } else if (CHECK_REF_PTR_IS_NULL(info)) {
        real_->OnNeedOutputData(
            index, nullptr, (OHOS::NWeb::BufferFlag)flag, std::make_shared<ArkOhosBufferAdapterWrapper>(buffer));
    } else {
        real_->OnNeedOutputData(
            index, std::make_shared<ArkBufferInfoAdapterWrapper>(info), (OHOS::NWeb::BufferFlag)flag, nullptr);
    }
}

} // namespace OHOS::ArkWeb
