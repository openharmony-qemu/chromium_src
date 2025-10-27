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

#ifndef ARK_WEB_HISTORY_ITEM_IMPL_H_
#define ARK_WEB_HISTORY_ITEM_IMPL_H_
#pragma once

#include "include/nweb_history_list.h"
#include "ohos_nweb/include/ark_web_history_item.h"

namespace OHOS::ArkWeb {

class ArkWebHistoryItemImpl : public ArkWebHistoryItem {
    IMPLEMENT_REFCOUNTING(ArkWebHistoryItemImpl);

public:
    ArkWebHistoryItemImpl(std::shared_ptr<OHOS::NWeb::NWebHistoryItem> nweb_history_item);
    ~ArkWebHistoryItemImpl() = default;

    bool GetFavicon(void** data, int& width, int& height, int& color_type, int& alpha_type) override;

    ArkWebString GetHistoryUrl() override;

    ArkWebString GetHistoryTitle() override;

    ArkWebString GetHistoryRawUrl() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebHistoryItem> nweb_history_item_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HISTORY_ITEM_IMPL_H_
