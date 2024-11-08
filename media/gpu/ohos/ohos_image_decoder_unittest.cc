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

#include <cstddef>
#define private public
#include "media/gpu/ohos/ohos_image_decoder.h"
#undef private
#include "base/logging.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {

class ConcreteOhosImageDecoder : public OhosImageDecoder {
 public:
  gpu::ImageDecodeAcceleratorType GetType() const override {
    return gpu::ImageDecodeAcceleratorType::kUnknown;
  }

  SkYUVColorSpace GetYUVColorSpace() const override {
    return SkYUVColorSpace::kJPEG_Full_SkYUVColorSpace;
  }

  OHOS::NWeb::AllocatorType GetDecodeAllocatorType() const override {
    return OHOS::NWeb::AllocatorType::kDefault;
  }

  bool IsYuvFormat() const override { return false; }
};

TEST(OhosImageDecoderTest, Decode_001) {
  auto concreteDecoder = std::make_unique<ConcreteOhosImageDecoder>();
  concreteDecoder->Initialize();
  std::vector<uint8_t> imageData = {12};
  base::span<const uint8_t> encodedImage(imageData.data(), imageData.size());
  auto ret = concreteDecoder->Decode(encodedImage);
  ASSERT_EQ(ret,OhosImageDecodeStatus::kExecuteDecodeFailed);
}

TEST(OhosImageDecoderTest, ReleaseDecodedPixelMap_001) {
  auto concreteDecoder = std::make_unique<ConcreteOhosImageDecoder>();
  concreteDecoder->Initialize();
  concreteDecoder->ReleaseDecodedPixelMap();
  ASSERT_NE(concreteDecoder->OhosImageDecoderAdapter_,nullptr);
}
}  // namespace media
