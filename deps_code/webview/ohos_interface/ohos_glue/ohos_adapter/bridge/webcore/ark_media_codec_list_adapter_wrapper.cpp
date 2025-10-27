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

#include "ohos_adapter/bridge/ark_media_codec_list_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_capability_data_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaCodecListAdapterWrapper::ArkMediaCodecListAdapterWrapper(ArkWebRefPtr<ArkMediaCodecListAdapter> ref)
    : ctocpp_(ref)
{}

std::shared_ptr<OHOS::NWeb::CapabilityDataAdapter> ArkMediaCodecListAdapterWrapper::GetCodecCapability(
    const std::string& mime, const bool isCodec)
{
    if (!ctocpp_) {
        return nullptr;
    }
    ArkWebString str = ArkWebStringClassToStruct(mime);
    ArkWebRefPtr<ArkCapabilityDataAdapter> adapter = ctocpp_->GetCodecCapability(str, isCodec);
    ArkWebStringStructRelease(str);

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkCapabilityDataAdapterWrapper>(adapter);
}

} // namespace OHOS::ArkWeb
