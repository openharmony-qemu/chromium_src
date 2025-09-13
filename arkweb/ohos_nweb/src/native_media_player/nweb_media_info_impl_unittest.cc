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

#include "arkweb/ohos_nweb/src/native_media_player/nweb_media_info_impl.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace OHOS::NWeb {

class NWebMediaInfoImplTest : public ::testing::Test {
 public:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(NWebMediaInfoImplTest, GetSourceInfosWithMediaSources) {
  CefCustomMediaInfo media_info;
  CefMediaSourceInfo source1;
  source1.source_type = 1;
  source1.media_format = "video/mp4";
  source1.media_source = "test1.mp4";

  CefMediaSourceInfo source2;
  source2.source_type = 2;
  source2.media_format = "audio/mp3";
  source2.media_source = "test2.mp3";
  media_info.media_src_list.push_back(source1);
  media_info.media_src_list.push_back(source2);

  std::unique_ptr<NWebMediaInfoImpl> media_info_impl =
      std::make_unique<NWebMediaInfoImpl>(media_info);
  auto source_infos = media_info_impl->GetSourceInfos();
  EXPECT_EQ(source_infos.size(), 2);

  EXPECT_EQ(source_infos[0]->GetType(), static_cast<SourceType>(1));
  EXPECT_EQ(source_infos[0]->GetFormat(), "video/mp4");
  EXPECT_EQ(source_infos[0]->GetSource(), "test1.mp4");
  EXPECT_EQ(source_infos[1]->GetType(), static_cast<SourceType>(2));
  EXPECT_EQ(source_infos[1]->GetFormat(), "audio/mp3");
  EXPECT_EQ(source_infos[1]->GetSource(), "test2.mp3");
}

TEST_F(NWebMediaInfoImplTest, GetSourceInfosWithEmptyMediaSources) {
  CefCustomMediaInfo media_info;
  std::unique_ptr<NWebMediaInfoImpl> media_info_impl =
      std::make_unique<NWebMediaInfoImpl>(media_info);
  auto source_infos = media_info_impl->GetSourceInfos();
  EXPECT_EQ(source_infos.size(), 0);
  EXPECT_TRUE(source_infos.empty());
}

}  // namespace OHOS::NWeb
