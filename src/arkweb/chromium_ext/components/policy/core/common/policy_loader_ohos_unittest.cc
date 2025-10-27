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

#include "components/policy/core/common/policy_loader_ohos.h"

#include <gmock/gmock.h>

#include <iostream>
#include <string>
#include <vector>

#include "base/base_paths_ohos.h"
#include "base/command_line.h"
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/json/json_reader.h"
#include "base/json/json_string_value_serializer.h"
#include "base/logging.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "base/path_service.h"
#include "base/values.h"
#include "components/policy/core/common/policy_bundle.h"
#include "components/policy/core/common/policy_loader_ohos.h"
#include "components/policy/core/common/policy_pref_names.h"
#include "components/policy/core/common/policy_types.h"
#include "components/prefs/pref_registry_simple.h"
#include "gtest/gtest.h"

namespace policy {
constexpr char kOhosAppApiVersion[] = "user-api-version";
constexpr int kApiMinApiVersion = 16;

using base::CreateDirectory;
using base::FilePath;
using base::PathService;
using base::ScopedTempDir;
using base::WriteFile;

class PolicyLoaderOhosTest : public ::testing::Test {
 protected:
  void SetUp() {
    policy_loader_ = std::make_shared<policy::PolicyLoaderOhos>(nullptr);
    base::CommandLine::Init(0, nullptr);
    ASSERT_TRUE(temp_dir_.CreateUniqueTempDir());
    original_cache_dir_ = ::base::PathService::CheckedGet(::base::DIR_CACHE);
    ::base::PathService::Override(::base::DIR_CACHE, temp_dir_.GetPath());
    bundle_ = std::make_unique<PolicyBundle>();
  }

  void TearDown() {
    policy_loader_.reset();
    ::base::PathService::Override(::base::DIR_CACHE, original_cache_dir_);
    ASSERT_TRUE(temp_dir_.Delete());
  }

  ::base::FilePath original_cache_dir_;
  ::base::ScopedTempDir temp_dir_;
  std::shared_ptr<policy::PolicyLoaderOhos> policy_loader_ = nullptr;
  std::unique_ptr<PolicyBundle> bundle_;
  void SetUseBrowserPolicy(bool tmp) {
    if (policy_loader_) {
      policy_loader_->use_browser_policy_ = tmp;
    }
  }
  bool GetUseBrowserPolicy() {
    if (policy_loader_) {
      return policy_loader_->use_browser_policy_;
    }
  }
  void SetPolicySourceChoosed(bool tmp) {
    if (policy_loader_) {
      policy_loader_->policy_source_choosed_ = tmp;
    }
  }
  bool GetPolicySourceChoose() {
    if (policy_loader_) {
      return policy_loader_->policy_source_choosed_;
    }
  }
  std::string GetString() {
    if (policy_loader_) {
      return policy_loader_->ReadTestPolices();
    }
  }
  void SetMockApiVersion(const std::string& version) {
    auto* cmd_line = base::CommandLine::ForCurrentProcess();
    if (cmd_line) {
      cmd_line->RemoveSwitch(kOhosAppApiVersion);
      cmd_line->AppendSwitchASCII(kOhosAppApiVersion, version);
    }
  }

  void CreateTestFile(const std::string& content) {
    ::base::FilePath file_path =
        temp_dir_.GetPath().Append("test_polices.json");
    ASSERT_TRUE(base::WriteFile(file_path, content));
  }

  void CreateInvalidFile() {
    ::base::FilePath dir_path = temp_dir_.GetPath().Append("test_polices.json");
    ASSERT_TRUE(::base::CreateDirectory(dir_path));
  }

  void CreateEmptyFile() {
    ::base::FilePath file_path =
        temp_dir_.GetPath().Append("test_polices.json");
    base::File file(file_path,
                    ::base::File::FLAG_CREATE | ::base::File::FLAG_WRITE);
    ASSERT_TRUE(file.IsValid());
  }

  bool ParsePolicy(const std::string& json) {
    return PolicyLoaderOhos::ParsePolicy(json, bundle_.get());
  }

  bool PolicyExists(const std::string& name) {
    if (bundle_) {
      const PolicyMap& domain_policies =
          bundle_->Get(PolicyNamespace(POLICY_DOMAIN_CHROME, std::string()));
      return domain_policies.GetValue(name, ::base::Value::Type::NONE) !=
             nullptr;
    }
    return false;
  }

  template <typename T>
  T GetPolicyValue(const std::string& name) {
    const PolicyMap& domain_policies =
        bundle_->Get(PolicyNamespace(POLICY_DOMAIN_CHROME, std::string()));
    const base::Value* value =
        domain_policies.GetValue(name, base::Value::Type::NONE);
    if (!value) {
      return T();
    }
    if constexpr (std::is_same_v<T, bool>) {
      return value->GetBool();
    } else if constexpr (std::is_same_v<T, int>) {
      return value->GetInt();
    } else if constexpr (std::is_same_v<T, double>) {
      return value->GetDouble();
    } else if constexpr (std::is_same_v<T, std::string>) {
      return value->GetString();
    } else if constexpr (std::is_same_v<T, base::Value::List>) {
      return value->GetList().Clone();
    }
    return T();
  }

  void ParseExtensionsPolicy(const base::Value& policies) {
    PolicyLoaderOhos::ParseExtensionsPolicy(&policies, bundle_.get());
  }

  bool HasExtensionPolicy(const std::string& extension_id) {
    if (bundle_) {
      return bundle_
                 ->Get(PolicyNamespace(POLICY_DOMAIN_EXTENSIONS, extension_id))
                 .size() > 0;
    }
  }
};

PolicyBundle BrowserPolicyHandler::GetPolicyBundle() {
  return PolicyBundle();
}

void BrowserPolicyHandler::AddObserver(Observer* loader) {}

void BrowserPolicyHandler::RemoveObserver(Observer* loader) {}

BrowserPolicyHandler* BrowserPolicyHandler::GetInstance() {
  static BrowserPolicyHandler instance;
  return &instance;
}

class MockPolicyLoaderOhos : public PolicyLoaderOhos {
 public:
  MockPolicyLoaderOhos() : PolicyLoaderOhos(nullptr) {}
  MOCK_METHOD1(Reload, void(bool));
};

class PolicyChangedEventCallbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_loader_ = new MockPolicyLoaderOhos();
    callback_ = std::make_unique<PolicyChangedEventCallback>(mock_loader_);
  }
  void TearDown() override {
    delete mock_loader_;
    mock_loader_ = nullptr;
  }
  MockPolicyLoaderOhos* mock_loader_;
  std::unique_ptr<PolicyChangedEventCallback> callback_;
  void TmpOnPolicyChangedImpl() {
    if (callback_) {
      return callback_->OnPolicyChangedImpl();
    }
  }
  void CallOnPolicyChangedImpl(PolicyChangedEventCallback& callback) {
    callback.OnPolicyChangedImpl();
  }
  void SetLoader() {
    if (callback_) {
      callback_->loader_ = nullptr;
    }
  }
  bool IsLoader() {
    if (callback_) {
      if (callback_->loader_) {
        return true;
      }
    }
    return false;
  }
};

TEST_F(PolicyLoaderOhosTest, TryChoosePolicySource_001) {
  ASSERT_NE(policy_loader_, nullptr);
  SetPolicySourceChoosed(true);
  bool initial_state = GetUseBrowserPolicy();
  policy_loader_->TryChoosePolicySource();
  EXPECT_TRUE(GetPolicySourceChoose());
  EXPECT_EQ(initial_state, GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, TryChoosePolicySource_002) {
  ASSERT_NE(policy_loader_, nullptr);
  SetPolicySourceChoosed(false);
  policy_loader_->TryChoosePolicySource();
  EXPECT_TRUE(GetPolicySourceChoose());
  EXPECT_FALSE(GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, TryChoosePolicySource_003) {
  ASSERT_NE(policy_loader_, nullptr);
  SetPolicySourceChoosed(false);
  SetMockApiVersion("20");
  policy_loader_->TryChoosePolicySource();
  EXPECT_TRUE(GetPolicySourceChoose());
  EXPECT_TRUE(GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, TryChoosePolicySource_004) {
  ASSERT_NE(policy_loader_, nullptr);
  SetPolicySourceChoosed(false);
  SetMockApiVersion("15");
  policy_loader_->TryChoosePolicySource();
  EXPECT_TRUE(GetPolicySourceChoose());
  EXPECT_FALSE(GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, TryChoosePolicySource_005) {
  ASSERT_NE(policy_loader_, nullptr);
  SetPolicySourceChoosed(false);
  SetMockApiVersion("-1");
  policy_loader_->TryChoosePolicySource();
  EXPECT_TRUE(GetPolicySourceChoose());
  EXPECT_FALSE(GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, InitOnBackgroundThread_001) {
  ASSERT_NE(policy_loader_, nullptr);
  SetMockApiVersion("20");
  policy_loader_->InitOnBackgroundThread();
  EXPECT_TRUE(GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, InitOnBackgroundThread_002) {
  ASSERT_NE(policy_loader_, nullptr);
  SetMockApiVersion("15");
  policy_loader_->InitOnBackgroundThread();
  EXPECT_FALSE(GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, Load_001) {
  ASSERT_NE(policy_loader_, nullptr);
  SetUseBrowserPolicy(true);
  SetMockApiVersion("20");
  policy_loader_->Load();
  EXPECT_TRUE(GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, Load_002) {
  ASSERT_NE(policy_loader_, nullptr);
  SetMockApiVersion("15");
  policy_loader_->Load();
  EXPECT_FALSE(GetUseBrowserPolicy());
}

TEST_F(PolicyLoaderOhosTest, ReadTestPolices_001) {
  std::string result = GetString();
  EXPECT_TRUE(result.empty());
}

TEST_F(PolicyLoaderOhosTest, ReadTestPolices_002) {
  CreateInvalidFile();

  std::string result = GetString();
  EXPECT_TRUE(result.empty());
}

TEST_F(PolicyLoaderOhosTest, ReadTestPolices_003) {
  CreateEmptyFile();

  std::string result = GetString();
  EXPECT_TRUE(result.empty());
}

TEST_F(PolicyLoaderOhosTest, ReadTestPolices_004) {
  const std::string json_content = R"({
    "policy1": "value1",
    "policy2": {
      "sub_policy": 42
    }
  })";
  CreateTestFile(json_content);

  std::string result = GetString();
  EXPECT_EQ(json_content, result);
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_001) {
  const std::string json = R"({
    "TestPolicy": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": true
    }
  })";

  EXPECT_FALSE(PolicyLoaderOhos::ParsePolicy(json, nullptr));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_002) {
  const std::string json = "{invalid json}";
  EXPECT_FALSE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_003) {
  const std::string json = R"({
    "3rdparty": {
      "extension_id": {
        "policy_name": {
          "value": "policy_value"
        }
      }
    },
    "TestPolicy": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": true
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_004) {
  const std::string json = R"({
    "TestPolicy": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": true
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_005) {
  const std::string json = R"({
    "TestPolicy": "this is not a dict"
  })";

  EXPECT_TRUE(ParsePolicy(json));
  EXPECT_FALSE(PolicyExists("TestPolicy"));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_006) {
  const std::string json = R"({
    "TestPolicy": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform"
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
  EXPECT_FALSE(PolicyExists("TestPolicy"));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_007) {
  const std::string json = R"({
    "BoolPolicy": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": true
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_008) {
  const std::string json = R"({
    "InsecurePrivateNetworkRequestsAllowed": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": 42
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_009) {
  const std::string json = R"({
    "InsecurePrivateNetworkRequestsAllowed": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": 3.14
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_010) {
  const std::string json = R"({
    "InsecurePrivateNetworkRequestsAllowed": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": "hello world"
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_011) {
  const std::string json = R"({
    "InsecurePrivateNetworkRequestsAllowed": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": ["item1", "item2", "item3"]
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParsePolicy_012) {
  const std::string json = R"({
    "InsecurePrivateNetworkRequestsAllowed": {
      "level": "mandatory",
      "scope": "machine",
      "source": "platform",
      "value": {
        "nested": "value"
      }
    }
  })";

  EXPECT_TRUE(ParsePolicy(json));
}

TEST_F(PolicyLoaderOhosTest, ParseExtensionsPolicy_001) {
  PolicyLoaderOhos::ParseExtensionsPolicy(nullptr, bundle_.get());
  ::base::Value policies = ::base::Value(::base::Value::Type::DICT);
  PolicyLoaderOhos::ParseExtensionsPolicy(&policies, nullptr);
  PolicyLoaderOhos::ParseExtensionsPolicy(nullptr, nullptr);
  EXPECT_NO_FATAL_FAILURE(
      PolicyLoaderOhos::ParseExtensionsPolicy(nullptr, nullptr));
}

TEST_F(PolicyLoaderOhosTest, ParseExtensionsPolicy_002) {
  ::base::Value policies = ::base::Value(::base::Value::Type::LIST);
  ParseExtensionsPolicy(policies);
  EXPECT_NO_FATAL_FAILURE(ParseExtensionsPolicy(policies));
}

TEST_F(PolicyLoaderOhosTest, ParseExtensionsPolicy_003) {
  ::base::Value policies = ::base::Value(::base::Value::Type::DICT);
  ParseExtensionsPolicy(policies);
  EXPECT_NO_FATAL_FAILURE(ParseExtensionsPolicy(policies));
}

TEST_F(PolicyLoaderOhosTest, ParseExtensionsPolicy_004) {
  ::base::Value::Dict dict;
  dict.Set("invalid_key", "some_value");

  ::base::Value policies = ::base::Value(std::move(dict));
  ParseExtensionsPolicy(policies);
  EXPECT_NO_FATAL_FAILURE(ParseExtensionsPolicy(policies));
}

TEST_F(PolicyLoaderOhosTest, ParseExtensionsPolicy_005) {
  ::base::Value::Dict dict;
  dict.Set("extensions", "not_a_dict");

  ::base::Value policies = ::base::Value(std::move(dict));
  ParseExtensionsPolicy(policies);
  EXPECT_NO_FATAL_FAILURE(ParseExtensionsPolicy(policies));
}

TEST_F(PolicyLoaderOhosTest, ParseExtensionsPolicy_006) {
  ::base::Value::Dict extensions_dict;

  ::base::Value::Dict dict;
  dict.Set("extensions", ::base::Value(std::move(extensions_dict)));

  ::base::Value policies = ::base::Value(std::move(dict));
  ParseExtensionsPolicy(policies);
  EXPECT_NO_FATAL_FAILURE(ParseExtensionsPolicy(policies));
}

TEST_F(PolicyLoaderOhosTest, ParseExtensionsPolicy_007) {
  ::base::Value::Dict items_dict;
  items_dict.Set("extension_id_1", "not_a_dict");

  ::base::Value::Dict extensions_dict;
  extensions_dict.Set("extensions", ::base::Value(std::move(items_dict)));

  ::base::Value policies = ::base::Value(std::move(extensions_dict));
  ParseExtensionsPolicy(policies);
  EXPECT_NO_FATAL_FAILURE(ParseExtensionsPolicy(policies));
}

TEST_F(PolicyLoaderOhosTest, ParseExtensionsPolicy_008) {
  ::base::Value::Dict policy_dict;
  policy_dict.Set("policy1", "value1");

  ::base::Value::Dict items_dict;
  items_dict.Set("extension_id_1", ::base::Value(std::move(policy_dict)));

  ::base::Value::Dict extensions_dict;
  extensions_dict.Set("extensions", ::base::Value(std::move(items_dict)));

  ::base::Value policies = ::base::Value(std::move(extensions_dict));
  ParseExtensionsPolicy(policies);

  EXPECT_TRUE(HasExtensionPolicy("extension_id_1"));
}

TEST_F(PolicyChangedEventCallbackTest, OnPolicyChangedImpl_001) {
  TmpOnPolicyChangedImpl();
  EXPECT_TRUE(IsLoader());
}

TEST_F(PolicyChangedEventCallbackTest, OnPolicyChangedImpl_002) {
  SetLoader();
  TmpOnPolicyChangedImpl();
  EXPECT_FALSE(IsLoader());
}
}  // namespace policy