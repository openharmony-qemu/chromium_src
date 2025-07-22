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

#include <memory>
#include <ranges>

#include "nweb_common.cc"
#include "nweb_common.h"

#define private public
#include "capi/nweb_notification_options_item.h"
#include "nweb_notification_manager_delegate.cc"
#include "nweb_notification_manager_delegate.h"
#undef private

namespace OHOS::NWeb {

class NWebNotificationManagerDelegateTest : public ::testing::Test {
 public:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(NWebNotificationManagerDelegateTest, GetOrCreateDelegateTest) {
  auto& delegate = NWebNotificationManagerDelegate::GetOrCreateDelegate();
  EXPECT_NE(delegate, nullptr);
}

TEST_F(NWebNotificationManagerDelegateTest,
       NWebNotificationManagerDelegateTest) {
  std::unique_ptr<NWebNotificationManagerDelegate> delegate;
  delegate = std::make_unique<NWebNotificationManagerDelegate>();
  EXPECT_NE(delegate, nullptr);
}

TEST_F(NWebNotificationManagerDelegateTest,
       RegisterNotificationManagerDelegateListenerTest_1stIf) {
  auto delegate = std::make_unique<NWebNotificationManagerDelegate>();
  EXPECT_NE(delegate, nullptr);
  auto listener = std::make_shared<NWebNotificationDelegateCallback>();
  delegate->RegisterNotificationManagerDelegateListener(listener);
  EXPECT_NE(NWebNotificationManagerDelegate::notification_delegate_listener_,
            nullptr);
}

TEST_F(NWebNotificationManagerDelegateTest,
       RegisterNotificationManagerDelegateListenerTest_2ndIf) {
  auto delegate = std::make_unique<NWebNotificationManagerDelegate>();
  EXPECT_NE(delegate, nullptr);
  NWebNotificationManagerDelegate::notification_delegate_listener_ =
      std::make_shared<NWebNotificationDelegateCallback>();
  delegate->RegisterNotificationManagerDelegateListener(nullptr);
  EXPECT_NE(NWebNotificationManagerDelegate::notification_delegate_listener_,
            nullptr);
}

TEST_F(NWebNotificationManagerDelegateTest, OnShowNotificationTest_1stIf) {
  EnableNativeApi();
  std::shared_ptr<NWebNotificationOptionsItem> options;
  EXPECT_EQ(options, nullptr);
  NWebNotificationManagerDelegate::OnShowNotification(options);
  bool result = IsNativeApiEnable();
  EXPECT_TRUE(result);
  g_is_native_api_enable = false;
}

TEST_F(NWebNotificationManagerDelegateTest, OnShowNotificationTest_2ndIf) {
  g_is_native_api_enable = false;
  bool result = IsNativeApiEnable();
  EXPECT_FALSE(result);
  std::shared_ptr<NWebNotificationOptionsItem> options;
  EXPECT_EQ(options, nullptr);
  NWebNotificationManagerDelegate::OnShowNotification(options);
  EXPECT_EQ(options, nullptr);
}

TEST_F(NWebNotificationManagerDelegateTest, OnShowNotificationTest_3rdIf) {
  g_is_native_api_enable = false;
  bool result = IsNativeApiEnable();
  EXPECT_FALSE(result);
  std::shared_ptr options = std::make_shared<NWebNotificationOptionsItem>();
  EXPECT_NE(options, nullptr);
  auto callback = std::make_shared<NWebNotificationDelegateCallback>();
  EXPECT_NE(callback, nullptr);
  callback->showNotification =
      [](std::shared_ptr<NWebNotificationOptionsItem>) {};
  callback->closeNotification = [](std::string) {};
  NWebNotificationManagerDelegate::notification_delegate_listener_ = callback;
  NWebNotificationManagerDelegate::OnShowNotification(options);
  EXPECT_NE(NWebNotificationManagerDelegate::notification_delegate_listener_,
            nullptr);
  NWebNotificationManagerDelegate::notification_delegate_listener_ = nullptr;
}

TEST_F(NWebNotificationManagerDelegateTest, OnShowNotificationTest_4thIf) {
  g_is_native_api_enable = false;
  bool result = IsNativeApiEnable();
  EXPECT_FALSE(result);
  std::shared_ptr options = std::make_shared<NWebNotificationOptionsItem>();
  EXPECT_NE(options, nullptr);
  NWebNotificationManagerDelegate::notification_delegate_listener_ = nullptr;
  NWebNotificationManagerDelegate::OnShowNotification(options);
  EXPECT_EQ(NWebNotificationManagerDelegate::notification_delegate_listener_,
            nullptr);
}

TEST_F(NWebNotificationManagerDelegateTest, OnCloseNotificationTest_1stIf) {
  std::string id = "123";
  EnableNativeApi();
  NWebNotificationManagerDelegate::OnCloseNotification(id);
  bool result = IsNativeApiEnable();
  EXPECT_TRUE(result);
  g_is_native_api_enable = false;
}

TEST_F(NWebNotificationManagerDelegateTest, OnCloseNotificationTest_2ndIf) {
  g_is_native_api_enable = false;
  std::string id = "123";
  bool result = IsNativeApiEnable();
  EXPECT_FALSE(result);
  auto callback = std::make_shared<NWebNotificationDelegateCallback>();
  EXPECT_NE(callback, nullptr);
  callback->showNotification =
      [](std::shared_ptr<NWebNotificationOptionsItem>) {};
  callback->closeNotification = [](std::string) {};
  NWebNotificationManagerDelegate::notification_delegate_listener_ = callback;
  NWebNotificationManagerDelegate::OnCloseNotification(id);
  EXPECT_NE(NWebNotificationManagerDelegate::notification_delegate_listener_,
            nullptr);
  NWebNotificationManagerDelegate::notification_delegate_listener_ = nullptr;
}

TEST_F(NWebNotificationManagerDelegateTest, OnCloseNotificationTest_3rdIf) {
  g_is_native_api_enable = false;
  std::string id = "123";
  bool result = IsNativeApiEnable();
  EXPECT_FALSE(result);
  NWebNotificationManagerDelegate::notification_delegate_listener_ = nullptr;
  NWebNotificationManagerDelegate::OnCloseNotification(id);
  EXPECT_EQ(NWebNotificationManagerDelegate::notification_delegate_listener_,
            nullptr);
}

TEST_F(NWebNotificationManagerDelegateTest, OnShowedTest) {
  const std::string id = "123";
  NWebNotificationManagerDelegate::OnShowed(id);
}

TEST_F(NWebNotificationManagerDelegateTest, OnClosedTest) {
  const std::string id = "123";
  NWebNotificationManagerDelegate::OnClosed(id);
}

TEST_F(NWebNotificationManagerDelegateTest, OnClickedTest) {
  const std::string id = "123";
  int buttonIndex = 123;
  NWebNotificationManagerDelegate::OnClicked(id, buttonIndex);
}
}  // namespace OHOS::NWeb
