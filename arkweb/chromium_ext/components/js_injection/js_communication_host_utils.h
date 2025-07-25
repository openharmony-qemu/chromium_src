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
#ifndef COMPONENTS_JS_INJECTION_BROWSER_JS_COMMUNICATION_HOST_UTILS_H_
#define COMPONENTS_JS_INJECTION_BROWSER_JS_COMMUNICATION_HOST_UTILS_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "components/js_injection/browser/js_communication_host.h"
#include "components/js_injection/common/interfaces.mojom.h"
#include "content/public/browser/global_routing_id.h"
#include "content/public/browser/web_contents_observer.h"

namespace js_injection {

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
struct DocumentEndJavaScript {
  DocumentEndJavaScript(std::u16string script,
                        OriginMatcher allowed_origin_rules,
                        int32_t script_id);

  DocumentEndJavaScript(DocumentEndJavaScript&) = delete;
  DocumentEndJavaScript& operator=(DocumentEndJavaScript&) = delete;
  DocumentEndJavaScript(DocumentEndJavaScript&&) = default;
  DocumentEndJavaScript& operator=(DocumentEndJavaScript&&) = default;

  std::u16string script_;
  OriginMatcher allowed_origin_rules_;
  int32_t script_id_;
};
#endif

class JsCommunicationHost;

class JsCommunicationHostUtils {
  public:
    JsCommunicationHostUtils(JsCommunicationHost* js_communication_host);
    raw_ptr<JsCommunicationHost> js_communication_host_;

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  // Native side AddDocumentEndJavaScript, returns an error message if the
  // parameters didn't pass necessary checks.
  JsCommunicationHost::AddScriptResult AddDocumentEndJavaScript(
      const std::u16string& script,
      const std::vector<std::string>& allowed_origin_rules);
  JsCommunicationHost::AddScriptResult AddDocumentEndPendingJavaScript(
      const std::u16string& script,
      const std::vector<std::string>& allowed_origin_rules);
  bool RemoveDocumentEndJavaScript(int script_id);
  void NotifyFrameForAllDocumentEndsJavaScripts(
      content::RenderFrameHost* render_frame_host);
  void NotifyFrameForAddDocumentEndJavaScript(
      const DocumentEndJavaScript* script,
      content::RenderFrameHost* render_frame_host);
  void NotifyFrameForRemoveDocumentEndJavaScript(
      int32_t script_id,
      content::RenderFrameHost* render_frame_host);
  void NotifyFrameForAddDocumentEndPendingJavaScript(
      const DocumentEndJavaScript* script,
      content::RenderFrameHost* render_frame_host);
  void CommitPendingJavascriptsAtDocumentEnd();

  std::vector<DocumentEndJavaScript> document_end_scripts_;
  std::vector<DocumentEndJavaScript> swap_document_end_scripts_;
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  // Native side AddHeadReadyJavaScript, returns an error message if the
  // parameters didn't pass necessary checks.
  JsCommunicationHost::AddScriptResult AddHeadReadyJavaScript(
      const std::u16string& script,
      const std::vector<std::string>& allowed_origin_rules);
  JsCommunicationHost::AddScriptResult AddHeadReadyPendingJavaScript(
      const std::u16string& script,
      const std::vector<std::string>& allowed_origin_rules);

  bool RemoveHeadReadyJavaScript(int script_id);
  void NotifyFrameForAddHeadReadyJavaScript(
      const DocumentStartJavaScript* script,
      content::RenderFrameHost* render_frame_host);
  void CommitPendingJavascriptsAtHeadReady();

  std::vector<DocumentStartJavaScript> head_ready_scripts_;
  std::vector<DocumentStartJavaScript> swap_head_ready_scripts_;
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  JsCommunicationHost::AddScriptResult AddDocumentStartPendingJavaScript(
      const std::u16string& script,
      const std::vector<std::string>& allowed_origin_rules);
  void CommitPendingJavascriptsAtDocumentStart();

  std::vector<DocumentStartJavaScript> swap_document_start_scripts_;
#endif

private:
#if BUILDFLAG(ARKWEB_JSPROXY)
  void NotifyFrameForCommitForDocumentEnd(content::RenderFrameHost* render_frame_host);
  void NotifyFrameForCommitForHeadReady(content::RenderFrameHost* render_frame_host);
  void NotifyFrameForAddHeadReadyPendingJavaScript(
      const DocumentStartJavaScript* script,
      content::RenderFrameHost* render_frame_host);
  void NotifyFrameForRemoveHeadReadyJavaScript(
      int32_t script_id,
      content::RenderFrameHost* render_frame_host);
  void NotifyFrameForAddDocumentStartPendingJavaScript(
    const DocumentStartJavaScript* script,
    content::RenderFrameHost* render_frame_host);
  void NotifyFrameForCommitForDocumentStart(content::RenderFrameHost* render_frame_host);
#endif
};

}

#endif