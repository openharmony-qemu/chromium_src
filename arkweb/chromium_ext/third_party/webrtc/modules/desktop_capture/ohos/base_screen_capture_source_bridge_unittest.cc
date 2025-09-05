/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "base/memory/weak_ptr.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "base_window_capturer.h"
#include "base_screen_capture_source_bridge.h"
#include "base/task/single_thread_task_runner.h"
#undef private
#include "base/logging.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/task_runner.h"
#include "ohos_adapter_helper.h"
#include "audio_capturer_adapter.h"
#include "base/memory/scoped_refptr.h"
#include "base/run_loop.h"
#include "base/task/sequenced_task_runner.h"
#include "base/test/gtest_util.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "media/audio/ohos/ohos_audio_capturer_source.h"

using namespace media;
using namespace base;
using namespace testing;

namespace webrtc {
class ScreenCaptureAdapterMock : public OHOS::NWeb::ScreenCaptureAdapter {
 public:
  MOCK_METHOD(int32_t, Init, (const std::shared_ptr<ScreenCaptureConfigAdapter> config), (override));
  MOCK_METHOD(int32_t, SetMicrophoneEnable, (bool enable), (override));
  MOCK_METHOD(int32_t, StartCapture, (), (override));
  MOCK_METHOD(int32_t, StopCapture, (), (override));
  MOCK_METHOD(int32_t, SetCaptureCallback,
      (const std::shared_ptr<ScreenCaptureCallbackAdapter> callback), (override));
  MOCK_METHOD(std::shared_ptr<SurfaceBufferAdapter>, AcquireVideoBuffer, (), (override));
  MOCK_METHOD(int32_t, ReleaseVideoBuffer, (), (override));
  MOCK_METHOD(int32_t, AcquireAudioBuffer,
      (std::shared_ptr<AudioBufferAdapter> audiobuffer, AudioCaptureSourceTypeAdapter type), (override));
  MOCK_METHOD(int32_t, ReleaseAudioBuffer, (AudioCaptureSourceTypeAdapter type), (override));
};

class MockCallback : public BaseScreenCaptureReadCallback {
 public:
    MOCK_METHOD(void, OnReadData, (), (override));
    MOCK_METHOD(void, OnReadData, (OHOS::NWeb::AudioCaptureSourceTypeAdapter type), (override));
};

class BaseScreenCaptureSourceTest : public ::testing::Test {
 public:
  void SetUp() override {
    base_screen_capture = std::make_unique<BaseScreenCaptureSource>();
  }
  
  void TearDown() override {
    base_screen_capture.reset();
  }

 protected:
  std::unique_ptr<BaseScreenCaptureSource> base_screen_capture;
};

TEST_F(BaseScreenCaptureSourceTest, RegisterAudioCaptureCallback01) {
  std::shared_ptr<MockCallback> callback =
      std::make_shared<MockCallback>();
  int nweb_id = 1;

  int32_t res = base_screen_capture->RegisterAudioCaptureCallback(callback, nweb_id);
  EXPECT_EQ(res, 0);
}

TEST_F(BaseScreenCaptureSourceTest, RegisterAudioCaptureCallback02) {
  std::shared_ptr<MockCallback> callback =
      nullptr;
  int nweb_id = 1;

  int32_t res = base_screen_capture->RegisterAudioCaptureCallback(callback, nweb_id);
  EXPECT_EQ(res, -1);
}

TEST_F(BaseScreenCaptureSourceTest, RegisterWindowCaptureCallback01) {
  std::shared_ptr<MockCallback> callback =
      std::make_shared<MockCallback>();
  int nweb_id = 1;

  int32_t res = base_screen_capture->RegisterWindowCaptureCallback(callback, nweb_id);
  EXPECT_EQ(res, 0);
}

TEST_F(BaseScreenCaptureSourceTest, RegisterWindowCaptureCallback02) {
  std::shared_ptr<MockCallback> callback =
      nullptr;
  int nweb_id = 1;

  int32_t res = base_screen_capture->RegisterWindowCaptureCallback(callback, nweb_id);
  EXPECT_EQ(res, -1);
}

TEST_F(BaseScreenCaptureSourceTest, SetScreenCaptureConfig01) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>().get());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1.get(),
      SingleThreadTaskRunner::CurrentDefaultHandle::MayAlreadyExist{});
  std::shared_ptr<MockCallback> callback =
      nullptr;
  int nweb_id = 1;

  bool res = base_screen_capture->SetScreenCaptureConfig(nweb_id);
  EXPECT_TRUE(res);
}

TEST_F(BaseScreenCaptureSourceTest, SetScreenCaptureConfig02) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>().get());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1.get(),
      SingleThreadTaskRunner::CurrentDefaultHandle::MayAlreadyExist{});
  std::shared_ptr<MockCallback> callback =
      nullptr;
  int nweb_id = 1;
  base_screen_capture->is_picker_show_ = true;

  bool res = base_screen_capture->SetScreenCaptureConfig(nweb_id);
  EXPECT_TRUE(res);
}

TEST_F(BaseScreenCaptureSourceTest, SetScreenCapturePickerShow01) {
  base_screen_capture->is_picker_show_ = true;
  base_screen_capture->SetScreenCapturePickerShow(false);

  EXPECT_FALSE(base_screen_capture->is_picker_show_);
}

TEST_F(BaseScreenCaptureSourceTest, SetScreenCapturePickerShow02) {
  base_screen_capture->is_picker_show_ = false;
  base_screen_capture->SetScreenCapturePickerShow(true);

  EXPECT_TRUE(base_screen_capture->is_picker_show_);
}

TEST_F(BaseScreenCaptureSourceTest, SetScreenCaptureState01) {
  OHOS::NWeb::ScreenCaptureStateCodeAdapter stateCode =
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID;
  int nweb_id = 1;
  base_screen_capture->SetScreenCaptureState(stateCode, nweb_id);

  EXPECT_EQ(base_screen_capture->capture_state_code_map_[nweb_id], stateCode);
}

TEST_F(BaseScreenCaptureSourceTest, ReleaseCapture01) {
  int nweb_id = 1;
  std::unique_ptr<OHOS::NWeb::ScreenCaptureAdapter> screen_capture_adapter = 
    std::make_unique<ScreenCaptureAdapterMock>();
  std::shared_ptr<MockCallback> audio_callback =
    std::make_shared<MockCallback>();
  std::shared_ptr<MockCallback> window_callback =
    std::make_shared<MockCallback>();
  base_screen_capture->screen_capture_adapter_map_[nweb_id] = std::move(screen_capture_adapter);
  base_screen_capture->audio_callback_map_[nweb_id] = audio_callback;
  base_screen_capture->window_callback_map_[nweb_id] = window_callback;

  base_screen_capture->ReleaseCapture(nweb_id);

  auto res_screen_capture_adapter = base_screen_capture->screen_capture_adapter_map_.find(nweb_id);
  auto res_audio_callback = base_screen_capture->audio_callback_map_.find(nweb_id);
  auto res_window_callback = base_screen_capture->window_callback_map_.find(nweb_id);

  EXPECT_EQ(res_screen_capture_adapter, base_screen_capture->screen_capture_adapter_map_.end());
  EXPECT_EQ(res_audio_callback, base_screen_capture->audio_callback_map_.end());
  EXPECT_EQ(res_window_callback, base_screen_capture->window_callback_map_.end());
}

TEST_F(BaseScreenCaptureSourceTest, ReleaseCapture02) {
  int nweb_id = 1;
  std::unique_ptr<OHOS::NWeb::ScreenCaptureAdapter> screen_capture_adapter = 
    std::make_unique<ScreenCaptureAdapterMock>();
  std::shared_ptr<MockCallback> audio_callback =
    std::make_shared<MockCallback>();
  base_screen_capture->screen_capture_adapter_map_[nweb_id] = std::move(screen_capture_adapter);
  base_screen_capture->audio_callback_map_[nweb_id] = audio_callback;

  base_screen_capture->ReleaseCapture(nweb_id);

  auto res_screen_capture_adapter = base_screen_capture->screen_capture_adapter_map_.find(nweb_id);
  auto res_audio_callback = base_screen_capture->audio_callback_map_.find(nweb_id);

  EXPECT_EQ(res_screen_capture_adapter, base_screen_capture->screen_capture_adapter_map_.end());
  EXPECT_EQ(res_audio_callback, base_screen_capture->audio_callback_map_.end());
}

TEST_F(BaseScreenCaptureSourceTest, ReleaseCapture03) {
  int nweb_id = 1;
  std::unique_ptr<OHOS::NWeb::ScreenCaptureAdapter> screen_capture_adapter = 
    std::make_unique<ScreenCaptureAdapterMock>();
  base_screen_capture->screen_capture_adapter_map_[nweb_id] = std::move(screen_capture_adapter);

  base_screen_capture->ReleaseCapture(nweb_id);

  auto res_screen_capture_adapter = base_screen_capture->screen_capture_adapter_map_.find(nweb_id);

  EXPECT_EQ(res_screen_capture_adapter, base_screen_capture->screen_capture_adapter_map_.end());
}

TEST_F(BaseScreenCaptureSourceTest, ReleaseCapture04) {
  int nweb_id = 1;

  base_screen_capture->ReleaseCapture(nweb_id);

  auto res_screen_capture_adapter = base_screen_capture->screen_capture_adapter_map_.find(nweb_id);

  EXPECT_EQ(res_screen_capture_adapter, base_screen_capture->screen_capture_adapter_map_.end());
}

TEST_F(BaseScreenCaptureSourceTest, ScreenCaptureAdapterIsExist01) {
  int nweb_id = 1;
  std::unique_ptr<OHOS::NWeb::ScreenCaptureAdapter> screen_capture_adapter = 
    std::make_unique<ScreenCaptureAdapterMock>();
  base_screen_capture->screen_capture_adapter_map_[nweb_id] = std::move(screen_capture_adapter);

  bool res = base_screen_capture->ScreenCaptureAdapterIsExist(nweb_id);

  EXPECT_TRUE(res);
}

TEST_F(BaseScreenCaptureSourceTest, ScreenCaptureAdapterIsExist02) {
  int nweb_id = 1;
  bool res = base_screen_capture->ScreenCaptureAdapterIsExist(nweb_id);

  EXPECT_FALSE(res);
}

TEST_F(BaseScreenCaptureSourceTest, AudioCallbackIsExist01) {
  int nweb_id = 1;
  std::shared_ptr<MockCallback> audio_callback =
    std::make_shared<MockCallback>();
  base_screen_capture->audio_callback_map_[nweb_id] = audio_callback;

  bool res = base_screen_capture->AudioCallbackIsExist(nweb_id);

  EXPECT_TRUE(res);
}

TEST_F(BaseScreenCaptureSourceTest, AudioCallbackIsExist02) {
  int nweb_id = 1;
  bool res = base_screen_capture->AudioCallbackIsExist(nweb_id);

  EXPECT_FALSE(res);
}

TEST_F(BaseScreenCaptureSourceTest, WindowCallbackIsExist01) {
  int nweb_id = 1;
  std::shared_ptr<MockCallback> window_callback =
    std::make_shared<MockCallback>();
  base_screen_capture->window_callback_map_[nweb_id] = window_callback;
  bool res = base_screen_capture->WindowCallbackIsExist(nweb_id);

  EXPECT_TRUE(res);
}

TEST_F(BaseScreenCaptureSourceTest, WindowCallbackIsExist02) {
  int nweb_id = 1;
  bool res = base_screen_capture->WindowCallbackIsExist(nweb_id);

  EXPECT_FALSE(res);
}

TEST_F(BaseScreenCaptureSourceTest, StartCapture01) {
  int nweb_id = 1;
  std::unique_ptr<OHOS::NWeb::ScreenCaptureAdapter> screen_capture_adapter = 
    std::make_unique<ScreenCaptureAdapterMock>();
  base_screen_capture->screen_capture_adapter_map_[nweb_id] = std::move(screen_capture_adapter);

  int32_t res = base_screen_capture->StartCapture(nweb_id);

  EXPECT_EQ(res, 0);
}

TEST_F(BaseScreenCaptureSourceTest, StartCapture02) {
  int nweb_id = 1;
  int32_t res = base_screen_capture->StartCapture(nweb_id);

  EXPECT_EQ(res, -1);
}

TEST_F(BaseScreenCaptureSourceTest, StopCapture01) {
  int nweb_id = 1;
  std::unique_ptr<OHOS::NWeb::ScreenCaptureAdapter> screen_capture_adapter = 
    std::make_unique<ScreenCaptureAdapterMock>();
  base_screen_capture->screen_capture_adapter_map_[nweb_id] = std::move(screen_capture_adapter);

  int32_t res = base_screen_capture->StopCapture(nweb_id);

  EXPECT_EQ(res, 0);
}

TEST_F(BaseScreenCaptureSourceTest, StopCapture02) {
  int nweb_id = 1;
  int32_t res = base_screen_capture->StopCapture(nweb_id);

  EXPECT_EQ(res, -1);
}
}