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

#ifndef ARK_PASTE_DATA_RECORD_ADAPTER_IMPL_H
#define ARK_PASTE_DATA_RECORD_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkPasteDataRecordAdapterImpl : public ArkPasteDataRecordAdapter {
public:
    explicit ArkPasteDataRecordAdapterImpl(std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>);

    bool SetHtmlText(void* htmlText) override;

    bool SetHtmlTextV2(const ArkWebString& htmlText) override;

    bool SetPlainText(void* plainText) override;

    bool SetPlainTextV2(const ArkWebString& plainText) override;

    bool SetImgData(ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData) override;

    ArkWebString GetMimeType() override;

    void GetHtmlText(void* data) override;

    bool GetHtmlTextV2(ArkWebString& data) override;

    void GetPlainText(void* data) override;

    bool GetPlainTextV2(ArkWebString& data) override;

    bool GetImgData(ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData) override;

    bool SetUri(const ArkWebString& uriString) override;

    bool SetCustomData(void* data) override;

    bool SetCustomDataV2(ArkWebUInt8VectorMap& data) override;

    void GetUri(void* data) override;

    bool GetUriV2(ArkWebString& data) override;

    void GetCustomData(void* data) override;

    bool GetCustomDataV2(ArkWebUInt8VectorMap& data) override;

    std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkPasteDataRecordAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTEBOARD_CLIENT_ADAPTER_H
