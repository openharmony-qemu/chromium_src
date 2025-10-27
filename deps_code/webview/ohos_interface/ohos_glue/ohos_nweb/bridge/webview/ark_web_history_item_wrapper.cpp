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

#include "ohos_nweb/bridge/ark_web_history_item_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebHistoryItemWrapper::ArkWebHistoryItemWrapper(ArkWebRefPtr<ArkWebHistoryItem> ark_web_history_item)
    : ark_web_history_item_(ark_web_history_item)
{}

bool ArkWebHistoryItemWrapper::GetFavicon(
    void** data, int& width, int& height, ArkWebImageColorType& color_type, ArkWebImageAlphaType& alpha_type)
{
    int enum_color_type = static_cast<int>(ArkWebImageColorType::COLOR_TYPE_UNKNOWN);
    int enum_alpha_type = static_cast<int>(ArkWebImageAlphaType::ALPHA_TYPE_UNKNOWN);
    bool result = ark_web_history_item_->GetFavicon(data, width, height, enum_color_type, enum_alpha_type);
    color_type = static_cast<ArkWebImageColorType>(enum_color_type);
    alpha_type = static_cast<ArkWebImageAlphaType>(enum_alpha_type);
    return result;
}

std::string ArkWebHistoryItemWrapper::GetHistoryUrl()
{
    ArkWebString stUrl = ark_web_history_item_->GetHistoryUrl();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

std::string ArkWebHistoryItemWrapper::GetHistoryTitle()
{
    ArkWebString stTitle = ark_web_history_item_->GetHistoryTitle();

    std::string objTitle = ArkWebStringStructToClass(stTitle);
    ArkWebStringStructRelease(stTitle);
    return objTitle;
}

std::string ArkWebHistoryItemWrapper::GetHistoryRawUrl()
{
    ArkWebString stUrl = ark_web_history_item_->GetHistoryRawUrl();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

} // namespace OHOS::ArkWeb
