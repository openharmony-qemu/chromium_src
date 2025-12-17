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

#include "content/browser/renderer_host/back_forward_cache_impl.h"
#include "content/browser/renderer_host/ark_web_back_forward_cache_impl.h"
#include "content/public/common/content_switches.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "base/time/time.h"
#include "base/command_line.h"

class ArkWebBackForwardCacheImplTest : public ::testing::Test {
 public:
  void SetUp() override {
    back_forward_cache_impl_ = std::make_unique<content::BackForwardCacheImpl>(nullptr);
  }

  void TearDown() override {
    back_forward_cache_impl_.reset();
  }

 protected:
  std::unique_ptr<content::BackForwardCacheImpl> back_forward_cache_impl_;
};

TEST_F(ArkWebBackForwardCacheImplTest, TestGetArkWebBackForwardCacheFeatures)
{
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableCacheNativeEmbed);
  blink::scheduler::WebSchedulerTrackedFeatures features;
  bool result = GetArkWebBackForwardCacheFeatures(features);
  EXPECT_TRUE(result);
  auto feature = blink::scheduler::StringToFeature("EnableCacheNativeEmbed");
  EXPECT_TRUE(feature.has_value());
  auto it = std::find(features.begin(), features.end(), feature.value());
  EXPECT_NE(it, features.end());
}

TEST_F(ArkWebBackForwardCacheImplTest, TestGetArkWebBackForwardCacheFeatures2)
{
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableCacheMediaTakeOver);
  blink::scheduler::WebSchedulerTrackedFeatures features;
  bool result = GetArkWebBackForwardCacheFeatures(features);
  EXPECT_TRUE(result);
  auto feature = blink::scheduler::StringToFeature("EnableCacheMediaTakeOver");
  EXPECT_TRUE(feature.has_value());
  auto it = std::find(features.begin(), features.end(), feature.value());
  EXPECT_NE(it, features.end());
}

namespace content {

namespace {

TEST_F(ArkWebBackForwardCacheImplTest, TestSetCacheSize)
{
  back_forward_cache_impl_->SetCacheSize(-1);
  EXPECT_EQ(back_forward_cache_impl_->size_, 0);
  testing::internal::CaptureStderr();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("BackForwardCacheImpl set backforward cache size: 0"), std::string::npos);
}

TEST_F(ArkWebBackForwardCacheImplTest, TestSetCacheSize2)
{
  back_forward_cache_impl_->SetCacheSize(60);
  EXPECT_EQ(back_forward_cache_impl_->size_, 50);
  testing::internal::CaptureStderr();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("BackForwardCacheImpl set backforward cache size: 50"), std::string::npos);
}

TEST_F(ArkWebBackForwardCacheImplTest, TestArkWebGetTimeToLiveInBackForwardCache)
{
  base::TimeDelta ttl = back_forward_cache_impl_->ArkWebGetTimeToLiveInBackForwardCache();
  EXPECT_EQ(ttl, base::Seconds(600));
}

TEST_F(ArkWebBackForwardCacheImplTest, TestArkWebGetTimeToLiveInBackForwardCache2)
{
  base::CommandLine::ForCurrentProcess()->AppendSwitch(switches::kEnableBFCache);
  base::TimeDelta ttl = back_forward_cache_impl_->ArkWebGetTimeToLiveInBackForwardCache();
  EXPECT_EQ(ttl, base::Seconds(back_forward_cache_impl_->time_to_live_));
}

}  // namespace

}  // namespace content