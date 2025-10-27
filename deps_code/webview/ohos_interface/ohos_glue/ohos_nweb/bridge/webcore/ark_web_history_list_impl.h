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

#ifndef ARK_WEB_HISTORY_LIST_IMPL_H_
#define ARK_WEB_HISTORY_LIST_IMPL_H_
#pragma once

#include "include/nweb_history_list.h"
#include "ohos_nweb/include/ark_web_history_list.h"

namespace OHOS::ArkWeb {

class ArkWebHistoryListImpl : public ArkWebHistoryList {
    IMPLEMENT_REFCOUNTING(ArkWebHistoryListImpl);

public:
    ArkWebHistoryListImpl(std::shared_ptr<OHOS::NWeb::NWebHistoryList> nweb_history_list);
    ~ArkWebHistoryListImpl() = default;

    ArkWebRefPtr<ArkWebHistoryItem> GetItem(int32_t index) override;

    int32_t GetListSize() override;

    int32_t GetCurrentIndex() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebHistoryList> nweb_history_list_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HISTORY_LIST_IMPL_H_
