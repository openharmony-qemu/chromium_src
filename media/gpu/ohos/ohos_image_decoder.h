/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MEDIA_GPU_OHOS_OHOS_IMAGE_DECODER_H_
#define MEDIA_GPU_OHOS_OHOS_IMAGE_DECODER_H_

#include <stdint.h>

#include <memory>

#include "base/containers/span.h"
#include "base/functional/callback_forward.h"
#include "base/memory/scoped_refptr.h"
#include "gpu/config/gpu_info.h"
#include "third_party/skia/include/core/SkImageInfo.h"

namespace gfx {
class NativePixmapDmaBuf;
}  // namespace gfx

namespace media {

struct NativePixmapAndSizeInfo {
  NativePixmapAndSizeInfo() = default;
  ~NativePixmapAndSizeInfo() = default;

  size_t byte_size = 0u;
  scoped_refptr<gfx::NativePixmapDmaBuf> pixmap;
};

enum class OhosImageDecodeStatus : uint32_t {
  kSuccess,
  kUnsupportedSubsampling,
  kExecuteDecodeFailed,
  kInvalidWindowBuffer,
};

class ImageDecoderAdapterOhos;

class OhosImageDecoder {
 public:
  OhosImageDecoder();
  OhosImageDecoder(const OhosImageDecoder&) = delete;
  OhosImageDecoder& operator=(const OhosImageDecoder&) = delete;
  ~OhosImageDecoder();

  bool Initialize();
  OhosImageDecodeStatus Decode(base::span<const uint8_t> encoded_image);
  SkYUVColorSpace GetYUVColorSpace() const;
  bool IsYuvFormat() const { return false; }
  std::vector<gpu::ImageDecodeAcceleratorSupportedProfile>
  GetSupportedProfiles() const;
  std::unique_ptr<media::NativePixmapAndSizeInfo> ExportAsNativePixmapDmaBuf(
      OhosImageDecodeStatus* status);
  void ReleaseDecodedPixelMap();

 protected:
  std::unique_ptr<ImageDecoderAdapterOhos> decoder_adapter_;
};
}  // namespace media

#endif  // MEDIA_GPU_OHOS_OHOS_IMAGE_DECODER_H_
