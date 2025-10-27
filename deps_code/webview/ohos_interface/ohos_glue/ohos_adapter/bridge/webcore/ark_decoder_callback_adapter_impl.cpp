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

#include "ohos_adapter/bridge/ark_decoder_callback_adapter_impl.h"

#include "ohos_adapter/bridge/ark_buffer_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_buffer_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDecoderCallbackAdapterImpl::ArkDecoderCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::DecoderCallbackAdapter> ref)
    : real_(ref)
{}

void ArkDecoderCallbackAdapterImpl::OnError(int32_t errorType, int32_t errorCode)
{
    real_->OnError((OHOS::NWeb::ErrorType)errorType, errorCode);
}

void ArkDecoderCallbackAdapterImpl::OnStreamChanged(int32_t width, int32_t height, double frameRate)
{
    real_->OnStreamChanged(width, height, frameRate);
}

void ArkDecoderCallbackAdapterImpl::OnNeedInputData(uint32_t index, ArkWebRefPtr<ArkOhosBufferAdapter> buffer)
{
    if (CHECK_REF_PTR_IS_NULL(buffer)) {
        return real_->OnNeedInputData(index, nullptr);
    }
    real_->OnNeedInputData(index, std::make_shared<ArkOhosBufferAdapterWrapper>(buffer));
}

void ArkDecoderCallbackAdapterImpl::OnNeedOutputData(
    uint32_t index, ArkWebRefPtr<ArkBufferInfoAdapter> info, uint32_t flag)
{
    if (CHECK_REF_PTR_IS_NULL(info)) {
        return real_->OnNeedOutputData(index, nullptr, (OHOS::NWeb::BufferFlag)flag);
    }
    real_->OnNeedOutputData(index, std::make_shared<ArkBufferInfoAdapterWrapper>(info), (OHOS::NWeb::BufferFlag)flag);
}

} // namespace OHOS::ArkWeb
