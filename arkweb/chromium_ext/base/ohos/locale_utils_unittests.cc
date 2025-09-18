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

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#define private public
#include "arkweb/chromium_ext/base/ohos/locale_utils.h"
#undef private
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace OHOS::NWeb;
namespace OHOS::NWeb {
class MockOhosResourceAdapter : public OhosResourceAdapter {
public:
  MOCK_METHOD(bool, GetRawFileData, (
      const std::string& rawFile, size_t& len, uint8_t** dest, bool isSys), (override));
  MOCK_METHOD(std::shared_ptr<OhosFileMapper>, GetRawFileMapper, (
      const std::string& rawFile, bool isSys), (override));
  MOCK_METHOD(bool, IsRawFileExist, (const std::string& rawFile, bool isSys), (override));
  MOCK_METHOD(bool, GetRawFileLastModTime, (
      const std::string& rawFile, uint16_t& date, uint16_t& time, bool isSys), (override));
  MOCK_METHOD(bool, GetRawFileLastModTime, (
      const std::string& rawFile, time_t& time, bool isSys), (override));
  MOCK_METHOD(std::string, GetSystemLanguage, (), (override));
};
}
namespace base {
namespace ohos {
class LocaleUtilsTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(LocaleUtilsTest, IsTraditionalLanguage001) {
  bool is_traditional_language = IsTraditionalLanguage();
  EXPECT_FALSE(is_traditional_language);
}

TEST_F(LocaleUtilsTest, IsTraditionalLanguage002) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("zh-Hant"));
  EXPECT_TRUE(IsTraditionalLanguage());
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(LocaleUtilsTest, ComputeLanguageByRegionDefault) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("zh-Hans"));
  std::string language = ComputeLanguageByRegion("HK");
  EXPECT_EQ("zh-CN", language);
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(LocaleUtilsTest, ComputeLanguageByRegionHant001) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("zh-Hant"));
  EXPECT_EQ(ComputeLanguageByRegion("HK"), "zh-HK");
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(LocaleUtilsTest, ComputeLanguageByRegionHant002) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("zh-Hant"));
  EXPECT_EQ(ComputeLanguageByRegion("MO"), "zh-HK");
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(LocaleUtilsTest, ComputeLanguageByRegionHant003) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("zh-Hant"));
  EXPECT_EQ(ComputeLanguageByRegion("TW"), "zh-TW");
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(LocaleUtilsTest, ComputeLanguageByRegion004) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("en"));
  EXPECT_EQ(ComputeLanguageByRegion("en-US"), "en-US");
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(LocaleUtilsTest, ComputeLanguageByRegion005) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("zh"));
  EXPECT_EQ(ComputeLanguageByRegion("zh-CN"), "zh-CN");
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(LocaleUtilsTest, ComputeLanguageByRegion006) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("bo"));
  EXPECT_EQ(ComputeLanguageByRegion("bo-CN"), "bo-CN");
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}


TEST_F(LocaleUtilsTest, ComputeLanguageByRegion007) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("ug"));
  EXPECT_EQ(ComputeLanguageByRegion("ug"), "ug");
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(LocaleUtilsTest, ComputeLanguageByRegion008) {
  MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
  OhosAdapterHelper::SetInstance(instance);
  auto adapter = std::make_unique<MockOhosResourceAdapter>();
  auto adapter_ptr = adapter.get();
  EXPECT_CALL(*instance, GetResourceAdapter("")).WillOnce(testing::Return(testing::ByMove(std::move(adapter))));
  EXPECT_CALL(*adapter_ptr, GetSystemLanguage()).WillOnce(testing::Return("aa"));
  EXPECT_EQ(ComputeLanguageByRegion("ug"), "");
  OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}
}  // namespace ohos
}  // namespace base
