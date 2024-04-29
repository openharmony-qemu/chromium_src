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

#include "ohos_adapter/ctocpp/ark_video_device_descriptor_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/ctocpp/ark_video_control_support_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
ArkWebString ArkVideoDeviceDescriptorAdapterCToCpp::GetDisplayName() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_device_descriptor_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_display_name,
                                   ark_web_string_default);

  // Execute
  return _struct->get_display_name(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkVideoDeviceDescriptorAdapterCToCpp::GetDeviceId() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_device_descriptor_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_device_id,
                                   ark_web_string_default);

  // Execute
  return _struct->get_device_id(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkVideoDeviceDescriptorAdapterCToCpp::GetModelId() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_device_descriptor_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_model_id,
                                   ark_web_string_default);

  // Execute
  return _struct->get_model_id(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkVideoControlSupportAdapter>
ArkVideoDeviceDescriptorAdapterCToCpp::GetControlSupport() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_device_descriptor_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_control_support, nullptr);

  // Execute
  ark_video_control_support_adapter_t *_retval =
      _struct->get_control_support(_struct);

  // Return type: refptr_same
  return ArkVideoControlSupportAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
int32_t ArkVideoDeviceDescriptorAdapterCToCpp::GetTransportType() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_device_descriptor_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_transport_type, 0);

  // Execute
  return _struct->get_transport_type(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkVideoDeviceDescriptorAdapterCToCpp::GetFacingMode() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_device_descriptor_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_facing_mode, 0);

  // Execute
  return _struct->get_facing_mode(_struct);
}

ARK_WEB_NO_SANITIZE
ArkFormatAdapterVector
ArkVideoDeviceDescriptorAdapterCToCpp::GetSupportCaptureFormats() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_video_device_descriptor_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, {0});

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_support_capture_formats, {0});

  // Execute
  return _struct->get_support_capture_formats(_struct);
}

ArkVideoDeviceDescriptorAdapterCToCpp::ArkVideoDeviceDescriptorAdapterCToCpp() {
}

ArkVideoDeviceDescriptorAdapterCToCpp::
    ~ArkVideoDeviceDescriptorAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkVideoDeviceDescriptorAdapterCToCpp, ArkVideoDeviceDescriptorAdapter,
    ark_video_device_descriptor_adapter_t>::kBridgeType =
    ARK_VIDEO_DEVICE_DESCRIPTOR_ADAPTER;

} // namespace OHOS::ArkWeb
