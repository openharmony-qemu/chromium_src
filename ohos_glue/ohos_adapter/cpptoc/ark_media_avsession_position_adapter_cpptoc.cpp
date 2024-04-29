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

#include "ohos_adapter/cpptoc/ark_media_avsession_position_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_media_avsession_position_adapter_set_duration(
    struct _ark_media_avsession_position_adapter_t *self, int64_t duration) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMediaAVSessionPositionAdapterCppToC::Get(self)->SetDuration(duration);
}

int64_t ARK_WEB_CALLBACK ark_media_avsession_position_adapter_get_duration(
    struct _ark_media_avsession_position_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMediaAVSessionPositionAdapterCppToC::Get(self)->GetDuration();
}

void ARK_WEB_CALLBACK ark_media_avsession_position_adapter_set_elapsed_time(
    struct _ark_media_avsession_position_adapter_t *self, int64_t elapsedTime) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMediaAVSessionPositionAdapterCppToC::Get(self)->SetElapsedTime(
      elapsedTime);
}

int64_t ARK_WEB_CALLBACK ark_media_avsession_position_adapter_get_elapsed_time(
    struct _ark_media_avsession_position_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMediaAVSessionPositionAdapterCppToC::Get(self)->GetElapsedTime();
}

void ARK_WEB_CALLBACK ark_media_avsession_position_adapter_set_update_time(
    struct _ark_media_avsession_position_adapter_t *self, int64_t updateTime) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkMediaAVSessionPositionAdapterCppToC::Get(self)->SetUpdateTime(updateTime);
}

int64_t ARK_WEB_CALLBACK ark_media_avsession_position_adapter_get_update_time(
    struct _ark_media_avsession_position_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkMediaAVSessionPositionAdapterCppToC::Get(self)->GetUpdateTime();
}

} // namespace

ArkMediaAVSessionPositionAdapterCppToC::
    ArkMediaAVSessionPositionAdapterCppToC() {
  GetStruct()->set_duration = ark_media_avsession_position_adapter_set_duration;
  GetStruct()->get_duration = ark_media_avsession_position_adapter_get_duration;
  GetStruct()->set_elapsed_time =
      ark_media_avsession_position_adapter_set_elapsed_time;
  GetStruct()->get_elapsed_time =
      ark_media_avsession_position_adapter_get_elapsed_time;
  GetStruct()->set_update_time =
      ark_media_avsession_position_adapter_set_update_time;
  GetStruct()->get_update_time =
      ark_media_avsession_position_adapter_get_update_time;
}

ArkMediaAVSessionPositionAdapterCppToC::
    ~ArkMediaAVSessionPositionAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkMediaAVSessionPositionAdapterCppToC, ArkMediaAVSessionPositionAdapter,
    ark_media_avsession_position_adapter_t>::kBridgeType =
    ARK_MEDIA_AVSESSION_POSITION_ADAPTER;

} // namespace OHOS::ArkWeb
