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

#include "media/base/video_frame.h"
#include "gtest/gtest.h"
#include "video_capture_common_ohos.h"

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
  auto temp = media::VideoCaptureTransportType::APPLE_USB_OR_BUILT_IN;
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

TEST(VideoCaptureCommonOHOSTest, GetCameraTransportType0) {
  VideoTransportType trans_type = {};
  auto temp = media::VideoCaptureTransportType::APPLE_USB_OR_BUILT_IN;
  auto result = VideoCaptureCommonOHOS::GetCameraTransportType(trans_type);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, GetCameraTransportType1) {
  VideoTransportType trans_type = VideoTransportType::VIDEO_TRANS_TYPE_BUILD_IN;
  auto temp = media::VideoCaptureTransportType::APPLE_USB_OR_BUILT_IN;
  auto result = VideoCaptureCommonOHOS::GetCameraTransportType(trans_type);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, GetCameraTransportType2) {
  testing::internal::CaptureStderr();
  std::string log_output1 = testing::internal::GetCapturedStderr();
  VideoTransportType trans_type = VideoTransportType::VIDEO_TRANS_TYPE_OTHER;
  VideoCaptureCommonOHOS::GetCameraTransportType(trans_type);
  EXPECT_EQ(log_output1.find("concect type:"), std::string::npos);
}

TEST(VideoCaptureCommonOHOSTest, GetCameraFacingMode0) {
  VideoFacingModeAdapter facing_mode = {};
  auto item = FACING_MODE_MAP.find(facing_mode);
  auto result = VideoCaptureCommonOHOS::GetCameraFacingMode(facing_mode);
  EXPECT_EQ(result, item->second);
}

TEST(VideoCaptureCommonOHOSTest, GetCameraFacingMode1) {
  VideoFacingModeAdapter facing_mode =
      VideoFacingModeAdapter::FACING_ENVIRONMENT;
  auto item = FACING_MODE_MAP.find(facing_mode);
  auto result = VideoCaptureCommonOHOS::GetCameraFacingMode(facing_mode);
  EXPECT_EQ(result, item->second);
}

TEST(VideoCaptureCommonOHOSTest, GetCameraFacingMode2) {
  testing::internal::CaptureStderr();
  std::string log_output1 = testing::internal::GetCapturedStderr();
  VideoFacingModeAdapter facing_mode = VideoFacingModeAdapter::NUM_FACING_MODES;
  VideoCaptureCommonOHOS::GetCameraFacingMode(facing_mode);
  EXPECT_EQ(log_output1.find("facine mode:"), std::string::npos);
}

TEST(VideoCaptureCommonOHOSTest, GetCameraPixelFormatType0) {
  VideoPixelFormatAdapter pixel_format = {};
  auto temp = media::PIXEL_FORMAT_ABGR;
  auto result = VideoCaptureCommonOHOS::GetCameraPixelFormatType(pixel_format);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, GetCameraPixelFormatType1) {
  auto temp = media::PIXEL_FORMAT_YUV420P12;
  VideoPixelFormatAdapter pixel_format1 = VideoPixelFormatAdapter::FORMAT_JPEG;
  VideoCaptureCommonOHOS::GetCameraPixelFormatType(pixel_format1);
  VideoPixelFormatAdapter pixel_format2 =
      VideoPixelFormatAdapter::FORMAT_RGBA_8888;
  VideoCaptureCommonOHOS::GetCameraPixelFormatType(pixel_format2);
  VideoPixelFormatAdapter pixel_format3 =
      VideoPixelFormatAdapter::FORMAT_UNKNOWN;
  VideoCaptureCommonOHOS::GetCameraPixelFormatType(pixel_format3);
  VideoPixelFormatAdapter pixel_format4 =
      VideoPixelFormatAdapter::FORMAT_YCBCR_420_888;
  auto result = VideoCaptureCommonOHOS::GetCameraPixelFormatType(pixel_format4);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, GetCameraPixelFormat0) {
  auto temp = media::PIXEL_FORMAT_NV21;
  VideoPixelFormatAdapter pixel_format = {
      VideoPixelFormatAdapter::FORMAT_YUV_420_SP};
  auto result = VideoCaptureCommonOHOS::GetCameraPixelFormatType(pixel_format);
  EXPECT_EQ(result, temp);
}

TEST(VideoCaptureCommonOHOSTest, GetAdapterCameraPixelFormat0) {
  VideoPixelFormat pixel_format = {};
  auto item = ADPATER_PIXEL_FORMAT_MAP.find(pixel_format);
  auto result =
      VideoCaptureCommonOHOS::GetAdapterCameraPixelFormatType(pixel_format);
  EXPECT_EQ(result, item->second);
}

TEST(VideoCaptureCommonOHOSTest, GetAdapterCameraPixelFormat1) {
  testing::internal::CaptureStderr();
  std::string log_output1 = testing::internal::GetCapturedStderr();
  VideoPixelFormat pixel_format = {PIXEL_FORMAT_YUV420P9};
  VideoCaptureCommonOHOS::GetAdapterCameraPixelFormatType(pixel_format);
  EXPECT_EQ(log_output1.find("adapter camera pixel format:"),
            std::string::npos);
}
}  // namespace media