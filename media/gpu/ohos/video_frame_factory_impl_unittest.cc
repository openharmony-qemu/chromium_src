// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
#include "media/gpu/ohos/codec_buffer_wait_coordinator.h"
#include "media/gpu/ohos/codec_image.h"
#include "media/gpu/ohos/codec_wrapper.cc"
#include "media/gpu/ohos/codec_wrapper.h"
#include "media/gpu/ohos/shared_image_video_provider.h"
#include "media/gpu/ohos/video_frame_factory_impl.h"

using base::test::RunOnceCallback;
using testing::_;
using testing::SaveArg;

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

  base::test::TaskEnvironment task_environment_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  std::unique_ptr<VideoFrameFactoryImpl> impl_;

  raw_ptr<MockSharedImageVideoProvider> image_provider_raw_ = nullptr;
  raw_ptr<CodecOutputBuffer> output_buffer_raw_ = nullptr;

  base::MockCallback<VideoFrameFactory::OnceOutputCB> output_cb_;
  gpu::GpuPreferences gpu_preferences_;
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
      std::move(codec), 0, coded_size, gfx::ColorSpace());
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

}  // namespace media
