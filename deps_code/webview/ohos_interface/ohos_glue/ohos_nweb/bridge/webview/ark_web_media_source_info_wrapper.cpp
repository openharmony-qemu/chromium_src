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

#include "ohos_nweb/bridge/ark_web_media_source_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebMediaSourceInfoWrapper::ArkWebMediaSourceInfoWrapper(
    ArkWebRefPtr<ArkWebMediaSourceInfo> ark_web_media_source_info)
    : ark_web_media_source_info_(ark_web_media_source_info)
{}

ArkWebSourceType ArkWebMediaSourceInfoWrapper::GetType()
{
    return static_cast<ArkWebSourceType>(ark_web_media_source_info_->GetType());
}

std::string ArkWebMediaSourceInfoWrapper::GetFormat()
{
    ArkWebString stFormat = ark_web_media_source_info_->GetFormat();

    std::string objFormat = ArkWebStringStructToClass(stFormat);
    ArkWebStringStructRelease(stFormat);
    return objFormat;
}

std::string ArkWebMediaSourceInfoWrapper::GetSource()
{
    ArkWebString stSource = ark_web_media_source_info_->GetSource();

    std::string objSource = ArkWebStringStructToClass(stSource);
    ArkWebStringStructRelease(stSource);
    return objSource;
}

} // namespace OHOS::ArkWeb
