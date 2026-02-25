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

#include "base/files/file_path.h"
#include "base/memory/weak_ptr.h"
#include "cef/include/cef_browser.h"
#include "content/public/browser/browser_context.h"
#define private public
#include "ohos_nweb/src/cef_delegate/nweb_extension_tab_groups_cef_delegate.cc"
#include "ohos_nweb/src/cef_delegate/nweb_extension_tab_groups_cef_delegate.h"
#undef private

namespace OHOS::NWeb {

class NWebExtensionTabGroupsCefDelegateTest : public ::testing::Test {
 protected:
  void SetUp() override {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    g_tab_groups_callback_ =
        std::make_shared<ArkWebExtensionTabGroupsCallback>(nullptr);
#endif

    g_get_tab_group_callback_map_.clear();
    g_move_tab_group_callback_map_.clear();
    g_query_tab_group_callback_map_.clear();
    g_update_tab_group_callback_map_.clear();
  }

  void TearDown() override {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    g_tab_groups_callback_ = nullptr;
#endif

    g_get_tab_group_callback_map_.clear();
    g_move_tab_group_callback_map_.clear();
    g_query_tab_group_callback_map_.clear();
    g_update_tab_group_callback_map_.clear();
  }

#if BUILDFLAG(ARKWEB_NWEB_EX)
  std::shared_ptr<ArkWebExtensionTabGroupsCallback> g_tab_groups_callback_ =
      nullptr;
#endif
};

TEST_F(NWebExtensionTabGroupsCefDelegateTest, GetBrowserContextInUse001) {
  NWebExtensionTabGroup tabGroup;
  EXPECT_NO_FATAL_FAILURE(GetBrowserContextInUse(tabGroup));
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, GetBrowserContextInUse002) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = false;
  EXPECT_NO_FATAL_FAILURE(GetBrowserContextInUse(tabGroup));
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, GetTabGroup001) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionFunctionContext function_context = {};
  GetTabGroupsCallback callback;
  EXPECT_FALSE(NWebExtensionTabGroupsCefDelegate::GetTabGroup(
      callback, 1, function_context));
#endif
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, GetTabGroup002) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionFunctionContext function_context = {};
  function_context.context_type = "background";
  function_context.extension_id = "test_extension";
  function_context.include_incognito_info = true;
  auto test_lambda = [](const NWebExtensionTabGroup& tab_group,
                        std::optional<std::string>& error) { (void)error; };

  base::RepeatingCallback<void(const NWebExtensionTabGroup&,std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda);
  NWebExtensionTabGroupsDispatcher::RegistArkWebExtensionTabGroupsCallback(
      g_tab_groups_callback_);
  auto get_func = [](int32_t request_id, int32_t groupId,
                     const ArkWebPbBuffer* pb_buffer) {};
  g_tab_groups_callback_->on_get = get_func;
  EXPECT_TRUE(NWebExtensionTabGroupsCefDelegate::GetTabGroup(test_callback, 1,
                                                             function_context));
  NWebExtensionTabGroupsDispatcher::UnRegistArkWebExtensionTabGroupsCallback();
#endif
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, GetTabGroupCallback001) {
  int test_request_id = 1;
  NWebExtensionTabGroup tabGroup;
  std::optional<std::string> test_error = "test_error";

  auto test_lambda = [](const NWebExtensionTabGroup& tab_group,
                        std::optional<std::string>& error) { (void)error; };

  base::RepeatingCallback<void(const NWebExtensionTabGroup&,std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda);
  g_get_tab_group_callback_map_.emplace(test_request_id, test_callback);

  EXPECT_EQ(g_get_tab_group_callback_map_.count(test_request_id), 1);

  NWebExtensionTabGroupsCefDelegate::GetTabGroupCallback(test_request_id,
                                                         tabGroup, test_error);
  EXPECT_EQ(g_get_tab_group_callback_map_.count(test_request_id), 0);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, GetTabGroupCallback002) {
  int request_id = 999;
  NWebExtensionTabGroup tabGroup;
  std::optional<std::string> test_error = "test_error";

  EXPECT_EQ(g_get_tab_group_callback_map_.count(request_id), 0);

  NWebExtensionTabGroupsCefDelegate::GetTabGroupCallback(request_id, tabGroup,
                                                         test_error);
  EXPECT_EQ(g_get_tab_group_callback_map_.count(request_id), 0);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, MoveTabGroup001) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionTabGroupsMoveProperties move_properties;
  MoveTabGroupsCallback callback;
  EXPECT_FALSE(NWebExtensionTabGroupsCefDelegate::MoveTabGroup(
      callback, 1, move_properties));
#endif
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, MoveTabGroup002) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionTabGroupsMoveProperties move_properties;
  move_properties.index = 1;
  move_properties.window_id = 10;
  move_properties.contextType = "background";
  move_properties.includeIncognitoInfo = true;
  auto test_lambda = [](const NWebExtensionTabGroup& tab_group,
                        std::optional<std::string>& error) { (void)error; };

  base::RepeatingCallback<void(const NWebExtensionTabGroup&,std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda);
  NWebExtensionTabGroupsDispatcher::RegistArkWebExtensionTabGroupsCallback(
      g_tab_groups_callback_);
  auto move_func = [](int32_t request_id, int32_t groupId,
                      const ArkWebPbBuffer* pb_buffer) {};
  g_tab_groups_callback_->on_move = move_func;
  EXPECT_TRUE(NWebExtensionTabGroupsCefDelegate::MoveTabGroup(test_callback, 1,
                                                              move_properties));
  NWebExtensionTabGroupsDispatcher::UnRegistArkWebExtensionTabGroupsCallback();
#endif
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, MoveTabGroupCallback001) {
  int test_request_id = 1;
  NWebExtensionTabGroup tabGroup;
  std::optional<std::string> test_error = "test_error";

  auto test_lambda = [](const NWebExtensionTabGroup& tab_group,
                        std::optional<std::string>& error) { (void)error; };

  base::RepeatingCallback<void(const NWebExtensionTabGroup&,std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda);
  g_move_tab_group_callback_map_.emplace(test_request_id, test_callback);

  EXPECT_EQ(g_move_tab_group_callback_map_.count(test_request_id), 1);

  NWebExtensionTabGroupsCefDelegate::MoveTabGroupCallback(test_request_id,
                                                          tabGroup, test_error);
  EXPECT_EQ(g_move_tab_group_callback_map_.count(test_request_id), 0);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, MoveTabGroupCallback002) {
  int request_id = 999;
  NWebExtensionTabGroup tabGroup;
  std::optional<std::string> test_error = "test_error";

  EXPECT_EQ(g_move_tab_group_callback_map_.count(request_id), 0);

  NWebExtensionTabGroupsCefDelegate::MoveTabGroupCallback(request_id, tabGroup,
                                                          test_error);
  EXPECT_EQ(g_move_tab_group_callback_map_.count(request_id), 0);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, QueryTabGroup001) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  QueryTabGroupsCallback callback;
  NWebExtensionTabGroupsQueryInfo query_info;
  EXPECT_FALSE(
      NWebExtensionTabGroupsCefDelegate::QueryTabGroup(callback, query_info));
#endif
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, QueryTabGroup002) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionTabGroupsQueryInfo query_info;
  query_info.collapsed = false;
  query_info.color = NWebExtensionTabGroupsColor::BLUE;
  query_info.contextType = "background";
  query_info.includeIncognitoInfo = true;
  query_info.shared = true;
  query_info.title = "test_tile";
  query_info.window_id = 10;

  auto test_lambda = [](const std::vector<NWebExtensionTabGroup>& tab_groups,
                        std::optional<std::string>& error) { (void)error; };

  base::RepeatingCallback<void(const std::vector<NWebExtensionTabGroup>&,std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda);
  NWebExtensionTabGroupsDispatcher::RegistArkWebExtensionTabGroupsCallback(
      g_tab_groups_callback_);
  auto query_func = [](int32_t request_id, const ArkWebPbBuffer* pb_buffer) {};
  g_tab_groups_callback_->on_query = query_func;
  EXPECT_TRUE(NWebExtensionTabGroupsCefDelegate::QueryTabGroup(test_callback,
                                                               query_info));
  NWebExtensionTabGroupsDispatcher::UnRegistArkWebExtensionTabGroupsCallback();
#endif
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, QueryTabGroupCallback001) {
  int test_request_id = 1;
  std::vector<NWebExtensionTabGroup> tab_groups;
  std::optional<std::string> test_error = "test_error";

  auto test_lambda = [](const std::vector<NWebExtensionTabGroup>& tab_groups,
                        std::optional<std::string>& error) { (void)error; };

  base::RepeatingCallback<void(const std::vector<NWebExtensionTabGroup>&,std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda);
  g_query_tab_group_callback_map_.emplace(test_request_id, test_callback);

  EXPECT_EQ(g_query_tab_group_callback_map_.count(test_request_id), 1);

  NWebExtensionTabGroupsCefDelegate::QueryTabGroupCallback(
      test_request_id, tab_groups, test_error);
  EXPECT_EQ(g_query_tab_group_callback_map_.count(test_request_id), 0);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, QueryTabGroupCallback002) {
  int request_id = 999;
  std::vector<NWebExtensionTabGroup> tab_groups;
  std::optional<std::string> test_error = "test_error";

  EXPECT_EQ(g_query_tab_group_callback_map_.count(request_id), 0);

  NWebExtensionTabGroupsCefDelegate::QueryTabGroupCallback(
      request_id, tab_groups, test_error);
  EXPECT_EQ(g_query_tab_group_callback_map_.count(request_id), 0);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, UpdateTabGroup001) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  UpdateTabGroupsCallback callback;
  NWebExtensionTabGroupsUpdateProperties update_properties;
  EXPECT_FALSE(NWebExtensionTabGroupsCefDelegate::UpdateTabGroup(
      callback, 1, update_properties));
#endif
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, UpdateTabGroup002) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionTabGroupsUpdateProperties update_properties;
  update_properties.collapsed = false;
  update_properties.color = NWebExtensionTabGroupsColor::BLUE;
  update_properties.contextType = "background";
  update_properties.includeIncognitoInfo = true;
  update_properties.title = "test_title";

  auto test_lambda = [](const NWebExtensionTabGroup& tab_group,
                        std::optional<std::string>& error) { (void)error; };

  base::RepeatingCallback<void(const NWebExtensionTabGroup&,std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda);
  NWebExtensionTabGroupsDispatcher::RegistArkWebExtensionTabGroupsCallback(
      g_tab_groups_callback_);
  auto update_func = [](int32_t request_id, int32_t groupId,
                        const ArkWebPbBuffer* pb_buffer) {};
  g_tab_groups_callback_->on_update = update_func;
  EXPECT_TRUE(NWebExtensionTabGroupsCefDelegate::UpdateTabGroup(
      test_callback, 1, update_properties));
  NWebExtensionTabGroupsDispatcher::UnRegistArkWebExtensionTabGroupsCallback();
#endif
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, UpdateTabGroupCallback001) {
  int test_request_id = 1;
  NWebExtensionTabGroup tabGroup;
  std::optional<std::string> test_error = "test_error";

  auto test_lambda = [](const NWebExtensionTabGroup& tab_group,
                        std::optional<std::string>& error) { (void)error; };

  base::RepeatingCallback<void(const NWebExtensionTabGroup&,std::optional<std::string>&)>
        test_callback = base::BindRepeating(test_lambda);
  g_update_tab_group_callback_map_.emplace(test_request_id, test_callback);

  EXPECT_EQ(g_update_tab_group_callback_map_.count(test_request_id), 1);

  NWebExtensionTabGroupsCefDelegate::UpdateTabGroupCallback(
      test_request_id, tabGroup, test_error);
  EXPECT_EQ(g_update_tab_group_callback_map_.count(test_request_id), 0);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, UpdateTabGroupCallback002) {
  int request_id = 999;
  NWebExtensionTabGroup tabGroup;
  std::optional<std::string> test_error = "test_error";

  EXPECT_EQ(g_update_tab_group_callback_map_.count(request_id), 0);

  NWebExtensionTabGroupsCefDelegate::UpdateTabGroupCallback(
      request_id, tabGroup, test_error);
  EXPECT_EQ(g_update_tab_group_callback_map_.count(request_id), 0);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, TabGroupCreated001) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = std::nullopt;
  testing::internal::CaptureStderr();
  NWebExtensionTabGroupsCefDelegate::TabGroupCreated(tabGroup);
  std::string log_output_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output_err.find("TabGroupCreated get browser context failed."),
            std::string::npos);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, TabGroupCreated002) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = false;
  testing::internal::CaptureStderr();
  NWebExtensionTabGroupsCefDelegate::TabGroupCreated(tabGroup);
  std::string log_output_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output_err.find("TabGroupCreated get browser context failed."),
            std::string::npos);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, TabGroupMoved001) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = std::nullopt;
  testing::internal::CaptureStderr();
  NWebExtensionTabGroupsCefDelegate::TabGroupMoved(tabGroup);
  std::string log_output_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output_err.find("TabGroupMoved get browser context failed."),
            std::string::npos);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, TabGroupMoved002) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = false;
  testing::internal::CaptureStderr();
  NWebExtensionTabGroupsCefDelegate::TabGroupMoved(tabGroup);
  std::string log_output_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output_err.find("TabGroupMoved get browser context failed."),
            std::string::npos);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, TabGroupRemoved001) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = std::nullopt;
  testing::internal::CaptureStderr();
  NWebExtensionTabGroupsCefDelegate::TabGroupRemoved(tabGroup);
  std::string log_output_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output_err.find("TabGroupRemoved get browser context failed."),
            std::string::npos);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, TabGroupRemoved002) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = false;
  testing::internal::CaptureStderr();
  NWebExtensionTabGroupsCefDelegate::TabGroupRemoved(tabGroup);
  std::string log_output_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output_err.find("TabGroupRemoved get browser context failed."),
            std::string::npos);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, TabGroupUpdated001) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = std::nullopt;
  testing::internal::CaptureStderr();
  NWebExtensionTabGroupsCefDelegate::TabGroupUpdated(tabGroup);
  std::string log_output_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output_err.find("TabGroupUpdated get browser context failed."),
            std::string::npos);
}

TEST_F(NWebExtensionTabGroupsCefDelegateTest, TabGroupUpdated002) {
  NWebExtensionTabGroup tabGroup;
  tabGroup.incognito = false;
  testing::internal::CaptureStderr();
  NWebExtensionTabGroupsCefDelegate::TabGroupUpdated(tabGroup);
  std::string log_output_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output_err.find("TabGroupUpdated get browser context failed."),
            std::string::npos);
}
}  // namespace OHOS::NWeb