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
#include "media/gpu/ohos/ohos_video_decoder.h"
#undef private

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/weak_ptr.h"
#include "base/run_loop.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/test/task_environment.h"
#include "media/base/async_destroy_video_decoder.h"
#include "media/base/decoder_buffer.h"
#include "media/base/media_log.h"
#include "media/base/ohos/ohos_media_codec_bridge.h"
#include "media/base/ohos/ohos_media_decoder_bridge_impl.h"
#include "media/base/scoped_async_trace.h"
#include "media/base/supported_video_decoder_config.h"
#include "media/base/video_codecs.h"
#include "media/base/video_decoder_config.h"
#include "media/base/video_frame.h"
#include "media/gpu/ohos/codec_surface_bundle.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

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
  }

  void TearDown() override {}

 private:
  base::test::TaskEnvironment task_environment_;

 public:
  std::unique_ptr<OhosVideoDecoder> ohos_video_decoder_;
  // mock pointers should only be used for EXPECT_CALL
  MockMediaLog* media_log_ = NULL;
  MockVideoFrameFactory* video_frame_factory_ = NULL;
  MockRefCountedLock* ref_counted_lock_ = NULL;
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

}  // namespace base
