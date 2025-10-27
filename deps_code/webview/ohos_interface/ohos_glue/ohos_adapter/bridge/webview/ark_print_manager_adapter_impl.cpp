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

#include "ohos_adapter/bridge/ark_print_manager_adapter_impl.h"

#include "ohos_adapter/bridge/ark_print_document_adapter_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPrintManagerAdapterImpl::ArkPrintManagerAdapterImpl(NWeb::PrintManagerAdapter& ref) : real_(ref) {}

int32_t ArkPrintManagerAdapterImpl::StartPrint(
    const ArkWebStringVector& arkFileList, const ArkWebUint32Vector& arkFdList, ArkWebString& arkTaskId)
{
    std::vector<std::string> fileList = ArkWebStringVectorStructToClass(arkFileList);
    std::vector<uint32_t> fdList = ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>(arkFdList);
    std::string taskId;
    int32_t result = real_.StartPrint(fileList, fdList, taskId);
    arkTaskId = ArkWebStringClassToStruct(taskId);
    return result;
}

int32_t ArkPrintManagerAdapterImpl::Print(const ArkWebString& printJobName,
    const ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> listener, const ArkPrintAttributesAdapter& printAttributes)
{
    std::string str = ArkWebStringStructToClass(printJobName);

    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_.Print(str, nullptr, printAttributes);
    }

    return real_.Print(str, std::make_shared<ArkPrintDocumentAdapterAdapterWrapper>(listener), printAttributes);
}

int32_t ArkPrintManagerAdapterImpl::Print(const ArkWebString& printJobName,
    const ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> listener, const ArkPrintAttributesAdapter& printAttributes,
    void* contextToken)
{
    std::string str = ArkWebStringStructToClass(printJobName);

    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_.Print(str, nullptr, printAttributes, contextToken);
    }

    return real_.Print(
        str, std::make_shared<ArkPrintDocumentAdapterAdapterWrapper>(listener), printAttributes, contextToken);
}

} // namespace OHOS::ArkWeb
