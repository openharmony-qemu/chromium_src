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

#ifndef ARK_PASTE_DATA_ADAPTER_WRAPPER_H
#define ARK_PASTE_DATA_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkPasteDataAdapterWrapper : public NWeb::PasteDataAdapter {
public:
    explicit ArkPasteDataAdapterWrapper(ArkWebRefPtr<ArkPasteDataAdapter>);

    void AddHtmlRecord(const std::string& html) override;

    void AddTextRecord(const std::string& text) override;

    std::vector<std::string> GetMimeTypes() override;

    std::shared_ptr<std::string> GetPrimaryHtml() override;

    std::shared_ptr<std::string> GetPrimaryText() override;

    std::shared_ptr<std::string> GetPrimaryMimeType() override;

    std::shared_ptr<NWeb::PasteDataRecordAdapter> GetRecordAt(std::size_t index) override;

    std::size_t GetRecordCount() override;

    NWeb::PasteRecordVector AllRecords() override;

private:
    ArkWebRefPtr<ArkPasteDataAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_DATA_ADAPTER_WRAPPER_H
