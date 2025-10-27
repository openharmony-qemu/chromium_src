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

#include "ohos_adapter/bridge/ark_print_manager_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_print_document_adapter_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPrintManagerAdapterWrapper::ArkPrintManagerAdapterWrapper(ArkWebRefPtr<ArkPrintManagerAdapter> ref) : ctocpp_(ref) {}

int32_t ArkPrintManagerAdapterWrapper::StartPrint(
    const std::vector<std::string>& fileList, const std::vector<uint32_t>& fdList, std::string& taskId)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebStringVector arkFileList = ArkWebStringVectorClassToStruct(fileList);
    ArkWebUint32Vector arkFdList = ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(fdList);
    ArkWebString arkTaskId;
    int32_t result = ctocpp_->StartPrint(arkFileList, arkFdList, arkTaskId);
    taskId = ArkWebStringStructToClass(arkTaskId);
    ArkWebStringStructRelease(arkTaskId);
    ArkWebStringVectorStructRelease(arkFileList);
    ArkWebBasicVectorStructRelease(arkFdList);
    return result;
}

int32_t ArkPrintManagerAdapterWrapper::Print(const std::string& printJobName,
    const std::shared_ptr<NWeb::PrintDocumentAdapterAdapter> listener,
    const NWeb::PrintAttributesAdapter& printAttributes)
{
    if (!ctocpp_) {
        return -1;
    }

    ArkWebString str = ArkWebStringClassToStruct(printJobName);
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        result = ctocpp_->Print(str, nullptr, printAttributes);
    } else {
        result = ctocpp_->Print(str, new ArkPrintDocumentAdapterAdapterImpl(listener), printAttributes);
    }

    ArkWebStringStructRelease(str);
    return result;
}

int32_t ArkPrintManagerAdapterWrapper::Print(const std::string& printJobName,
    const std::shared_ptr<NWeb::PrintDocumentAdapterAdapter> listener,
    const NWeb::PrintAttributesAdapter& printAttributes, void* contextToken)
{
    if (!ctocpp_) {
        return -1;
    }

    ArkWebString str = ArkWebStringClassToStruct(printJobName);
    int32_t result;

    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        result = ctocpp_->Print(str, nullptr, printAttributes, contextToken);
    } else {
        result = ctocpp_->Print(str, new ArkPrintDocumentAdapterAdapterImpl(listener), printAttributes, contextToken);
    }

    ArkWebStringStructRelease(str);
    return result;
}

} // namespace OHOS::ArkWeb
