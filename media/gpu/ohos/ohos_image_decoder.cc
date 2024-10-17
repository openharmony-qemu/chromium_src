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

#include "media/gpu/ohos/ohos_image_decoder.h"

#include <utility>

#include "base/logging.h"
#include "media/gpu/macros.h"
#include "ui/gfx/geometry/size.h"

namespace media {

namespace {

// Size limitation of OHOS hardware image decoder.
constexpr int kMaxDecodedImageWidth = 7680;
constexpr int kMaxDecodedImageHeight = 4320;
constexpr int kMinDecodedImageWidth = 2;
constexpr int kMinDecodedImageHeight = 2;

}  // namespace

OhosImageDecoder::OhosImageDecoder() = default;
OhosImageDecoder::~OhosImageDecoder() = default;

bool OhosImageDecoder::Initialize() {
  OhosImageDecoderAdapter_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .CreateOhosImageDecoderAdapter();
  return !!OhosImageDecoderAdapter_;
}

OhosImageDecodeStatus OhosImageDecoder::Decode(
    base::span<const uint8_t> encoded_image) {
  if (!OhosImageDecoderAdapter_->Decode(
          encoded_image.data(), encoded_image.size(), GetDecodeAllocatorType(),
          IsYuvFormat())) {
    LOG(ERROR) << "[HeifSupport] Decode image failed."
    return OhosImageDecodeStatus::kExecuteDecodeFailed;
  }
  LOG(INFO) << "[HeifSupport] Decode native window buffer "
            << GetOhosImageDecoderAdapter()->GetNativeWindowBuffer();
  return OhosImageDecodeStatus::kSuccess;
}

gpu::ImageDecodeAcceleratorSupportedProfile
OhosImageDecoder::GetSupportedProfile() const {
  gpu::ImageDecodeAcceleratorSupportedProfile profile;
  profile.image_type = GetType();
  profile.min_encoded_dimensions =
      gfx::Size(kMinDecodedImageWidth, kMinDecodedImageHeight);
  profile.max_encoded_dimensions =
      gfx::Size(kMaxDecodedImageWidth, kMaxDecodedImageHeight);
  DCHECK_NE(gpu::ImageDecodeAcceleratorType::kUnknown, profile.image_type);

  profile.subsamplings.push_back(gpu::ImageDecodeAcceleratorSubsampling::k420);
  return profile;
}

std::unique_ptr<media::NativePixmapAndSizeInfo>
OhosImageDecoder::ExportAsNativePixmapDmaBuf(OhosImageDecodeStatus* status) {
  return nullptr;
}

OHOS::NWeb::OhosImageDecoderAdapter*
OhosImageDecoder::GetOhosImageDecoderAdapter() {
  return OhosImageDecoderAdapter_.get();
}

void OhosImageDecoder::ReleaseDecodedPixelMap() {
  if (OhosImageDecoderAdapter_) {
    LOG(INFO) << "[HeifSupport] ReleaseDecodedPixelMap native window buffer "
              << GetOhosImageDecoderAdapter()->GetNativeWindowBuffer();
    OhosImageDecoderAdapter_->ReleasePixelMap();
  }
}

}  // namespace media
