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

#include "base/files/file_util.h"
#define private public
#include "media/audio/ohos/ohos_audio_capturer_source.h"
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
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace media;
using namespace testing;

namespace {
constexpr int kDefaultSampleRate = 48000;
constexpr int kDefaultChannelCount = 2;
constexpr int kMinimumInputBufferSize = 2048;
}

class MockCaptureCallback : public media::AudioCapturerSource::CaptureCallback {
public:
    MockCaptureCallback() = default;
    virtual ~MockCaptureCallback() = default;

    // Mock 所有虚函数
    MOCK_METHOD(void, OnCaptureStarted, (), (override));

    MOCK_METHOD(void, Capture, (
        const media::AudioBus* audio_source,
        base::TimeTicks audio_capture_time,
        const media::AudioGlitchInfo& glitch_info,
        double volume,
        bool key_pressed), (override));

    MOCK_METHOD(void, OnCaptureError, (
        media::AudioCapturerSource::ErrorCode code,
        const std::string& message), (override));

    MOCK_METHOD(void, OnCaptureMuted, (bool is_muted), (override));

    MOCK_METHOD(void, OnCaptureProcessorCreated, (
        media::AudioProcessorControls* controls), (override));
};

class MockAudioCapturerAdapter : public OHOS::NWeb::AudioCapturerAdapter {
public:
    MockAudioCapturerAdapter() = default;
    virtual ~MockAudioCapturerAdapter() = default;

    // Mock 所有纯虚函数
    MOCK_METHOD(int32_t, Create, (
        const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions, 
        std::string cachePath), (override));

    MOCK_METHOD(bool, Start, (), (override));
    MOCK_METHOD(bool, Stop, (), (override));
    MOCK_METHOD(bool, Release, (), (override));

    MOCK_METHOD(int32_t, SetCapturerReadCallback, (
        std::shared_ptr<AudioCapturerReadCallbackAdapter> callback), (override));

    MOCK_METHOD(int32_t, GetBufferDesc, (
        std::shared_ptr<BufferDescAdapter> bufferDesc), (override));

    MOCK_METHOD(int32_t, Enqueue, (
        const std::shared_ptr<BufferDescAdapter> bufferDesc), (override));

    MOCK_METHOD(int32_t, GetFrameCount, (uint32_t& frameCount), (override));

    MOCK_METHOD(int64_t, GetAudioTime, (), (override));
};

class MockReadDataCallback {
 public:
  MOCK_METHOD(void, Run, (), ());
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

namespace base {

class OHOSAudioCapturerSourceTest : public ::testing::Test {
 public:
  void SetUp() override {
    audio_capturer_source_ = new OHOSAudioCapturerSource(capturer_task_runner_);
    mock_callback_ = std::make_unique<MockCaptureCallback>();
  }
  
  void TearDown() override {
    delete audio_capturer_source_;
    audio_capturer_source_ = nullptr;
  }

 protected:
  scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner_;
  OHOSAudioCapturerSource* audio_capturer_source_;
  std::unique_ptr<MockCaptureCallback> mock_callback_;
};

TEST_F(OHOSAudioCapturerSourceTest, Initialize01) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);

  EXPECT_CALL(*mock_capturer_ptr, Create(testing::_, testing::_))
      .Times(1);

  auto params = AudioParameters(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount),
      kDefaultSampleRate,
      kMinimumInputBufferSize
  );

  audio_capturer_source_->Initialize(params, mock_callback_.get());
}

TEST_F(OHOSAudioCapturerSourceTest, Initialize02) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);

  EXPECT_CALL(*mock_capturer_ptr, Create(testing::_, testing::_))
      .Times(0);

  auto params = AudioParameters(
      AudioParameters::AUDIO_PCM_LINEAR,
      ChannelLayoutConfig::Guess(kDefaultChannelCount),
      kDefaultSampleRate,
      kMinimumInputBufferSize
  );

  audio_capturer_source_->Initialize(params, mock_callback_.get());
}

TEST_F(OHOSAudioCapturerSourceTest, Start01) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();

  EXPECT_CALL(*mock_capturer_ptr, Start())
      .Times(1).WillOnce(Return(true));
  audio_capturer_source_->Start();
}

TEST_F(OHOSAudioCapturerSourceTest, Start02) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();

  EXPECT_CALL(*mock_capturer_ptr, Start())
      .Times(1).WillOnce(Return(false));
  audio_capturer_source_->Start();
}

TEST_F(OHOSAudioCapturerSourceTest, Stop01) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  audio_capturer_source_->callback_ = mock_callback_.get();

  EXPECT_CALL(*mock_capturer_ptr, Stop())
      .Times(1).WillOnce(Return(false));
  audio_capturer_source_->Stop();
  EXPECT_EQ(audio_capturer_source_->callback_, nullptr);
}

TEST_F(OHOSAudioCapturerSourceTest, Stop02) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  audio_capturer_source_->callback_ = mock_callback_.get();

  EXPECT_CALL(*mock_capturer_ptr, Stop())
      .Times(1).WillOnce(Return(true));
  audio_capturer_source_->Stop();
  EXPECT_EQ(audio_capturer_source_->callback_, nullptr);
}

TEST_F(OHOSAudioCapturerSourceTest, Stop03) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  audio_capturer_source_->callback_ = nullptr;

  EXPECT_CALL(*mock_capturer_ptr, Stop())
      .Times(1).WillOnce(Return(false));
  audio_capturer_source_->Stop();
  EXPECT_EQ(audio_capturer_source_->callback_, nullptr);
}

TEST_F(OHOSAudioCapturerSourceTest, Stop04) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  audio_capturer_source_->callback_ = nullptr;

  EXPECT_CALL(*mock_capturer_ptr, Stop())
      .Times(1).WillOnce(Return(true));
  audio_capturer_source_->Stop();
  EXPECT_EQ(audio_capturer_source_->callback_, nullptr);
}

TEST_F(OHOSAudioCapturerSourceTest, ReadData01) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = nullptr;
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  audio_capturer_source_->callback_ = mock_callback_.get();
  audio_capturer_source_->is_stopped_ = false;
  audio_capturer_source_->params_ = AudioParameters(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount),
      kDefaultSampleRate,
      kMinimumInputBufferSize
  );
  audio_capturer_source_->frameCount_ =  2 * kDefaultSampleRate / 100;

  EXPECT_CALL(*mock_capturer_ptr, GetBufferDesc(testing::_))
      .Times(0);
  EXPECT_CALL(*mock_capturer_ptr, Enqueue(testing::_))
      .Times(0);
  audio_capturer_source_->ReadData();
}

TEST_F(OHOSAudioCapturerSourceTest, ReadData02) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  audio_capturer_source_->callback_ = mock_callback_.get();
  audio_capturer_source_->is_stopped_ = true;
  audio_capturer_source_->params_ = AudioParameters(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount),
      kDefaultSampleRate,
      kMinimumInputBufferSize
  );
  audio_capturer_source_->frameCount_ =  2 * kDefaultSampleRate / 100;

  EXPECT_CALL(*mock_capturer_ptr, GetBufferDesc(testing::_))
      .Times(0);
  EXPECT_CALL(*mock_capturer_ptr, Enqueue(testing::_))
      .Times(0);
  audio_capturer_source_->ReadData();
}

TEST_F(OHOSAudioCapturerSourceTest, ReadData03) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  EXPECT_EQ(task_runner_1, SingleThreadTaskRunner::GetCurrentDefault());

  auto mock_capturer = std::make_unique<MockAudioCapturerAdapter>();
  MockCaptureCallback* mock_callback_ptr = mock_callback_.get();
  MockAudioCapturerAdapter* mock_capturer_ptr = mock_capturer.get();
  audio_capturer_source_->capturer_ = std::move(mock_capturer);
  audio_capturer_source_->main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  audio_capturer_source_->callback_ = mock_callback_ptr;
  audio_capturer_source_->is_stopped_ = false;
  audio_capturer_source_->params_ = AudioParameters(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount),
      kDefaultSampleRate,
      kMinimumInputBufferSize
  );
  audio_capturer_source_->frameCount_ =  2 * kDefaultSampleRate / 100 + 1;

  EXPECT_CALL(*mock_capturer_ptr, GetBufferDesc(testing::_))
      .Times(1);
  EXPECT_CALL(*mock_capturer_ptr, Enqueue(testing::_))
      .Times(1);
  EXPECT_CALL(*mock_callback_ptr, Capture(testing::_, testing::_, testing::_, testing::_, testing::_))
      .Times(0);
  audio_capturer_source_->ReadData();
}

TEST_F(OHOSAudioCapturerSourceTest, NotifyCaptureError01) {
  MockCaptureCallback* mock_callback_ptr = mock_callback_.get();
  audio_capturer_source_->callback_ = nullptr;

  EXPECT_CALL(*mock_callback_ptr, OnCaptureError(testing::_, testing::_))
      .Times(0);
  const std::string& message = "test";
  audio_capturer_source_->NotifyCaptureError(message);
}

TEST_F(OHOSAudioCapturerSourceTest, NotifyCaptureError02) {
  MockCaptureCallback* mock_callback_ptr = mock_callback_.get();
  audio_capturer_source_->callback_ = mock_callback_ptr;

  EXPECT_CALL(*mock_callback_ptr, OnCaptureError(testing::_, testing::_))
      .Times(1);
  const std::string& message = "test";
  audio_capturer_source_->NotifyCaptureError(message);
}

TEST_F(OHOSAudioCapturerSourceTest, NotifyCaptureStarted01) {
  MockCaptureCallback* mock_callback_ptr = mock_callback_.get();
  audio_capturer_source_->callback_ = nullptr;

  EXPECT_CALL(*mock_callback_ptr, OnCaptureStarted())
      .Times(0);
  audio_capturer_source_->NotifyCaptureStarted();
}

TEST_F(OHOSAudioCapturerSourceTest, NotifyCaptureStarted02) {
  MockCaptureCallback* mock_callback_ptr = mock_callback_.get();
  audio_capturer_source_->callback_ = mock_callback_ptr;

  EXPECT_CALL(*mock_callback_ptr, OnCaptureStarted())
      .Times(1);
  audio_capturer_source_->NotifyCaptureStarted();
}
} // namespace base