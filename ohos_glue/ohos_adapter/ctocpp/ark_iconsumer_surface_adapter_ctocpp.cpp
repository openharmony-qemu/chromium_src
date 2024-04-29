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

#include "ohos_adapter/ctocpp/ark_iconsumer_surface_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_ibuffer_consumer_listener_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_surface_buffer_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkIConsumerSurfaceAdapterCToCpp::RegisterConsumerListener(
    ArkWebRefPtr<ArkIBufferConsumerListenerAdapter> listener) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_iconsumer_surface_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, register_consumer_listener, 0);

  // Execute
  return _struct->register_consumer_listener(
      _struct, ArkIBufferConsumerListenerAdapterCppToC::Invert(listener));
}

ARK_WEB_NO_SANITIZE
int32_t ArkIConsumerSurfaceAdapterCToCpp::ReleaseBuffer(
    ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer, int32_t fence) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_iconsumer_surface_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release_buffer, 0);

  // Execute
  return _struct->release_buffer(
      _struct, ArkSurfaceBufferAdapterCToCpp::Revert(buffer), fence);
}

ARK_WEB_NO_SANITIZE
int32_t ArkIConsumerSurfaceAdapterCToCpp::SetUserData(const ArkWebString &key,
                                                      const ArkWebString &val) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_iconsumer_surface_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_user_data, 0);

  // Execute
  return _struct->set_user_data(_struct, &key, &val);
}

ARK_WEB_NO_SANITIZE
int32_t ArkIConsumerSurfaceAdapterCToCpp::SetQueueSize(uint32_t queueSize) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_iconsumer_surface_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_queue_size, 0);

  // Execute
  return _struct->set_queue_size(_struct, queueSize);
}

ArkIConsumerSurfaceAdapterCToCpp::ArkIConsumerSurfaceAdapterCToCpp() {
}

ArkIConsumerSurfaceAdapterCToCpp::~ArkIConsumerSurfaceAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkIConsumerSurfaceAdapterCToCpp, ArkIConsumerSurfaceAdapter,
    ark_iconsumer_surface_adapter_t>::kBridgeType =
    ARK_ICONSUMER_SURFACE_ADAPTER;

} // namespace OHOS::ArkWeb
