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
#include "media/gpu/ohos/ohos_heif_image_decoder.h"
#undef private
#include "base/files/scoped_file.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "ui/gfx/linux/native_pixmap_dmabuf.h"

namespace media {

class MockOhosImageDecoderAdapter : public OHOS::NWeb::OhosImageDecoderAdapter {
 public:
  MOCK_METHOD(bool, ParseImageInfo, (const uint8_t* data, uint32_t size), ());
  MOCK_METHOD(std::string, GetEncodedFormat, (), ());
  MOCK_METHOD(int32_t, GetImageWidth, (), ());
  MOCK_METHOD(int32_t, GetImageHeight, (), ());
  MOCK_METHOD(bool, DecodeToPixelMap, (const uint8_t*, uint32_t), ());
  MOCK_METHOD(int32_t, GetFd, (), ());
  MOCK_METHOD(int32_t, GetStride, (), ());
  MOCK_METHOD(int32_t, GetOffset, (), ());
  MOCK_METHOD(uint64_t, GetSize, (), ());
  MOCK_METHOD(void*, GetNativeWindowBuffer, (), ());
  MOCK_METHOD(int32_t, GetPlanesCount, (), ());
  MOCK_METHOD(void, ReleasePixelMap, (), ());
  MOCK_METHOD(bool,
              Decode,
              (const uint8_t*, uint32_t, OHOS::NWeb::AllocatorType, bool),
              ());
  MOCK_METHOD(void*, GetDecodeData, (), ());
};

class MockOhosHeifImageDecoder : public OhosHeifImageDecoder {
 public:
  MOCK_METHOD(bool, IsYuvFormat, (), (const override));
};

class OhosHeifImageDecoderTest : public ::testing::Test,
                                 public OhosHeifImageDecoder {
 public:
  std::unique_ptr<MockOhosHeifImageDecoder> decoder_ =
      std::make_unique<MockOhosHeifImageDecoder>();
};

TEST_F(OhosHeifImageDecoderTest, ExportAsNativePixmapDmaBuf_001) {
  std::unique_ptr<MockOhosImageDecoderAdapter> adapter =
      std::make_unique<MockOhosImageDecoderAdapter>();
  EXPECT_CALL(*adapter, GetNativeWindowBuffer())
      .WillRepeatedly(testing::Return(nullptr));
  decoder_->OhosImageDecoderAdapter_ = std::move(adapter);
  std::unique_ptr<OhosImageDecodeStatus> status =
      std::make_unique<OhosImageDecodeStatus>();
  decoder_->ExportAsNativePixmapDmaBuf(status.get());
  EXPECT_EQ(decoder_->OhosImageDecoderAdapter_->GetNativeWindowBuffer(),
            nullptr);
}

TEST_F(OhosHeifImageDecoderTest, ExportAsNativePixmapDmaBuf_002) {
  std::unique_ptr<MockOhosImageDecoderAdapter> adapter =
      std::make_unique<MockOhosImageDecoderAdapter>();
  EXPECT_CALL(*adapter, GetStride()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetOffset()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetSize()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageWidth()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageHeight()).Times(testing::AnyNumber());
  const char* buffer = "abc";
  EXPECT_CALL(*adapter, GetNativeWindowBuffer())
      .WillRepeatedly(testing::Return(reinterpret_cast<void*>(&buffer)));
  EXPECT_CALL(*adapter, GetPlanesCount()).WillRepeatedly(testing::Return(-1));
  EXPECT_CALL(*decoder_, IsYuvFormat()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*adapter, GetFd()).WillRepeatedly(testing::Return(-1));
  decoder_->OhosImageDecoderAdapter_ = std::move(adapter);
  std::unique_ptr<OhosImageDecodeStatus> status =
      std::make_unique<OhosImageDecodeStatus>();
  decoder_->ExportAsNativePixmapDmaBuf(status.get());
  EXPECT_EQ(decoder_->OhosImageDecoderAdapter_->GetNativeWindowBuffer(),
            reinterpret_cast<void*>(&buffer));
}

TEST_F(OhosHeifImageDecoderTest, ExportAsNativePixmapDmaBuf_003) {
  std::unique_ptr<MockOhosImageDecoderAdapter> adapter =
      std::make_unique<MockOhosImageDecoderAdapter>();
  EXPECT_CALL(*adapter, GetSize()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageWidth()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageHeight()).Times(testing::AnyNumber());
  const char* buffer = "abc";
  EXPECT_CALL(*adapter, GetNativeWindowBuffer())
      .WillRepeatedly(testing::Return(reinterpret_cast<void*>(&buffer)));
  EXPECT_CALL(*decoder_, IsYuvFormat()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(*adapter, GetPlanesCount()).WillRepeatedly(testing::Return(-1));
  EXPECT_CALL(*adapter, GetFd()).WillRepeatedly(testing::Return(-1));
  decoder_->OhosImageDecoderAdapter_ = std::move(adapter);
  std::unique_ptr<OhosImageDecodeStatus> status =
      std::make_unique<OhosImageDecodeStatus>();
  decoder_->ExportAsNativePixmapDmaBuf(status.get());
  EXPECT_TRUE(decoder_->IsYuvFormat());
}

TEST_F(OhosHeifImageDecoderTest, ExportAsNativePixmapDmaBuf_004) {
  std::unique_ptr<MockOhosImageDecoderAdapter> adapter =
      std::make_unique<MockOhosImageDecoderAdapter>();
  EXPECT_CALL(*adapter, GetStride()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetOffset()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetSize()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageWidth()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageHeight()).Times(testing::AnyNumber());
  const char* buffer = "abc";
  EXPECT_CALL(*adapter, GetNativeWindowBuffer())
      .WillRepeatedly(testing::Return(reinterpret_cast<void*>(&buffer)));
  EXPECT_CALL(*decoder_, IsYuvFormat()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*adapter, GetPlanesCount()).WillRepeatedly(testing::Return(-1));
  EXPECT_CALL(*adapter, GetFd()).WillRepeatedly(testing::Return(-1));
  decoder_->OhosImageDecoderAdapter_ = std::move(adapter);
  std::unique_ptr<OhosImageDecodeStatus> status =
      std::make_unique<OhosImageDecodeStatus>();
  decoder_->ExportAsNativePixmapDmaBuf(status.get());
  EXPECT_FALSE(decoder_->IsYuvFormat());
}

TEST_F(OhosHeifImageDecoderTest, ExportAsNativePixmapDmaBuf_005) {
  std::unique_ptr<MockOhosImageDecoderAdapter> adapter =
      std::make_unique<MockOhosImageDecoderAdapter>();
  EXPECT_CALL(*adapter, GetSize()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageWidth()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageHeight()).Times(testing::AnyNumber());
  const char* buffer = "abc";
  EXPECT_CALL(*adapter, GetNativeWindowBuffer())
      .WillRepeatedly(testing::Return(reinterpret_cast<void*>(&buffer)));
  EXPECT_CALL(*decoder_, IsYuvFormat()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(*adapter, GetPlanesCount()).WillRepeatedly(testing::Return(-1));
  EXPECT_CALL(*adapter, GetFd()).WillRepeatedly(testing::Return(-1));
  decoder_->OhosImageDecoderAdapter_ = std::move(adapter);
  std::unique_ptr<OhosImageDecodeStatus> status =
      std::make_unique<OhosImageDecodeStatus>();
  decoder_->ExportAsNativePixmapDmaBuf(status.get());
  EXPECT_EQ(decoder_->IsYuvFormat()
                ? decoder_->GetOhosImageDecoderAdapter()->GetPlanesCount()
                : 1,
            -1);
}

TEST_F(OhosHeifImageDecoderTest, ExportAsNativePixmapDmaBuf_006) {
  std::unique_ptr<MockOhosImageDecoderAdapter> adapter =
      std::make_unique<MockOhosImageDecoderAdapter>();
  EXPECT_CALL(*adapter, GetStride()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetOffset()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetSize()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageWidth()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageHeight()).Times(testing::AnyNumber());
  const char* buffer = "abc";
  EXPECT_CALL(*adapter, GetNativeWindowBuffer())
      .WillRepeatedly(testing::Return(reinterpret_cast<void*>(&buffer)));
  EXPECT_CALL(*decoder_, IsYuvFormat()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*adapter, GetPlanesCount()).WillRepeatedly(testing::Return(-1));
  EXPECT_CALL(*adapter, GetFd()).WillRepeatedly(testing::Return(-1));
  decoder_->OhosImageDecoderAdapter_ = std::move(adapter);
  std::unique_ptr<OhosImageDecodeStatus> status =
      std::make_unique<OhosImageDecodeStatus>();
  decoder_->ExportAsNativePixmapDmaBuf(status.get());
  EXPECT_EQ(decoder_->IsYuvFormat()
                ? decoder_->GetOhosImageDecoderAdapter()->GetPlanesCount()
                : 1,
            1);
}

TEST_F(OhosHeifImageDecoderTest, ExportAsNativePixmapDmaBuf_007) {
  std::unique_ptr<MockOhosImageDecoderAdapter> adapter =
      std::make_unique<MockOhosImageDecoderAdapter>();
  EXPECT_CALL(*adapter, GetSize()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageWidth()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageHeight()).Times(testing::AnyNumber());
  const char* buffer = "abc";
  EXPECT_CALL(*adapter, GetNativeWindowBuffer())
      .WillRepeatedly(testing::Return(reinterpret_cast<void*>(&buffer)));
  EXPECT_CALL(*decoder_, IsYuvFormat()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(*adapter, GetPlanesCount()).WillRepeatedly(testing::Return(-1));
  EXPECT_CALL(*adapter, GetFd()).WillRepeatedly(testing::Return(-1));
  decoder_->OhosImageDecoderAdapter_ = std::move(adapter);
  std::unique_ptr<OhosImageDecodeStatus> status =
      std::make_unique<OhosImageDecodeStatus>();
  decoder_->ExportAsNativePixmapDmaBuf(status.get());
  EXPECT_EQ(decoder_->IsYuvFormat() ? gfx::BufferFormat::YUV_420_BIPLANAR
                                    : gfx::BufferFormat::RGBA_8888,
            gfx::BufferFormat::YUV_420_BIPLANAR);
}

TEST_F(OhosHeifImageDecoderTest, ExportAsNativePixmapDmaBuf_008) {
  std::unique_ptr<MockOhosImageDecoderAdapter> adapter =
      std::make_unique<MockOhosImageDecoderAdapter>();
  EXPECT_CALL(*adapter, GetStride()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetOffset()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetSize()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageWidth()).Times(testing::AnyNumber());
  EXPECT_CALL(*adapter, GetImageHeight()).Times(testing::AnyNumber());
  const char* buffer = "abc";
  EXPECT_CALL(*adapter, GetNativeWindowBuffer())
      .WillRepeatedly(testing::Return(reinterpret_cast<void*>(&buffer)));
  EXPECT_CALL(*decoder_, IsYuvFormat()).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(*adapter, GetPlanesCount()).WillRepeatedly(testing::Return(-1));
  EXPECT_CALL(*adapter, GetFd()).WillRepeatedly(testing::Return(-1));
  decoder_->OhosImageDecoderAdapter_ = std::move(adapter);
  std::unique_ptr<OhosImageDecodeStatus> status =
      std::make_unique<OhosImageDecodeStatus>();
  decoder_->ExportAsNativePixmapDmaBuf(status.get());
  EXPECT_EQ(decoder_->IsYuvFormat() ? gfx::BufferFormat::YUV_420_BIPLANAR
                                    : gfx::BufferFormat::RGBA_8888,
            gfx::BufferFormat::RGBA_8888);
}
}  // namespace media
