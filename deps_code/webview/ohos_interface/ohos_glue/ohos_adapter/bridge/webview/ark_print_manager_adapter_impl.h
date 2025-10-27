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

#ifndef ARK_PRINT_MANAGER_ADAPTER_IMPL_H
#define ARK_PRINT_MANAGER_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_print_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkPrintManagerAdapterImpl : public ArkPrintManagerAdapter {
public:
    explicit ArkPrintManagerAdapterImpl(NWeb::PrintManagerAdapter&);

    int32_t StartPrint(
        const ArkWebStringVector& fileList, const ArkWebUint32Vector& fdList, ArkWebString& taskId) override;

    int32_t Print(const ArkWebString& printJobName, const ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> listener,
        const ArkPrintAttributesAdapter& printAttributes) override;

    int32_t Print(const ArkWebString& printJobName, const ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> listener,
        const ArkPrintAttributesAdapter& printAttributes, void* contextToken) override;

private:
    NWeb::PrintManagerAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkPrintManagerAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_PRINT_MANAGER_ADAPTER_IMPL_H
