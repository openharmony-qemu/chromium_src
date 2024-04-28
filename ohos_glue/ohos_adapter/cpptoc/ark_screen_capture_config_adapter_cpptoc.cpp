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

#include "ohos_adapter/cpptoc/ark_screen_capture_config_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/cpptoc/ark_audio_info_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_recorder_info_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_video_info_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK ark_screen_capture_config_adapter_get_capture_mode(
    struct _ark_screen_capture_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkScreenCaptureConfigAdapterCppToC::Get(self)->GetCaptureMode();
}

int32_t ARK_WEB_CALLBACK ark_screen_capture_config_adapter_get_data_type(
    struct _ark_screen_capture_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkScreenCaptureConfigAdapterCppToC::Get(self)->GetDataType();
}

ark_audio_info_adapter_t *ARK_WEB_CALLBACK
ark_screen_capture_config_adapter_get_audio_info(
    struct _ark_screen_capture_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkAudioInfoAdapter> _retval =
      ArkScreenCaptureConfigAdapterCppToC::Get(self)->GetAudioInfo();

  // Return type: refptr_same
  return ArkAudioInfoAdapterCppToC::Invert(_retval);
}

ark_video_info_adapter_t *ARK_WEB_CALLBACK
ark_screen_capture_config_adapter_get_video_info(
    struct _ark_screen_capture_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkVideoInfoAdapter> _retval =
      ArkScreenCaptureConfigAdapterCppToC::Get(self)->GetVideoInfo();

  // Return type: refptr_same
  return ArkVideoInfoAdapterCppToC::Invert(_retval);
}

ark_recorder_info_adapter_t *ARK_WEB_CALLBACK
ark_screen_capture_config_adapter_get_recorder_info(
    struct _ark_screen_capture_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkRecorderInfoAdapter> _retval =
      ArkScreenCaptureConfigAdapterCppToC::Get(self)->GetRecorderInfo();

  // Return type: refptr_same
  return ArkRecorderInfoAdapterCppToC::Invert(_retval);
}

} // namespace

ArkScreenCaptureConfigAdapterCppToC::ArkScreenCaptureConfigAdapterCppToC() {
  GetStruct()->get_capture_mode =
      ark_screen_capture_config_adapter_get_capture_mode;
  GetStruct()->get_data_type = ark_screen_capture_config_adapter_get_data_type;
  GetStruct()->get_audio_info =
      ark_screen_capture_config_adapter_get_audio_info;
  GetStruct()->get_video_info =
      ark_screen_capture_config_adapter_get_video_info;
  GetStruct()->get_recorder_info =
      ark_screen_capture_config_adapter_get_recorder_info;
}

ArkScreenCaptureConfigAdapterCppToC::~ArkScreenCaptureConfigAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkScreenCaptureConfigAdapterCppToC, ArkScreenCaptureConfigAdapter,
    ark_screen_capture_config_adapter_t>::kBridgeType =
    ARK_SCREEN_CAPTURE_CONFIG_ADAPTER;

} // namespace OHOS::ArkWeb
