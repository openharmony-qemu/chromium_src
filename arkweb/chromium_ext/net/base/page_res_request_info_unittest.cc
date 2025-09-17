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
 
#include "gtest/gtest.h"
#include <string>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#define private public
#include "arkweb/chromium_ext/net/base/page_res_request_info.h"
#undef private

namespace ohos_prp_preload {

class PRRequestInfoTest : public ::testing::Test {
 protected:
  PRRequestInfo info_;
};

TEST_F(PRRequestInfoTest, CacheInfoSetAndGet) {
  info_.set_cache_info(PRRequestCacheType::NEGOTIATION_CACHE, 12345, "etag", "lastmod");
  CacheInfo ci = info_.cache_info();
  EXPECT_EQ(ci.cache_type_, PRRequestCacheType::NEGOTIATION_CACHE);
  EXPECT_EQ(ci.freshness_life_times_, 12345);
  EXPECT_EQ(ci.e_tag_, "etag");
  EXPECT_EQ(ci.last_modified_, "lastmod");
}

TEST_F(PRRequestInfoTest, RequestEndTime) {
  info_.set_request_start_time(100);
  info_.set_request_end_time(100);
  EXPECT_EQ(info_.request_end_time(), 0);
  
  info_.set_request_end_time(101);
  EXPECT_EQ(info_.request_end_time(), 101);

  info_.set_request_end_time(10);
  EXPECT_EQ(info_.request_end_time(), 0);
}

TEST_F(PRRequestInfoTest, PreloadFlagSetAndOr) {
  info_.set_preload_flag(PRRequestFlags::PRPP_FLAGS_HDR_NOT_MATCH);
  EXPECT_EQ(info_.preload_flag(), PRRequestFlags::PRPP_FLAGS_HDR_NOT_MATCH);
  info_.or_preload_flag(PRRequestFlags::PRPP_FLAGS_VISIBLE);
  EXPECT_EQ(info_.preload_flag(), PRRequestFlags::PRPP_FLAGS_VISIBLE | PRRequestFlags::PRPP_FLAGS_HDR_NOT_MATCH);
}

TEST_F(PRRequestInfoTest, PreconnectNumCounters) {
  info_.add_preconnect_num(true);
  info_.add_preconnect_num(false);
  info_.add_reused_preconnect_num(true);
  info_.add_reused_preconnect_num(false);
  info_.compute_limit_num();
  // reused_preconnect_num_ = 1
  EXPECT_EQ(info_.limit_num_, 1);
}

TEST_F(PRRequestInfoTest, PreconnectNumCounters_Max) {
  info_.add_preconnect_num(true);
  for (int i = 0; i < MAX_PRECONNECT_COUNT; i++) {
    info_.add_reused_preconnect_num(true);
  }
  info_.compute_limit_num();
  // reused_preconnect_num_ = MAX_PRECONNECT_COUNT
  EXPECT_EQ(info_.limit_num_, MAX_PRECONNECT_COUNT);
}

TEST_F(PRRequestInfoTest, PreconnectNumCounters_Zero) {
  info_.add_preconnect_num(true);
  info_.add_reused_preconnect_num(false);
  info_.compute_limit_num();
  // reused_preconnect_num_ = 0
  EXPECT_EQ(info_.limit_num_, 0);
}

TEST_F(PRRequestInfoTest, PreconnectNumCounters_Zero1) {
  info_.add_preconnect_num(false);
  info_.add_reused_preconnect_num(true);
  info_.compute_limit_num();
  // reused_preconnect_num_ = 0
  EXPECT_EQ(info_.limit_num_, 0);
}

TEST_F(PRRequestInfoTest, PreconnectAndReusedSocketFlags) {
  info_.set_is_preconnect(true);
  EXPECT_TRUE(info_.is_preconnect());
  info_.set_is_preconnect(false);
  EXPECT_FALSE(info_.is_preconnect());

  info_.set_is_reused_sokcet(true);
  EXPECT_TRUE(info_.is_reused_sokcet());
  info_.set_is_reused_sokcet(false);
  EXPECT_FALSE(info_.is_reused_sokcet());
}

TEST_F(PRRequestInfoTest, ExtraRequestHeadersSetAndGet) {
  net::HttpRequestHeaders headers;
  headers.SetHeader("test1", "value1");
  info_.set_extra_request_headers(headers);
  EXPECT_TRUE(info_.extra_request_headers().HasHeader("test1"));
  EXPECT_TRUE(info_.extra_request_headers().GetHeader("test1").has_value());
  EXPECT_EQ(info_.extra_request_headers().GetHeader("test1").value(), "value1");
}
} // namespace ohos_prp_preload