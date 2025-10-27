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

#include "ohos_adapter/bridge/ark_print_document_adapter_adapter_impl.h"

#include "ohos_adapter/bridge/ark_print_write_result_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPrintDocumentAdapterAdapterImpl::ArkPrintDocumentAdapterAdapterImpl(
    std::shared_ptr<OHOS::NWeb::PrintDocumentAdapterAdapter> ref)
    : real_(ref)
{}

void ArkPrintDocumentAdapterAdapterImpl::OnStartLayoutWrite(const ArkWebString& jobId,
    const ArkPrintAttributesAdapter& oldAttrs, const ArkPrintAttributesAdapter& newAttrs, uint32_t fd,
    ArkWebRefPtr<ArkPrintWriteResultCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_->OnStartLayoutWrite(ArkWebStringStructToClass(jobId), oldAttrs, newAttrs, fd, nullptr);
    }

    real_->OnStartLayoutWrite(ArkWebStringStructToClass(jobId), oldAttrs, newAttrs, fd,
        std::make_shared<ArkPrintWriteResultCallbackAdapterWrapper>(callback));
}

void ArkPrintDocumentAdapterAdapterImpl::OnJobStateChanged(const ArkWebString& jobId, uint32_t state)
{
    real_->OnJobStateChanged(ArkWebStringStructToClass(jobId), state);
}

} // namespace OHOS::ArkWeb
