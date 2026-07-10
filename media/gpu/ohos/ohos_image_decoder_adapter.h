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

#ifndef MEDIA_GPU_OHOS_IMAGE_DECODER_ADAPTER_OHOS_H_
#define MEDIA_GPU_OHOS_IMAGE_DECODER_ADAPTER_OHOS_H_

#include <multimedia/image_framework/image/image_common.h>
#include <multimedia/image_framework/image/image_source_native.h>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "base/memory/raw_ptr_exclusion.h"
#include "gpu/config/gpu_info.h"

namespace media {

class ImageDecoderAdapterOhos {
 public:
   struct SupportedFormat {
    gpu::ImageDecodeAcceleratorType type;
    uint32_t min_width;
    uint32_t min_height;
    uint32_t max_width;
    uint32_t max_height;
  };
  ImageDecoderAdapterOhos();
  ~ImageDecoderAdapterOhos();

  bool ParseRawData(const uint8_t* data, uint32_t size);
  int32_t GetImageWidth();
  int32_t GetImageHeight();
  bool Decode();
  int32_t GetStride();
  void ReleasePixelMap();
  void* GetDecodeData();
  int32_t GetFd();
  int32_t GetOffset();
  uint64_t GetSize();
  void* GetNativeWindowBuffer();
  int32_t GetPlanesCount();
  OH_PixelmapNative* GetPixelMap();
  std::vector<SupportedFormat> GetSupportedFormats();

 private:
  RAW_PTR_EXCLUSION OH_ImageSourceNative* image_source_ = nullptr;
  RAW_PTR_EXCLUSION OH_ImageSource_Info* image_info_ = nullptr;
  RAW_PTR_EXCLUSION OH_PixelmapNative* pixel_map_ = nullptr;
  RAW_PTR_EXCLUSION OHNativeWindowBuffer* native_window_buffer_ = nullptr;
  RAW_PTR_EXCLUSION OH_NativeBuffer* native_buffer_ = nullptr;
  RAW_PTR_EXCLUSION OH_Pixelmap_InitializationOptions* pixelmap_init_opt_ =
      nullptr;
  RAW_PTR_EXCLUSION BufferHandle* buffer_handle_ = nullptr;
  RAW_PTR_EXCLUSION OH_Pixelmap_ImageInfo* pixel_map_info_ = nullptr;
  RAW_PTR_EXCLUSION OH_DecodingOptions* decoding_options_ = nullptr;
  bool has_lock_pixel_map_ = false;

  bool CreateNativeWindowBuffer();
  bool GetBufferHandle();
  void NativeBufferFromPixelMap();
  void ReleaseDecodeOptions();
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_IMAGE_DECODER_ADAPTER_OHOS_H_
