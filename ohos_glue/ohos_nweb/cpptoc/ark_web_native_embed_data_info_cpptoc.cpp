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

#include "ohos_nweb/cpptoc/ark_web_native_embed_data_info_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/cpptoc/ark_web_native_embed_info_cpptoc.h"

namespace OHOS::ArkWeb {

namespace {

int ARK_WEB_CALLBACK ark_web_native_embed_data_info_get_status(
    struct _ark_web_native_embed_data_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNativeEmbedDataInfoCppToC::Get(self)->GetStatus();
}

ArkWebString ARK_WEB_CALLBACK ark_web_native_embed_data_info_get_embed_id(
    struct _ark_web_native_embed_data_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNativeEmbedDataInfoCppToC::Get(self)->GetEmbedId();
}

ArkWebString ARK_WEB_CALLBACK ark_web_native_embed_data_info_get_surface_id(
    struct _ark_web_native_embed_data_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNativeEmbedDataInfoCppToC::Get(self)->GetSurfaceId();
}

ark_web_native_embed_info_t *ARK_WEB_CALLBACK
ark_web_native_embed_data_info_get_native_embed_info(
    struct _ark_web_native_embed_data_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebNativeEmbedInfo> _retval =
      ArkWebNativeEmbedDataInfoCppToC::Get(self)->GetNativeEmbedInfo();

  // Return type: refptr_same
  return ArkWebNativeEmbedInfoCppToC::Invert(_retval);
}

} // namespace

ArkWebNativeEmbedDataInfoCppToC::ArkWebNativeEmbedDataInfoCppToC() {
  GetStruct()->get_status = ark_web_native_embed_data_info_get_status;
  GetStruct()->get_embed_id = ark_web_native_embed_data_info_get_embed_id;
  GetStruct()->get_surface_id = ark_web_native_embed_data_info_get_surface_id;
  GetStruct()->get_native_embed_info =
      ark_web_native_embed_data_info_get_native_embed_info;
}

ArkWebNativeEmbedDataInfoCppToC::~ArkWebNativeEmbedDataInfoCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebNativeEmbedDataInfoCppToC, ArkWebNativeEmbedDataInfo,
    ark_web_native_embed_data_info_t>::kBridgeType =
    ARK_WEB_NATIVE_EMBED_DATA_INFO;

} // namespace OHOS::ArkWeb
