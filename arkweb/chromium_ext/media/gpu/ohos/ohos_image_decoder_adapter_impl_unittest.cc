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

#include <cstring>
#include <memory>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/mman.h>

#define private public
#include "arkweb/ohos_adapter_ndk/ohos_image_adapter/ohos_image_decoder_adapter_impl.h"
#undef private

using namespace testing;

namespace OHOS::NWeb {

class ImageDecoderAdapterImplTest : public testing::Test {
 public:
    void SetUp() override {
      adapter_ = std::make_shared<OhosImageDecoderAdapterImpl>();
    };
    void TearDown() override {
      adapter_.reset();
    };
  protected:
    std::shared_ptr<OhosImageDecoderAdapterImpl> adapter_;
};

static unsigned char g_testPngBuffer[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x01, 0xd8, 0x00, 0x00, 0x00, 0x4b,
  0x08, 0x02, 0x00, 0x00, 0x00, 0x5c, 0x3e, 0x01, 0x97, 0x00, 0x00, 0x00,
  0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xae, 0xce, 0x1c, 0xe9, 0x00, 0x00,
  0x00, 0x04, 0x67, 0x41, 0x4d, 0x41, 0x00, 0x00, 0xb1, 0x8f, 0x0b, 0xfc,
  0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00,
  0x0e, 0xc3, 0x00, 0x00, 0x0e, 0xc3, 0x01, 0xc7, 0x6f, 0xa8, 0x64, 0x00,
  0x00, 0x01, 0x1a, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5e, 0xed, 0xd4, 0x31,
  0x01, 0x00, 0x20, 0x0c, 0xc0, 0xb0, 0x81, 0x7f, 0xcf, 0xe3, 0xc1, 0x43,
  0x9f, 0xe4, 0xa9, 0x83, 0x9e, 0xdd, 0x1d, 0x00, 0x3a, 0xf7, 0x17, 0x80,
  0x88, 0x11, 0x03, 0xc4, 0x8c, 0x18, 0x20, 0x66, 0xc4, 0x00, 0x31, 0x23,
  0x06, 0x88, 0x19, 0x31, 0x40, 0xcc, 0x88, 0x01, 0x62, 0x46, 0x0c, 0x10,
  0x33, 0x62, 0x80, 0x98, 0x11, 0x03, 0xc4, 0x8c, 0x18, 0x20, 0x66, 0xc4,
  0x00, 0x31, 0x23, 0x06, 0x88, 0x19, 0x31, 0x40, 0xcc, 0x88, 0x01, 0x62,
  0x46, 0x0c, 0x10, 0x33, 0x62, 0x80, 0x98, 0x11, 0x03, 0xc4, 0x8c, 0x18,
  0x20, 0x66, 0xc4, 0x00, 0x31, 0x23, 0x06, 0x88, 0x19, 0x31, 0x40, 0xcc,
  0x88, 0x01, 0x62, 0x46, 0x0c, 0x10, 0x33, 0x62, 0x80, 0x98, 0x11, 0x03,
  0xc4, 0x8c, 0x18, 0x20, 0x66, 0xc4, 0x00, 0x31, 0x23, 0x06, 0x88, 0x19,
  0x31, 0x40, 0xcc, 0x88, 0x01, 0x62, 0x46, 0x0c, 0x10, 0x33, 0x62, 0x80,
  0x98, 0x11, 0x03, 0xc4, 0x8c, 0x18, 0x20, 0x66, 0xc4, 0x00, 0x31, 0x23,
  0x06, 0x88, 0x19, 0x31, 0x40, 0xcc, 0x88, 0x01, 0x62, 0x46, 0x0c, 0x10,
  0x33, 0x62, 0x80, 0x98, 0x11, 0x03, 0xc4, 0x8c, 0x18, 0x20, 0x66, 0xc4,
  0x00, 0x31, 0x23, 0x06, 0x88, 0x19, 0x31, 0x40, 0xcc, 0x88, 0x01, 0x62,
  0x46, 0x0c, 0x10, 0x33, 0x62, 0x80, 0x98, 0x11, 0x03, 0xc4, 0x8c, 0x18,
  0x20, 0x66, 0xc4, 0x00, 0x31, 0x23, 0x06, 0x88, 0x19, 0x31, 0x40, 0xcc,
  0x88, 0x01, 0x62, 0x46, 0x0c, 0x10, 0x33, 0x62, 0x80, 0x98, 0x11, 0x03,
  0xc4, 0x8c, 0x18, 0x20, 0x66, 0xc4, 0x00, 0x31, 0x23, 0x06, 0x88, 0x19,
  0x31, 0x40, 0xcc, 0x88, 0x01, 0x62, 0x46, 0x0c, 0x10, 0x33, 0x62, 0x80,
  0x98, 0x11, 0x03, 0xc4, 0x8c, 0x18, 0x20, 0x66, 0xc4, 0x00, 0x31, 0x23,
  0x06, 0x88, 0x19, 0x31, 0x40, 0xcc, 0x88, 0x01, 0x62, 0x46, 0x0c, 0x10,
  0x33, 0x62, 0x80, 0x98, 0x11, 0x03, 0xa4, 0x66, 0x1e, 0x13, 0xed, 0x03,
  0x93, 0xb8, 0xed, 0xb9, 0xac, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
  0x44, 0xae, 0x42, 0x60, 0x82
};
static unsigned int g_testPngLen = 389;
const std::string IMAGE_HEIF = "image/heif";

uint8_t* GetPngBuffer(size_t& bufSize){
  bufSize = g_testPngLen;
  return g_testPngBuffer;
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_NativeBufferFromPixelMap) {
  EXPECT_NE(adapter_, nullptr);
  adapter_->pixelMap_ = nullptr;
  adapter_->NativeBufferFromPixelMap();
  EXPECT_EQ(adapter_->nativeBuffer_, nullptr);
  ASSERT_NO_FATAL_FAILURE(adapter_->NativeBufferFromPixelMap());
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  bool result = adapter_->DecodeToPixelMap(buffer, bufferSize);
  EXPECT_EQ(result, true);
  adapter_->NativeBufferFromPixelMap();
  EXPECT_NE(adapter_->pixelMap_, nullptr);
  EXPECT_NE(adapter_->nativeBuffer_, nullptr);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_ParseRawData) {
  EXPECT_NE(adapter_, nullptr);
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  bool result = adapter_->ParseRawData(buffer, bufferSize);
  EXPECT_EQ(result, true);
  result = adapter_->ParseRawData(nullptr, bufferSize);
  EXPECT_EQ(result, false);
  result = adapter_->ParseRawData(buffer, 0);
  EXPECT_EQ(result, false);
  adapter_->imageInfo_ = nullptr;
  result = adapter_->ParseRawData(buffer, 0);
  EXPECT_EQ(result, false);
  uint8_t *imageData = (uint8_t *)malloc(1024);
  result = adapter_->ParseRawData(imageData, 1024);
  EXPECT_EQ(result, false);
  free(imageData);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_ParseImageInfo) {
  EXPECT_NE(adapter_, nullptr);
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  bool result = adapter_->ParseImageInfo(buffer, bufferSize);
  EXPECT_EQ(result, true);
  result = adapter_->ParseImageInfo(nullptr, bufferSize);
  EXPECT_EQ(result, false);
  result = adapter_->ParseImageInfo(buffer, 0);
  EXPECT_EQ(result, false);
  adapter_->imageInfo_ = nullptr;
  result = adapter_->ParseImageInfo(buffer, 0);
  EXPECT_EQ(result, false);
  uint8_t *imageData = (uint8_t *)malloc(1024);
  result = adapter_->ParseImageInfo(imageData, 1024);
  EXPECT_EQ(result, false);
  free(imageData);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetEncodedFormat) {
  EXPECT_NE(adapter_, nullptr);
  std::string format = adapter_->GetEncodedFormat();
  EXPECT_EQ(format, IMAGE_HEIF);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetImageWidthHeight) {
  EXPECT_NE(adapter_, nullptr);
  int32_t result = adapter_->GetImageWidth();
  EXPECT_TRUE(result == 0);
  adapter_->imageInfo_ = nullptr;
  EXPECT_TRUE(adapter_->imageInfo_ == nullptr);
  adapter_->imageInfo_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter_->GetImageWidth());

  result = adapter_->GetImageHeight();
  EXPECT_TRUE(result == 0);
  adapter_->imageInfo_ = nullptr;
  EXPECT_TRUE(adapter_->imageInfo_ == nullptr);
  adapter_->imageInfo_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter_->GetImageHeight());
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_Decode) {
  EXPECT_NE(adapter_, nullptr);
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  bool result = adapter_->Decode(buffer, bufferSize, AllocatorType::kDmaAlloc, true);
  EXPECT_TRUE(result == true);
  uint8_t *imageData = (uint8_t *)malloc(1024);
  result = adapter_->Decode(imageData, 1024, AllocatorType::kDmaAlloc, true);
  EXPECT_TRUE(result == false);
  free(imageData);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_SetMemoryName) {
  EXPECT_NE(adapter_, nullptr);
  ASSERT_NO_FATAL_FAILURE(adapter_->SetMemoryName(AllocatorType::kDmaAlloc));
  adapter_->pixelMap_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter_->SetMemoryName(AllocatorType::kDmaAlloc));
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetFd) {
  EXPECT_NE(adapter_, nullptr);
  std::shared_ptr<BufferHandle> bufferHandle = std::make_shared<BufferHandle>();
  adapter_->bufferHandle_ = bufferHandle.get();
  int32_t result = adapter_->GetFd();
  std::cout << "result:" << result << std::endl;
  EXPECT_TRUE(result != -1);
  adapter_->bufferHandle_ = nullptr;
  result = adapter_->GetFd();
  EXPECT_EQ(result, -1);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetStride) {
  EXPECT_NE(adapter_, nullptr);
  int32_t result = adapter_->GetStride();
  EXPECT_EQ(result, 0);
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  int32_t ret = adapter_->GetStride();
  EXPECT_NE(ret, 0);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetOffset) {
  EXPECT_NE(adapter_, nullptr);
  int32_t result = adapter_->GetOffset();
  EXPECT_EQ(result, 0);
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  result = adapter_->GetOffset();
  EXPECT_NE(adapter_->pixelMap_, nullptr);
  EXPECT_EQ(result, 0);
  adapter_->nativeBuffer_ = nullptr;
  result = adapter_->GetOffset();
  EXPECT_EQ(result, 0);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetSize) {
  EXPECT_NE(adapter_, nullptr);
  std::shared_ptr<BufferHandle> bufferHandle = std::make_shared<BufferHandle>();
  adapter_->bufferHandle_ = bufferHandle.get();
  uint64_t result = adapter_->GetSize();
  EXPECT_FALSE(result != 0);
  adapter_->bufferHandle_ = nullptr;
  result = adapter_->GetSize();
  EXPECT_EQ(result, 0);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetNativeWindowBuffer) {
  EXPECT_NE(adapter_, nullptr);
  auto res = adapter_->GetNativeWindowBuffer();
  EXPECT_EQ(res, nullptr);
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  res = adapter_->GetNativeWindowBuffer();
  EXPECT_NE(res, nullptr);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetPlanesCount) {
  EXPECT_NE(adapter_, nullptr);
  int32_t result = adapter_->GetPlanesCount();
  EXPECT_EQ(result, 0);
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  result = adapter_->GetPlanesCount();
  EXPECT_EQ(result, 0);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_ReleasePixelMap) {
  EXPECT_NE(adapter_, nullptr);
  ASSERT_NO_FATAL_FAILURE(adapter_->ReleasePixelMap());
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  EXPECT_NE(adapter_->pixelMap_, nullptr);
  bool result = adapter_->ParseRawData(buffer, bufferSize);
  EXPECT_NE(adapter_->imageInfo_, nullptr);
  OH_ImageSourceInfo_Create(&adapter_->imageInfo_);
  EXPECT_NE(adapter_->imageSource_, nullptr);
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  ASSERT_NO_FATAL_FAILURE(adapter_->ReleasePixelMap());
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  adapter_->has_lock_pixelmap_ = true;
  ASSERT_NO_FATAL_FAILURE(adapter_->ReleasePixelMap());
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  adapter_->NativeBufferFromPixelMap();
  EXPECT_NE(adapter_->nativeBuffer_, nullptr);
  adapter_->CreateNativeWindowBuffer();
  EXPECT_NE(adapter_->nativeWindowBuffer_, nullptr);
  adapter_->bufferHandle_ = new BufferHandle();
  EXPECT_NE(adapter_->bufferHandle_, nullptr);
  ASSERT_NO_FATAL_FAILURE(adapter_->ReleasePixelMap());
  EXPECT_EQ(adapter_->imageSource_, nullptr);
  EXPECT_EQ(adapter_->imageInfo_, nullptr);
  EXPECT_EQ(adapter_->pixelMap_, nullptr);
  EXPECT_EQ(adapter_->nativeWindowBuffer_, nullptr);
  EXPECT_EQ(adapter_->nativeBuffer_, nullptr);
  EXPECT_EQ(adapter_->opt_, nullptr);
  EXPECT_EQ(adapter_->bufferHandle_, nullptr);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetBufferHandle) {
  EXPECT_NE(adapter_, nullptr);
  std::shared_ptr<BufferHandle> bufferHandle = std::make_shared<BufferHandle>();
  adapter_->bufferHandle_ = bufferHandle.get();
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  bool result = adapter_->DecodeToPixelMap(buffer, bufferSize);
  EXPECT_EQ(result, true);
  adapter_->bufferHandle_ = nullptr;
  adapter_->NativeBufferFromPixelMap();
  adapter_->CreateNativeWindowBuffer();
  result = adapter_->GetBufferHandle();
  EXPECT_EQ(result, true);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_GetDecodeData) {
  EXPECT_NE(adapter_, nullptr);
  auto res = adapter_->GetDecodeData();
  EXPECT_EQ(res, nullptr);
  size_t bufferSize = 0;
  uint8_t* buffer = GetPngBuffer(bufferSize);
  adapter_->DecodeToPixelMap(buffer, bufferSize);
  res = adapter_->GetDecodeData();
  EXPECT_NE(res, nullptr);
}

TEST_F(ImageDecoderAdapterImplTest, ImageDecoderAdapterImplTest_DecodeByPath) {
  EXPECT_NE(adapter_, nullptr);
  bool result = adapter_->DecodeByPath("", AllocatorType::kDefault);
  EXPECT_EQ(result, false);
  result = adapter_->DecodeByPath("111", AllocatorType::kDefault);
  EXPECT_EQ(result, false);
  adapter_->ReleasePixelMap();
  EXPECT_EQ(adapter_->pixelMap_, nullptr);
}

} // namespace OHOS::NWeb