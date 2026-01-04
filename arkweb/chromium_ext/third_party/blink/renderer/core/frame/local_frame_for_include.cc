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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_for_include.h"

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/page/page_utils.h"
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/blank_screen_detector.h"
#endif
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
#include "base/memory/safe_ref.h"
#endif
#if BUILDFLAG(IS_ARKWEB)
#include "arkweb/chromium_ext/third_party/blink/renderer/core/editing/frame_selection_ext.h"
#include "base/ohos/sys_info_utils_ext.h"
#endif

namespace blink {
  
// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void LocalFrame::NotifyContextMenuWillShow() {
  GetLocalFrameHostRemote().NotifyContextMenuWillShow();
}
#endif
// LCOV_EXCL_STOP

bool LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
    LocalFrame* LocalFrameObj,
    float& layout_zoom_factor,
    float& text_zoom_factor,
    Page* page) {
  if (!page) {
    return false;
  }

  text_zoom_factor = page->GetSettings().GetTextZoomFactor();
  if (LocalFrameObj->layout_zoom_factor_ == layout_zoom_factor &&
      LocalFrameObj->text_zoom_factor_ == text_zoom_factor) {
    return false;
  }
  return true;
}

void LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
    LocalFrame* LocalFrameObj,
    float& layout_zoom_factor,
    float& text_zoom_factor,
    bool& layout_zoom_changed,
    Page* page) {
  if (base::ohos::IsTabletDevice() && !base::ohos::IsPcMode()) {
    float zoom_factor_for_device_scale =
        page->GetChromeClient().ZoomFactorForViewportLayout();
    zoom_factor_for_device_scale =
        zoom_factor_for_device_scale ? zoom_factor_for_device_scale : 1;
    float current_zoom_factor =
        layout_zoom_factor / zoom_factor_for_device_scale;
    if (layout_zoom_changed && current_zoom_factor > 1.0f &&
        !LocalFrameObj->scale_limits_min_changed_) {
      LocalFrameObj->scale_limits_min_changed_ = true;
      page->page_utils()->ResetPageScaleConstraints(false);
      LocalFrameObj->scale_limits_max_changed_ = false;
    }
    if (layout_zoom_changed && current_zoom_factor <= 1.0f &&
        !LocalFrameObj->scale_limits_max_changed_) {
      LocalFrameObj->scale_limits_max_changed_ = true;
      page->page_utils()->ResetPageScaleConstraints(true);
      LocalFrameObj->scale_limits_min_changed_ = false;
    }
  }
}

void LocalFrameUtil::SetTextZoomFactorsExt(LocalFrame* LocalFrameObj) {
  for (Frame* child = LocalFrameObj->Tree().FirstChild(); child;
      child = child->Tree().NextSibling()) {
    if (auto* child_local_frame = DynamicTo<LocalFrame>(child)) {
        child_local_frame->SetTextZoomFactor(LocalFrameObj->text_zoom_factor_);
    }
  }
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void LocalFrame::OnOverScrollOffsetChanged(float offset_x, float offset_y) {
  if (!IsMainThread()) {
    GetTaskRunner(TaskType::kInternalDefault)
        ->PostTask(FROM_HERE,
                   WTF::BindOnce(&LocalFrame::OnOverScrollOffsetChanged,
                                 WrapPersistent(weak_local_frame_.GetWeakCell()), offset_x, offset_y));
  } else {
    GetLocalFrameHostRemote().OnOverScrollOffsetChanged(offset_x, offset_y);
  }
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
void LocalFrame::OnDetectedBlankScreen(const WTF::String& url,
                                       int32_t blankScreenReason,
                                       int32_t detectedContentfulNodesCount) {
  GetLocalFrameHostRemote().OnDetectedBlankScreen(url, blankScreenReason,
                                                  detectedContentfulNodesCount);
}

std::shared_ptr<BlankScreenDetector> LocalFrame::GetBlankScreenDetector(
    bool force) {
  if (!blank_screen_detector_ && force) {
    blank_screen_detector_ = std::make_shared<BlankScreenDetector>(this);
  }
  return blank_screen_detector_;
}
#endif

#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
void LocalFrame::OnFirstScreenPaint(
    const WTF::String& url,
    const base::TimeTicks& navigation_start,
    const base::TimeTicks& first_screen_paint) {
  base::Time reference_wall_time = base::Time::Now();
  base::TimeTicks reference_monotonic_time = base::TimeTicks().Now();

  base::TimeDelta navigation_start_elapsed_time =
      navigation_start - reference_monotonic_time;
  base::Time navigation_start_wall_time =
      reference_wall_time + navigation_start_elapsed_time;
  double navigation_start_wall_time_double_t =
      navigation_start_wall_time.InSecondsFSinceUnixEpoch();
  int64_t navigation_start_time =
      static_cast<int64_t>(navigation_start_wall_time_double_t *
                           base::Time::kMicrosecondsPerMillisecond);

  base::TimeDelta first_screen_paint_elapsed_time =
      first_screen_paint - reference_monotonic_time;
  base::Time first_screen_paint_wall_time =
      reference_wall_time + first_screen_paint_elapsed_time;
  double first_screen_paint_time_t =
      first_screen_paint_wall_time.InSecondsFSinceUnixEpoch();
  int64_t first_screen_paint_time = static_cast<int64_t>(
      first_screen_paint_time_t * base::Time::kMicrosecondsPerMillisecond);
  GetLocalFrameHostRemote().OnFirstScreenPaint(url,
                                               navigation_start_time,
                                               first_screen_paint_time);
}
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_ADBLOCK)
void LocalFrame::DidSubresourceFiltered() {
  Client()->DispatchDidSubresourceFiltered();
}

bool LocalFrame::GetGlobalAdblockEnabled() {
  return Client()->GetGlobalAdblockEnabled();
}

void LocalFrame::SetAdBlockEnableForSite(bool site_adblock_enabled) {
  LOG(WARNING) << "[Adblock] Set adblock enable for site: "
               << site_adblock_enabled;
  adblock_enabled_for_site_ = site_adblock_enabled;
}

void LocalFrame::SetHasElemHideTypeOption(bool has_elemhide_type_option) {
  has_elemhide_type_option_ = has_elemhide_type_option;
}

void LocalFrame::SetHasDocumentTypeOption(bool has_document_type_option) {
  has_document_type_option_ = has_document_type_option;
}

void LocalFrame::SetHasGenericHideTypeOption(bool has_generichide_type_option) {
  has_generichide_type_option_ = has_generichide_type_option;
}
#endif

#if BUILDFLAG(ARKWEB_PDF)
bool LocalFrame::IsPDF() {
  if (!Client()) {
    LOG(ERROR) << "Client() null";
    return false;
  }
  WebLocalFrame* web_frame = Client()->GetWebFrame();
  if (web_frame) {
    WebLocalFrameClient* client = web_frame->Client();
    if (client) {
      bool is_pdf = client->IsPDF();
      LOG_IF(INFO, is_pdf) << "current frame is pdf.";
      return is_pdf;      
    }
  }
  return false;
}
#endif

// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
bool LocalFrame::IsVideoPrioritySupported() {
  KURL url;
  if (GetDocument()) {
    url = GetDocument()->Url();
  }

  GURL main_url;
  if (!url.IsEmpty() && url.IsValid()) {
    main_url = GURL(url.GetString().Utf8().data());
  }
  std::string surl = main_url.DeprecatedGetOriginAsURL().spec();

  if (Client() && Client()->GetWebFrame() &&
      Client()->GetWebFrame()->Client()) {
    WebLocalFrameClient* client = Client()->GetWebFrame()->Client();
    return client->AsWebLocalFrameClientExt()->IsVideoLoadOptimizationEnabled(surl);
  }
  return false;
}

bool LocalFrame::SetNewsFeedPageFitted() {
  if (Client() && Client()->GetWebFrame() &&
      Client()->GetWebFrame()->Client()) {
    WebLocalFrameClient* client = Client()->GetWebFrame()->Client();
    return client->AsWebLocalFrameClientExt()->SetNewsFeedPageFitted();
  }
  return false;
}

void LocalFrame::SetVideoIsPlaying(std::string id, bool playing) {
  loader_manager_.SetVideoIsPlaying(id, playing);
}

void LocalFrame::SetVideoPriority(const HeapVector<Member<VideoPriority>>& vec) {
  loader_manager_.SetVideoPriority(vec);
}

void LocalFrame::RegisterUrlLoader(base::WeakPtr<VideoURLLoaderImpl> loader,
                                   std::string id,
                                   int64_t start,
                                   WebURLRequest request,
                                   base::WeakPtr<WebAssociatedURLLoaderClient> client) {
  PriorityLoader load =
      PriorityLoader(loader, id, start, std::move(request), client);
  loader_manager_.AddUrlLoader(std::move(load));
}

void LocalFrame::NotifyFinished(base::WeakPtr<VideoURLLoaderImpl> loader) {
  loader_manager_.RemoveUrlLoader(loader);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_AI)
void LocalFrame::StartHighlightFadeTimer(base::TimeDelta delay) {
  if (!(GetSettings() && GetSettings()->GetArkwebAgentEnabled())) {
    return;
  }
  highlight_fade_timer_.Stop();
  highlight_fade_timer_.SetTaskRunner(
      GetTaskRunner(TaskType::kInternalFindInPage));

  if (delay.is_zero()) {
    ClearHighlight();
  } else {
    highlight_fade_timer_.Start(FROM_HERE, delay,
                                WTF::BindOnce(&LocalFrame::ClearHighlight,
                                              WrapPersistent(weak_local_frame_.GetWeakCell())));
  }
}

void LocalFrame::ClearHighlight() {
  if (!GetTextFragmentHandler()) {
    LOG(ERROR) << "LocalFrame::ClearHighlight failed, no handler.";
    return;
  }
  GetTextFragmentHandler()->RemoveFragments();
}
#endif
}  // namespace blink
