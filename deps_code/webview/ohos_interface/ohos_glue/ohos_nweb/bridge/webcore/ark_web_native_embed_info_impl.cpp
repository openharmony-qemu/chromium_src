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

#include "ohos_nweb/bridge/ark_web_native_embed_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedInfoImpl::ArkWebNativeEmbedInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedInfo> nweb_native_embed_info)
    : nweb_native_embed_info_(nweb_native_embed_info)
{}

int32_t ArkWebNativeEmbedInfoImpl::GetWidth()
{
    return nweb_native_embed_info_->GetWidth();
}

int32_t ArkWebNativeEmbedInfoImpl::GetHeight()
{
    return nweb_native_embed_info_->GetHeight();
}

ArkWebString ArkWebNativeEmbedInfoImpl::GetId()
{
    return ArkWebStringClassToStruct(nweb_native_embed_info_->GetId());
}

ArkWebString ArkWebNativeEmbedInfoImpl::GetSrc()
{
    return ArkWebStringClassToStruct(nweb_native_embed_info_->GetSrc());
}

ArkWebString ArkWebNativeEmbedInfoImpl::GetUrl()
{
    return ArkWebStringClassToStruct(nweb_native_embed_info_->GetUrl());
}

ArkWebString ArkWebNativeEmbedInfoImpl::GetType()
{
    return ArkWebStringClassToStruct(nweb_native_embed_info_->GetType());
}

ArkWebString ArkWebNativeEmbedInfoImpl::GetTag()
{
    return ArkWebStringClassToStruct(nweb_native_embed_info_->GetTag());
}

ArkWebStringMap ArkWebNativeEmbedInfoImpl::GetParams()
{
    return ArkWebStringMapClassToStruct(nweb_native_embed_info_->GetParams());
}

int32_t ArkWebNativeEmbedInfoImpl::GetX()
{
    return nweb_native_embed_info_->GetX();
}

int32_t ArkWebNativeEmbedInfoImpl::GetY()
{
    return nweb_native_embed_info_->GetY();
}

} // namespace OHOS::ArkWeb
