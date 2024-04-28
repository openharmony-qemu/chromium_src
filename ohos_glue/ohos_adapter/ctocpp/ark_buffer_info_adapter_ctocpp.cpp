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

#include "ohos_adapter/ctocpp/ark_buffer_info_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int64_t ArkBufferInfoAdapterCToCpp::GetPresentationTimeUs() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_buffer_info_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_presentation_time_us, 0);

  // Execute
  return _struct->get_presentation_time_us(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkBufferInfoAdapterCToCpp::GetSize() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_buffer_info_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_size, 0);

  // Execute
  return _struct->get_size(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkBufferInfoAdapterCToCpp::GetOffset() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_buffer_info_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_offset, 0);

  // Execute
  return _struct->get_offset(_struct);
}

ArkBufferInfoAdapterCToCpp::ArkBufferInfoAdapterCToCpp() {
}

ArkBufferInfoAdapterCToCpp::~ArkBufferInfoAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkBufferInfoAdapterCToCpp, ArkBufferInfoAdapter,
                           ark_buffer_info_adapter_t>::kBridgeType =
        ARK_BUFFER_INFO_ADAPTER;

} // namespace OHOS::ArkWeb
