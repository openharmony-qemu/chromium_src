/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#define private public
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#undef private

#include <memory>

#include "testing/gtest/include/gtest/gtest.h"

namespace OHOS {
namespace adblock {
class AdBlockConfigTest : public testing::Test {
 public:
  void SetUp() override { adBlockConfig = std::make_shared<AdBlockConfig>(); }

  void TearDown() override {}

  std::shared_ptr<AdBlockConfig> adBlockConfig;
};

TEST_F(AdBlockConfigTest, CheckIsInDisallowData001) {
  GURL url("");
  auto result = adBlockConfig->CheckIsInDisallowData(url);
  EXPECT_TRUE(url.spec().empty());
  EXPECT_FALSE(result);
}

TEST_F(AdBlockConfigTest, AddAdsBlockDisallowListInternal001) {
  std::vector<std::string> domain_suffixes = {"example.com", "exampl.org"};
  auto beforeSize = adBlockConfig->disallow_list_.size();
  adBlockConfig->disallow_data_ =
      adBlockConfig->AddAdsBlockDisallowListInternal(domain_suffixes);
  EXPECT_NE(beforeSize, adBlockConfig->disallow_list_.size());
  EXPECT_FALSE(adBlockConfig->disallow_data_->empty());
}

TEST_F(AdBlockConfigTest, AddAdsBlockDisallowListInternal002) {
  std::vector<std::string> domain_suffixes = {};
  auto beforeSize = adBlockConfig->disallow_list_.size();
  adBlockConfig->disallow_data_ =
      adBlockConfig->AddAdsBlockDisallowListInternal(domain_suffixes);
  EXPECT_EQ(beforeSize, adBlockConfig->disallow_list_.size());
  EXPECT_TRUE(adBlockConfig->disallow_data_->empty());
}

TEST_F(AdBlockConfigTest, UpdateDisallowDataOnIOThread001) {
  std::vector<std::string> domain_suffixes = {"example.com", "exampl.org"};
  auto disallow_data_ptr =
      adBlockConfig->AddAdsBlockDisallowListInternal(domain_suffixes);
  adBlockConfig->UpdateDisallowDataOnIOThread(std::move(disallow_data_ptr));
  EXPECT_FALSE(adBlockConfig->disallow_data_->empty());
}

TEST_F(AdBlockConfigTest, CheckIsInAllowData001) {
  GURL url("");
  auto result = adBlockConfig->CheckIsInAllowData(url);
  EXPECT_TRUE(url.spec().empty());
  EXPECT_FALSE(result);
}

TEST_F(AdBlockConfigTest, IsAdblockEnabledForUrl001) {
  GURL url("");
  auto result = adBlockConfig->IsAdblockEnabledForUrl(url);
  EXPECT_TRUE(url.spec().empty());
  EXPECT_FALSE(result);
}

TEST_F(AdBlockConfigTest, IsAdblockEnabledForUrl002) {
  GURL url("http://example.com");
  url.is_valid_ = true;
  auto result = adBlockConfig->IsAdblockEnabledForUrl(url);
  EXPECT_FALSE(url.spec().empty());
  EXPECT_TRUE(result);
}

TEST_F(AdBlockConfigTest, ReadFromPrefService_001) {
  adBlockConfig->SetPrefService(nullptr);
  adBlockConfig->is_valid_ = false;
  adBlockConfig->ReadFromPrefService();
  EXPECT_TRUE(adBlockConfig->rules_files_.empty());
  EXPECT_FALSE(adBlockConfig->GetUserEasylistReplaceSwitch());
  EXPECT_FALSE(adBlockConfig->is_valid_);
}

}  // namespace adblock
}  // namespace OHOS
