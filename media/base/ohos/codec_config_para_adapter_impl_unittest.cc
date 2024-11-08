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
#include "codec_config_para_adapter_impl.h"
#undef private

using namespace testing;

namespace OHOS::NWeb {

namespace {
std::shared_ptr<CodecConfigParaAdapterImpl> g_adapter;
}

class CodecConfigParaAdapterImplTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};

void CodecConfigParaAdapterImplTest::SetUpTestCase(void) {}

void CodecConfigParaAdapterImplTest::TearDownTestCase(void) {}

void CodecConfigParaAdapterImplTest::SetUp(void) {
  g_adapter = std::make_shared<CodecConfigParaAdapterImpl>();
  ASSERT_NE(g_adapter, nullptr);
}

void CodecConfigParaAdapterImplTest::TearDown(void) {
  g_adapter = nullptr;
}

TEST_F(CodecConfigParaAdapterImplTest,
       NWebCodecAdapterTest_CodecConfigParaAdapterImpl_001) {
  int32_t width = 0;
  g_adapter->width_ = width;
  auto ret = g_adapter->GetWidth();
  EXPECT_EQ(ret, width);
}

TEST_F(CodecConfigParaAdapterImplTest,
       NWebCodecAdapterTest_CodecConfigParaAdapterImpl_002) {
  int32_t height = 0;
  g_adapter->height_ = height;
  auto ret = g_adapter->GetHeight();
  EXPECT_EQ(ret, height);
}

TEST_F(CodecConfigParaAdapterImplTest,
       NWebCodecAdapterTest_CodecConfigParaAdapterImpl_003) {
  int64_t bit_rate = 0;
  g_adapter->bit_rate_ = bit_rate;
  auto ret = g_adapter->GetBitRate();
  EXPECT_EQ(ret, bit_rate);
}

TEST_F(CodecConfigParaAdapterImplTest,
       NWebCodecAdapterTest_CodecConfigParaAdapterImpl_004) {
  double frame_rate = 0.0;
  g_adapter->frame_rate_ = frame_rate;
  auto ret = g_adapter->GetFrameRate();
  EXPECT_DOUBLE_EQ(ret, frame_rate);
}

TEST_F(CodecConfigParaAdapterImplTest,
       NWebCodecAdapterTest_CodecConfigParaAdapterImpl_005) {
  int32_t width = 0;
  g_adapter->SetWidth(width);
  auto ret = g_adapter->GetWidth();
  EXPECT_EQ(ret, width);
}

TEST_F(CodecConfigParaAdapterImplTest,
       NWebCodecAdapterTest_CodecConfigParaAdapterImpl_006) {
  int32_t height = 0;
  g_adapter->SetHeight(height);
  auto ret = g_adapter->GetHeight();
  EXPECT_EQ(ret, height);
}

TEST_F(CodecConfigParaAdapterImplTest,
       NWebCodecAdapterTest_CodecConfigParaAdapterImpl_007) {
  int64_t bitrate = 0;
  g_adapter->SetBitRate(bitrate);
  auto ret = g_adapter->GetBitRate();
  EXPECT_EQ(ret, bitrate);
}

TEST_F(CodecConfigParaAdapterImplTest,
       NWebCodecAdapterTest_CodecConfigParaAdapterImpl_008) {
  double frameRate = 0.0;
  g_adapter->SetFrameRate(frameRate);
  auto ret = g_adapter->GetFrameRate();
  EXPECT_DOUBLE_EQ(ret, frameRate);
}

}  // namespace OHOS::NWeb