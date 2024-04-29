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

#include "ohos_adapter/ctocpp/ark_event_handler_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_event_handler_fdlistener_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkEventHandlerAdapterCToCpp::AddFileDescriptorListener(
    int32_t fileDescriptor, uint32_t events,
    const ArkWebRefPtr<ArkEventHandlerFDListenerAdapter> listener) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_event_handler_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, add_file_descriptor_listener,
                                   false);

  // Execute
  return _struct->add_file_descriptor_listener(
      _struct, fileDescriptor, events,
      ArkEventHandlerFDListenerAdapterCppToC::Invert(listener));
}

ARK_WEB_NO_SANITIZE
void ArkEventHandlerAdapterCToCpp::RemoveFileDescriptorListener(
    int32_t fileDescriptor) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_event_handler_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, remove_file_descriptor_listener, );

  // Execute
  _struct->remove_file_descriptor_listener(_struct, fileDescriptor);
}

ArkEventHandlerAdapterCToCpp::ArkEventHandlerAdapterCToCpp() {
}

ArkEventHandlerAdapterCToCpp::~ArkEventHandlerAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkEventHandlerAdapterCToCpp, ArkEventHandlerAdapter,
                           ark_event_handler_adapter_t>::kBridgeType =
        ARK_EVENT_HANDLER_ADAPTER;

} // namespace OHOS::ArkWeb
