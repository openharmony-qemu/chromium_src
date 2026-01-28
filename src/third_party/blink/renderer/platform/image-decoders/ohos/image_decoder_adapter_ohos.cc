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

#include "third_party/blink/renderer/platform/image-decoders/ohos/image_decoder_adapter_ohos.h"

#include "base/logging.h"

namespace blink {

ImageDecoderAdapterOhos::ImageDecoderAdapterOhos() {
  Image_ErrorCode ret = OH_ImageSourceInfo_Create(&image_info_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] ParseRawData create imageInfo failed, "
                  "result: "
               << ret;
    return;
  }

  ret = OH_PixelmapImageInfo_Create(&pixel_map_info_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] ParseRawData create imageInfo failed, "
                  "result: "
               << ret;
    return;
  }

  ret = OH_DecodingOptions_Create(&decoding_options_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] Create decode options failed, result: "
               << ret;
    return;
  }
}

ImageDecoderAdapterOhos::~ImageDecoderAdapterOhos() {
  ReleasePixelMap();
}

bool ImageDecoderAdapterOhos::ParseRawData(const uint8_t* data, uint32_t size) {
  Image_ErrorCode ret = OH_ImageSourceNative_CreateFromData(
      const_cast<uint8_t*>(data), size, &image_source_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] ParseRawData create imageSource failed, "
                  "result: "
               << ret;
    return false;
  }

  if (image_source_ != nullptr && image_info_ != nullptr) {
    ret = OH_ImageSourceNative_GetImageInfo(image_source_, 0, image_info_);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << "[OhosImageDecode] ParseRawData GetImageInfo failed, result: "
          << ret;
      return false;
    }
  }
  return true;
}

int32_t ImageDecoderAdapterOhos::GetImageWidth() {
  uint32_t width = 0;
  Image_ErrorCode ret = OH_ImageSourceInfo_GetWidth(image_info_, &width);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] GetImageWidth failed, result: " << ret;
  }
  return width;
}

int32_t ImageDecoderAdapterOhos::GetImageHeight() {
  uint32_t height = 0;
  Image_ErrorCode ret = OH_ImageSourceInfo_GetHeight(image_info_, &height);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] GetImageHeight failed, result: " << ret;
  }
  return height;
}

int32_t ImageDecoderAdapterOhos::GetStride() {
  uint32_t row_stride = 0;
  Image_ErrorCode ret =
      OH_PixelmapImageInfo_GetRowStride(pixel_map_info_, &row_stride);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] OH_PixelmapImageInfo GetRowStride failed, "
                  "result: "
               << ret;
  }
  return row_stride;
}

int32_t ImageDecoderAdapterOhos::GetByteCount() {
  uint32_t byte_count = 0;
  Image_ErrorCode ret = OH_PixelmapNative_GetByteCount(pixel_map_, &byte_count);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] OH_PixelmapImageInfo GetByteCount failed, "
                  "result: "
               << ret;
  }
  return byte_count;
}

bool ImageDecoderAdapterOhos::Decode() {
  if (!image_source_) {
    LOG(ERROR) << "[OhosImageDecode] Decode  failed, image_source is null.";
    return false;
  }

  if (has_lock_pixel_map_) {
    LOG(ERROR) << "[OhosImageDecode] pixel map is locked,not decode";
    return false;
  }

  bool decode_success = false;

  do {
    Image_ErrorCode ret = OH_DecodingOptions_SetPixelFormat(
        decoding_options_, PIXEL_FORMAT::PIXEL_FORMAT_BGRA_8888);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << "[OhosImageDecode] Set pixel format failed, result: "
                 << ret;
      break;
    }

    ret = OH_ImageSourceNative_CreatePixelmapUsingAllocator(
        image_source_, decoding_options_, IMAGE_ALLOCATOR_TYPE_DMA,
        &pixel_map_);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << "[OhosImageDecode] Create pixel map failed, result: "
                 << ret;
      break;
    }

    ret = OH_PixelmapNative_GetImageInfo(pixel_map_, pixel_map_info_);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << "[OhosImageDecode] get pixel map info failed, result: "
                 << ret;
      break;
    }

    decode_success = true;
  } while (false);

  if (decoding_options_) {
    ReleaseDecodeOptions();
  }

  return decode_success;
}

void ImageDecoderAdapterOhos::ReleasePixelMap() {
  if (image_source_) {
    Image_ErrorCode ret = OH_ImageSourceNative_Release(image_source_);
    image_source_ = nullptr;
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << "[OhosImageDecode] OH_ImageSourceNative_Release failed, "
                    "result: "
                 << ret;
    }
  }
  if (image_info_) {
    Image_ErrorCode ret = OH_ImageSourceInfo_Release(image_info_);
    image_info_ = nullptr;
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << "[OhosImageDecode] OH_ImageSourceInfo_Release failed, result: "
          << ret;
    }
  }
  if (pixel_map_info_) {
    Image_ErrorCode ret = OH_PixelmapImageInfo_Release(pixel_map_info_);
    pixel_map_info_ = nullptr;
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << "[OhosImageDecode] OH_PixelmapImageInfo_Release failed, result: "
          << ret;
    }
  }
  if (pixel_map_) {
    Image_ErrorCode ret = OH_PixelmapNative_UnaccessPixels(pixel_map_);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << "[OhosImageDecode] OH_PixelmapNative_UnaccessPixels "
                    "failed, result: "
                 << ret;
    }

    ret = OH_PixelmapNative_Release(pixel_map_);
    pixel_map_ = nullptr;
    has_lock_pixel_map_ = false;
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << "[OhosImageDecode] OH_PixelmapNative_Release failed, result: "
          << ret;
    }
  }
}

void* ImageDecoderAdapterOhos::GetDecodeData() {
  if (!pixel_map_) {
    LOG(ERROR) << "[OhosImageDecode] ImageDecoderAdapterOhos::GetDecodeData. "
                  "PixelMap is null.";
    return nullptr;
  }

  void* ptr = nullptr;
  Image_ErrorCode ret = OH_PixelmapNative_AccessPixels(pixel_map_, &ptr);
  has_lock_pixel_map_ = true;
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] ImageDecoderAdapterOhos::GetDecodeData. "
                  "get PixelMap data fail. result:"
               << ret;
    return nullptr;
  }

  return ptr;
}

void ImageDecoderAdapterOhos::ReleaseDecodeOptions() {
  if (!decoding_options_) {
    LOG(ERROR) << "[OhosImageDecode] ReleaseDecodeOptions failed, "
                  "decoding_options_ is nullptr ";
    return;
  }

  Image_ErrorCode ret = OH_DecodingOptions_Release(decoding_options_);
  decoding_options_ = nullptr;
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << "[OhosImageDecode] ReleaseDecodeOptions failed, result: "
               << ret;
  }
}

}  // namespace blink
