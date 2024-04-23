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

#include "ohos_nweb/cpptoc/ark_web_cursor_info_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK
ark_web_cursor_info_get_x(struct _ark_web_cursor_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebCursorInfoCppToC::Get(self)->GetX();
}

int32_t ARK_WEB_CALLBACK
ark_web_cursor_info_get_y(struct _ark_web_cursor_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebCursorInfoCppToC::Get(self)->GetY();
}

uint8_t *ARK_WEB_CALLBACK
ark_web_cursor_info_get_buff(struct _ark_web_cursor_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  return ArkWebCursorInfoCppToC::Get(self)->GetBuff();
}

float ARK_WEB_CALLBACK
ark_web_cursor_info_get_scale(struct _ark_web_cursor_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebCursorInfoCppToC::Get(self)->GetScale();
}

int32_t ARK_WEB_CALLBACK
ark_web_cursor_info_get_width(struct _ark_web_cursor_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebCursorInfoCppToC::Get(self)->GetWidth();
}

int32_t ARK_WEB_CALLBACK
ark_web_cursor_info_get_height(struct _ark_web_cursor_info_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebCursorInfoCppToC::Get(self)->GetHeight();
}

} // namespace

ArkWebCursorInfoCppToC::ArkWebCursorInfoCppToC() {
  GetStruct()->get_x = ark_web_cursor_info_get_x;
  GetStruct()->get_y = ark_web_cursor_info_get_y;
  GetStruct()->get_buff = ark_web_cursor_info_get_buff;
  GetStruct()->get_scale = ark_web_cursor_info_get_scale;
  GetStruct()->get_width = ark_web_cursor_info_get_width;
  GetStruct()->get_height = ark_web_cursor_info_get_height;
}

ArkWebCursorInfoCppToC::~ArkWebCursorInfoCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebCursorInfoCppToC, ArkWebCursorInfo,
                           ark_web_cursor_info_t>::kBridgeType =
        ARK_WEB_CURSOR_INFO;

} // namespace OHOS::ArkWeb
