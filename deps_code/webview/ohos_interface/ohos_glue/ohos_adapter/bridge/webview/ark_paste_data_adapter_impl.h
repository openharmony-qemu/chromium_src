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

#ifndef ARK_PASTE_DATA_ADAPTER_IMPL_H
#define ARK_PASTE_DATA_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkPasteDataAdapterImpl : public ArkPasteDataAdapter {
public:
    explicit ArkPasteDataAdapterImpl(std::shared_ptr<OHOS::NWeb::PasteDataAdapter>);

    void AddHtmlRecord(const ArkWebString& html) override;

    void AddTextRecord(const ArkWebString& text) override;

    ArkWebStringVector GetMimeTypes() override;

    void GetPrimaryHtml(void* data) override;

    void GetPrimaryText(void* data) override;

    void GetPrimaryMimeType(void* data) override;

    ArkWebRefPtr<ArkPasteDataRecordAdapter> GetRecordAt(size_t index) override;

    size_t GetRecordCount() override;

    ArkPasteRecordVector AllRecords() override;

private:
    std::shared_ptr<OHOS::NWeb::PasteDataAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkPasteDataAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_DATA_ADAPTER_IMPL_H
