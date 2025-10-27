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

#include "ohos_adapter/bridge/ark_print_document_adapter_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_print_write_result_callback_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPrintDocumentAdapterAdapterWrapper::ArkPrintDocumentAdapterAdapterWrapper(
    ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> ref)
    : ctocpp_(ref)
{}

void ArkPrintDocumentAdapterAdapterWrapper::OnStartLayoutWrite(const std::string& jobId,
    const NWeb::PrintAttributesAdapter& oldAttrs, const NWeb::PrintAttributesAdapter& newAttrs, uint32_t fd,
    std::shared_ptr<NWeb::PrintWriteResultCallbackAdapter> callback)
{
    ArkWebString str = ArkWebStringClassToStruct(jobId);
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ctocpp_->OnStartLayoutWrite(str, oldAttrs, newAttrs, fd, nullptr);
    } else {
        ctocpp_->OnStartLayoutWrite(str, oldAttrs, newAttrs, fd, new ArkPrintWriteResultCallbackAdapterImpl(callback));
    }
    ArkWebStringStructRelease(str);
}

void ArkPrintDocumentAdapterAdapterWrapper::OnJobStateChanged(const std::string& jobId, uint32_t state)
{
    ArkWebString str = ArkWebStringClassToStruct(jobId);
    ctocpp_->OnJobStateChanged(str, state);
    ArkWebStringStructRelease(str);
}

} // namespace OHOS::ArkWeb
