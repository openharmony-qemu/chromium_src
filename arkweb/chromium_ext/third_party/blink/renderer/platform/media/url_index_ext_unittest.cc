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
TEST_F(UrlIndexTest, VideoLoadOpt_FeedsPageTest) {
  EXPECT_FALSE(url_index_.IsNewsFeedPageFitted());

  url_index_.SetNewsFeedPageFitted(true);
  EXPECT_TRUE(url_index_.IsNewsFeedPageFitted());
}

TEST_F(UrlIndexTest, VideoLoadOpt_NeedCallbackTest) {
  KURL url("http://foo.bar.com");
  scoped_refptr<UrlData> url_data_ = GetByUrl(url, UrlData::CORS_UNSPECIFIED);
  EXPECT_FALSE(url_data_->IsNeedFallback());

  url_data_->SetNeedFallback();
  EXPECT_TRUE(url_data_->IsNeedFallback());
}

TEST_F(UrlIndexTest, VideoLoadOpt_CreateSegmentationTest) {
  KURL url("http://foo.bar.com");
  scoped_refptr<UrlData> url_data_ = GetByUrl(url, UrlData::CORS_UNSPECIFIED);
  EXPECT_FALSE(url_data_->IsCreateSegmentationProvider());
  
  url_index_.SetNewsFeedPageFitted(true);
  EXPECT_TRUE(url_data_->IsCreateSegmentationProvider());

  url_data_->SetNeedFallback();
  EXPECT_FALSE(url_data_->IsCreateSegmentationProvider());
}

TEST_F(UrlIndexTest, VideoLoadOpt_CreateWriterTest) {
  KURL url("http://foo.bar.com");
  scoped_refptr<UrlData> url_data_ = GetByUrl(url, UrlData::CORS_UNSPECIFIED);
  url_index_.SetNewsFeedPageFitted(true);
  std::string writerInfo = url_data_->CreateWriterInfo();
  std::string searchTmp = "useLoadOptimization";
  EXPECT_TRUE(writerInfo.find(searchTmp) != std::string::npos);
  
  searchTmp = "IsNeedFallback";
  EXPECT_TRUE(writerInfo.find(searchTmp) != std::string::npos);

  searchTmp = "IsHttp";
  EXPECT_TRUE(writerInfo.find(searchTmp) != std::string::npos);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}