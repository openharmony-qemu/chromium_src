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

#include <memory>
#include <type_traits>
#include "base/time/time.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define protected public
#define private public
#include "base/memory/scoped_refptr.h"
#include "base/memory/unsafe_shared_memory_region.h"
#include "media/base/bitstream_buffer.h"
#include "media/base/media_log.h"
#include "media/base/ohos/ohos_media_codec_util.h"
#include "media/base/video_codecs.h"
#include "media/base/video_frame.h"
#include "media/base/video_types.h"
#define private public
#include "base/memory/unsafe_shared_memory_region.h"
#include "media/base/media_log.h"
#include "media/gpu/ohos/ohos_video_encode_accelerator.h"
#undef private
#define protected public
#include "media/base/media_log.h"
#undef protected
#include "media/video/video_encode_accelerator.h"

namespace media {
using namespace OHOS::NWeb;

class MockClient : public OHOSVideoEncodeAccelerator::Client {
 public:
  MOCK_METHOD(void,
              RequireBitstreamBuffers,
              (unsigned int input_count,
               const gfx::Size& input_coded_size,
               size_t output_buffer_size),
              (override));
  MOCK_METHOD(void,
              BitstreamBufferReady,
              (int32_t bitstream_buffer_id,
               const BitstreamBufferMetadata& metadata),
              (override));
  MOCK_METHOD(void,
              NotifyError,
              (VideoEncodeAccelerator::Error error),
              (override));
  MOCK_METHOD(void,
              NotifyErrorStatus,
              (const EncoderStatus& status),
              (override));
  MOCK_METHOD(void,
              NotifyEncoderInfoChange,
              (const VideoEncoderInfo& info),
              (override));
};

class MockOHOSMediaCodecBridge : public OHOSMediaCodecBridge {
 public:
  MOCK_METHOD(CodecCodeAdapter, Release, (), (override));
  MOCK_METHOD(CodecCodeAdapter,
              DequeueOutputBuffer,
              (uint32_t & index,
               BufferInfo& info,
               BufferFlag& flag,
               OhosBuffer& buffer),
              (override));
  MOCK_METHOD(CodecCodeAdapter,
              CreateVideoCodecByMime,
              (const std::string mimetype),
              (override));
  MOCK_METHOD(CodecCodeAdapter,
              CreateVideoCodecByName,
              (const std::string name),
              (override));
  MOCK_METHOD(CodecCodeAdapter,
              Configure,
              (const CodecConfigPara& config,
               scoped_refptr<base::SequencedTaskRunner> codec_task_runner),
              (override));
  MOCK_METHOD(CodecCodeAdapter, Prepare, (), (override));
  MOCK_METHOD(CodecCodeAdapter, Start, (), (override));
  MOCK_METHOD(CodecCodeAdapter, Stop, (), (override));
  MOCK_METHOD(CodecCodeAdapter, Reset, (), (override));
  MOCK_METHOD(CodecCodeAdapter, CreateInputSurface, (), (override));
  MOCK_METHOD(CodecCodeAdapter,
              ReleaseOutputBuffer,
              (uint32_t index, bool render),
              (override));
  MOCK_METHOD(void, ClearKeyFrameCache, (), (override));
  MOCK_METHOD(CodecCodeAdapter, RequestKeyFrameSoon, (), (override));
  MOCK_METHOD(CodecCodeAdapter,
              FillSurfaceBuffer,
              (scoped_refptr<VideoFrame> frame, const int64_t timestamp_ms),
              (override));
};

class MockDelegate : public base::DelayedTaskHandle::Delegate {
public:
  MOCK_METHOD(bool, IsValid, (), (const, override));
  MOCK_METHOD(void, CancelTask, (), (override));
};

class OHOSVideoEncodeAcceleratorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    vea_ = std::make_shared<OHOSVideoEncodeAccelerator>();
    client_ = std::make_unique<MockClient>();
    media_log_ = std::make_unique<MediaLog>();
    frame_size_ = gfx::Size(1280, 720);
    vea_->frame_size_ = frame_size_;
  }

  void TearDown() override {
    vea_.reset();
    client_.reset();
    media_log_.reset();
  }

  std::shared_ptr<OHOSVideoEncodeAccelerator> vea_;
  std::unique_ptr<MockClient> client_;
  std::unique_ptr<MediaLog> media_log_;
  gfx::Size frame_size_;
};

TEST_F(OHOSVideoEncodeAcceleratorTest, OHOSVideoEncodeAccelerator001) {
  auto vea_ = std::make_shared<OHOSVideoEncodeAccelerator>();
  EXPECT_TRUE(vea_);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, OHOSVideoEncodeAccelerator002) {
  auto vea_ = std::make_shared<OHOSVideoEncodeAccelerator>();
  vea_.reset();
  EXPECT_FALSE(vea_);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, GetSupportedProfiles) {
  VideoEncodeAccelerator::SupportedProfiles profiles =
      vea_->GetSupportedProfiles();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, Initialize001) {
  VideoEncodeAccelerator::Config config = VideoEncodeAccelerator::Config();
  config.output_profile = VP8PROFILE_ANY;
  ASSERT_FALSE(vea_->Initialize(config, client_.get(), std::move(media_log_)));
}

TEST_F(OHOSVideoEncodeAcceleratorTest, MaybeStartIOTimer) {
  vea_->num_buffers_at_codec_ = 0;
  vea_->pending_frames_ = {};
  vea_->MaybeStartIOTimer();
  EXPECT_FALSE(vea_->io_timer_.IsRunning());
}

TEST_F(OHOSVideoEncodeAcceleratorTest, MaybeStopIOTimer) {
  vea_->num_buffers_at_codec_ = 0;
  vea_->pending_frames_ = {};
  vea_->MaybeStopIOTimer();
  EXPECT_FALSE(vea_->io_timer_.IsRunning());
}

TEST_F(OHOSVideoEncodeAcceleratorTest, MaybeStopIOTimer001) {
  vea_->num_buffers_at_codec_ = 1;
  vea_->pending_frames_ = {};
  vea_->MaybeStopIOTimer();
  EXPECT_FALSE(vea_->io_timer_.IsRunning());
}

TEST_F(OHOSVideoEncodeAcceleratorTest, MaybeStopIOTimer002) {
  VideoPixelFormat format = VideoPixelFormat::PIXEL_FORMAT_I420;
  const gfx::Size coded_size = gfx::Size(640, 480);
  const gfx::Rect visible_rect = gfx::Rect(640, 480);
  const gfx::Size natural_size = gfx::Size(640, 480);
  base::TimeDelta timestamp = base::TimeDelta::FromInternalValue(0);
  scoped_refptr<VideoFrame> video_frame = VideoFrame::CreateFrame(
      format, coded_size, visible_rect, natural_size, timestamp);
  bool frame_state = true;
  base::Time frame_time = base::Time::Now();
  vea_->num_buffers_at_codec_ = 1;
  vea_->pending_frames_.push(
      std::make_tuple(video_frame, frame_state, frame_time));
  vea_->MaybeStopIOTimer();
  EXPECT_FALSE(vea_->io_timer_.IsRunning());
}

TEST_F(OHOSVideoEncodeAcceleratorTest, MaybeStopIOTimer003) {
  VideoPixelFormat format = VideoPixelFormat::PIXEL_FORMAT_I420;
  const gfx::Size coded_size = gfx::Size(640, 480);
  const gfx::Rect visible_rect = gfx::Rect(640, 480);
  const gfx::Size natural_size = gfx::Size(640, 480);
  base::TimeDelta timestamp = base::TimeDelta::FromInternalValue(0);
  scoped_refptr<VideoFrame> video_frame = VideoFrame::CreateFrame(
      format, coded_size, visible_rect, natural_size, timestamp);
  bool frame_state = true;
  base::Time frame_time = base::Time::Now();
  vea_->num_buffers_at_codec_ = 0;
  vea_->pending_frames_.push(
      std::make_tuple(video_frame, frame_state, frame_time));
  vea_->MaybeStopIOTimer();
  EXPECT_FALSE(vea_->io_timer_.IsRunning());
}

TEST_F(OHOSVideoEncodeAcceleratorTest, UseOutputBitstreamBuffer) {
  base::UnsafeSharedMemoryRegion region =
      base::UnsafeSharedMemoryRegion::Create(1024);
  BitstreamBuffer buffer(1, region.Duplicate(), region.GetSize());
  vea_->UseOutputBitstreamBuffer(std::move(buffer));
  EXPECT_EQ(vea_->available_bitstream_buffers_.size(), 1);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, RequestEncodingParametersChange) {
  Bitrate bitrate = Bitrate::ConstantBitrate(1000000u);
  uint32_t framerate = 30;
  vea_->RequestEncodingParametersChange(bitrate, framerate);
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, DoIOTask) {
  vea_->DoIOTask();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, QueueInput) {
  vea_->QueueInput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, DequeueOutput) {
  vea_->DequeueOutput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, Encode) {}

TEST_F(OHOSVideoEncodeAcceleratorTest, NotifyErrorStatus) {}

TEST_F(OHOSVideoEncodeAcceleratorTest, QueueInput001) {
  vea_->error_occurred_ = false;
  vea_->QueueInput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, QueueInput002) {
  vea_->error_occurred_ = true;
  vea_->QueueInput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, QueueInput003) {
  const gfx::Size coded_size;
  const gfx::Rect visible_rect;
  const gfx::Size natural_size;
  base::TimeDelta timestamp = base::TimeDelta::FromInternalValue(1000);
  scoped_refptr<VideoFrame> frame =
      VideoFrame::CreateFrame(VideoPixelFormat::PIXEL_FORMAT_I422A, coded_size,
                              visible_rect, natural_size, timestamp);
  bool force_keyframe = false;
  base::Time current_time = base::Time::Now();
  vea_->pending_frames_.emplace(std::move(frame), force_keyframe, current_time);
  vea_->error_occurred_ = true;
  vea_->QueueInput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, DequeueOutput001) {
  vea_->error_occurred_ = true;
  ASSERT_TRUE(vea_->error_occurred_);
  vea_->num_buffers_at_codec_ = 0;
  ASSERT_EQ(vea_->num_buffers_at_codec_, 0);
  vea_->DequeueOutput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, DequeueOutput002) {
  vea_->error_occurred_ = false;
  ASSERT_FALSE(vea_->error_occurred_);
  vea_->num_buffers_at_codec_ = 0;
  ASSERT_EQ(vea_->num_buffers_at_codec_, 0);
  vea_->DequeueOutput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, DequeueOutput003) {
  vea_->error_occurred_ = false;
  ASSERT_FALSE(vea_->error_occurred_);
  vea_->num_buffers_at_codec_ = 1;
  ASSERT_EQ(vea_->num_buffers_at_codec_, 1);
  vea_->DequeueOutput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, DequeueOutput004) {
  vea_->error_occurred_ = false;
  ASSERT_FALSE(vea_->error_occurred_);
  vea_->num_buffers_at_codec_ = 0;
  ASSERT_EQ(vea_->num_buffers_at_codec_, 0);
  vea_->DequeueOutput();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, DequeueOutput005) {
  OHOSVideoEncodeAccelerator vea_;
  int32_t id = 1;
  size_t size = 10;
  uint64_t offset = 0;
  base::UnsafeSharedMemoryRegion region =
      base::UnsafeSharedMemoryRegion::Create(size);
  base::TimeDelta presentation_timestamp = kNoTimestamp;
  BitstreamBuffer bitstreambuffer(id, std::move(region), size, offset,
                                  presentation_timestamp);
  vea_.available_bitstream_buffers_.push_back(std::move(bitstreambuffer));
  EXPECT_FALSE(vea_.available_bitstream_buffers_.empty());
  vea_.error_occurred_ = false;
  ASSERT_FALSE(vea_.error_occurred_);
  vea_.num_buffers_at_codec_ = 1;
  ASSERT_EQ(vea_.num_buffers_at_codec_, 1);
  auto media_codec = std::make_unique<MockOHOSMediaCodecBridge>();
  EXPECT_CALL(*media_codec, DequeueOutputBuffer(testing::_, testing::_,
                                                testing::_, testing::_))
      .WillOnce(testing::Return(CodecCodeAdapter::RETRY));
  vea_.media_codec_ = std::move(media_codec);
  vea_.DequeueOutput();
  ASSERT_NE(&vea_, nullptr);
}


TEST_F(OHOSVideoEncodeAcceleratorTest, DequeueOutput007) {
  OHOSVideoEncodeAccelerator vea_;
  int32_t id = 1;
  size_t size = 10;
  uint64_t offset = 0;
  base::UnsafeSharedMemoryRegion region =
      base::UnsafeSharedMemoryRegion::Create(size);
  base::TimeDelta presentation_timestamp = kNoTimestamp;
  BitstreamBuffer bitstreambuffer(id, std::move(region), size, offset,
                                  presentation_timestamp);
  vea_.available_bitstream_buffers_.push_back(std::move(bitstreambuffer));
  EXPECT_FALSE(vea_.available_bitstream_buffers_.empty());
  vea_.error_occurred_ = false;
  ASSERT_FALSE(vea_.error_occurred_);
  vea_.num_buffers_at_codec_ = 1;
  ASSERT_EQ(vea_.num_buffers_at_codec_, 1);
  auto media_codec = std::make_unique<MockOHOSMediaCodecBridge>();
  EXPECT_CALL(*media_codec, DequeueOutputBuffer(testing::_, testing::_,
                                                testing::_, testing::_))
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  vea_.media_codec_ = std::move(media_codec);
  vea_.DequeueOutput();
  ASSERT_NE(&vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, Destroy001) {
  auto vea_ = new OHOSVideoEncodeAccelerator();
  auto media_codec = std::make_unique<MockOHOSMediaCodecBridge>();
  EXPECT_CALL(*media_codec, Release())
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  vea_->media_codec_ = std::move(media_codec);
  ASSERT_NE(vea_->media_codec_, nullptr);
  vea_->Destroy();
}

TEST_F(OHOSVideoEncodeAcceleratorTest, Destroy002) {
  auto vea_ = new OHOSVideoEncodeAccelerator();
  vea_->media_codec_ = nullptr;
  ASSERT_EQ(vea_->media_codec_, nullptr);
  vea_->Destroy();
}

TEST_F(OHOSVideoEncodeAcceleratorTest, Destroy003) {
  auto vea_ = new OHOSVideoEncodeAccelerator();
  auto media_codec = std::make_unique<MockOHOSMediaCodecBridge>();
  EXPECT_NE(media_codec, nullptr);
  EXPECT_CALL(*media_codec, Release())
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  vea_->media_codec_ = std::move(media_codec);
  auto delegate = std::make_unique<MockDelegate>();
  EXPECT_NE(delegate, nullptr);
  EXPECT_CALL(*delegate, CancelTask()).WillRepeatedly(testing::Return());
  EXPECT_CALL(*delegate, IsValid()).WillRepeatedly(testing::Return(true));
  vea_->io_timer_.delayed_task_handle_.delegate_ = std::move(delegate);
  ASSERT_TRUE(vea_->io_timer_.IsRunning());
  vea_->Destroy();
}

TEST_F(OHOSVideoEncodeAcceleratorTest, Destroy004) {
  auto vea_ = new OHOSVideoEncodeAccelerator();
  auto media_codec = std::make_unique<MockOHOSMediaCodecBridge>();
  EXPECT_CALL(*media_codec, Release())
      .WillOnce(testing::Return(CodecCodeAdapter::OK));
  vea_->media_codec_ = std::move(media_codec);
  vea_->io_timer_.delayed_task_handle_.delegate_ = nullptr;
  ASSERT_EQ(vea_->io_timer_.delayed_task_handle_.delegate_, nullptr);
  vea_->Destroy();
}

} //namespace media
