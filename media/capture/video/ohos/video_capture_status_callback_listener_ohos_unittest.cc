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
#include <gtest/gtest.h>
#include "base/task/single_thread_task_runner.h"
#include "gmock/gmock.h"
#define private public
#include "base/logging.h"
#include "media/capture/video/ohos/video_capture_device_factory_ohos.h"
#include "ohos_adapter_helper.h"
#include "ohos_capture_delegate.h"
#include "video_capture_device_ohos.h"
#include "video_capture_status_callback_listener_ohos.h"
#undef private
using namespace testing;

namespace media {

namespace {
std::shared_ptr<VideoCaptureCameraStatusCallbackListenerOHOS> c_callback;

}

class VideoCaptureDeviceFactoryOHOSMock : public VideoCaptureDeviceFactoryOHOS {
 public:
  VideoCaptureDeviceFactoryOHOSMock()
      : VideoCaptureDeviceFactoryOHOS(nullptr) {}

  MOCK_METHOD(VideoCaptureErrorOrDevice,
              CreateDevice,
              (const VideoCaptureDeviceDescriptor& device_descriptor),
              (override));
  MOCK_METHOD(void,
              GetDevicesInfo,
              (GetDevicesInfoCallback callback),
              (override));
  MOCK_METHOD(void,
              OnCameraStatusChanged,
              (CameraStatusAdapter camera_status,
               std::string callback_device_id),
              ());
};

class SingleThreadTaskRunnerMock : public base::SingleThreadTaskRunner {
 public:
  MOCK_METHOD(bool,
              PostDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
};

class VideoCaptureCameraStatusCallbackListenerOHOSTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};

void VideoCaptureCameraStatusCallbackListenerOHOSTest::SetUpTestCase(void) {}
void VideoCaptureCameraStatusCallbackListenerOHOSTest::TearDownTestCase(void) {}
void VideoCaptureCameraStatusCallbackListenerOHOSTest::SetUp(void) {
  scoped_refptr<SingleThreadTaskRunnerMock> task_runner =
      base::MakeRefCounted<SingleThreadTaskRunnerMock>();
  auto capture_device_factory =
      std::make_unique<VideoCaptureDeviceFactoryOHOSMock>();
  auto weak_factory =
      std::make_unique<base::WeakPtrFactory<VideoCaptureDeviceFactoryOHOSMock>>(
          capture_device_factory.get());
  base::WeakPtr<VideoCaptureDeviceFactoryOHOSMock> weak_factory_ptr =
      weak_factory->GetWeakPtr();
  c_callback = std::make_shared<VideoCaptureCameraStatusCallbackListenerOHOS>(
      task_runner, weak_factory_ptr);
  ASSERT_NE(c_callback, nullptr);
}

void VideoCaptureCameraStatusCallbackListenerOHOSTest::TearDown(void) {
  c_callback = nullptr;
}

TEST_F(VideoCaptureCameraStatusCallbackListenerOHOSTest,
       OnCameraStatusChanged_001) {
  CameraStatusAdapter camera_status = CameraStatusAdapter::APPEAR;
  std::string callback_device_id = "callback_device_id";
  auto task_runner = std::make_shared<SingleThreadTaskRunnerMock>();
  c_callback->OnCameraStatusChanged(camera_status, callback_device_id);
}
}  // namespace media