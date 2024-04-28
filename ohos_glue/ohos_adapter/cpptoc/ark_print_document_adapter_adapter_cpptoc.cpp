/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_adapter/cpptoc/ark_print_document_adapter_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/ctocpp/ark_print_write_result_callback_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_print_document_adapter_adapter_on_start_layout_write(
    struct _ark_print_document_adapter_adapter_t *self,
    const ArkWebString *jobId, const ArkPrintAttributesAdapter *oldAttrs,
    const ArkPrintAttributesAdapter *newAttrs, uint32_t fd,
    ark_print_write_result_callback_adapter_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(jobId, );

  ARK_WEB_CPPTOC_CHECK_PARAM(oldAttrs, );

  ARK_WEB_CPPTOC_CHECK_PARAM(newAttrs, );

  // Execute
  ArkPrintDocumentAdapterAdapterCppToC::Get(self)->OnStartLayoutWrite(
      *jobId, *oldAttrs, *newAttrs, fd,
      ArkPrintWriteResultCallbackAdapterCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK ark_print_document_adapter_adapter_on_job_state_changed(
    struct _ark_print_document_adapter_adapter_t *self,
    const ArkWebString *jobId, uint32_t state) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(jobId, );

  // Execute
  ArkPrintDocumentAdapterAdapterCppToC::Get(self)->OnJobStateChanged(*jobId,
                                                                     state);
}

} // namespace

ArkPrintDocumentAdapterAdapterCppToC::ArkPrintDocumentAdapterAdapterCppToC() {
  GetStruct()->on_start_layout_write =
      ark_print_document_adapter_adapter_on_start_layout_write;
  GetStruct()->on_job_state_changed =
      ark_print_document_adapter_adapter_on_job_state_changed;
}

ArkPrintDocumentAdapterAdapterCppToC::~ArkPrintDocumentAdapterAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkPrintDocumentAdapterAdapterCppToC, ArkPrintDocumentAdapterAdapter,
    ark_print_document_adapter_adapter_t>::kBridgeType =
    ARK_PRINT_DOCUMENT_ADAPTER_ADAPTER;

} // namespace OHOS::ArkWeb
