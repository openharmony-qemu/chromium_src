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

#ifndef COMPONENTS_JS_INJECTION_RENDERER_JS_COMMUNICATION_UTILS_H
#define COMPONENTS_JS_INJECTION_RENDERER_JS_COMMUNICATION_UTILS_H

#include "components/js_injection/renderer/js_communication.h"

typedef int int32_t;

namespace js_injection {
class JsCommunication;

class JsCommunicationUtils {
 public:
  struct ScriptRegexRule {
    std::string second_level_domain;
    std::string rule;
  };
  struct DocumentJavaScriptRegexRules {
    int32_t script_id;
    blink::WebString script;
    std::vector<ScriptRegexRule> script_regex_rules;
  };
  JsCommunicationUtils(JsCommunication* impl);
  void AddDocumentEndScript(mojom::DocumentEndJavaScriptPtr& script_ptr);
  void RemoveDocumentEndScript(int32_t script_id);
  void AddDocumentEndScriptRegexRules(mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr);
  void RunScriptsAtDocumentEnd();

  static void RunScriptsAtDocumentEndInternal(
      base::WeakPtr<JsCommunicationUtils> js_communication_utils);
  void AddHeadReadyScript(mojom::DocumentStartJavaScriptPtr& script_ptr);
  void RemoveHeadReadyScript(int32_t script_id);
  void RunScriptsAtHeadReady();

  static void RunScriptsAtHeadReadyInternal(
      base::WeakPtr<JsCommunicationUtils> js_communication_utils);
  void AddHeadReadyScriptRegexRules(mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr);
  void AddDocumentStartScriptRegexRules(mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr);

  void AddPendingJavascriptAtDocumentEnd(
      mojom::DocumentEndJavaScriptPtr& script_ptr,
      mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr);
  void AddPendingJavascriptAtHeadReady(
      mojom::DocumentStartJavaScriptPtr& script_ptr,
      mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr);
  void CommitPendingJavascriptsAtDocumentEnd();
  void CommitPendingJavascriptsAtHeadReady();
  void AddPendingJavascriptAtDocumentStartRegexRules(
    mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr);
  void CommitPendingJavascriptsAtDocumentStartRegexRules();
  bool RunScriptsAtDocumentStartRegexRules(blink::WebString& script);

  bool MatchUrlRegexRules(blink::WebString& script,
    std::vector<std::unique_ptr<DocumentJavaScriptRegexRules>>& scripts_regex_rules);
  std::unique_ptr<JsCommunicationUtils::DocumentJavaScriptRegexRules> CreateDocumentJavaScriptRegexRules(
    const mojom::DocumentJavaScriptRegexRulesPtr& script_regex_rules_ptr);
 private:
  struct DocumentStartJavaScript {
    OriginMatcher origin_matcher;
    blink::WebString script;
    int32_t script_id;
  };
  struct DocumentEndJavaScript {
    OriginMatcher origin_matcher;
    blink::WebString script;
    int32_t script_id;
  };
  raw_ptr<JsCommunication> jsCommunication_;
  std::vector<std::unique_ptr<DocumentEndJavaScript>> document_end_scripts_;
  std::vector<std::unique_ptr<DocumentStartJavaScript>> head_ready_scripts_;
  std::vector<std::unique_ptr<DocumentEndJavaScript>> swap_document_end_scripts_;
  std::vector<std::unique_ptr<DocumentStartJavaScript>> swap_head_ready_scripts_;
  std::vector<std::unique_ptr<DocumentJavaScriptRegexRules>> swap_end_scripts_regex_rules_;
  std::vector<std::unique_ptr<DocumentJavaScriptRegexRules>> end_scripts_regex_rules_;
  std::vector<std::unique_ptr<DocumentJavaScriptRegexRules>> swap_head_ready_regex_rules_;
  std::vector<std::unique_ptr<DocumentJavaScriptRegexRules>> head_ready_regex_rules_;
  std::vector<std::unique_ptr<DocumentJavaScriptRegexRules>> swap_start_scripts_regex_rules_;
  std::vector<std::unique_ptr<DocumentJavaScriptRegexRules>> start_scripts_regex_rules_;

  base::WeakPtrFactory<JsCommunication> weak_ptr_factory_{this}
};

}  // namespace js_injection

#endif  // COMPONENTS_JS_INJECTION_RENDERER_JS_COMMUNICATION_UTILS_H
