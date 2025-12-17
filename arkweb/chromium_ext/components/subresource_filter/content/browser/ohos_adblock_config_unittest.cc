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

#include "ohos_adblock_config.h"

#include <gmock/gmock.h>

#include "base/logging.h"
#include "components/prefs/pref_notifier_impl.h"
#include "components/prefs/pref_registry.h"
#include "components/prefs/pref_registry_simple.h"
#include "gtest/gtest.h"
#include "third_party/libaddressinput/chromium/trie.h"

namespace OHOS {
namespace adblock {
class MockPrefNotifierImpl : public PrefNotifierImpl {
 public:
  MockPrefNotifierImpl() = default;
};

class MockPrefStore : public PersistentPrefStore {
 public:
  MOCK_METHOD(bool,
              GetValue,
              (std::string_view key, const base::Value** result),
              (const, override));
  MOCK_METHOD(base::Value::Dict, GetValues, (), (const, override));
  MOCK_METHOD(void, AddObserver, (Observer * observer), (override));
  MOCK_METHOD(void, RemoveObserver, (Observer * observer), (override));
  MOCK_METHOD(bool,
              GetMutableValue,
              (std::string_view key, base::Value** result),
              (override));
  MOCK_METHOD(void,
              ReportValueChanged,
              (std::string_view key, uint32_t flags),
              (override));
  MOCK_METHOD(void,
              SetValue,
              (std::string_view key, base::Value value, uint32_t flags),
              (override));
  MOCK_METHOD(void,
              SetValueSilently,
              (std::string_view key, base::Value value, uint32_t flags),
              (override));
  MOCK_METHOD(void,
              RemoveValue,
              (std::string_view key, uint32_t flags),
              (override));
  MOCK_METHOD(void,
              RemoveValuesByPrefixSilently,
              (std::string_view prefix),
              (override));
  MOCK_METHOD(bool, ReadOnly, (), (const, override));
  MOCK_METHOD(PrefReadError, GetReadError, (), (const, override));
  MOCK_METHOD(void,
              ReadPrefsAsync,
              (ReadErrorDelegate * error_delegate),
              (override));
  MOCK_METHOD(void, SchedulePendingLossyWrites, (), (override));
  MOCK_METHOD(void,
              CommitPendingWrite,
              (base::OnceClosure reply_callback,
               base::OnceClosure synchronous_done_callback),
              (override));
  MOCK_METHOD(void, OnStoreDeletionFromDisk, (), (override));
  MOCK_METHOD(PrefReadError, ReadPrefs, (), (override));
  MOCK_METHOD(bool, HasReadErrorDelegate, (), (const, override));
};

class MockPrefService : public PrefService {
 public:
  MockPrefService()
      : PrefService(
            std::make_unique<MockPrefNotifierImpl>(),
            nullptr,
            base::MakeRefCounted<::testing::NiceMock<MockPrefStore>>(),
            base::MakeRefCounted<::testing::NiceMock<MockPrefStore>>(),
            base::MakeRefCounted<PrefRegistry>(),
            base::BindRepeating([](PersistentPrefStore::PrefReadError) {}),
            false) {}
  ~MockPrefService() override = default;
};

class AdBlockConfigTest : public ::testing::Test {
 protected:
  void SetUp() {
    ad_block_config_ = AdBlockConfig::GetInstance();
    pref_service_ = std::make_unique<MockPrefService>();
  }
  void SetIsValid(bool value) {
    if (ad_block_config_) {
      ad_block_config_->is_valid_ = value;
    }
  }
  bool GetIsValid() {
    if (ad_block_config_) {
      return ad_block_config_->is_valid_;
    }
    return false;
  }
  bool GetReplace() {
    if (ad_block_config_) {
      return ad_block_config_->replace_;
    }
    return false;
  }
  std::string GetRulesFiles() {
    if (ad_block_config_) {
      return ad_block_config_->rules_files_;
    }
    return std::string();
  }
  autofill::Trie<std::string>* GetDisallowData() {
    if (ad_block_config_) {
      return ad_block_config_->disallow_data_.get();
    }
    return nullptr;
  }
  autofill::Trie<std::string>* GetAllowData() {
    if (ad_block_config_) {
      return ad_block_config_->allow_data_.get();
    }
    return nullptr;
  }
  void TestUpdateDisallowDataOnIOThread(
      std::unique_ptr<autofill::Trie<std::string>> disallow_data) {
    if (ad_block_config_) {
      ad_block_config_->UpdateDisallowDataOnIOThread(std::move(disallow_data));
    }
  }
  bool TestCheckIsInDisallowData(GURL url) {
    if (ad_block_config_) {
      return ad_block_config_->CheckIsInDisallowData(url);
    }
    return false;
  }
  void TestUpdateAllowDataOnIOThread(
      std::unique_ptr<autofill::Trie<std::string>> allow_data) {
    if (ad_block_config_) {
      ad_block_config_->UpdateAllowDataOnIOThread(std::move(allow_data));
    }
  }
  bool TestCheckIsInAllowData(GURL url) {
    if (ad_block_config_) {
      return ad_block_config_->CheckIsInAllowData(url);
    }
    return false;
  }
  std::unique_ptr<autofill::Trie<std::string>>
  TestAddAdsBlockDisallowListInternal(
      const std::vector<std::string>& domain_suffixes) {
    if (ad_block_config_) {
      return ad_block_config_->AddAdsBlockDisallowListInternal(domain_suffixes);
    }
    return nullptr;
  }
  std::unique_ptr<autofill::Trie<std::string>>
  TestRemoveAdsBlockDisallowedListInternal(
      const std::vector<std::string>& domain_suffixes) {
    if (ad_block_config_) {
      return ad_block_config_->RemoveAdsBlockDisallowedListInternal(
          domain_suffixes);
    }
    return nullptr;
  }
  std::unique_ptr<autofill::Trie<std::string>> TestAddAdsBlockAllowListInternal(
      const std::vector<std::string>& domain_suffixes) {
    if (ad_block_config_) {
      return ad_block_config_->AddAdsBlockAllowListInternal(domain_suffixes);
    }
    return nullptr;
  }
  std::unique_ptr<autofill::Trie<std::string>>
  TestRemoveAdsBlockAllowedListInternal(
      const std::vector<std::string>& domain_suffixes) {
    if (ad_block_config_) {
      return ad_block_config_->RemoveAdsBlockAllowedListInternal(
          domain_suffixes);
    }
    return nullptr;
  }
  void TearDown() {
    ad_block_config_ = nullptr;
    pref_service_ = nullptr;
  }

  AdBlockConfig* ad_block_config_ = nullptr;
  std::unique_ptr<MockPrefService> pref_service_;
};

TEST_F(AdBlockConfigTest, GetInstance) {
  auto first_instance = AdBlockConfig::GetInstance();
  EXPECT_NE(first_instance, nullptr);
  auto second_instance = AdBlockConfig::GetInstance();
  EXPECT_EQ(first_instance, second_instance);
}

TEST_F(AdBlockConfigTest, SetAdsBlockRules_001) {
  ASSERT_NE(ad_block_config_, nullptr);
  ad_block_config_->SetPrefService(nullptr);
  const std::string test_rules = "test_rules.json";
  const bool test_replace = true;
  ad_block_config_->SetAdsBlockRules(test_rules, test_replace);
  EXPECT_EQ(GetRulesFiles(), test_rules);
  EXPECT_EQ(GetReplace(), test_replace);
}

TEST_F(AdBlockConfigTest, SetAdsBlockRules_002) {
  ASSERT_NE(ad_block_config_, nullptr);
  ad_block_config_->SetPrefService(pref_service_.get());
  const std::string test_rules = "test_rules.json";
  const bool test_replace = true;
  ad_block_config_->SetAdsBlockRules(test_rules, test_replace);
  EXPECT_EQ(GetRulesFiles(), test_rules);
  EXPECT_EQ(GetReplace(), test_replace);
  EXPECT_EQ(GetIsValid(), false);
}

TEST_F(AdBlockConfigTest, ReadFromPrefService_001) {
  ASSERT_NE(ad_block_config_, nullptr);
  SetIsValid(true);
  ad_block_config_->ReadFromPrefService();
}

TEST_F(AdBlockConfigTest, ReadFromPrefService_002) {
  ASSERT_NE(ad_block_config_, nullptr);
  SetIsValid(false);
  ad_block_config_->SetPrefService(nullptr);
  ad_block_config_->ReadFromPrefService();
}

TEST_F(AdBlockConfigTest, UpdateDisallowDataOnIOThread_001) {
  std::unique_ptr<autofill::Trie<std::string>> null_data = nullptr;
  TestUpdateDisallowDataOnIOThread(std::move(null_data));
}

TEST_F(AdBlockConfigTest, UpdateDisallowDataOnIOThread_002) {
  auto test_data = std::make_unique<autofill::Trie<std::string>>();
  ASSERT_NE(test_data, nullptr);
  std::vector<uint8_t> key = {'a'};
  std::string value = "example.com";
  test_data->AddDataForKey(key, value);
  TestUpdateDisallowDataOnIOThread(std::move(test_data));
  std::set<std::string> result;
  GetDisallowData()->FindDataForKeyPrefix(key, &result);
  for (auto item : result) {
    LOG(DEBUG) << "[Adblock] result item:" << item;
    EXPECT_EQ(item, value);
  }
}

TEST_F(AdBlockConfigTest, CheckIsInDisallowData_001) {
  GURL empty_url("");
  bool result = TestCheckIsInDisallowData(empty_url);
  EXPECT_FALSE(result);
}

TEST_F(AdBlockConfigTest, CheckIsInDisallowData_002) {
  GURL test_url("http://example.com");
  std::unique_ptr<autofill::Trie<std::string>> null_data = nullptr;
  TestUpdateDisallowDataOnIOThread(std::move(null_data));
  bool result = TestCheckIsInDisallowData(test_url);
  EXPECT_FALSE(result);
}

TEST_F(AdBlockConfigTest, CheckIsInDisallowData_003) {
  GURL test_url("http://example.com");
  auto test_data = std::make_unique<autofill::Trie<std::string>>();
  ASSERT_NE(test_data, nullptr);
  std::string key_str = "example.com";
  std::vector<uint8_t> key(key_str.begin(), key_str.end());
  std::reverse(std::begin(key), std::end(key));
  std::string value = "example.com";
  test_data->AddDataForKey(key, value);
  TestUpdateDisallowDataOnIOThread(std::move(test_data));
  bool result = TestCheckIsInDisallowData(test_url);
  EXPECT_TRUE(result);
}

TEST_F(AdBlockConfigTest, AddAdsBlockDisallowListInternal_001) {
  std::vector<std::string> empty_domains;
  auto result = TestAddAdsBlockDisallowListInternal(empty_domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, AddAdsBlockDisallowListInternal_002) {
  std::vector<std::string> domains = {"example.com"};
  auto result = TestAddAdsBlockDisallowListInternal(domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, AddAdsBlockDisallowListInternal_003) {
  std::vector<std::string> domains = {"ads.example.com", "tracking.com",
                                      "doubleclick.net"};
  auto result = TestAddAdsBlockDisallowListInternal(domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, RemoveAdsBlockDisallowedListInternal_001) {
  std::vector<std::string> empty_domains;
  auto result = TestRemoveAdsBlockDisallowedListInternal(empty_domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, RemoveAdsBlockDisallowedListInternal_002) {
  std::vector<std::string> domains = {"example.com"};
  auto result = TestRemoveAdsBlockDisallowedListInternal(domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, RemoveAdsBlockDisallowedListInternal_003) {
  std::vector<std::string> domains = {"ads.example.com", "tracking.com",
                                      "doubleclick.net"};
  auto result = TestRemoveAdsBlockDisallowedListInternal(domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, UpdateAllowDataOnIOThread_001) {
  std::unique_ptr<autofill::Trie<std::string>> null_data = nullptr;
  TestUpdateAllowDataOnIOThread(std::move(null_data));
}

TEST_F(AdBlockConfigTest, UpdateAllowDataOnIOThread_002) {
  auto test_data = std::make_unique<autofill::Trie<std::string>>();
  ASSERT_NE(test_data, nullptr);
  std::vector<uint8_t> key = {'a'};
  std::string value = "example.com";
  test_data->AddDataForKey(key, value);
  TestUpdateAllowDataOnIOThread(std::move(test_data));
  std::set<std::string> result;
  GetAllowData()->FindDataForKeyPrefix(key, &result);
  for (auto item : result) {
    LOG(DEBUG) << "[Adblock] result item:" << item;
    EXPECT_EQ(item, value);
  }
}

TEST_F(AdBlockConfigTest, CheckIsInAllowData_001) {
  GURL empty_url("");
  bool result = TestCheckIsInAllowData(empty_url);
  EXPECT_FALSE(result);
}

TEST_F(AdBlockConfigTest, CheckIsInAllowData_002) {
  GURL test_url("http://example.com");
  std::unique_ptr<autofill::Trie<std::string>> null_data = nullptr;
  TestUpdateAllowDataOnIOThread(std::move(null_data));
  bool result = TestCheckIsInAllowData(test_url);
  EXPECT_FALSE(result);
}

TEST_F(AdBlockConfigTest, CheckIsInAllowData_003) {
  GURL test_url("http://example.com");
  auto test_data = std::make_unique<autofill::Trie<std::string>>();
  ASSERT_NE(test_data, nullptr);
  std::string key_str = "example.com";
  std::vector<uint8_t> key(key_str.begin(), key_str.end());
  std::reverse(std::begin(key), std::end(key));
  std::string value = "example.com";
  test_data->AddDataForKey(key, value);
  TestUpdateAllowDataOnIOThread(std::move(test_data));
  bool result = TestCheckIsInAllowData(test_url);
  EXPECT_TRUE(result);
}

TEST_F(AdBlockConfigTest, CheckIsInAllowData_004) {
  GURL test_url("http://nonexistent.org");
  auto test_data = std::make_unique<autofill::Trie<std::string>>();
  ASSERT_NE(test_data, nullptr);
  std::string key_str = "nonexistent.org";
  std::vector<uint8_t> key(key_str.begin(), key_str.end());
  std::reverse(std::begin(key), std::end(key));
  std::string value = "https://example.com";
  test_data->AddDataForKey(key, value);
  TestUpdateAllowDataOnIOThread(std::move(test_data));
  bool result = TestCheckIsInAllowData(test_url);
  EXPECT_FALSE(result);
}

TEST_F(AdBlockConfigTest, AddAdsBlockAllowListInternal_001) {
  std::vector<std::string> empty_domains;
  auto result = TestAddAdsBlockAllowListInternal(empty_domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, AddAdsBlockAllowListInternal_002) {
  std::vector<std::string> domains = {"example.com"};
  auto result = TestAddAdsBlockAllowListInternal(domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, AddAdsBlockAllowListInternal_003) {
  std::vector<std::string> domains = {"ads.example.com", "tracking.com",
                                      "doubleclick.net"};
  auto result = TestAddAdsBlockAllowListInternal(domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, RemoveAdsBlockAllowedListInternal_001) {
  std::vector<std::string> empty_domains;
  auto result = TestRemoveAdsBlockAllowedListInternal(empty_domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, RemoveAdsBlockAllowedListInternal_002) {
  std::vector<std::string> domains = {"example.com"};
  auto result = TestRemoveAdsBlockAllowedListInternal(domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, RemoveAdsBlockAllowedListInternal_003) {
  std::vector<std::string> domains = {"ads.example.com", "tracking.com",
                                      "doubleclick.net"};
  auto result = TestRemoveAdsBlockAllowedListInternal(domains);
  EXPECT_TRUE(result != nullptr);
}

TEST_F(AdBlockConfigTest, IsAdblockEnabledForUrl_001) {
  ASSERT_NE(ad_block_config_, nullptr);
  GURL invalid_url("invalid_url");
  EXPECT_FALSE(ad_block_config_->IsAdblockEnabledForUrl(invalid_url));

  GURL empty_url("");
  EXPECT_FALSE(ad_block_config_->IsAdblockEnabledForUrl(empty_url));
}

TEST_F(AdBlockConfigTest, IsAdblockEnabledForUrl_002) {
  ASSERT_NE(ad_block_config_, nullptr);
  GURL test_url("http://example.com");

  auto test_data = std::make_unique<autofill::Trie<std::string>>();
  ASSERT_NE(test_data, nullptr);
  std::string key_str = "example.com";
  std::vector<uint8_t> key(key_str.begin(), key_str.end());
  std::reverse(std::begin(key), std::end(key));

  std::string value = "http://example.com";
  test_data->AddDataForKey(key, value);
  TestUpdateAllowDataOnIOThread(std::move(test_data));
  auto test_data_d = std::make_unique<autofill::Trie<std::string>>();
  std::string value_d = "http://baidu.com";
  test_data_d->AddDataForKey(key, value_d);
  TestUpdateDisallowDataOnIOThread(std::move(test_data_d));

  EXPECT_TRUE(ad_block_config_->IsAdblockEnabledForUrl(test_url));
}

TEST_F(AdBlockConfigTest, IsAdblockEnabledForUrl_003) {
  ASSERT_NE(ad_block_config_, nullptr);
  GURL test_url("http://example.com");

  auto test_data = std::make_unique<autofill::Trie<std::string>>();
  ASSERT_NE(test_data, nullptr);
  std::string key_str = "example.com";
  std::vector<uint8_t> key(key_str.begin(), key_str.end());
  std::reverse(std::begin(key), std::end(key));
  std::string value = "http://baidu.com";
  test_data->AddDataForKey(key, value);
  TestUpdateAllowDataOnIOThread(std::move(test_data));

  auto test_data_d = std::make_unique<autofill::Trie<std::string>>();
  std::string value_d = "http://example.com";
  test_data_d->AddDataForKey(key, value_d);
  TestUpdateDisallowDataOnIOThread(std::move(test_data_d));

  EXPECT_FALSE(ad_block_config_->IsAdblockEnabledForUrl(test_url));
}
}  // namespace adblock
}  // namespace OHOS
