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

#include "ohos_adapter/cpptoc/ark_video_capture_info_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

uint64_t ARK_WEB_CALLBACK ark_video_capture_info_adapter_get_display_id(
    struct _ark_video_capture_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoCaptureInfoAdapterCppToC::Get(self)->GetDisplayId();
}

ArkWebInt32List ARK_WEB_CALLBACK ark_video_capture_info_adapter_get_task_ids(
    struct _ark_video_capture_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_int32_list_default);

  // Execute
  return ArkVideoCaptureInfoAdapterCppToC::Get(self)->GetTaskIDs();
}

int32_t ARK_WEB_CALLBACK ark_video_capture_info_adapter_get_video_frame_width(
    struct _ark_video_capture_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoCaptureInfoAdapterCppToC::Get(self)->GetVideoFrameWidth();
}

int32_t ARK_WEB_CALLBACK ark_video_capture_info_adapter_get_video_frame_height(
    struct _ark_video_capture_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoCaptureInfoAdapterCppToC::Get(self)->GetVideoFrameHeight();
}

int32_t ARK_WEB_CALLBACK ark_video_capture_info_adapter_get_video_source_type(
    struct _ark_video_capture_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkVideoCaptureInfoAdapterCppToC::Get(self)->GetVideoSourceType();
}

} // namespace

ArkVideoCaptureInfoAdapterCppToC::ArkVideoCaptureInfoAdapterCppToC() {
  GetStruct()->get_display_id = ark_video_capture_info_adapter_get_display_id;
  GetStruct()->get_task_ids = ark_video_capture_info_adapter_get_task_ids;
  GetStruct()->get_video_frame_width =
      ark_video_capture_info_adapter_get_video_frame_width;
  GetStruct()->get_video_frame_height =
      ark_video_capture_info_adapter_get_video_frame_height;
  GetStruct()->get_video_source_type =
      ark_video_capture_info_adapter_get_video_source_type;
}

ArkVideoCaptureInfoAdapterCppToC::~ArkVideoCaptureInfoAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkVideoCaptureInfoAdapterCppToC, ArkVideoCaptureInfoAdapter,
    ark_video_capture_info_adapter_t>::kBridgeType =
    ARK_VIDEO_CAPTURE_INFO_ADAPTER;

} // namespace OHOS::ArkWeb
