/// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PAGE_LOAD_METRICS_OH_PAGE_LOAD_METRICS_OBSERVER_H_
#define CEF_LIBCEF_BROWSER_PAGE_LOAD_METRICS_OH_PAGE_LOAD_METRICS_OBSERVER_H_

#include "base/memory/raw_ptr.h"
#include "components/page_load_metrics/browser/page_load_metrics_observer.h"
#include "net/nqe/effective_connection_type.h"
#if BUILDFLAG(ARKWEB_NETWORK_DFX)
#include "ohos_nweb/src/sysevent/oh_web_performance_timing.h"
#endif

namespace network {
class NetworkQualityTracker;
}

class GURL;

class OhPageLoadMetricsObserver
    : public page_load_metrics::PageLoadMetricsObserver {
 public:
  OhPageLoadMetricsObserver();

  OhPageLoadMetricsObserver(const OhPageLoadMetricsObserver&) = delete;
  OhPageLoadMetricsObserver& operator=(const OhPageLoadMetricsObserver&) =
      delete;

  // page_load_metrics::PageLoadMetricsObserver:
  ObservePolicy OnStart(content::NavigationHandle* navigation_handle,
                        const GURL& currently_committed_url,
                        bool started_in_foreground) override;
  ObservePolicy OnFencedFramesStart(
      content::NavigationHandle* navigation_handle,
      const GURL& currently_committed_url) override;
  ObservePolicy OnPrerenderStart(content::NavigationHandle* navigation_handle,
                                 const GURL& currently_committed_url) override;

  // PageLoadMetricsObserver lifecycle callbacks
  ObservePolicy FlushMetricsOnAppEnterBackground(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  ObservePolicy OnHidden(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnComplete(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;

  // PageLoadMetricsObserver event callbacks
  void OnFirstContentfulPaintInPage(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnFirstMeaningfulPaintInMainFrameDocument(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;

#if BUILDFLAG(ARKWEB_NETWORK_DFX)
  ObservePolicy OnRedirect(
      content::NavigationHandle* navigation_handle) override;
  void OnDomContentLoadedEventStart(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnLoadEventEnd(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnLoadedResource(const page_load_metrics::ExtraRequestCompleteInfo&
                            extra_request_complete_info) override;
  void OnFirstPaintInPage(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  static void OnNavigationStart();
  static void RenderInitBlock(int64_t block_time);
#endif
#if BUILDFLAG(ARKWEB_BFCACHE)
  page_load_metrics::PageLoadMetricsObserver::ObservePolicy
  OnEnterBackForwardCache(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnFirstContentfulPaintAfterBackForwardCacheRestoreInPage(
      const page_load_metrics::mojom::BackForwardCacheTiming& timing,
      size_t index) override;
  void OnRestoreFromBackForwardCache(
      const page_load_metrics::mojom::PageLoadTiming& timing,
      content::NavigationHandle* navigation_handle) override;
#endif
 protected:
  OhPageLoadMetricsObserver(
      network::NetworkQualityTracker* network_quality_tracker)
      : network_quality_tracker_(network_quality_tracker) {}

  virtual void ReportFirstContentfulPaint(int64_t navigation_start_tick,
                                          int64_t first_contentful_paint_ms);
  virtual void ReportFirstMeaningfulPaint(int64_t navigation_start_tick,
                                          int64_t first_meaningful_paint_ms);
  virtual void ReportLargestContentfulPaint(
      const page_load_metrics::mojom::PageLoadTiming& timing);

  int64_t GetNavigationStartTime();
#if BUILDFLAG(ARKWEB_NETWORK_DFX)
  void ReportBufferedMetrics(
      const page_load_metrics::mojom::PageLoadTiming& timing);
  void ReportPerformanceTiming();
#endif

 private:
  int64_t navigation_id_ = -1;

  raw_ptr<network::NetworkQualityTracker> network_quality_tracker_ = nullptr;

#if BUILDFLAG(ARKWEB_NETWORK_DFX)
  bool did_dispatch_on_main_resource_ = false;
  bool reported_buffered_metrics_ = false;
  uint32_t main_frame_request_redirect_count_ = 0;
  OhWebPerformanceTiming web_performance_timing_;
  static int64_t navigation_start_timestamp_;
  static int64_t render_init_block_;
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
  // IDs for the navigations when the page is restored from the back-forward
  // cache.
  std::vector<ukm::SourceId> back_forward_cache_navigation_ids_;
#endif
};

#endif  // CEF_LIBCEF_BROWSER_PAGE_LOAD_METRICS_OH_PAGE_LOAD_METRICS_OBSERVER_H_
