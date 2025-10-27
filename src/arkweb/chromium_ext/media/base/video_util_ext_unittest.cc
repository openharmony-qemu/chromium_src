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

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "media/base/video_frame.h"
#include "media/base/encoder_status.h"
#include "third_party/libyuv/include/libyuv.h"
#include "base/time/time.h"
#include "base/logging.h"
#include "arkweb/chromium_ext/media/base/video_util_ext.h"

using namespace testing;

namespace media {

class ConvertAndScaleFrameTest : public testing::Test {
 protected:
  // 创建测试用的源帧
  scoped_refptr<VideoFrame> CreateSourceFrame(
      VideoPixelFormat format,
      const gfx::Size& visible_size,
      const gfx::Size& coded_size = gfx::Size(320, 240)) {
    return VideoFrame::CreateFrame(
        format, coded_size, gfx::Rect(visible_size), visible_size,
        base::TimeDelta::FromInternalValue(0));
  }

  // 创建测试用的目标帧（可写）
  scoped_refptr<VideoFrame> CreateDestFrame(
      VideoPixelFormat format,
      const gfx::Size& visible_size,
      const gfx::Size& coded_size = gfx::Size(640, 480)) {
    return VideoFrame::CreateFrame(
        format, coded_size, gfx::Rect(visible_size), visible_size,
        base::TimeDelta::FromInternalValue(0));
  }

  void SetStorageType (scoped_refptr<VideoFrame> VideoFrame, VideoFrame::StorageType StorageType) {
    (*VideoFrame).storage_type_ = StorageType;
  }

  std::vector<uint8_t> tmp_buf_;  // 临时缓冲区
};

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame001) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_OPAQUE);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame002) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_OPAQUE);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame003) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame004) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame005) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame006) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame007) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame008) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame009) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame010) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame011) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame012) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame013) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame014) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame015) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame016) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame017) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame018) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame019) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame020) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame021) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame022) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame023) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame024) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame025) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame026) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame027) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame028) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame029) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame030) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame031) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame032) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame033) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame034) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame035) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame036) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame037) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame038) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame039) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame040) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame041) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame042) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame043) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame044) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame045) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame046) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame047) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_I420, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame048) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame049) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame050) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame051) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame052) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame053) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame054) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame055) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame056) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame057) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame058) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame059) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame060) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame061) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame062) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame063) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame064) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame065) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame066) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame067) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XRGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame068) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame069) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame070) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame071) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ARGB, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame072) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame073) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame074) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame075) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_XBGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kOk);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame076) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kUnsupportedFrameFormat);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame077) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(tmp_buf_.size(), tmp_buffer_size);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame078) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(640, 480));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_NE(status, EncoderStatus::Codes::kScalingError);
}

TEST_F(ConvertAndScaleFrameTest, ConvertAndScaleFrame079) {
  auto src = CreateSourceFrame(PIXEL_FORMAT_ABGR, gfx::Size(320, 240));
  SetStorageType(src, VideoFrame::STORAGE_UNOWNED_MEMORY);
  auto dst = CreateDestFrame(PIXEL_FORMAT_NV12A, gfx::Size(320, 240));
  SetStorageType(dst, VideoFrame::STORAGE_UNOWNED_MEMORY);

  size_t tmp_buffer_size = VideoFrame::AllocationSize(
          (*src).format(), (*dst).coded_size());

  EncoderStatus status = ConvertAndScaleFrame(*src, *dst, tmp_buf_);
  EXPECT_EQ(status, EncoderStatus::Codes::kOk);
}
}  // namespace media