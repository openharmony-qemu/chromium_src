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

#include "ohos_adapter/ctocpp/ark_media_codec_list_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/ctocpp/ark_capability_data_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkCapabilityDataAdapter>
ArkMediaCodecListAdapterCToCpp::GetCodecCapability(const ArkWebString mime,
                                                   const bool isCodec) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_media_codec_list_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_codec_capability, nullptr);

  // Execute
  ark_capability_data_adapter_t *_retval =
      _struct->get_codec_capability(_struct, mime, isCodec);

  // Return type: refptr_same
  return ArkCapabilityDataAdapterCToCpp::Invert(_retval);
}

ArkMediaCodecListAdapterCToCpp::ArkMediaCodecListAdapterCToCpp() {
}

ArkMediaCodecListAdapterCToCpp::~ArkMediaCodecListAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkMediaCodecListAdapterCToCpp, ArkMediaCodecListAdapter,
    ark_media_codec_list_adapter_t>::kBridgeType = ARK_MEDIA_CODEC_LIST_ADAPTER;

} // namespace OHOS::ArkWeb
