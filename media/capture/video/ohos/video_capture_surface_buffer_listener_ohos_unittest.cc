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
#include <memory>
#include "base/task/single_thread_task_runner.h"
#include "gmock/gmock.h"
#define private public
#include "base/logging.h"
#include "ohos_adapter_helper.h"
#include "ohos_capture_delegate.h"
#include "video_capture_device_ohos.h"
#include "video_capture_surface_buffer_listener_ohos.h"
#undef private
using namespace testing;

namespace media {

namespace {
std::shared_ptr<VideoCaptureSufaceBufferListenerOHOS> v_listener;

}

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

class VideoCaptureSufaceBufferListenerOHOSTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};

void VideoCaptureSufaceBufferListenerOHOSTest::SetUpTestCase(void) {}

void VideoCaptureSufaceBufferListenerOHOSTest::TearDownTestCase(void) {}

void VideoCaptureSufaceBufferListenerOHOSTest::SetUp(void) {
  auto task_runner = base::MakeRefCounted<SingleThreadTaskRunnerMock>();
  VideoCaptureDeviceDescriptor device_descriptor;
  VideoCaptureParams capture_params;
  auto media_player = std::make_shared<OHOSCaptureDelegate>(
      device_descriptor, task_runner, capture_params);
  auto weak_media_player = media_player->GetWeakPtr();
  v_listener = std::make_shared<VideoCaptureSufaceBufferListenerOHOS>(
      task_runner, weak_media_player);
}

void VideoCaptureSufaceBufferListenerOHOSTest::TearDown(void) {
  v_listener = nullptr;
}

TEST_F(VideoCaptureSufaceBufferListenerOHOSTest, OnBufferAvailable_001) {
  auto surface = std::shared_ptr<CameraSurfaceAdapter>();
  auto buffer = std::shared_ptr<OHOS::NWeb::CameraSurfaceBufferAdapter>();
  auto roration_info = std::shared_ptr<OHOS::NWeb::CameraRotationInfoAdapter>();
  v_listener->OnBufferAvailable(surface, buffer, roration_info);
}
}  // namespace media