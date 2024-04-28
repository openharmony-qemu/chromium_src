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

#include "ohos_adapter/cpptoc/ark_video_capture_params_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

uint32_t ARK_WEB_CALLBACK ark_video_capture_params_adapter_get_width(
    struct _ark_video_capture_params_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoCaptureParamsAdapterCppToC::Get(self)->GetWidth();
}

uint32_t ARK_WEB_CALLBACK ark_video_capture_params_adapter_get_height(
    struct _ark_video_capture_params_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoCaptureParamsAdapterCppToC::Get(self)->GetHeight();
}

float ARK_WEB_CALLBACK ark_video_capture_params_adapter_get_frame_rate(
    struct _ark_video_capture_params_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoCaptureParamsAdapterCppToC::Get(self)->GetFrameRate();
}

int32_t ARK_WEB_CALLBACK ark_video_capture_params_adapter_get_pixel_format(
    struct _ark_video_capture_params_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoCaptureParamsAdapterCppToC::Get(self)->GetPixelFormat();
}

bool ARK_WEB_CALLBACK
ark_video_capture_params_adapter_get_enable_face_detection(
    struct _ark_video_capture_params_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkVideoCaptureParamsAdapterCppToC::Get(self)
      ->GetEnableFaceDetection();
}

} // namespace

ArkVideoCaptureParamsAdapterCppToC::ArkVideoCaptureParamsAdapterCppToC() {
  GetStruct()->get_width = ark_video_capture_params_adapter_get_width;
  GetStruct()->get_height = ark_video_capture_params_adapter_get_height;
  GetStruct()->get_frame_rate = ark_video_capture_params_adapter_get_frame_rate;
  GetStruct()->get_pixel_format =
      ark_video_capture_params_adapter_get_pixel_format;
  GetStruct()->get_enable_face_detection =
      ark_video_capture_params_adapter_get_enable_face_detection;
}

ArkVideoCaptureParamsAdapterCppToC::~ArkVideoCaptureParamsAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkVideoCaptureParamsAdapterCppToC, ArkVideoCaptureParamsAdapter,
    ark_video_capture_params_adapter_t>::kBridgeType =
    ARK_VIDEO_CAPTURE_PARAMS_ADAPTER;

} // namespace OHOS::ArkWeb
