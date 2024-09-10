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

#define private public
#include "video_capture_types_mojom_traits.h"
#include "media/capture/video_capture_types.h"
#undef private

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/gfx/geometry/mojom/geometry.mojom.h"
#include "ui/gfx/geometry/mojom/geometry_mojom_traits.h"

namespace mojo {
class VideoCaptureTypesMojomTraitsTest : public ::testing ::Test {
  void SetUp() override;
  void TearDown() override;
};

void VideoCaptureTypesMojomTraitsTest::SetUp(void) {}

void VideoCaptureTypesMojomTraitsTest::TearDown(void) {}

TEST_F(VideoCaptureTypesMojomTraitsTest,
       kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed_001) {
  media::mojom::VideoCaptureError result;
  media::VideoCaptureError input_ = media::VideoCaptureError::
      kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed;
  result = EnumTraits<media::mojom::VideoCaptureError,
                      media::VideoCaptureError>::ToMojom(input_);
  EXPECT_EQ(result, media::mojom::VideoCaptureError::
                        kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed);
}

TEST_F(VideoCaptureTypesMojomTraitsTest,
       kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed_002) {
  media::mojom::VideoCaptureError result;
  media::VideoCaptureError input_ =
      media::VideoCaptureError::kWinDirectShowDeviceInitializationFailed;
  result = EnumTraits<media::mojom::VideoCaptureError,
                      media::VideoCaptureError>::ToMojom(input_);
  EXPECT_NE(result, media::mojom::VideoCaptureError::
                        kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed);
}

TEST_F(VideoCaptureTypesMojomTraitsTest,
       kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed_003) {
  bool result;
  media::mojom::VideoCaptureError input_ =
      media::mojom::VideoCaptureError::kWinDirectShowDeviceInitializationFailed;
  media::VideoCaptureError output_ =
      media::VideoCaptureError::kWinDirectShowDeviceInitializationFailed;
  media::VideoCaptureError* output_1 = &output_;
  result = EnumTraits<media::mojom::VideoCaptureError,
                      media::VideoCaptureError>::FromMojom(input_, output_1);
  EXPECT_NE(*output_1, media::VideoCaptureError::
                           kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed);
}

TEST_F(VideoCaptureTypesMojomTraitsTest,
       kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed_004) {
  bool result;
  media::mojom::VideoCaptureError input_ = media::mojom::VideoCaptureError::
      kVideoCaptureDeviceFactoryOHOSCreateDeviceFailed;
  media::VideoCaptureError output_ =
      media::VideoCaptureError::kWinDirectShowDeviceInitializationFailed;
  media::VideoCaptureError* output_1 = &output_;
  result = EnumTraits<media::mojom::VideoCaptureError,
                      media::VideoCaptureError>::FromMojom(input_, output_1);
  EXPECT_EQ(result, true);
}
}  // namespace mojo
