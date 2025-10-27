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

#ifndef ARK_PASTE_BOARD_CLIENT_ADAPTER_WRAPPER_H
#define ARK_PASTE_BOARD_CLIENT_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkPasteBoardClientAdapterWrapper : public NWeb::PasteBoardClientAdapter {
public:
    explicit ArkPasteBoardClientAdapterWrapper(ArkWebRefPtr<ArkPasteBoardClientAdapter>);

    bool GetPasteData(NWeb::PasteRecordVector& data) override;

    void SetPasteData(const NWeb::PasteRecordVector& data, NWeb::CopyOptionMode copyOption) override;

    bool HasPasteData() override;

    void Clear() override;

    int32_t OpenRemoteUri(const std::string& path) override;

    bool IsLocalPaste() override;

    uint32_t GetTokenId() override;

    int32_t AddPasteboardChangedObserver(std::shared_ptr<NWeb::PasteboardObserverAdapter> callback) override;

    void RemovePasteboardChangedObserver(int32_t callbackId) override;

private:
    ArkWebRefPtr<ArkPasteBoardClientAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_BOARD_CLIENT_ADAPTER_WRAPPER_H
