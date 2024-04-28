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

#include "ohos_nweb/cpptoc/ark_web_media_info_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/cpptoc/ark_web_native_media_player_surface_info_cpptoc.h"

namespace OHOS::ArkWeb {

namespace {

int ARK_WEB_CALLBACK
ark_web_media_info_get_preload(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetPreload();
}

bool ARK_WEB_CALLBACK
ark_web_media_info_get_is_muted(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetIsMuted();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_media_info_get_embed_id(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetEmbedId();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_media_info_get_poster_url(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetPosterUrl();
}

int ARK_WEB_CALLBACK
ark_web_media_info_get_media_type(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetMediaType();
}

bool ARK_WEB_CALLBACK
ark_web_media_info_get_is_controls_shown(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetIsControlsShown();
}

ArkWebStringVector ARK_WEB_CALLBACK
ark_web_media_info_get_controls(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_vector_default);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetControls();
}

ArkWebStringMap ARK_WEB_CALLBACK
ark_web_media_info_get_headers(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_map_default);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetHeaders();
}

ArkWebStringMap ARK_WEB_CALLBACK
ark_web_media_info_get_attributes(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_map_default);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetAttributes();
}

ArkWebMediaSourceInfoVector ARK_WEB_CALLBACK
ark_web_media_info_get_source_infos(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_media_source_info_vector_default);

  // Execute
  return ArkWebMediaInfoCppToC::Get(self)->GetSourceInfos();
}

ark_web_native_media_player_surface_info_t *ARK_WEB_CALLBACK
ark_web_media_info_get_surface_info(struct _ark_web_media_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebNativeMediaPlayerSurfaceInfo> _retval =
      ArkWebMediaInfoCppToC::Get(self)->GetSurfaceInfo();

  // Return type: refptr_same
  return ArkWebNativeMediaPlayerSurfaceInfoCppToC::Invert(_retval);
}

} // namespace

ArkWebMediaInfoCppToC::ArkWebMediaInfoCppToC() {
  GetStruct()->get_preload = ark_web_media_info_get_preload;
  GetStruct()->get_is_muted = ark_web_media_info_get_is_muted;
  GetStruct()->get_embed_id = ark_web_media_info_get_embed_id;
  GetStruct()->get_poster_url = ark_web_media_info_get_poster_url;
  GetStruct()->get_media_type = ark_web_media_info_get_media_type;
  GetStruct()->get_is_controls_shown = ark_web_media_info_get_is_controls_shown;
  GetStruct()->get_controls = ark_web_media_info_get_controls;
  GetStruct()->get_headers = ark_web_media_info_get_headers;
  GetStruct()->get_attributes = ark_web_media_info_get_attributes;
  GetStruct()->get_source_infos = ark_web_media_info_get_source_infos;
  GetStruct()->get_surface_info = ark_web_media_info_get_surface_info;
}

ArkWebMediaInfoCppToC::~ArkWebMediaInfoCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkWebMediaInfoCppToC, ArkWebMediaInfo,
                                        ark_web_media_info_t>::kBridgeType =
    ARK_WEB_MEDIA_INFO;

} // namespace OHOS::ArkWeb
