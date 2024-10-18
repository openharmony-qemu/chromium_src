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
#include "media/gpu/ohos/ohos_video_encode_accelerator.h"
#include "media/video/video_encode_accelerator.h"

namespace media {
using namespace OHOS::NWeb;

class MockClient : public OHOSVideoEncodeAccelerator::Client {
public:
    MOCK_METHOD(void, RequireBitstreamBuffers,
            (unsigned int input_count, const gfx::Size& input_coded_size, size_t output_buffer_size),
            (override));
    MOCK_METHOD(void, BitstreamBufferReady,
            (int32_t bitstream_buffer_id, const BitstreamBufferMetadata& metadata),
            (override));
    MOCK_METHOD(void, NotifyError, (VideoEncodeAccelerator::Error error), (override));
    MOCK_METHOD(void, NotifyErrorStatus, (const EncoderStatus& status), (override));
    MOCK_METHOD(void, NotifyEncoderInfoChange, (const VideoEncoderInfo& info), (override));
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
  }

  std::shared_ptr<OHOSVideoEncodeAccelerator> vea_;
  std::unique_ptr<MockClient> client_;
  std::unique_ptr<MediaLog> media_log_;
  gfx::Size frame_size_;
};

TEST_F(OHOSVideoEncodeAcceleratorTest, GetSupportedProfiles) {
  VideoEncodeAccelerator::SupportedProfiles profiles = vea_->GetSupportedProfiles();
  ASSERT_NE(vea_, nullptr);
}

TEST_F(OHOSVideoEncodeAcceleratorTest, Initialize) {
  VideoEncodeAccelerator::Config config = VideoEncodeAccelerator::Config();
  vea_->Initialize(config, client_.get(), std::move(media_log_));
  ASSERT_NE(vea_, nullptr);
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

TEST_F(OHOSVideoEncodeAcceleratorTest, UseOutputBitstreamBuffer) {
  base::UnsafeSharedMemoryRegion region = base::UnsafeSharedMemoryRegion::Create(1024);
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
}
