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

#include <memory>
#include <ostream>
#include "base/logging.h"
#include "media/capture/mojom/image_capture.mojom-forward.h"
#define private public
#include "media/capture/video/ohos/video_capture_device_ohos.h"
#undef private
#include "base/test/task_environment.h"
#include "media/capture/video/ohos/video_capture_device_ohos.cc"
#include "media/capture/video/video_capture_device_descriptor.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {

class VideoCaptureDeviceOHOSTest : public ::testing::Test {
 protected:
  void SetUp() override {
    task_environment_ = std::make_unique<base::test::TaskEnvironment>();
    device_descriptor_ = VideoCaptureDeviceDescriptor();
    device_ = std::make_unique<VideoCaptureDeviceOHOS>(device_descriptor_);
    photo_taken_ = false;
  }
  void TearDown() override {
    device_.reset();
    task_environment_.reset();
  }
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
  VideoCaptureDeviceDescriptor device_descriptor_;
  std::unique_ptr<VideoCaptureDeviceOHOS> device_;
  bool photo_taken_;
};

TEST_F(VideoCaptureDeviceOHOSTest, AllocateAndStart_ThreadIsRunning) {
  device_->capture_thread_.Start();
  VideoCaptureParams params;
  params.requested_format.frame_size.SetSize(640, 480);
  params.requested_format.frame_rate = 30.0f;
  params.requested_format.pixel_format = PIXEL_FORMAT_I420;
  std::unique_ptr<VideoCaptureDevice::Client> client;
  device_->AllocateAndStart(params, std::move(client));
  EXPECT_EQ(device_->capture_impl_, nullptr);
  EXPECT_TRUE(device_->capture_thread_.IsRunning());
}

TEST_F(VideoCaptureDeviceOHOSTest, StopAndDeAllocate_ThreadNotRunning) {
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
  device_->StopAndDeAllocate();
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
}

TEST_F(VideoCaptureDeviceOHOSTest, TakePhoto_ThreadNotRunning) {
  device_->capture_thread_.Stop();
  bool photo_taken = false;
  auto callback = base::BindOnce(
      [](bool* photo_taken, mojom::BlobPtr blob) { *photo_taken = true; },
      &photo_taken_);
  device_->capture_impl_ = std::make_unique<media::OHOSCaptureDelegate>(
      VideoCaptureDeviceDescriptor(),
      base::SingleThreadTaskRunner::GetCurrentDefault(), VideoCaptureParams());
  device_->TakePhoto(std::move(callback));
  EXPECT_EQ(device_->photo_requests_queue_.size(), 1U);
  EXPECT_FALSE(photo_taken);
}

TEST_F(VideoCaptureDeviceOHOSTest, GetPhotoState_ThreadNotRunning) {
  device_->capture_thread_.Stop();
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
  base::OnceCallback<void(mojom::PhotoStatePtr)> callback =
      base::BindOnce([](mojom::PhotoStatePtr photo) { EXPECT_TRUE(photo); });
  device_->capture_impl_ = std::make_unique<media::OHOSCaptureDelegate>(
      VideoCaptureDeviceDescriptor(),
      base::SingleThreadTaskRunner::GetCurrentDefault(), VideoCaptureParams());
  device_->GetPhotoState(std::move(callback));
  EXPECT_EQ(device_->photo_requests_queue_.size(), 1U);
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
}

TEST_F(VideoCaptureDeviceOHOSTest, SetPhotoOptions_ThreadNotRunning) {
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
  mojom::PhotoSettingsPtr settings = mojom::PhotoSettings::New();
  settings->width = 1280;
  settings->height = 720;
  base::OnceCallback<void(bool)> callback =
      base::BindOnce([](bool success) { EXPECT_FALSE(success); });
  device_->capture_impl_ = std::make_unique<media::OHOSCaptureDelegate>(
      VideoCaptureDeviceDescriptor(),
      base::SingleThreadTaskRunner::GetCurrentDefault(), VideoCaptureParams());
  device_->SetPhotoOptions(std::move(settings), std::move(callback));
  EXPECT_EQ(device_->photo_requests_queue_.size(), 1U);
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
}

TEST_F(VideoCaptureDeviceOHOSTest, MaybeSuspend_ThreadNotRunning) {
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
  device_->capture_impl_ = std::make_unique<media::OHOSCaptureDelegate>(
      VideoCaptureDeviceDescriptor(),
      base::SingleThreadTaskRunner::GetCurrentDefault(), VideoCaptureParams());
  device_->MaybeSuspend();
  EXPECT_EQ(device_->photo_requests_queue_.size(), 1U);
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
}

TEST_F(VideoCaptureDeviceOHOSTest, Resume_ThreadNotRunning) {
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
  device_->capture_impl_ = std::make_unique<media::OHOSCaptureDelegate>(
      VideoCaptureDeviceDescriptor(),
      base::SingleThreadTaskRunner::GetCurrentDefault(), VideoCaptureParams());
  device_->Resume();
  EXPECT_EQ(device_->photo_requests_queue_.size(), 1U);
  EXPECT_FALSE(device_->capture_thread_.IsRunning());
}
}  // namespace media
