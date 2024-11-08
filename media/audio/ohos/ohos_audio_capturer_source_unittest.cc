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
 
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include "media/base/audio_parameters.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "media/audio/ohos/ohos_audio_capturer_source.h"
#undef private

#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "base/threading/thread.h"
#include "media/audio/ohos/buffer_desc_adapter_impl.h"
#include "media/base/audio_capturer_source.h"

using testing::_;
using testing::AtLeast;
using testing::Eq;
using testing::Exactly;
using testing::Invoke;
using testing::Mock;
using testing::MockFunction;
using testing::Return;
using namespace media;

class MockReadDataCallback {
 public:
  MOCK_METHOD(void, Run, (), ());
};

class MockAudioCapturerAdapter : public AudioCapturerAdapter {
 public:
  MOCK_METHOD(
      int32_t,
      Create,
      (const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions,
       std::string cachePath),
      (override));
  MOCK_METHOD(bool, Start, (), (override));
  MOCK_METHOD(bool, Stop, (), (override));
  MOCK_METHOD(bool, Release, (), (override));
  MOCK_METHOD(int32_t,
              SetCapturerReadCallback,
              (std::shared_ptr<AudioCapturerReadCallbackAdapter> callback),
              (override));
  MOCK_METHOD(int32_t,
              GetBufferDesc,
              (std::shared_ptr<BufferDescAdapter> bufferDesc),
              (override));
  MOCK_METHOD(int32_t,
              Enqueue,
              (const std::shared_ptr<BufferDescAdapter> bufferDesc),
              (override));
  MOCK_METHOD(int32_t, GetFrameCount, (uint32_t & frameCount), (override));
  MOCK_METHOD(int64_t, GetAudioTime, (), (override));
};

class MockSingleThreadTaskRunner : public base::SingleThreadTaskRunner {
 public:
  MOCK_CONST_METHOD0(GetCurrentDefault,
                     const scoped_refptr<base::SingleThreadTaskRunner>&());
  MOCK_METHOD(bool, BelongsToCurrentThread, (), (const));
  MOCK_METHOD(bool, HasCurrentDefault, (), ());
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              ());
  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
  MOCK_METHOD(bool,
              PostDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              ());
};

class MockCaptureCallback : public AudioCapturerSource::CaptureCallback {
 public:
  MOCK_METHOD(void, OnCapture, (const uint8_t*, int), ());
  MOCK_METHOD(void,
              OnCaptureError,
              (AudioCapturerSource::ErrorCode code, const std::string& message),
              (override));
  MOCK_METHOD(void, OnCaptureStarted, (), (override));
  MOCK_METHOD(void,
              Capture,
              (const AudioBus* audio_bus,
               base::TimeTicks capture_time,
               double volume,
               bool key_pressed),
              (override));
  MOCK_METHOD(void, OnCaptureMuted, (bool is_muted), (override));
  MOCK_METHOD(void,
              OnCaptureProcessorCreated,
              (AudioProcessorControls * controls),
              (override));
};

class AudioCapturerReadCallbackTest : public ::testing::Test {
 public:
  AudioCapturerReadCallbackTest()
      : mock_callback(),
        read_callback(base::BindRepeating(&MockReadDataCallback::Run,
                                          base::Unretained(&mock_callback))) {}

 protected:
  MockReadDataCallback mock_callback;
  AudioCapturerReadCallback read_callback;
};

class AudioCapturerSourceTest : public ::testing::Test {
 public:
  void SetUp() override {
    capturer_source_ =
        std::make_shared<OHOSAudioCapturerSource>(capturer_task_runner);
  }
  void TearDown() override {}

 protected:
  std::unique_ptr<MockCaptureCallback> capture_callback_;
  scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner;
  std::shared_ptr<OHOSAudioCapturerSource> capturer_source_;
};

TEST_F(AudioCapturerReadCallbackTest, OnReadDataTest) {
  size_t length = 10;
  bool result = read_callback.readDataCallback_.is_null();
  EXPECT_CALL(mock_callback, Run()).Times(1);
  read_callback.OnReadData(length);
  EXPECT_FALSE(result);
}

TEST_F(AudioCapturerReadCallbackTest, OnReadDataNoCallbackTest) {
  size_t length = 10;
  read_callback.readDataCallback_ = base::RepeatingCallback<void()>();
  bool result = read_callback.readDataCallback_.is_null();
  EXPECT_CALL(mock_callback, Run()).Times(0);
  read_callback = AudioCapturerReadCallback(base::RepeatingCallback<void()>());
  read_callback.OnReadData(length);
  EXPECT_TRUE(result);
}

TEST_F(AudioCapturerSourceTest, DestructorOHOSAudioCapturerSource) {
  {
    OHOSAudioCapturerSource* destruct_capturer_source_ =
        new OHOSAudioCapturerSource(capturer_task_runner);
    EXPECT_TRUE(destruct_capturer_source_);
    delete destruct_capturer_source_;
  }
}

TEST_F(AudioCapturerSourceTest, ReadData_Test001) {
  capturer_source_->frameCount_ = 100;
  capturer_source_->params_.set_sample_rate(0);

  capturer_source_->ReadData();
  EXPECT_GT(static_cast<int>(capturer_source_->frameCount_),
            capturer_source_->params_.sample_rate() / 100);
}

TEST_F(AudioCapturerSourceTest, NotifyCaptureErrorFalse) {
  std::string message = "Test NotifyCaptureError Function";
  capturer_source_->NotifyCaptureError(message);
  EXPECT_FALSE(capturer_source_->callback_);
}

TEST_F(AudioCapturerSourceTest, NotifyCaptureErrorTrue) {
  auto capturer_call_back_ = std::make_unique<MockCaptureCallback>();
  std::string message = "Test NotifyCaptureError Function";
  capturer_source_->callback_ = new MockCaptureCallback();

  EXPECT_CALL(*capturer_call_back_,
              OnCaptureError(AudioCapturerSource::ErrorCode::kUnknown, message))
      .Times(1);
  capturer_source_->callback_ = capturer_call_back_.get();
  capturer_source_->NotifyCaptureError(message);
  EXPECT_TRUE(capturer_source_->callback_);
}

TEST_F(AudioCapturerSourceTest, NotifyCaptureStartedFalse) {
  auto capturer_call_back_ = std::make_unique<MockCaptureCallback>();
  capturer_source_->callback_ = new MockCaptureCallback();

  EXPECT_CALL(*capturer_call_back_, OnCaptureStarted()).Times(1);
  capturer_source_->callback_ = capturer_call_back_.get();

  capturer_source_->NotifyCaptureStarted();
  EXPECT_TRUE(capturer_source_->callback_);
}

TEST_F(AudioCapturerSourceTest, NotifyCaptureStartedTrue) {
  capturer_source_->NotifyCaptureStarted();
  EXPECT_FALSE(capturer_source_->callback_);
}

TEST_F(AudioCapturerSourceTest, Stop_CallbackNotNull) {
  auto mock_callback = std::make_unique<MockCaptureCallback>();
  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  EXPECT_CALL(*mock_capturer, Stop()).WillOnce(Return(true));
  capturer_source_->callback_ = mock_callback.get();
  capturer_source_->capturer_ = std::move(mock_capturer);
  EXPECT_NE(nullptr, capturer_source_->callback_);
  capturer_source_->Stop();
  EXPECT_EQ(nullptr, capturer_source_->callback_);
  mock_callback.reset();
}

TEST_F(AudioCapturerSourceTest, Stop_Pase) {
  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  EXPECT_CALL(*mock_capturer, Stop()).WillOnce(Return(true));
  capturer_source_->capturer_ = std::move(mock_capturer);
  capturer_source_->Stop();
  EXPECT_TRUE(capturer_source_->capturer_);
}

TEST_F(AudioCapturerSourceTest, Stop_FalseStop) {
  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  auto mock_callback = std::make_unique<MockCaptureCallback>();
  EXPECT_CALL(*mock_capturer, Stop()).WillOnce(Return(false));
  capturer_source_->callback_ = mock_callback.get();
  capturer_source_->capturer_ = std::move(mock_capturer);
  EXPECT_FALSE(capturer_source_->capturer_->Stop());
}