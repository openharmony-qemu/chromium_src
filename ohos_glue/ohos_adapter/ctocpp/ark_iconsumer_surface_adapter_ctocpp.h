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

#ifndef ARK_ICONSUMER_SURFACE_ADAPTER_CTOCPP_H_
#define ARK_ICONSUMER_SURFACE_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_graphic_adapter_capi.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkIConsumerSurfaceAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkIConsumerSurfaceAdapterCToCpp,
                                    ArkIConsumerSurfaceAdapter,
                                    ark_iconsumer_surface_adapter_t> {
public:
  ArkIConsumerSurfaceAdapterCToCpp();
  virtual ~ArkIConsumerSurfaceAdapterCToCpp();

  // ArkIConsumerSurfaceAdapter methods.
  int32_t RegisterConsumerListener(
      ArkWebRefPtr<ArkIBufferConsumerListenerAdapter> listener) override;

  int32_t ReleaseBuffer(ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer,
                        int32_t fence) override;

  int32_t SetUserData(const ArkWebString &key,
                      const ArkWebString &val) override;

  int32_t SetQueueSize(uint32_t queueSize) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_ICONSUMER_SURFACE_ADAPTER_CTOCPP_H_
