/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/components/dom_distiller/core/distiller_page_ohos.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/run_loop.h"
#include "base/test/task_environment.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/dom_distiller_js/dom_distiller.pb.h"
#include "url/gurl.h"

#if BUILDFLAG(ARKWEB_READER_MODE)

namespace dom_distiller {

class DistillerPageOhosTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}

  base::test::TaskEnvironment task_environment_;
};

TEST_F(DistillerPageOhosTest, GetPathWithGURL_ValidUrlWithPath) {
  GURL url("http://example.com/path/to/page");
  std::string result = GetPathWithGURL(url);
  EXPECT_EQ(result, "/path/to/page");
}

TEST_F(DistillerPageOhosTest, GetPathWithGURL_ValidUrlWithPathAndFragment) {
  GURL url("http://example.com/path/to/page#section");
  std::string result = GetPathWithGURL(url);
  EXPECT_EQ(result, "/path/to/page#section");
}

TEST_F(DistillerPageOhosTest, GetPathWithGURL_ValidUrlWithOnlyFragment) {
  GURL url("http://example.com#section");
  std::string result = GetPathWithGURL(url);
  EXPECT_EQ(result, "/#section");
}

TEST_F(DistillerPageOhosTest, GetPathWithGURL_ValidUrlWithoutPathOrFragment) {
  GURL url("http://example.com");
  std::string result = GetPathWithGURL(url);
  EXPECT_EQ(result, "/");
}

TEST_F(DistillerPageOhosTest, GetPathWithGURL_ValidUrlWithOnlyPath) {
  GURL url("http://example.com/");
  std::string result = GetPathWithGURL(url);
  EXPECT_EQ(result, "/");
}

TEST_F(DistillerPageOhosTest, GetPathWithGURL_InvalidUrl) {
  GURL url("not a valid url");
  std::string result = GetPathWithGURL(url);
  EXPECT_EQ(result, "");
}

TEST_F(DistillerPageOhosTest, ParserXpathAndUpdateConfig_EmptyString) {
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserXpathAndUpdateConfig("", &config);
  EXPECT_FALSE(result);
}

TEST_F(DistillerPageOhosTest, ParserXpathAndUpdateConfig_ValidJson) {
  std::string json = R"({
    "book_name": "Test Book",
    "chapter_name": "Chapter 1",
    "content": "Test Content"
  })";
  
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserXpathAndUpdateConfig(json, &config);
  EXPECT_TRUE(result);
  EXPECT_EQ(config.xpath_config().book_name(), "Test Book");
  EXPECT_EQ(config.xpath_config().chapter_name(), "Chapter 1");
  EXPECT_EQ(config.xpath_config().content(), "Test Content");
}

TEST_F(DistillerPageOhosTest, ParserXpathAndUpdateConfig_InvalidJson) {
  std::string json = "{ invalid json }";
  
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserXpathAndUpdateConfig(json, &config);
  EXPECT_FALSE(result);
}

TEST_F(DistillerPageOhosTest, ParserXpathAndUpdateConfig_NotDict) {
  std::string json = "[\"array\", \"not\", \"dict\"]";
  
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserXpathAndUpdateConfig(json, &config);
  EXPECT_FALSE(result);
}

TEST_F(DistillerPageOhosTest, ParserXpathAndUpdateConfig_AllFields) {
  std::string json = R"({
    "book_name": "Book",
    "chapter_name": "Chapter",
    "description": "Description",
    "image_url": "http://example.com/image.jpg",
    "author": "Author",
    "content": "Content",
    "category": "Category",
    "last_chapter_name": "Last Chapter",
    "last_chapter_url": "http://example.com/last",
    "start_read_link": "http://example.com/start",
    "next_content_page": "http://example.com/next",
    "prev_content_page": "http://example.com/prev",
    "current_catalog_page": "http://example.com/catalog",
    "all_chapters": "All Chapters",
    "latest_chapters": "Latest Chapters",
    "next_catalog_page": "http://example.com/next-catalog",
    "prev_catalog_page": "http://example.com/prev-catalog",
    "whole_catalog_page": "http://example.com/whole-catalog",
    "all_catalog_pages": "All Catalog Pages"
  })";
  
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserXpathAndUpdateConfig(json, &config);
  EXPECT_TRUE(result);
  EXPECT_EQ(config.xpath_config().book_name(), "Book");
  EXPECT_EQ(config.xpath_config().chapter_name(), "Chapter");
  EXPECT_EQ(config.xpath_config().description(), "Description");
  EXPECT_EQ(config.xpath_config().image_url(), "http://example.com/image.jpg");
  EXPECT_EQ(config.xpath_config().author(), "Author");
  EXPECT_EQ(config.xpath_config().content(), "Content");
  EXPECT_EQ(config.xpath_config().category(), "Category");
  EXPECT_EQ(config.xpath_config().last_chapter_name(), "Last Chapter");
  EXPECT_EQ(config.xpath_config().last_chapter_url(), "http://example.com/last");
  EXPECT_EQ(config.xpath_config().start_read_link(), "http://example.com/start");
  EXPECT_EQ(config.xpath_config().next_content_page(), "http://example.com/next");
  EXPECT_EQ(config.xpath_config().prev_content_page(), "http://example.com/prev");
  EXPECT_EQ(config.xpath_config().current_catalog_page(), "http://example.com/catalog");
  EXPECT_EQ(config.xpath_config().all_chapters(), "All Chapters");
  EXPECT_EQ(config.xpath_config().latest_chapters(), "Latest Chapters");
  EXPECT_EQ(config.xpath_config().next_catalog_page(), "http://example.com/next-catalog");
  EXPECT_EQ(config.xpath_config().prev_catalog_page(), "http://example.com/prev-catalog");
  EXPECT_EQ(config.xpath_config().whole_catalog_page(), "http://example.com/whole-catalog");
  EXPECT_EQ(config.xpath_config().all_catalog_pages(), "All Catalog Pages");
}

TEST_F(DistillerPageOhosTest, ParserJavascriptAndUpdateConfig_EmptyString) {
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserJavascriptAndUpdateConfig("", &config);
  EXPECT_FALSE(result);
}

TEST_F(DistillerPageOhosTest, ParserJavascriptAndUpdateConfig_ValidJson) {
  std::string json = R"({
    "book_name": "Test Book",
    "chapter_name": "Chapter 1",
    "content": "Test Content",
    "reserved": "Reserved Value"
  })";
  
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserJavascriptAndUpdateConfig(json, &config);
  EXPECT_TRUE(result);
  EXPECT_EQ(config.javascript_config().book_name(), "Test Book");
  EXPECT_EQ(config.javascript_config().chapter_name(), "Chapter 1");
  EXPECT_EQ(config.javascript_config().content(), "Test Content");
  EXPECT_EQ(config.javascript_config().reserved(), "Reserved Value");
}

TEST_F(DistillerPageOhosTest, ParserJavascriptAndUpdateConfig_InvalidJson) {
  std::string json = "{ invalid json }";
  
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserJavascriptAndUpdateConfig(json, &config);
  EXPECT_FALSE(result);
}

TEST_F(DistillerPageOhosTest, ParserJavascriptAndUpdateConfig_AllFields) {
  std::string json = R"({
    "book_name": "Book",
    "chapter_name": "Chapter",
    "description": "Description",
    "image_url": "http://example.com/image.jpg",
    "author": "Author",
    "content": "Content",
    "category": "Category",
    "last_chapter_name": "Last Chapter",
    "last_chapter_url": "http://example.com/last",
    "start_read_link": "http://example.com/start",
    "next_content_page": "http://example.com/next",
    "prev_content_page": "http://example.com/prev",
    "current_catalog_page": "http://example.com/catalog",
    "all_chapters": "All Chapters",
    "latest_chapters": "Latest Chapters",
    "next_catalog_page": "http://example.com/next-catalog",
    "prev_catalog_page": "http://example.com/prev-catalog",
    "whole_catalog_page": "http://example.com/whole-catalog",
    "all_catalog_pages": "All Catalog Pages",
    "reserved": "Reserved Value"
  })";
  
  dom_distiller::proto::DistillOptionsConfig config;
  bool result = ParserJavascriptAndUpdateConfig(json, &config);
  EXPECT_TRUE(result);
  EXPECT_EQ(config.javascript_config().book_name(), "Book");
  EXPECT_EQ(config.javascript_config().chapter_name(), "Chapter");
  EXPECT_EQ(config.javascript_config().description(), "Description");
  EXPECT_EQ(config.javascript_config().image_url(), "http://example.com/image.jpg");
  EXPECT_EQ(config.javascript_config().author(), "Author");
  EXPECT_EQ(config.javascript_config().content(), "Content");
  EXPECT_EQ(config.javascript_config().category(), "Category");
  EXPECT_EQ(config.javascript_config().last_chapter_name(), "Last Chapter");
  EXPECT_EQ(config.javascript_config().last_chapter_url(), "http://example.com/last");
  EXPECT_EQ(config.javascript_config().start_read_link(), "http://example.com/start");
  EXPECT_EQ(config.javascript_config().next_content_page(), "http://example.com/next");
  EXPECT_EQ(config.javascript_config().prev_content_page(), "http://example.com/prev");
  EXPECT_EQ(config.javascript_config().current_catalog_page(), "http://example.com/catalog");
  EXPECT_EQ(config.javascript_config().all_chapters(), "All Chapters");
  EXPECT_EQ(config.javascript_config().latest_chapters(), "Latest Chapters");
  EXPECT_EQ(config.javascript_config().next_catalog_page(), "http://example.com/next-catalog");
  EXPECT_EQ(config.javascript_config().prev_catalog_page(), "http://example.com/prev-catalog");
  EXPECT_EQ(config.javascript_config().whole_catalog_page(), "http://example.com/whole-catalog");
  EXPECT_EQ(config.javascript_config().all_catalog_pages(), "All Catalog Pages");
  EXPECT_EQ(config.javascript_config().reserved(), "Reserved Value");
}

TEST_F(DistillerPageOhosTest, GetCustomDomDistillerOptions_IsArticle) {
  GURL url("http://example.com/path");
  dom_distiller::proto::DomDistillerOptions source_options;
  source_options.set_distill_ext_info("test_ext_info");
  
  dom_distiller::proto::DomDistillerOptions result = 
      GetCustomDomDistillerOptions(true, url, source_options);
  
  EXPECT_EQ(result.distill_ext_info(), "test_ext_info");
}

TEST_F(DistillerPageOhosTest, GetCustomDomDistillerOptions_NotArticle) {
  GURL url("http://example.com/path");
  dom_distiller::proto::DomDistillerOptions source_options;
  source_options.set_distill_ext_info("test_ext_info");
  
  dom_distiller::proto::DomDistillerOptions result = 
      GetCustomDomDistillerOptions(false, url, source_options);
  
  EXPECT_EQ(result.distill_ext_info(), "test_ext_info");
}

TEST_F(DistillerPageOhosTest, SetCustomOptions_EmptyPath) {
  GURL url("http://example.com");
  dom_distiller::proto::HwCustomOptions custom_options;
  
  bool result = SetCustomOptions(url, &custom_options);
  EXPECT_FALSE(result);
}

TEST_F(DistillerPageOhosTest, SetCustomOptions_WithPath) {
  GURL url("http://example.com/path");
  dom_distiller::proto::HwCustomOptions custom_options;
  
  bool result = SetCustomOptions(url, &custom_options);
  EXPECT_FALSE(result);
}

}  // namespace dom_distiller

#endif  // ARKWEB_READER_MODE
