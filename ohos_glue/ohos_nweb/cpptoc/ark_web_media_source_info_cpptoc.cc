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

#include "ohos_nweb/cpptoc/ark_web_media_source_info_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int ARK_WEB_CALLBACK
ark_web_media_source_info_get_type(struct _ark_web_media_source_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebMediaSourceInfoCppToC::Get(self)->GetType();
}

ArkWebString ARK_WEB_CALLBACK ark_web_media_source_info_get_format(
    struct _ark_web_media_source_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebMediaSourceInfoCppToC::Get(self)->GetFormat();
}

ArkWebString ARK_WEB_CALLBACK ark_web_media_source_info_get_source(
    struct _ark_web_media_source_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebMediaSourceInfoCppToC::Get(self)->GetSource();
}

} // namespace

ArkWebMediaSourceInfoCppToC::ArkWebMediaSourceInfoCppToC() {
  GetStruct()->get_type = ark_web_media_source_info_get_type;
  GetStruct()->get_format = ark_web_media_source_info_get_format;
  GetStruct()->get_source = ark_web_media_source_info_get_source;
}

ArkWebMediaSourceInfoCppToC::~ArkWebMediaSourceInfoCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebMediaSourceInfoCppToC, ArkWebMediaSourceInfo,
                           ark_web_media_source_info_t>::kBridgeType =
        ARK_WEB_MEDIA_SOURCE_INFO;

} // namespace OHOS::ArkWeb
