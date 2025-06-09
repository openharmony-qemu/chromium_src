/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#include "media/capture/video/ohos/video_capture_common_ohos.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {

class VideoCaptureCommonOhosTest : public ::testing::Test {
 public:
  VideoCaptureCommonOhosTest() {}
  ~VideoCaptureCommonOhosTest() override = default;
};

TEST_F(VideoCaptureCommonOhosTest, GetCameraTransportType) {
  EXPECT_EQ(VideoCaptureTransportType::APPLE_USB_OR_BUILT_IN,
            VideoCaptureCommonOHOS::GetCameraTransportType(
                Camera_Connection::CAMERA_CONNECTION_BUILT_IN));
  EXPECT_EQ(VideoCaptureTransportType::APPLE_USB_OR_BUILT_IN,
            VideoCaptureCommonOHOS::GetCameraTransportType(
                Camera_Connection::CAMERA_CONNECTION_USB_PLUGIN));
  EXPECT_EQ(VideoCaptureTransportType::OTHER_TRANSPORT,
            VideoCaptureCommonOHOS::GetCameraTransportType(
                Camera_Connection::CAMERA_CONNECTION_REMOTE));
}

TEST_F(VideoCaptureCommonOhosTest, GetCameraFacingMode) {
  EXPECT_EQ(MEDIA_VIDEO_FACING_NONE,
            VideoCaptureCommonOHOS::GetCameraFacingMode(
                Camera_Position::CAMERA_POSITION_UNSPECIFIED));
  EXPECT_EQ(MEDIA_VIDEO_FACING_ENVIRONMENT,
            VideoCaptureCommonOHOS::GetCameraFacingMode(
                Camera_Position::CAMERA_POSITION_BACK));
  EXPECT_EQ(MEDIA_VIDEO_FACING_USER,
            VideoCaptureCommonOHOS::GetCameraFacingMode(
                Camera_Position::CAMERA_POSITION_FRONT));
}

TEST_F(VideoCaptureCommonOhosTest, GetCameraPixelFormatType) {
  EXPECT_EQ(PIXEL_FORMAT_ABGR, VideoCaptureCommonOHOS::GetCameraPixelFormatType(
                                   Camera_Format::CAMERA_FORMAT_RGBA_8888));
  EXPECT_EQ(PIXEL_FORMAT_NV21, VideoCaptureCommonOHOS::GetCameraPixelFormatType(
                                   Camera_Format::CAMERA_FORMAT_YUV_420_SP));
  EXPECT_EQ(PIXEL_FORMAT_MJPEG,
            VideoCaptureCommonOHOS::GetCameraPixelFormatType(
                Camera_Format::CAMERA_FORMAT_JPEG));
}

TEST_F(VideoCaptureCommonOhosTest, GetAdapterCameraPixelFormatType) {
  EXPECT_EQ(Camera_Format::CAMERA_FORMAT_RGBA_8888,
            VideoCaptureCommonOHOS::GetAdapterCameraPixelFormatType(
                PIXEL_FORMAT_ABGR));
  EXPECT_EQ(Camera_Format::CAMERA_FORMAT_YUV_420_SP,
            VideoCaptureCommonOHOS::GetAdapterCameraPixelFormatType(
                PIXEL_FORMAT_NV21));
  EXPECT_EQ(Camera_Format::CAMERA_FORMAT_JPEG,
            VideoCaptureCommonOHOS::GetAdapterCameraPixelFormatType(
                PIXEL_FORMAT_MJPEG));
}

}  // namespace media
