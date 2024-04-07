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

#include "ohos_nweb/cpptoc/ark_web_native_media_player_surface_info_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

ArkWebString ARK_WEB_CALLBACK ark_web_native_media_player_surface_info_get_id(
    struct _ark_web_native_media_player_surface_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNativeMediaPlayerSurfaceInfoCppToC::Get(self)->GetId();
}

double ARK_WEB_CALLBACK ark_web_native_media_player_surface_info_get_x(
    struct _ark_web_native_media_player_surface_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeMediaPlayerSurfaceInfoCppToC::Get(self)->GetX();
}

double ARK_WEB_CALLBACK ark_web_native_media_player_surface_info_get_y(
    struct _ark_web_native_media_player_surface_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeMediaPlayerSurfaceInfoCppToC::Get(self)->GetY();
}

double ARK_WEB_CALLBACK ark_web_native_media_player_surface_info_get_width(
    struct _ark_web_native_media_player_surface_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeMediaPlayerSurfaceInfoCppToC::Get(self)->GetWidth();
}

double ARK_WEB_CALLBACK ark_web_native_media_player_surface_info_get_height(
    struct _ark_web_native_media_player_surface_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeMediaPlayerSurfaceInfoCppToC::Get(self)->GetHeight();
}

} // namespace

ArkWebNativeMediaPlayerSurfaceInfoCppToC::
    ArkWebNativeMediaPlayerSurfaceInfoCppToC() {
  GetStruct()->get_id = ark_web_native_media_player_surface_info_get_id;
  GetStruct()->get_x = ark_web_native_media_player_surface_info_get_x;
  GetStruct()->get_y = ark_web_native_media_player_surface_info_get_y;
  GetStruct()->get_width = ark_web_native_media_player_surface_info_get_width;
  GetStruct()->get_height = ark_web_native_media_player_surface_info_get_height;
}

ArkWebNativeMediaPlayerSurfaceInfoCppToC::
    ~ArkWebNativeMediaPlayerSurfaceInfoCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebNativeMediaPlayerSurfaceInfoCppToC,
    ArkWebNativeMediaPlayerSurfaceInfo,
    ark_web_native_media_player_surface_info_t>::kBridgeType =
    ARK_WEB_NATIVE_MEDIA_PLAYER_SURFACE_INFO;

} // namespace OHOS::ArkWeb
