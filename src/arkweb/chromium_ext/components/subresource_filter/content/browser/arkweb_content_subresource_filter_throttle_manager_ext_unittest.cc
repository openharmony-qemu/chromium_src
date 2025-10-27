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
#include <string>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "arkweb/chromium_ext/components/subresource_filter/content/browser/arkweb_content_subresource_filter_throttle_manager_ext.h"
#include "content/public/test/mock_navigation_handle.h"
#include "components/subresource_filter/content/shared/browser/activation_state_computing_navigation_throttle.h"
#include "components/subresource_filter/content/shared/browser/page_load_statistics.h"

namespace subresource_filter {
class MockNavigationHandleTest : public content::MockNavigationHandle {
public:
 MOCK_METHOD(bool, IsDownload, ());
 MOCK_METHOD(bool, IsInMainFrame, (), (const));
 MOCK_METHOD(content::WebContents*, GetWebContents, ());
};

class ArkWebContentSubresourceFilterThrottleManagerExtTest: public ContentSubresourceFilterThrottleManagerTest {
public:
  void SetUp() override {
    ContentSubresourceFilterThrottleManagerTest::SetUp();
    auto* throttle_manager =
      ContentSubresourceFilterThrottleManager::FromPage(main_rfh()->GetPage());
    ASSERT_TRUE(throttle_manager);
    ext = throttle_manager->AsArkWebContentSubresourceFilterThrottleManagerExt();
    ASSERT_TRUE(ext);
  }

  ArkWebContentSubresourceFilterThrottleManagerExt* ext;
  MockNavigationHandleTest handle;
};

INSTANTIATE_TEST_SUITE_P(All,
                         ArkWebContentSubresourceFilterThrottleManagerExtTest,
                         ::testing::Values(WILL_START_REQUEST,
                                           WILL_PROCESS_RESPONSE));
TEST_P(ArkWebContentSubresourceFilterThrottleManagerExtTest, ReportSubresourceMap) {
  CreateSubframeWithTestNavigation(GURL("https://www.example.com/not_disallowed.html"), main_rfh());
  navigation_simulator()->Start();

  CreateFencedFrameWithTestNavigation(GURL("https://www.a.com/allowed.html"),
                                      main_rfh());
  EXPECT_EQ(content::NavigationThrottle::PROCEED,
            SimulateStartAndGetResult(navigation_simulator()));
  EXPECT_EQ(content::NavigationThrottle::PROCEED,
            SimulateCommitAndGetResult(navigation_simulator()));
  content::RenderFrameHost* fenced_frame =
      navigation_simulator()->GetFinalRenderFrameHost();
  ::testing::internal::CaptureStderr();
  ext->ReportSubresourceMap(fenced_frame, GURL("https://www.a.com/allowed.html"));
  navigation_simulator()->StopLoading();
  std::string log_output = ::testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("map.size():0"), std::string::npos);
}

TEST_P(ArkWebContentSubresourceFilterThrottleManagerExtTest, AdBlockMaybeCreateActivationState001) {
  std::unique_ptr<ActivationStateComputingNavigationThrottle> ptr =
    ActivationStateComputingNavigationThrottle::CreateForRoot(&handle, kSafeBrowsingRulesetConfig.uma_tag);
  ::testing::internal::CaptureStderr();
  ext->AdBlockMaybeCreateActivationState(nullptr, ptr);
  std::string log_output = ::testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("[Adblock]"), std::string::npos);
}

TEST_P(ArkWebContentSubresourceFilterThrottleManagerExtTest, AdBlockMaybeCreateActivationState002) {
  std::unique_ptr<ActivationStateComputingNavigationThrottle> ptr =
    ActivationStateComputingNavigationThrottle::CreateForRoot(&handle, kSafeBrowsingRulesetConfig.uma_tag);

  EXPECT_CALL(handle, IsDownload()).WillRepeatedly(::testing::Return(true));
  ::testing::internal::CaptureStderr();
  ext->AdBlockMaybeCreateActivationState(&handle, ptr);
  std::string log_output = ::testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("[Adblock]"), std::string::npos);
}

TEST_P(ArkWebContentSubresourceFilterThrottleManagerExtTest, AdBlockMaybeCreateActivationState003) {
  std::unique_ptr<ActivationStateComputingNavigationThrottle> ptr =
    ActivationStateComputingNavigationThrottle::CreateForRoot(&handle, kSafeBrowsingRulesetConfig.uma_tag);

  EXPECT_CALL(handle, IsDownload()).WillRepeatedly(::testing::Return(false));
  EXPECT_CALL(handle, IsInMainFrame()).WillRepeatedly(::testing::Return(false));
  ::testing::internal::CaptureStderr();
  ext->AdBlockMaybeCreateActivationState(&handle, ptr);
  std::string log_output = ::testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("[Adblock]"), std::string::npos);
}

TEST_P(ArkWebContentSubresourceFilterThrottleManagerExtTest, AdBlockMaybeCreateActivationState004) {
  std::unique_ptr<ActivationStateComputingNavigationThrottle> ptr =
    ActivationStateComputingNavigationThrottle::CreateForRoot(&handle, kSafeBrowsingRulesetConfig.uma_tag);

  EXPECT_CALL(handle, IsDownload()).WillRepeatedly(::testing::Return(false));
  EXPECT_CALL(handle, IsInMainFrame()).WillRepeatedly(::testing::Return(true));
  handle.set_url(GURL("https://www.example.com/not_disallowed.html"));
  EXPECT_CALL(handle, GetWebContents()).WillRepeatedly(::testing::Return(nullptr));
  ::testing::internal::CaptureStderr();
  ext->AdBlockMaybeCreateActivationState(&handle, ptr);
  std::string log_output = ::testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("[Adblock]"), std::string::npos);
}

TEST_P(ArkWebContentSubresourceFilterThrottleManagerExtTest, AdBlockMaybeCreateActivationState005) {
  std::unique_ptr<ActivationStateComputingNavigationThrottle> ptr =
    ActivationStateComputingNavigationThrottle::CreateForRoot(&handle, kSafeBrowsingRulesetConfig.uma_tag);

  EXPECT_CALL(handle, IsDownload()).WillRepeatedly(::testing::Return(false));
  EXPECT_CALL(handle, IsInMainFrame()).WillRepeatedly(::testing::Return(true));
  handle.set_url(GURL("html"));
  EXPECT_CALL(handle, GetWebContents()).WillRepeatedly(::testing::Return(nullptr));
  ::testing::internal::CaptureStderr();
  ext->AdBlockMaybeCreateActivationState(&handle, ptr);
  std::string log_output = ::testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("[Adblock]"), std::string::npos);
}
}