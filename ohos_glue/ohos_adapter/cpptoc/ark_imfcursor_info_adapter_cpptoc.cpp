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

#include "ohos_adapter/cpptoc/ark_imfcursor_info_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

double ARK_WEB_CALLBACK ark_imfcursor_info_adapter_get_left(
    struct _ark_imfcursor_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkIMFCursorInfoAdapterCppToC::Get(self)->GetLeft();
}

double ARK_WEB_CALLBACK
ark_imfcursor_info_adapter_get_top(struct _ark_imfcursor_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkIMFCursorInfoAdapterCppToC::Get(self)->GetTop();
}

double ARK_WEB_CALLBACK ark_imfcursor_info_adapter_get_width(
    struct _ark_imfcursor_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkIMFCursorInfoAdapterCppToC::Get(self)->GetWidth();
}

double ARK_WEB_CALLBACK ark_imfcursor_info_adapter_get_height(
    struct _ark_imfcursor_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkIMFCursorInfoAdapterCppToC::Get(self)->GetHeight();
}

} // namespace

ArkIMFCursorInfoAdapterCppToC::ArkIMFCursorInfoAdapterCppToC() {
  GetStruct()->get_left = ark_imfcursor_info_adapter_get_left;
  GetStruct()->get_top = ark_imfcursor_info_adapter_get_top;
  GetStruct()->get_width = ark_imfcursor_info_adapter_get_width;
  GetStruct()->get_height = ark_imfcursor_info_adapter_get_height;
}

ArkIMFCursorInfoAdapterCppToC::~ArkIMFCursorInfoAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkIMFCursorInfoAdapterCppToC, ArkIMFCursorInfoAdapter,
    ark_imfcursor_info_adapter_t>::kBridgeType = ARK_IMFCURSOR_INFO_ADAPTER;

} // namespace OHOS::ArkWeb
