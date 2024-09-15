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

#include "testing/gtest/include/gtest/gtest.h"
#define private public
#define protectd public
#include "media/base/renderer.h"
#undef protectd public
#undef private public
namespace media {

class RendererTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(RendererTest, KnownRendererTypes) {
  EXPECT_EQ(GetRendererName(RendererType::kRendererImpl), "RendererImpl");
  EXPECT_EQ(GetRendererName(RendererType::kMojo), "MojoRenderer");
  EXPECT_EQ(GetRendererName(RendererType::kMediaPlayer), "MediaPlayerRenderer");
  EXPECT_EQ(GetRendererName(RendererType::kCourier), "CourierRenderer");
  EXPECT_EQ(GetRendererName(RendererType::kFlinging), "FlingingRenderer");
  EXPECT_EQ(GetRendererName(RendererType::kCast), "CastRenderer");
  EXPECT_EQ(GetRendererName(RendererType::kMediaFoundation),
            "MediaFoundationRenderer");
  EXPECT_EQ(GetRendererName(RendererType::kRemoting), "RemotingRenderer");
  EXPECT_EQ(GetRendererName(RendererType::kCastStreaming),
            "CastStreamingRenderer");
  EXPECT_EQ(GetRendererName(RendererType::kContentEmbedderDefined),
            "EmbedderDefined");
  EXPECT_EQ(GetRendererName(RendererType::kTest),
            "Media Renderer Implementation For Testing");
}

TEST_F(RendererTest, GetRendererNameTest01) {
#ifdef BUILDFLAG(IS_OHOS)
  EXPECT_EQ(GetRendererName(RendererType::kNative), "Native");
#endif
}

TEST_F(RendererTest, GetRendererNameTest02) {
#ifdef BUILDFLAG(IS_OHOS)
  EXPECT_EQ(GetRendererName(RendererType::kOHOSMediaPlayer),
            "OHOSMediaPlayerRenderer");
#endif
}

TEST_F(RendererTest, GetRendererNameTest03) {
#ifdef OHOS_CUSTOM_VIDEO_PLAYER
  EXPECT_EQ(GetRendererName(RendererType::kOHOSCustomMediaPlayer),
            "OHOSCustomMediaPlayerRednerer");
#endif
}

}  // namespace media
