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

#ifndef MEDIA_GPU_OHOS_OHOS_HEIF_IMAGE_DECODER_H_
#define MEDIA_GPU_OHOS_OHOS_HEIF_IMAGE_DECODER_H_

#include <stdint.h>

#include <memory>

#include "base/containers/span.h"
#include "base/functional/callback_forward.h"
#include "base/memory/scoped_refptr.h"
#include "media/gpu/ohos/ohos_image_decoder.h"

namespace media {

class OhosHeifImageDecoder : public OhosImageDecoder {
 public:
  explicit OhosHeifImageDecoder();
  OhosHeifImageDecoder(const OhosHeifImageDecoder&) = delete;
  OhosHeifImageDecoder& operator=(const OhosHeifImageDecoder&) = delete;

  virtual ~OhosHeifImageDecoder();

  // OhosImageDecoder implementation.
  gpu::ImageDecodeAcceleratorType GetType() const override;
  SkYUVColorSpace GetYUVColorSpace() const override;
  bool IsYuvFormat() const override { return false; }
  OHOS::NWeb::AllocatorType GetDecodeAllocatorType() const override {
    return OHOS::NWeb::AllocatorType::kDmaAlloc;
  }

  std::unique_ptr<media::NativePixmapAndSizeInfo> ExportAsNativePixmapDmaBuf(
      OhosImageDecodeStatus* status) override;
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_OHOS_HEIF_IMAGE_DECODER_H_