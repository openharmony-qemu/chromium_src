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

#include "arkweb/chromium_ext/content/renderer/ark_web_render_frame_impl.h"
#include "content/child/child_process.h"

#include "arkweb/build/features/features.h"
#include "content/public/common/content_client.h"
#include "content/public/renderer/content_renderer_client.h"
#include "content/public/renderer/render_frame_observer.h"
#include "content/renderer/render_frame_impl.h"
#include "third_party/blink/public/web/web_document_loader.h"
#include "third_party/blink/public/web/web_navigation_control.h"
#include "third_party/blink/public/web/web_view.h"

namespace content {

std::optional<blink::WebString> ArkWebUserAgentOverride(
    content::RenderFrame* render_frame) {
  if (render_frame &&
      render_frame->GetWebView()->MainFrame()->IsWebRemoteFrame()) {
    return blink::WebString::FromUTF8(
        render_frame->GetWebView()
            ->GetRendererPreferences()
            .user_agent_override.ua_string_override);
  }
  return std::nullopt;
}

#if BUILDFLAG(ARKWEB_JSPROXY)
void RenderFrameImpl::RunScriptsAtHeadReady() {
  if (!initialized_) {
    return;
  }

  if (GetContentClient() && GetContentClient()->renderer()) {
    GetContentClient()->renderer()->RunScriptsAtHeadReady(this);
  }
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void RenderFrameImpl::DidSubresourceFiltered() {
  TRACE_EVENT1("navigation,benchmark,rail",
               "RenderFrameImpl::DidSubresourceFiltered", "frame_token",
               frame_token_);
  for (auto& observer : observers_) {
    observer.DidSubresourceFiltered();
  }
}

bool RenderFrameImpl::GetGlobalAdblockEnabled() {
  return GetRendererPreferences().is_global_adblock_enabled;
}

void RenderFrameImpl::OnUpdateAdBlockEnabledToRender(
    bool site_adblock_enabled) {
  // send switch to render by RenderViewImpl
  site_adblock_enabled = true;
  if (GetWebView()) {
    LOG(INFO) << "[Adblock] render frame update adblock:"
              << site_adblock_enabled;
    GetWebView()->OnSetAdBlockEnable(site_adblock_enabled);
  }

  if (!frame_) {
    return;
  }
  blink::WebDocumentLoader* document_loader = frame_->GetDocumentLoader();
  if (!document_loader) {
    return;
  }
  blink::WebDocumentSubresourceFilter* filter =
      document_loader->GetWebSubresourceFilter();

  blink::WebDocumentSubresourceFilter* user_filter =
      document_loader->GetWebUserSubresourceFilter();
  if (filter) {
    filter->set_activation_state(site_adblock_enabled);
    return;
  }

  if (user_filter) {
    user_filter->set_activation_state(site_adblock_enabled);
  }
}
#endif

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
void RenderFrameImpl::AddNamedObject(const std::string& name,
                                     int32_t object_id,
                                     base::Value::List async_method_list,
                                     bool need_update) {
  for (auto& observer : observers_) {
    observer.AddNamedObject(name, object_id, async_method_list, need_update);
  }
}
#endif  // BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)

RenderFrameImplUtils::RenderFrameImplUtils(RenderFrameImpl* impl) {
  this->renderFrameImpl = impl;
}
#if BUILDFLAG(ARKWEB_DFX_TRACING)
int64_t RenderFrameImplUtils::GetCurrentTimestampMS() {
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(currentTime)
              .count() /
          kMicrosecondsPerMillisecond;
}

void RenderFrameImplUtils::ReportRenderInitBlock() {
  int64_t initialize_time = GetCurrentTimestampMS();
  std::string mode = "ReportRenderInitBlock";
  if (is_complete_initialize) {
    is_complete_initialize = false;
    int64_t block_time = initialize_time - commit_navigation_time_;
    if(ChildProcess::current()) {
      ChildProcess::current()->ReportHisyevent(block_time, mode);
    }
  }
}

void RenderFrameImplUtils::ChangeCommitNavigationTime(int64_t time) {
  commit_navigation_time_ = time;
}

void RenderFrameImplUtils::ChangeCompleteInitialize(bool complete) {
  is_complete_initialize = complete;
}

void RenderFrameImpl::SendCommitNavigationTime(int64_t start_time) {
  implUtils->ChangeCommitNavigationTime(start_time);
  implUtils->ChangeCompleteInitialize(true);
}
#endif

}  // namespace content
