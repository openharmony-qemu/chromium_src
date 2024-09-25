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
#define protected public
#include "media/gpu/ohos/ohos_video_decoder.h"
#undef private
#undef protected
#include <atomic>
#include <cstdint>
#include <memory>
#include <utility>
#include "base/allocator/partition_allocator/pointers/raw_ptr.h"
#include "base/functional/bind.h"
#include "base/functional/callback_forward.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/run_loop.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/task_features.h"
#include "base/task/thread_pool.h"
#include "base/test/task_environment.h"
#include "command_buffer/service/ohos/shared_image_video_ohos.h"
#include "media/base/async_destroy_video_decoder.h"
#include "media/base/decoder_buffer.h"
#include "media/base/decoder_status.h"
#include "media/base/media_log.h"
#include "media/base/mock_filters.h"
#include "media/base/null_video_sink.h"
#include "media/base/ohos/ohos_media_codec_bridge.h"
#include "media/base/ohos/ohos_media_decoder_bridge_impl.h"
#include "media/base/scoped_async_trace.h"
#include "media/base/supported_video_decoder_config.h"
#include "media/base/video_codecs.h"
#include "media/base/video_decoder.h"
#include "media/base/video_decoder_config.h"
#include "media/base/video_frame.h"
#include "media/base/video_frame_layout.h"
#include "media/gpu/ohos/codec_allocator.h"
#include "media/gpu/ohos/codec_surface_bundle.h"
#include "media/gpu/ohos/codec_wrapper.h"
#include "media_codec_decoder_adapter.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtMost;
using ::testing::Invoke;
using ::testing::Return;

namespace media {

namespace {

class MockMediaLog : public MediaLog {
 public:
  MockMediaLog() = default;
  ~MockMediaLog() override {}

  MockMediaLog(const MockMediaLog&) = delete;
  MockMediaLog& operator=(const MockMediaLog&) = delete;

  MOCK_METHOD(std::unique_ptr<MediaLog>, Clone, (), (override));
  MOCK_METHOD(void, Stop, (), (override));
  MOCK_METHOD(void, AddLogRecordLocked, (std::unique_ptr<MediaLogRecord> event), (override));
  MOCK_METHOD(void, OnWebMediaPlayerDestroyedLocked, (), (override));
  MOCK_METHOD(std::string, GetErrorMessageLocked, (), (override));
};

class MockVideoFrameFactory : public VideoFrameFactory {
 public:
  MockVideoFrameFactory() = default;
  ~MockVideoFrameFactory() override {}

  MockVideoFrameFactory(const MockVideoFrameFactory&) = delete;
  MockVideoFrameFactory& operator=(const MockVideoFrameFactory&) = delete;

  MOCK_METHOD(void, Initialize, (InitCB init_cb), (override));
  MOCK_METHOD(void, SetSurfaceBundle,
    (scoped_refptr<CodecSurfaceBundle> surface_bundle), (override));
  MOCK_METHOD(void, CreateVideoFrame, (
      std::unique_ptr<CodecOutputBuffer> output_buffer,
      base::TimeDelta timestamp,
      gfx::Size natural_size,
      OnceOutputCB output_cb
    ), (override));
  MOCK_METHOD(void,
    RunAfterPendingVideoFrames, (base::OnceClosure closure), (override));
};

class MockRefCountedLock : public gpu::RefCountedLock {
 public:
  MockRefCountedLock() = default;
  ~MockRefCountedLock() override {}

  MockRefCountedLock(const MockRefCountedLock&) = delete;
  MockRefCountedLock& operator=(const MockRefCountedLock&) = delete;

  MOCK_METHOD(base::Lock*, GetDrDcLockPtr, (), (override));
  MOCK_METHOD(void, AssertAcquired, (), (override));

};
class MockMediaCodecDecoderAdapter : public MediaCodecDecoderAdapter {
 public:
  MOCK_METHOD(DecoderAdapterCode,
              CreateVideoDecoderByMime,
              (const std::string& mimetype),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              CreateVideoDecoderByName,
              (const std::string& name),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              ConfigureDecoder,
              (const std::shared_ptr<DecoderFormatAdapter> format),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetParameterDecoder,
              (const std::shared_ptr<DecoderFormatAdapter> format),
              (override));
  MOCK_METHOD(DecoderAdapterCode, SetOutputSurface, (void* window), (override));
  MOCK_METHOD(DecoderAdapterCode, PrepareDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, StartDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, StopDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, FlushDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, ResetDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, ReleaseDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode,
              QueueInputBufferDec,
              (uint32_t index,
               int64_t presentationTimeUs,
               int32_t size,
               int32_t offset,
               BufferFlag flag),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              GetOutputFormatDec,
              (std::shared_ptr<DecoderFormatAdapter> format),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              ReleaseOutputBufferDec,
              (uint32_t index, bool isRender),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetCallbackDec,
              (const std::shared_ptr<DecoderCallbackAdapter> callback),
              (override));
};
class MockSequencedTaskRunner : public base::SequencedTaskRunner {
 public:
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));

  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));

  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTask,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));

  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location& from_here,
               base::OnceClosure task,
               base::TimeTicks delayed_run_time,
               base::subtle::DelayPolicy delay_policy),
              (override));

  MOCK_METHOD(bool,
              PostDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location& from_here,
               base::OnceClosure task,
               base::TimeTicks delayed_run_time,
               base::subtle::DelayPolicy delay_policy),
              (override));
  MOCK_METHOD(bool,
              PostDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
};

class OhosVideoDecoderTest : public ::testing::Test {
 public:
  OhosVideoDecoderTest() = default;

 protected:
  void SetUp() override {
    gpu::GpuPreferences gpu_preferences;
    gpu::GpuFeatureInfo gpu_feature_info;
    std::unique_ptr<MediaLog> media_log = std::make_unique<MockMediaLog>();
    media_log_ = (MockMediaLog*)(media_log.get());
    CodecAllocator* codec_allocator = NULL;
    std::unique_ptr<VideoFrameFactory> video_frame_factory =
        std::make_unique<MockVideoFrameFactory>();
    video_frame_factory_ = (MockVideoFrameFactory*)(video_frame_factory.get());
    scoped_refptr<gpu::RefCountedLock> drdc_lock =
        base::MakeRefCounted<MockRefCountedLock>();
    ref_counted_lock_ = (MockRefCountedLock*)(drdc_lock.get());
    ohos_video_decoder_ = std::make_unique<OhosVideoDecoder>(
        gpu_preferences, gpu_feature_info, std::move(media_log),
        codec_allocator, std::move(video_frame_factory), std::move(drdc_lock));
    surface_bundle_ = base::MakeRefCounted<CodecSurfaceBundle>();
    task_runner = base::MakeRefCounted<MockSequencedTaskRunner>();
    codec_allocator = CodecAllocator::GetInstance(task_runner);
    ohos_video_decoder_->codec_allocator_ = std::move(codec_allocator);
    task_runner = static_cast<MockSequencedTaskRunner*>(
        ohos_video_decoder_->codec_allocator_->task_runner_.get());
  }

  void TearDown() override { task_runner->Release(); }

 private:
  base::test::TaskEnvironment task_environment_;

 public:
  std::unique_ptr<OhosVideoDecoder> ohos_video_decoder_;
  // mock pointers should only be used for EXPECT_CALL
  MockMediaLog* media_log_ = NULL;
  MockVideoFrameFactory* video_frame_factory_ = NULL;
  MockRefCountedLock* ref_counted_lock_ = NULL;
  scoped_refptr<CodecSurfaceBundle> surface_bundle_;
  scoped_refptr<MockSequencedTaskRunner> task_runner;
};
}  // namespace

TEST_F(OhosVideoDecoderTest, PendingDecode_CreateEos) {
  PendingDecode pending_decode = PendingDecode::CreateEos();
  ASSERT_TRUE(pending_decode.buffer.get());
  EXPECT_FALSE(pending_decode.buffer->data());
  EXPECT_EQ(0u, pending_decode.buffer->data_size());
  EXPECT_TRUE(pending_decode.buffer->end_of_stream());
}

TEST_F(OhosVideoDecoderTest, GetSupportedConfigs) {
  auto configs = OhosVideoDecoder::GetSupportedConfigs();
  EXPECT_FALSE(configs.empty());
}

TEST_F(OhosVideoDecoderTest, Create) {
  gpu::GpuPreferences gpu_preferences;
  gpu::GpuFeatureInfo gpu_feature_info;
  std::unique_ptr<MediaLog> media_log = std::make_unique<MockMediaLog>();
  CodecAllocator* codec_allocator = NULL;
  std::unique_ptr<VideoFrameFactory> video_frame_factory =
      std::make_unique<MockVideoFrameFactory>();
  scoped_refptr<gpu::RefCountedLock> drdc_lock =
      base::MakeRefCounted<MockRefCountedLock>();
  auto decoder = OhosVideoDecoder::Create(
      gpu_preferences, gpu_feature_info, std::move(media_log),
      codec_allocator, std::move(video_frame_factory), std::move(drdc_lock));
  EXPECT_NE(decoder, nullptr);
}

TEST_F(OhosVideoDecoderTest, DestroyAsync) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  OhosVideoDecoder::DestroyAsync(std::move(ohos_video_decoder_));
}

TEST_F(OhosVideoDecoderTest, Initialize) {
  VideoDecoderConfig config;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
      std::move(init_cb), std::move(output_cb), std::move(waiting_cb));
}

TEST_F(OhosVideoDecoderTest, StartLazyInit) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  EXPECT_CALL((*video_frame_factory_), Initialize);
  ohos_video_decoder_->StartLazyInit();
}

TEST_F(OhosVideoDecoderTest, OnVideoFrameFactoryInitialized) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  scoped_refptr<gpu::NativeImageTextureOwner> texture_owner;
  ohos_video_decoder_->OnVideoFrameFactoryInitialized(std::move(texture_owner));
}

TEST_F(OhosVideoDecoderTest, OnSurfaceChosen) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kError;
  ohos_video_decoder_->OnSurfaceChosen();
}

TEST_F(OhosVideoDecoderTest, SurfaceTransitionPending) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  bool result = ohos_video_decoder_->SurfaceTransitionPending();
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest, TransitionToTargetSurface) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ASSERT_NE(codec_impl, nullptr);
  scoped_refptr<CodecSurfaceBundle> surface_bundle;
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->target_surface_bundle_ =
      base::MakeRefCounted<CodecSurfaceBundle>();
    ohos_video_decoder_->codec_allocator_ = CodecAllocator::GetInstance(
      base::SequencedTaskRunner::GetCurrentDefault());
  EXPECT_CALL((*video_frame_factory_), SetSurfaceBundle);
  ohos_video_decoder_->TransitionToTargetSurface();
}

TEST_F(OhosVideoDecoderTest, CreateCodec) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->codec_allocator_ = CodecAllocator::GetInstance(
      base::SequencedTaskRunner::GetCurrentDefault());
  EXPECT_CALL((*video_frame_factory_), SetSurfaceBundle);
  ohos_video_decoder_->CreateCodec();
}

TEST_F(OhosVideoDecoderTest, OnCodecConfiguredInternal) {
  base::WeakPtr<OhosVideoDecoder> weak;
  CodecAllocator* codec_allocator = NULL;
  scoped_refptr<CodecSurfaceBundle> surface_bundle;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec;
  OhosVideoDecoder::OnCodecConfiguredInternal(
      weak, codec_allocator, surface_bundle, std::move(codec));
}

TEST_F(OhosVideoDecoderTest, OnCodecConfigured) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  scoped_refptr<CodecSurfaceBundle> surface_bundle;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec;
  ohos_video_decoder_->OnCodecConfigured(surface_bundle, std::move(codec));
}

TEST_F(OhosVideoDecoderTest, Decode) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  const uint8_t kData[] = "HelloWorld";
  const size_t kDataSize = std::size(kData);
  scoped_refptr<DecoderBuffer> buffer(DecoderBuffer::CopyFrom(
      reinterpret_cast<const uint8_t*>(&kData), kDataSize));
  ASSERT_TRUE(buffer.get());
  EXPECT_NE(kData, buffer->data());
  EXPECT_EQ(buffer->data_size(), kDataSize);
  EXPECT_EQ(0, memcmp(buffer->data(), kData, kDataSize));
  EXPECT_FALSE(buffer->end_of_stream());
  EXPECT_FALSE(buffer->is_key_frame());
  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kError;
  ohos_video_decoder_->Decode(std::move(buffer), base::DoNothing());
}

TEST_F(OhosVideoDecoderTest, FlushCodec) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->FlushCodec();
}

TEST_F(OhosVideoDecoderTest, PumpCodec) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->PumpCodec();
}

TEST_F(OhosVideoDecoderTest, QueueInput) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest, DequeueOutput) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  bool result = ohos_video_decoder_->DequeueOutput();
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest, RunEosDecodeCb) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->reset_generation_ = 1;
  int reset_generation = 0;
  ohos_video_decoder_->RunEosDecodeCb(reset_generation);
}

TEST_F(OhosVideoDecoderTest, RunEosDecodeCb1) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->reset_generation_ = 1;
  int reset_generation = 1;
  ohos_video_decoder_->eos_decode_cb_.Reset();
  ohos_video_decoder_->RunEosDecodeCb(reset_generation);
  ASSERT_TRUE(ohos_video_decoder_->eos_decode_cb_.is_null());
}

TEST_F(OhosVideoDecoderTest, RunEosDecodeCb2) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->reset_generation_ = 1;
  int reset_generation = 1;
  ohos_video_decoder_->eos_decode_cb_ = base::DoNothing();
  ohos_video_decoder_->RunEosDecodeCb(reset_generation);
  ASSERT_TRUE(ohos_video_decoder_->eos_decode_cb_.is_null());
}

TEST_F(OhosVideoDecoderTest, ForwardVideoFrame) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->reset_generation_ = 1;
  int reset_generation = 0;
  std::unique_ptr<ScopedAsyncTrace> async_trace = nullptr;
  base::TimeTicks started_at = base::TimeTicks::Now();
  scoped_refptr<VideoFrame> frame;
  ohos_video_decoder_->ForwardVideoFrame(
      reset_generation, std::move(async_trace), started_at, frame);
}

TEST_F(OhosVideoDecoderTest, Reset) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->reset_generation_ = 0;
  ohos_video_decoder_->Reset(base::DoNothing());
  EXPECT_EQ(ohos_video_decoder_->reset_generation_, 1);
}

TEST_F(OhosVideoDecoderTest, StartDrainingCodec) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->StartDrainingCodec(
      OhosVideoDecoder::DrainType::kForDestroy);
}

TEST_F(OhosVideoDecoderTest, EnterTerminalState_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  OhosVideoDecoder::State state = OhosVideoDecoder::State::kError;
  char reason[10] = "reason";
  ohos_video_decoder_->EnterTerminalState(state, reason);
  EXPECT_FALSE(ohos_video_decoder_->drain_type_);
}

TEST_F(OhosVideoDecoderTest, EnterTerminalState_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  OhosVideoDecoder::State state = OhosVideoDecoder::State::kInitializing;
  char reason[10] = "reason";
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType::kForDestroy;
  ohos_video_decoder_->EnterTerminalState(state, reason);
  EXPECT_EQ(ohos_video_decoder_->codec_, nullptr);
}

TEST_F(OhosVideoDecoderTest, OnCodecDrained) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  ohos_video_decoder_->OnCodecDrained();
}

TEST_F(OhosVideoDecoderTest, EnterTerminalState) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kInitializing;
  OhosVideoDecoder::State state = OhosVideoDecoder::State::kSurfaceDestroyed;
  std::string reason = "reason";
  ohos_video_decoder_->EnterTerminalState(state, reason.c_str());
  EXPECT_EQ(ohos_video_decoder_->state_, state);
  EXPECT_EQ(ohos_video_decoder_->target_surface_bundle_, nullptr);
  EXPECT_EQ(ohos_video_decoder_->texture_owner_bundle_, nullptr);
}

TEST_F(OhosVideoDecoderTest, InTerminalState) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kError;
  bool result = ohos_video_decoder_->InTerminalState();
  EXPECT_TRUE(result);
}

TEST_F(OhosVideoDecoderTest, CancelPendingDecodes) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->CancelPendingDecodes(DecoderStatus::Codes::kOk);
  EXPECT_EQ(ohos_video_decoder_->pending_decodes_.size(), 0u);
}

TEST_F(OhosVideoDecoderTest, ReleaseCodec) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->ReleaseCodec();
  EXPECT_EQ(ohos_video_decoder_->codec_, nullptr);
}

TEST_F(OhosVideoDecoderTest, DestroyAsync_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->reset_cb_.Reset();
  ASSERT_FALSE(ohos_video_decoder_->reset_cb_);
  OhosVideoDecoder::DestroyAsync(std::move(ohos_video_decoder_));
}

TEST_F(OhosVideoDecoderTest, DestroyAsync_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  auto reset_cb = base::BindOnce([]() {});
  ohos_video_decoder_->reset_cb_ = std::move(reset_cb);
  ASSERT_TRUE(ohos_video_decoder_->reset_cb_);
  OhosVideoDecoder::DestroyAsync(std::move(ohos_video_decoder_));
}

TEST_F(OhosVideoDecoderTest, Initialize_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kUnknown;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 50));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kUnknown;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);
  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_NE(ohos_video_decoder_->output_cb_, output_cb);
}

TEST_F(OhosVideoDecoderTest, Initialize_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kUnknown;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 50));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kDolbyVision;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);
  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_NE(ohos_video_decoder_->output_cb_, output_cb);
}

TEST_F(OhosVideoDecoderTest, Initialize_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kUnknown;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 50));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kUnknown;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);

  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_NE(ohos_video_decoder_->output_cb_, output_cb);
}

TEST_F(OhosVideoDecoderTest, Initialize_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kDolbyVision;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 50));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kMaxValue;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);

  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_NE(ohos_video_decoder_->output_cb_, output_cb);
}

TEST_F(OhosVideoDecoderTest, Initialize_005) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kDolbyVision;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 50));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kUnknown;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);

  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_EQ(ohos_video_decoder_->output_cb_, output_cb);
  EXPECT_EQ(ohos_video_decoder_->last_width_,
            ohos_video_decoder_->decoder_config_.coded_size().width());
}

TEST_F(OhosVideoDecoderTest, Initialize_006) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kMaxValue;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 50));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kMaxValue;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);

  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_EQ(ohos_video_decoder_->output_cb_, output_cb);
  EXPECT_NE(ohos_video_decoder_->last_width_,
            ohos_video_decoder_->decoder_config_.coded_size().width());
}

TEST_F(OhosVideoDecoderTest, Initialize_007) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kDolbyVision;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 200));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kMaxValue;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);

  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_EQ(ohos_video_decoder_->output_cb_, output_cb);
  EXPECT_EQ(ohos_video_decoder_->last_width_,
            ohos_video_decoder_->decoder_config_.coded_size().width());
}

TEST_F(OhosVideoDecoderTest, Initialize_008) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kMaxValue;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 200));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kMaxValue;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);

  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_EQ(ohos_video_decoder_->output_cb_, output_cb);
  EXPECT_EQ(ohos_video_decoder_->last_width_,
            ohos_video_decoder_->decoder_config_.coded_size().width());
}

TEST_F(OhosVideoDecoderTest, Initialize_009) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kMaxValue;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 50));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kMaxValue;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);

  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_EQ(ohos_video_decoder_->output_cb_, output_cb);
  EXPECT_NE(ohos_video_decoder_->last_width_,
            ohos_video_decoder_->decoder_config_.coded_size().width());
}

TEST_F(OhosVideoDecoderTest, Initialize_010) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoDecoderConfig config;
  config.set_coded_size(gfx::Size(100, 100));
  config.set_visible_rect(gfx::Rect(50, 50));
  config.set_natural_size(gfx::Size(50, 50));
  config.codec_ = VideoCodec::kMaxValue;
  bool low_delay = false;
  CdmContext* cdm_context = NULL;
  VideoDecoder::InitCB init_cb = base::DoNothing();
  VideoDecoder::OutputCB output_cb = base::DoNothing();
  WaitingCB waiting_cb = base::DoNothing();
  auto decoder_config = std::make_unique<VideoDecoderConfig>();
  decoder_config->set_coded_size(gfx::Size(100, 100));
  decoder_config->set_visible_rect(gfx::Rect(50, 200));
  decoder_config->set_natural_size(gfx::Size(50, 50));
  decoder_config->codec_ = VideoCodec::kMaxValue;
  ohos_video_decoder_->decoder_config_ = std::move(*decoder_config);
  ohos_video_decoder_->Initialize(config, low_delay, cdm_context,
                                  std::move(init_cb), output_cb, waiting_cb);
  EXPECT_EQ(ohos_video_decoder_->output_cb_, output_cb);
  EXPECT_EQ(ohos_video_decoder_->last_width_,
            ohos_video_decoder_->decoder_config_.coded_size().width());
}

TEST_F(OhosVideoDecoderTest, InTerminalState_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kError;
  bool result = ohos_video_decoder_->InTerminalState();
  EXPECT_TRUE(result);
}

TEST_F(OhosVideoDecoderTest, InTerminalState_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kSurfaceDestroyed;
  bool result = ohos_video_decoder_->InTerminalState();
  EXPECT_TRUE(result);
}

TEST_F(OhosVideoDecoderTest, InTerminalState_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kRunning;
  bool result = ohos_video_decoder_->InTerminalState();
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest, InTerminalState_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);

  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kRunning;
  bool result = ohos_video_decoder_->InTerminalState();
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest, CancelPendingDecodes_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->pending_decodes_.emplace_front(
      PendingDecode::CreateEos());
  ASSERT_FALSE(ohos_video_decoder_->pending_decodes_.empty());
  DecoderStatus status;
  ohos_video_decoder_->CancelPendingDecodes(status);
}

TEST_F(OhosVideoDecoderTest, CancelPendingDecodes_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->pending_decodes_.emplace_back(
      PendingDecode::CreateEos());
  ASSERT_FALSE(ohos_video_decoder_->pending_decodes_.empty());
  ohos_video_decoder_->pending_decodes_.clear();
  ASSERT_TRUE(ohos_video_decoder_->pending_decodes_.empty());
  DecoderStatus status;
  ohos_video_decoder_->CancelPendingDecodes(status);
}

TEST_F(OhosVideoDecoderTest, CancelPendingDecodes_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  DecoderStatus status;
  ohos_video_decoder_->eos_decode_cb_.Reset();
  ASSERT_FALSE(ohos_video_decoder_->eos_decode_cb_);
  ohos_video_decoder_->CancelPendingDecodes(status);
}

TEST_F(OhosVideoDecoderTest, CancelPendingDecodes_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  DecoderStatus status;
  auto eos_decode_cb = base::BindOnce([](DecoderStatus) {});
  ohos_video_decoder_->eos_decode_cb_ = std::move(eos_decode_cb);
  ASSERT_TRUE(ohos_video_decoder_->eos_decode_cb_);
  ohos_video_decoder_->CancelPendingDecodes(status);
}

TEST_F(OhosVideoDecoderTest, ReleaseCodec_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle_)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  ASSERT_TRUE(ohos_video_decoder_->codec_);
  ohos_video_decoder_->ReleaseCodec();
  EXPECT_EQ(ohos_video_decoder_->codec_, nullptr);
  task_runner->Release();
}

TEST_F(OhosVideoDecoderTest, ReleaseCodec_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
  ASSERT_FALSE(ohos_video_decoder_->codec_);
  ohos_video_decoder_->ReleaseCodec();
  EXPECT_EQ(ohos_video_decoder_->codec_, nullptr);
}

TEST_F(OhosVideoDecoderTest, StartLazyInit_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->lazy_init_pending_ = true;
  EXPECT_CALL(*video_frame_factory_, Initialize);
  ohos_video_decoder_->StartLazyInit();
  EXPECT_FALSE(ohos_video_decoder_->lazy_init_pending_);
}

TEST_F(OhosVideoDecoderTest, OnVideoFrameFactoryInitialized_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->OnVideoFrameFactoryInitialized(std::move(nullptr));
  EXPECT_EQ(ohos_video_decoder_->texture_owner_bundle_, nullptr);
}

TEST_F(OhosVideoDecoderTest, OnSurfaceChosen_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kInitializing;
  EXPECT_CALL(*video_frame_factory_, SetSurfaceBundle);
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  ohos_video_decoder_->OnSurfaceChosen();
  EXPECT_EQ(ohos_video_decoder_->state_, OhosVideoDecoder::State::kRunning);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest, OnSurfaceChosen_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kRunning;
  ohos_video_decoder_->OnSurfaceChosen();
}

TEST_F(OhosVideoDecoderTest, SurfaceTransitionPending_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
  ASSERT_FALSE(ohos_video_decoder_->codec_);
  bool result = ohos_video_decoder_->SurfaceTransitionPending();
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest, SurfaceTransitionPending_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
  ASSERT_FALSE(ohos_video_decoder_->codec_);
  bool result = ohos_video_decoder_->SurfaceTransitionPending();
  ohos_video_decoder_->codec_ = nullptr;
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest, SurfaceTransitionPending_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  scoped_refptr<MockSequencedTaskRunner> task_runner =
      base::MakeRefCounted<MockSequencedTaskRunner>();
  raw_ptr<CodecAllocator> codec_allocator =
      CodecAllocator::GetInstance(task_runner);
  ASSERT_TRUE(codec_allocator);
  ohos_video_decoder_->codec_allocator_ = std::move(codec_allocator);
  bool result = ohos_video_decoder_->SurfaceTransitionPending();
  ohos_video_decoder_->codec_ = nullptr;
  EXPECT_TRUE(result);
}

TEST_F(OhosVideoDecoderTest, SurfaceTransitionPending_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  scoped_refptr<MockSequencedTaskRunner> task_runner =
      base::MakeRefCounted<MockSequencedTaskRunner>();
  raw_ptr<CodecAllocator> codec_allocator =
      CodecAllocator::GetInstance(task_runner);
  ASSERT_TRUE(codec_allocator);
  ohos_video_decoder_->codec_allocator_ = std::move(codec_allocator);
  ohos_video_decoder_->target_surface_bundle_ =
      ohos_video_decoder_->codec_->SurfaceBundle();
  ASSERT_TRUE(ohos_video_decoder_->codec_);
  bool result = ohos_video_decoder_->SurfaceTransitionPending();
  ohos_video_decoder_->codec_ = nullptr;
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest, OnCodecConfigured_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec = nullptr;
  ohos_video_decoder_->OnCodecConfigured(surface_bundle_, std::move(codec));
}

TEST_F(OhosVideoDecoderTest, OnCodecConfigured_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  ASSERT_TRUE(config);
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  ASSERT_TRUE(codec);
  std::shared_ptr<DecoderBridgeSignal> signal =
      std::make_shared<DecoderBridgeSignal>();
  auto cb = std::make_shared<CodecBridgeCallback>(signal);
  codec->cb_ = std::move(cb);
  codec->videoDecoder_ = std::make_unique<MockMediaCodecDecoderAdapter>();
  auto videoDecoder =
      static_cast<MockMediaCodecDecoderAdapter*>(codec->videoDecoder_.get());
  EXPECT_CALL(*videoDecoder, SetOutputSurface)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, PrepareDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, StartDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, ConfigureDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, ReleaseDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  ohos_video_decoder_->OnCodecConfigured(std::move(surface_bundle_),
                                         std::move(codec));
}

TEST_F(OhosVideoDecoderTest, OnCodecConfigured_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ASSERT_TRUE(surface_bundle_);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  ASSERT_TRUE(config);
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  ASSERT_TRUE(codec);
  std::shared_ptr<DecoderBridgeSignal> signal =
      std::make_shared<DecoderBridgeSignal>();
  auto cb = std::make_shared<CodecBridgeCallback>(signal);
  codec->cb_ = std::move(cb);
  codec->videoDecoder_ = std::make_unique<MockMediaCodecDecoderAdapter>();
  auto videoDecoder =
      static_cast<MockMediaCodecDecoderAdapter*>(codec->videoDecoder_.get());
  EXPECT_CALL(*videoDecoder, SetOutputSurface)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, PrepareDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, StartDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, ConfigureDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, ReleaseDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  ohos_video_decoder_->OnCodecConfigured(std::move(surface_bundle_),
                                         std::move(codec));
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest, OnCodecConfigured_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ASSERT_TRUE(surface_bundle_);
  uint32_t ohos_native_window = 10;
  surface_bundle_->ohos_native_window_ = &ohos_native_window;
  auto config = std::make_unique<VideoBridgeCodecConfig>();
  ASSERT_TRUE(config);
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  ASSERT_TRUE(codec);
  std::shared_ptr<DecoderBridgeSignal> signal =
      std::make_shared<DecoderBridgeSignal>();
  auto cb = std::make_shared<CodecBridgeCallback>(signal);
  codec->cb_ = std::move(cb);
  std::unique_ptr<MockMediaCodecDecoderAdapter> videoDecoder =
      std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*videoDecoder, ConfigureDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, ReleaseDecoder)
      .WillRepeatedly(
          testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, SetOutputSurface)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, PrepareDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*videoDecoder, StartDecoder)
      .WillOnce(testing::Return(OHOS::NWeb::DecoderAdapterCode::DECODER_OK));
  codec->videoDecoder_ = std::move(videoDecoder);
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  ohos_video_decoder_->codec_ = nullptr;
  ohos_video_decoder_->OnCodecConfigured(std::move(surface_bundle_),
                                         std::move(codec));
}

TEST_F(OhosVideoDecoderTest, Decode_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  size_t size = 1;
  scoped_refptr<DecoderBuffer> buffer =
      base::MakeRefCounted<DecoderBuffer>(size);
  std::shared_ptr<DecoderBridgeSignal> signal =
      std::make_shared<DecoderBridgeSignal>();
  EXPECT_CALL(*video_frame_factory_, Initialize);
  OhosVideoDecoder::DecodeCB cb = base::BindOnce([](DecoderStatus) {});
  ohos_video_decoder_->Decode(buffer, std::move(cb));
  ASSERT_NE(buffer, nullptr);
}

TEST_F(OhosVideoDecoderTest, Decode_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  size_t size = 1;
  scoped_refptr<DecoderBuffer> buffer =
      base::MakeRefCounted<DecoderBuffer>(size);
  std::shared_ptr<DecoderBridgeSignal> signal =
      std::make_shared<DecoderBridgeSignal>();
  OhosVideoDecoder::DecodeCB cb = base::BindOnce([](DecoderStatus) {});
  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kError;
  ohos_video_decoder_->Decode(std::move(buffer), std::move(cb));
  EXPECT_NE(ohos_video_decoder_->state_, OhosVideoDecoder::State::kInitializing);
}

TEST_F(OhosVideoDecoderTest, Decode_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  size_t size = 1;
  scoped_refptr<DecoderBuffer> buffer =
      base::MakeRefCounted<DecoderBuffer>(size);
  std::shared_ptr<DecoderBridgeSignal> signal =
      std::make_shared<DecoderBridgeSignal>();
  OhosVideoDecoder::DecodeCB cb = base::BindOnce([](DecoderStatus) {});
  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kInitializing;
  ohos_video_decoder_->lazy_init_pending_ = false;
  ohos_video_decoder_->Decode(std::move(buffer), std::move(cb));
  EXPECT_FALSE(ohos_video_decoder_->lazy_init_pending_);
}

TEST_F(OhosVideoDecoderTest, Decode_005) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  size_t size = 1;
  scoped_refptr<DecoderBuffer> buffer =
      base::MakeRefCounted<DecoderBuffer>(size);
  std::shared_ptr<DecoderBridgeSignal> signal =
      std::make_shared<DecoderBridgeSignal>();
  OhosVideoDecoder::DecodeCB cb = base::BindOnce([](DecoderStatus) {});
  EXPECT_CALL(*video_frame_factory_, Initialize);
  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kInitializing;
  ohos_video_decoder_->lazy_init_pending_ = true;
  ohos_video_decoder_->Decode(std::move(buffer), std::move(cb));
  EXPECT_FALSE(ohos_video_decoder_->lazy_init_pending_);
}

TEST_F(OhosVideoDecoderTest, Decode_006) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  size_t size = 1;
  scoped_refptr<DecoderBuffer> buffer =
      base::MakeRefCounted<DecoderBuffer>(size);
  std::shared_ptr<DecoderBridgeSignal> signal =
      std::make_shared<DecoderBridgeSignal>();
  OhosVideoDecoder::DecodeCB cb = base::BindOnce([](DecoderStatus) {});
  ohos_video_decoder_->state_ = OhosVideoDecoder::State::kRunning;
  ohos_video_decoder_->Decode(std::move(buffer), std::move(cb));
  EXPECT_NE(ohos_video_decoder_->state_, OhosVideoDecoder::State::kInitializing);
}

TEST_F(OhosVideoDecoderTest, ForwardVideoFrame_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  int reset_generation = 0;
  std::unique_ptr<ScopedAsyncTrace> async_trace = nullptr;
  base::TimeTicks now = base::TimeTicks::Now();
  scoped_refptr<VideoFrame> frame;
  ohos_video_decoder_->ForwardVideoFrame(reset_generation,
                                         std::move(async_trace), now, frame);
  ASSERT_FALSE(frame);
}

TEST_F(OhosVideoDecoderTest, ForwardVideoFrame_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  int reset_generation = 1;
  std::unique_ptr<ScopedAsyncTrace> async_trace = nullptr;
  base::TimeTicks now = base::TimeTicks::Now();
  auto layout = VideoFrameLayout::Create(VideoPixelFormat::PIXEL_FORMAT_I422A,
                                         gfx::Size());
  base::TimeDelta delta = base::Seconds(1);
  scoped_refptr<VideoFrame> frame(new VideoFrame(
      layout.value(), VideoFrame::StorageType::STORAGE_GPU_MEMORY_BUFFER,
      gfx::Rect(), gfx::Size(), delta));
  ohos_video_decoder_->ForwardVideoFrame(reset_generation,
                                         std::move(async_trace), now, frame);
  ASSERT_TRUE(frame);
  EXPECT_NE(reset_generation, ohos_video_decoder_->reset_generation_);
}

TEST_F(OhosVideoDecoderTest, DequeueOutput_001) {
  ohos_video_decoder_->codec_ = nullptr;
  testing::internal::CaptureStderr();
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OhosVideoDecoder::DequeueOutput failed"), std::string::npos);
}

}  // namespace media
