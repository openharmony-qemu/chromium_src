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

#include "ohos_adapter/ctocpp/ark_surface_buffer_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkSurfaceBufferAdapterCToCpp::GetFileDescriptor() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_surface_buffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_file_descriptor, 0);

  // Execute
  return _struct->get_file_descriptor(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSurfaceBufferAdapterCToCpp::GetWidth() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_surface_buffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_width, 0);

  // Execute
  return _struct->get_width(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSurfaceBufferAdapterCToCpp::GetHeight() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_surface_buffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_height, 0);

  // Execute
  return _struct->get_height(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSurfaceBufferAdapterCToCpp::GetStride() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_surface_buffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_stride, 0);

  // Execute
  return _struct->get_stride(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSurfaceBufferAdapterCToCpp::GetFormat() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_surface_buffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_format, 0);

  // Execute
  return _struct->get_format(_struct);
}

ARK_WEB_NO_SANITIZE
uint32_t ArkSurfaceBufferAdapterCToCpp::GetSize() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_surface_buffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_size, 0);

  // Execute
  return _struct->get_size(_struct);
}

ARK_WEB_NO_SANITIZE
void *ArkSurfaceBufferAdapterCToCpp::GetVirAddr() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_surface_buffer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_vir_addr, nullptr);

  // Execute
  return _struct->get_vir_addr(_struct);
}

ArkSurfaceBufferAdapterCToCpp::ArkSurfaceBufferAdapterCToCpp() {
}

ArkSurfaceBufferAdapterCToCpp::~ArkSurfaceBufferAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkSurfaceBufferAdapterCToCpp, ArkSurfaceBufferAdapter,
    ark_surface_buffer_adapter_t>::kBridgeType = ARK_SURFACE_BUFFER_ADAPTER;

} // namespace OHOS::ArkWeb
