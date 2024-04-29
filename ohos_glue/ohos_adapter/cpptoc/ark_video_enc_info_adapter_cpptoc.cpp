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

#include "ohos_adapter/cpptoc/ark_video_enc_info_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK ark_video_enc_info_adapter_get_video_codec_format(
    struct _ark_video_enc_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoEncInfoAdapterCppToC::Get(self)->GetVideoCodecFormat();
}

int32_t ARK_WEB_CALLBACK ark_video_enc_info_adapter_get_video_bitrate(
    struct _ark_video_enc_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoEncInfoAdapterCppToC::Get(self)->GetVideoBitrate();
}

int32_t ARK_WEB_CALLBACK ark_video_enc_info_adapter_get_video_frame_rate(
    struct _ark_video_enc_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoEncInfoAdapterCppToC::Get(self)->GetVideoFrameRate();
}

} // namespace

ArkVideoEncInfoAdapterCppToC::ArkVideoEncInfoAdapterCppToC() {
  GetStruct()->get_video_codec_format =
      ark_video_enc_info_adapter_get_video_codec_format;
  GetStruct()->get_video_bitrate = ark_video_enc_info_adapter_get_video_bitrate;
  GetStruct()->get_video_frame_rate =
      ark_video_enc_info_adapter_get_video_frame_rate;
}

ArkVideoEncInfoAdapterCppToC::~ArkVideoEncInfoAdapterCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkVideoEncInfoAdapterCppToC, ArkVideoEncInfoAdapter,
                           ark_video_enc_info_adapter_t>::kBridgeType =
        ARK_VIDEO_ENC_INFO_ADAPTER;

} // namespace OHOS::ArkWeb
