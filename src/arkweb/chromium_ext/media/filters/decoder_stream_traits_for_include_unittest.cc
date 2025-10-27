/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "media/base/mock_filters.h"
#define protected public
#include "media/base/media_log.h"
#undef protected
#include "media/filters/decoder_stream_traits.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {

class DecoderStreamTraitsForIncludeTest : public testing::Test {
 public:
  void SetUp() override {
    media_log_ = std::make_unique<MediaLog>();
    video_traits_ = std::make_unique<DecoderStreamTraits<DemuxerStream::VIDEO>>(media_log_.get());
    mock_video_decoder_ = std::make_unique<testing::NiceMock<MockVideoDecoder>>();
  }

  void TearDown() override {
    media_log_.reset();
    video_traits_.reset();
    mock_video_decoder_.reset();
  }

  std::unique_ptr<DecoderStreamTraits<DemuxerStream::VIDEO>> video_traits_;
  std::unique_ptr<MediaLog> media_log_;
  std::unique_ptr<testing::NiceMock<MockVideoDecoder>> mock_video_decoder_;
};

TEST_F(DecoderStreamTraitsForIncludeTest, PipEnable) {
  EXPECT_CALL(*mock_video_decoder_, PipEnable(true)).Times(0);
  ASSERT_NO_FATAL_FAILURE(video_traits_->PipEnable(nullptr, true));
  EXPECT_CALL(*mock_video_decoder_, PipEnable(true)).Times(1);
  ASSERT_NO_FATAL_FAILURE(video_traits_->PipEnable(mock_video_decoder_.get(), true));
}

TEST_F(DecoderStreamTraitsForIncludeTest, RecycleDmaBuffer) {
  EXPECT_CALL(*mock_video_decoder_, RecycleDmaBuffer()).Times(0);
  ASSERT_NO_FATAL_FAILURE(video_traits_->RecycleDmaBuffer(nullptr));
  EXPECT_CALL(*mock_video_decoder_, RecycleDmaBuffer()).Times(1);
  ASSERT_NO_FATAL_FAILURE(video_traits_->RecycleDmaBuffer(mock_video_decoder_.get()));
}

TEST_F(DecoderStreamTraitsForIncludeTest, ResumeDmaBuffer) {
  EXPECT_CALL(*mock_video_decoder_, ResumeDmaBuffer()).Times(0);
  ASSERT_NO_FATAL_FAILURE(video_traits_->ResumeDmaBuffer(nullptr));
  EXPECT_CALL(*mock_video_decoder_, ResumeDmaBuffer()).Times(1);
  ASSERT_NO_FATAL_FAILURE(video_traits_->ResumeDmaBuffer(mock_video_decoder_.get()));
}

}  // namespace media