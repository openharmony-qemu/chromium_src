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

#ifndef ARK_WEB_PRINT_ATTRIBUTES_ADAPTER_IMPL_H_
#define ARK_WEB_PRINT_ATTRIBUTES_ADAPTER_IMPL_H_
#pragma once

#include "ohos_nweb/include/ark_web_print_document_adapter_adapter.h"
#include "include/nweb_print_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkWebPrintAttributesAdapterImpl : public ArkWebPrintAttributesAdapter {
public:
    explicit ArkWebPrintAttributesAdapterImpl(std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> ref)
        : ref_(ref) {}
    ~ArkWebPrintAttributesAdapterImpl() = default;
    virtual bool GetBool(uint32_t attrId) override;
    virtual uint32_t GetUInt32(uint32_t attrId) override;
    virtual ArkWebString GetString(uint32_t attrId) override;
    virtual ArkWebUint32Vector GetUint32Vector(uint32_t attrId) override;
private:
    std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> ref_;

    IMPLEMENT_REFCOUNTING(ArkWebPrintAttributesAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_PRINT_ATTRIBUTES_ADAPTER_IMPL_H_