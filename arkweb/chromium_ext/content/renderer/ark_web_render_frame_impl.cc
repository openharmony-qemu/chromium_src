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
#include "third_party/blink/public/web/web_settings.h"
#include "third_party/blink/public/web/web_view.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "third_party/blink/public/web/web_performance_metrics_for_reporting.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing_detector.h"
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "url/ohos/log_utils.h"
#endif

using blink::WebDocumentLoader;

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

// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_ADBLOCK)
// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
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

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void RenderFrameImpl::NotifyLcpForBlankless() {
  if (blankless_key_ == base::ohos::BlanklessController::INVALID_BLANKLESS_KEY) {
    return;
  }
  if (RenderThreadImpl* render_thread = RenderThreadImpl::current()) {
    const blink::WebPerformanceMetricsForReporting& perf = GetWebFrame()->PerformanceMetricsForReporting();
    double start = perf.NavigationStart();
    blink::LargestContentfulPaintDetailsForReporting lcp_details = perf.LargestContentfulDetailsForMetrics();
    double ms = (std::max(lcp_details.image_paint_time, lcp_details.text_paint_time) - start) * 1000;
    int32_t lcp_time = ms > INT32_MAX ? INT32_MAX : static_cast<int32_t>(ms);
    render_thread->SetBlanklessDumpInfo(nweb_id_, blankless_key_, frame_sink_id_, lcp_time, pref_hash_);
  }
}

void RenderFrameImpl::SendBlanklessKeyToRenderFrame(uint32_t nweb_id,
                                                    uint64_t blankless_key,
                                                    uint64_t frame_sink_id,
                                                    int64_t pref_hash)
{
  nweb_id_ = nweb_id;
  blankless_key_ = blankless_key;
  frame_sink_id_ = frame_sink_id;
  pref_hash_ = pref_hash;
  if (blankless_key == base::ohos::BlanklessController::INVALID_BLANKLESS_KEY) {
    return;
  }
  // need restart painttimingdetector for blankless
  blink::WebLocalFrameImpl* web_frame = static_cast<blink::WebLocalFrameImpl*>(GetWebFrame());
  if (!web_frame) {
    return;
  }

  blink::LocalFrame* local_frame = web_frame->GetFrame();
  if (!local_frame) {
    return;
  }

  blink::LocalFrameView* lfv = local_frame->View();
  if (!lfv) {
    return;
  }
  LOG(DEBUG) << "blankless lcp:RenderFrameImpl::SendBlanklessKeyToRenderFrame restart PTD for blankless, nweb_id:"
    << nweb_id << ", key:" << blankless_key << "lfv:" << (uint64_t)lfv;
    lfv->GetPaintTimingDetector().RestartRecordingForBlankless();
}
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
bool RenderFrameImpl::IsStrictLogMode() {
  if (GetWebView()) {
    return GetWebView()->IsStrictLogMode();
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT) && !BUILDFLAG(ARKWEB_NWEB_EX)
void RenderFrameImpl::OnCommitNavigation(
    const GURL& url,
    bool is_client_redirect,
    blink::mojom::NavigationType navigation_type,
    const base::UnguessableToken& devtools_navigation_token,
    network::mojom::NavigationDeliveryType navigation_delivery_type) {}

void RenderFrameImpl::OnDidCommitNavigation(
    WebDocumentLoader* document_loader,
    blink::WebHistoryCommitType commit_type) {}

void RenderFrameImpl::OnDidDispatchDOMContentLoadedEvent() {}

void RenderFrameImpl::OnDidHandleOnloadEvents() {}
#endif  // BUILDFLAG(ARKWEB_LOGGER_REPORT) && !BUILDFLAG(ARKWEB_NWEB_EX)
// LCOV_EXCL_STOP

}  // namespace content
