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

// 模拟客户端类
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

// 测试构造函数
TEST_F(OHOSAudioDecoderLoopTest, Constructor) {
  CreateDecoderLoop();
  EXPECT_NE(decoder_loop_.get(), nullptr);
}

// 测试空客户端构造函数
TEST_F(OHOSAudioDecoderLoopTest, ConstructorWithNullClient) {
  decoder_loop_ = std::make_unique<OHOSAudioDecoderLoop>(
      nullptr, task_runner_);
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::ERROR);
}

// 测试 TryFlush 在 ERROR 状态
TEST_F(OHOSAudioDecoderLoopTest, TryFlushInErrorState) {
  CreateDecoderLoop();
  // 强制设置 ERROR 状态
  decoder_loop_->SetState(OHOSAudioDecoderLoop::ERROR);
  
  EXPECT_FALSE(decoder_loop_->TryFlush());
}

// 测试 TryFlush 成功
TEST_F(OHOSAudioDecoderLoopTest, TryFlushSuccess) {
  CreateDecoderLoop();
  
  EXPECT_CALL(*mock_client_, FlushDecoder())
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_OK));
  
  EXPECT_TRUE(decoder_loop_->TryFlush());
}

// 测试 TryFlush 失败
TEST_F(OHOSAudioDecoderLoopTest, TryFlushFailure) {
  CreateDecoderLoop();
  
  EXPECT_CALL(*mock_client_, FlushDecoder())
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_ERROR));
  
  EXPECT_FALSE(decoder_loop_->TryFlush());
}

// 测试 OnKeyAdded 在 WAITING_FOR_KEY 状态
TEST_F(OHOSAudioDecoderLoopTest, OnKeyAddedInWaitingState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::WAITING_FOR_KEY);

  EXPECT_CALL(*mock_client_, IsAnyInputPending()).WillOnce(Return(false));
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Return(-1));
  
  decoder_loop_->OnKeyAdded();

  // 应该回到 READY 状态
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::READY);
}

// 测试 OnKeyAdded 在其他状态
TEST_F(OHOSAudioDecoderLoopTest, OnKeyAddedInReadyState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::READY);

  EXPECT_CALL(*mock_client_, IsAnyInputPending()).WillOnce(Return(false));
  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Return(-1));

  // 处于READY状态会直接触发循环
  decoder_loop_->OnKeyAdded();

  // 状态应该保持不变
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::READY);
}

// 测试 ExpectWork 启动定时器
TEST_F(OHOSAudioDecoderLoopTest, ExpectWorkStartsTimer) {
  CreateDecoderLoop();
  
  EXPECT_CALL(*mock_client_, IsAnyInputPending()).WillOnce(Return(false));

  decoder_loop_->ExpectWork();
  // 应该启动定时器
  ASSERT_TRUE(decoder_loop_->io_timer_.IsRunning());
}

// 测试 DoPendingWork 在 ERROR 状态
TEST_F(OHOSAudioDecoderLoopTest, DoPendingWorkInErrorState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::ERROR);

  // 不应该处理任何工作
  decoder_loop_->DoPendingWork();
}

// 测试 ProcessOneInputBuffer 在非 READY 状态
TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferInNonReadyState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::WAITING_FOR_KEY);

  EXPECT_FALSE(decoder_loop_->ProcessOneInputBuffer());
}

// 测试 ProcessOneInputBuffer 无输入数据
TEST_F(OHOSAudioDecoderLoopTest, ProcessOneInputBufferNoInput) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillOnce(Return(false));

  EXPECT_FALSE(decoder_loop_->ProcessOneInputBuffer());
}

// 测试 ProcessOneInputBuffer 获取输入缓冲区失败
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

// 测试 ProcessOneInputBuffer 成功入队
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

// 测试 ProcessOneInputBuffer EOS 处理
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
  // 应该进入 DRAINING 状态
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::DRAINING);

  delete[] input_data.memory;
}

// 测试 ProcessOneInputBuffer 需要重试（等待密钥）
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
  // 应该进入 WAITING_FOR_KEY 状态
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::WAITING_FOR_KEY);

  delete[] input_data.memory;
}

// 测试 ProcessOneInputBuffer 入队错误
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
  // 应该进入 ERROR 状态
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::ERROR);
  delete[] input_data.memory;
}

// 测试 ProcessOneOutputBuffer 在 ERROR 状态
TEST_F(OHOSAudioDecoderLoopTest, ProcessOneOutputBufferInErrorState) {
  CreateDecoderLoop();
  decoder_loop_->SetState(OHOSAudioDecoderLoop::ERROR);

  EXPECT_FALSE(decoder_loop_->ProcessOneOutputBuffer());
}

// 测试 ProcessOneOutputBuffer 无输出
TEST_F(OHOSAudioDecoderLoopTest, ProcessOneOutputBufferNoOutput) {
  CreateDecoderLoop();

  EXPECT_CALL(*mock_client_, DequeueOutputBuffer(_))
      .WillOnce(Return(-1));

  EXPECT_FALSE(decoder_loop_->ProcessOneOutputBuffer());
}

// 测试 ProcessOneOutputBuffer EOS 输出
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
  // 应该进入 DRAINED 状态
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::DRAINED);
}

// 测试 ProcessOneOutputBuffer EOS 处理失败
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
  // 应该进入 ERROR 状态
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::ERROR);
}

// 测试 ProcessOneOutputBuffer 正常输出
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

// 测试 ProcessOneOutputBuffer 正常输出处理失败
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
  // 应该进入 ERROR 状态
  EXPECT_EQ(decoder_loop_->state_, OHOSAudioDecoderLoop::State::ERROR);
  
}

// 设置定时器禁用
TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_DisableTimer) {
  CreateDecoderLoop(true);

  decoder_loop_->ManageTimer(true);
  ASSERT_FALSE(decoder_loop_->io_timer_.IsRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_DidWork_StartTimer) {
  CreateDecoderLoop();
  decoder_loop_->idle_time_begin_ = base::TimeTicks();
  
  // 调用ManageTimer，did_work = true
  decoder_loop_->ManageTimer(true);
  
  // 验证idle_time_begin_被更新
  EXPECT_NE(decoder_loop_->idle_time_begin_, base::TimeTicks());
  ASSERT_TRUE(decoder_loop_->io_timer_.IsRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_DidWork_UpdateIdleTime) {
  CreateDecoderLoop();
  // 设置一个旧的空闲时间
  base::TimeTicks old_time = base::TimeTicks::Now() - base::Seconds(30);
  SetIdleTimeBegin(old_time);
  
  decoder_loop_->ManageTimer(true);
  
  // 验证空闲时间被更新为当前时间
  EXPECT_GT(GetIdleTimeBegin(), old_time);
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_NoWork_FirstIdle) {
  CreateDecoderLoop();
  // 初始状态：空闲时间为0，没有工作
  SetIdleTimeBegin(base::TimeTicks());
  
  decoder_loop_->ManageTimer(false);
  
  // 验证空闲时间被设置，定时器应该启动
  EXPECT_NE(GetIdleTimeBegin(), base::TimeTicks());
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_NoWork_NotTimeout) {
  CreateDecoderLoop();
  // 设置空闲时间在500ms前（未超时）
  base::TimeTicks idle_start = base::TimeTicks::Now() - base::Milliseconds(500);
  SetIdleTimeBegin(idle_start);
  
  // 确保定时器正在运行
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());
  
  // 设置空闲时间在500ms前（未超时）
  idle_start = base::TimeTicks::Now() - base::Milliseconds(500);
  SetIdleTimeBegin(idle_start);
  // 没有工作，但未超时
  decoder_loop_->ManageTimer(false);
  
  // 验证定时器继续运行
  EXPECT_TRUE(IsTimerRunning());
  // 空闲时间不应改变
  EXPECT_EQ(GetIdleTimeBegin(), idle_start);
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_NoWork_Timeout) {
  CreateDecoderLoop();
  // 设置空闲时间在2秒前（已超时，假设kIdleTimerTimeout=1s）
  base::TimeTicks idle_start = base::TimeTicks::Now() - base::Seconds(2);
  SetIdleTimeBegin(idle_start);
  
  // 确保定时器正在运行
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());

  idle_start = base::TimeTicks::Now() - base::Seconds(2);
  SetIdleTimeBegin(idle_start);
  // 没有工作，且已超时
  decoder_loop_->ManageTimer(false);
  
  // 验证定时器被停止
  EXPECT_FALSE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_TimerAlreadyRunning_NoChange) {
  CreateDecoderLoop();
  // 启动定时器
  SetIdleTimeBegin(base::TimeTicks());
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());
  
  // 再次调用ManageTimer，有工作，定时器已经在运行
  decoder_loop_->ManageTimer(true);
  
  // 验证定时器继续运行
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_TimerNotRunning_ShouldNotStart) {
  CreateDecoderLoop();
  // 设置超时状态
  base::TimeTicks idle_start = base::TimeTicks::Now() - base::Seconds(2);
  SetIdleTimeBegin(idle_start);
  
  // 没有工作，已超时，定时器不应该启动
  decoder_loop_->ManageTimer(false);
  EXPECT_FALSE(IsTimerRunning());
  
  // 再次调用，仍然不应该启动
  decoder_loop_->ManageTimer(false);
  EXPECT_FALSE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, ManageTimer_TimeoutThenWork) {
  CreateDecoderLoop();
  // 先设置超时状态
  base::TimeTicks idle_start = base::TimeTicks::Now() - base::Seconds(2);
  SetIdleTimeBegin(idle_start);
  decoder_loop_->ManageTimer(false);
  EXPECT_FALSE(IsTimerRunning());
  
  // 然后有工作完成
  decoder_loop_->ManageTimer(true);
  
  // 验证定时器重新启动，空闲时间更新
  EXPECT_TRUE(IsTimerRunning());
  EXPECT_GT(GetIdleTimeBegin(), idle_start);
}

TEST_F(OHOSAudioDecoderLoopTest, TimerCallback_DoPendingWork) {
  CreateDecoderLoop();
  // 启动定时器
  SetIdleTimeBegin(base::TimeTicks());
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());
  
  // 设置DoPendingWork的期望
  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillRepeatedly(Return(false));
  
  // 快进时间触发定时器回调
  FastForwardAndRunTimer(base::Milliseconds(10));
  
  // 验证定时器继续运行（因为回调会再次调用ManageTimer）
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, RealTimerBehavior_WithTimeControl) {
  CreateDecoderLoop();
  // 测试真实定时器行为
  SetIdleTimeBegin(base::TimeTicks());
  decoder_loop_->ManageTimer(true);
  
  // 验证定时器确实在运行
  EXPECT_TRUE(IsTimerRunning());
  
  // 记录当前空闲时间
  base::TimeTicks initial_idle_time = GetIdleTimeBegin();
  
  // 快进时间，但不超过超时时间
  FastForwardAndRunTimer(base::Milliseconds(500));
  
  // 定时器应该继续运行，空闲时间不变（因为没有工作完成）
  EXPECT_TRUE(IsTimerRunning());
  
  // 模拟有工作完成
  decoder_loop_->ManageTimer(true);
  
  // 空闲时间应该更新
  EXPECT_TRUE(IsTimerRunning());
}

TEST_F(OHOSAudioDecoderLoopTest, TimerStopsAfterTimeout) {
  CreateDecoderLoop();
  // 启动定时器
  SetIdleTimeBegin(base::TimeTicks());
  decoder_loop_->ManageTimer(true);
  EXPECT_TRUE(IsTimerRunning());
  
  // 快进时间超过超时时间
  FastForwardAndRunTimer(base::Seconds(2));
  
  // 没有工作完成，定时器应该停止
  decoder_loop_->ManageTimer(false);
  EXPECT_FALSE(IsTimerRunning());
}

// 测试 SetState 到 ERROR 状态触发回调
TEST_F(OHOSAudioDecoderLoopTest, SetStateToErrorTriggersCallback) {
  CreateDecoderLoop();
  
  EXPECT_CALL(*mock_client_, OnCodecLoopError());
  
  decoder_loop_->SetState(OHOSAudioDecoderLoop::ERROR);
}

// 测试 SetState 到其他状态不触发回调
TEST_F(OHOSAudioDecoderLoopTest, SetStateToOtherNoCallback) {
  CreateDecoderLoop();
  EXPECT_CALL(*mock_client_, OnCodecLoopError()).Times(0);
  // OnCodecLoopError 不应该被调用
  decoder_loop_->SetState(OHOSAudioDecoderLoop::READY);
}

// 测试完整工作流程
TEST_F(OHOSAudioDecoderLoopTest, CompleteWorkflow) {
  CreateDecoderLoop();
  
  // 设置输入数据可用
  EXPECT_CALL(*mock_client_, IsAnyInputPending())
      .WillRepeatedly(Return(true));
  
  // 提供输入缓冲区 - 第一次调用返回index=1，第二次调用返回index=-1
  EXPECT_CALL(*mock_client_, DequeueInputBuffer(_))
      .WillOnce(Invoke([](int64_t& index) {
        index = 1;
        return 0;
      }))
      .WillOnce(Invoke([](int64_t& index) {
        index = OHOSAudioDecoderLoop::kInvalidBufferIndex;
        return 0;
      }));
  
  // 提供输入数据
  OHOSAudioDecoderLoop::InputData input_data;
  input_data.is_valid = true;
  input_data.memory = new uint8_t[10];
  input_data.length = 10;
  
  EXPECT_CALL(*mock_client_, ProvideInputData())
      .WillOnce(Return(input_data));
  
  // 成功入队
  EXPECT_CALL(*mock_client_, QueueInputBufferDec(_, _, _, _, _, _, _))
      .WillOnce(Return(AudioDecoderAdapterCode::DECODER_OK));
  
  EXPECT_CALL(*mock_client_, OnInputDataQueued(true));
  
  // 提供输出数据
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
  
  // 执行工作
  decoder_loop_->DoPendingWork();
  
  delete[] input_data.memory;
}

}  // namespace
}  // namespace media