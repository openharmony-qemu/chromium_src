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

#ifndef ARK_WEB_HISTORY_ITEM_WRAPPER_H_
#define ARK_WEB_HISTORY_ITEM_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "include/nweb_history_list.h"
#include "ohos_nweb/include/ark_web_history_item.h"

namespace OHOS::ArkWeb {

using ArkWebImageColorType = OHOS::NWeb::ImageColorType;
using ArkWebImageAlphaType = OHOS::NWeb::ImageAlphaType;

class ArkWebHistoryItemWrapper : public OHOS::NWeb::NWebHistoryItem {
public:
    ArkWebHistoryItemWrapper(ArkWebRefPtr<ArkWebHistoryItem> ark_web_history_item);
    ~ArkWebHistoryItemWrapper() = default;

    bool GetFavicon(void** data, int& width, int& height, ArkWebImageColorType& color_type,
        ArkWebImageAlphaType& alpha_type) override;

    std::string GetHistoryUrl() override;

    std::string GetHistoryTitle() override;

    std::string GetHistoryRawUrl() override;

private:
    ArkWebRefPtr<ArkWebHistoryItem> ark_web_history_item_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HISTORY_ITEM_WRAPPER_H_
