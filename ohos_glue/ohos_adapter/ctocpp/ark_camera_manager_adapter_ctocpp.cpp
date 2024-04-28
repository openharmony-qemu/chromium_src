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

#include "ohos_adapter/ctocpp/ark_camera_manager_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_camera_buffer_listener_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_camera_status_callback_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_video_capture_params_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_video_capture_range_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::Create(
    ArkWebRefPtr<ArkCameraStatusCallbackAdapter> cameraStatusCallback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create, 0);

  // Execute
  return _struct->create(_struct, ArkCameraStatusCallbackAdapterCppToC::Invert(
                                      cameraStatusCallback));
}

ARK_WEB_NO_SANITIZE
ArkVideoDeviceDescriptorAdapterVector
ArkCameraManagerAdapterCToCpp::GetDevicesInfo() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, {0});

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_devices_info, {0});

  // Execute
  return _struct->get_devices_info(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::ReleaseCameraManger() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release_camera_manger, 0);

  // Execute
  return _struct->release_camera_manger(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::GetExposureModes(
    ArkWebInt32Vector &exposureModesAdapter) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_exposure_modes, 0);

  // Execute
  return _struct->get_exposure_modes(_struct, &exposureModesAdapter);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::GetCurrentExposureMode(
    int32_t &exposureModeAdapter) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_current_exposure_mode, 0);

  // Execute
  return _struct->get_current_exposure_mode(_struct, &exposureModeAdapter);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkVideoCaptureRangeAdapter>
ArkCameraManagerAdapterCToCpp::GetCaptionRangeById(int32_t rangeId) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_caption_range_by_id, nullptr);

  // Execute
  ark_video_capture_range_adapter_t *_retval =
      _struct->get_caption_range_by_id(_struct, rangeId);

  // Return type: refptr_same
  return ArkVideoCaptureRangeAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
bool ArkCameraManagerAdapterCToCpp::IsFocusModeSupported(int32_t focusMode) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_focus_mode_supported, false);

  // Execute
  return _struct->is_focus_mode_supported(_struct, focusMode);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::GetCurrentFocusMode() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_current_focus_mode, 0);

  // Execute
  return _struct->get_current_focus_mode(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkCameraManagerAdapterCToCpp::IsFlashModeSupported(int32_t flashMode) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_flash_mode_supported, false);

  // Execute
  return _struct->is_flash_mode_supported(_struct, flashMode);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::RestartSession() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, restart_session, 0);

  // Execute
  return _struct->restart_session(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::StopSession(int32_t stopType) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_session, 0);

  // Execute
  return _struct->stop_session(_struct, stopType);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::GetCameraStatus() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_camera_status, 0);

  // Execute
  return _struct->get_camera_status(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkCameraManagerAdapterCToCpp::IsExistCaptureTask() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_exist_capture_task, false);

  // Execute
  return _struct->is_exist_capture_task(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkCameraManagerAdapterCToCpp::StartStream(
    const ArkWebString &deviceId,
    const ArkWebRefPtr<ArkVideoCaptureParamsAdapter> captureParams,
    ArkWebRefPtr<ArkCameraBufferListenerAdapter> listener) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_stream, 0);

  // Execute
  return _struct->start_stream(
      _struct, &deviceId,
      ArkVideoCaptureParamsAdapterCppToC::Invert(captureParams),
      ArkCameraBufferListenerAdapterCppToC::Invert(listener));
}

ARK_WEB_NO_SANITIZE
void ArkCameraManagerAdapterCToCpp::SetForegroundFlag(bool isForeground) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_foreground_flag, );

  // Execute
  _struct->set_foreground_flag(_struct, isForeground);
}

ARK_WEB_NO_SANITIZE
void ArkCameraManagerAdapterCToCpp::SetCameraStatus(int32_t status) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_camera_status, );

  // Execute
  _struct->set_camera_status(_struct, status);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkCameraManagerAdapterCToCpp::GetCurrentDeviceId() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_camera_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_current_device_id,
                                   ark_web_string_default);

  // Execute
  return _struct->get_current_device_id(_struct);
}

ArkCameraManagerAdapterCToCpp::ArkCameraManagerAdapterCToCpp() {
}

ArkCameraManagerAdapterCToCpp::~ArkCameraManagerAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkCameraManagerAdapterCToCpp, ArkCameraManagerAdapter,
    ark_camera_manager_adapter_t>::kBridgeType = ARK_CAMERA_MANAGER_ADAPTER;

} // namespace OHOS::ArkWeb
