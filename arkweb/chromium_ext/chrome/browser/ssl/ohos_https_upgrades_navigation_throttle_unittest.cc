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

#include "chrome/browser/ssl/ohos_https_upgrades_navigation_throttle.h"
#include "chrome/browser/ssl/https_only_mode_tab_helper.h"
#include "chrome/browser/ssl/ohos_https_upgrades_helper.h"
#include "content/public/browser/navigation_throttle.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/test/mock_navigation_handle.h"

class OhosHttpsUpgradesNavigationThrottleTest : public ::testing::Test {};

TEST_F(OhosHttpsUpgradesNavigationThrottleTest, MaybeCreateThrottleForTest) {
  GURL http_url("ftp://example.com");
  content::MockNavigationHandle handle(http_url, nullptr);
  std::unique_ptr<OhosHttpsUpgradesNavigationThrottle> throttle =
      OhosHttpsUpgradesNavigationThrottle::MaybeCreateThrottleFor(&handle);

  EXPECT_EQ(throttle, nullptr);
}

TEST_F(OhosHttpsUpgradesNavigationThrottleTest, MaybeCreateThrottleForTest2) {
  GURL http_url("ftp://example.com");
  content::MockNavigationHandle handle(http_url, nullptr);
  handle.IsInPrimaryMainFrame();
  std::unique_ptr<OhosHttpsUpgradesNavigationThrottle> throttle =
      OhosHttpsUpgradesNavigationThrottle::MaybeCreateThrottleFor(&handle);

  EXPECT_EQ(throttle, nullptr);
}

TEST_F(OhosHttpsUpgradesNavigationThrottleTest, MaybeCreateThrottleForTest3) {
  GURL http_url("ftp://example.com");
  content::MockNavigationHandle handle(http_url, nullptr);
  handle.IsSameDocument();
  std::unique_ptr<OhosHttpsUpgradesNavigationThrottle> throttle =
      OhosHttpsUpgradesNavigationThrottle::MaybeCreateThrottleFor(&handle);

  EXPECT_EQ(throttle, nullptr);
}
