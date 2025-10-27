/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_PRINT_DOCUMENT_ADAPTER_ADAPTER_WRAPPER_H
#define ARK_PRINT_DOCUMENT_ADAPTER_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_print_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkPrintDocumentAdapterAdapterWrapper : public OHOS::NWeb::PrintDocumentAdapterAdapter {
public:
    explicit ArkPrintDocumentAdapterAdapterWrapper(ArkWebRefPtr<ArkPrintDocumentAdapterAdapter>);

    void OnStartLayoutWrite(const std::string& jobId, const NWeb::PrintAttributesAdapter& oldAttrs,
        const NWeb::PrintAttributesAdapter& newAttrs, uint32_t fd,
        std::shared_ptr<NWeb::PrintWriteResultCallbackAdapter> callback) override;

    void OnJobStateChanged(const std::string& jobId, uint32_t state) override;

private:
    ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PRINT_DOCUMENT_ADAPTER_ADAPTER_WRAPPER_H
