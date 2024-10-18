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

#include <gmock/gmock.h>

#define private public
#include "gtest/gtest.h"
#include "video_capture_common_ohos.h"
#include "media/base/video_frame.h"

using namespace testing;

namespace media {
class MockFormatAdapter : public FormatAdapter {
 public:
  MockFormatAdapter() = default;

  ~MockFormatAdapter() = default;
  MOCK_METHOD(uint32_t, GetWidth, (), (override));
  MOCK_METHOD(uint32_t, GetHeight, (), (override));
  MOCK_METHOD(float, GetFrameRate, (), (override));
  MOCK_METHOD(VideoPixelFormatAdapter, GetPixelFormat, (), (override));
};

TEST(VideoCaptureCommonOHOSTest, TestGetCameraTransportTypeFound) {
  auto transType = OHOS::NWeb::VideoTransportType::VIDEO_TRANS_TYPE_BUILD_IN;
  auto temp = media::VideoCaptureTransportType::MACOSX_USB_OR_BUILT_IN;
  auto result = VideoCaptureCommonOHOS::GetCameraTransportType(transType);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, TestGetCameraFacingModeTypeFound) {
  auto facingMode = OHOS::NWeb::VideoFacingModeAdapter::FACING_USER;
  auto temp = media::MEDIA_VIDEO_FACING_USER;
  auto result = VideoCaptureCommonOHOS::GetCameraFacingMode(facingMode);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, TestGetCameraPixelFormatTypeFound) {
  auto pixelFormat = OHOS::NWeb::VideoPixelFormatAdapter::FORMAT_RGBA_8888;
  auto temp = media::PIXEL_FORMAT_ABGR;
  auto result = VideoCaptureCommonOHOS::GetCameraPixelFormatType(pixelFormat);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, TestGetAdapterCameraPixelFormatTypeFound) {
  auto pixelFormat = media::PIXEL_FORMAT_ABGR;
  auto temp = OHOS::NWeb::VideoPixelFormatAdapter::FORMAT_RGBA_8888;
  auto result = VideoCaptureCommonOHOS::GetAdapterCameraPixelFormatType(pixelFormat);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, TestGetSupportedFormats) {
  std::vector<std::shared_ptr<FormatAdapter>> capture_formats_adapter;
  auto adapter1 = std::make_shared<MockFormatAdapter>();
  EXPECT_CALL(*adapter1, GetWidth()).WillRepeatedly(::testing::Return(640));
  EXPECT_CALL(*adapter1, GetHeight()).WillRepeatedly(::testing::Return(480));
  EXPECT_CALL(*adapter1, GetFrameRate())
      .WillRepeatedly(::testing::Return(30.0f));
  EXPECT_CALL(*adapter1, GetPixelFormat())
      .WillRepeatedly(::testing::Return(
          OHOS::NWeb::VideoPixelFormatAdapter::FORMAT_YUV_420_SP));
  capture_formats_adapter.push_back(adapter1);
  auto result = VideoCaptureCommonOHOS::GetSupportedFormats(capture_formats_adapter);
  auto frameSizeWidth = result[0].frame_size.width();
  auto frameSizeheight = result[0].frame_size.height();
  auto frameRate = result[0].frame_rate;
  auto pixelFormat = result[0].pixel_format;
  auto temp = VideoCaptureCommonOHOS::GetCameraPixelFormatType(
      OHOS::NWeb::VideoPixelFormatAdapter::FORMAT_YUV_420_SP);
  EXPECT_EQ(frameSizeWidth, 640);
  EXPECT_EQ(frameSizeheight, 480);
  EXPECT_FLOAT_EQ(frameRate, 30.0f);
  EXPECT_EQ(pixelFormat, temp);
}
}  // namespace media
