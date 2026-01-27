// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

/*
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved.
 *     (http://www.torchmobile.com/)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "content/browser/renderer_host/navigation_controller_impl.h"
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_navigation_controller_impl_ext.h"

#include <algorithm>
#include <utility>

#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "base/command_line.h"
#include "base/containers/adapters.h"
#include "base/debug/dump_without_crashing.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/numerics/safe_conversions.h"
#include "base/strings/escape.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "base/trace_event/optional_trace_event.h"
#include "base/trace_event/trace_event.h"
#include "base/types/optional_util.h"
#include "build/build_config.h"
#include "cc/base/switches.h"
#include "content/browser/bad_message.h"
#include "content/browser/blob_storage/chrome_blob_storage_context.h"
#include "content/browser/browser_context_impl.h"
#include "content/browser/browser_url_handler_impl.h"
#include "content/browser/dom_storage/dom_storage_context_wrapper.h"
#include "content/browser/dom_storage/session_storage_namespace_impl.h"
#include "content/browser/preloading/prerender/prerender_host.h"
#include "content/browser/process_lock.h"
#include "content/browser/renderer_host/back_forward_cache_impl.h"
#include "content/browser/renderer_host/debug_urls.h"
#include "content/browser/renderer_host/frame_tree.h"
#include "content/browser/renderer_host/frame_tree_node.h"
#include "content/browser/renderer_host/navigation_controller_delegate.h"
#include "content/browser/renderer_host/navigation_entry_impl.h"
#include "content/browser/renderer_host/navigation_entry_restore_context_impl.h"
#include "content/browser/renderer_host/navigation_request.h"
#include "content/browser/renderer_host/navigation_transitions/navigation_entry_screenshot_cache.h"
#include "content/browser/renderer_host/navigation_transitions/navigation_entry_screenshot_manager.h"
#include "content/browser/renderer_host/navigation_transitions/navigation_transition_config.h"
#include "content/browser/renderer_host/navigator.h"
#include "content/browser/renderer_host/page_delegate.h"
#include "content/browser/renderer_host/render_frame_host_delegate.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/browser/renderer_host/system_entropy_utils.h"
#include "content/browser/site_info.h"
#include "content/browser/site_instance_impl.h"
#include "content/common/content_constants_internal.h"
#include "content/common/content_navigation_policy.h"
#include "content/common/navigation_params_utils.h"
#include "content/common/trace_utils.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/disallow_activation_reason.h"
#include "content/public/browser/invalidate_type.h"
#include "content/public/browser/navigation_details.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/replaced_navigation_entry_data.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_constants.h"
#include "content/public/common/content_features.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/url_utils.h"
#include "media/base/mime_util.h"
#include "net/base/net_errors.h"
#include "net/http/http_status_code.h"
#include "services/metrics/public/cpp/ukm_builders.h"
#include "services/metrics/public/cpp/ukm_recorder.h"
#include "services/metrics/public/cpp/ukm_source_id.h"
#include "services/network/public/mojom/fetch_api.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom-shared.h"
#include "skia/ext/platform_canvas.h"
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
#include "third_party/abseil-cpp/absl/types/optional.h"
#endif
#include "third_party/blink/public/common/blob/blob_utils.h"
#include "third_party/blink/public/common/chrome_debug_urls.h"
#include "third_party/blink/public/common/history/session_history_constants.h"
#include "third_party/blink/public/common/mime_util/mime_util.h"
#include "third_party/blink/public/common/navigation/navigation_params.h"
#include "third_party/blink/public/common/page_state/page_state_serialization.h"
#include "third_party/blink/public/mojom/navigation/navigation_params.mojom.h"
#include "third_party/blink/public/mojom/navigation/prefetched_signed_exchange_info.mojom.h"
#include "third_party/blink/public/mojom/runtime_feature_state/runtime_feature.mojom.h"
#include "url/url_constants.h"

#if BUILDFLAG(ARKWEB_NETWORK_DFX)
#include "cef/ohos_cef_ext/libcef/browser/page_load_metrics/arkweb_page_load_metrics_observer.h"
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "url/ohos/log_utils.h"
#endif

namespace content {

ArkWebNavigationControllerImplExt::ArkWebNavigationControllerImplExt(
    BrowserContext* browser_context,
    FrameTree& frame_tree,
    NavigationControllerDelegate* delegate)
    : NavigationControllerImpl(browser_context,
                               frame_tree,
                               delegate) {}

ArkWebNavigationControllerImplExt::~ArkWebNavigationControllerImplExt() = default;

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void ArkWebNavigationControllerImplExt::NewEntrySetExtraHeaders(
    RenderFrameHostImpl* rfh,
    const mojom::DidCommitProvisionalLoadParams& params,
    std::unique_ptr<NavigationEntryImpl>& new_entry) {
  bool is_currently_error_page = rfh->IsErrorDocument();
  if (is_currently_error_page &&
      base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx)) {
    new_entry->set_extra_headers(params.headers);
  }
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
const std::string& ArkWebNavigationControllerImplExt::GetOriginalUrl() {
  int cur_index = GetCurrentEntryIndex();
  int count = GetEntryCount();
  if (cur_index >= 0 && cur_index < count) {
    NavigationEntryImpl* entry = GetEntryAtIndex(cur_index);
    if (entry) {
      return entry->GetOriginalRequestURL().spec();
    }
  }
  return base::EmptyString();
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
NavigationController::NavigationEntryUpdateError
ArkWebNavigationControllerImplExt::InsertBackForwardEntry(int index, const GURL& url) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO, kNavigation)
      << "InsertBackForwardEntry url:"
      << url::LogUtils::ConvertUrlWithMask(url.spec()) << " index:" << index;
#endif
  if (index < 0 || static_cast<size_t>(index) > entries_.size()) {
    return NavigationEntryUpdateError::ERR_WRONG_OFFSET;
  }
  if (GetLastCommittedEntry()->IsInitialEntry() && entries_.size() > 0) {
    entries_.clear();
  }
  std::unique_ptr<NavigationEntryImpl> entry =
      NavigationEntryImpl::FromNavigationEntry(
          NavigationController::CreateNavigationEntry(
              url, Referrer(), absl::nullopt, absl::nullopt,
              ui::PAGE_TRANSITION_FORWARD_BACK, false, std::string(),
              browser_context_, nullptr
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
              , nullptr, delegate_
#endif
              ));
  std::unique_ptr<content::NavigationEntryRestoreContext> context =
      content::NavigationEntryRestoreContext::Create();
  entry->SetPageState(blink::PageState::CreateFromURL(entry->GetURL()),
                      context.get());
  if (entries_.size() == 0) {
    InsertOrReplaceEntry(std::move(entry), false, false, false, nullptr);
    return NavigationEntryUpdateError::UPDATE_OK;
  }

  entries_.insert(entries_.begin() + index, std::move(entry));
  if (index <= last_committed_entry_index_) {
    if (pending_entry_ && pending_entry_index_ != -1) {
      pending_entry_index_++;
    }
    last_committed_entry_index_++;
  }

  return NavigationEntryUpdateError::UPDATE_OK;
}

NavigationController::NavigationEntryUpdateError
ArkWebNavigationControllerImplExt::UpdateNavigationEntryUrl(int index, const GURL& url) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO, kNavigation)
      << "UpdateNavigationEntryUrl index:" << index
      << " url:" << url::LogUtils::ConvertUrlWithMask(url.spec());
#endif
  if (frame_tree_->IsLoadingIncludingInnerFrameTrees()) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR, kNavigation)
        << "UpdateNavigationEntryUrl message:failedDueLoading";
#endif
    return NavigationEntryUpdateError::ERR_OTHER;
  }

  NavigationEntryImpl* entry = GetEntryAtIndex(index);
  if (!entry) {
    return NavigationEntryUpdateError::ERR_WRONG_OFFSET;
  }
  GURL new_url = GURL(url);
  entry->SetURL(new_url);
  entry->SetVirtualURL(new_url);
  entry->root_node()->frame_entry->set_committed_origin(
      url::Origin::Create(new_url));
  entry->SetOriginalRequestURL(new_url);
  return NavigationEntryUpdateError::UPDATE_OK;
}
#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
void ArkWebNavigationControllerImplExt::ReloadWithNetError(
    ReloadType reload_type,
    bool check_for_repost,
    ErrorPageReloadReason  reason) {
  reload_reason_ = reason;
  Reload(reload_type, check_for_repost);
}
#endif

}  // namespace content
