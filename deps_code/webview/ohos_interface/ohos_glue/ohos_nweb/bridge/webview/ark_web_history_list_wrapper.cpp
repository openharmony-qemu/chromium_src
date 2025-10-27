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

#include "ohos_nweb/bridge/ark_web_history_list_wrapper.h"

#include "ohos_nweb/bridge/ark_web_history_item_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebHistoryListWrapper::ArkWebHistoryListWrapper(ArkWebRefPtr<ArkWebHistoryList> ark_web_history_list)
    : ark_web_history_list_(ark_web_history_list)
{}

std::shared_ptr<OHOS::NWeb::NWebHistoryItem> ArkWebHistoryListWrapper::GetItem(int32_t index)
{
    ArkWebRefPtr<ArkWebHistoryItem> ark_web_history_item = ark_web_history_list_->GetItem(index);
    if (CHECK_REF_PTR_IS_NULL(ark_web_history_item)) {
        return nullptr;
    }

    return std::make_shared<ArkWebHistoryItemWrapper>(ark_web_history_item);
}

int32_t ArkWebHistoryListWrapper::GetListSize()
{
    return ark_web_history_list_->GetListSize();
}

int32_t ArkWebHistoryListWrapper::GetCurrentIndex()
{
    return ark_web_history_list_->GetCurrentIndex();
}

} // namespace OHOS::ArkWeb
