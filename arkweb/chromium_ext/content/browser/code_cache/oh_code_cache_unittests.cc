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
} // oh_code_cache
