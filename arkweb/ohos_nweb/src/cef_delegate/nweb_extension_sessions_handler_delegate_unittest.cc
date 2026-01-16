/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "cef/include/cef_browser.h"
#include "content/public/browser/browser_context.h"
#include "base/files/file_path.h"
#include "base/memory/weak_ptr.h"
#define private public
#include "ohos_nweb/src/cef_delegate/nweb_extension_sessions_handler_delegate.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_sessions_handler_delegate.cc"
#undef private

namespace OHOS::NWeb {

class NWebExtensionSessionsHandlerDelegateTest : public ::testing::Test {
 protected:
  void SetUp() override {
    g_get_recently_closed_callbacks_.clear();
    g_restore_callbacks_.clear();
  }

  void TearDown() override {
    g_get_recently_closed_callbacks_.clear();
    g_restore_callbacks_.clear();
  }
};

TEST_F(NWebExtensionSessionsHandlerDelegateTest, OnRecentlyClosedReceived001) {
  int test_request_id = 1;
  bool callback_called = false;

  auto test_lambda = [](
      bool* cl_flag, 
      std::vector<NWebExtensionSession>& sessions,
      const std::optional<std::string>& error){
        *cl_flag = true;
      };

  base::RepeatingCallback<void(std::vector<NWebExtensionSession>&, const std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda, base::Unretained(&callback_called));  
  g_get_recently_closed_callbacks_.emplace(test_request_id, test_callback);

  EXPECT_EQ(g_get_recently_closed_callbacks_.count(test_request_id), 1);
  NWebExtensionSessionsGetRecentlyClosedCallbackParams params;
  params.sessions = {NWebExtensionSession{}, NWebExtensionSession{}};
  params.error = std::nullopt;

  NWebExtensionSessionsHandlerDelegate::OnRecentlyClosedReceived(test_request_id, params);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(g_get_recently_closed_callbacks_.count(test_request_id), 0);
}

TEST_F(NWebExtensionSessionsHandlerDelegateTest, OnRecentlyClosedReceived002) {
  int request_id = 999;
  bool callback_called = false;

  EXPECT_EQ(g_get_recently_closed_callbacks_.count(request_id), 0);
  
  NWebExtensionSessionsGetRecentlyClosedCallbackParams params;
  params.sessions = {NWebExtensionSession{}, NWebExtensionSession{}};
  params.error = std::nullopt;
  testing::internal::CaptureStderr();
  NWebExtensionSessionsHandlerDelegate::OnRecentlyClosedReceived(request_id, params);
  std::string log_out = testing::internal::GetCapturedStderr();
  EXPECT_FALSE(callback_called);
  EXPECT_EQ(g_get_recently_closed_callbacks_.count(request_id), 0);
  EXPECT_NE(log_out.find("NWeb GetRecentlyClosed callback called more than once"), std::string::npos);
}

TEST_F(NWebExtensionSessionsHandlerDelegateTest, OnSessionRestored001) {
  int test_request_id = 1;
  bool callback_called = false;

  auto test_lambda = [](
      bool* cl_flag, 
      std::optional<NWebExtensionSession>& sessions,
      const std::optional<std::string>& error){
        *cl_flag = true;
      };

  base::RepeatingCallback<void(std::optional<NWebExtensionSession>&, const std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda, base::Unretained(&callback_called));  
  g_restore_callbacks_.emplace(test_request_id, test_callback);

  EXPECT_EQ(g_restore_callbacks_.count(test_request_id), 1);
  NWebExtensionSessionsRestoreCallbackParams params;
  params.session = {NWebExtensionSession{}};
  params.error = std::nullopt;

  NWebExtensionSessionsHandlerDelegate::OnSessionRestored(test_request_id, params);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(g_restore_callbacks_.count(test_request_id), 0);
}

TEST_F(NWebExtensionSessionsHandlerDelegateTest, OnSessionRestored002) {
  int request_id = 999;
  bool callback_called = false;

  EXPECT_EQ(g_restore_callbacks_.count(request_id), 0);
  
  NWebExtensionSessionsRestoreCallbackParams params;
  params.session = NWebExtensionSession{};
  params.error = std::nullopt;
  testing::internal::CaptureStderr();
  NWebExtensionSessionsHandlerDelegate::OnSessionRestored(request_id, params);
  std::string log_out = testing::internal::GetCapturedStderr();
  EXPECT_FALSE(callback_called);
  EXPECT_EQ(g_restore_callbacks_.count(request_id), 0);
  EXPECT_NE(log_out.find("NWeb Restore callback called more than once"), std::string::npos);
}

}  // namespace OHOS::NWeb