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

#include "arkweb/chromium_ext/components/js_injection/renderer/js_communication_utils.h"

#include "base/strings/utf_string_conversions.h"
#include "components/url_formatter/url_fixer.h"
#include "components/url_formatter/url_formatter.h"
#include "content/public/renderer/render_frame.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_script_source.h"
#include "third_party/re2/src/re2/re2.h"

namespace js_injection {

JsCommunicationUtils::JsCommunicationUtils(JsCommunication* impl)
    : jsCommunication_(impl) {}

bool MatchUrlRegex(const std::string& url, const std::string& regex) {
  if (url.empty() || regex.empty()) {
    return false;
  }
  if (regex[0] == '*' || regex[0] == '+' || regex[0] == '?' ||
      regex[0] == '{') {
    LOG(ERROR) << "Invalid regex: starts with an unsupported character:'*','+','?','{'.";
    return false;
  }

  const RE2 pattern(regex);
  if (!pattern.ok()) {
    LOG(ERROR) << "MatchUrlRegex: Failed to convert regex to RE2.";
    return false;
  }

  std::vector<re2::StringPiece> match_groups(pattern.NumberOfCapturingGroups() + 1);
  return pattern.Match(url, 0, url.size(), RE2::UNANCHORED, match_groups.data(),
                       match_groups.size());
}

bool JsCommunicationUtils::MatchUrlRegexRules(blink::WebString& script,
  std::vector<std::unique_ptr<DocumentJavaScriptRegexRules>>& scripts_regex_rules) {
  std::string url_text =
      jsCommunication_->render_frame()->GetWebFrame()->GetDocument().Url().GetString().Utf8();
  
  GURL url = url_formatter::FixupURL(url_text, "");
  if (!url.is_valid() || scripts_regex_rules.empty()) {
    LOG(DEBUG) << "MatchUrlRegexRules: fixupURL failed or scripts_regex_rules is empty.";
    return false;
  }

  std::string frame_origin =  net::registry_controlled_domains::GetDomainAndRegistry(
      url, net::registry_controlled_domains::PrivateRegistryFilter::INCLUDE_PRIVATE_REGISTRIES);
  for (const auto& rule : scripts_regex_rules) {
    if (rule->script != script) {
      continue;
    }

    for (const auto& regexRule : rule->script_regex_rules) {
      if (frame_origin != regexRule.second_level_domain) {
        continue;
      }

      if (MatchUrlRegex(url_text, regexRule.rule)) {
        LOG(DEBUG) << "MatchUrlRegexRule: regexRules match success.";
        return true;
      }
    }
    break;
  }

  LOG(DEBUG) << "JsCommunicationUtils::MatchUrlRegexRules regexRules no match.";
  return false;
}

std::unique_ptr<JsCommunicationUtils::DocumentJavaScriptRegexRules>
JsCommunicationUtils::CreateDocumentJavaScriptRegexRules(
    const mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr) {
  auto script_regex_rules = std::make_unique<JsCommunicationUtils::DocumentJavaScriptRegexRules>();
  script_regex_rules->script_id = script_regex_rules_ptr->script_id;
  script_regex_rules->script = blink::WebString::FromUTF16(script_regex_rules_ptr->script);

  for (const auto& mojom_rule : script_regex_rules_ptr->script_regex_rules) {
    ScriptRegexRule regex_rule;
    regex_rule.second_level_domain = base::UTF16ToUTF8(mojom_rule->second_level_domain);
    regex_rule.rule = base::UTF16ToUTF8(mojom_rule->rule);
    script_regex_rules->script_regex_rules.push_back(regex_rule);
  }

  return script_regex_rules;
}

void JsCommunicationUtils::AddDocumentEndScript(
    mojom::DocumentEndJavaScriptPtr& script_ptr)
{
  DocumentEndJavaScript* script = new DocumentEndJavaScript{
      script_ptr->origin_matcher,
      blink::WebString::FromUTF16(script_ptr->script), script_ptr->script_id};
  document_end_scripts_.push_back(
      std::unique_ptr<DocumentEndJavaScript>(script));
}

void JsCommunicationUtils::RemoveDocumentEndScript(int32_t script_id)
{
  for (auto it = document_end_scripts_.begin();
       it != document_end_scripts_.end(); ++it) {
    if ((*it)->script_id == script_id) {
      document_end_scripts_.erase(it);
      break;
    }
  }
}

void JsCommunicationUtils::AddDocumentEndScriptRegexRules(
  mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr) {
  auto script_regex_rules = CreateDocumentJavaScriptRegexRules(script_regex_rules_ptr);
  end_scripts_regex_rules_.push_back(std::move(script_regex_rules));
}

void JsCommunicationUtils::RunScriptsAtDocumentEnd() {
  RunScriptsAtDocumentEndInternal(weak_ptr_factory_.GetWeakPtr());
  // Careful 'this' may be destroyed.
}

void JsCommunicationUtils::RunScriptsAtDocumentEndInternal(
    base::WeakPtr<JsCommunicationUtils> js_communication_utils) {
  CHECK(js_communication_utils);
  url::Origin frame_origin = url::Origin(
      js_communication_utils->jsCommunication_->render_frame()->
      GetWebFrame()->GetSecurityOrigin());
  for (const auto& script : js_communication_utils->document_end_scripts_) {
    if (!script->origin_matcher.rules().empty()) {
      if (!script->origin_matcher.Matches(frame_origin)) {
        continue;
      }
    } else {
      if (!js_communication_utils->MatchUrlRegexRules(script->script,
          js_communication_utils->end_scripts_regex_rules_)) {
        continue;
      }
    }

    js_communication_utils->jsCommunication_->render_frame()->GetWebFrame()->ExecuteScript(
        blink::WebScriptSource(script->script));
    // Careful, executing a script may cause JsCommunicationUtils object to be
    // destroyed.
    if (!js_communication_utils) {
      return;
    }
  }

  js_communication_utils->jsCommunication_->render_frame()->OnDocumentEndReady();
}

void JsCommunicationUtils::AddHeadReadyScript(
    mojom::DocumentStartJavaScriptPtr& script_ptr)
{
  DocumentStartJavaScript* script = new DocumentStartJavaScript{
      script_ptr->origin_matcher,
      blink::WebString::FromUTF16(script_ptr->script), script_ptr->script_id};

  head_ready_scripts_.push_back(
      std::unique_ptr<DocumentStartJavaScript>(script));
}

void JsCommunicationUtils::RemoveHeadReadyScript(int32_t script_id)
{
  for (auto it = head_ready_scripts_.begin();
       it != head_ready_scripts_.end(); ++it) {
    if ((*it)->script_id == script_id) {
      head_ready_scripts_.erase(it);
      break;
    }
  }
}

void JsCommunicationUtils::AddHeadReadyScriptRegexRules(
  mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr) {
  auto script_regex_rules = CreateDocumentJavaScriptRegexRules(script_regex_rules_ptr);
  head_ready_regex_rules_.push_back(std::move(script_regex_rules));
}

void JsCommunicationUtils::AddDocumentStartScriptRegexRules(
  mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr) {
  auto script_regex_rules = CreateDocumentJavaScriptRegexRules(script_regex_rules_ptr);
  start_scripts_regex_rules_.push_back(std::move(script_regex_rules));
}

void JsCommunicationUtils::RunScriptsAtHeadReadyInternal() {
  RunScriptsAtHeadReadyInternal(weak_ptr_factory_.GetWeakPtr());
  // Careful 'this' may be destroyed.
}

// static
static void JsCommunicationUtils::RunScriptsAtHeadReadyInternal(
    base::WeakPtr<JsCommunicationUtils> js_communication_utils) {
  CHECK(js_communication_utils);
  url::Origin frame_origin = url::Origin(
      js_communication_utils->jsCommunication_->render_frame()->GetWebFrame()->GetSecurityOrigin());

  for (const auto& script : js_communication_utils->head_ready_scripts_) {
    if (!script->origin_matcher.rules().empty()) {
      if (!script->origin_matcher.Matches(frame_origin)) {
        continue;
      }
    } else {
      if (!js_communication_utils->MatchUrlRegexRules(script->script,
          js_communication_utils->head_ready_regex_rules_)) {
        continue;
      }
    }

    js_communication_utils->jsCommunication_->render_frame()->GetWebFrame()->ExecuteScript(
        blink::WebScriptSource(script->script));
    // Careful, executing a script may cause JsCommunicationUtils object to be
    // destroyed.
    if (!js_communication_utils) {
      return;
    }
  }
}

void JsCommunicationUtils::AddPendingJavascriptAtDocumentEnd(
    mojom::DocumentEndJavaScriptPtr& script_ptr,
    mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr)
{
  DocumentEndJavaScript* script = new DocumentEndJavaScript{
      script_ptr->origin_matcher,
      blink::WebString::FromUTF16(script_ptr->script), script_ptr->script_id};
  swap_document_end_scripts_.push_back(
      std::unique_ptr<DocumentEndJavaScript>(script));

  auto script_regex_rules = CreateDocumentJavaScriptRegexRules(script_regex_rules_ptr);
  swap_end_scripts_regex_rules_.push_back(std::move(script_regex_rules));
}

void JsCommunicationUtils::AddPendingJavascriptAtHeadReady(
    mojom::DocumentStartJavaScriptPtr& script_ptr,
    mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr)
{
  DocumentStartJavaScript* script = new DocumentStartJavaScript{
      script_ptr->origin_matcher,
      blink::WebString::FromUTF16(script_ptr->script), script_ptr->script_id};

  swap_head_ready_scripts_.push_back(
      std::unique_ptr<DocumentStartJavaScript>(script));

  auto script_regex_rules = CreateDocumentJavaScriptRegexRules(script_regex_rules_ptr);
  swap_head_ready_regex_rules_.push_back(std::move(script_regex_rules));
}

void JsCommunicationUtils::CommitPendingJavascriptsAtDocumentEnd()
{
  document_end_scripts_.clear();
  for (auto& item : swap_document_end_scripts_) {
    document_end_scripts_.push_back(std::move(item));
  }
  swap_document_end_scripts_.clear();

  end_scripts_regex_rules_.clear();
  for (auto& regex_item : swap_end_scripts_regex_rules_) {
    end_scripts_regex_rules_.push_back(std::move(regex_item));
  }
  swap_end_scripts_regex_rules_.clear();
}

void JsCommunicationUtils::CommitPendingJavascriptsAtHeadReady()
{
  head_ready_scripts_.clear();
  for (auto& item : swap_head_ready_scripts_) {
    head_ready_scripts_.push_back(std::move(item));
  }
  swap_head_ready_scripts_.clear();

  head_ready_regex_rules_.clear();
  for (auto& regex_item : swap_head_ready_regex_rules_) {
    head_ready_regex_rules_.push_back(std::move(regex_item));
  }
  swap_head_ready_regex_rules_.clear();
}

void JsCommunicationUtils::AddPendingJavascriptAtDocumentStartRegexRules(
    mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr)
{
  auto script_regex_rules = CreateDocumentJavaScriptRegexRules(script_regex_rules_ptr);
  swap_start_scripts_regex_rules_.push_back(std::move(script_regex_rules));
}

void JsCommunicationUtils::CommitPendingJavascriptsAtDocumentStartRegexRules()
{
  start_scripts_regex_rules_.clear();
  for (auto& regex_item : swap_start_scripts_regex_rules_) {
    start_scripts_regex_rules_.push_back(std::move(regex_item));
  }
  swap_start_scripts_regex_rules_.clear();
}

bool JsCommunicationUtils::RunScriptsAtDocumentStartRegexRules(blink::WebString& script)
{
  if (!MatchUrlRegexRules(script, start_scripts_regex_rules_)) {
    return false;
  }

  return true;
}
}  // namespace js_injection
