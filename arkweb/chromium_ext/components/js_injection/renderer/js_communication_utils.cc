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

#include "content/public/renderer/render_frame.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_script_source.h"

namespace js_injection {

JsCommunicationUtils::JsCommunicationUtils(JsCommunication* impl)
    : jsCommunication_(impl) {}

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

void JsCommunicationUtils::RunScriptsAtDocumentEnd()
{
  url::Origin frame_origin = url::Origin(
      jsCommunication_->render_frame()->GetWebFrame()->GetSecurityOrigin());
  for (const auto& script : document_end_scripts_) {
    if (!script->origin_matcher.Matches(frame_origin)) {
      continue;
    }
    jsCommunication_->render_frame()->GetWebFrame()->ExecuteScript(
        blink::WebScriptSource(script->script));
  }
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

void JsCommunicationUtils::RunScriptsAtHeadReady()
{
  url::Origin frame_origin = url::Origin(
      jsCommunication_->render_frame()->GetWebFrame()->GetSecurityOrigin());

  for (const auto& script : head_ready_scripts_) {
    if (!script->origin_matcher.Matches(frame_origin)) {
      continue;
    }

    jsCommunication_->render_frame()->GetWebFrame()->ExecuteScript(
        blink::WebScriptSource(script->script));
  }
}

void JsCommunicationUtils::AddPendingJavascriptAtDocumentEnd(
    mojom::DocumentEndJavaScriptPtr& script_ptr)
{
  DocumentEndJavaScript* script = new DocumentEndJavaScript{
      script_ptr->origin_matcher,
      blink::WebString::FromUTF16(script_ptr->script), script_ptr->script_id};
  swap_document_end_scripts_.push_back(
      std::unique_ptr<DocumentEndJavaScript>(script));
}

void JsCommunicationUtils::AddPendingJavascriptAtHeadReady(
    mojom::DocumentStartJavaScriptPtr& script_ptr)
{
  DocumentStartJavaScript* script = new DocumentStartJavaScript{
      script_ptr->origin_matcher,
      blink::WebString::FromUTF16(script_ptr->script), script_ptr->script_id};

  swap_head_ready_scripts_.push_back(
      std::unique_ptr<DocumentStartJavaScript>(script));
}

void JsCommunicationUtils::CommitPendingJavascriptsAtDocumentEnd()
{
  document_end_scripts_.clear();
  for (auto& item : swap_document_end_scripts_) {
    document_end_scripts_.push_back(std::move(item));
  }
  swap_document_end_scripts_.clear();
}

void JsCommunicationUtils::CommitPendingJavascriptsAtHeadReady()
{
  head_ready_scripts_.clear();
  for (auto& item : swap_head_ready_scripts_) {
    head_ready_scripts_.push_back(std::move(item));
  }
  swap_head_ready_scripts_.clear();
}
}  // namespace js_injection
