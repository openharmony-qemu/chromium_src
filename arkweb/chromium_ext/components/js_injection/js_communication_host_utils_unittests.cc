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

#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "components/js_injection/browser/js_communication_host.h"
#define private public
#include "components/js_injection/renderer/js_communication_utils.h"
#include "arkweb/chromium_ext/components/js_injection/js_communication_host_utils.h"
#undef private

namespace js_injection {

class JsCommunicationHostUtilsTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
TEST_F(JsCommunicationHostUtilsTest, AddDocumentEndJavaScript001)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);

  std::u16string script = u"123";
  std::vector<std::string> allowed_origin_rules = {"0"};
  JsCommunicationHost::AddScriptResult result = utils.AddDocumentEndJavaScript(script, allowed_origin_rules);
  EXPECT_FALSE(result.error_message->empty());
}

TEST_F(JsCommunicationHostUtilsTest, RemoveDocumentEndJavaScript001)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  EXPECT_FALSE(utils.RemoveDocumentEndJavaScript(0));
}

TEST_F(JsCommunicationHostUtilsTest, RemoveDocumentEndJavaScript002)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  std::u16string str = u"1";
  OriginMatcher allowed_origin_rules;
  int32_t script_id = 0;
  DocumentEndJavaScript script1(str, allowed_origin_rules, script_id);
  utils.document_end_scripts_.push_back(std::move(script1));
  EXPECT_FALSE(utils.RemoveDocumentEndJavaScript(1));
}

TEST_F(JsCommunicationHostUtilsTest, NotifyFrameForAllDocumentEndsJavaScripts001)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  utils.NotifyFrameForAllDocumentEndsJavaScripts(nullptr);
  EXPECT_EQ(utils.document_end_scripts_.size(), 0);
}
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
TEST_F(JsCommunicationHostUtilsTest, AddHeadReadyJavaScript001)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  std::u16string script = u"123";
  std::vector<std::string> rules = {"0"};
  JsCommunicationHost::AddScriptResult result = utils.AddHeadReadyJavaScript(script, rules);
  EXPECT_FALSE(result.error_message->empty());
}

TEST_F(JsCommunicationHostUtilsTest, RemoveHeadReadyJavaScript001)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  EXPECT_FALSE(utils.RemoveHeadReadyJavaScript(0));
}

TEST_F(JsCommunicationHostUtilsTest, RemoveHeadReadyJavaScript002)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);

  std::u16string str = u"1";
  OriginMatcher allowed_origin_rules;
  int32_t script_id = 0;
  DocumentStartJavaScript script1(str, allowed_origin_rules, script_id);
  utils.head_ready_scripts_.push_back(std::move(script1));
  EXPECT_FALSE(utils.RemoveHeadReadyJavaScript(1));
}

TEST_F(JsCommunicationHostUtilsTest, AddHeadReadyPendingJavaScript001)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  std::u16string script = u"123";
  std::vector<std::string> rules = {"0"};
  std::vector<std::pair<std::string, std::string>> script_regex_rules;
  JsCommunicationHost::AddScriptResult result = utils.AddHeadReadyPendingJavaScript(
    script, rules, script_regex_rules);
  EXPECT_FALSE(result.error_message->empty());
}

TEST_F(JsCommunicationHostUtilsTest, AddHeadReadyPendingJavaScript002) {
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  std::u16string script = u"123";
  std::vector<std::string> rules = {"0", "1"};
  std::vector<std::pair<std::string, std::string>> script_regex_rules;
  JsCommunicationHost::AddScriptResult result = utils.AddHeadReadyPendingJavaScript(
    script, rules, script_regex_rules);
  EXPECT_FALSE(result.error_message->empty());
}

TEST_F(JsCommunicationHostUtilsTest, AddDocumentEndPendingJavaScript001)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  std::u16string script = u"123";
  std::vector<std::string> rules = {"0"};
  std::vector<std::pair<std::string, std::string>> script_regex_rules;
  JsCommunicationHost::AddScriptResult result = utils.AddDocumentEndPendingJavaScript(
    script, rules, script_regex_rules);
  EXPECT_FALSE(result.error_message->empty());
}

TEST_F(JsCommunicationHostUtilsTest, AddDocumentEndPendingJavaScript002) {
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  std::u16string script = u"123";
  std::vector<std::string> rules = {"0", "1"};
  std::vector<std::pair<std::string, std::string>> script_regex_rules;
  JsCommunicationHost::AddScriptResult result = utils.AddDocumentEndPendingJavaScript(
    script, rules, script_regex_rules);
  EXPECT_FALSE(result.error_message->empty());
}

TEST_F(JsCommunicationHostUtilsTest, AddDocumentStartPendingJavaScript001)
{
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  std::u16string script = u"123";
  std::vector<std::string> rules = {"0"};
  std::vector<std::pair<std::string, std::string>> script_regex_rules;
  JsCommunicationHost::AddScriptResult result = utils.AddDocumentStartPendingJavaScript(
    script, rules, script_regex_rules);
  EXPECT_FALSE(result.error_message->empty());
}

TEST_F(JsCommunicationHostUtilsTest, AddDocumentStartPendingJavaScript002) {
  JsCommunicationHost* host = nullptr;
  JsCommunicationHostUtils utils(host);
  std::u16string script = u"123";
  std::vector<std::string> rules = {"0", "1"};
  std::vector<std::pair<std::string, std::string>> script_regex_rules;
  JsCommunicationHost::AddScriptResult result = utils.AddDocumentStartPendingJavaScript(
    script, rules, script_regex_rules);
  EXPECT_FALSE(result.error_message->empty());
}
#endif
}