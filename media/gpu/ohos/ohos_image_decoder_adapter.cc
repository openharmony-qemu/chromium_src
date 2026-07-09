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

#include "media/gpu/ohos/ohos_image_decoder_adapter.h"

#include "base/logging.h"
#include "native_buffer/native_buffer.h"

extern "C" {
__attribute__((weak_import)) Image_ErrorCode OH_PixelmapNative_GetByteCount(
    OH_PixelmapNative* pixelmap,
    uint32_t* byteCount);
__attribute__((weak_import)) Image_ErrorCode
OH_ImageSourceNative_CreatePixelmapUsingAllocator(
    OH_ImageSourceNative* source,
    OH_DecodingOptions* options,
    IMAGE_ALLOCATOR_TYPE allocator,
    OH_PixelmapNative** pixelmap);
__attribute__((weak_import)) Image_ErrorCode OH_PixelmapNative_UnaccessPixels(
    OH_PixelmapNative* pixelmap);
__attribute__((weak_import)) Image_ErrorCode OH_PixelmapNative_AccessPixels(
    OH_PixelmapNative* pixelmap,
    void** addr);
__attribute__((weak_import)) Image_ErrorCode
OH_ImageSourceNative_GetSupportedFormats(Image_MimeType** supportedFormats,
                                         size_t* length);
}

namespace media {

// Size limitation of OHOS hardware image decoder.
constexpr int kMaxDecodedImageWidth = 7680;
constexpr int kMaxDecodedImageHeight = 4320;
constexpr int kMinDecodedImageWidth = 2;
constexpr int kMinDecodedImageHeight = 2;

ImageDecoderAdapterOhos::ImageDecoderAdapterOhos() {
  Image_ErrorCode ret = OH_ImageSourceInfo_Create(&image_info_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] ParseRawData create imageInfo failed, result: "
        << ret;
    return;
  }
  ret = OH_PixelmapImageInfo_Create(&pixel_map_info_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] ParseRawData create pixel_map_info_ "
                  "failed, result: "
               << ret;
    return;
  }
  ret = OH_DecodingOptions_Create(&decoding_options_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] Create decode options failed, result: "
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
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] ParseRawData create imageSource failed, result: "
        << ret;
    return false;
  }
  if (!image_info_) {
    ret = OH_ImageSourceInfo_Create(&image_info_);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << __func__
                 << "[OhosImageDecode]  ParseRawData create imageInfo failed, "
                    "errorCode : "
                 << ret;
      return false;
    }
  }
  if (image_source_ != nullptr && image_info_ != nullptr) {
    ret = OH_ImageSourceNative_GetImageInfo(image_source_, 0, image_info_);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << __func__
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
    LOG(ERROR) << __func__
               << "[OhosImageDecode] GetImageWidth failed, result: " << ret;
  }
  return width;
}

int32_t ImageDecoderAdapterOhos::GetImageHeight() {
  uint32_t height = 0;
  Image_ErrorCode ret = OH_ImageSourceInfo_GetHeight(image_info_, &height);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] GetImageHeight failed, result: " << ret;
  }
  return height;
}

int32_t ImageDecoderAdapterOhos::GetStride() {
  uint32_t row_stride = 0;
  Image_ErrorCode ret =
      OH_PixelmapImageInfo_GetRowStride(pixel_map_info_, &row_stride);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] OH_PixelmapImageInfo GetRowStride failed, "
                  "result: "
               << ret;
  }
  return row_stride;
}

bool ImageDecoderAdapterOhos::Decode() {
  if (!image_source_) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] Decode failed, image_source is null.";
    return false;
  }
  if (has_lock_pixel_map_) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] pixel map is locked, not decode";
    return false;
  }
  if (pixel_map_) {
    LOG(WARNING) << __func__ << "[OhosImageDecode] Release existing pixel_map_";
    ReleasePixelMap();
  }
  bool decode_success = false;
  if (!pixel_map_info_) {
    LOG(WARNING)
        << __func__
        << "[OhosImageDecode] pixel_map_info_ is nullptr, creating new one";
    Image_ErrorCode create_ret = OH_PixelmapImageInfo_Create(&pixel_map_info_);
    if (create_ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << __func__
          << "[OhosImageDecode] Create PixelmapImageInfo failed, result: "
          << create_ret;
      ReleaseDecodeOptions();
      return false;
    }
  }
  if (!decoding_options_) {
    LOG(WARNING)
        << __func__
        << "[OhosImageDecode] decoding_options_ is nullptr, creating new one";
    Image_ErrorCode create_ret = OH_DecodingOptions_Create(&decoding_options_);
    if (create_ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << __func__
                 << "[OhosImageDecode] Create decode options failed, result: "
                 << create_ret;
      ReleaseDecodeOptions();
      return false;
    }
  }
  Image_ErrorCode ret = OH_DecodingOptions_SetPixelFormat(
      decoding_options_, PIXEL_FORMAT::PIXEL_FORMAT_BGRA_8888);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] Set pixel format failed, result: " << ret;
  }
  if (!OH_ImageSourceNative_CreatePixelmapUsingAllocator) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] "
           "OH_ImageSourceNative_CreatePixelmapUsingAllocator API not exist.";
    ReleaseDecodeOptions();
    return decode_success;
  }
  ret = OH_ImageSourceNative_CreatePixelmapUsingAllocator(
      image_source_, decoding_options_, IMAGE_ALLOCATOR_TYPE_DMA, &pixel_map_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] Create pixel map failed, result: " << ret;
  }
  ret = OH_PixelmapNative_GetImageInfo(pixel_map_, pixel_map_info_);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] get pixel map info failed, result: "
               << ret;
  }
  decode_success = true;
  ReleaseDecodeOptions();
  return decode_success;
}

void ImageDecoderAdapterOhos::ReleasePixelMap() {
  if (image_source_) {
    Image_ErrorCode ret = OH_ImageSourceNative_Release(image_source_);
    image_source_ = nullptr;
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << __func__
          << "[OhosImageDecode] OH_ImageSourceNative_Release failed, result: "
          << ret;
    }
  }
  if (image_info_) {
    Image_ErrorCode ret = OH_ImageSourceInfo_Release(image_info_);
    image_info_ = nullptr;
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << __func__
          << "[OhosImageDecode] OH_ImageSourceInfo_Release failed, result: "
          << ret;
    }
  }
  if (pixel_map_info_) {
    Image_ErrorCode ret = OH_PixelmapImageInfo_Release(pixel_map_info_);
    pixel_map_info_ = nullptr;
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << __func__
          << "[OhosImageDecode] OH_PixelmapImageInfo_Release failed, result: "
          << ret;
    }
  }
  if (pixel_map_) {
    if (!OH_PixelmapNative_UnaccessPixels) {
      LOG(ERROR) << __func__
                 << "[OhosImageDecode] OH_PixelmapNative_UnaccessPixels API "
                    "not exist.";
      return;
    }
    Image_ErrorCode ret = OH_PixelmapNative_UnaccessPixels(pixel_map_);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << __func__
                 << "[OhosImageDecode] OH_PixelmapNative_UnaccessPixels "
                    "failed, result: "
                 << ret;
    }
    ret = OH_PixelmapNative_Release(pixel_map_);
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << __func__
                 << "[OhosImageDecode] OH_PixelmapNative_Release "
                    "failed, result: "
                 << ret;
    }
    if (native_window_buffer_) {
      OH_NativeWindow_DestroyNativeWindowBuffer(native_window_buffer_);
      native_window_buffer_ = nullptr;
    }
    if (native_buffer_) {
      int errorCode = OH_NativeBuffer_Unreference(native_buffer_);
      native_buffer_ = nullptr;
      if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        LOG(ERROR) << __func__
                   << "[OhosImageDecode] OH_NativeBuffer_Unreference failed, "
                      "errorCode = "
                   << errorCode;
      }
    }
    if (pixelmap_init_opt_) {
      Image_ErrorCode errorCode =
          OH_PixelmapInitializationOptions_Release(pixelmap_init_opt_);
      pixelmap_init_opt_ = nullptr;
      if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        LOG(ERROR)
            << __func__
            << "[OhosImageDecode] OH_PixelmapInitializationOptions_Release "
               "failed, errorCode = "
            << errorCode;
      }
    }
    if (buffer_handle_) {
      buffer_handle_ = nullptr;
    }
    pixel_map_ = nullptr;
    has_lock_pixel_map_ = false;
    if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR)
          << __func__
          << "[OhosImageDecode] OH_PixelmapNative_Release failed, result: "
          << ret;
    }
  }
}

void* ImageDecoderAdapterOhos::GetDecodeData() {
  if (!pixel_map_) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] ImageDecoderAdapterOhos::GetDecodeData. "
                  "PixelMap is null.";
    return nullptr;
  }
  void* ptr = nullptr;
  if (!OH_PixelmapNative_AccessPixels) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] OH_PixelmapNative_AccessPixels API not exist.";
    return ptr;
  }
  Image_ErrorCode ret = OH_PixelmapNative_AccessPixels(pixel_map_, &ptr);
  has_lock_pixel_map_ = true;
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] ImageDecoderAdapterOhos::GetDecodeData. "
                  "get PixelMap data fail. result: "
               << ret;
    return nullptr;
  }
  return ptr;
}

void ImageDecoderAdapterOhos::ReleaseDecodeOptions() {
  if (!decoding_options_) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] ReleaseDecodeOptions failed, "
                  "decoding_options_ is nullptr";
    return;
  }
  Image_ErrorCode ret = OH_DecodingOptions_Release(decoding_options_);
  decoding_options_ = nullptr;
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] ReleaseDecodeOptions failed, result: "
               << ret;
  }
}

int32_t ImageDecoderAdapterOhos::GetFd() {
  if (!GetBufferHandle()) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] bufferHandle is null.";
    return -1;
  }
  return buffer_handle_->fd;
}

int32_t ImageDecoderAdapterOhos::GetOffset() {
  if (!pixel_map_) {
    LOG(WARNING) << __func__ << "[OhosImageDecode] PixelMap is null.";
    return 0;
  }
  NativeBufferFromPixelMap();
  if (native_buffer_) {
    OH_NativeBuffer_Config config;
    OH_NativeBuffer_GetConfig(native_buffer_, &config);
    int32_t format = config.format;
    LOG(INFO) << __func__ << "[OhosImageDecode] buffer format:" << format;
    if (format == OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888 ||
        format == OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_BGRA_8888) {
      return 0;
    } else {
      OH_NativeBuffer_Planes* nativeBufferPlanes = new OH_NativeBuffer_Planes;
      void* virAddr;
      int32_t errorCode = OH_NativeBuffer_MapPlanes(native_buffer_, &virAddr,
                                                    nativeBufferPlanes);
      if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        LOG(ERROR)
            << __func__
            << "[OhosImageDecode] GetOffset get planes failed, errorCode: "
            << errorCode;
        delete nativeBufferPlanes;
        return 0;
      }
      int32_t offset = nativeBufferPlanes->planes[0].offset;
      delete nativeBufferPlanes;
      LOG(INFO) << __func__ << "[OhosImageDecode] GetOffset: " << offset;
      return offset;
    }
  }
  LOG(ERROR) << __func__ << "[OhosImageDecode] GetOffset native buffer is null";
  return 0;
}

void ImageDecoderAdapterOhos::NativeBufferFromPixelMap() {
  if (!pixel_map_) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] GetNativeBuffer pixel_map_ not exists";
    return;
  }
  if (native_buffer_) {
    LOG(INFO) << __func__
              << "[OhosImageDecode] native_buffer_ already exists, return";
    return;
  }
  Image_ErrorCode errorCode =
      OH_PixelmapNative_GetNativeBuffer(pixel_map_, &native_buffer_);
  if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] create native_buffer_ failed, result: "
               << errorCode;
    native_buffer_ = nullptr;
    return;
  }
}

uint64_t ImageDecoderAdapterOhos::GetSize() {
  if (!GetBufferHandle()) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] GetSize bufferHandle is null.";
    return 0;
  }
  return buffer_handle_->size;
}

void* ImageDecoderAdapterOhos::GetNativeWindowBuffer() {
  if (!CreateNativeWindowBuffer()) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] CreateNativeWindowBuffer failed, return nullptr";
    return nullptr;
  }
  return static_cast<void*>(native_window_buffer_);
}

bool ImageDecoderAdapterOhos::CreateNativeWindowBuffer() {
  if (!pixel_map_) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] CreateNativeWindowBuffer PixelMap is nullptr";
    return false;
  }
  NativeBufferFromPixelMap();
  if (!native_buffer_) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] CreateNativeWindowBuffer nativeBuffer is nullptr";
    return false;
  }
  native_window_buffer_ =
      OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(native_buffer_);
  if (!native_window_buffer_) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] CreateNativeWindowBuffer create "
                  "nativeWindowBuffer failed.";
    return false;
  }
  return true;
}

int32_t ImageDecoderAdapterOhos::GetPlanesCount() {
  if (!pixel_map_) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] PixelMap is null.";
    return 0;
  }
  NativeBufferFromPixelMap();
  if (native_buffer_) {
    OH_NativeBuffer_Planes* nativeBufferPlanes = new OH_NativeBuffer_Planes;
    void* virAddr;
    int32_t errorCode =
        OH_NativeBuffer_MapPlanes(native_buffer_, &virAddr, nativeBufferPlanes);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
      LOG(ERROR) << __func__
                 << "[OhosImageDecode] get planes failed, errorCode %{public}d"
                 << errorCode;
      delete nativeBufferPlanes;
      return 0;
    }
    int32_t planeCount = static_cast<int32_t>(nativeBufferPlanes->planeCount);
    delete nativeBufferPlanes;
    return planeCount;
  }
  LOG(ERROR) << __func__ << "[OhosImageDecode] native buffer is null";
  return 0;
}

bool ImageDecoderAdapterOhos::GetBufferHandle() {
  if (buffer_handle_) {
    return true;
  }
  if (!native_window_buffer_ && !CreateNativeWindowBuffer()) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] GetBufferHandle, nativeWindowBuffer get "
                  "and create both failed.";
    return false;
  }
  buffer_handle_ =
      OH_NativeWindow_GetBufferHandleFromNative(native_window_buffer_);
  if (!buffer_handle_) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] GetBufferHandle, get bufferHandle failed.";
    return false;
  }
  return true;
}

OH_PixelmapNative* ImageDecoderAdapterOhos::GetPixelMap() {
  return pixel_map_;
}

std::vector<ImageDecoderAdapterOhos::SupportedFormat> ImageDecoderAdapterOhos::GetSupportedFormats() {
  std::vector<SupportedFormat> formats;
  if (!OH_ImageSourceNative_GetSupportedFormats) {
    LOG(WARNING) << __func__
                 << "[OhosImageDecode] "
                    "OH_ImageSourceNative_GetSupportedFormats API not exist.";
    return formats;
  }
  size_t length = 0;
  Image_MimeType* supported_formats = nullptr;
  Image_ErrorCode ret =
      OH_ImageSourceNative_GetSupportedFormats(&supported_formats, &length);
  if (ret != Image_ErrorCode::IMAGE_SUCCESS) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] GetSupportedFormats failed, result: "
               << ret;
    return formats;
  }
  if (length == 0 || supported_formats == nullptr) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] GetSupportedFormats returned empty list";
    return formats;
  }
  for (size_t i = 0; i < length; ++i) {
    SupportedFormat format;
    format.type = gpu::ImageDecodeAcceleratorType::kUnknown;
    format.min_width = kMinDecodedImageWidth;
    format.min_height = kMinDecodedImageHeight;
    format.max_width = kMaxDecodedImageWidth;
    format.max_height = kMaxDecodedImageHeight;
    if (supported_formats[i].data != nullptr && supported_formats[i].size > 0) {
      std::string mime_type(supported_formats[i].data,
                            supported_formats[i].size);
      if (mime_type == "image/jpeg" || mime_type == "image/jpg") {
        format.type = gpu::ImageDecodeAcceleratorType::kJpeg;
      } else if (mime_type == "image/webp") {
        format.type = gpu::ImageDecodeAcceleratorType::kWebP;
      }
      LOG(INFO) << __func__
                << "[OhosImageDecode] Supported format: " << mime_type
                << " (type: " << static_cast<int>(format.type) << ")";
      if (format.type != gpu::ImageDecodeAcceleratorType::kUnknown) {
        formats.push_back(format);
      }
    }
  }
  return formats;
}

}  // namespace media
