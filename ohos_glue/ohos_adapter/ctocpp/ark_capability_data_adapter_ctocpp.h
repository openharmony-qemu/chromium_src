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

#ifndef ARK_CAPABILITY_DATA_ADAPTER_CTOCPP_H_
#define ARK_CAPABILITY_DATA_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_media_codec_adapter_capi.h"
#include "ohos_adapter/include/ark_media_codec_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkCapabilityDataAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkCapabilityDataAdapterCToCpp,
                                    ArkCapabilityDataAdapter,
                                    ark_capability_data_adapter_t> {
public:
  ArkCapabilityDataAdapterCToCpp();
  virtual ~ArkCapabilityDataAdapterCToCpp();

  // ArkCapabilityDataAdapter methods.
  int32_t GetMaxWidth() override;

  int32_t GetMaxHeight() override;

  int32_t GetMaxframeRate() override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_CAPABILITY_DATA_ADAPTER_CTOCPP_H_
