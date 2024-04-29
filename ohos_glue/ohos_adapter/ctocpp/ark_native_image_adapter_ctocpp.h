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

#ifndef ARK_NATIVE_IMAGE_ADAPTER_CTOCPP_H_
#define ARK_NATIVE_IMAGE_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_graphic_adapter_capi.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkNativeImageAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkNativeImageAdapterCToCpp,
                                    ArkNativeImageAdapter,
                                    ark_native_image_adapter_t> {
public:
  ArkNativeImageAdapterCToCpp();
  virtual ~ArkNativeImageAdapterCToCpp();

  // ArkNativeImageAdapter methods.
  void CreateNativeImage(uint32_t textureId, uint32_t textureTarget) override;

  void *AquireNativeWindowFromNativeImage() override;

  int32_t AttachContext(uint32_t textureId) override;

  int32_t DetachContext() override;

  int32_t UpdateSurfaceImage() override;

  int64_t GetTimestamp() override;

  int32_t GetTransformMatrix(float matrix[16]) override;

  int32_t GetSurfaceId(uint64_t *surfaceId) override;

  int32_t SetOnFrameAvailableListener(
      ArkWebRefPtr<ArkFrameAvailableListener> listener) override;

  int32_t UnsetOnFrameAvailableListener() override;

  void DestroyNativeImage() override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_NATIVE_IMAGE_ADAPTER_CTOCPP_H_
