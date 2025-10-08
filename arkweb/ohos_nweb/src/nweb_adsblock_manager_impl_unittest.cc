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
#include "ohos_nweb/src/cef_delegate/nweb_adsblock_manager_delegate.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#define private public
#include "ohos_nweb/src/nweb_adsblock_manager_impl.h"
#undef private

using namespace OHOS::NWeb;
using namespace testing;

namespace {
  static bool g_adsblock_method_called = false;
}

class MockNWebAdsBlockManagerDelegate : public NWebAdsBlockManagerDelegate {
public:
  void SetAdsBlockRules(const std::string& rulesFile, bool replace) override {
    g_adsblock_method_called = true;
  }
  
  void AddAdsBlockDisallowedList(const std::vector<std::string>& domainSuffixes) override {
    g_adsblock_method_called = true;
  }
  
  void RemoveAdsBlockDisallowedList(const std::vector<std::string>& domainSuffixes) override {
    g_adsblock_method_called = true;
  }
  
  void ClearAdsBlockDisallowedList() override {
    g_adsblock_method_called = true;
  }
  
  void AddAdsBlockAllowedList(const std::vector<std::string>& domainSuffixes) override {
    g_adsblock_method_called = true;
  }
  
  void RemoveAdsBlockAllowedList(const std::vector<std::string>& domainSuffixes) override {
    g_adsblock_method_called = true;
  }
  
  void ClearAdsBlockAllowedList() override {
    g_adsblock_method_called = true;
  }
};

class NWebAdsBlockManagerImplTest : public testing::Test {};

TEST_F(NWebAdsBlockManagerImplTest, AllBranchesCoverage) {
  auto manager = std::make_shared<NWebAdsBlockManagerImpl>();
  auto mock_delegate = std::make_shared<MockNWebAdsBlockManagerDelegate>();
  manager->delegate_ = mock_delegate;

  g_adsblock_method_called = false;
  manager->SetAdsBlockRules("test.json", true);
  EXPECT_TRUE(g_adsblock_method_called);
  
  std::vector<std::string> test_domains = {"example.com"};
  g_adsblock_method_called = false;
  manager->AddAdsBlockDisallowedList(test_domains);
  EXPECT_TRUE(g_adsblock_method_called);

  g_adsblock_method_called = false;
  manager->RemoveAdsBlockDisallowedList(test_domains);
  EXPECT_TRUE(g_adsblock_method_called);

  g_adsblock_method_called = false;
  manager->ClearAdsBlockDisallowedList();
  EXPECT_TRUE(g_adsblock_method_called);

  g_adsblock_method_called = false;
  manager->AddAdsBlockAllowedList(test_domains);
  EXPECT_TRUE(g_adsblock_method_called);

  g_adsblock_method_called = false;
  manager->RemoveAdsBlockAllowedList(test_domains);
  EXPECT_TRUE(g_adsblock_method_called);

  g_adsblock_method_called = false;
  manager->ClearAdsBlockAllowedList();
  EXPECT_TRUE(g_adsblock_method_called);

  manager->delegate_ = nullptr;

  g_adsblock_method_called = false;
  manager->SetAdsBlockRules("test.json", true);
  EXPECT_FALSE(g_adsblock_method_called);

  manager->AddAdsBlockDisallowedList(test_domains);
  EXPECT_FALSE(g_adsblock_method_called);

  manager->RemoveAdsBlockDisallowedList(test_domains);
  EXPECT_FALSE(g_adsblock_method_called);

  manager->ClearAdsBlockDisallowedList();
  EXPECT_FALSE(g_adsblock_method_called);

  manager->AddAdsBlockAllowedList(test_domains);
  EXPECT_FALSE(g_adsblock_method_called);

  manager->RemoveAdsBlockAllowedList(test_domains);
  EXPECT_FALSE(g_adsblock_method_called);

  manager->ClearAdsBlockAllowedList();
  EXPECT_FALSE(g_adsblock_method_called);
}