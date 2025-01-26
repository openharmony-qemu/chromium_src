// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <memory>
#include <utility>
#include "gtest/gtest.h"
#define private public
#include "base/memory/scoped_refptr.h"
#include "media/base/video_frame.h"
#include "media/gpu/ohos/frame_info_helper.h"
#include "media/gpu/ohos/video_frame_factory.h"
#include "media/gpu/ohos/video_frame_factory_impl.cc"
#include "ui/gfx/color_space.h"
#undef private
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/gmock_callback_support.h"
#include "base/test/mock_callback.h"
#include "base/test/task_environment.h"
#include "gpu/command_buffer/service/ref_counted_lock_for_test.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/config/gpu_preferences.h"
#include "media/base/limits.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "media/base/media_log.h"
#include "media/gpu/ohos/codec_buffer_wait_coordinator.h"
#include "media/gpu/ohos/codec_image.h"
#include "media/gpu/ohos/codec_wrapper.cc"
#include "media/gpu/ohos/codec_wrapper.h"
#include "media/gpu/ohos/ohos_video_decoder.h"
#include "media/gpu/ohos/shared_image_video_provider.h"
#include "media/gpu/ohos/video_frame_factory_impl.h"

using base::test::RunOnceCallback;
using testing::_;
using testing::SaveArg;
using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtMost;
using ::testing::Invoke;
using ::testing::Return;

namespace media {

class MockSharedImageVideoProvider : public SharedImageVideoProvider {
 public:
  MockSharedImageVideoProvider() = default;
  MockSharedImageVideoProvider(ImageReadyCB cb, ImageSpec spec)
      : cb_(std::move(cb)), spec_(std::move(spec)) {}

  void Initialize(GpuInitCB gpu_init_cb) override {
    Initialize_(gpu_init_cb);
    gpu_init_cb_ = std::move(gpu_init_cb);
  }

  void RequestImage(ImageReadyCB cb, const ImageSpec& spec) override {
    MockRequestImage();
  }
  MOCK_METHOD1(Initialize_, void(GpuInitCB& gpu_init_cb));
  MOCK_METHOD0(MockRequestImage, void());
  ImageReadyCB cb_;
  ImageSpec spec_;
  GpuInitCB gpu_init_cb_;
};

class MockFrameInfoHelper : public FrameInfoHelper {
 public:
  void GetFrameInfo(std::unique_ptr<CodecOutputBufferRenderer> buffer_renderer,
                    FrameInfoReadyCB cb) override {
    FrameInfo info;
    info.coded_size = buffer_renderer->size();
    info.visible_rect = gfx::Rect(info.coded_size);

    std::move(cb).Run(std::move(buffer_renderer), info);
  }
};

class MockMediaLog : public MediaLog {
 public:
  MockMediaLog() = default;
  ~MockMediaLog() override {}

  MockMediaLog(const MockMediaLog&) = delete;
  MockMediaLog& operator=(const MockMediaLog&) = delete;

  MOCK_METHOD(std::unique_ptr<MediaLog>, Clone, (), (override));
  MOCK_METHOD(void, Stop, (), (override));
  MOCK_METHOD(void,
              AddLogRecordLocked,
              (std::unique_ptr<MediaLogRecord> event),
              (override));
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
  MOCK_METHOD(void,
              SetSurfaceBundle,
              (scoped_refptr<CodecSurfaceBundle> surface_bundle),
              (override));
  MOCK_METHOD(void,
              CreateVideoFrame,
              (std::unique_ptr<CodecOutputBuffer> output_buffer,
               base::TimeDelta timestamp,
               gfx::Size natural_size,
               OnceOutputCB output_cb),
              (override));
  MOCK_METHOD(void,
              RunAfterPendingVideoFrames,
              (base::OnceClosure closure),
              (override));
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

class MockMediaCodecDecoderAdapter : public MediaCodecDecoderAdapter {
 public:
  MOCK_METHOD(DecoderAdapterCode,
              CreateVideoDecoderByMime,
              (const std::string&),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              CreateVideoDecoderByName,
              (const std::string&),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              ConfigureDecoder,
              (std::shared_ptr<DecoderFormatAdapter>),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetParameterDecoder,
              (std::shared_ptr<DecoderFormatAdapter>),
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
              (uint32_t, int64_t, int32_t, int32_t, BufferFlag),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              GetOutputFormatDec,
              (std::shared_ptr<DecoderFormatAdapter> format),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              ReleaseOutputBufferDec,
              (uint32_t, bool),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetCallbackDec,
              (std::shared_ptr<DecoderCallbackAdapter>),
              (override));
};

class VideoFrameFactoryImplTest : public testing::Test {
 public:
  VideoFrameFactoryImplTest()
      : task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {
    auto image_provider = std::make_unique<MockSharedImageVideoProvider>();
    image_provider_raw_ = image_provider.get();

    auto info_helper = std::make_unique<MockFrameInfoHelper>();
    impl_ = std::make_unique<VideoFrameFactoryImpl>(
        task_runner_, gpu_preferences_, std::move(image_provider),
        std::move(info_helper), nullptr);
  }

  ~VideoFrameFactoryImplTest() override = default;

  struct {
    gfx::Size coded_size{100, 100};
    gfx::Rect visible_rect{coded_size};
    gfx::Size natural_size{coded_size};
    gfx::ColorSpace color_space{gfx::ColorSpace::CreateSRGBLinear()};
  } video_frame_params_;

  void RequestVideoFrame() {
    scoped_refptr<CodecWrapperImpl> codec = nullptr;
    auto output_buffer = std::make_unique<CodecOutputBuffer>(
        std::move(codec), 0, video_frame_params_.coded_size,
#ifdef OHOS_VIDEO_ASSISTANT
        false,
#endif // OHOS_VIDEO_ASSISTANT
        video_frame_params_.color_space);
    ASSERT_TRUE(VideoFrame::IsValidConfig(
        PIXEL_FORMAT_ARGB, VideoFrame::STORAGE_OPAQUE,
        video_frame_params_.coded_size, video_frame_params_.visible_rect,
        video_frame_params_.natural_size));

    output_buffer_raw_ = output_buffer.get();
    EXPECT_CALL(*image_provider_raw_, MockRequestImage());
    impl_->CreateVideoFrame(std::move(output_buffer), base::TimeDelta(),
                            video_frame_params_.natural_size, output_cb_.Get());
  }

 protected:
  void SetUp() override {
    image_provider_ = std::make_unique<MockSharedImageVideoProvider>();
    frame_info_helper_ = std::make_unique<MockFrameInfoHelper>();
    drdc_lock_ = base::MakeRefCounted<gpu::RefCountedLock>();
    video_frame_factory_ = std::make_unique<VideoFrameFactoryImpl>(
        gpu_task_runner_, gpu_preferences_, std::move(image_provider_),
        std::move(frame_info_helper_), std::move(drdc_lock_));
  }

  base::test::TaskEnvironment task_environment_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  std::unique_ptr<VideoFrameFactoryImpl> impl_;

  raw_ptr<MockSharedImageVideoProvider> image_provider_raw_ = nullptr;
  raw_ptr<CodecOutputBuffer> output_buffer_raw_ = nullptr;

  base::MockCallback<VideoFrameFactory::OnceOutputCB> output_cb_;
  gpu::GpuPreferences gpu_preferences_;

  scoped_refptr<base::SingleThreadTaskRunner> gpu_task_runner_;
  std::unique_ptr<MockSharedImageVideoProvider> image_provider_;
  std::unique_ptr<MockFrameInfoHelper> frame_info_helper_;
  scoped_refptr<gpu::RefCountedLock> drdc_lock_;
  std::unique_ptr<VideoFrameFactoryImpl> video_frame_factory_;
};

class OhosVideoDecoderTest2 : public ::testing::Test {
 public:
  OhosVideoDecoderTest2() = default;

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
  MockMediaLog* media_log_ = NULL;
  MockVideoFrameFactory* video_frame_factory_ = NULL;
  MockRefCountedLock* ref_counted_lock_ = NULL;
  scoped_refptr<CodecSurfaceBundle> surface_bundle_;
  scoped_refptr<MockSequencedTaskRunner> task_runner;
};

class OhosVideoDecoderTest3 : public ::testing::Test {
 public:
  OhosVideoDecoderTest3() = default;

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
    ON_CALL(*task_runner, RunsTasksInCurrentSequence)
        .WillByDefault(testing::Return(false));
    ON_CALL(*task_runner, PostDelayedTask).WillByDefault(testing::Return(false));
  }

  void TearDown() override { task_runner->Release(); }

 private:
  base::test::TaskEnvironment task_environment_;

 public:
  std::unique_ptr<OhosVideoDecoder> ohos_video_decoder_;
  MockMediaLog* media_log_ = NULL;
  MockVideoFrameFactory* video_frame_factory_ = NULL;
  MockRefCountedLock* ref_counted_lock_ = NULL;
  scoped_refptr<CodecSurfaceBundle> surface_bundle_;
  scoped_refptr<MockSequencedTaskRunner> task_runner;
};

TEST_F(VideoFrameFactoryImplTest, Initialize) {
  EXPECT_CALL(*image_provider_raw_, Initialize_(_))
      .Times(1)
      .WillOnce(RunOnceCallback<0>(nullptr));
  base::MockCallback<VideoFrameFactory::InitCB> init_cb;
  impl_->Initialize(init_cb.Get());
}

TEST_F(VideoFrameFactoryImplTest, SetSurfaceBundle) {
  impl_->SetSurfaceBundle(nullptr);
  EXPECT_EQ(impl_->codec_buffer_wait_coordinator_, nullptr);
}

TEST_F(VideoFrameFactoryImplTest, CreateVideoFrame) {
  gfx::Size coded_size(limits::kMaxDimension + 1, limits::kMaxDimension + 1);
  gfx::Rect visible_rect(coded_size);
  gfx::Size natural_size(0, 0);
  scoped_refptr<CodecWrapperImpl> codec = nullptr;
  auto output_buffer = std::make_unique<CodecOutputBuffer>(
      std::move(codec), 0, coded_size,
#ifdef OHOS_VIDEO_ASSISTANT
      false,
#endif // OHOS_VIDEO_ASSISTANT
      gfx::ColorSpace());
  ASSERT_FALSE(VideoFrame::IsValidConfig(PIXEL_FORMAT_ARGB,
                                         VideoFrame::STORAGE_OPAQUE, coded_size,
                                         visible_rect, natural_size));

  base::MockCallback<VideoFrameFactory::OnceOutputCB> output_cb;
  EXPECT_CALL(output_cb, Run(scoped_refptr<VideoFrame>(nullptr)));
  EXPECT_CALL(*image_provider_raw_, MockRequestImage()).Times(0);

  impl_->CreateVideoFrame(std::move(output_buffer), base::TimeDelta(),
                          natural_size, output_cb.Get());
}

TEST_F(VideoFrameFactoryImplTest, CreateVideoFrame_OnFrameInfoReady) {
  base::MockCallback<VideoFrameFactoryImpl::ImageWithInfoReadyCB>
      image_ready_cb;
  std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer;
  FrameInfoHelper::FrameInfo frame_info;
  EXPECT_CALL(image_ready_cb, Run(_, _, _));
  impl_->CreateVideoFrame_OnFrameInfoReady(
      image_ready_cb.Get(), std::move(output_buffer_renderer), frame_info);
  EXPECT_TRUE(image_ready_cb.Get());
}

TEST_F(VideoFrameFactoryImplTest, CreateVideoFrame_OnImageReady) {
  RequestVideoFrame();
  base::WeakPtr<VideoFrameFactoryImpl> thiz;
  FrameInfoHelper::FrameInfo frame_info;
  gfx::Size natural_size(0, 0);
  VideoPixelFormat pixel_format = VideoPixelFormat::PIXEL_FORMAT_ABGR;
  scoped_refptr<base::SequencedTaskRunner> gpu_task_runner;
  std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer;
  SharedImageVideoProvider::ImageRecord record;

  impl_->CreateVideoFrame_OnImageReady(
      thiz, output_cb_.Get(), base::TimeDelta(), natural_size, false,
      pixel_format, false, gpu_task_runner, std::move(output_buffer_renderer),
      frame_info, std::move(record));
}

TEST_F(VideoFrameFactoryImplTest,
       AllocateTextureOwnerOnGpuThread_NullSharedContextState) {
  scoped_refptr<gpu::SharedContextState> shared_context_state = nullptr;
  bool init_cb_called = false;
  VideoFrameFactory::InitCB init_cb = base::BindRepeating(
      [](bool* called,
         scoped_refptr<gpu::NativeImageTextureOwner> texture_owner) {
        *called = true;
        EXPECT_EQ(texture_owner, nullptr);
      },
      &init_cb_called);
  AllocateTextureOwnerOnGpuThread(std::move(init_cb), nullptr,
                                  shared_context_state);
  task_environment_.RunUntilIdle();
  EXPECT_TRUE(init_cb_called);
  EXPECT_TRUE(init_cb.is_null());
}

TEST_F(VideoFrameFactoryImplTest, TestSetSurfaceBundle_NullSurfaceBundle) {
  scoped_refptr<CodecSurfaceBundle> surface_bundle = nullptr;
  video_frame_factory_->SetSurfaceBundle(surface_bundle);
  EXPECT_EQ(video_frame_factory_->codec_buffer_wait_coordinator_, nullptr);
}

TEST_F(VideoFrameFactoryImplTest, CreateVideoFrame_InvalidConfig) {
  int64_t id = 123;
  scoped_refptr<CodecWrapperImpl> codec;
  gfx::ColorSpace color_space = gfx::ColorSpace::CreateSRGB();
  gfx::Size natural_size(-1, -1);
  gfx::Size coded_size(-1, -1);
  gfx::Rect visible_rect(coded_size);
  gfx::Size size(1920, 1080);
  base::TimeDelta timestamp;
  auto output_buffer =
      std::make_unique<CodecOutputBuffer>(codec, id, size,
#ifdef OHOS_VIDEO_ASSISTANT
          false,
#endif // OHOS_VIDEO_ASSISTANT
          color_space);
  scoped_refptr<VideoFrame> result_frame;
  VideoFrameFactoryImpl::OnceOutputCB output_cb = base::BindOnce(
      [](scoped_refptr<VideoFrame>* frame_dest,
         scoped_refptr<VideoFrame> frame) { *frame_dest = frame; },
      &result_frame);
  video_frame_factory_->CreateVideoFrame(std::move(output_buffer), timestamp,
                                         natural_size, std::move(output_cb));
  EXPECT_EQ(result_frame, nullptr);
}

TEST_F(OhosVideoDecoderTest2, FlushCodec_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->deferred_reallocation_pending_ = false;
  ohos_video_decoder_->FlushCodec();
  EXPECT_FALSE(ohos_video_decoder_->deferred_reallocation_pending_);
}

TEST_F(OhosVideoDecoderTest2, FlushCodec_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->deferred_reallocation_pending_ = true;
  EXPECT_TRUE(ohos_video_decoder_->deferred_reallocation_pending_);
  EXPECT_CALL(*video_frame_factory_, SetSurfaceBundle);
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  ohos_video_decoder_->FlushCodec();
  EXPECT_FALSE(ohos_video_decoder_->deferred_reallocation_pending_);
}

TEST_F(OhosVideoDecoderTest2, FlushCodec_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->deferred_reallocation_pending_ = false;
  ohos_video_decoder_->codec_ = nullptr;
  ohos_video_decoder_->FlushCodec();
  ASSERT_FALSE(ohos_video_decoder_->codec_);
}

TEST_F(OhosVideoDecoderTest2, FlushCodec_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->deferred_reallocation_pending_ = false;
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle_)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ASSERT_TRUE(ohos_video_decoder_->codec_);
  ASSERT_TRUE(ohos_video_decoder_->codec_->IsFlushed());
  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kDrained;
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  ohos_video_decoder_->FlushCodec();
  ASSERT_FALSE(ohos_video_decoder_->codec_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, FlushCodec_005) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->deferred_reallocation_pending_ = false;
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle_)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kFlushed;
  ohos_video_decoder_->FlushCodec();
  ASSERT_TRUE(ohos_video_decoder_->codec_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, StartDrainingCodec_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  OhosVideoDecoder::DrainType drain_type =
      OhosVideoDecoder::DrainType::kForDestroy;
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle_)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kDraining;
  VideoDecoderConfig decoder_config;
  decoder_config.codec_ = VideoCodec::kVP8;
  ohos_video_decoder_->decoder_config_ = std::move(decoder_config);
  EXPECT_FALSE(ohos_video_decoder_->decoder_config_.codec() !=
               VideoCodec::kVP8);
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  ohos_video_decoder_->StartDrainingCodec(drain_type);
  EXPECT_FALSE(ohos_video_decoder_->drain_type_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, StartDrainingCodec_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  OhosVideoDecoder::DrainType drain_type =
      OhosVideoDecoder::DrainType::kForDestroy;
  ohos_video_decoder_->codec_ = nullptr;
  ohos_video_decoder_->deferred_flush_pending_ = true;
  VideoDecoderConfig decoder_config;
  decoder_config.codec_ = VideoCodec::kVP8;
  ohos_video_decoder_->decoder_config_ = std::move(decoder_config);
  ohos_video_decoder_->StartDrainingCodec(drain_type);
  ohos_video_decoder_->codec_ = nullptr;
  EXPECT_FALSE(ohos_video_decoder_->deferred_flush_pending_);
}

TEST_F(OhosVideoDecoderTest2, StartDrainingCodec_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->deferred_flush_pending_ = false;
  OhosVideoDecoder::DrainType drain_type =
      OhosVideoDecoder::DrainType::kForDestroy;
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle_)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  VideoDecoderConfig decoder_config;
  decoder_config.codec_ = VideoCodec::kVP9;
  ohos_video_decoder_->decoder_config_ = std::move(decoder_config);
  ohos_video_decoder_->StartDrainingCodec(drain_type);
  EXPECT_EQ(ohos_video_decoder_->deferred_flush_pending_, true);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, StartDrainingCodec_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->deferred_flush_pending_ = true;
  OhosVideoDecoder::DrainType drain_type =
      OhosVideoDecoder::DrainType::kForDestroy;
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle_)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kFlushed;
  VideoDecoderConfig decoder_config;
  decoder_config.codec_ = VideoCodec::kVP8;
  ohos_video_decoder_->decoder_config_ = std::move(decoder_config);
  ASSERT_FALSE(ohos_video_decoder_->decoder_config_.codec() !=
               VideoCodec::kVP8);
  ohos_video_decoder_->StartDrainingCodec(drain_type);
  EXPECT_EQ(ohos_video_decoder_->deferred_flush_pending_, false);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, StartDrainingCodec_005) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->deferred_flush_pending_ = true;
  OhosVideoDecoder::DrainType drain_type =
      OhosVideoDecoder::DrainType::kForDestroy;
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle_)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kDrained;
  VideoDecoderConfig decoder_config;
  decoder_config.codec_ = VideoCodec::kVP8;
  ohos_video_decoder_->decoder_config_ = std::move(decoder_config);
  ASSERT_FALSE(ohos_video_decoder_->decoder_config_.codec() !=
               VideoCodec::kVP8);
  ohos_video_decoder_->StartDrainingCodec(drain_type);
  EXPECT_EQ(ohos_video_decoder_->deferred_flush_pending_, false);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, StartDrainingCodec_006) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  OhosVideoDecoder::DrainType drain_type =
      OhosVideoDecoder::DrainType::kForDestroy;
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle_)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  VideoDecoderConfig decoder_config;
  decoder_config.codec_ = VideoCodec::kVP8;
  ohos_video_decoder_->decoder_config_ = std::move(decoder_config);
  ASSERT_FALSE(ohos_video_decoder_->decoder_config_.codec() !=
               VideoCodec::kVP8);
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence).WillOnce(Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(Return(false));
  int i = 2;
  do {
    ohos_video_decoder_->pending_decodes_.push_back(PendingDecode::CreateEos());
    i--;
  } while (i);
  auto pending_decodes_size = ohos_video_decoder_->pending_decodes_.size();
  ohos_video_decoder_->StartDrainingCodec(drain_type);
  EXPECT_NE(pending_decodes_size, ohos_video_decoder_->pending_decodes_.size());
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForDestroy;
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_EQ(ohos_video_decoder_->codec_, nullptr);
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_EQ(ohos_video_decoder_->codec_, nullptr);
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  ohos_video_decoder_->deferred_flush_pending_ = true;
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_EQ(ohos_video_decoder_->codec_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_004) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_EQ(ohos_video_decoder_->codec_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_005) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ASSERT_NE(codec_impl, nullptr);
  scoped_refptr<CodecSurfaceBundle> surface_bundle;
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kFlushed;
  ohos_video_decoder_->deferred_flush_pending_ = true;
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_NE(ohos_video_decoder_->codec_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_006) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ASSERT_NE(codec_impl, nullptr);
  scoped_refptr<CodecSurfaceBundle> surface_bundle;
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kFlushed;
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_NE(ohos_video_decoder_->codec_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_007) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ASSERT_NE(codec_impl, nullptr);
  scoped_refptr<CodecSurfaceBundle> surface_bundle;
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kDrained;
  ohos_video_decoder_->deferred_flush_pending_ = true;
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_NE(ohos_video_decoder_->codec_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_008) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ASSERT_NE(codec_impl, nullptr);
  scoped_refptr<CodecSurfaceBundle> surface_bundle;
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kDrained;
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_NE(ohos_video_decoder_->codec_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, OnCodecDrained_009) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType ::kForReset;
  ohos_video_decoder_->reset_cb_ = base::DoNothing();
  VideoBridgeCodecConfig codec_config;
  codec_config.codec = media::VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(codec_config);
  ASSERT_NE(codec_impl, nullptr);
  scoped_refptr<CodecSurfaceBundle> surface_bundle;
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kDraining;
  ohos_video_decoder_->deferred_flush_pending_ = true;
  ohos_video_decoder_->OnCodecDrained();
  EXPECT_NE(ohos_video_decoder_->codec_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_001) {
  ohos_video_decoder_->codec_ = nullptr;
  testing::internal::CaptureStderr();
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OhosVideoDecoder::DequeueOutput failed"), std::string::npos);
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_002) {
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ASSERT_TRUE(ohos_video_decoder_->codec_);
  ohos_video_decoder_->codec_->impl_->state_ =
      CodecWrapperImpl::State::kDrained;
  testing::internal::CaptureStderr();
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OhosVideoDecoder::DequeueOutput failed"),
            std::string::npos);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_003) {
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ASSERT_TRUE(ohos_video_decoder_->codec_);
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  ASSERT_FALSE(ohos_video_decoder_->codec_->IsDrained());
  ohos_video_decoder_->codec_->impl_->elided_eos_pending_ = true;
  testing::internal::CaptureStderr();
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OhosVideoDecoder::DequeueOutput is drained"),
            std::string::npos);
  EXPECT_FALSE(ohos_video_decoder_->drain_type_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_004) {
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  ohos_video_decoder_->codec_->impl_->elided_eos_pending_ = false;
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType::kForReset;
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence)
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(testing::Return(false));
  ohos_video_decoder_->reset_cb_ = base::BindOnce([](void) {});
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  EXPECT_FALSE(ohos_video_decoder_->eos_decode_cb_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_005) {
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  ohos_video_decoder_->codec_->impl_->elided_eos_pending_ = false;
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType::kForReset;
  ohos_video_decoder_->codec_->impl_->codec_->signal_ =
      std::make_shared<DecoderBridgeSignal>();
  testing::internal::CaptureStderr();
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("DequeueOutputBuffer()"), std::string::npos);
  EXPECT_EQ(log_output.find("EnterTerminalState"), std::string::npos);
  EXPECT_FALSE(ohos_video_decoder_->eos_decode_cb_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_006) {
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  ohos_video_decoder_->codec_->impl_->elided_eos_pending_ = false;
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType::kForReset;
  ohos_video_decoder_->codec_->impl_->codec_->signal_ =
      std::make_shared<DecoderBridgeSignal>();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->isDecoderFlushing_ =
      false;
  VideoBridgeDecoderOutputBuffer buffer;
  buffer.outputBufferIndex = 0;
  buffer.outputBufferFlag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  buffer.outputBufferInfo = BufferInfo();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->outputQueue_.push(
      buffer);
  ohos_video_decoder_->codec_->impl_->codec_->videoDecoder_ =
      OhosAdapterHelper::GetInstance().CreateMediaCodecDecoderAdapter();
  EXPECT_TRUE(ohos_video_decoder_->DequeueOutput());
  EXPECT_TRUE(ohos_video_decoder_->drain_type_);
  EXPECT_FALSE(ohos_video_decoder_->deferred_flush_pending_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_007) {
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  ohos_video_decoder_->codec_->impl_->elided_eos_pending_ = false;
  ohos_video_decoder_->codec_->impl_->codec_->signal_ =
      std::make_shared<DecoderBridgeSignal>();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->isDecoderFlushing_ =
      false;
  VideoBridgeDecoderOutputBuffer buffer;
  buffer.outputBufferIndex = 0;
  buffer.outputBufferFlag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  buffer.outputBufferInfo = BufferInfo();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->outputQueue_.push(
      buffer);
  ohos_video_decoder_->codec_->impl_->codec_->videoDecoder_ =
      OhosAdapterHelper::GetInstance().CreateMediaCodecDecoderAdapter();
  ohos_video_decoder_->deferred_flush_pending_ = true;
  EXPECT_TRUE(ohos_video_decoder_->DequeueOutput());
  EXPECT_FALSE(ohos_video_decoder_->drain_type_);
  EXPECT_TRUE(ohos_video_decoder_->deferred_flush_pending_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_008) {
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  ohos_video_decoder_->codec_->impl_->elided_eos_pending_ = false;
  ohos_video_decoder_->codec_->impl_->codec_->signal_ =
      std::make_shared<DecoderBridgeSignal>();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->isDecoderFlushing_ =
      false;
  VideoBridgeDecoderOutputBuffer buffer;
  buffer.outputBufferIndex = 0;
  buffer.outputBufferFlag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  buffer.outputBufferInfo = BufferInfo();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->outputQueue_.push(
      buffer);
  ohos_video_decoder_->codec_->impl_->codec_->videoDecoder_ =
      OhosAdapterHelper::GetInstance().CreateMediaCodecDecoderAdapter();
  ohos_video_decoder_->deferred_flush_pending_ = false;
  EXPECT_CALL(*video_frame_factory_, CreateVideoFrame(_, _, _, _)).Times(1);
  EXPECT_TRUE(ohos_video_decoder_->DequeueOutput());
  EXPECT_FALSE(ohos_video_decoder_->drain_type_);
  EXPECT_FALSE(ohos_video_decoder_->deferred_flush_pending_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest2, DequeueOutput_009) {
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ASSERT_TRUE(ohos_video_decoder_->codec_);
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  ASSERT_FALSE(ohos_video_decoder_->codec_->IsDrained());
  ohos_video_decoder_->codec_->impl_->elided_eos_pending_ = true;
  ohos_video_decoder_->eos_decode_cb_ = base::BindOnce([](DecoderStatus) {});
  ohos_video_decoder_->drain_type_ = OhosVideoDecoder::DrainType::kForReset;
  EXPECT_CALL(*video_frame_factory_, RunAfterPendingVideoFrames(_)).Times(1);
  ohos_video_decoder_->reset_cb_ = base::BindOnce([](void) {});
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  EXPECT_FALSE(ohos_video_decoder_->drain_type_);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, PumpCodec) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;

  ohos_video_decoder_->PumpCodec();
  EXPECT_FALSE(ohos_video_decoder_->QueueInput());
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, PumpCodec1) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ = CodecWrapperImpl::State::kError;
  ohos_video_decoder_->codec_->impl_->elided_eos_pending_ = false;
  ohos_video_decoder_->codec_->impl_->codec_->signal_ =
      std::make_shared<DecoderBridgeSignal>();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->isDecoderFlushing_ =
      false;
  VideoBridgeDecoderOutputBuffer buffer;
  buffer.outputBufferIndex = 0;
  buffer.outputBufferFlag = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  buffer.outputBufferInfo = BufferInfo();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->outputQueue_.push(
      buffer);
  ohos_video_decoder_->codec_->impl_->codec_->videoDecoder_ =
      OhosAdapterHelper::GetInstance().CreateMediaCodecDecoderAdapter();
  ohos_video_decoder_->deferred_flush_pending_ = true;

  ohos_video_decoder_->PumpCodec();
  EXPECT_FALSE(ohos_video_decoder_->QueueInput());
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, PumpCodec2) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());

  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kRunning;
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->pending_decodes_.emplace_front(
      PendingDecode::CreateEos());
  EXPECT_FALSE(ohos_video_decoder_->pending_decodes_.empty());
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence)
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(testing::Return(false));

  ohos_video_decoder_->PumpCodec();
  EXPECT_FALSE(ohos_video_decoder_->QueueInput());
  EXPECT_FALSE(ohos_video_decoder_->DequeueOutput());
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, QueueInput) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  ohos_video_decoder_->codec_ = nullptr;

  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_FALSE(result);
}

TEST_F(OhosVideoDecoderTest3, QueueInput_001) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kDrained;
  ohos_video_decoder_->deferred_flush_pending_ = true;
  ohos_video_decoder_->pending_decodes_.clear();
  EXPECT_TRUE(ohos_video_decoder_->pending_decodes_.empty());

  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_FALSE(result);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, QueueInput_002) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kFlushed;
  ohos_video_decoder_->deferred_flush_pending_ = true;
  ohos_video_decoder_->deferred_reallocation_pending_ = false;
  ohos_video_decoder_->pending_decodes_.emplace_front(
      PendingDecode::CreateEos());
  EXPECT_FALSE(ohos_video_decoder_->pending_decodes_.empty());

  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_TRUE(result);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, QueueInput_003) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());
  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kRunning;
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->pending_decodes_.clear();
  EXPECT_TRUE(ohos_video_decoder_->pending_decodes_.empty());

  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_FALSE(result);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, QueueInput_004_Switch_Kagain) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());

  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kRunning;
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->pending_decodes_.emplace_front(
      PendingDecode::CreateEos());
  EXPECT_FALSE(ohos_video_decoder_->pending_decodes_.empty());
  ohos_video_decoder_->codec_->impl_->codec_->signal_ =
      std::make_shared<DecoderBridgeSignal>();
  ohos_video_decoder_->codec_->impl_->codec_->signal_->isDecoderFlushing_.store(
      true);
  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_FALSE(result);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, QueueInput_005_Switch_Kok) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());

  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kRunning;
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->pending_decodes_.emplace_front(
      PendingDecode::CreateEos());
  EXPECT_FALSE(ohos_video_decoder_->pending_decodes_.empty());

  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_TRUE(result);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, QueueInput_005_Switch_Kok2) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());

  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kRunning;
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->codec_->impl_->codec_->signal_ =
      std::make_unique<DecoderBridgeSignal>();

  auto video_decoder_s = std::make_unique<MockMediaCodecDecoderAdapter>();
  auto test = std::make_unique<uint8_t>();
  uint8_t* addr = test.get();
  VideoBridgeDecoderInputBuffer inputbuffer = {10, {addr, 8}};
  ohos_video_decoder_->codec_->impl_->codec_->signal_->inputQueue_.push(
      inputbuffer);
  EXPECT_CALL(*video_decoder_s,
              QueueInputBufferDec(testing::_, testing::_, testing::_, 0,
                                  BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*video_decoder_s, ReleaseDecoder()).Times(1);
  ohos_video_decoder_->codec_->impl_->codec_->videoDecoder_ =
      std::move(video_decoder_s);

  scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(8);
  ohos_video_decoder_->pending_decodes_.emplace_front(
      PendingDecode(buffer, base::DoNothing()));
  EXPECT_FALSE(ohos_video_decoder_->pending_decodes_.empty());

  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_TRUE(result);
  ohos_video_decoder_->codec_ = nullptr;
}

TEST_F(OhosVideoDecoderTest3, QueueInput_005_Switch_Kerr) {
  ASSERT_NE(ohos_video_decoder_, nullptr);
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<VideoBridgeCodecConfig>();
  config->codec = VideoCodec::kH264;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_impl =
      MediaCodecDecoderBridgeImpl::CreateVideoDecoder(std::move(*config));
  scoped_refptr<CodecSurfaceBundle> surface_bundle =
      base::MakeRefCounted<CodecSurfaceBundle>();
  ohos_video_decoder_->codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec_impl), std::move(surface_bundle)),
      base::DoNothing(), base::SequencedTaskRunner::GetCurrentDefault());

  ohos_video_decoder_->codec_->impl_->state_ =
      media::CodecWrapperImpl::State::kRunning;
  ohos_video_decoder_->deferred_flush_pending_ = false;
  ohos_video_decoder_->codec_->impl_->codec_->signal_ = nullptr;
  scoped_refptr<DecoderBuffer> buffer = base::MakeRefCounted<DecoderBuffer>(8);
  ohos_video_decoder_->pending_decodes_.emplace_front(
      PendingDecode(buffer, base::DoNothing()));
  EXPECT_FALSE(ohos_video_decoder_->pending_decodes_.empty());
  EXPECT_CALL(*task_runner, RunsTasksInCurrentSequence)
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*task_runner, PostDelayedTask).WillOnce(testing::Return(false));

  bool result = ohos_video_decoder_->QueueInput();
  EXPECT_FALSE(result);
  ohos_video_decoder_->codec_ = nullptr;
}

}  // namespace media
