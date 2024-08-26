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
#include <gtest/gtest.h>
#define private public
#include "buffer_request_config_adapter_impl.h"
#undef private

using namespace testing;

namespace OHOS::NWeb {

namespace {
std::shared_ptr<BufferRequestConfigAdapterImpl> r_adapter;
}

class BufferRequestConfigAdapterImplTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};

void BufferRequestConfigAdapterImplTest::SetUpTestCase(void) {}
void BufferRequestConfigAdapterImplTest::TearDownTestCase(void) {}
void BufferRequestConfigAdapterImplTest::SetUp(void) {
  r_adapter = std::make_shared<BufferRequestConfigAdapterImpl>();
  ASSERT_NE(r_adapter, nullptr);
}

void BufferRequestConfigAdapterImplTest::TearDown(void) {
  r_adapter = nullptr;
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_001) {
  int32_t width = 0;
  r_adapter->width_ = width;
  auto ret = r_adapter->GetWidth();
  EXPECT_EQ(ret, width);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_002) {
  const int32_t height = 720;
  r_adapter->height_ = height;
  auto ret = r_adapter->GetHeight();
  EXPECT_EQ(ret, height);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_003) {
  const int32_t stride_alignment = 128;
  r_adapter->stride_alignment_ = stride_alignment;
  auto ret = r_adapter->GetStrideAlignment();
  EXPECT_EQ(ret, stride_alignment);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_004) {
  int32_t format = 1;
  r_adapter->format_ = format;
  auto ret = r_adapter->GetFormat();
  EXPECT_EQ(ret, format);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_005) {
  const uint64_t usage = 0x1234567890ABCDEF;
  r_adapter->usage_ = usage;
  auto ret = r_adapter->GetUsage();
  EXPECT_EQ(ret, usage);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_006) {
  const int32_t timeout = 10000;
  r_adapter->timeout_ = timeout;
  auto ret = r_adapter->GetTimeout();
  EXPECT_EQ(ret, timeout);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_007) {
  ColorGamutAdapter color_gamut = ColorGamutAdapter::INVALID;
  r_adapter->color_gamut_ = color_gamut;
  auto ret = r_adapter->GetColorGamut();
  EXPECT_EQ(ret, color_gamut);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_008) {
  TransformTypeAdapter transform_type = TransformTypeAdapter::ROTATE_NONE;
  r_adapter->transform_type_ = transform_type;
  auto ret = r_adapter->GetTransformType();
  EXPECT_EQ(ret, transform_type);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_009) {
  const int32_t width = 1280;
  r_adapter->SetWidth(width);
  auto ret = r_adapter->GetWidth();
  EXPECT_EQ(ret, width);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_010) {
  const int32_t height = 720;
  r_adapter->SetHeight(height);
  auto ret = r_adapter->GetHeight();
  EXPECT_EQ(ret, height);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_011) {
  const int32_t stride_alignment = 128;
  r_adapter->SetStrideAlignment(stride_alignment);
  auto ret = r_adapter->GetStrideAlignment();
  EXPECT_EQ(ret, stride_alignment);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_012) {
  const int32_t format = 1;
  r_adapter->SetFormat(format);
  auto ret = r_adapter->GetFormat();
  EXPECT_EQ(ret, format);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_013) {
  const uint64_t usage = 0x1234567890ABCDEF;
  r_adapter->SetUsage(usage);
  auto ret = r_adapter->GetUsage();
  EXPECT_EQ(ret, usage);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_014) {
  const int32_t timeout = 10000;
  r_adapter->SetTimeout(timeout);
  auto ret = r_adapter->GetTimeout();
  EXPECT_EQ(ret, timeout);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_015) {
  ColorGamutAdapter color_gamut = ColorGamutAdapter::INVALID;
  r_adapter->SetColorGamut(color_gamut);
  auto ret = r_adapter->GetColorGamut();
  EXPECT_EQ(ret, color_gamut);
}

TEST_F(BufferRequestConfigAdapterImplTest,
       NWebBufferAdapterTest_BufferRequestConfigAdapterImpl_016) {
  TransformTypeAdapter transform_type = TransformTypeAdapter::ROTATE_NONE;
  r_adapter->SetTransformType(transform_type);
  auto ret = r_adapter->GetTransformType();
  EXPECT_EQ(ret, transform_type);
}
}  // namespace OHOS::NWeb