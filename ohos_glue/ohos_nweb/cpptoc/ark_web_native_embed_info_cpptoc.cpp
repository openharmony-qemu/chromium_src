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

#include "ohos_nweb/cpptoc/ark_web_native_embed_info_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK
ark_web_native_embed_info_get_width(struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetWidth();
}

int32_t ARK_WEB_CALLBACK ark_web_native_embed_info_get_height(
    struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetHeight();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_native_embed_info_get_id(struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetId();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_native_embed_info_get_src(struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetSrc();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_native_embed_info_get_url(struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetUrl();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_native_embed_info_get_type(struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetType();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_native_embed_info_get_tag(struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetTag();
}

ArkWebStringMap ARK_WEB_CALLBACK ark_web_native_embed_info_get_params(
    struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_map_default);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetParams();
}

int32_t ARK_WEB_CALLBACK
ark_web_native_embed_info_get_x(struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetX();
}

int32_t ARK_WEB_CALLBACK
ark_web_native_embed_info_get_y(struct _ark_web_native_embed_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeEmbedInfoCppToC::Get(self)->GetY();
}

} // namespace

ArkWebNativeEmbedInfoCppToC::ArkWebNativeEmbedInfoCppToC() {
  GetStruct()->get_width = ark_web_native_embed_info_get_width;
  GetStruct()->get_height = ark_web_native_embed_info_get_height;
  GetStruct()->get_id = ark_web_native_embed_info_get_id;
  GetStruct()->get_src = ark_web_native_embed_info_get_src;
  GetStruct()->get_url = ark_web_native_embed_info_get_url;
  GetStruct()->get_type = ark_web_native_embed_info_get_type;
  GetStruct()->get_tag = ark_web_native_embed_info_get_tag;
  GetStruct()->get_params = ark_web_native_embed_info_get_params;
  GetStruct()->get_x = ark_web_native_embed_info_get_x;
  GetStruct()->get_y = ark_web_native_embed_info_get_y;
}

ArkWebNativeEmbedInfoCppToC::~ArkWebNativeEmbedInfoCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebNativeEmbedInfoCppToC, ArkWebNativeEmbedInfo,
                           ark_web_native_embed_info_t>::kBridgeType =
        ARK_WEB_NATIVE_EMBED_INFO;

} // namespace OHOS::ArkWeb
