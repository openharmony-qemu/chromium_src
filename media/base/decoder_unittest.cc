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

#include <string>
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "media/base/decoder.h"
#undef private
namespace media {
namespace {
class GetDecoderNameTest : public testing::Test {
 public:
  void SetUp() override { g_videodecoder = VideoDecoderType::kUnknown; }
  void TearDown() override {}
  VideoDecoderType g_videodecoder;
};
TEST_F(GetDecoderNameTest, GetDecoderName001) {
  g_videodecoder = VideoDecoderType::kOHOS;
  std::string decoder_name = GetDecoderName(g_videodecoder);
  EXPECT_EQ(decoder_name, "OHOSVideoDecoder");
}
}  // namespace
}  // namespace media
