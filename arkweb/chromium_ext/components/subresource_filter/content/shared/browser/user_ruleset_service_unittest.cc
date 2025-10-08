// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on ruleset_service.cc originally written by
// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <utility>

#include "base/environment.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/functional/bind.h"
#include "base/memory/ref_counted.h"
#include "base/path_service.h"
#include "base/run_loop.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/test/metrics/histogram_tester.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "components/prefs/testing_pref_service.h"
#include "components/subresource_filter/content/shared/browser/user_unindexed_ruleset_stream_generator.h"
#include "components/subresource_filter/core/browser/subresource_filter_features.h"
#include "components/subresource_filter/core/browser/user_ruleset_version.h"
#include "components/subresource_filter/core/browser/user_subresource_filter_constants.h"
#include "components/subresource_filter/core/common/common_features.h"
#include "components/subresource_filter/core/common/constants.h"
#include "components/subresource_filter/core/common/indexed_ruleset.h"
#include "components/subresource_filter/core/common/ruleset_config.h"
#include "components/subresource_filter/core/common/test_ruleset_creator.h"
#include "content/public/browser/browser_context.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_content_client_initializer.h"
#define private public
#include "components/subresource_filter/content/shared/browser/user_ruleset_publisher.h"
#include "components/subresource_filter/content/shared/browser/user_ruleset_service.h"
#undef private
#include "ui/base/resource/mock_resource_bundle_delegate.h"
#include "ui/base/resource/resource_bundle.h"

namespace subresource_filter {
using ::testing::_;
using ::testing::Return;
using testing::TestRulesetCreator;
using testing::TestRulesetPair;
using IndexAndWriteRulesetResult =
    UserRulesetService::IndexAndWriteRulesetResult;
using MockClosureTarget =
    ::testing::StrictMock<::testing::MockFunction<void()>>;

namespace {
const std::set<std::string_view> kTestSuffixs = {"foo"};
constexpr int kSuffixCount = 1;
constexpr RulesetConfig kTestUserRulesetConfig = {
    .filter_tag = std::string_view("subresource_filter"),
    .top_level_directory = FILE_PATH_LITERAL("User Subresource Filter"),
    .uma_tag = std::string_view("UserSubresourceFilter")};

static int CurrentFormatVersion() {
  return UserIndexedRulesetVersion::CurrentFormatVersion();
}

class MockUserRulesetServiceClient : public UserRulesetServiceClient {
 public:
  void OnDeleteUserRulesetFile() override {}
};

class MockUserRulesetPublisher : public UserRulesetPublisher {
 public:
  explicit MockUserRulesetPublisher(
      UserRulesetService* ruleset_service,
      base::TestSimpleTaskRunner* blocking_task_runner,
      base::TestSimpleTaskRunner* best_effort_task_runner)
      : UserRulesetPublisher(ruleset_service,
                             blocking_task_runner,
                             ruleset_service->config()),
        blocking_task_runner_(blocking_task_runner),
        best_effort_task_runner_(best_effort_task_runner) {}

  MockUserRulesetPublisher(const MockUserRulesetPublisher&) = delete;
  MockUserRulesetPublisher& operator=(const MockUserRulesetPublisher&) = delete;

  class Factory : public UserRulesetPublisher::Factory {
   public:
    Factory(base::TestSimpleTaskRunner* blocking_task_runner,
            base::TestSimpleTaskRunner* best_effort_task_runner)
        : blocking_task_runner_(blocking_task_runner),
          best_effort_task_runner_(best_effort_task_runner) {}

    std::unique_ptr<UserRulesetPublisher> Create(
        UserRulesetService* ruleset_service,
        scoped_refptr<base::SequencedTaskRunner> blocking_task_runner)
        const override {
      // Intentionally ignore the task runner argument.
      return std::make_unique<MockUserRulesetPublisher>(
          ruleset_service, blocking_task_runner_, best_effort_task_runner_);
    }

   private:
    base::TestSimpleTaskRunner* blocking_task_runner_;
    base::TestSimpleTaskRunner* best_effort_task_runner_;
  };

  ~MockUserRulesetPublisher() override = default;

  void UserSendRulesetToRenderProcess(
      base::File* file,
      content::RenderProcessHost* process) override {
    send_rulset_count_++;
  }

  void TryOpenAndSetRulesetFile(
      const base::FilePath& path,
      int expected_checksum,
      base::OnceCallback<void(RulesetFilePtr)> callback) override {
    send_rulset_count_++;
  }

  scoped_refptr<base::SingleThreadTaskRunner> BestEffortTaskRunner() override {
    return scoped_refptr<base::SingleThreadTaskRunner>(
        best_effort_task_runner_);
  }

  void RunBestEffortUntilIdle() {
    best_effort_task_runner_->RunUntilIdle();
    base::RunLoop().RunUntilIdle();
  }
  int GetSendRulesetCount() { return send_rulset_count_; }

 private:
  static RulesetFilePtr OpenRulesetFile(base::FilePath file_path) {
    return RulesetFilePtr(
        new base::File(file_path, base::File::FLAG_OPEN |
                                      base::File::FLAG_READ |
                                      base::File::FLAG_WIN_SHARE_DELETE),
        base::OnTaskRunnerDeleter(
            base::SequencedTaskRunner::GetCurrentDefault()));
  }

  std::vector<RulesetFilePtr> published_rulesets_;
  base::TestSimpleTaskRunner* blocking_task_runner_;
  base::TestSimpleTaskRunner* best_effort_task_runner_;
  int send_rulset_count_{0};
};
}  // namespace

class UserRulesetServiceTest : public ::testing::Test {
 public:
  UserRulesetServiceTest()
      : blocking_task_runner_(
            base::MakeRefCounted<base::TestSimpleTaskRunner>()),
        background_task_runner_(
            base::MakeRefCounted<base::TestSimpleTaskRunner>()),
        best_effort_task_runner_(
            base::MakeRefCounted<base::TestSimpleTaskRunner>()) {}

  UserRulesetServiceTest(const UserRulesetServiceTest&) = delete;
  UserRulesetServiceTest& operator=(const UserRulesetServiceTest&) = delete;
  void OnWriteRulesetCallback(const UserIndexedRulesetVersion& version) {
    write_callback_version_ = version;
  }

 protected:
  void SetUp() override {
    UserIndexedRulesetVersion::RegisterPrefs(pref_service_.registry(),
                                             kTestUserRulesetConfig.filter_tag);
    mock_client_ = std::make_unique<MockUserRulesetServiceClient>();
    EXPECT_NE(mock_client_, nullptr);
    ASSERT_TRUE(scoped_temp_dir_.CreateUniqueTempDir());

    int index = 0;
#if BUILDFLAG(ARKWEB_ADBLOCK)
    // for css ruleset
    for (const auto& suffix : kTestSuffixs) {
      ASSERT_NO_FATAL_FAILURE(ruleset_creator_.CreateCssRulesetWithPathSuffix(
          suffix, &test_rulesets_[index++]));
    }
#endif
    // for url ruleset
    index = 0;
    for (const auto& suffix : kTestSuffixs) {
      ASSERT_NO_FATAL_FAILURE(
          ruleset_creator_.CreateRulesetToDisallowURLsWithPathSuffix(
              suffix, &test_rulesets_[index++]));
    }
  }

  void TearDown() override {
    service_.reset();
    RunAllUntilIdle();
    // delete file
    base::DeletePathRecursively(GetTempDir());
    ::testing::Test::TearDown();
  }

  void CreateUserRulesetService() {
    service_ = std::make_unique<UserRulesetService>(
        kTestUserRulesetConfig, &pref_service_, background_task_runner_,
        BaseIndexedDir(), BaseUnindexedDir(), mock_client_.get(),
        blocking_task_runner_.get(),
        MockUserRulesetPublisher::Factory(blocking_task_runner_.get(),
                                          background_task_runner_.get()));
  }

  base::FilePath CreateAndGetTestLicenseFilePath(const std::string& contents) {
    base::FilePath license_path = GetTempDir().Append("license/license.txt");
    EXPECT_NO_FATAL_FAILURE(base::WriteFile(license_path, contents));
    return license_path;
  }

  void RunAllUntilIdle() {
    best_effort_task_runner_->RunUntilIdle();
    blocking_task_runner_->RunUntilIdle();
    background_task_runner_->RunUntilIdle();
    base::RunLoop().RunUntilIdle();
  }

  void RunBlockingUntilIdle() {
    blocking_task_runner_->RunUntilIdle();
    base::RunLoop().RunUntilIdle();
  }

  base::TestSimpleTaskRunner* GetBackgroudTaskRunner() const {
    return background_task_runner_.get();
  }

  PrefService* prefs() {
    return &pref_service_;
  }
  UserRulesetService* service() {
    return service_.get();
  }

  MockUserRulesetPublisher* GetUserRulesetPublisher() {
    return static_cast<MockUserRulesetPublisher*>(service_->publisher_.get());
  }

  base::FilePath GetTempDir() const {
    return scoped_temp_dir_.GetPath();
  }

  TestRulesetCreator* GetRulesetCreator() {
    return &ruleset_creator_;
  }
  const TestRulesetPair& GetRuleset(int index) const {
    return test_rulesets_[index];
  }
  base::FilePath GetValidunindexRulesetPath(int index) {
#if BUILDFLAG(ARKWEB_ADBLOCK)
    return GetRuleset(index).unindexedCss.path;
#else
    return GetRuleset(index).unindexed.path;
#endif
  }

  base::FilePath GetValidIndexRulesetPath(int index) {
#if BUILDFLAG(ARKWEB_ADBLOCK)
    return GetRuleset(index).indexedCss.path;
#else
    return GetRuleset(index).indexed.path;
#endif
  }

  const std::vector<uint8_t>& GetValidIndexRulesetContent(int index) {
#if BUILDFLAG(ARKWEB_ADBLOCK)
    return GetRuleset(index).indexedCss.contents;
#else
    return GetRuleset(index).indexed.contents;
#endif
  }

  base::FilePath BaseIndexedDir() const {
    return GetTempDir().AppendASCII("Rules").AppendASCII("Indexed");
  }
  base::FilePath BaseUnindexedDir() const {
    return GetTempDir().AppendASCII("Rules").AppendASCII("unindexed");
  }

  UserUnindexedRulesetInfo CreateTestUnindexedRuleset(
      const std::string& content_version = "test_version_1.0") {
    UserUnindexedRulesetInfo info;
    info.content_version = content_version;
#if BUILDFLAG(ARKWEB_ADBLOCK)
    info.ruleset_path = GetRuleset(0).unindexedCss.path;
#else
    info.ruleset_path = GetRuleset(0).unindexed.path;
#endif

    // Create a test license file
    info.license_path = CreateAndGetTestLicenseFilePath("test_license");
    return info;
  }

  UserUnindexedRulesetInfo CreateTestUnindexedUrlRuleset(
      const std::string& content_version = "test_version_1.0") {
    UserUnindexedRulesetInfo info;
    info.content_version = content_version;
    info.ruleset_path = GetRuleset(0).unindexed.path;
    // Create a test license file
    info.license_path = CreateAndGetTestLicenseFilePath("test_license");
    return info;
  }

  MockUserRulesetServiceClient* GetServiceClient() {
    return mock_client_.get();
  }
  base::WeakPtr<UserRulesetServiceTest> GetWeakPtr() {
    return weak_ptr_factory_.GetWeakPtr();
  }
  UserIndexedRulesetVersion GetWriteResult() {
    return write_callback_version_;
  }

  void UserRulesetServiceCreateTest() {
    base::test::ScopedFeatureList scoped_feature;
    scoped_feature.InitAndDisableFeature(kSafeBrowsingSubresourceFilter);
    auto ruleset_service = UserRulesetService::Create(
        kTestUserRulesetConfig, &pref_service_, scoped_temp_dir_.GetPath(),
        mock_client_.get(),
        MockUserRulesetPublisher::Factory(blocking_task_runner_.get(),
                                          background_task_runner_.get()));
    EXPECT_EQ(ruleset_service, nullptr);

    scoped_feature.InitAndEnableFeature(kSafeBrowsingSubresourceFilter);
    ruleset_service = UserRulesetService::Create(
        kTestUserRulesetConfig, &pref_service_, scoped_temp_dir_.GetPath(),
        mock_client_.get(),
        MockUserRulesetPublisher::Factory(blocking_task_runner_.get(),
                                          background_task_runner_.get()));
    EXPECT_NE(ruleset_service, nullptr);
  }

 private:
  content::BrowserTaskEnvironment task_environment_;
  scoped_refptr<base::TestSimpleTaskRunner> blocking_task_runner_;
  scoped_refptr<base::TestSimpleTaskRunner> background_task_runner_;
  scoped_refptr<base::TestSimpleTaskRunner> best_effort_task_runner_;
  TestingPrefServiceSimple pref_service_;
  base::ScopedTempDir scoped_temp_dir_;
  TestRulesetCreator ruleset_creator_;
  TestRulesetPair test_rulesets_[kSuffixCount];

  UserIndexedRulesetVersion write_callback_version_{""};
  std::unique_ptr<MockUserRulesetServiceClient> mock_client_;
  std::unique_ptr<UserRulesetService> service_;
  base::WeakPtrFactory<UserRulesetServiceTest> weak_ptr_factory_{this};
};

static void CheckDefaultUserIndexedRulesetVersion(
    std::string_view filter_tag,
    const UserIndexedRulesetVersion& loaded_version) {
  UserIndexedRulesetVersion expect_version(filter_tag);
  EXPECT_EQ(loaded_version.content_version, expect_version.content_version);
  EXPECT_EQ(loaded_version.format_version, expect_version.format_version);
  EXPECT_EQ(loaded_version.checksum, expect_version.checksum);
  EXPECT_EQ(loaded_version.filter_tag, expect_version.filter_tag);
  EXPECT_FALSE(loaded_version.IsValid());
}

TEST_F(UserRulesetServiceTest, UserRulesetService_Create) {
  UserRulesetServiceCreateTest();
}

TEST_F(UserRulesetServiceTest, UserRulesetService_SaveToPrefs) {
  // invalid version
  CreateUserRulesetService();
  EXPECT_NE(service(), nullptr);
  RunAllUntilIdle();
  // default
  UserIndexedRulesetVersion loaded_version(kTestUserRulesetConfig.filter_tag);
  loaded_version.ReadFromPrefs(prefs());
  CheckDefaultUserIndexedRulesetVersion(kTestUserRulesetConfig.filter_tag,
                                        loaded_version);
  // invalid current version
  UserIndexedRulesetVersion version("content_1.0", CurrentFormatVersion() + 1,
                                    kTestUserRulesetConfig.filter_tag);
  version.SaveToPrefs(prefs());
  // get version from pref
  CreateUserRulesetService();
  EXPECT_NE(service(), nullptr);
  RunAllUntilIdle();
  loaded_version.ReadFromPrefs(prefs());
  // Verify data was correctly saved and loaded
  CheckDefaultUserIndexedRulesetVersion(kTestUserRulesetConfig.filter_tag,
                                        loaded_version);
}

TEST_F(UserRulesetServiceTest, UserRulesetService_Publisher) {
  UserIndexedRulesetVersion version("content_1.0", CurrentFormatVersion(),
                                    kTestUserRulesetConfig.filter_tag);
  version.SaveToPrefs(prefs());
  // get version from pref
  CreateUserRulesetService();
  EXPECT_NE(service(), nullptr);
  RunAllUntilIdle();
  EXPECT_EQ(GetUserRulesetPublisher()->GetSendRulesetCount(), 1);
}

TEST_F(UserRulesetServiceTest, IndexAndStoreAndPublishRulesetIfNeeded_NoNeed) {
  const char test_version[] = "test_version_1.0";
  CreateUserRulesetService();
  EXPECT_NE(service(), nullptr);
  RunAllUntilIdle();

  UserUnindexedRulesetInfo info = CreateTestUnindexedRuleset(test_version);
  // empty
  info.content_version = "";
  EXPECT_NO_FATAL_FAILURE(
      service()->IndexAndStoreAndPublishRulesetIfNeeded(info));
  // equal, return
  UserIndexedRulesetVersion version("content_1.0", CurrentFormatVersion(),
                                    kTestUserRulesetConfig.filter_tag);
  version.SaveToPrefs(prefs());
  info.content_version = "content_1.0";
  EXPECT_NO_FATAL_FAILURE(
      service()->IndexAndStoreAndPublishRulesetIfNeeded(info));

  // not equal content_version
  // is_initialized_
  service()->is_initialized_ = false;
  info.content_version = test_version;
  EXPECT_NO_FATAL_FAILURE(
      service()->IndexAndStoreAndPublishRulesetIfNeeded(info));
  EXPECT_EQ(service()->queued_unindexed_ruleset_info_.content_version,
            info.content_version);
}

TEST_F(UserRulesetServiceTest, IndexAndStoreAndPublishRulesetIfNeeded) {
  const char test_version[] = "test_version_1.0";
  CreateUserRulesetService();
  EXPECT_NE(service(), nullptr);
  RunAllUntilIdle();

  UserUnindexedRulesetInfo info = CreateTestUnindexedRuleset(test_version);
  // none current version
  UserIndexedRulesetVersion version("content_1.0", CurrentFormatVersion() + 1,
                                    kTestUserRulesetConfig.filter_tag);
  version.SaveToPrefs(prefs());
  service()->is_initialized_ = false;
  info.content_version = "content_1.0";
  EXPECT_NO_FATAL_FAILURE(
      service()->IndexAndStoreAndPublishRulesetIfNeeded(info));
  EXPECT_NO_FATAL_FAILURE(
      service()->IndexAndStoreAndPublishRulesetIfNeeded(info));
  EXPECT_EQ(service()->queued_unindexed_ruleset_info_.content_version,
            info.content_version);

  service()->is_initialized_ = true;
  info.content_version = "test-001";
  EXPECT_NO_FATAL_FAILURE(
      service()->IndexAndStoreAndPublishRulesetIfNeeded(info));
  RunAllUntilIdle();
  EXPECT_EQ(GetUserRulesetPublisher()->GetSendRulesetCount(), 1);
}

TEST_F(UserRulesetServiceTest, WriteRuleset) {
  base::FilePath version_dir = GetValidIndexRulesetPath(0);
  std::string license = "test_license";
  base::FilePath license_path = CreateAndGetTestLicenseFilePath(license);

  // save
  IndexAndWriteRulesetResult result = UserRulesetService::WriteRuleset(
      version_dir, license_path,
      base::span<const uint8_t>(GetValidIndexRulesetContent(0)));
  EXPECT_EQ(result, IndexAndWriteRulesetResult::SUCCESS);
}

TEST_F(UserRulesetServiceTest, WriteRuleset_InvalidPath) {
  // invalid indexed ruleset path
  IndexAndWriteRulesetResult result = UserRulesetService::WriteRuleset(base::FilePath(), base::FilePath(),
                                            base::span<const uint8_t>());
  EXPECT_EQ(result, IndexAndWriteRulesetResult::FAILED_REPLACE_FILE);

  // invalid license path
  base::FilePath version_dir = GetValidIndexRulesetPath(0);
  base::FilePath non_existent_license =
      GetTempDir().Append("no_file/no_such_file.txt");
  result = UserRulesetService::WriteRuleset(
      version_dir, non_existent_license,
      base::span<const uint8_t>(GetValidIndexRulesetContent(0)));
  // do not care license path
  EXPECT_EQ(result, IndexAndWriteRulesetResult::SUCCESS);
}

TEST_F(UserRulesetServiceTest, IndexAndWriteRuleset) {
  const char test_version[] = "test_version_1.0";
  base::FilePath indexed_ruleset_base_dir =
      GetTempDir().AppendASCII(base::NumberToString(CurrentFormatVersion()));
  ASSERT_TRUE(base::CreateDirectory(indexed_ruleset_base_dir));
  UserUnindexedRulesetInfo unindexed_ruleset_info =
      CreateTestUnindexedRuleset();
  UserIndexedRulesetVersion result = UserRulesetService::IndexAndWriteRuleset(
      kTestUserRulesetConfig, indexed_ruleset_base_dir, unindexed_ruleset_info);
  EXPECT_EQ(result.filter_tag, kTestUserRulesetConfig.filter_tag);
  EXPECT_TRUE(result.IsValid());
}

TEST_F(UserRulesetServiceTest, IndexAndWriteRuleset_Invalid) {
  const char test_version[] = "test_version_1.0";
  base::FilePath indexed_ruleset_base_dir =
      GetTempDir().AppendASCII(base::NumberToString(CurrentFormatVersion()));

  UserUnindexedRulesetInfo unindexed_ruleset_info;
  unindexed_ruleset_info.content_version = test_version;
  unindexed_ruleset_info.ruleset_path = indexed_ruleset_base_dir;
  // no ruleset
  UserIndexedRulesetVersion result = UserRulesetService::IndexAndWriteRuleset(
      kTestUserRulesetConfig, indexed_ruleset_base_dir, unindexed_ruleset_info);
  EXPECT_EQ(result.filter_tag, kTestUserRulesetConfig.filter_tag);
  EXPECT_FALSE(result.IsValid());

  unindexed_ruleset_info = CreateTestUnindexedRuleset();
  // no indexed_ruleset_base_dir, but will create it
  result = UserRulesetService::IndexAndWriteRuleset(
      kTestUserRulesetConfig, indexed_ruleset_base_dir, unindexed_ruleset_info);
  EXPECT_EQ(result.filter_tag, kTestUserRulesetConfig.filter_tag);
  EXPECT_TRUE(result.IsValid());
}

TEST_F(UserRulesetServiceTest, IndexRuleset) {
#if BUILDFLAG(ARKWEB_ADBLOCK)
  ArkWebRulesetIndexerExt indexer;
#else
  RulesetIndexer indexer;
#endif
  UserUnindexedRulesetInfo ruleset_info = CreateTestUnindexedRuleset();
  UserUnindexedRulesetStreamGenerator generator(ruleset_info);
  bool result = UserRulesetService::IndexRuleset(kTestUserRulesetConfig,
                                                 &generator, &indexer);
  ASSERT_TRUE(result);
}

TEST_F(UserRulesetServiceTest, IndexRulesetForUrl) {
#if BUILDFLAG(ARKWEB_ADBLOCK)
  ArkWebRulesetIndexerExt indexer;
#else
  RulesetIndexer indexer;
#endif
  UserUnindexedRulesetInfo ruleset_info = CreateTestUnindexedUrlRuleset();
  UserUnindexedRulesetStreamGenerator generator(ruleset_info);
  bool result = UserRulesetService::IndexRuleset(kTestUserRulesetConfig,
                                                 &generator, &indexer);
  ASSERT_TRUE(result);
}

TEST_F(UserRulesetServiceTest, IndexRuleset_InvalidRuleset) {
#if BUILDFLAG(ARKWEB_ADBLOCK)
  ArkWebRulesetIndexerExt indexer;
#else
  RulesetIndexer indexer;
#endif
  // ruleset_size
  UserUnindexedRulesetInfo ruleset_info;
  ruleset_info.content_version = "test_version";
  ruleset_info.ruleset_path = GetTempDir().AppendASCII("no_data");
  UserUnindexedRulesetStreamGenerator generator(ruleset_info);
  bool result = UserRulesetService::IndexRuleset(kTestUserRulesetConfig,
                                                 &generator, &indexer);
  ASSERT_FALSE(result);
}

TEST_F(UserRulesetServiceTest, OnWrittenRuleset) {
  UserIndexedRulesetVersion version("");
  version.content_version = "version_1.0";
  CreateUserRulesetService();
  EXPECT_NO_FATAL_FAILURE(service()->OnWrittenRuleset(
      base::BindOnce(&UserRulesetServiceTest::OnWriteRulesetCallback,
                     GetWeakPtr()),
      version));
  RunAllUntilIdle();
  UserIndexedRulesetVersion result = GetWriteResult();
  EXPECT_FALSE(result.IsValid());
  // valid
  version.content_version = "version_1.0";
  version.format_version = CurrentFormatVersion();
  EXPECT_NO_FATAL_FAILURE(service()->OnWrittenRuleset(
      base::BindOnce(&UserRulesetServiceTest::OnWriteRulesetCallback,
                     GetWeakPtr()),
      version));
  RunAllUntilIdle();
  result = GetWriteResult();
  EXPECT_TRUE(result.IsValid());
  EXPECT_EQ(result.content_version, version.content_version);
}

TEST_F(UserRulesetServiceTest,
       UserIndexedRulesetLocator_DeleteObsoleteRulesets) {
  auto client = GetServiceClient();
  base::FilePath indexed_ruleset_base_dir =
      GetTempDir().AppendASCII("indexed_ruleset_data");
  base::FilePath current_format_dir(indexed_ruleset_base_dir.AppendASCII(
      base::NumberToString(UserIndexedRulesetVersion::CurrentFormatVersion())));
  ASSERT_TRUE(
      base::CreateDirectory(indexed_ruleset_base_dir.AppendASCII("data1")));
  ASSERT_TRUE(
      base::CreateDirectory(indexed_ruleset_base_dir.AppendASCII("data2")));
  ASSERT_TRUE(base::CreateDirectory(current_format_dir));
  UserIndexedRulesetVersion most_recent_version(
      "content_1.0", CurrentFormatVersion(), kTestUserRulesetConfig.filter_tag);

  ASSERT_TRUE(base::CreateDirectory(current_format_dir.AppendASCII("data1")));
  ASSERT_TRUE(
      base::CreateDirectory(current_format_dir.AppendASCII("data1").AppendASCII(
          kUserSentinelFileName)));
  EXPECT_TRUE(
      base::PathExists(current_format_dir.AppendASCII("data1").AppendASCII(
          kUserSentinelFileName)));
  ASSERT_TRUE(base::CreateDirectory(current_format_dir.AppendASCII("data2")));
  ASSERT_TRUE(base::CreateDirectory(
      current_format_dir.AppendASCII(most_recent_version.content_version)));
  UserIndexedRulesetLocator::DeleteObsoleteRulesets(
      indexed_ruleset_base_dir, base::FilePath(), client, most_recent_version);
  EXPECT_TRUE(base::PathExists(current_format_dir.AppendASCII("data1")));
}

TEST_F(UserRulesetServiceTest,
       UserIndexedRulesetLocator_DeleteObsoleteRulesets_InvalidVersion) {
  auto client = GetServiceClient();
  base::FilePath indexed_ruleset_base_dir =
      GetTempDir().AppendASCII("indexed_ruleset_data");
  base::FilePath current_format_dir(indexed_ruleset_base_dir.AppendASCII(
      base::NumberToString(UserIndexedRulesetVersion::CurrentFormatVersion())));
  ASSERT_TRUE(
      base::CreateDirectory(indexed_ruleset_base_dir.AppendASCII("data1")));
  ASSERT_TRUE(
      base::CreateDirectory(indexed_ruleset_base_dir.AppendASCII("data2")));
  ASSERT_TRUE(base::CreateDirectory(current_format_dir));
  UserIndexedRulesetVersion most_recent_version(
      "", CurrentFormatVersion() + 1, kTestUserRulesetConfig.filter_tag);

  ASSERT_TRUE(base::CreateDirectory(current_format_dir.AppendASCII("data1")));
  ASSERT_TRUE(base::CreateDirectory(current_format_dir.AppendASCII("data2")));
  ASSERT_TRUE(base::CreateDirectory(
      current_format_dir.AppendASCII(most_recent_version.content_version)));
  UserIndexedRulesetLocator::DeleteObsoleteRulesets(
      indexed_ruleset_base_dir, base::FilePath(), client, most_recent_version);
  EXPECT_FALSE(base::PathExists(current_format_dir.AppendASCII("data1")));
}

TEST_F(UserRulesetServiceTest,
       UserRulesetPublisher_OnRenderProcessHostCreated) {
  CreateUserRulesetService();
  EXPECT_NE(service(), nullptr);
  EXPECT_NE(service()->publisher_, nullptr);

  auto publisher = GetUserRulesetPublisher();
  // ruleset_data_ is null, do call
  EXPECT_NO_FATAL_FAILURE(publisher->OnRenderProcessHostCreated(nullptr));
  EXPECT_EQ(publisher->GetSendRulesetCount(), 0);

  // invalid ruleset_data_
  RulesetFilePtr file1(
      new base::File(GetTempDir().AppendASCII("indexed_ruleset_data_xxx"),
                     base::File::FLAG_OPEN | base::File::FLAG_READ),
      base::OnTaskRunnerDeleter(GetBackgroudTaskRunner()));
  publisher->ruleset_data_ = std::move(file1);
  EXPECT_NO_FATAL_FAILURE(publisher->OnRenderProcessHostCreated(nullptr));
  EXPECT_EQ(publisher->GetSendRulesetCount(), 0);

  // valid
  const char kTestFileContents[] = "foobar";
  base::FilePath path = GetTempDir().AppendASCII("indexed_ruleset_data");
  base::WriteFile(path, kTestFileContents);
  RulesetFilePtr file(
      new base::File(path, base::File::FLAG_OPEN | base::File::FLAG_READ),
      base::OnTaskRunnerDeleter(GetBackgroudTaskRunner()));
  publisher->ruleset_data_ = std::move(file);
  EXPECT_NO_FATAL_FAILURE(publisher->OnRenderProcessHostCreated(nullptr));
  EXPECT_EQ(publisher->GetSendRulesetCount(), 1);
}

// class UserRulesetPublisher::PublishNewRulesetVersion
TEST_F(UserRulesetServiceTest, OnRulesetSet) {
  const char kTestFileContents[] = "foobar";
  base::FilePath path = GetTempDir().AppendASCII("indexed_ruleset_data");
  base::WriteFile(path, kTestFileContents);

  RulesetFilePtr file(
      new base::File(path, base::File::FLAG_OPEN | base::File::FLAG_READ),
      base::OnTaskRunnerDeleter(GetBackgroudTaskRunner()));
  CreateUserRulesetService();
  EXPECT_NE(service(), nullptr);
  EXPECT_NE(service()->publisher_, nullptr);

  // set callback
  base::test::ScopedFeatureList scoped_feature;
  scoped_feature.InitAndEnableFeature(kAdTagging);
  MockClosureTarget publish_callback_target;
  EXPECT_CALL(publish_callback_target, Call()).Times(1);
  service()->publisher_->SetRulesetPublishedCallbackForTesting(base::BindOnce(
      &MockClosureTarget::Call, base::Unretained(&publish_callback_target)));
  EXPECT_NO_FATAL_FAILURE(service()->OnRulesetSet(std::move(file)));
  ::testing::Mock::VerifyAndClearExpectations(&publish_callback_target);
}
}  // namespace subresource_filter
