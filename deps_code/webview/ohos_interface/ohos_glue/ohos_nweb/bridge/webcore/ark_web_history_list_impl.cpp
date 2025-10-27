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

#include "ohos_nweb/bridge/ark_web_history_list_impl.h"

#include "ohos_nweb/bridge/ark_web_history_item_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebHistoryListImpl::ArkWebHistoryListImpl(std::shared_ptr<OHOS::NWeb::NWebHistoryList> nweb_history_list)
    : nweb_history_list_(nweb_history_list)
{}

ArkWebRefPtr<ArkWebHistoryItem> ArkWebHistoryListImpl::GetItem(int32_t index)
{
    std::shared_ptr<OHOS::NWeb::NWebHistoryItem> nweb_history_item = nweb_history_list_->GetItem(index);
    if (CHECK_SHARED_PTR_IS_NULL(nweb_history_item)) {
        return nullptr;
    }

    return new ArkWebHistoryItemImpl(nweb_history_item);
}

int32_t ArkWebHistoryListImpl::GetListSize()
{
    return nweb_history_list_->GetListSize();
}

int32_t ArkWebHistoryListImpl::GetCurrentIndex()
{
    return nweb_history_list_->GetCurrentIndex();
}

} // namespace OHOS::ArkWeb
