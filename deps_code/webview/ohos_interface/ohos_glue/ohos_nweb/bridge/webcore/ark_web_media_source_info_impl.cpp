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

#include "ohos_nweb/bridge/ark_web_media_source_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebMediaSourceInfoImpl::ArkWebMediaSourceInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebMediaSourceInfo> nweb_media_source_info)
    : nweb_media_source_info_(nweb_media_source_info)
{}

int ArkWebMediaSourceInfoImpl::GetType()
{
    return static_cast<int>(nweb_media_source_info_->GetType());
}

ArkWebString ArkWebMediaSourceInfoImpl::GetFormat()
{
    return ArkWebStringClassToStruct(nweb_media_source_info_->GetFormat());
}

ArkWebString ArkWebMediaSourceInfoImpl::GetSource()
{
    return ArkWebStringClassToStruct(nweb_media_source_info_->GetSource());
}

} // namespace OHOS::ArkWeb
