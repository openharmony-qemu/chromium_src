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

#define private public
#include "base/ohos/blankless/blankless_controller.h"
#undef private
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace base {
namespace ohos {

class BlanklessControllerTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}

  BlanklessController& controller = BlanklessController::GetInstance();

  int32_t nweb_id = 1;
  std::string test_url = "test";
  bool globalProperty = false;
  bool deviceEnable = false;
};

TEST_F(BlanklessControllerTest, PrivacyStatus)
{
  EXPECT_FALSE(controller.GetPrivacyStatus(nweb_id));

  controller.SetPrivacyStatus(nweb_id, true);
  EXPECT_TRUE(controller.GetPrivacyStatus(nweb_id));
}

TEST_F(BlanklessControllerTest, EnabledUrlList)
{
  EXPECT_FALSE(controller.CheckEnableForUrl(test_url));

  std::vector<std::string> urls = {test_url};
  EXPECT_EQ(controller.AddEnabledUrlList(urls), 1);
  EXPECT_TRUE(controller.CheckEnableForUrl(test_url));

  controller.ClearEnabledUrlList();
  EXPECT_FALSE(controller.CheckEnableForUrl(test_url));
}
}  // namespace ohos
}  // namespace base