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

#ifndef ARK_MEDIA_CODEC_LIST_ADAPTER_CTOCPP_H_
#define ARK_MEDIA_CODEC_LIST_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_media_codec_adapter_capi.h"
#include "ohos_adapter/include/ark_media_codec_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkMediaCodecListAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkMediaCodecListAdapterCToCpp,
                                    ArkMediaCodecListAdapter,
                                    ark_media_codec_list_adapter_t> {
public:
  ArkMediaCodecListAdapterCToCpp();
  virtual ~ArkMediaCodecListAdapterCToCpp();

  // ArkMediaCodecListAdapter methods.
  ArkWebRefPtr<ArkCapabilityDataAdapter>
  GetCodecCapability(const ArkWebString mime, const bool isCodec) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_CODEC_LIST_ADAPTER_CTOCPP_H_
