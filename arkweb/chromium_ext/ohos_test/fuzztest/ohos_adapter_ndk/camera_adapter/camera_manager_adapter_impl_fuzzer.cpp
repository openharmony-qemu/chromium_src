
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#define private public
 #include "arkweb/ohos_adapter_ndk/camera_adapter/include/camera_manager_adapter_impl.h"
 
#include <fuzzer/FuzzedDataProvider.h>
#include <inputmethod/inputmethod_controller_capi.h>
 
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
 
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
namespace OHOS::NWeb {

void FuzzCameraManagerAdapterImpl(FuzzedDataProvider* fdp) {

  CameraManagerAdapterImpl adapter;

  adapter.GetRotationInfo();

  int32_t connectType = fdp->ConsumeIntegralInRange<int32_t>(0, 2);
  adapter.GetCameraTransportType(static_cast<Camera_Connection>(connectType));

  int32_t cameraPosition = fdp->ConsumeIntegralInRange<int32_t>(0, 2);
  adapter.GetCameraFacingMode(static_cast<Camera_Position>(cameraPosition));

  int32_t cameraFormat = fdp->ConsumeIntegralInRange<int32_t>(2000, 2002);
  adapter.TransToAdapterCameraFormat(static_cast<Camera_Format>(cameraFormat));

  int32_t videoPixelFormatAdapter = fdp->ConsumeIntegralInRange<int32_t>(0, 4);
  adapter.TransToOriCameraFormat(static_cast<VideoPixelFormatAdapter>(videoPixelFormatAdapter));

  int32_t cameraExposureMode = fdp->ConsumeIntegralInRange<int32_t>(0, 2);
  adapter.GetAdapterExposureMode(static_cast<Camera_ExposureMode>(cameraExposureMode));

  int32_t focusModeAdapter = fdp->ConsumeIntegralInRange<int32_t>(0, 3);
  adapter.GetOriFocusMode(static_cast<FocusModeAdapter>(focusModeAdapter));

  int32_t flashModeAdapter = fdp->ConsumeIntegralInRange<int32_t>(0, 3);
  adapter.GetOriFlashMode(static_cast<FlashModeAdapter>(flashModeAdapter));

  int32_t cameraFocusMode = fdp->ConsumeIntegralInRange<int32_t>(0, 3);
  adapter.GetAdapterFocusMode(static_cast<Camera_FocusMode>(cameraFocusMode));

  adapter.GetInstance();

  std::string sourceUrl;
  int32_t cameraErrorType = fdp->ConsumeIntegralInRange<int32_t>(0, 14);
  adapter.ErrorTypeToString(static_cast<CameraErrorType>(cameraErrorType), sourceUrl);
  adapter.ReportErrorSysEvent(static_cast<CameraErrorType>(cameraErrorType));

  adapter.GetDevicesInfo();

  std::string deviceId = fdp->ConsumeRandomLengthString(32);
  adapter.InitCameraInput(deviceId);
  adapter.ReleaseSessionResource(deviceId);

  std::vector<Camera_ExposureMode> exposureModes(1,
    static_cast<Camera_ExposureMode>(fdp->ConsumeIntegralInRange<int32_t>(0, 2)));
  std::vector<ExposureModeAdapter> exposureModesAdapter(1,
    static_cast<ExposureModeAdapter>(fdp->ConsumeIntegralInRange<int32_t>(-1, 2)));
  adapter.TransToAdapterExposureModes(exposureModes, exposureModesAdapter);
  adapter.GetExposureModes(exposureModesAdapter);

  ExposureModeAdapter exposureModeAdapter;
  adapter.GetCurrentExposureMode(exposureModeAdapter);

  adapter.GetExposureCompensation();

  int32_t focusMode = fdp->ConsumeIntegralInRange<int32_t>(0, 3);
  adapter.IsFocusModeSupported(static_cast<FocusModeAdapter>(focusMode));
  adapter.GetCurrentFocusMode();

  int32_t flashMode = fdp->ConsumeIntegralInRange<int32_t>(0, 3);
  adapter.IsFlashModeSupported(static_cast<FlashModeAdapter>(flashMode));

  adapter.CreateAndStartSession();
  adapter.RestartSession();

  int32_t stopType = fdp->ConsumeIntegralInRange<int32_t>(0, 1);
  adapter.StopSession(static_cast<CameraStopType>(stopType));
  adapter.ReleaseSession();
  adapter.DestroyNativeImageAndWindow();
  adapter.ReleaseCameraManger();
  adapter.GetCameraStatus();

  int32_t cameraStatusAdapter = fdp->ConsumeIntegralInRange<int32_t>(0, 3);
  adapter.SetCameraStatus(static_cast<CameraStatusAdapter>(cameraStatusAdapter));

  adapter.GetCurrentDeviceId();
  adapter.GetNativeImage();
  adapter.GetBufferListener();
  adapter.GetStatusCallback();
  adapter.IsExistCaptureTask();

  adapter.SetForegroundFlag(fdp->ConsumeBool());
  
  std::string cameraId = fdp->ConsumeRandomLengthString(32);
  adapter.GetCameraDisplayName(cameraId, static_cast<Camera_Position>(cameraPosition));
}

} // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }
 
  FuzzedDataProvider fdp(data, size);
  FuzzCameraManagerAdapterImpl(&fdp);
  return 0;
}