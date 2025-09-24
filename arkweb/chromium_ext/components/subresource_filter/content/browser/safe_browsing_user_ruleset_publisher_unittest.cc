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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/files/file.h"
#include "base/files/scoped_temp_dir.h"
#include "base/memory/ref_counted.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "components/prefs/testing_pref_service.h"
#include "components/subresource_filter/content/shared/browser/user_ruleset_service.h"
#include "components/subresource_filter/core/browser/subresource_filter_features.h"
#include "components/subresource_filter/core/mojom/subresource_filter.mojom.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/test/mock_render_process_host.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "ipc/ipc_channel.h"
#include "ipc/ipc_channel_proxy.h"
#define protected public
#include "components/subresource_filter/content/browser/safe_browsing_user_ruleset_publisher.h"
#undef protected

namespace content {
class TestRenderProcessHost : public MockRenderProcessHost {
 public:
  explicit TestRenderProcessHost(BrowserContext* browser_context,
                                 bool is_for_guests_only = false)
      : MockRenderProcessHost(browser_context, is_for_guests_only) {}

  TestRenderProcessHost(const TestRenderProcessHost&) = delete;
  TestRenderProcessHost& operator=(const TestRenderProcessHost&) = delete;

  ~TestRenderProcessHost() override {}

  MOCK_METHOD(IPC::ChannelProxy*, GetChannel, (), (override));
};
}  // namespace content

namespace subresource_filter {
namespace {
constexpr RulesetConfig kTestUserRulesetConfig = {
    .filter_tag = std::string_view("subresource_filter"),
    .top_level_directory = FILE_PATH_LITERAL("User Subresource Filter"),
    .uma_tag = std::string_view("UserSubresourceFilter")};
}

class MockUserRulesetServiceClient : public UserRulesetServiceClient {
 public:
  void OnDeleteUserRulesetFile() override {}
};

class SafeBrowsingUserRulesetPublisherTest
    : public content::RenderViewHostTestHarness {
 public:
  SafeBrowsingUserRulesetPublisherTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME),
        blocking_task_runner_(
            base::MakeRefCounted<base::TestSimpleTaskRunner>()) {}

  ~SafeBrowsingUserRulesetPublisherTest() {}
  SafeBrowsingUserRulesetPublisherTest(
      const SafeBrowsingUserRulesetPublisherTest&) = delete;
  SafeBrowsingUserRulesetPublisherTest& operator=(
      const SafeBrowsingUserRulesetPublisherTest&) = delete;

 protected:
  SafeBrowsingUserRulesetPublisher* CreateSafeBrowsingUserRulesetPublisher() {
    SafeBrowsingUserRulesetPublisher::Factory factory;
    ruleset_service_ = UserRulesetService::Create(
        kTestUserRulesetConfig, &pref_service_, scoped_temp_dir_.GetPath(),
        mock_client_.get(), factory);
    EXPECT_NE(ruleset_service_, nullptr);
    publisher_ = factory.Create(ruleset_service_.get(), blocking_task_runner_);
    EXPECT_NE(publisher_, nullptr);
    return static_cast<SafeBrowsingUserRulesetPublisher*>(publisher_.get());
  }

  void SetUp() override {
    content::RenderViewHostTestHarness::SetUp();
    UserIndexedRulesetVersion::RegisterPrefs(pref_service_.registry(),
                                             kTestUserRulesetConfig.filter_tag);
    mock_client_ = std::make_unique<MockUserRulesetServiceClient>();
    EXPECT_NE(mock_client_, nullptr);
    ASSERT_TRUE(scoped_temp_dir_.CreateUniqueTempDir());
    base::test::ScopedFeatureList scoped_feature;
    scoped_feature.InitAndEnableFeature(kSafeBrowsingSubresourceFilter);
  }

  void TearDown() override {
    ruleset_service_.reset();
    publisher_.reset();

    // delete file
    base::DeletePathRecursively(scoped_temp_dir_.GetPath());
    content::RenderViewHostTestHarness::TearDown();
  }

 private:
  scoped_refptr<base::TestSimpleTaskRunner> blocking_task_runner_;
  std::unique_ptr<MockUserRulesetServiceClient> mock_client_;
  std::unique_ptr<UserRulesetService> ruleset_service_{};
  std::unique_ptr<UserRulesetPublisher> publisher_{};
  TestingPrefServiceSimple pref_service_;
  base::ScopedTempDir scoped_temp_dir_;
};

TEST_F(SafeBrowsingUserRulesetPublisherTest, FactoryCreateReturnsNonNull) {
  auto publisher = CreateSafeBrowsingUserRulesetPublisher();
  EXPECT_NE(publisher, nullptr);
}

TEST_F(SafeBrowsingUserRulesetPublisherTest,
       UserSendRulesetToRenderProcess_Success) {
  auto publisher = CreateSafeBrowsingUserRulesetPublisher();
  EXPECT_NE(publisher, nullptr);

  base::ScopedTempDir scoped_temp_dir;
  base::FilePath license_path =
      scoped_temp_dir.GetPath().Append("license/license.txt");
  ASSERT_TRUE(base::CreateDirectory(license_path.DirName()));
  EXPECT_NO_FATAL_FAILURE(base::WriteFile(license_path, "test xxxx "));
  base::File file(license_path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  // no channel
  auto host =
      std::make_unique<content::TestRenderProcessHost>(browser_context());
  EXPECT_NE(host, nullptr);
  EXPECT_CALL(*host, GetChannel()).WillOnce(::testing::Return(nullptr));
  EXPECT_NO_FATAL_FAILURE(
      publisher->UserSendRulesetToRenderProcess(&file, host.get()));

  // with channel
  mojo::MessagePipe pipe;
  auto ipc_proxy_channel = IPC::ChannelProxy::Create(
      pipe.handle0.release(), IPC::Channel::MODE_CLIENT, nullptr,
      base::SingleThreadTaskRunner::GetCurrentDefault().get(),
      base::SingleThreadTaskRunner::GetCurrentDefault().get());
  EXPECT_CALL(*host, GetChannel())
      .Times(2).WillRepeatedly(::testing::Return(ipc_proxy_channel.get()));
  EXPECT_NO_FATAL_FAILURE(
      publisher->UserSendRulesetToRenderProcess(&file, host.get()));
  base::DeletePathRecursively(license_path);
}
}  // namespace subresource_filter