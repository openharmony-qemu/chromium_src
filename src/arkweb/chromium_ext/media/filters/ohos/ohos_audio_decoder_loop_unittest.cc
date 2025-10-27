// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/test/task_environment.h"
#include "base/test/mock_callback.h"
#include "base/test/test_mock_time_task_runner.h"
#include "base/test/test_simple_task_runner.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#define private public
#define protected public
#include "arkweb/chromium_ext/media/filters/ohos/ohos_audio_decoder_loop.h"
#undef protected
#undef private

using namespace testing;

namespace media {
namespace {

class MockClient : public OHOSAudioDecoderLoop::Client {
 public:
  MOCK_METHOD(bool, IsAnyInputPending, (), (const, override));
  MOCK_METHOD(OHOSAudioDecoderLoop::InputData, ProvideInputData, (), (override));
  MOCK_METHOD(void, OnInputDataQueued, (bool success), (override));
  MOCK_METHOD(bool, OnDecodedEos, (const OutputBufferData& out), (override));
  MOCK_METHOD(bool, OnDecodedFrame, (const OutputBufferData& out), (override));
  MOCK_METHOD(void, OnCodecLoopError, (), (override));
  MOCK_METHOD(int32_t, DequeueInputBuffer, (int64_t& buffer_index), (override));
  MOCK_METHOD(void, EnqueueInputBuffer, (int64_t buffer_index), (override));
  MOCK_METHOD(int32_t, DequeueOutputBuffer, (OutputBufferData& out), (override));
  MOCK_METHOD(AudioDecoderAdapterCode, FlushDecoder, (), (override));
  MOCK_METHOD(AudioDecoderAdapterCode, QueueInputBufferDec, 
              (uint32_t index, int64_t presentationTimeUs, uint8_t* bufferData,
               int32_t bufferSize, std::shared_ptr<AudioCencInfoAdapter> cencInfo, 
               bool isEncrypted, BufferFlag flag), (override));
  MOCK_METHOD(AudioDecoderAdapterCode, ReleaseOutputBufferDec, (uint32_t index), (override));
};

class OHOSAudioDecoderLoopTest : public testing::Test {
 public:
  OHOSAudioDecoderLoopTest()
      : task_environment_(base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

 protected:
  void SetUp() override {
    mock_client_ = std::make_unique<MockClient>();
    task_runner_ = task_environment_.GetMainThreadTaskRunner();
  }

  void TearDown() override {
    mock_client_.reset();
    
  }

  void FastForwardAndRunTimer(base::TimeDelta delay) {
    task_environment_.FastForwardBy(delay);
    task_environment_.RunUntilIdle();
  }

  void SetIdleTimeBegin(base::TimeTicks time) {
    EXPECT_NE(decoder_loop_, nullptr);
    decoder_loop_->idle_time_begin_ = time;
  }
  
  base::TimeTicks GetIdleTimeBegin() const {
    return decoder_loop_->idle_time_begin_;
  }

  bool IsTimerRunning() const {
    return decoder_loop_->io_timer_.IsRunning();
  }

  void CreateDecoderLoop(bool disable_timer = false) {
    decoder_loop_.reset();
    decoder_loop_ = std::make_unique<OHOSAudioDecoderLoop>(
        mock_client_.get(), task_runner_, disable_timer);
  }

  base::test::SingleThreadTaskEnvironment task_environment_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  std::unique_ptr<MockClient> mock_client_;
  std::unique_ptr<OHOSAudioDecoderLoop> decoder_loop_;
};

TEST_F(OHOSAudioDecoderLoopTest, Constructor) {
  CreateDecoderLoop();
  EXPECT_NE(decoder_loop_.get(), nullptr);
}

TEST_F(OHOSAudioDecoderLoopTest, ConstructorWithNullClient) {
  decoder_loop_ = std::make_unique<OHOSAudioDecoderLoop>(
      nullptr, task_runner_);
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::ERROR);
}

TEST_F(OHOSAudioDecoderLoopTest, TryFlushInErrorState) {
  CreateDecoderLoop();

  decoder_loop_->SetState(OHOSAudioDecoderLoop::ERROR);
  
  EXPECT_FALSE(decoder_loop_->TryFlush());
}

TEST_F(OHOSAudioDecoderLoopTest, TryFlushSuccess) {
  CreateDecoderLoop();
  
  EXPECT_CALL(*mock_client_, FlushDecoder())
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_OK));
  
  EXPECT_TRUE(decoder_loop_->TryFlush());
}

TEST_F(OHOSAudioDecoderLoopTest, TryFlushFailure) {
  CreateDecoderLoop();
  
  EXPECT_CALL(*mock_client_, FlushDecoder())
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_ERROR));
  
  EXPECT_FALSE(decoder_loop_->TryFlush());
}

TEST_F(OHOSAudioDecoderLoopTest, OnKeyAddedInWaitingState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::WAITING_FOR_KEY);

  EXPECT_CALL(*mock_client_, IsAnyInputPending()).WillOnce(Return(false));
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Return(-1));
  
  decoder_loop_->OnKeyAdded();

  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::READY);
}

TEST_F(OHOSAudioDecoderLoopTest, OnKeyAddedInReadyState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::READY);

  EXPECT_CALL(*mock_client_, IsAnyInputPending()).WillOnce(Return(false));
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Return(-1));

  decoder_loop_->OnKeyAdded();

  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::READY);
}

TEST_F(OHOSAudioDecoderLoopTest, ExpectWorkStartsTimer) {
  CreateDecoderLoop();
  EXPECT_CALL(*mock_client_, IsAnyInputPending()).WillOnce(Return(false));
  decoder_loop_->ExpectWork();

  ASSERT_TRUE(decoder_loop_->io_timer_.IsRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferInNonReadyState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::WAITING_FOR_KEY);

  EXPECT_FALSE(decoder_loop_->ProcessOneInputBuffer());
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferNoInput) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillOnce(Return(false));

  EXPECT_FALSE(decoder_loop_->ProcessOneInputBuffer());
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferDequeueFail) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillOnce(Return(true));

  EXPECT_CALL(*mock_client_, DequeueInputBuffer(_))
      .WillOnce(Invoke([](int64_t& index) {
        index = OHOSAudioDecoderLoop::kInvalidBufferIndex;
        return -1;
      }));

  EXPECT_FALSE(decoder_loop_->ProcessOneInputBuffer());
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferSuccess) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillOnce(Return(true));

  EXPECT_CALL(*mock_client_, DequeueInputBuffer(_))
      .WillOnce(Invoke([](int64_t& index) {
        index = 1;
        return 0;
      }));
  
  OHOSAudioDecoderLoop::InputData input_data;
  input_data.is_valid = true;
  input_data.memory = new uint8_t[10];
  input_data.length = 10;

  EXPECT_CALL(*mock_client_, ProvideInputData())
      .WillOnce(Return(input_data));

  EXPECT_CALL(*mock_client_, QueueInputBufferDec(_, _, _, _, _, _, _))
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_OK));

  EXPECT_CALL(*mock_client_, OnInputDataQueued(true));

  EXPECT_TRUE(decoder_loop_->ProcessOneInputBuffer());

  delete[] input_data.memory;
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferEos) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillOnce(Return(true));

  EXPECT_CALL(*mock_client_, DequeueInputBuffer(_))
      .WillOnce(Invoke([](int64_t& index) {
        index = 1;
        return 0;
      }));

  OHOSAudioDecoderLoop::InputData input_data;
  input_data.is_valid = true;
  input_data.is_eos = true;
  input_data.memory = new uint8_t[10];
  input_data.length = 10;

  EXPECT_CALL(*mock_client_, ProvideInputData())
      .WillOnce(Return(input_data));

  EXPECT_CALL(*mock_client_, QueueInputBufferDec(_, _, _, _, _, _, _))
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_OK));

  EXPECT_CALL(*mock_client_, OnInputDataQueued(true));

  EXPECT_TRUE(decoder_loop_->ProcessOneInputBuffer());
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::DRAINING);

  delete[] input_data.memory;
}

// test ProcessOneInputBuffer function which need waiting for key
TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferRetry) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillOnce(Return(true));

  EXPECT_CALL(*mock_client_, DequeueInputBuffer(_))
      .WillOnce(Invoke([](int64_t& index) {
        index = 1;
        return 0;
      }));

  OHOSAudioDecoderLoop::InputData input_data;
  input_data.is_valid = true;
  input_data.memory = new uint8_t[10];
  input_data.length = 10;

  EXPECT_CALL(*mock_client_, ProvideInputData())
      .WillOnce(Return(input_data));

  EXPECT_CALL(*mock_client_, QueueInputBufferDec(_, _, _, _, _, _, _))
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_RETRY));

  EXPECT_TRUE(decoder_loop_->ProcessOneInputBuffer());
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::WAITING_FOR_KEY);

  delete[] input_data.memory;
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferError) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillOnce(Return(true));

  EXPECT_CALL(*mock_client_, DequeueInputBuffer(_))
      .WillOnce(Invoke([](int64_t& index) {
        index = 1;
        return 0;
      }));

  OHOSAudioDecoderLoop::InputData input_data;
  input_data.is_valid = true;
  input_data.memory = new uint8_t[10];
  input_data.length = 10;

  EXPECT_CALL(*mock_client_, ProvideInputData())
      .WillOnce(Return(input_data));

  EXPECT_CALL(*mock_client_, QueueInputBufferDec(_, _, _, _, _, _, _))
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_ERROR));

  EXPECT_CALL(*mock_client_, OnInputDataQueued(false));
  EXPECT_FALSE(decoder_loop_->ProcessOneInputBuffer());

  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::ERROR);
  delete[] input_data.memory;
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneOutputBufferInErrorState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::ERROR);

  EXPECT_FALSE(decoder_loop_->ProcessOneOutputBuffer());
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneOutputBufferNoOutput) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Return(-1));

  EXPECT_FALSE(decoder_loop_->ProcessOneOutputBuffer());
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneOutputBufferEos) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::DRAINING);
  
  OutputBufferData eos_output;
  eos_output.flag_ = BufferFlag::CODEC_BUFFER_FLAG_EOS;
  eos_output.index_ = 1;
  
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Invoke([&eos_output](OutputBufferData& out) {
        out = eos_output;
        return 0;
      }));
  
  EXPECT_CALL(*mock_client_, ReleaseOutputBufferDec(1))
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_OK));
  
  EXPECT_CALL(*mock_client_, OnDecodedEos(_))
      .WillOnce(Return(true));
  
  EXPECT_TRUE(decoder_loop_->ProcessOneOutputBuffer());
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::DRAINED);
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneOutputBufferEosFailure) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::DRAINING);
  
  OutputBufferData eos_output;
  eos_output.flag_ = BufferFlag::CODEC_BUFFER_FLAG_EOS;
  eos_output.index_ = 1;
  
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Invoke([&eos_output](OutputBufferData& out) {
        out = eos_output;
        return 0;
      }));
  
  EXPECT_CALL(*mock_client_, ReleaseOutputBufferDec(1))
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_OK));
  
  EXPECT_CALL(*mock_client_, OnDecodedEos(_))
      .WillOnce(Return(false));
  
  EXPECT_TRUE(decoder_loop_->ProcessOneOutputBuffer());
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::ERROR);
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneOutputBufferNormal) {
  CreateDecoderLoop();
  
  OutputBufferData normal_output;
  normal_output.flag_ = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  normal_output.index_ = 1;
  
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Invoke([&normal_output](OutputBufferData& out) {
        out = normal_output;
        return 0;
      }));
  
  EXPECT_CALL(*mock_client_, OnDecodedFrame(_))
      .WillOnce(Return(true));
  
  EXPECT_TRUE(decoder_loop_->ProcessOneOutputBuffer());
}

TEST_F(OHOSAudioDecoderLoopTest, ProcessOneOutputBufferNormalFailure) {
  CreateDecoderLoop();
  
  OutputBufferData normal_output;
  normal_output.flag_ = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  normal_output.index_ = 1;
  
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Invoke([&normal_output](OutputBufferData& out) {
        out = normal_output;
        return 0;
      }));
  
  EXPECT_CALL(*mock_client_, OnDecodedFrame(_))
      .WillOnce(Return(false));
  
  EXPECT_TRUE(decoder_loop_->ProcessOneOutputBuffer());
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::ERROR);
  
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_DisableTimer) {
  CreateDecoderLoop(true);

  decoder_loop_->ManageTimer(true);
  ASSERT_FALSE(decoder_loop_->io_timer_.IsRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_DidWork_StartTimer) {
  CreateDecoderLoop();
  decoder_loop_->idle_time_begin_ = base::TimeTicks();
  
  // did_work = true
  decoder_loop_->ManageTimer(true);
  
  // Verify that idle_time_begin has been updated
  EXPECT_NE(decoder_loop_->idle_time_begin_, base::TimeTicks());
  ASSERT_TRUE(decoder_loop_->io_timer_.IsRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_DidWork_UpdateIdleTime) {
  CreateDecoderLoop();
  // Set an old idle time
  base::TimeTicks old_time = base::TimeTicks::Now() - base::Seconds(30);
  SetIdleTimeBegin(old_time);
  
  decoder_loop_->ManageTimer(true);
  
  // Verify that idle time is updated to the current time
  EXPECT_GT(GetIdleTimeBegin(), old_time);
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_NoWork_FirstIdle) {
  CreateDecoderLoop();
  // Initial state: Idle time is 0, no work
  SetIdleTimeBegin(base::TimeTicks());
  
  decoder_loop_->ManageTimer(false);
  
  // Verify that idle time is set and the timer should start
  EXPECT_NE(GetIdleTimeBegin(), base::TimeTicks());
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_NoWork_NotTimeout) {
  CreateDecoderLoop();
  // Set idle time before 500ms (without timeout)
  base::TimeTicks idle_start = base::TimeTicks::Now() - base::Milliseconds(500);
  SetIdleTimeBegin(idle_start);
  
  // Ensure that the timer is running
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());
  
  idle_start = base::TimeTicks::Now() - base::Milliseconds(500);
  SetIdleTimeBegin(idle_start);
  // No work, but not exceeding the time limit
  decoder_loop_->ManageTimer(false);
  
  // Verify that the timer continues to run
  EXPECT_TRUE(IsTimerRunning());
  // Idle time should not be changed
  EXPECT_EQ(GetIdleTimeBegin(), idle_start);
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_NoWork_Timeout) {
  CreateDecoderLoop();
  // Set the idle time to 2 seconds ago (timed out, assuming kIdleTimerTimeout=1s)
  base::TimeTicks idle_start = base::TimeTicks::Now() - base::Seconds(2);
  SetIdleTimeBegin(idle_start);
  
  // Ensure that the timer is running
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());

  idle_start = base::TimeTicks::Now() - base::Seconds(2);
  SetIdleTimeBegin(idle_start);
  // No work and timeout
  decoder_loop_->ManageTimer(false);
  
  // The verification timer has been stopped
  EXPECT_FALSE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_TimerAlreadyRunning_NoChange) {
  CreateDecoderLoop();
  SetIdleTimeBegin(base::TimeTicks());
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());
  
  // Call ManageTimer again, there is work and the timer is already running
  decoder_loop_->ManageTimer(true);
  
  // Verify that the timer continues to run
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_TimerNotRunning_ShouldNotStart) {
  CreateDecoderLoop();
  // Set timeout status
  base::TimeTicks idle_start = base::TimeTicks::Now() - base::Seconds(2);
  SetIdleTimeBegin(idle_start);
  
  // No work, timed out, timer should not start
  decoder_loop_->ManageTimer(false);
  EXPECT_FALSE(IsTimerRunning());
  
  // Call again, still should not start
  decoder_loop_->ManageTimer(false);
  EXPECT_FALSE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_TimeoutThenWork) {
  CreateDecoderLoop();
  // Set timeout status first
  base::TimeTicks idle_start = base::TimeTicks::Now() - base::Seconds(2);
  SetIdleTimeBegin(idle_start);
  decoder_loop_->ManageTimer(false);
  EXPECT_FALSE(IsTimerRunning());
  
  // Then the work is completed
  decoder_loop_->ManageTimer(true);
  
  // Verify timer restart, idle time update
  EXPECT_TRUE(IsTimerRunning());
  EXPECT_GT(GetIdleTimeBegin(), idle_start);
}

TEST_F(OHOSAudioDecoderLoopTest, TimerCallback_DoPendingWork) {
  CreateDecoderLoop();
  // Start the timer
  SetIdleTimeBegin(base::TimeTicks());
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());
  
  // Set expectation for DoPendingWork
  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillRepeatedly(Return(false));
  
  // Fast forward time to trigger the timer callback
  FastForwardAndRunTimer(base::Milliseconds(10));
  
  // Verify the timer continues running (because the callback will call ManageTimer again)
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, RealTimerBehavior_WithTimeControl) {
  CreateDecoderLoop();
  // Test real timer behavior
  SetIdleTimeBegin(base::TimeTicks());
  decoder_loop_->ManageTimer(true);
  
  // Verify the timer is indeed running
  EXPECT_TRUE(IsTimerRunning());
  
  // Record current idle time
  base::TimeTicks initial_idle_time = GetIdleTimeBegin();
  
  // Fast forward time, but not beyond the timeout threshold
  FastForwardAndRunTimer(base::Milliseconds(500));
  
  // Timer should continue running, idle time remains unchanged (because no work was completed)
  EXPECT_TRUE(IsTimerRunning());
  
  // Simulate work completion
  decoder_loop_->ManageTimer(true);
  
  // Idle time should be updated
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, TimerStopsAfterTimeout) {
  CreateDecoderLoop();
  // Start the timer
  SetIdleTimeBegin(base::TimeTicks());
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());
  
  // Fast forward time beyond the timeout threshold
  FastForwardAndRunTimer(base::Seconds(2));
  
  // No work was completed, the timer should stop
  decoder_loop_->ManageTimer(false);
  EXPECT_FALSE(IsTimerRunning());
}

// Test that setting state to ERROR triggers the callback
TEST_F(OHOSAudioDecoderLoopTest, SetStateToErrorTriggersCallback) {
  CreateDecoderLoop();
  
  EXPECT_CALL(*mock_client_, OnCodecLoopError());
  
  decoder_loop_->SetState(OHOSAudioDecoderLoop::ERROR);
}

// Test that setting state to other states does not trigger the callback
TEST_F(OHOSAudioDecoderLoopTest, SetStateToOtherNoCallback) {
  CreateDecoderLoop();
  EXPECT_CALL(*mock_client_, OnCodecLoopError()).Times(0);
  // OnCodecLoopError should not be called
  decoder_loop_->SetState(OHOSAudioDecoderLoop::READY);
}

// Test the complete workflow
TEST_F(OHOSAudioDecoderLoopTest, CompleteWorkflow) {
  CreateDecoderLoop();
  
  // Set input data as available
  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillRepeatedly(Return(true));
  
  // Provide input buffer - first call returns index=1, second call returns index=-1
  EXPECT_CALL(*mock_client_, DequeueInputBuffer(_))
      .WillOnce(Invoke([](int64_t& index) {
        index = 1;
        return 0;
      }))
      .WillOnce(Invoke([](int64_t& index) {
        index = OHOSAudioDecoderLoop::kInvalidBufferIndex;
        return 0;
      }));
  
  // Provide input data
  OHOSAudioDecoderLoop::InputData input_data;
  input_data.is_valid = true;
  input_data.memory = new uint8_t[10];
  input_data.length = 10;
  
  EXPECT_CALL(*mock_client_, ProvideInputData())
      .WillOnce(Return(input_data));
  
  // Successfully enqueue
  EXPECT_CALL(*mock_client_, QueueInputBufferDec(_, _, _, _, _, _, _))
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_OK));
  
  EXPECT_CALL(*mock_client_, OnInputDataQueued(true));
  
  // Provide output data
  OutputBufferData output_data;
  output_data.flag_ = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  output_data.index_ = 1;
  
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Invoke([&output_data](OutputBufferData& out) {
        out = output_data;
        return 0;
      }))
      .WillOnce(Invoke([&output_data](OutputBufferData& out) {
        out = output_data;
        return -1;
      }));
  
  EXPECT_CALL(*mock_client_, OnDecodedFrame(_))
      .WillOnce(Return(true));
  
  // Execute the work
  decoder_loop_->DoPendingWork();
  
  delete[] input_data.memory;
}

}  // namespace
}  // namespace media