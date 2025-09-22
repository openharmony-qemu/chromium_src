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
using namespace testing;

#include "content/browser/code_cache/oh_code_cache.h"

namespace oh_code_cache {

class OhCodeCacheTest : public ::testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}
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
} // oh_code_cache
