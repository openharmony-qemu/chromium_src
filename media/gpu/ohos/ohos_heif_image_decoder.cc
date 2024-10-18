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

#include "media/gpu/ohos/ohos_heif_image_decoder.h"

#include "base/files/scoped_file.h"
#include "base/posix/eintr_wrapper.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/gfx/linux/native_pixmap_dmabuf.h"

namespace media {

OhosHeifImageDecoder::OhosHeifImageDecoder() : OhosImageDecoder() {}

OhosHeifImageDecoder::~OhosHeifImageDecoder() = default;

gpu::ImageDecodeAcceleratorType OhosHeifImageDecoder::GetType() const {
  return gpu::ImageDecodeAcceleratorType::kHeif;
}

SkYUVColorSpace OhosHeifImageDecoder::GetYUVColorSpace() const {
  return SkYUVColorSpace::kIdentity_SkYUVColorSpace;
}

std::unique_ptr<media::NativePixmapAndSizeInfo>
OhosHeifImageDecoder::ExportAsNativePixmapDmaBuf(
    OhosImageDecodeStatus* status) {
  auto* window_buffer = GetOhosImageDecoderAdapter()->GetNativeWindowBuffer();
  if (!window_buffer) {
    *status = OhosImageDecodeStatus::kInvalidWindowBuffer;
    return nullptr;
  }

  gfx::NativePixmapHandle handle{};
  int planes_count =
      IsYuvFormat() ? GetOhosImageDecoderAdapter()->GetPlanesCount() : 1;
  for (auto num = 0; num < planes_count; num++) {
    auto plane_fd = base::ScopedFD(
        HANDLE_EINTR(dup(GetOhosImageDecoderAdapter()->GetFd())));
    handle.planes.emplace_back(GetOhosImageDecoderAdapter()->GetStride(),
                               GetOhosImageDecoderAdapter()->GetOffset(),
                               GetOhosImageDecoderAdapter()->GetSize(),
                               std::move(plane_fd));
  }

  std::unique_ptr<media::NativePixmapAndSizeInfo> exported_pixmap =
      std::make_unique<media::NativePixmapAndSizeInfo>();
  exported_pixmap->byte_size = GetOhosImageDecoderAdapter()->GetSize();
  // TODO: To support YUV(NV12) format.
  exported_pixmap->pixmap = base::MakeRefCounted<gfx::NativePixmapDmaBuf>(
      gfx::Size(GetOhosImageDecoderAdapter()->GetImageWidth(),
                GetOhosImageDecoderAdapter()->GetImageHeight()),
      IsYuvFormat() ? gfx::BufferFormat::YUV_420_BIPLANAR
                    : gfx::BufferFormat::RGBA_8888,
      std::move(handle), window_buffer);

  *status = OhosImageDecodeStatus::kSuccess;

  return exported_pixmap;
}

}  // namespace media