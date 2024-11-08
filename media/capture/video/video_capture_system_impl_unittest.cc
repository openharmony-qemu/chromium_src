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
#include "media/capture/video/video_capture_system_impl.h"
#undef private
#include <utility>
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {
class MockVideoCaptureDeviceFactory : public VideoCaptureDeviceFactory {
 public:
  MOCK_METHOD(VideoCaptureErrorOrDevice,
              CreateDevice,
              (const VideoCaptureDeviceDescriptor& device_descriptor),
              (override));
  MOCK_METHOD(void,
              GetDevicesInfo,
              (GetDevicesInfoCallback callback),
              (override));
#if BUILDFLAG(IS_WIN)
  MOCK_METHOD(scoped_refptr<DXGIDeviceManager>,
              GetDxgiDeviceManager,
              (),
              (override));
  MOCK_METHOD(void, OnGpuInfoUpdate, (const CHROME_LUID& luid), (override));
#endif
};

class VideoCaptureSystemImplTest : public ::testing::Test {
 public:
  void SetUp() override {
    std::unique_ptr<MockVideoCaptureDeviceFactory> factory =
        std::make_unique<MockVideoCaptureDeviceFactory>();
    video_capture =
        std::make_shared<VideoCaptureSystemImpl>(std::move(factory));
  }
  void TearDown() override { video_capture.reset(); }
  std::shared_ptr<VideoCaptureSystemImpl> video_capture;
};

TEST_F(VideoCaptureSystemImplTest, CreateDevice) {
  const std::string device_id_ = "device_id_";
  const std::string& device_id = device_id_;
  VideoCaptureErrorOrDevice result = video_capture->CreateDevice(device_id);
  EXPECT_EQ(result.error_code_,
            VideoCaptureError::kVideoCaptureSystemDeviceIdNotFound);
}

}  // namespace media
