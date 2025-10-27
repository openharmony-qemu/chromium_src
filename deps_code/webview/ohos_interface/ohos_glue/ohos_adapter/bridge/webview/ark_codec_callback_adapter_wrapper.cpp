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

#include "ohos_adapter/bridge/ark_codec_callback_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_buffer_info_adapter_impl.h"
#include "ohos_adapter/bridge/ark_codec_format_adapter_impl.h"
#include "ohos_adapter/bridge/ark_ohos_buffer_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkCodecCallbackAdapterWapper::ArkCodecCallbackAdapterWapper(ArkWebRefPtr<ArkCodecCallbackAdapter> ref) : ctocpp_(ref)
{}

void ArkCodecCallbackAdapterWapper::OnError(OHOS::NWeb::ErrorType errorType, int32_t errorCode)
{
    ctocpp_->OnError((int32_t)errorType, errorCode);
}

void ArkCodecCallbackAdapterWapper::OnStreamChanged(const std::shared_ptr<NWeb::CodecFormatAdapter> format)
{
    if (CHECK_SHARED_PTR_IS_NULL(format)) {
        return ctocpp_->OnStreamChanged(nullptr);
    }

    ctocpp_->OnStreamChanged(new ArkCodecFormatAdapterImpl(format));
}

void ArkCodecCallbackAdapterWapper::OnNeedInputData(uint32_t index, std::shared_ptr<NWeb::OhosBufferAdapter> buffer)
{
    if (CHECK_SHARED_PTR_IS_NULL(buffer)) {
        return ctocpp_->OnNeedInputData(index, nullptr);
    }

    ctocpp_->OnNeedInputData(index, new ArkOhosBufferAdapterImpl(buffer));
}

void ArkCodecCallbackAdapterWapper::OnNeedOutputData(uint32_t index, std::shared_ptr<NWeb::BufferInfoAdapter> info,
    NWeb::BufferFlag flag, std::shared_ptr<NWeb::OhosBufferAdapter> buffer)
{
    if (CHECK_SHARED_PTR_IS_NULL(info) && CHECK_SHARED_PTR_IS_NULL(buffer)) {
        ctocpp_->OnNeedOutputData(index, nullptr, (uint32_t)flag, nullptr);
    } else if (!CHECK_SHARED_PTR_IS_NULL(info) && !CHECK_SHARED_PTR_IS_NULL(buffer)) {
        ctocpp_->OnNeedOutputData(
            index, new ArkBufferInfoAdapterImpl(info), (uint32_t)flag, new ArkOhosBufferAdapterImpl(buffer));
    } else if (CHECK_SHARED_PTR_IS_NULL(info)) {
        ctocpp_->OnNeedOutputData(index, nullptr, (uint32_t)flag, new ArkOhosBufferAdapterImpl(buffer));
    } else {
        ctocpp_->OnNeedOutputData(index, new ArkBufferInfoAdapterImpl(info), (uint32_t)flag, nullptr);
    }
}

} // namespace OHOS::ArkWeb
