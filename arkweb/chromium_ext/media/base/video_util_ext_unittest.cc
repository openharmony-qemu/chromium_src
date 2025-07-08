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

}  // namespace media