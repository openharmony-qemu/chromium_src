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
#include "arkweb/chromium_ext/components/js_injection/js_communication_host_utils.h"
#include "base/functional/bind.h"
#include "base/functional/function_ref.h"
#include "base/strings/utf_string_conversions.h"
#include "components/js_injection/browser/js_to_browser_messaging.h"
#include "components/js_injection/browser/navigation_web_message_sender.h"
#include "components/js_injection/browser/web_message_host.h"
#include "components/js_injection/browser/web_message_host_factory.h"
#include "components/js_injection/common/origin_matcher.h"
#include "components/js_injection/common/origin_matcher_mojom_traits.h"
#include "content/public/browser/back_forward_cache.h"
#include "content/public/browser/page.h"
#include "content/public/browser/web_contents.h"
#include "mojo/public/cpp/bindings/pending_associated_remote.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"

namespace js_injection {

extern std::string ConvertToNativeAllowedOriginRulesWithSanityCheck(
    const std::vector<std::string>& allowed_origin_rules_strings,
    OriginMatcher& allowed_origin_rules);

extern void ForEachRenderFrameHostWithinSameWebContents(
    content::RenderFrameHost* render_frame_host,
    base::FunctionRef<void(content::RenderFrameHost*)> func_ref);

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
DocumentEndJavaScript::DocumentEndJavaScript(std::u16string script,
    OriginMatcher allowed_origin_rules,
    int32_t script_id)
    : script_(std::move(script)),
      allowed_origin_rules_(allowed_origin_rules),
      script_id_(script_id) {}
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
DocumentJavaScriptRegexRules::DocumentJavaScriptRegexRules(const std::u16string& script,
    const std::vector<std::pair<std::string, std::string>>& script_regex_rules,
    int32_t script_id)
    : script_(script),
      script_regex_rules_(script_regex_rules),
      script_id_(script_id) {}

mojom::DocumentJavaScriptRegexRulesPtr JsCommunicationHostUtils::ConvertToMojomScriptRegexRules(
    const DocumentJavaScriptRegexRules* script_regex_rules) {
  auto mojom_script_regex_rules = mojom::DocumentJavaScriptRegexRules::New();
  mojom_script_regex_rules->script_id = script_regex_rules->script_id_;
  mojom_script_regex_rules->script = script_regex_rules->script_;

  for (const auto& rule : script_regex_rules->script_regex_rules_) {
    auto mojom_rule = mojom::ScriptRegexRule::New();
    mojom_rule->second_level_domain = base::UTF8ToUTF16(rule.first);
    mojom_rule->rule = base::UTF8ToUTF16(rule.second);
    mojom_script_regex_rules->script_regex_rules.push_back(std::move(mojom_rule));
  }

  return mojom_script_regex_rules;
}
#endif

JsCommunicationHostUtils::JsCommunicationHostUtils(JsCommunicationHost* js_communication_host)
{
  this->js_communication_host_ = js_communication_host;
}

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
JsCommunicationHost::AddScriptResult JsCommunicationHostUtils::AddDocumentEndJavaScript(
    const std::u16string& script,
    const std::vector<std::string>& allowed_origin_rules)
{
  OriginMatcher origin_matcher;
  std::string error_message = ConvertToNativeAllowedOriginRulesWithSanityCheck(
      allowed_origin_rules, origin_matcher);
  JsCommunicationHost::AddScriptResult result;
  if (!error_message.empty()) {
    result.error_message = std::move(error_message);
    return result;
  }

  document_end_scripts_.emplace_back(script, origin_matcher, js_communication_host_->next_script_id_++);

  ForEachRenderFrameHostWithinSameWebContents(
      js_communication_host_->web_contents()->GetPrimaryMainFrame(),
      [this](content::RenderFrameHost* render_frame_host) {
        NotifyFrameForAddDocumentEndJavaScript(&*document_end_scripts_.rbegin(),
                                               render_frame_host);
      });
  result.script_id = document_end_scripts_.rbegin()->script_id_;
  return result;
}

bool JsCommunicationHostUtils::RemoveDocumentEndJavaScript(int script_id)
{
  for (auto it = document_end_scripts_.begin();
       it != document_end_scripts_.end(); ++it) {
    if (it->script_id_ == script_id) {
      document_end_scripts_.erase(it);
      ForEachRenderFrameHostWithinSameWebContents(
          js_communication_host_->web_contents()->GetPrimaryMainFrame(),
          [this, script_id](content::RenderFrameHost* render_frame_host) {
            NotifyFrameForRemoveDocumentEndJavaScript(script_id,
                                                      render_frame_host);
          });
      return true;
    }
  }
  return false;
}

void JsCommunicationHostUtils::NotifyFrameForAllDocumentEndsJavaScripts(
    content::RenderFrameHost* render_frame_host)
{
  for (const auto& script : document_end_scripts_) {
    NotifyFrameForAddDocumentEndJavaScript(&script, render_frame_host);
  }
  for (const auto& regex_item : end_scripts_regex_rules_) {
    NotifyFrameForAddDocumentEndJavaScriptRegexRules(&regex_item, render_frame_host);
  }
}

void JsCommunicationHostUtils::NotifyFrameForAddDocumentEndJavaScript(
    const DocumentEndJavaScript* script,
    content::RenderFrameHost* render_frame_host)
{
  DCHECK(script);
  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->AddDocumentEndScript(mojom::DocumentEndJavaScript::New(
      script->script_id_, script->script_, script->allowed_origin_rules_));
}

void JsCommunicationHostUtils::NotifyFrameForAddDocumentEndJavaScriptRegexRules(
    const DocumentJavaScriptRegexRules* script_regex_rules,
    content::RenderFrameHost* render_frame_host)
{
  DCHECK(script_regex_rules);
  auto mojom_script_regex_rules = ConvertToMojomScriptRegexRules(script_regex_rules);

  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->AddDocumentEndScriptRegexRules(std::move(mojom_script_regex_rules));
}

void JsCommunicationHostUtils::NotifyFrameForRemoveDocumentEndJavaScript(
    int32_t script_id,
    content::RenderFrameHost* render_frame_host)
{
  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->RemoveDocumentEndScript(script_id);
}
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
JsCommunicationHost::AddScriptResult JsCommunicationHostUtils::AddHeadReadyJavaScript(
    const std::u16string& script,
    const std::vector<std::string>& allowed_origin_rules)
{
  OriginMatcher origin_matcher;
  std::string error_message = ConvertToNativeAllowedOriginRulesWithSanityCheck(
      allowed_origin_rules, origin_matcher);
  JsCommunicationHost::AddScriptResult result;
  if (!error_message.empty()) {
    result.error_message = std::move(error_message);
    return result;
  }

  head_ready_scripts_.emplace_back(script, origin_matcher, js_communication_host_->next_script_id_++);

  ForEachRenderFrameHostWithinSameWebContents(
      js_communication_host_->web_contents()->GetPrimaryMainFrame(),
      [this](content::RenderFrameHost* render_frame_host) {
        NotifyFrameForAddHeadReadyJavaScript(&*head_ready_scripts_.rbegin(),
                                             render_frame_host);
      });
  result.script_id = head_ready_scripts_.rbegin()->script_id_;
  return result;
}

bool JsCommunicationHostUtils::RemoveHeadReadyJavaScript(int script_id)
{
  for (auto it = head_ready_scripts_.begin(); it != head_ready_scripts_.end();
       ++it) {
    if (it->script_id_ == script_id) {
      head_ready_scripts_.erase(it);
      ForEachRenderFrameHostWithinSameWebContents(
          js_communication_host_->web_contents()->GetPrimaryMainFrame(),
          [this, script_id](content::RenderFrameHost* render_frame_host) {
            NotifyFrameForRemoveHeadReadyJavaScript(script_id,
                                                    render_frame_host);
          });
      return true;
    }
  }
  return false;
}

void JsCommunicationHostUtils::NotifyFrameForAddHeadReadyJavaScript(
    const DocumentStartJavaScript* script,
    content::RenderFrameHost* render_frame_host)
{
  DCHECK(script);
  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->AddHeadReadyScript(mojom::DocumentStartJavaScript::New(
      script->script_id_, script->script_, script->allowed_origin_rules_));
}

void JsCommunicationHostUtils::NotifyFrameForRemoveHeadReadyJavaScript(
    int32_t script_id,
    content::RenderFrameHost* render_frame_host)
{
  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->RemoveHeadReadyScript(script_id);
}

void JsCommunicationHostUtils::NotifyFrameForAddHeadReadyJavaScriptRegexRules(
    const DocumentJavaScriptRegexRules* script_regex_rules,
    content::RenderFrameHost* render_frame_host)
{
  DCHECK(script_regex_rules);
  auto mojom_script_regex_rules = ConvertToMojomScriptRegexRules(script_regex_rules);

  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->AddHeadReadyScriptRegexRules(std::move(mojom_script_regex_rules));
}

JsCommunicationHost::AddScriptResult JsCommunicationHostUtils::AddHeadReadyPendingJavaScript(
    const std::u16string& script,
    const std::vector<std::string>& allowed_origin_rules,
    const std::vector<std::pair<std::string, std::string>>& script_regex_rules)
{
  OriginMatcher origin_matcher;
  JsCommunicationHost::AddScriptResult result;
  if (!allowed_origin_rules.empty() && (allowed_origin_rules.size() > 1 || !allowed_origin_rules[0].empty())) {
    std::string error_message = ConvertToNativeAllowedOriginRulesWithSanityCheck(
        allowed_origin_rules, origin_matcher);
    if (!error_message.empty()) {
      result.error_message = std::move(error_message);
      return result;
    }
  }

  swap_head_ready_regex_rules_.emplace_back(script, script_regex_rules, js_communication_host_->next_script_id_);
  swap_head_ready_scripts_.emplace_back(script, origin_matcher, js_communication_host_->next_script_id_++);

  ForEachRenderFrameHostWithinSameWebContents(
      js_communication_host_->web_contents()->GetPrimaryMainFrame(),
      [this](content::RenderFrameHost* render_frame_host) {
        NotifyFrameForAddHeadReadyPendingJavaScript(&*swap_head_ready_scripts_.rbegin(),
                                                    &*swap_head_ready_regex_rules_.rbegin(),
                                                    render_frame_host);
      });
  result.script_id = swap_head_ready_scripts_.rbegin()->script_id_;
  return result;
}

void JsCommunicationHostUtils::NotifyFrameForAddHeadReadyPendingJavaScript(
    const DocumentStartJavaScript* script,
    const DocumentJavaScriptRegexRules* script_regex_rules,
    content::RenderFrameHost* render_frame_host)
{
  DCHECK(script);
  auto mojom_script_regex_rules = ConvertToMojomScriptRegexRules(script_regex_rules);

  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->AddPendingJavascriptAtHeadReady(mojom::DocumentStartJavaScript::New(
      script->script_id_, script->script_, script->allowed_origin_rules_), std::move(mojom_script_regex_rules));
}

void JsCommunicationHostUtils::CommitPendingJavascriptsAtHeadReady()
{
  head_ready_scripts_.clear();
  for (auto& item : swap_head_ready_scripts_) {
    head_ready_scripts_.emplace_back(item.script_, item.allowed_origin_rules_, item.script_id_);
  }
  swap_head_ready_scripts_.clear();

  head_ready_regex_rules_.clear();
  for (auto& regex_item : swap_head_ready_regex_rules_) {
    head_ready_regex_rules_.emplace_back(
      regex_item.script_, regex_item.script_regex_rules_, regex_item.script_id_);
  }
  swap_head_ready_regex_rules_.clear();

  ForEachRenderFrameHostWithinSameWebContents(
      js_communication_host_->web_contents()->GetPrimaryMainFrame(),
      [this](content::RenderFrameHost* render_frame_host) {
        NotifyFrameForCommitForHeadReady(render_frame_host);
      });
}

void JsCommunicationHostUtils::NotifyFrameForCommitForHeadReady(content::RenderFrameHost* render_frame_host)
{
  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->CommitPendingJavascriptsAtHeadReady();
}

JsCommunicationHost::AddScriptResult JsCommunicationHostUtils::AddDocumentEndPendingJavaScript(
    const std::u16string& script,
    const std::vector<std::string>& allowed_origin_rules,
    const std::vector<std::pair<std::string, std::string>>& script_regex_rules)
{
  OriginMatcher origin_matcher;
  JsCommunicationHost::AddScriptResult result;
  if (!allowed_origin_rules.empty() && (allowed_origin_rules.size() > 1 || !allowed_origin_rules[0].empty())) {
    std::string error_message = ConvertToNativeAllowedOriginRulesWithSanityCheck(
        allowed_origin_rules, origin_matcher);
    if (!error_message.empty()) {
      result.error_message = std::move(error_message);
      return result;
    }
  }
  swap_end_scripts_regex_rules_.emplace_back(script, script_regex_rules, js_communication_host_->next_script_id_);
  swap_document_end_scripts_.emplace_back(script, origin_matcher, js_communication_host_->next_script_id_++);

  ForEachRenderFrameHostWithinSameWebContents(
      js_communication_host_->web_contents()->GetPrimaryMainFrame(),
      [this](content::RenderFrameHost* render_frame_host) {
        NotifyFrameForAddDocumentEndPendingJavaScript(&*swap_document_end_scripts_.rbegin(),
                                                      &*swap_end_scripts_regex_rules_.rbegin(),
                                                      render_frame_host);
      });
  result.script_id = swap_document_end_scripts_.rbegin()->script_id_;
  return result;
}

void JsCommunicationHostUtils::NotifyFrameForAddDocumentEndPendingJavaScript(
    const DocumentEndJavaScript* script,
    const DocumentJavaScriptRegexRules* script_regex_rules,
    content::RenderFrameHost* render_frame_host)
{
  DCHECK(script);
  auto mojom_script_regex_rules = ConvertToMojomScriptRegexRules(script_regex_rules);

  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->AddPendingJavascriptAtDocumentEnd(mojom::DocumentEndJavaScript::New(
      script->script_id_, script->script_, script->allowed_origin_rules_), std::move(mojom_script_regex_rules));
}

void JsCommunicationHostUtils::CommitPendingJavascriptsAtDocumentEnd()
{
  document_end_scripts_.clear();
  for (auto& item : swap_document_end_scripts_) {
    document_end_scripts_.emplace_back(item.script_, item.allowed_origin_rules_, item.script_id_);
  }
  swap_document_end_scripts_.clear();

  end_scripts_regex_rules_.clear();
  for (auto& regex_item : swap_end_scripts_regex_rules_) {
    end_scripts_regex_rules_.emplace_back(
      regex_item.script_, regex_item.script_regex_rules_, regex_item.script_id_);
  }
  swap_end_scripts_regex_rules_.clear();

  ForEachRenderFrameHostWithinSameWebContents(
      js_communication_host_->web_contents()->GetPrimaryMainFrame(),
      [this](content::RenderFrameHost* render_frame_host) {
        NotifyFrameForCommitForDocumentEnd(render_frame_host);
      });
}

void JsCommunicationHostUtils::NotifyFrameForCommitForDocumentEnd(content::RenderFrameHost* render_frame_host)
{
  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->CommitPendingJavascriptsAtDocumentEnd();
}

void JsCommunicationHostUtils::NotifyFrameForAddDocumentStartJavaScriptRegexRules(
    const DocumentJavaScriptRegexRules* script_regex_rules,
    content::RenderFrameHost* render_frame_host)
{
  DCHECK(script_regex_rules);
  auto mojom_script_regex_rules = ConvertToMojomScriptRegexRules(script_regex_rules);

  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->AddDocumentStartScriptRegexRules(std::move(mojom_script_regex_rules));
}

JsCommunicationHost::AddScriptResult JsCommunicationHostUtils::AddDocumentStartPendingJavaScript(
    const std::u16string& script,
    const std::vector<std::string>& allowed_origin_rules,
    const std::vector<std::pair<std::string, std::string>>& script_regex_rules)
{
  OriginMatcher origin_matcher;
  JsCommunicationHost::AddScriptResult result;
  if (!allowed_origin_rules.empty() && (allowed_origin_rules.size() > 1 || !allowed_origin_rules[0].empty())) {
    std::string error_message = ConvertToNativeAllowedOriginRulesWithSanityCheck(
        allowed_origin_rules, origin_matcher);
    if (!error_message.empty()) {
      result.error_message = std::move(error_message);
      return result;
    }
  }
  swap_start_scripts_regex_rules_.emplace_back(script, script_regex_rules, js_communication_host_->next_script_id_);
  swap_document_start_scripts_.emplace_back(script, origin_matcher, js_communication_host_->next_script_id_++);

  ForEachRenderFrameHostWithinSameWebContents(
      js_communication_host_->web_contents()->GetPrimaryMainFrame(),
      [this](content::RenderFrameHost* render_frame_host) {
        NotifyFrameForAddDocumentStartPendingJavaScript(&*swap_document_start_scripts_.rbegin(),
                                                        &*swap_start_scripts_regex_rules_.rbegin(),
                                                        render_frame_host);
      });
  result.script_id = swap_document_start_scripts_.rbegin()->script_id_;
  return result;
}

void JsCommunicationHostUtils::NotifyFrameForAddDocumentStartPendingJavaScript(
    const DocumentStartJavaScript* script,
    const DocumentJavaScriptRegexRules* script_regex_rules,
    content::RenderFrameHost* render_frame_host)
{
  DCHECK(script);
  auto mojom_script_regex_rules = ConvertToMojomScriptRegexRules(script_regex_rules);

  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->AddPendingJavascriptAtDocumentStart(mojom::DocumentStartJavaScript::New(
      script->script_id_, script->script_, script->allowed_origin_rules_), std::move(mojom_script_regex_rules));
}

void JsCommunicationHostUtils::CommitPendingJavascriptsAtDocumentStart()
{
  js_communication_host_->scripts_.clear();
  for (auto& item : swap_document_start_scripts_) {
    js_communication_host_->scripts_.emplace_back(item.script_, item.allowed_origin_rules_, item.script_id_);
  }
  swap_document_start_scripts_.clear();

  start_scripts_regex_rules_.clear();
  for (auto& regex_item : swap_start_scripts_regex_rules_) {
    start_scripts_regex_rules_.emplace_back(
      regex_item.script_, regex_item.script_regex_rules_, regex_item.script_id_);
  }
  swap_start_scripts_regex_rules_.clear();

  ForEachRenderFrameHostWithinSameWebContents(
      js_communication_host_->web_contents()->GetPrimaryMainFrame(),
      [this](content::RenderFrameHost* render_frame_host) {
        NotifyFrameForCommitForDocumentStart(render_frame_host);
      });
}

void JsCommunicationHostUtils::NotifyFrameForCommitForDocumentStart(content::RenderFrameHost* render_frame_host)
{
  mojo::AssociatedRemote<mojom::JsCommunication> configurator_remote;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      &configurator_remote);
  configurator_remote->CommitPendingJavascriptsAtDocumentStart();
}
#endif
}
