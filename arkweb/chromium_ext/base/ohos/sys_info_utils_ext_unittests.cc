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
#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#define private public
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.cc"
#undef private
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "base/test/scoped_command_line.h"

namespace base {
namespace ohos {

class SystemPropertiesTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {
    if (properties_) {
      properties_ = nullptr;
    }
  }

  SystemProperties* properties_ = SystemProperties::Instance();
};

TEST_F(SystemPropertiesTest, ApplicationApiVersion001) {
  EXPECT_EQ(ApplicationApiVersion(), -1);
}

TEST_F(SystemPropertiesTest, ApplicationApiVersion002) {
  const char* test_argv[] = {"test_program",
                              "--ohos-app-api-version=invalid",
                              "--ohos-app-api-version=208",
                              nullptr};
  CommandLine::Init(3, test_argv);
  EXPECT_EQ(ApplicationApiVersion(), -1);
}

TEST_F(SystemPropertiesTest, ApplicationApiVersion003) {
  const char* test_argv[] = {"test_program",
                              "--ohos-app-api-version=invalid",
                              "--ohos-app-api-version=208",
                              nullptr};
  CommandLine::Init(3, test_argv);

  base::test::ScopedCommandLine scoped_cl;
  scoped_cl.GetProcessCommandLine()->AppendSwitchASCII(
      switches::kOhosAppApiVersion, "");
  
  EXPECT_EQ(ApplicationApiVersion(), -1);
}

TEST_F(SystemPropertiesTest, ApplicationApiVersion004) {
  const char* test_argv[] = {"test_program",
                              "--ohos-app-api-version=invalid",
                              "--ohos-app-api-version=208",
                              nullptr};
  CommandLine::Init(3, test_argv);

  base::test::ScopedCommandLine scoped_cl;
  scoped_cl.GetProcessCommandLine()->AppendSwitchASCII(
      switches::kOhosAppApiVersion, "99");
  
  EXPECT_EQ(ApplicationApiVersion(), 99);
}

TEST_F(SystemPropertiesTest, ApplicationApiVersion005) {
  const char* test_argv[] = {"test_program",
                              "--ohos-app-api-version=invalid",
                              "--ohos-app-api-version=208",
                              nullptr};
  CommandLine::Init(3, test_argv);

  base::test::ScopedCommandLine scoped_cl;
  scoped_cl.GetProcessCommandLine()->AppendSwitchASCII(
      switches::kOhosAppApiVersion, "105");
  
  EXPECT_EQ(ApplicationApiVersion(), 5);
}

TEST_F(SystemPropertiesTest, ApplicationApiVersion006) {
  const char* test_argv[] = {"test_program",
                              "--ohos-app-api-version=invalid",
                              "--ohos-app-api-version=208",
                              nullptr};
  CommandLine::Init(3, test_argv);

  base::test::ScopedCommandLine scoped_cl;
  scoped_cl.GetProcessCommandLine()->AppendSwitchASCII(
      switches::kOhosAppApiVersion, "0");
  
  EXPECT_EQ(ApplicationApiVersion(), 0);
}

TEST_F(SystemPropertiesTest, is_compatible_mode001) {
  EXPECT_FALSE(properties_->is_compatible_mode());
}

TEST_F(SystemPropertiesTest, is_compatible_mode002) {
  properties_->device_type_ = ProductDeviceType::DEVICE_TYPE_2IN1;
  properties_->compatible_device_type_ = "abc";
  EXPECT_FALSE(properties_->is_compatible_mode());
}

TEST_F(SystemPropertiesTest, is_compatible_mode003) {
  properties_->device_type_ = ProductDeviceType::DEVICE_TYPE_2IN1;
  properties_->compatible_device_type_ = "Tablet";
  EXPECT_TRUE(properties_->is_compatible_mode());
}

TEST_F(SystemPropertiesTest, is_compatible_mode004) {
  properties_->device_type_ = ProductDeviceType::DEVICE_TYPE_2IN1;
  properties_->compatible_device_type_ = "Phone";
  EXPECT_TRUE(properties_->is_compatible_mode());
}

TEST_F(SystemPropertiesTest, is_compatible_mode005) {
  properties_->device_type_ = ProductDeviceType::DEVICE_TYPE_TV;
  properties_->compatible_device_type_ = "Phone";
  EXPECT_FALSE(properties_->is_compatible_mode());
}
}  // namespace ohos
}  // namespace base
