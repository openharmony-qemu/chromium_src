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

#include <memory>

#include <gtest/gtest.h>
#include "content/browser/code_cache/oh_code_cache.h"

using namespace testing;

namespace oh_code_cache {

const char* const TEST_HTTP_URL = "http://test.com/example.js";
const char* const TEST_CONTENT = "OhCodeCache Test";
const char* const TEST_NEW_CONTENT = "new test content";
const char* const HEADER_TAG = "E-Tag";
const char* const HEADER_LAST_MODIFIED = "Last-Modified";
const char* const HEADER_ACCESS_CONTROL = "Access-Control-Allow-Origin";
const char* const HEADER_CONTENT_LENGTH = "Content-Length";
const char* const HEADER_FILE_HASH = "File-Hash";
const char* const METADATA_FILENAME = "metadata";
const char* const TEST_FILENAME = "test_file.txt";
const char* const CACHE_FILENAME = "RC12345";
const char* const TEMP_DIRECTORY_NAME = "test_empty_directory";
const char* const INVALID_METADATA_PATH = "/invalid/path/metadata";
const char* const INVALID_CACHE_PATH = "/invalid/path/cache";
const char* const NON_EXIST_PATH = "/non/existent/file.js";
const char* const TEST_HASHES[] = {"test_hash", "diff_hash", "12345"};
const char* const TEST_TAGS[] = {"abc", "def", "ghi"};
const char* const TEST_CONTENT_LENGTHS[] = {"5", "11", "100", "200"};
const char* const TEST_FILE_HASHES[] = {"123", "456"};
const char* const TEST_CHARS[] = {"Hello", " World", "Hello World"};
const char* const ACCESS_CONTROL_VALUES[] = {"example.com", "test.com"};
const int READ_SIZES[] = {5, 6, 10};

class OhCodeCacheTest : public ::testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}

    std::string CreateTestMetadataContent(
      const std::string& url_hash,
      const std::string& content_length,
      const std::string& e_tag,
      const std::string& last_modified,
      const std::string& access_control,
      const std::string& file_hash) {
      return std::string("URL:") + url_hash + "|" + HEADER_CONTENT_LENGTH +
          ":" + content_length + "|" + HEADER_TAG + ":" + e_tag + "|" +
          HEADER_LAST_MODIFIED + ":" + last_modified + "|" +
          HEADER_ACCESS_CONTROL + ":" + access_control + "|" +
          HEADER_FILE_HASH + ":" + file_hash + "\n";
    }
};

TEST_F(OhCodeCacheTest, ToString001) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find("URL") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ToString002) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find("Content-Length") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ToString003) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find("E-Tag") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ToString004) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find("Last-Modified") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ToString005) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find("Access-Control-Allow-Origin") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ToString006) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find("File-Hash") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ToString007) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find(":") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ToString008) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find("|") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ToString009) {
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    std::string res = RCMptr->ToString();
    EXPECT_TRUE(res.find("\n") != std::string::npos);
}

TEST_F(OhCodeCacheTest, ResponseCacheMetadat001) {
    std::string url = "https://test.com";
    std::string response_body = "Hello Test";
    std::map<std::string, std::string> headers = {
        {"E-Tag", "abc"},
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
    };
    ResponseCacheMetadata metaDate(url, response_body, headers);
    EXPECT_EQ(metaDate.e_tag_, "abc");
    EXPECT_EQ(metaDate.last_modified_, "def");
    EXPECT_EQ(metaDate.access_control_allow_origin_, "ghi");
}

TEST_F(OhCodeCacheTest, ResponseCacheMetadat002) {
    std::string url = "https://test.com";
    std::string response_body = "Hello Test";
    std::map<std::string, std::string> headers = {
        {"E-Tag", "abc"}
    };
    ResponseCacheMetadata metaDate(url, response_body, headers);
    EXPECT_EQ(metaDate.e_tag_, "abc");
    EXPECT_TRUE(metaDate.last_modified_.empty());
    EXPECT_TRUE(metaDate.access_control_allow_origin_.empty());
}

TEST_F(OhCodeCacheTest, ResponseCacheMetadat003) {
    std::string url = "https://test.com";
    std::string response_body = "Hello Test";
    std::map<std::string, std::string> headers = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
    };
    ResponseCacheMetadata metaDate(url, response_body, headers);
    EXPECT_TRUE(metaDate.e_tag_.empty());
    EXPECT_EQ(metaDate.last_modified_, "def");
    EXPECT_EQ(metaDate.access_control_allow_origin_, "ghi");
}

TEST_F(OhCodeCacheTest, ResponseCacheMetadat004) {
    std::string url = "https://test.com";
    std::string response_body = "Hello Test";
    std::map<std::string, std::string> headers = {};
    ResponseCacheMetadata metaDate(url, response_body, headers);
    EXPECT_TRUE(metaDate.e_tag_.empty());
    EXPECT_TRUE(metaDate.last_modified_.empty());
    EXPECT_TRUE(metaDate.access_control_allow_origin_.empty());
}

TEST_F(OhCodeCacheTest, InitCacheDirectory001) {
    std::string url = "http://test/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    base::FilePath path;
    testing::internal::CaptureStderr();
    response_cache->InitCacheDirectory(path);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("Response Cache directory does not exist"), std::string::npos);
    EXPECT_TRUE(path.empty());
}

TEST_F(OhCodeCacheTest, InitCacheDirectory002) {
    std::string url = "http://test/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    base::FilePath temp_dir;
    ASSERT_TRUE(base::GetTempDir(&temp_dir));
    base::FilePath path = temp_dir.AppendASCII("test_create_directory");
    EXPECT_TRUE(base::CreateDirectory(path));
    base::FilePath non_existent_path = path.AppendASCII("non_existent_dir");
    EXPECT_FALSE(base::PathExists(non_existent_path));
    response_cache->InitCacheDirectory(non_existent_path);
    EXPECT_TRUE(base::PathExists(non_existent_path));
    base::DeletePathRecursively(path);
}

TEST_F(OhCodeCacheTest, InitCacheDirectory003) {
    std::string url = "http://test/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    base::FilePath temp_dir;
    ASSERT_TRUE(base::GetTempDir(&temp_dir));
    base::FilePath path = temp_dir.AppendASCII("test_create_directory");
    EXPECT_TRUE(base::CreateDirectory(path));
    EXPECT_TRUE(base::PathExists(path));
    response_cache->InitCacheDirectory(path);
    EXPECT_EQ((*response_cache->cache_dir_path_)->value(), path.value());
    base::DeletePathRecursively(path);
}

TEST_F(OhCodeCacheTest, CreateResponseCache001) {
    std::string url = "";
    EXPECT_TRUE(url.empty());
    auto res = ResponseCache::CreateResponseCache(url);
    EXPECT_EQ(res, nullptr);
}

TEST_F(OhCodeCacheTest, CreateResponseCache002) {
    std::string url = "ftp://test/example.js";
    auto res = ResponseCache::CreateResponseCache(url);
    EXPECT_EQ(res, nullptr);
}

TEST_F(OhCodeCacheTest, CreateResponseCache003) {
    std::string url = "http://test.com/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    (*response_cache->cache_dir_path_) = std::make_unique<base::FilePath>("");
    EXPECT_TRUE((*response_cache->cache_dir_path_)->empty());
    auto res = response_cache->CreateResponseCache(url);
    EXPECT_EQ(res, nullptr);
}

TEST_F(OhCodeCacheTest, CreateResponseCache004) {
    std::string url = "http://test.com/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    (*response_cache->cache_dir_path_) = nullptr;
    EXPECT_EQ((*response_cache->cache_dir_path_), nullptr);
    auto res = response_cache->CreateResponseCache(url);
    EXPECT_EQ(res, nullptr);
}

TEST_F(OhCodeCacheTest, CreateResponseCache005) {
    std::string url = "http://test.com/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    (*response_cache->cache_dir_path_) = std::make_unique<base::FilePath>("/tmp/test/cache");
    EXPECT_NE((*response_cache->cache_dir_path_), nullptr);
    auto res = response_cache->CreateResponseCache(url);
    EXPECT_NE(res, nullptr);
}

TEST_F(OhCodeCacheTest, ClearAllCache001) {
    std::string url = "http://test.com/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    (*ResponseCache::cache_metadata_map_).emplace("key1", RCMptr);
    (*ResponseCache::cache_dir_path_) = nullptr;
    EXPECT_EQ((*response_cache->cache_dir_path_), nullptr);
    ResponseCache::ClearAllCache();
    EXPECT_FALSE((*ResponseCache::cache_metadata_map_).empty());
}

TEST_F(OhCodeCacheTest, ClearAllCache002) {
    std::string url = "http://test.com/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    (*ResponseCache::cache_metadata_map_).emplace("key1", RCMptr);
    base::FilePath temp_dir;
    ASSERT_TRUE(base::GetTempDir(&temp_dir));
    base::FilePath path = temp_dir.AppendASCII("test_create_directory");
    EXPECT_TRUE(base::CreateDirectory(path));
    ResponseCache::InitCacheDirectory(path);
    EXPECT_TRUE(base::PathExists(path));
    base::DeletePathRecursively(path);
    EXPECT_FALSE(base::PathExists(path));
    ResponseCache::ClearAllCache();
    EXPECT_FALSE((*ResponseCache::cache_metadata_map_).empty());
}

TEST_F(OhCodeCacheTest, ClearAllCache003) {
    std::string url = "http://test.com/example.js";
    std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
    std::shared_ptr<ResponseCacheMetadata> RCMptr = std::make_shared<ResponseCacheMetadata>();
    (*ResponseCache::cache_metadata_map_).emplace("key1", RCMptr);
    base::FilePath temp_dir;
    ASSERT_TRUE(base::GetTempDir(&temp_dir));
    base::FilePath path = temp_dir.AppendASCII("test_create_directory");
    EXPECT_TRUE(base::CreateDirectory(path));
    ResponseCache::InitCacheDirectory(path);
    EXPECT_TRUE(base::PathExists(path));
    ResponseCache::ClearAllCache();
    EXPECT_TRUE((*ResponseCache::cache_metadata_map_).empty());
}

TEST_F(OhCodeCacheTest, Write001) {
    ResponseCache cache("http://example.com");
    std::string response_body = "Hello Test";
    std::map<std::string, std::string> headers = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
    };
    
    NextOp res = cache.Write(headers, response_body);
    EXPECT_EQ(res, NextOp::THROW_ERROR);
}

TEST_F(OhCodeCacheTest, GetResponseHeaders001) {
    int32_t request_id = 1;
    int status_code = 0;
    std::string reason_phrase;
    std::string mime_type;
    std::string charset;
    int64_t content_length = 0;
    HeaderMap extra_headers;

    std::string url = "http://test.com/example.js";
    std::shared_ptr<ResponseCache> cache = std::make_shared<ResponseCache>(url);
    cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
    ResourceResponse resource_response(cache);
    resource_response.response_cache_ = cache;
    ASSERT_NE(resource_response.response_cache_->metadata_out_, nullptr);
    resource_response.GetResponseHeaders(request_id, &status_code, &reason_phrase, &mime_type, &charset,
        &content_length, &extra_headers);
    EXPECT_EQ(status_code, 200);
    EXPECT_EQ(reason_phrase, "OK");
    EXPECT_EQ(mime_type, "text/javascript");
    EXPECT_EQ(charset, "utf-8");
    EXPECT_EQ(content_length, resource_response.response_cache_->response_body_out_.size());
}

TEST_F(OhCodeCacheTest, ResponseCacheMetadataConstructor005) {
  std::string url = TEST_HTTP_URL;
  std::string response_body = TEST_CONTENT;
  std::map<std::string, std::string> headers = {{HEADER_LAST_MODIFIED, TEST_TAGS[1]}};
  ResponseCacheMetadata metaDate(url, response_body, headers);
  EXPECT_TRUE(metaDate.e_tag_.empty());
  EXPECT_EQ(metaDate.last_modified_, TEST_TAGS[1]);
  EXPECT_TRUE(metaDate.access_control_allow_origin_.empty());
}

TEST_F(OhCodeCacheTest, ResponseCacheMetadataConstructor006) {
  std::string url = TEST_HTTP_URL;
  std::string response_body = TEST_CONTENT;
  std::map<std::string, std::string> headers = {{HEADER_ACCESS_CONTROL, TEST_TAGS[2]}};
  ResponseCacheMetadata metaDate(url, response_body, headers);
  EXPECT_TRUE(metaDate.e_tag_.empty());
  EXPECT_TRUE(metaDate.last_modified_.empty());
  EXPECT_EQ(metaDate.access_control_allow_origin_, TEST_TAGS[2]);
}

TEST_F(OhCodeCacheTest, CreateResponseCache006) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto res = ResponseCache::CreateResponseCache(url);
  EXPECT_NE(res, nullptr);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, CreateResponseCache007) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto res = ResponseCache::CreateResponseCache(url);
  EXPECT_NE(res, nullptr);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, CreateResponseCache008) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto res = ResponseCache::CreateResponseCache(url);
  EXPECT_NE(res, nullptr);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, ClearAllCache004) {
  std::string url = TEST_HTTP_URL;
  std::shared_ptr<ResponseCache> response_cache = std::make_shared<ResponseCache>(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath path = temp_dir.AppendASCII(TEMP_DIRECTORY_NAME);
  EXPECT_TRUE(base::CreateDirectory(path));
  ResponseCache::InitCacheDirectory(path);
  EXPECT_TRUE(base::PathExists(path));
  base::FilePath metadata_file = path.AppendASCII(METADATA_FILENAME);
  std::string metadata_content = TEST_NEW_CONTENT;
  base::WriteFile(metadata_file, metadata_content);

  base::FilePath cache_file = path.AppendASCII(CACHE_FILENAME);
  std::string cache_content = TEST_CONTENT;
  base::WriteFile(cache_file, cache_content);

  EXPECT_TRUE(base::PathExists(metadata_file));
  EXPECT_TRUE(base::PathExists(cache_file));

  ResponseCache::ClearAllCache();
  EXPECT_TRUE((*ResponseCache::cache_metadata_map_).empty());
  EXPECT_FALSE(base::PathExists(metadata_file));
  EXPECT_FALSE(base::PathExists(cache_file));
  base::DeletePathRecursively(path);
}

TEST_F(OhCodeCacheTest, Write002) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  (*ResponseCache::cache_metadata_map_).clear();
  response_cache->metadata_file_path_ = base::FilePath(INVALID_METADATA_PATH);
  response_cache->cache_file_path_ = base::FilePath(INVALID_CACHE_PATH);

  std::string response_body = TEST_CONTENT;
  std::map<std::string, std::string> headers = {{HEADER_TAG, TEST_TAGS[0]}, {HEADER_LAST_MODIFIED, TEST_TAGS[1]}};
  NextOp res = response_cache->Write(headers, response_body);
  EXPECT_EQ(res, NextOp::THROW_ERROR);

  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, Write003) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);

  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_out_->content_length_ = TEST_CONTENT_LENGTHS[0];

  std::string response_body = TEST_CONTENT;
  std::map<std::string, std::string> headers = {{HEADER_TAG, TEST_TAGS[0]}, {HEADER_LAST_MODIFIED, TEST_TAGS[1]}};
  response_cache->url_hash_ = TEST_HASHES[2];
  (*ResponseCache::cache_metadata_map_)
      .emplace(response_cache->url_hash_, response_cache->metadata_out_);

  NextOp res = response_cache->Write(headers, response_body);
  EXPECT_EQ(res, NextOp::THROW_ERROR);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, Write005) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  std::string response_body = TEST_CONTENT;
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_out_->content_length_ = TEST_CONTENT_LENGTHS[0];
  response_cache->metadata_out_->url_hash_ = response_cache->url_hash_;
  (*ResponseCache::cache_metadata_map_).emplace(response_cache->url_hash_, response_cache->metadata_out_);
  response_cache->metadata_file_path_ = base::FilePath(INVALID_METADATA_PATH);

  std::map<std::string, std::string> headers = {{HEADER_CONTENT_LENGTH, TEST_CONTENT_LENGTHS[0]}};
  NextOp res = response_cache->Write(headers, response_body);
  EXPECT_EQ(res, NextOp::THROW_ERROR);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, Write006) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath cache_dir = temp_dir.AppendASCII(CACHE_FILENAME);
  base::CreateDirectory(cache_dir);
  ResponseCache::InitCacheDirectory(cache_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  (*ResponseCache::cache_metadata_map_).clear();

  std::string response_body = TEST_CONTENT;
  std::map<std::string, std::string> headers = {{HEADER_TAG, TEST_TAGS[0]}};
  NextOp res = response_cache->Write(headers, response_body);
  EXPECT_TRUE(res == NextOp::WRITE_CODE_CACHE || res == NextOp::THROW_ERROR);
  ResponseCache::ClearAllCache();
  base::DeletePathRecursively(cache_dir);
}

TEST_F(OhCodeCacheTest, CanUseCache001) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  (*ResponseCache::cache_metadata_map_).clear();
  bool res = cache.CanUseCache();
  EXPECT_FALSE(res);
}

TEST_F(OhCodeCacheTest, CanUseCache002) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);

  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->url_hash_ = TEST_HASHES[2];
  (*ResponseCache::cache_metadata_map_)
      .emplace(response_cache->url_hash_, response_cache->metadata_out_);
  response_cache->cache_file_path_ = base::FilePath(INVALID_CACHE_PATH);
  bool res = response_cache->CanUseCache();
  EXPECT_FALSE(res);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, CanUseCache003) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  auto target_metadata = std::make_shared<ResponseCacheMetadata>();
  target_metadata->url_hash_ = TEST_HASHES[0];
  response_cache->url_hash_ = TEST_HASHES[0];
  (*ResponseCache::cache_metadata_map_)
      .emplace(response_cache->url_hash_, target_metadata);

  base::FilePath cache_file = response_cache->cache_file_path_;
  std::string cache_content = TEST_CONTENT;
  base::WriteFile(cache_file, cache_content);

  bool res = response_cache->CanUseCache();
  EXPECT_TRUE(res);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, CreateStream001) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  cache.metadata_file_path_ = base::FilePath(NON_EXIST_PATH);
  bool res = cache.CreateStream();
  EXPECT_FALSE(res);
}

TEST_F(OhCodeCacheTest, CreateStream003) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath metadata_file = temp_dir.AppendASCII(METADATA_FILENAME);
  base::WriteFile(metadata_file, "");
  cache.metadata_file_path_ = metadata_file;

  bool res = cache.CreateStream();
  EXPECT_TRUE(res);
  EXPECT_NE(cache.metadata_file_stream_, nullptr);

  cache.CloseStream();
  base::DeleteFile(metadata_file);
}

TEST_F(OhCodeCacheTest, FindMetadata001) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  (*ResponseCache::cache_metadata_map_).clear();
  cache.metadata_file_path_ = base::FilePath(INVALID_METADATA_PATH);
  bool res = cache.FindMetadata();
  EXPECT_FALSE(res);
}

TEST_F(OhCodeCacheTest, FindMetadata002) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  (*ResponseCache::cache_metadata_map_).clear();

  base::FilePath metadata_file = response_cache->metadata_file_path_;
  std::string url_hash = response_cache->url_hash_;
  std::string metadata_content = 
      CreateTestMetadataContent(url_hash, TEST_CONTENT_LENGTHS[1], TEST_TAGS[0],
          TEST_TAGS[1], TEST_TAGS[2], TEST_FILE_HASHES[0]);
  base::WriteFile(metadata_file, metadata_content);

  bool res = response_cache->FindMetadata();
  EXPECT_TRUE(res);
  EXPECT_EQ(response_cache->metadata_out_->url_hash_, url_hash);
  EXPECT_EQ(response_cache->metadata_out_->content_length_, TEST_CONTENT_LENGTHS[1]);
  EXPECT_EQ(response_cache->metadata_out_->e_tag_, TEST_TAGS[0]);

  base::DeleteFile(metadata_file);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, FindMetadata003) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  auto dummy_metadata = std::make_shared<ResponseCacheMetadata>();
  dummy_metadata->url_hash_ = TEST_HASHES[1];
  (*ResponseCache::cache_metadata_map_).emplace(TEST_HASHES[1], dummy_metadata);
  cache.url_hash_ = TEST_HASHES[0];

  bool res = cache.FindMetadata();
  EXPECT_FALSE(res);
}

TEST_F(OhCodeCacheTest, FindMetadata004) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  auto target_metadata = std::make_shared<ResponseCacheMetadata>();
  target_metadata->url_hash_ = TEST_HASHES[0];
  cache.url_hash_ = TEST_HASHES[0];
  (*ResponseCache::cache_metadata_map_).emplace(TEST_HASHES[0], target_metadata);

  bool res = cache.FindMetadata();
  EXPECT_TRUE(res);
  EXPECT_EQ(cache.metadata_out_, target_metadata);
}

TEST_F(OhCodeCacheTest, ReadContent001) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  cache.cache_file_path_ = base::FilePath(NON_EXIST_PATH);
  bool res = cache.ReadContent();
  EXPECT_FALSE(res);
}

TEST_F(OhCodeCacheTest, ReadContent002) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath fake_file = temp_dir.AppendASCII(TEMP_DIRECTORY_NAME);
  base::CreateDirectory(fake_file);
  cache.cache_file_path_ = fake_file;

  bool res = cache.ReadContent();
  EXPECT_FALSE(res);
  base::DeletePathRecursively(fake_file);
}

TEST_F(OhCodeCacheTest, ReadContent003) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath cache_file = temp_dir.AppendASCII(CACHE_FILENAME);
  std::string cache_content = TEST_CONTENT;
  base::WriteFile(cache_file, cache_content);

  cache.cache_file_path_ = cache_file;
  bool res = cache.ReadContent();
  EXPECT_TRUE(res);
  EXPECT_EQ(cache.response_body_out_, cache_content);

  base::DeleteFile(cache_file);
}

TEST_F(OhCodeCacheTest, ReadMetadata001) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  cache.metadata_file_stream_ = nullptr;
  bool res = cache.ReadMetadata();
  EXPECT_FALSE(res);
}

TEST_F(OhCodeCacheTest, ReadMetadata002) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath path = temp_dir.AppendASCII(METADATA_FILENAME);
  base::WriteFile(path, "");
  cache.metadata_file_path_ = path;
  cache.metadata_file_stream_ = std::make_shared<std::fstream>(path.AsUTF8Unsafe());
  bool res = cache.ReadMetadata();
  EXPECT_FALSE(res);
  base::DeleteFile(path);
}

TEST_F(OhCodeCacheTest, ReadMetadata003) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath path = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string invalid_metadata = "INVALID_FORMAT_DATA\n";
  base::WriteFile(path, invalid_metadata);
  cache.metadata_file_path_ = path;
  cache.metadata_file_stream_ = std::make_shared<std::fstream>(path.AsUTF8Unsafe());
  bool res = cache.ReadMetadata();
  EXPECT_TRUE(res);
  base::DeleteFile(path);
}

TEST_F(OhCodeCacheTest, ReadMetadata004) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  cache.metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath path = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string incomplete_metadata =
      std::string("URL:") + TEST_HASHES[0] + "|" +
      HEADER_CONTENT_LENGTH + TEST_CONTENT_LENGTHS[2] + "|" + HEADER_TAG + ":" +
      TEST_TAGS[0] + "\n";
  base::WriteFile(path, incomplete_metadata);
  cache.metadata_file_path_ = path;
  cache.metadata_file_stream_ = std::make_shared<std::fstream>(path.AsUTF8Unsafe());
  bool res = cache.ReadMetadata();
  EXPECT_TRUE(res);
  base::DeleteFile(path);
}

TEST_F(OhCodeCacheTest, ReadMetadata005) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath path = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string invalid_metadata = "INVALID_FORMAT_DATA\n";
  base::WriteFile(path, invalid_metadata);
  cache.metadata_file_path_ = path;
  cache.metadata_file_stream_ = std::make_shared<std::fstream>(path.AsUTF8Unsafe());
  bool res = cache.ReadMetadata();
  EXPECT_TRUE(res);
  base::DeleteFile(path);
}

TEST_F(OhCodeCacheTest, ReadMetadata006) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  cache.metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath path = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string valid_metadata = CreateTestMetadataContent(
      TEST_HASHES[0], TEST_CONTENT_LENGTHS[0], TEST_TAGS[0], TEST_TAGS[1],
      TEST_TAGS[2], TEST_FILE_HASHES[0]);
  base::WriteFile(path, valid_metadata);
  cache.metadata_file_path_ = path;
  cache.metadata_file_stream_ =
      std::make_shared<std::fstream>(path.AsUTF8Unsafe());

  bool res = cache.ReadMetadata();
  EXPECT_TRUE(res);
  EXPECT_EQ(cache.metadata_out_->url_hash_, TEST_HASHES[0]);
  EXPECT_EQ(cache.metadata_out_->content_length_, TEST_CONTENT_LENGTHS[0]);
  EXPECT_EQ(cache.metadata_out_->e_tag_, TEST_TAGS[0]);
  base::DeleteFile(path);
}

TEST_F(OhCodeCacheTest, NeedUpdate001) {
  ResponseCache cache(TEST_HTTP_URL);
  cache.metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_in_->content_length_ = TEST_CONTENT_LENGTHS[2];
  cache.metadata_out_->content_length_ = TEST_CONTENT_LENGTHS[3];
  bool res = cache.NeedUpdate();
  EXPECT_TRUE(res);
}

TEST_F(OhCodeCacheTest, NeedUpdate002) {
  ResponseCache cache(TEST_HTTP_URL);
  cache.metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_in_->e_tag_ = TEST_TAGS[0];
  cache.metadata_out_->e_tag_ = TEST_TAGS[1];
  bool res = cache.NeedUpdate();
  EXPECT_TRUE(res);
}

TEST_F(OhCodeCacheTest, NeedUpdate003) {
  ResponseCache cache(TEST_HTTP_URL);
  cache.metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_in_->last_modified_ = TEST_TAGS[0];
  cache.metadata_out_->last_modified_ = TEST_TAGS[1];
  bool res = cache.NeedUpdate();
  EXPECT_TRUE(res);
}

TEST_F(OhCodeCacheTest, NeedUpdate004) {
  ResponseCache cache(TEST_HTTP_URL);
  cache.metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_in_->access_control_allow_origin_ = ACCESS_CONTROL_VALUES[0];
  cache.metadata_out_->access_control_allow_origin_ = ACCESS_CONTROL_VALUES[1];
  bool res = cache.NeedUpdate();
  EXPECT_TRUE(res);
}

TEST_F(OhCodeCacheTest, NeedUpdate005) {
  ResponseCache cache(TEST_HTTP_URL);
  cache.metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_in_->file_hash_ = TEST_FILE_HASHES[0];
  cache.metadata_out_->file_hash_ = TEST_FILE_HASHES[1];
  bool res = cache.NeedUpdate();
  EXPECT_TRUE(res);
}

TEST_F(OhCodeCacheTest, NeedUpdate006) {
  ResponseCache cache(TEST_HTTP_URL);
  cache.metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  cache.metadata_in_->content_length_ = TEST_CONTENT_LENGTHS[2];
  cache.metadata_out_->content_length_ = TEST_CONTENT_LENGTHS[2];
  cache.metadata_in_->e_tag_ = TEST_TAGS[0];
  cache.metadata_out_->e_tag_ = TEST_TAGS[0];
  cache.metadata_in_->last_modified_ = TEST_TAGS[0];
  cache.metadata_out_->last_modified_ = TEST_TAGS[0];
  cache.metadata_in_->access_control_allow_origin_ = ACCESS_CONTROL_VALUES[0];
  cache.metadata_out_->access_control_allow_origin_ = ACCESS_CONTROL_VALUES[0];
  cache.metadata_in_->file_hash_ = TEST_FILE_HASHES[0];
  cache.metadata_out_->file_hash_ = TEST_FILE_HASHES[0];

  bool res = cache.NeedUpdate();
  EXPECT_FALSE(res);
}

TEST_F(OhCodeCacheTest, DoCreate001) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  cache.metadata_file_path_ = base::FilePath(INVALID_METADATA_PATH);
  cache.cache_file_path_ = base::FilePath(INVALID_CACHE_PATH);
  cache.metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  cache.url_hash_ = TEST_HASHES[0];

  NextOp res = cache.DoCreate();
  EXPECT_EQ(res, NextOp::THROW_ERROR);
}

TEST_F(OhCodeCacheTest, DoCreate002) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath metadata_file = temp_dir.AppendASCII(METADATA_FILENAME);
  cache.metadata_file_path_ = metadata_file;
  cache.cache_file_path_ = base::FilePath(INVALID_CACHE_PATH);
  cache.metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  cache.url_hash_ = TEST_HASHES[0];

  NextOp res = cache.DoCreate();
  EXPECT_EQ(res, NextOp::THROW_ERROR);

  base::DeleteFile(metadata_file);
}

TEST_F(OhCodeCacheTest, DoCreate003) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  response_cache->metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->url_hash_ = TEST_HASHES[0];
  response_cache->response_body_in_ = TEST_CONTENT;
  NextOp res = response_cache->DoCreate();
  EXPECT_EQ(res, NextOp::WRITE_CODE_CACHE);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoUpdateMetadataFail) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  response_cache->metadata_file_path_ = base::FilePath(INVALID_METADATA_PATH);
  response_cache->metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->url_hash_ = TEST_HASHES[0];
  response_cache->response_body_in_ = TEST_CONTENT;
  NextOp res = response_cache->DoUpdate();
  EXPECT_EQ(res, NextOp::THROW_ERROR);

  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoUpdateWriteContentFail) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  response_cache->metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->url_hash_ = TEST_HASHES[0];
  response_cache->response_body_in_ = TEST_CONTENT;
  response_cache->cache_file_path_ = base::FilePath(INVALID_CACHE_PATH);
  base::FilePath metadata_file = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string metadata_content = 
      CreateTestMetadataContent(TEST_HASHES[0], TEST_CONTENT_LENGTHS[1], TEST_TAGS[0],
           TEST_TAGS[1], TEST_TAGS[2], TEST_FILE_HASHES[0]);
  base::WriteFile(metadata_file, metadata_content);
  response_cache->metadata_file_path_ = metadata_file;
  NextOp res = response_cache->DoUpdate();
  EXPECT_EQ(res, NextOp::THROW_ERROR);

  base::DeleteFile(metadata_file);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoUpdateSuccess) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath cache_dir = temp_dir.AppendASCII(TEST_CONTENT);
  base::CreateDirectory(cache_dir);
  ResponseCache::InitCacheDirectory(cache_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  response_cache->metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->url_hash_ = TEST_HASHES[0];
  response_cache->response_body_in_ = TEST_NEW_CONTENT;
  base::FilePath metadata_file = cache_dir.AppendASCII(METADATA_FILENAME);
  std::string metadata_content = 
      CreateTestMetadataContent(TEST_HASHES[0], TEST_CONTENT_LENGTHS[1], TEST_TAGS[0],
          TEST_TAGS[1], TEST_TAGS[2], TEST_FILE_HASHES[0]);
  base::WriteFile(metadata_file, metadata_content);
  response_cache->metadata_file_path_ = metadata_file;
  response_cache->cache_file_path_ = cache_dir.AppendASCII(TEST_HASHES[2]);

  NextOp res = response_cache->DoUpdate();
  EXPECT_EQ(res, NextOp::WRITE_CODE_CACHE);
  EXPECT_TRUE(base::PathExists(response_cache->cache_file_path_));
  std::string content;
  EXPECT_TRUE(base::ReadFileToString(response_cache->cache_file_path_, &content));
  EXPECT_EQ(content, TEST_NEW_CONTENT);
  ResponseCache::ClearAllCache();
  base::DeletePathRecursively(cache_dir);
}

TEST_F(OhCodeCacheTest, DoUpdateMetadataCreateStreamFail) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  cache.metadata_file_path_ = base::FilePath(INVALID_METADATA_PATH);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  bool res = cache.DoUpdateMetadata();
  EXPECT_FALSE(res);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoUpdateMetadataTempFileInvalid) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  base::FilePath readonly_dir = temp_dir.AppendASCII("readonly");
  base::CreateDirectory(readonly_dir);
#ifndef _WIN32
  base::SetPosixFilePermissions(readonly_dir, base::FILE_PERMISSION_READ_BY_USER);
#endif
  (*ResponseCache::cache_dir_path_) = std::make_unique<base::FilePath>(readonly_dir);
  base::FilePath metadata_file = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string metadata_content = CreateTestMetadataContent(
      TEST_HASHES[0], TEST_CONTENT_LENGTHS[1], TEST_TAGS[0], TEST_TAGS[1],
      TEST_TAGS[2], TEST_FILE_HASHES[0]);
  base::WriteFile(metadata_file, metadata_content);
  response_cache->metadata_file_path_ = metadata_file;
  bool res = response_cache->DoUpdateMetadata();
  EXPECT_FALSE(res);

  base::DeleteFile(metadata_file);
  base::DeletePathRecursively(readonly_dir);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoUpdateMetadataReadMetadataFail) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  base::FilePath metadata_file = temp_dir.AppendASCII(METADATA_FILENAME);
  base::WriteFile(metadata_file, "");
  response_cache->metadata_file_path_ = metadata_file;
  bool res = response_cache->DoUpdateMetadata();
  EXPECT_FALSE(res);
  base::DeleteFile(metadata_file);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoUpdateMetadataWriteFail) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_out_->url_hash_ = TEST_HASHES[0];
  response_cache->url_hash_ = TEST_HASHES[0];
  response_cache->metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  base::FilePath metadata_file = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string metadata_content = CreateTestMetadataContent(
      TEST_HASHES[0], TEST_CONTENT_LENGTHS[1], TEST_TAGS[0], TEST_TAGS[1],
      TEST_TAGS[2], TEST_FILE_HASHES[0]);
  base::WriteFile(metadata_file, metadata_content);
  response_cache->metadata_file_path_ = metadata_file;
  base::FilePath temp_file_path = temp_dir.AppendASCII(kTempFilePath);
  base::CreateDirectory(temp_file_path);
  bool res = response_cache->DoUpdateMetadata();
  EXPECT_FALSE(res);

  base::DeleteFile(metadata_file);
  base::DeletePathRecursively(temp_file_path);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoUpdateMetadataReplaceFileFail) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_out_->url_hash_ = TEST_HASHES[0];
  response_cache->url_hash_ = TEST_HASHES[0];
  response_cache->metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  base::FilePath metadata_file = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string metadata_content = CreateTestMetadataContent(
      TEST_HASHES[0], TEST_CONTENT_LENGTHS[1], TEST_TAGS[0], TEST_TAGS[1],
      TEST_TAGS[2], TEST_FILE_HASHES[0]);
  base::WriteFile(metadata_file, metadata_content);
  response_cache->metadata_file_path_ = metadata_file;
  base::FilePath target_as_dir = temp_dir.AppendASCII(TEMP_DIRECTORY_NAME);
  base::CreateDirectory(target_as_dir);
  response_cache->metadata_file_path_ = target_as_dir;
  bool res = response_cache->DoUpdateMetadata();
  EXPECT_FALSE(res);

  base::DeleteFile(metadata_file);
  base::DeletePathRecursively(target_as_dir);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoUpdateMetadataSuccess) {
  std::string url = TEST_HTTP_URL;
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  ResponseCache::InitCacheDirectory(temp_dir);
  auto response_cache = ResponseCache::CreateResponseCache(url);
  ASSERT_NE(response_cache, nullptr);
  response_cache->metadata_out_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_out_->url_hash_ = TEST_HASHES[0];
  response_cache->url_hash_ = TEST_HASHES[0];
  response_cache->metadata_in_ = std::make_shared<ResponseCacheMetadata>();
  response_cache->metadata_in_->url_hash_ = TEST_HASHES[0];
  response_cache->metadata_in_->content_length_ = TEST_CONTENT_LENGTHS[0];
  response_cache->metadata_in_->e_tag_ = TEST_TAGS[0];
  base::FilePath metadata_file = temp_dir.AppendASCII(METADATA_FILENAME);
  std::string metadata_content = CreateTestMetadataContent(
      TEST_HASHES[0], TEST_CONTENT_LENGTHS[1], TEST_TAGS[0], TEST_TAGS[1],
      TEST_TAGS[2], TEST_FILE_HASHES[0]);
  base::WriteFile(metadata_file, metadata_content);
  response_cache->metadata_file_path_ = metadata_file;
  bool res = response_cache->DoUpdateMetadata();
  EXPECT_TRUE(res);
  auto it = (*ResponseCache::cache_metadata_map_).find(TEST_HASHES[0]);
  EXPECT_NE(it, (*ResponseCache::cache_metadata_map_).end());
  EXPECT_EQ(it->second->content_length_, TEST_CONTENT_LENGTHS[0]);
  EXPECT_EQ(it->second->e_tag_, TEST_TAGS[0]);
  base::DeleteFile(metadata_file);
  ResponseCache::ClearAllCache();
}

TEST_F(OhCodeCacheTest, DoWriteIntoFile001) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath test_file = temp_dir.AppendASCII(TEST_FILENAME);
  if (base::PathExists(test_file)) {
    base::DeleteFile(test_file);
  }
  bool res = cache.DoWriteIntoFile(test_file, TEST_CONTENT);
  EXPECT_TRUE(res);
  std::string content;
  EXPECT_TRUE(base::ReadFileToString(test_file, &content));
  EXPECT_EQ(content, TEST_CONTENT);
  base::DeleteFile(test_file);
}

TEST_F(OhCodeCacheTest, DoWriteIntoFile003) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath fake_file = temp_dir.AppendASCII(TEMP_DIRECTORY_NAME);
  base::CreateDirectory(fake_file);
  bool res = cache.DoWriteIntoFile(fake_file, TEST_CONTENT);
  EXPECT_FALSE(res);
  base::DeletePathRecursively(fake_file);
}

TEST_F(OhCodeCacheTest, DoWriteIntoFile004) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath test_file = temp_dir.AppendASCII(TEST_FILENAME);
  bool res = cache.DoWriteIntoFile(test_file, TEST_CONTENT);
  EXPECT_TRUE(res);
  std::string content;
  EXPECT_TRUE(base::ReadFileToString(test_file, &content));
  EXPECT_EQ(content, TEST_CONTENT);
  base::DeleteFile(test_file);
}

TEST_F(OhCodeCacheTest, DoWriteIntoFile005) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath test_file = temp_dir.AppendASCII(TEST_FILENAME);
  std::string initial_data = TEST_CONTENT;
  base::WriteFile(test_file, initial_data);
  EXPECT_TRUE(base::PathExists(test_file));
  bool res = cache.DoWriteIntoFile(test_file, TEST_TAGS[0]);
  EXPECT_TRUE(res);
  std::string content;
  EXPECT_TRUE(base::ReadFileToString(test_file, &content));
  EXPECT_EQ(content, std::string(TEST_CONTENT)+TEST_TAGS[0]);
  base::DeleteFile(test_file);
}

TEST_F(OhCodeCacheTest, DeleteCacheFile001) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  cache.cache_file_path_ = base::FilePath(NON_EXIST_PATH);
  bool res = cache.DeleteCacheFile();
  EXPECT_TRUE(res);
}

TEST_F(OhCodeCacheTest, DeleteCacheFile002) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath test_file = temp_dir.AppendASCII(TEST_FILENAME);
  std::string file_content = TEST_CONTENT;
  base::WriteFile(test_file, file_content);
  EXPECT_TRUE(base::PathExists(test_file));
  cache.cache_file_path_ = test_file;
  bool res = cache.DeleteCacheFile();
  EXPECT_TRUE(res);
  EXPECT_FALSE(base::PathExists(test_file));
}

TEST_F(OhCodeCacheTest, DeleteCacheFile003) {
  std::string url = TEST_HTTP_URL;
  ResponseCache cache(url);
  base::FilePath temp_dir;
  ASSERT_TRUE(base::GetTempDir(&temp_dir));
  base::FilePath readonly_dir = temp_dir.AppendASCII("readonly");
  base::CreateDirectory(readonly_dir);
  base::FilePath test_file = readonly_dir.AppendASCII(TEST_FILENAME);
  std::string file_content = TEST_CONTENT;
  base::WriteFile(test_file, file_content);
  EXPECT_TRUE(base::PathExists(test_file));
  cache.cache_file_path_ = test_file;
  bool res = cache.DeleteCacheFile();
  EXPECT_TRUE(res);
  base::DeletePathRecursively(readonly_dir);
}

TEST_F(OhCodeCacheTest, InputStreamRead001) {
  std::string data = TEST_CHARS[0];
  InputStream stream(data);
  auto buffer = base::MakeRefCounted<net::IOBufferWithSize>(data.size());
  buffer->data()[0] = '\0';
  int bytes_read = 0;
  bool res = stream.Read(buffer.get(), READ_SIZES[0], &bytes_read,
      base::BindOnce([](int read) { EXPECT_EQ(read, READ_SIZES[0]); }));
  EXPECT_TRUE(res);
}

TEST_F(OhCodeCacheTest, InputStreamReadNoData) {
  std::string data = TEST_CHARS[0];
  InputStream stream(data);
  auto buffer = base::MakeRefCounted<net::IOBufferWithSize>(data.size());
  int bytes_read = 0;
  bool res = stream.Read(buffer.get(), data.size(), &bytes_read,
      base::BindOnce([](int read) { EXPECT_EQ(read, READ_SIZES[0]); }));
  EXPECT_TRUE(res);
  EXPECT_EQ(bytes_read, READ_SIZES[0]);
  int bytes_read2 = 0;
  bool res2 = stream.Read(buffer.get(), data.size(), &bytes_read2,
      base::BindOnce([](int read) { EXPECT_EQ(read, 0); }));
  EXPECT_FALSE(res2);
  EXPECT_EQ(bytes_read2, 0);
}

TEST_F(OhCodeCacheTest, InputStreamReadPartial) {
  std::string data = TEST_CHARS[2];
  InputStream stream(data);
  auto buffer = base::MakeRefCounted<net::IOBufferWithSize>(READ_SIZES[0]);
  int bytes_read = 0;
  bool res = stream.Read(buffer.get(), READ_SIZES[0], &bytes_read,
      base::BindOnce([](int read) { EXPECT_EQ(read, READ_SIZES[0]); }));
  EXPECT_TRUE(res);
  EXPECT_EQ(bytes_read, READ_SIZES[0]);
  EXPECT_EQ(strncmp(buffer->data(), TEST_CHARS[0], READ_SIZES[0]), 0);

  auto buffer2 = base::MakeRefCounted<net::IOBufferWithSize>(READ_SIZES[1]);
  int bytes_read2 = 0;
  bool res2 = stream.Read(buffer2.get(), READ_SIZES[2], &bytes_read2,
      base::BindOnce([](int read) { EXPECT_EQ(read, READ_SIZES[1]); }));
  EXPECT_TRUE(res2);
  EXPECT_EQ(bytes_read2, READ_SIZES[1]);
  EXPECT_EQ(strncmp(buffer2->data(), TEST_CHARS[1], READ_SIZES[1]), 0);
}

TEST_F(OhCodeCacheTest, InputStreamReadExceedLength) {
  std::string data = TEST_CHARS[0];
  InputStream stream(data);
  auto buffer = base::MakeRefCounted<net::IOBufferWithSize>(READ_SIZES[2]);
  int bytes_read = 0;
  bool res = stream.Read(buffer.get(), READ_SIZES[2], &bytes_read,
      base::BindOnce([](int read) { EXPECT_EQ(read, READ_SIZES[0]); }));
  EXPECT_TRUE(res);
  EXPECT_EQ(bytes_read, READ_SIZES[0]);
  EXPECT_EQ(strncmp(buffer->data(), TEST_CHARS[0], READ_SIZES[0]), 0);
}

TEST_F(OhCodeCacheTest, InputStreamReadZeroLength) {
  std::string data = TEST_CHARS[2];
  InputStream stream(data);
  auto buffer = base::MakeRefCounted<net::IOBufferWithSize>(1);
  int bytes_read = 0;
  bool res = stream.Read(buffer.get(), 0, &bytes_read,
      base::BindOnce([](int read) { EXPECT_EQ(read, 0); }));
  EXPECT_FALSE(res);
}

TEST_F(OhCodeCacheTest, InputStreamReadEmptyData) {
  std::string data = "";
  InputStream stream(data);
  auto buffer = base::MakeRefCounted<net::IOBufferWithSize>(1);
  int bytes_read = 0;
  bool res = stream.Read(buffer.get(), 1, &bytes_read,
      base::BindOnce([](int read) { EXPECT_EQ(read, 0); }));
  EXPECT_FALSE(res);
  EXPECT_EQ(bytes_read, 0);
}

}  // namespace oh_code_cache