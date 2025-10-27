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

#include "ohos_nweb/bridge/ark_web_print_document_adapter_adapter_wrapper.h"

#include "ohos_nweb/bridge/ark_web_print_write_result_callback_adapter_impl.h"
#include "ohos_nweb/bridge/ark_web_print_attributes_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

void ArkWebPrintDocumentAdapterAdapterWrapper::OnStartLayoutWrite(const std::string& jobId,
    std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> oldAttrs,
    std::shared_ptr<OHOS::NWeb::NWebPrintAttributesAdapter> newAttrs, uint32_t fd,
    std::shared_ptr<OHOS::NWeb::NWebPrintWriteResultCallbackAdapter> callback)
{
    ArkWebString str = ArkWebStringClassToStruct(jobId);
    auto oldAttributes = new ArkWebPrintAttributesAdapterImpl(oldAttrs);
    auto newAttributes = new ArkWebPrintAttributesAdapterImpl(newAttrs);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ref_->OnStartLayoutWrite(str, oldAttributes, newAttributes, fd, nullptr);
    } else {
        ref_->OnStartLayoutWrite(str, oldAttributes, newAttributes, fd,
            new ArkWebPrintWriteResultCallbackAdapterImpl(callback));
    }
    ArkWebStringStructRelease(str);
}

void ArkWebPrintDocumentAdapterAdapterWrapper::OnJobStateChanged(const std::string& jobId, uint32_t state)
{
    ArkWebString str = ArkWebStringClassToStruct(jobId);
    ref_->OnJobStateChanged(str, state);
    ArkWebStringStructRelease(str);
}

} // namespace OHOS::ArkWeb