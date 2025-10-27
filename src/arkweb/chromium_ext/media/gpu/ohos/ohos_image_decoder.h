// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on vaapi_image_decoder.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#ifndef MEDIA_GPU_OHOS_OHOS_IMAGE_DECODER_H_
#define MEDIA_GPU_OHOS_OHOS_IMAGE_DECODER_H_

#include <stdint.h>

#include <memory>

#include "base/containers/span.h"
#include "base/functional/callback_forward.h"
#include "base/memory/scoped_refptr.h"
#include "gpu/config/gpu_info.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/skia/include/core/SkImageInfo.h"

namespace gfx {
class NativePixmapDmaBuf;
}  // namespace gfx

namespace OHOS::NWeb {
class OhosImageDecoderAdapter;
}

namespace media {

struct NativePixmapAndSizeInfo {
  NativePixmapAndSizeInfo() = default;
  ~NativePixmapAndSizeInfo() = default;

  // The size of the underlying Buffer Object. A use case for this is when an
  // image decode is requested and the caller needs to know the size of the
  // allocated buffer for caching purposes.
  size_t byte_size = 0u;

  // Contains the information needed to use the surface in a graphics API,
  // including the visible size (|pixmap|->GetBufferSize()) which should be no
  // larger than |va_surface_resolution|.
  scoped_refptr<gfx::NativePixmapDmaBuf> pixmap;
};

enum class OhosImageDecodeStatus : uint32_t {
  kSuccess,
  kUnsupportedSubsampling,
  kExecuteDecodeFailed,
  kInvalidWindowBuffer,
};

// This class abstracts the idea of VA-API format-specific decoders. It is the
// responsibility of each subclass to initialize |vaapi_wrapper_| appropriately
// for the purpose of performing hardware-accelerated image decodes of a
// particular format (e.g. JPEG or WebP). Objects of this class are not
// thread-safe, but they are also not thread-affine, i.e., the caller is free to
// call the methods on any thread, but calls must be synchronized externally.
class OhosImageDecoder {
 public:
  explicit OhosImageDecoder();
  OhosImageDecoder(const OhosImageDecoder&) = delete;
  OhosImageDecoder& operator=(const OhosImageDecoder&) = delete;

  virtual ~OhosImageDecoder();

  // Initializes |vaapi_wrapper_| in kDecode mode with the
  // appropriate VAAPI profile and |error_uma_cb| for error reporting.
  virtual bool Initialize();

  virtual OhosImageDecodeStatus Decode(base::span<const uint8_t> encoded_image);
  virtual void ReleaseDecodedPixelMap();

  virtual gpu::ImageDecodeAcceleratorType GetType() const = 0;

  virtual SkYUVColorSpace GetYUVColorSpace() const = 0;

  virtual OHOS::NWeb::AllocatorType GetDecodeAllocatorType() const = 0;

  virtual bool IsYuvFormat() const = 0;

  // Returns the image profile supported by this decoder.
  virtual gpu::ImageDecodeAcceleratorSupportedProfile GetSupportedProfile()
      const;

  OHOS::NWeb::OhosImageDecoderAdapter* GetOhosImageDecoderAdapter();

  // Exports the decoded data from the last Decode() call as a
  // gfx::NativePixmapDmaBuf. Returns nullptr on failure and sets *|status| to
  // the reason for failure. On success, the image decoder gives up ownership of
  // the buffer underlying the NativePixmapDmaBuf.
  virtual std::unique_ptr<media::NativePixmapAndSizeInfo>
  ExportAsNativePixmapDmaBuf(OhosImageDecodeStatus* status);

 private:
  std::unique_ptr<OHOS::NWeb::OhosImageDecoderAdapter> OhosImageDecoderAdapter_;
};

}  // namespace media

#endif  // MEDIA_GPU_VAAPI_VAAPI_IMAGE_DECODER_H_
