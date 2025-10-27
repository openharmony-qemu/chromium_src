/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_PRINT_DOCUMENT_ADAPTER_ADAPTER_WRAPPER_H_
#define ARK_WEB_PRINT_DOCUMENT_ADAPTER_ADAPTER_WRAPPER_H_
#pragma once

#include "ohos_nweb/include/ark_web_print_document_adapter_adapter.h"
#include "include/nweb_print_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkWebPrintDocumentAdapterAdapterWrapper : public OHOS::NWeb::NWebPrintDocumentAdapterAdapter {
public:
    explicit ArkWebPrintDocumentAdapterAdapterWrapper(ArkWebRefPtr<ArkWebPrintDocumentAdapterAdapter> ref): ref_(ref) {}

    void OnStartLayoutWrite(const std::string& jobId, std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> oldAttrs,
        std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> newAttrs, uint32_t fd,
        std::shared_ptr<OHOS::NWeb::NWebPrintWriteResultCallbackAdapter> callback) override;

    void OnJobStateChanged(const std::string& jobId, uint32_t state) override;

private:
    ArkWebRefPtr<ArkWebPrintDocumentAdapterAdapter> ref_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_PRINT_DOCUMENT_ADAPTER_ADAPTER_WRAPPER_H_