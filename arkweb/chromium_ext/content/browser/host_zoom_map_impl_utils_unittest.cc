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
#include "arkweb/chromium_ext/content/browser/host_zoom_map_impl_utils.h"

#include <gtest/gtest.h>

#include "arkweb/build/features/features.h"

namespace content {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
class HostZoomMapImplUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
    impl_ = std::make_unique<HostZoomMapImpl>();
    utils_ = std::make_unique<HostZoomMapImplUtils>(impl_.get());
  }

  void TearDown() override {}

  std::unique_ptr<HostZoomMapImpl> impl_;
  std::unique_ptr<HostZoomMapImplUtils> utils_;
};

/**
 * @tc.name: IsZoomTooFast_FrequencyLimit
 * @tc.desc: Test HostZoomMapImplUtils::IsZoomTooFast
 * @tc.type: FUNC
 */
TEST_F(HostZoomMapImplUtilsTest, IsZoomTooFast_FrequencyLimit) {
  std::string host = "test.com";
  base::Time now = base::Time::Now();
  double level = 1.0;
  EXPECT_FALSE(utils_->IsZoomTooFast(host, now, level));
  EXPECT_TRUE(utils_->IsZoomTooFast(host, now, level));
}

/**
 * @tc.name: IsZoomTooFast_FrequencyLimit1
 * @tc.desc: Test HostZoomMapImplUtils::IsZoomTooFast
 * @tc.type: FUNC
 */
TEST_F(HostZoomMapImplUtilsTest, IsZoomTooFast_FrequencyLimit1) {
  std::string host = "test.com";
  base::Time now = base::Time::Now();
  double level = 1.0;
  EXPECT_FALSE(utils_->IsZoomTooFast(host, now, level));
  base::Time second_time = now + base::Milliseconds(20);
  EXPECT_TRUE(utils_->IsZoomTooFast(host, second_time, level));
}

/**
 * @tc.name: IsZoomTooFast_FrequencyLimit2
 * @tc.desc: Test HostZoomMapImplUtils::IsZoomTooFast
 * @tc.type: FUNC
 */
TEST_F(HostZoomMapImplUtilsTest, IsZoomTooFast_FrequencyLimit2) {
  std::string host = "test.com";
  base::Time now = base::Time::Now();
  double level = 1.0;
  EXPECT_FALSE(utils_->IsZoomTooFast(host, now, level));
  base::Time second_time = now + base::Milliseconds(30);
  EXPECT_FALSE(utils_->IsZoomTooFast(host, second_time, level));
}

/**
 * @tc.name: IsZoomTooFast_FrequencyLimit3
 * @tc.desc: Test HostZoomMapImplUtils::IsZoomTooFast
 * @tc.type: FUNC
 */
TEST_F(HostZoomMapImplUtilsTest, IsZoomTooFast_FrequencyLimit3) {
  std::string host = "test.com";
  base::Time now = base::Time::Now();
  double level = 1.0;
  EXPECT_FALSE(utils_->IsZoomTooFast(host, now, level));
  base::Time second_time = now + base::Milliseconds(40);
  EXPECT_FALSE(utils_->IsZoomTooFast(host, second_time, level));
}

/**
 * @tc.name: IsZoomTooFast_FrequencyLimit4
 * @tc.desc: Test HostZoomMapImplUtils::IsZoomTooFast
 * @tc.type: FUNC
 */
TEST_F(HostZoomMapImplUtilsTest, IsZoomTooFast_FrequencyLimit4) {
  std::string host = "test.com";
  base::Time time = base::Time::Now();
  double zoom_level = 1.0;
  EXPECT_FALSE(utils_->IsZoomTooFast(host, time, zoom_level));
  time += base::Milliseconds(20);
  EXPECT_TRUE(utils_->IsZoomTooFast(host, time, zoom_level));
  time += base::Milliseconds(20);
  EXPECT_FALSE(utils_->IsZoomTooFast(host, time, zoom_level));
  time += base::Milliseconds(25);
  EXPECT_TRUE(utils_->IsZoomTooFast(host, time, zoom_level));
  time += base::Milliseconds(30);
  EXPECT_FALSE(utils_->IsZoomTooFast(host, time, zoom_level));
}
#endif
}  // namespace content