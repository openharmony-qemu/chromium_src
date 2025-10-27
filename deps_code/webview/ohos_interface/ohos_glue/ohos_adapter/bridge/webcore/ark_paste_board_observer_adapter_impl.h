/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_PASTE_BOARD_OBSERVER_ADAPTER_IMPL_H
#define ARK_PASTE_BOARD_OBSERVER_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkPasteBoardObserverAdapterImpl : public ArkPasteBoardObserverAdapter {
public:
    explicit ArkPasteBoardObserverAdapterImpl(std::shared_ptr<OHOS::NWeb::PasteboardObserverAdapter>);

    void OnPasteBoardChanged() override;

private:
    std::shared_ptr<OHOS::NWeb::PasteboardObserverAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkPasteBoardObserverAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_BOARD_OBSERVER_ADAPTER_IMPL_H
