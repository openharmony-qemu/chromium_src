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

#ifndef ARK_DISPLAY_ADAPTER_CTOCPP_H_
#define ARK_DISPLAY_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_display_manager_adapter_capi.h"
#include "ohos_adapter/include/ark_display_manager_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkDisplayAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkDisplayAdapterCToCpp, ArkDisplayAdapter,
                                    ark_display_adapter_t> {
public:
  ArkDisplayAdapterCToCpp();
  virtual ~ArkDisplayAdapterCToCpp();

  // ArkDisplayAdapter methods.
  uint64_t GetId() override;

  int32_t GetWidth() override;

  int32_t GetHeight() override;

  float GetVirtualPixelRatio() override;

  uint32_t GetRotation() override;

  uint32_t GetOrientation() override;

  int32_t GetDpi() override;

  uint32_t GetDisplayOrientation() override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_DISPLAY_ADAPTER_CTOCPP_H_
