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

namespace blink {

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_F(MultiBufferDataSourceTest, VideoLoadOpt_VLOUpdateBufferSize001) {
  Initialize(kHttpUrl, true);

  uint16_t preloadTime = 4;
  uint16_t minCacheTime = 2;
  uint16_t maxCacheTime = 6;
  uint16_t moovSize = 512;
  uint16_t bitRate = 2000;
  std::string videoId = "v_662102_html5_api";

  data_source_->SetVLOParams(preloadTime, minCacheTime, maxCacheTime,
                             moovSize, bitRate, videoId);
  data_source_->VLOUpdateBufferSizes();

  int64_t preload_high = loader()->preload_high();
  int64_t expect_val = 32768;
  EXPECT_EQ(preload_high, expect_val);
  Stop();
}

TEST_F(MultiBufferDataSourceTest, VideoLoadOpt_VLOUpdateBufferSize002) {
  Initialize(kHttpUrl, true);

  uint16_t preloadTime = 4;
  uint16_t minCacheTime = 2;
  uint16_t maxCacheTime = 6;
  uint16_t moovSize = 512;
  uint16_t bitRate = 2000;
  std::string videoId = "v_662102_html5_api";

  url_data()->set_length(kPositionNotSpecified);
  data_source_->SetVLOParams(preloadTime, minCacheTime, maxCacheTime,
                             moovSize, bitRate, videoId);
  data_source_->VLOUpdateBufferSizes();

  int64_t preload_high = loader()->preload_high();
  int64_t expect_val = 32768;
  EXPECT_EQ(preload_high, expect_val);
  Stop();
}

TEST_F(MultiBufferDataSourceTest, VideoLoadOpt_VLOUpdateBufferSize003) {
  Initialize(kHttpUrl, true);

  uint16_t preloadTime = 4;
  uint16_t minCacheTime = 2;
  uint16_t maxCacheTime = 6;
  uint16_t moovSize = 512;
  uint16_t bitRate = 2000;
  std::string videoId = "v_662102_html5_api";

  data_source_->OnMediaIsPlaying();
  data_source_->SetVLOParams(preloadTime, minCacheTime, maxCacheTime,
                             moovSize, bitRate, videoId);
  data_source_->VLOUpdateBufferSizes();

  int64_t preload_high = loader()->preload_high();
  int64_t expect_val = 500000;
  EXPECT_EQ(preload_high, expect_val);
  Stop();
}

TEST_F(MultiBufferDataSourceTest, VideoLoadOpt_SetVLOParamsTest) {
  Initialize(kHttpUrl, true);

  uint16_t preloadTime = 4;
  uint16_t minCacheTime = 2;
  uint16_t maxCacheTime = 6;
  uint16_t moovSize = 512;
  uint16_t bitRate = 1000;
  std::string videoId = "v_662102_html5_api";

  data_source_->SetVLOParams(preloadTime, maxCacheTime, minCacheTime,
                             bitRate, moovSize, videoId);
  uint16_t byteRate = data_source_byte_rate();
  EXPECT_EQ(byteRate, 125);
  Stop();
}

TEST_F(MultiBufferDataSourceTest, VideoLoadOpt_SetVLOParamsTest2) {
  Initialize(kHttpUrl, true);

  uint16_t preloadTime = 4;
  uint16_t minCacheTime = 2;
  uint16_t maxCacheTime = 6;
  uint16_t moovSize = 512;
  uint16_t bitRate = 0;
  std::string videoId = "v_662102_html5_api";

  data_source_->SetVLOParams(preloadTime, maxCacheTime, minCacheTime,
                             bitRate, moovSize, videoId);
  uint16_t byteRate = data_source_byte_rate();
  EXPECT_EQ(byteRate, 250);
  Stop();
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}