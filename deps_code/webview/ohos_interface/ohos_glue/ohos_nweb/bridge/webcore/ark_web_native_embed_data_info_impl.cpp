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

#include "ohos_nweb/bridge/ark_web_native_embed_data_info_impl.h"

#include "ohos_nweb/bridge/ark_web_native_embed_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedDataInfoImpl::ArkWebNativeEmbedDataInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedDataInfo> nweb_native_embed_data_info)
    : nweb_native_embed_data_info_(nweb_native_embed_data_info)
{}

int ArkWebNativeEmbedDataInfoImpl::GetStatus()
{
    return static_cast<int>(nweb_native_embed_data_info_->GetStatus());
}

ArkWebString ArkWebNativeEmbedDataInfoImpl::GetEmbedId()
{
    return ArkWebStringClassToStruct(nweb_native_embed_data_info_->GetEmbedId());
}

ArkWebString ArkWebNativeEmbedDataInfoImpl::GetSurfaceId()
{
    return ArkWebStringClassToStruct(nweb_native_embed_data_info_->GetSurfaceId());
}

ArkWebRefPtr<ArkWebNativeEmbedInfo> ArkWebNativeEmbedDataInfoImpl::GetNativeEmbedInfo()
{
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedInfo> nweb_native_embed_info =
        nweb_native_embed_data_info_->GetNativeEmbedInfo();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_native_embed_info)) {
        return nullptr;
    }

    return new ArkWebNativeEmbedInfoImpl(nweb_native_embed_info);
}

} // namespace OHOS::ArkWeb
