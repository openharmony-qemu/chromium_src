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

#include "ohos_nweb/bridge/ark_web_native_embed_data_info_wrapper.h"

#include "ohos_nweb/bridge/ark_web_native_embed_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedDataInfoWrapper::ArkWebNativeEmbedDataInfoWrapper(
    ArkWebRefPtr<ArkWebNativeEmbedDataInfo> ark_web_native_embed_data_info)
    : ark_web_native_embed_data_info_(ark_web_native_embed_data_info)
{}

ArkWebNativeEmbedStatus ArkWebNativeEmbedDataInfoWrapper::GetStatus()
{
    return static_cast<ArkWebNativeEmbedStatus>(ark_web_native_embed_data_info_->GetStatus());
}

std::string ArkWebNativeEmbedDataInfoWrapper::GetEmbedId()
{
    ArkWebString stEmbedId = ark_web_native_embed_data_info_->GetEmbedId();

    std::string objEmbedId = ArkWebStringStructToClass(stEmbedId);
    ArkWebStringStructRelease(stEmbedId);
    return objEmbedId;
}

std::string ArkWebNativeEmbedDataInfoWrapper::GetSurfaceId()
{
    ArkWebString stSurfaceId = ark_web_native_embed_data_info_->GetSurfaceId();

    std::string objSurfaceId = ArkWebStringStructToClass(stSurfaceId);
    ArkWebStringStructRelease(stSurfaceId);
    return objSurfaceId;
}

std::shared_ptr<OHOS::NWeb::NWebNativeEmbedInfo> ArkWebNativeEmbedDataInfoWrapper::GetNativeEmbedInfo()
{
    ArkWebRefPtr<ArkWebNativeEmbedInfo> ark_web_native_embed_info =
        ark_web_native_embed_data_info_->GetNativeEmbedInfo();
    if (CHECK_REF_PTR_IS_NULL(ark_web_native_embed_info)) {
        return nullptr;
    }

    return std::make_shared<ArkWebNativeEmbedInfoWrapper>(ark_web_native_embed_info);
}

} // namespace OHOS::ArkWeb
