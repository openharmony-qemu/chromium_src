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

#ifndef ARK_PASTE_DATA_RECORD_ADAPTER_WRAPPER_H
#define ARK_PASTE_DATA_RECORD_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkPasteDataRecordAdapterWrapper : public NWeb::PasteDataRecordAdapter {
public:
    explicit ArkPasteDataRecordAdapterWrapper(ArkWebRefPtr<ArkPasteDataRecordAdapter>);

    bool SetHtmlText(std::shared_ptr<std::string> htmlText) override;

    bool SetPlainText(std::shared_ptr<std::string> plainText) override;

    bool SetImgData(std::shared_ptr<NWeb::ClipBoardImageDataAdapter> imageData) override;

    std::string GetMimeType() override;

    std::shared_ptr<std::string> GetHtmlText() override;

    std::shared_ptr<std::string> GetPlainText() override;

    bool GetImgData(std::shared_ptr<NWeb::ClipBoardImageDataAdapter> imageData) override;

    bool SetUri(const std::string& uriString) override;

    bool SetCustomData(NWeb::PasteCustomData& data) override;

    std::shared_ptr<std::string> GetUri() override;

    std::shared_ptr<NWeb::PasteCustomData> GetCustomData() override;

    bool SetHtmlTextV2(std::shared_ptr<std::string> htmlText) override;

    bool SetPlainTextV2(std::shared_ptr<std::string> plainText) override;

    std::shared_ptr<std::string> GetHtmlTextV2() override;

    std::shared_ptr<std::string> GetPlainTextV2() override;

    bool SetCustomDataV2(NWeb::PasteCustomData& data) override;

    std::shared_ptr<std::string> GetUriV2() override;

    std::shared_ptr<NWeb::PasteCustomData> GetCustomDataV2() override;

    ArkWebRefPtr<ArkPasteDataRecordAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_DATA_RECORD_ADAPTER_WRAPPER_H
