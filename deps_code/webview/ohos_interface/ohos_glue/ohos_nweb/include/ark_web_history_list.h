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

#ifndef ARK_WEB_HISTORY_LIST_H_
#define ARK_WEB_HISTORY_LIST_H_
#pragma once

#include "ohos_nweb/include/ark_web_history_item.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebHistoryList : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebHistoryItem> GetItem(int32_t index) = 0;

    /*--ark web()--*/
    virtual int32_t GetListSize() = 0;

    /*--ark web()--*/
    virtual int32_t GetCurrentIndex() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HISTORY_LIST_H_
