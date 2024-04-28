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

#include "ohos_adapter/ctocpp/ark_native_image_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_frame_available_listener_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkNativeImageAdapterCToCpp::CreateNativeImage(uint32_t textureId,
                                                    uint32_t textureTarget) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_native_image, );

  // Execute
  _struct->create_native_image(_struct, textureId, textureTarget);
}

ARK_WEB_NO_SANITIZE
void *ArkNativeImageAdapterCToCpp::AquireNativeWindowFromNativeImage() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(
      _struct, aquire_native_window_from_native_image, nullptr);

  // Execute
  return _struct->aquire_native_window_from_native_image(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkNativeImageAdapterCToCpp::AttachContext(uint32_t textureId) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, attach_context, 0);

  // Execute
  return _struct->attach_context(_struct, textureId);
}

ARK_WEB_NO_SANITIZE
int32_t ArkNativeImageAdapterCToCpp::DetachContext() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, detach_context, 0);

  // Execute
  return _struct->detach_context(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkNativeImageAdapterCToCpp::UpdateSurfaceImage() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, update_surface_image, 0);

  // Execute
  return _struct->update_surface_image(_struct);
}

ARK_WEB_NO_SANITIZE
int64_t ArkNativeImageAdapterCToCpp::GetTimestamp() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_timestamp, 0);

  // Execute
  return _struct->get_timestamp(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkNativeImageAdapterCToCpp::GetTransformMatrix(float matrix[16]) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_transform_matrix, 0);

  // Execute
  return _struct->get_transform_matrix(_struct, matrix);
}

ARK_WEB_NO_SANITIZE
int32_t ArkNativeImageAdapterCToCpp::GetSurfaceId(uint64_t *surfaceId) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_surface_id, 0);

  // Execute
  return _struct->get_surface_id(_struct, surfaceId);
}

ARK_WEB_NO_SANITIZE
int32_t ArkNativeImageAdapterCToCpp::SetOnFrameAvailableListener(
    ArkWebRefPtr<ArkFrameAvailableListener> listener) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_on_frame_available_listener, 0);

  // Execute
  return _struct->set_on_frame_available_listener(
      _struct, ArkFrameAvailableListenerCppToC::Invert(listener));
}

ARK_WEB_NO_SANITIZE
int32_t ArkNativeImageAdapterCToCpp::UnsetOnFrameAvailableListener() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, unset_on_frame_available_listener,
                                   0);

  // Execute
  return _struct->unset_on_frame_available_listener(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkNativeImageAdapterCToCpp::DestroyNativeImage() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_native_image_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, destroy_native_image, );

  // Execute
  _struct->destroy_native_image(_struct);
}

ArkNativeImageAdapterCToCpp::ArkNativeImageAdapterCToCpp() {
}

ArkNativeImageAdapterCToCpp::~ArkNativeImageAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkNativeImageAdapterCToCpp, ArkNativeImageAdapter,
                           ark_native_image_adapter_t>::kBridgeType =
        ARK_NATIVE_IMAGE_ADAPTER;

} // namespace OHOS::ArkWeb
