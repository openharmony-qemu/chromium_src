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

#include "ohos_adapter/ctocpp/ark_window_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void *ArkWindowAdapterCToCpp::CreateNativeWindowFromSurface(void *pSurface) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_window_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_native_window_from_surface,
                                   nullptr);

  // Execute
  return _struct->create_native_window_from_surface(_struct, pSurface);
}

ARK_WEB_NO_SANITIZE
void ArkWindowAdapterCToCpp::DestroyNativeWindow(void *window) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_window_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, destroy_native_window, );

  // Execute
  _struct->destroy_native_window(_struct, window);
}

ARK_WEB_NO_SANITIZE
int32_t ArkWindowAdapterCToCpp::NativeWindowSetBufferGeometry(void *window,
                                                              int32_t width,
                                                              int32_t height) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_window_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, native_window_set_buffer_geometry,
                                   0);

  // Execute
  return _struct->native_window_set_buffer_geometry(_struct, window, width,
                                                    height);
}

ARK_WEB_NO_SANITIZE
void ArkWindowAdapterCToCpp::NativeWindowSurfaceCleanCache(void *window) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_window_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   native_window_surface_clean_cache, );

  // Execute
  _struct->native_window_surface_clean_cache(_struct, window);
}

ArkWindowAdapterCToCpp::ArkWindowAdapterCToCpp() {
}

ArkWindowAdapterCToCpp::~ArkWindowAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkWindowAdapterCToCpp, ArkWindowAdapter,
                           ark_window_adapter_t>::kBridgeType =
        ARK_WINDOW_ADAPTER;

} // namespace OHOS::ArkWeb
