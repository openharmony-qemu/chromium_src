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

#ifndef ARK_PASTE_BOARD_CLIENT_ADAPTER_IMPL_H
#define ARK_PASTE_BOARD_CLIENT_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkPasteBoardClientAdapterImpl : public ArkPasteBoardClientAdapter {
public:
    explicit ArkPasteBoardClientAdapterImpl(NWeb::PasteBoardClientAdapter&);

    bool GetPasteData(ArkPasteRecordVector& data) override;

    void SetPasteData(ArkPasteRecordVector& data, int32_t copyOption) override;

    bool HasPasteData() override;

    void Clear() override;

    int32_t OpenRemoteUri(const ArkWebString& path) override;

    bool IsLocalPaste() override;

    uint32_t GetTokenId() override;

    int32_t AddPasteboardChangedObserver(ArkWebRefPtr<ArkPasteBoardObserverAdapter> callback) override;

    void RemovePasteboardChangedObserver(int32_t callbackId) override;

private:
    NWeb::PasteBoardClientAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkPasteBoardClientAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_BOARD_CLIENT_ADAPTER_IMPL_H
