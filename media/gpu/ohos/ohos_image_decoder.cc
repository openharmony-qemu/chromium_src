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

#include "media/gpu/ohos/ohos_image_decoder.h"

#include <utility>

#include "base/files/scoped_file.h"
#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"
#include "media/gpu/macros.h"
#include "media/gpu/ohos/ohos_image_decoder_adapter.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/linux/native_pixmap_dmabuf.h"

namespace media {

OhosImageDecoder::OhosImageDecoder() = default;
OhosImageDecoder::~OhosImageDecoder() = default;

bool OhosImageDecoder::Initialize() {
  decoder_adapter_ = std::make_unique<ImageDecoderAdapterOhos>();
  if (!decoder_adapter_) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] Failed to create decoder adapter.";
    return false;
  }
  return true;
}

OhosImageDecodeStatus OhosImageDecoder::Decode(
    base::span<const uint8_t> encoded_image) {
  if (!decoder_adapter_) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] decoder_adapter_ is null.";
    return OhosImageDecodeStatus::kExecuteDecodeFailed;
  }
  if (!decoder_adapter_->ParseRawData(encoded_image.data(),
                                      encoded_image.size())) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] ParseRawData failed.";
    return OhosImageDecodeStatus::kExecuteDecodeFailed;
  }
  if (!decoder_adapter_->Decode()) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] DecodeToPixelMap failed.";
    return OhosImageDecodeStatus::kExecuteDecodeFailed;
  }
  return OhosImageDecodeStatus::kSuccess;
}

std::vector<gpu::ImageDecodeAcceleratorSupportedProfile>
OhosImageDecoder::GetSupportedProfiles() const {
  std::vector<gpu::ImageDecodeAcceleratorSupportedProfile> profiles;
  if (!decoder_adapter_) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] !decoder_adapter_";
    return profiles;
  }
  auto supported_formats = decoder_adapter_->GetSupportedFormats();
  for (const auto& format : supported_formats) {
    gpu::ImageDecodeAcceleratorSupportedProfile profile;
    profile.image_type = format.type;
    profile.min_encoded_dimensions =
        gfx::Size(format.min_width, format.min_height);
    profile.max_encoded_dimensions =
        gfx::Size(format.max_width, format.max_height);
    profile.subsamplings.push_back(
        gpu::ImageDecodeAcceleratorSubsampling::k420);
    profiles.push_back(profile);
  }
  return profiles;
}

std::unique_ptr<media::NativePixmapAndSizeInfo>
OhosImageDecoder::ExportAsNativePixmapDmaBuf(OhosImageDecodeStatus* status) {
  if (!status || !decoder_adapter_) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode]   status or GetOhosImageDecoderAdapter is null";
    return nullptr;
  }
  auto* window_buffer = decoder_adapter_->GetNativeWindowBuffer();
  if (!window_buffer) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode]  windows "
                  "buffer is nullptr";
    *status = OhosImageDecodeStatus::kInvalidWindowBuffer;
    return nullptr;
  }
  gfx::NativePixmapHandle handle{};
  int planes_count = IsYuvFormat() ? decoder_adapter_->GetPlanesCount() : 1;
  for (auto num = 0; num < planes_count; num++) {
    auto plane_fd =
        base::ScopedFD(HANDLE_EINTR(dup(decoder_adapter_->GetFd())));
    handle.planes.emplace_back(
        decoder_adapter_->GetStride(), decoder_adapter_->GetOffset(),
        decoder_adapter_->GetSize(), std::move(plane_fd));
  }
  std::unique_ptr<media::NativePixmapAndSizeInfo> exported_pixmap =
      std::make_unique<media::NativePixmapAndSizeInfo>();
  if (!exported_pixmap) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] exported_pixmap is nullptr";
    return nullptr;
  }
  exported_pixmap->byte_size = decoder_adapter_->GetSize();
  exported_pixmap->pixmap = base::MakeRefCounted<gfx::NativePixmapDmaBuf>(
      gfx::Size(decoder_adapter_->GetImageWidth(),
                decoder_adapter_->GetImageHeight()),
      IsYuvFormat() ? gfx::BufferFormat::YUV_420_BIPLANAR
                    : gfx::BufferFormat::BGRA_8888,
      std::move(handle), window_buffer);
  *status = OhosImageDecodeStatus::kSuccess;
  return exported_pixmap;
}

void OhosImageDecoder::ReleaseDecodedPixelMap() {
  if (decoder_adapter_) {
    decoder_adapter_->ReleasePixelMap();
  }
}

SkYUVColorSpace OhosImageDecoder::GetYUVColorSpace() const {
  return SkYUVColorSpace::kIdentity_SkYUVColorSpace;
}

}  // namespace media
