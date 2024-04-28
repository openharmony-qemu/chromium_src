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

#include "ohos_adapter/cpptoc/ark_recorder_info_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

ArkWebString ARK_WEB_CALLBACK
ark_recorder_info_adapter_get_url(struct _ark_recorder_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkRecorderInfoAdapterCppToC::Get(self)->GetUrl();
}

int32_t ARK_WEB_CALLBACK ark_recorder_info_adapter_get_file_format(
    struct _ark_recorder_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkRecorderInfoAdapterCppToC::Get(self)->GetFileFormat();
}

} // namespace

ArkRecorderInfoAdapterCppToC::ArkRecorderInfoAdapterCppToC() {
  GetStruct()->get_url = ark_recorder_info_adapter_get_url;
  GetStruct()->get_file_format = ark_recorder_info_adapter_get_file_format;
}

ArkRecorderInfoAdapterCppToC::~ArkRecorderInfoAdapterCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkRecorderInfoAdapterCppToC, ArkRecorderInfoAdapter,
                           ark_recorder_info_adapter_t>::kBridgeType =
        ARK_RECORDER_INFO_ADAPTER;

} // namespace OHOS::ArkWeb
