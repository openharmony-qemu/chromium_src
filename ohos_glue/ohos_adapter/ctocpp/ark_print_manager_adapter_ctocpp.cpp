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

#include "ohos_adapter/ctocpp/ark_print_manager_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_print_document_adapter_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t
ArkPrintManagerAdapterCToCpp::StartPrint(const ArkWebStringVector &fileList,
                                         const ArkWebUint32Vector &fdList,
                                         ArkWebString &taskId) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_print_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_print, 0);

  // Execute
  return _struct->start_print(_struct, &fileList, &fdList, &taskId);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPrintManagerAdapterCToCpp::Print(
    const ArkWebString &printJobName,
    const ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> listener,
    const ArkPrintAttributesAdapter &printAttributes) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_print_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, print1, 0);

  // Execute
  return _struct->print1(_struct, &printJobName,
                         ArkPrintDocumentAdapterAdapterCppToC::Invert(listener),
                         &printAttributes);
}

ARK_WEB_NO_SANITIZE
int32_t ArkPrintManagerAdapterCToCpp::Print(
    const ArkWebString &printJobName,
    const ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> listener,
    const ArkPrintAttributesAdapter &printAttributes, void *contextToken) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_print_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, print2, 0);

  // Execute
  return _struct->print2(_struct, &printJobName,
                         ArkPrintDocumentAdapterAdapterCppToC::Invert(listener),
                         &printAttributes, contextToken);
}

ArkPrintManagerAdapterCToCpp::ArkPrintManagerAdapterCToCpp() {
}

ArkPrintManagerAdapterCToCpp::~ArkPrintManagerAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkPrintManagerAdapterCToCpp, ArkPrintManagerAdapter,
                           ark_print_manager_adapter_t>::kBridgeType =
        ARK_PRINT_MANAGER_ADAPTER;

} // namespace OHOS::ArkWeb
