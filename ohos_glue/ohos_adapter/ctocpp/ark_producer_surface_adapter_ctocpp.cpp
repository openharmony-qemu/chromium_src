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

#include "ohos_adapter/ctocpp/ark_producer_surface_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_buffer_flush_config_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_buffer_request_config_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_surface_buffer_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkSurfaceBufferAdapter>
ArkProducerSurfaceAdapterCToCpp::RequestBuffer(
    int32_t &fence, ArkWebRefPtr<ArkBufferRequestConfigAdapter> config) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_producer_surface_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, request_buffer, nullptr);

  // Execute
  ark_surface_buffer_adapter_t *_retval = _struct->request_buffer(
      _struct, &fence, ArkBufferRequestConfigAdapterCppToC::Invert(config));

  // Return type: refptr_same
  return ArkSurfaceBufferAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
int32_t ArkProducerSurfaceAdapterCToCpp::FlushBuffer(
    ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer, int32_t fence,
    ArkWebRefPtr<ArkBufferFlushConfigAdapter> config) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_producer_surface_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, flush_buffer, 0);

  // Execute
  return _struct->flush_buffer(
      _struct, ArkSurfaceBufferAdapterCToCpp::Revert(buffer), fence,
      ArkBufferFlushConfigAdapterCppToC::Invert(config));
}

ArkProducerSurfaceAdapterCToCpp::ArkProducerSurfaceAdapterCToCpp() {
}

ArkProducerSurfaceAdapterCToCpp::~ArkProducerSurfaceAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkProducerSurfaceAdapterCToCpp, ArkProducerSurfaceAdapter,
    ark_producer_surface_adapter_t>::kBridgeType = ARK_PRODUCER_SURFACE_ADAPTER;

} // namespace OHOS::ArkWeb
