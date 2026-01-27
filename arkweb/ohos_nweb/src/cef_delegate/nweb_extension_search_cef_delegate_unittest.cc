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

#if BUILDFLAG(ARKWEB_NWEB_EX) 
#include "ohos_nweb_ex/core/extension/nweb_extension_search_dispatcher.h"
#endif

#define private public
#include "ohos_nweb/src/cef_delegate/nweb_extension_search_cef_delegate.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_search_cef_delegate.cc"
#undef private

namespace OHOS::NWeb {

class NWebExtensionSearchCefDelegateTest : public ::testing::Test {
 protected:
  void SetUp() override {
#if BUILDFLAG(ARKWEB_NWEB_EX) 
    g_search_callback_ = std::make_shared<ArkWebExtensionSearchCallback>(nullptr);
#endif

    g_search_callback_map_.clear();
  }

  void TearDown() override {
#if BUILDFLAG(ARKWEB_NWEB_EX) 
    g_search_callback_ = nullptr;
#endif

    g_search_callback_map_.clear();
  }

#if BUILDFLAG(ARKWEB_NWEB_EX) 
  std::shared_ptr<ArkWebExtensionSearchCallback> g_search_callback_ = nullptr;
#endif
};

TEST_F(NWebExtensionSearchCefDelegateTest, Query001) {
#if BUILDFLAG(ARKWEB_NWEB_EX) 
  NWebExtensionSearchQueryInfo query_info = {};
  query_info.disposition = 0;
  query_info.tab_id = 1;
  query_info.text = "test_text";
  query_info.contextType = "background";
  query_info.includeIncognitoInfo = false;
  SearchCallback callback;
  EXPECT_FALSE(NWebExtensionSearchCefDelegate::Query(callback, query_info));
#endif
}

TEST_F(NWebExtensionSearchCefDelegateTest, Query002) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionSearchQueryInfo query_info = {};
  query_info.disposition = 0;
  query_info.tab_id = 1;
  query_info.text = "test_text";
  query_info.contextType = "background";
  query_info.includeIncognitoInfo = false;
  auto test_lambda = [](const std::optional<std::string>& error){(void)error;};

  base::RepeatingCallback<void(const std::optional<std::string>&)> test_callback = base::BindRepeating(test_lambda);
  NWebExtensionSearchDispatcher::RegistArkWebExtensionSearchCallback(g_search_callback_);
  auto query_func = [](int32_t request_id, const ArkWebPbBuffer* pb_buffer) {};
  g_search_callback_->on_query = query_func;  
  EXPECT_TRUE(NWebExtensionSearchCefDelegate::Query(test_callback, query_info));
  NWebExtensionSearchDispatcher::UnRegistArkWebExtensionSearchCallback();
#endif
}

TEST_F(NWebExtensionSearchCefDelegateTest, QueryCallback001) {
  int test_request_id = 1;
  const std::optional<std::string> test_error = "test_error";
  bool callback_called = false;

  auto test_lambda = [](
      bool* cl_flag, 
      const std::optional<std::string>& error){
        *cl_flag = true;
      };

  base::RepeatingCallback<void(const std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda, base::Unretained(&callback_called));  
  g_search_callback_map_.emplace(test_request_id, test_callback);

  EXPECT_EQ(g_search_callback_map_.count(test_request_id), 1);

  NWebExtensionSearchCefDelegate::QueryCallback(test_request_id, test_error);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(g_search_callback_map_.count(test_request_id), 0);
}

TEST_F(NWebExtensionSearchCefDelegateTest, QueryCallback002) {
  int request_id = 999;
  const std::optional<std::string> test_error = "test_error";
  bool callback_called = false;

  EXPECT_EQ(g_search_callback_map_.count(request_id), 0);
  
  NWebExtensionSearchCefDelegate::QueryCallback(request_id, test_error);
  EXPECT_FALSE(callback_called);
  EXPECT_EQ(g_search_callback_map_.count(request_id), 0);
}
}