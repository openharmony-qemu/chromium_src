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

#include "ohos_nweb/bridge/ark_web_history_item_impl.h"

#include "include/nweb.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebImageColorType = OHOS::NWeb::ImageColorType;
using ArkWebImageAlphaType = OHOS::NWeb::ImageAlphaType;

ArkWebHistoryItemImpl::ArkWebHistoryItemImpl(std::shared_ptr<OHOS::NWeb::NWebHistoryItem> nweb_history_item)
    : nweb_history_item_(nweb_history_item)
{}

bool ArkWebHistoryItemImpl::GetFavicon(void** data, int& width, int& height, int& color_type, int& alpha_type)
{
    ArkWebImageColorType enum_color_type = ArkWebImageColorType::COLOR_TYPE_UNKNOWN;
    ArkWebImageAlphaType enum_alpha_type = ArkWebImageAlphaType::ALPHA_TYPE_UNKNOWN;
    bool result = nweb_history_item_->GetFavicon(data, width, height, enum_color_type, enum_alpha_type);
    color_type = static_cast<int>(enum_color_type);
    alpha_type = static_cast<int>(enum_alpha_type);
    return result;
}

ArkWebString ArkWebHistoryItemImpl::GetHistoryUrl()
{
    return ArkWebStringClassToStruct(nweb_history_item_->GetHistoryUrl());
}

ArkWebString ArkWebHistoryItemImpl::GetHistoryTitle()
{
    return ArkWebStringClassToStruct(nweb_history_item_->GetHistoryTitle());
}

ArkWebString ArkWebHistoryItemImpl::GetHistoryRawUrl()
{
    return ArkWebStringClassToStruct(nweb_history_item_->GetHistoryRawUrl());
}

} // namespace OHOS::ArkWeb
