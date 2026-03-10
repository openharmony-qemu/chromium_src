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

#include "components/js_injection/renderer/js_communication.h"
#define private public
#include "arkweb/chromium_ext/components/js_injection/renderer/js_communication_utils.cc"
#include "arkweb/chromium_ext/components/js_injection/renderer/js_communication_utils.h"
#undef private

namespace js_injection {

class JsCommunicationUtilsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    js_communication_utils_ = std::make_unique<JsCommunicationUtils>(nullptr);
  }

  void TearDown() override {
    js_communication_utils_.reset();
  }

  mojom::DocumentEndJavaScriptPtr CreateEndScript(
      int32_t id,
      const std::u16string& script_content) {
    auto ptr = mojom::DocumentEndJavaScript::New();
    ptr->script_id = id;
    ptr->script = script_content;
    return ptr;
  }

  mojom::DocumentStartJavaScriptPtr CreateStartScript(
      int32_t id,
      const std::u16string& script_content) {
    auto ptr = mojom::DocumentStartJavaScript::New();
    ptr->script_id = id;
    ptr->script = script_content;
    return ptr;
  }

  mojom::DocumentJavaScriptRegexRulesPtr CreateRegexRules(
      int32_t id,
      const std::u16string& script_content,
      const std::vector<std::pair<std::string, std::string>>& rules = {}) {
    auto ptr = mojom::DocumentJavaScriptRegexRules::New();
    ptr->script_id = id;
    ptr->script = script_content;
    for (const auto& rule_pair : rules) {
      auto rule = mojom::ScriptRegexRule::New();

      rule->second_level_domain = base::UTF8ToUTF16(rule_pair.first);
      rule->rule = base::UTF8ToUTF16(rule_pair.second);
      ptr->script_regex_rules.push_back(std::move(rule));
    }
    return ptr;
  }

  std::unique_ptr<JsCommunicationUtils> js_communication_utils_;
};

bool MatchUrlRegex(const std::string& url, const std::string& regex);

TEST_F(JsCommunicationUtilsTest, MatchUrlRegex_001) {
  EXPECT_FALSE(MatchUrlRegex("", "pattern"));
  EXPECT_FALSE(MatchUrlRegex("http://example.com", ""));
  EXPECT_FALSE(MatchUrlRegex("", ""));
}

TEST_F(JsCommunicationUtilsTest, MatchUrlRegex_002) {
  EXPECT_FALSE(MatchUrlRegex("http://example.com", "*abc"));
  EXPECT_FALSE(MatchUrlRegex("http://example.com", "+abc"));
  EXPECT_FALSE(MatchUrlRegex("http://example.com", "?abc"));
  EXPECT_FALSE(MatchUrlRegex("http://example.com", "{abc"));
}

TEST_F(JsCommunicationUtilsTest, MatchUrlRegex_003) {
  EXPECT_FALSE(MatchUrlRegex("http://example.com", "(unclosed"));
  EXPECT_FALSE(MatchUrlRegex("http://example.com", "\\"));
  EXPECT_FALSE(MatchUrlRegex("http://example.com", "[]"));
  EXPECT_TRUE(MatchUrlRegex("http://example.com/script.js", ".*\\.js$"));
  EXPECT_FALSE(MatchUrlRegex("http://example.com/page.html", ".*\\.js$"));
}

TEST_F(JsCommunicationUtilsTest, RemoveDocumentEndScript_001) {
  auto script1 = CreateEndScript(1, u"script1");
  auto script2 = CreateEndScript(2, u"script2");
  js_communication_utils_->AddDocumentEndScript(script1);
  js_communication_utils_->AddDocumentEndScript(script2);

  ASSERT_EQ(js_communication_utils_->document_end_scripts_.size(), 2u);

  js_communication_utils_->RemoveDocumentEndScript(1);

  ASSERT_EQ(js_communication_utils_->document_end_scripts_.size(), 1u);
  EXPECT_EQ(js_communication_utils_->document_end_scripts_[0]->script_id, 2);
  EXPECT_EQ(js_communication_utils_->document_end_scripts_[0]->script,
            blink::WebString::FromUTF16(u"script2"));
}

TEST_F(JsCommunicationUtilsTest, RemoveDocumentEndScript_002) {
  auto script = CreateEndScript(1, u"script");
  js_communication_utils_->AddDocumentEndScript(script);
  ASSERT_EQ(js_communication_utils_->document_end_scripts_.size(), 1u);

  js_communication_utils_->RemoveDocumentEndScript(99);

  ASSERT_EQ(js_communication_utils_->document_end_scripts_.size(), 1u);
  EXPECT_EQ(js_communication_utils_->document_end_scripts_[0]->script_id, 1);
}

TEST_F(JsCommunicationUtilsTest, RemoveDocumentEndScript_003) {
  ASSERT_TRUE(js_communication_utils_->document_end_scripts_.empty());

  js_communication_utils_->RemoveDocumentEndScript(100);

  EXPECT_TRUE(js_communication_utils_->document_end_scripts_.empty());
}

TEST_F(JsCommunicationUtilsTest, RemoveHeadReadyScript_001) {
  auto script1 = CreateStartScript(1, u"script1");
  auto script2 = CreateStartScript(2, u"script2");
  js_communication_utils_->AddHeadReadyScript(script1);
  js_communication_utils_->AddHeadReadyScript(script2);

  ASSERT_EQ(js_communication_utils_->head_ready_scripts_.size(), 2u);

  js_communication_utils_->RemoveHeadReadyScript(1);

  ASSERT_EQ(js_communication_utils_->head_ready_scripts_.size(), 1u);
  EXPECT_EQ(js_communication_utils_->head_ready_scripts_[0]->script_id, 2);
  EXPECT_EQ(js_communication_utils_->head_ready_scripts_[0]->script,
            blink::WebString::FromUTF16(u"script2"));
}

TEST_F(JsCommunicationUtilsTest, RemoveHeadReadyScript_002) {
  auto script = CreateStartScript(1, u"script");
  js_communication_utils_->AddHeadReadyScript(script);
  ASSERT_EQ(js_communication_utils_->head_ready_scripts_.size(), 1u);

  js_communication_utils_->RemoveHeadReadyScript(99);

  ASSERT_EQ(js_communication_utils_->head_ready_scripts_.size(), 1u);
  EXPECT_EQ(js_communication_utils_->head_ready_scripts_[0]->script_id, 1);
}

TEST_F(JsCommunicationUtilsTest, RemoveHeadReadyScript_003) {
  ASSERT_TRUE(js_communication_utils_->head_ready_scripts_.empty());

  js_communication_utils_->RemoveHeadReadyScript(100);

  EXPECT_TRUE(js_communication_utils_->head_ready_scripts_.empty());
}

TEST_F(JsCommunicationUtilsTest, CommitPendingJavascriptsAtDocumentEnd_001) {
  auto existing_script = CreateEndScript(99, u"existing");
  js_communication_utils_->AddDocumentEndScript(existing_script);
  auto existing_regex = CreateRegexRules(99, u"existing");
  auto regex_obj = js_communication_utils_->CreateDocumentJavaScriptRegexRules(existing_regex);
  js_communication_utils_->end_scripts_regex_rules_.push_back(std::move(regex_obj));

  ASSERT_EQ(js_communication_utils_->document_end_scripts_.size(), 1u);
  ASSERT_EQ(js_communication_utils_->end_scripts_regex_rules_.size(), 1u);
  ASSERT_TRUE(js_communication_utils_->swap_document_end_scripts_.empty());
  ASSERT_TRUE(js_communication_utils_->swap_end_scripts_regex_rules_.empty());

  js_communication_utils_->CommitPendingJavascriptsAtDocumentEnd();

  EXPECT_TRUE(js_communication_utils_->document_end_scripts_.empty());
  EXPECT_TRUE(js_communication_utils_->end_scripts_regex_rules_.empty());
  EXPECT_TRUE(js_communication_utils_->swap_document_end_scripts_.empty());
  EXPECT_TRUE(js_communication_utils_->swap_end_scripts_regex_rules_.empty());
}

TEST_F(JsCommunicationUtilsTest, CommitPendingJavascriptsAtDocumentEnd_002) {
  auto script = CreateEndScript(1, u"script");
  auto regex = CreateRegexRules(1, u"regex");
  js_communication_utils_->AddPendingJavascriptAtDocumentEnd(script, regex);

  ASSERT_EQ(js_communication_utils_->swap_document_end_scripts_.size(), 1u);
  ASSERT_EQ(js_communication_utils_->swap_end_scripts_regex_rules_.size(), 1u);

  js_communication_utils_->CommitPendingJavascriptsAtDocumentEnd();

  EXPECT_TRUE(js_communication_utils_->swap_document_end_scripts_.empty());
  EXPECT_TRUE(js_communication_utils_->swap_end_scripts_regex_rules_.empty());
}

TEST_F(JsCommunicationUtilsTest, CommitPendingJavascriptsAtHeadReady_001) {
  auto existing_script = CreateStartScript(99, u"existing");
  js_communication_utils_->AddHeadReadyScript(existing_script);
  auto existing_regex = CreateRegexRules(99, u"existing");
  auto regex_obj = js_communication_utils_->CreateDocumentJavaScriptRegexRules(existing_regex);
  js_communication_utils_->head_ready_regex_rules_.push_back(std::move(regex_obj));

  ASSERT_EQ(js_communication_utils_->head_ready_scripts_.size(), 1u);
  ASSERT_EQ(js_communication_utils_->head_ready_regex_rules_.size(), 1u);
  ASSERT_TRUE(js_communication_utils_->swap_head_ready_scripts_.empty());
  ASSERT_TRUE(js_communication_utils_->swap_head_ready_regex_rules_.empty());

  js_communication_utils_->CommitPendingJavascriptsAtHeadReady();

  EXPECT_TRUE(js_communication_utils_->head_ready_scripts_.empty());
  EXPECT_TRUE(js_communication_utils_->head_ready_regex_rules_.empty());
  EXPECT_TRUE(js_communication_utils_->swap_head_ready_scripts_.empty());
  EXPECT_TRUE(js_communication_utils_->swap_head_ready_regex_rules_.empty());
}

TEST_F(JsCommunicationUtilsTest, CommitPendingJavascriptsAtHeadReady_002) {
  auto script = CreateStartScript(1, u"script");
  auto regex = CreateRegexRules(1, u"regex");
  js_communication_utils_->AddPendingJavascriptAtHeadReady(script, regex);

  ASSERT_EQ(js_communication_utils_->swap_head_ready_scripts_.size(), 1u);
  ASSERT_EQ(js_communication_utils_->swap_head_ready_regex_rules_.size(), 1u);

  js_communication_utils_->CommitPendingJavascriptsAtHeadReady();

  EXPECT_TRUE(js_communication_utils_->swap_head_ready_scripts_.empty());
  EXPECT_TRUE(js_communication_utils_->swap_head_ready_regex_rules_.empty());
}

TEST_F(JsCommunicationUtilsTest, CommitPendingJavascriptsAtDocumentStartRegexRules_001) {
  auto existing_regex = CreateRegexRules(99, u"existing", {{"old.com", ".*"}});
  auto regex_obj = js_communication_utils_->CreateDocumentJavaScriptRegexRules(
      existing_regex);
  js_communication_utils_->start_scripts_regex_rules_.push_back(
      std::move(regex_obj));

  ASSERT_EQ(js_communication_utils_->start_scripts_regex_rules_.size(), 1u);
  ASSERT_TRUE(js_communication_utils_->swap_start_scripts_regex_rules_.empty());

  js_communication_utils_->CommitPendingJavascriptsAtDocumentStartRegexRules();

  EXPECT_TRUE(js_communication_utils_->start_scripts_regex_rules_.empty());
  EXPECT_TRUE(js_communication_utils_->swap_start_scripts_regex_rules_.empty());
}

TEST_F(JsCommunicationUtilsTest, CommitPendingJavascriptsAtDocumentStartRegexRules_002) {
  auto regex = CreateRegexRules(1, u"regex");
  js_communication_utils_->AddPendingJavascriptAtDocumentStartRegexRules(regex);

  ASSERT_EQ(js_communication_utils_->swap_start_scripts_regex_rules_.size(), 1u);

  js_communication_utils_->CommitPendingJavascriptsAtDocumentStartRegexRules();

  EXPECT_TRUE(js_communication_utils_->swap_start_scripts_regex_rules_.empty());
}
}  // namespace js_injection
