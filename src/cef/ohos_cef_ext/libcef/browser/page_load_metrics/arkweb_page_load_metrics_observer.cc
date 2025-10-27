// Copyright (c) 2014 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/page_load_metrics/arkweb_page_load_metrics_observer.h"

#include <string>

#include "arkweb/build/features/features.h"
#include "base/time/time.h"
#include "chrome/browser/browser_process.h"
#include "components/page_load_metrics/browser/observers/core/largest_contentful_paint_handler.h"
#include "components/page_load_metrics/browser/page_load_metrics_util.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents.h"
#include "content/public/renderer/render_frame.h"
#include "libcef/browser/alloy/alloy_browser_host_impl.h"
#include "libcef/browser/first_meaningful_paint_details_impl.h"
#include "libcef/browser/largest_contentful_paint_details_impl.h"
#include "libcef/common/app_manager.h"
#include "libcef/renderer/browser_impl.h"
#include "ohos_cef_ext/include/arkweb_load_handler_ext.h"
#include "ohos_nweb/include/nweb_handler.h"
#include "services/network/public/cpp/network_quality_tracker.h"
#include "url/gurl.h"
#if BUILDFLAG(ARKWEB_NETWORK_DFX)
#include "ohos_nweb/src/sysevent/event_reporter.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_DFX)
int64_t OhPageLoadMetricsObserver::navigation_start_timestamp_ = -1;
int64_t OhPageLoadMetricsObserver::render_init_block_ = -1;
#endif

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
#include "ohos_nweb/src/sysevent/event_reporter.h"
#endif

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
// TODO: ARKWEB_REPORT_SYS_EVENT
// int64_t OhPageLoadMetricsObserver::navigation_start_timestamp_ = -1;
#endif

static int64_t GetCurrentTimestampMS() {
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime)
      .count();
}

OhPageLoadMetricsObserver::OhPageLoadMetricsObserver() {
  network_quality_tracker_ = g_browser_process->network_quality_tracker();
  DCHECK(network_quality_tracker_);
}

OhPageLoadMetricsObserver::ObservePolicy OhPageLoadMetricsObserver::OnStart(
    content::NavigationHandle* navigation_handle,
    const GURL& currently_committed_url,
    bool started_in_foreground) {
  navigation_id_ = navigation_handle->GetNavigationId();
#if BUILDFLAG(ARKWEB_NETWORK_DFX)
  web_performance_timing_.navigation_id = navigation_id_;
  int64_t input_time = GetCurrentTimestampMS();
  web_performance_timing_.input_time = input_time;
  web_performance_timing_.is_paint_done = false;
  ReportFirstMeaningfulPaintDone(web_performance_timing_);
#endif

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  // TODO: ARKWEB_REPORT_SYS_EVENT
  // web_performance_timing_.navigation_id = navigation_id_;
#endif

  return CONTINUE_OBSERVING;
}

page_load_metrics::PageLoadMetricsObserver::ObservePolicy
OhPageLoadMetricsObserver::OnFencedFramesStart(
    content::NavigationHandle* navigation_handle,
    const GURL& currently_committed_url) {
  // This class uses the event OnLoadedResource, but only uses the one with
  // network::mojom::RequestDestination::kDocument, which never occur in
  // FencedFrames' navigation. So, we can use STOP_OBSERVING.
  return STOP_OBSERVING;
}

page_load_metrics::PageLoadMetricsObserver::ObservePolicy
OhPageLoadMetricsObserver::OnPrerenderStart(
    content::NavigationHandle* navigation_handle,
    const GURL& currently_committed_url) {
  return CONTINUE_OBSERVING;
}

page_load_metrics::PageLoadMetricsObserver::ObservePolicy
OhPageLoadMetricsObserver::FlushMetricsOnAppEnterBackground(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  ReportBufferedMetrics(timing);
  ReportLargestContentfulPaint(timing);
#endif
  // We continue observing after being backgrounded, in case we are foregrounded
  // again without being killed. In those cases we may still report non-buffered
  // metrics such as FCP after being re-foregrounded.
  return CONTINUE_OBSERVING;
}

OhPageLoadMetricsObserver::ObservePolicy OhPageLoadMetricsObserver::OnHidden(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  ReportBufferedMetrics(timing);
  ReportLargestContentfulPaint(timing);
#endif
  return CONTINUE_OBSERVING;
}

void OhPageLoadMetricsObserver::OnComplete(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  ReportBufferedMetrics(timing);
  ReportLargestContentfulPaint(timing);
#endif
}

int64_t OhPageLoadMetricsObserver::GetNavigationStartTime() {
  base::Time reference_wall_time = base::Time::Now();
  base::TimeTicks reference_monotonic_time = base::TimeTicks().Now();
  base::TimeTicks navigation_start = GetDelegate().GetNavigationStart();
  base::TimeDelta elapsed_time = navigation_start - reference_monotonic_time;
  base::Time navigation_start_wall_time = reference_wall_time + elapsed_time;
  double navigation_start_wall_time_double_t =
      navigation_start_wall_time.InSecondsFSinceUnixEpoch();
  int64_t navigation_start_time = navigation_start_wall_time_double_t *
                                  base::Time::kMicrosecondsPerMillisecond;
  return navigation_start_time;
}

void OhPageLoadMetricsObserver::OnFirstContentfulPaintInPage(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  int64_t first_contentful_paint_ms =
      timing.paint_timing->first_contentful_paint->InMilliseconds();

#if BUILDFLAG(ARKWEB_NETWORK_DFX)
  web_performance_timing_.first_contentful_paint = first_contentful_paint_ms;
#endif
  int64_t navigation_start_time = GetNavigationStartTime();
  ReportFirstContentfulPaint(navigation_start_time, first_contentful_paint_ms);
}

void OhPageLoadMetricsObserver::ReportFirstContentfulPaint(
    int64_t navigation_start_tick,
    int64_t first_contentful_paint_ms) {
  CefRefPtr<AlloyBrowserHostImpl> browser =
      AlloyBrowserHostImpl::GetBrowserForContents(
          GetDelegate().GetWebContents());
  if (!browser.get()) {
    return;
  }
  CefRefPtr<CefClient> client = browser->GetClient();
  if (!client.get()) {
    return;
  }
  CefRefPtr<ArkWebLoadHandlerExt> load_handler = client->GetLoadHandler();
  if (!load_handler.get()) {
    return;
  }
  load_handler->OnFirstContentfulPaint(navigation_start_tick,
                                       first_contentful_paint_ms);
}

void OhPageLoadMetricsObserver::OnFirstMeaningfulPaintInMainFrameDocument(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  int64_t first_meaningful_paint_time = 0;
  if (timing.paint_timing->first_meaningful_paint.has_value() &&
      timing.paint_timing->first_meaningful_paint->is_positive()) {
    first_meaningful_paint_time =
        timing.paint_timing->first_meaningful_paint->InMilliseconds();
  }

  int64_t navigation_start_time = GetNavigationStartTime();
  ReportFirstMeaningfulPaint(navigation_start_time,
                             first_meaningful_paint_time);
  web_performance_timing_.first_meaningful_paint = GetCurrentTimestampMS();
  web_performance_timing_.is_paint_done = true;
  ReportFirstMeaningfulPaintDone(web_performance_timing_);
}

void OhPageLoadMetricsObserver::ReportFirstMeaningfulPaint(
    int64_t navigation_start_time,
    int64_t first_meaningful_paint_time) {
  CefRefPtr<AlloyBrowserHostImpl> browser =
      AlloyBrowserHostImpl::GetBrowserForContents(
          GetDelegate().GetWebContents());
  if (!browser.get()) {
    return;
  }

  CefRefPtr<CefClient> client = browser->GetClient();
  if (!client.get()) {
    return;
  }

  CefRefPtr<ArkWebLoadHandlerExt> load_handler = client->GetLoadHandler();
  if (!load_handler.get()) {
    return;
  }
  CefRefPtr<CefFirstMeaningfulPaintDetails> details =
      new CefFirstMeaningfulPaintDetailsImpl(navigation_start_time,
                                             first_meaningful_paint_time);
  load_handler->OnFirstMeaningfulPaint(details);
}

void OhPageLoadMetricsObserver::ReportLargestContentfulPaint(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  CefRefPtr<AlloyBrowserHostImpl> browser =
      AlloyBrowserHostImpl::GetBrowserForContents(
          GetDelegate().GetWebContents());
  if (!browser.get()) {
    return;
  }

  CefRefPtr<CefClient> client = browser->GetClient();
  if (!client.get()) {
    return;
  }

  CefRefPtr<ArkWebLoadHandlerExt> load_handler = client->GetLoadHandler();
  if (!load_handler.get()) {
    return;
  }

  int64_t navigation_start_time = GetNavigationStartTime();
  int64_t largest_image_paint_time = 0;
  if (timing.paint_timing->largest_contentful_paint->largest_image_paint
          .has_value() &&
      timing.paint_timing->largest_contentful_paint->largest_image_paint
          ->is_positive()) {
    largest_image_paint_time = timing.paint_timing->largest_contentful_paint
                                   ->largest_image_paint->InMilliseconds();
  }

  int64_t largest_text_paint_time = 0;
  if (timing.paint_timing->largest_contentful_paint->largest_text_paint
          .has_value() &&
      timing.paint_timing->largest_contentful_paint->largest_text_paint
          ->is_positive()) {
    largest_text_paint_time = timing.paint_timing->largest_contentful_paint
                                  ->largest_text_paint->InMilliseconds();
  }

  int64_t largest_image_load_start_time = 0;
  if (timing.paint_timing->largest_contentful_paint->resource_load_timings
          ->load_start.has_value() &&
      timing.paint_timing->largest_contentful_paint->resource_load_timings
          ->load_start->is_positive()) {
    largest_image_load_start_time =
        timing.paint_timing->largest_contentful_paint->resource_load_timings
            ->load_start->InMilliseconds();
  }

  int64_t largest_image_load_end_time = 0;
  if (timing.paint_timing->largest_contentful_paint->resource_load_timings
          ->load_start.has_value() &&
      timing.paint_timing->largest_contentful_paint->resource_load_timings
          ->load_start->is_positive()) {
    largest_image_load_end_time =
        timing.paint_timing->largest_contentful_paint->resource_load_timings
            ->load_start->InMilliseconds();
  }

  double_t image_bpp = timing.paint_timing->largest_contentful_paint->image_bpp;

  CefRefPtr<CefLargestContentfulPaintDetails> details =
      new CefLargestContentfulPaintDetailsImpl(
          navigation_start_time, largest_image_paint_time,
          largest_text_paint_time, largest_image_load_start_time,
          largest_image_load_end_time, image_bpp);
  load_handler->OnLargestContentfulPaint(details);
}

#if BUILDFLAG(ARKWEB_NETWORK_DFX)
page_load_metrics::PageLoadMetricsObserver::ObservePolicy
OhPageLoadMetricsObserver::OnRedirect(
    content::NavigationHandle* navigation_handle) {
  main_frame_request_redirect_count_++;
  return CONTINUE_OBSERVING;
}

void OhPageLoadMetricsObserver::OnDomContentLoadedEventStart(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  web_performance_timing_.redirect_start =
      timing.redirect_start ? timing.redirect_start->InMilliseconds() : -1;
  web_performance_timing_.redirect_end =
      timing.redirect_end ? timing.redirect_end->InMilliseconds() : -1;
  web_performance_timing_.fetch_start =
      timing.fetch_start ? timing.fetch_start->InMilliseconds() : -1;
  web_performance_timing_.response_end =
      timing.response_end ? timing.response_end->InMilliseconds() : -1;
}

void OhPageLoadMetricsObserver::OnLoadEventEnd(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  web_performance_timing_.dom_interactive =
      timing.document_timing->dom_interactive
          ? timing.document_timing->dom_interactive->InMilliseconds()
          : -1;
  web_performance_timing_.dom_content_loaded_event_start =
      timing.document_timing->dom_content_loaded_event_start
          ? timing.document_timing->dom_content_loaded_event_start
                ->InMilliseconds()
          : -1;
  web_performance_timing_.dom_content_loaded_event_end =
      timing.document_timing->dom_content_loaded_event_end
          ? timing.document_timing->dom_content_loaded_event_end
                ->InMilliseconds()
          : -1;
  web_performance_timing_.load_event_start =
      timing.document_timing->load_event_start
          ? timing.document_timing->load_event_start->InMilliseconds()
          : -1;
  web_performance_timing_.load_event_end =
      timing.document_timing->load_event_end
          ? timing.document_timing->load_event_end->InMilliseconds()
          : -1;
}

void OhPageLoadMetricsObserver::OnLoadedResource(
    const page_load_metrics::ExtraRequestCompleteInfo&
        extra_request_complete_info) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (extra_request_complete_info.request_destination ==
      network::mojom::RequestDestination::kDocument) {
    if (did_dispatch_on_main_resource_) {
      return;
    }
    did_dispatch_on_main_resource_ = true;

    base::TimeTicks navigation_start = GetDelegate().GetNavigationStart();

    const net::LoadTimingInfo& timing =
        *extra_request_complete_info.load_timing_info;
    web_performance_timing_.worker_start =
        timing.service_worker_start_time.is_null()
            ? -1
            : (timing.service_worker_start_time - navigation_start)
                  .InMilliseconds();
    web_performance_timing_.domain_lookup_start =
        timing.connect_timing.domain_lookup_start.is_null()
            ? -1
            : (timing.connect_timing.domain_lookup_start - navigation_start)
                  .InMilliseconds();
    web_performance_timing_.domain_lookup_end =
        timing.connect_timing.domain_lookup_end.is_null()
            ? -1
            : (timing.connect_timing.domain_lookup_end - navigation_start)
                  .InMilliseconds();
    web_performance_timing_.connect_start =
        timing.connect_timing.connect_start.is_null()
            ? -1
            : (timing.connect_timing.connect_start - navigation_start)
                  .InMilliseconds();
    web_performance_timing_.secure_connect_start =
        timing.connect_timing.ssl_start.is_null()
            ? -1
            : (timing.connect_timing.ssl_start - navigation_start)
                  .InMilliseconds();
    web_performance_timing_.connect_end =
        timing.connect_timing.connect_end.is_null()
            ? -1
            : (timing.connect_timing.connect_end - navigation_start)
                  .InMilliseconds();
    web_performance_timing_.request_start =
        timing.send_start.is_null()
            ? -1
            : (timing.send_start - navigation_start).InMilliseconds();
    web_performance_timing_.response_start =
        timing.receive_headers_start.is_null()
            ? -1
            : (timing.receive_headers_start - navigation_start)
                  .InMilliseconds();
  }
}

void OhPageLoadMetricsObserver::OnFirstPaintInPage(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  web_performance_timing_.first_paint =
      timing.paint_timing->first_paint
          ? timing.paint_timing->first_paint->InMilliseconds()
          : -1;
}

void OhPageLoadMetricsObserver::ReportPerformanceTiming() {
  web_performance_timing_.render_init_block = render_init_block_;
  ReportPageLoadTimeStats(web_performance_timing_);
  web_performance_timing_.Reset();
  render_init_block_ = -1;
}

void OhPageLoadMetricsObserver::ReportBufferedMetrics(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  if (!GetDelegate().DidCommit() || reported_buffered_metrics_) {
    return;
  }

  reported_buffered_metrics_ = true;

  web_performance_timing_.navigation_start = navigation_start_timestamp_;
  web_performance_timing_.redirect_count = main_frame_request_redirect_count_;
  const page_load_metrics::ContentfulPaintTimingInfo& largest_contentful_paint =
      GetDelegate()
          .GetLargestContentfulPaintHandler()
          .MergeMainFrameAndSubframes();

  if (largest_contentful_paint.ContainsValidTime()) {
    web_performance_timing_.largest_contentful_paint =
        largest_contentful_paint.Time()->InMilliseconds();
    ReportPerformanceTiming();
    return;
  }
}

void OhPageLoadMetricsObserver::OnNavigationStart() {
  navigation_start_timestamp_ = GetCurrentTimestampMS();
}

void OhPageLoadMetricsObserver::RenderInitBlock(int64_t block_time) {
  render_init_block_ = block_time;
}
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
void OhPageLoadMetricsObserver::OnRestoreFromBackForwardCache(
    const page_load_metrics::mojom::PageLoadTiming& timing,
    content::NavigationHandle* navigation_handle) {
  back_forward_cache_navigation_ids_.push_back(
      navigation_handle->GetNavigationId());
}

page_load_metrics::PageLoadMetricsObserver::ObservePolicy
OhPageLoadMetricsObserver::OnEnterBackForwardCache(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  return CONTINUE_OBSERVING;
}

void OhPageLoadMetricsObserver::
    OnFirstContentfulPaintAfterBackForwardCacheRestoreInPage(
        const page_load_metrics::mojom::BackForwardCacheTiming& timing,
        size_t index) {
  if (index >= back_forward_cache_navigation_ids_.size()) {
    return;
  }

  int64_t first_contentful_paint_ms =
      timing.first_paint_after_back_forward_cache_restore.InMilliseconds();
#if defined(REPORT_SYS_EVENT)
  web_performance_timing_.first_contentful_paint = first_contentful_paint_ms;
#endif
  int64_t navigation_start_time = GetNavigationStartTime();
  ReportFirstContentfulPaint(navigation_start_time, first_contentful_paint_ms);
}
#endif
