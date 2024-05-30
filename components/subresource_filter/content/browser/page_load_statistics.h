// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_PAGE_LOAD_STATISTICS_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_PAGE_LOAD_STATISTICS_H_

#include "components/subresource_filter/core/mojom/subresource_filter.mojom.h"

#ifdef OHOS_ARKWEB_ADBLOCK
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "content/public/browser/web_contents_observer.h"

namespace content {
class WebContents;
} // namespace content

#endif  // OHOS_ARKWEB_ADBLOCK

namespace subresource_filter {

// This class is notified of metrics recorded for individual (sub-)documents of
// a page, aggregates them, and logs the aggregated metrics to UMA histograms
// when the page load is complete (at the load event).
class PageLoadStatistics {
 public:
  PageLoadStatistics(const mojom::ActivationState& state);

  PageLoadStatistics(const PageLoadStatistics&) = delete;
  PageLoadStatistics& operator=(const PageLoadStatistics&) = delete;

  ~PageLoadStatistics();

#ifdef OHOS_ARKWEB_ADBLOCK
  PageLoadStatistics(const mojom::ActivationState& state,
                     content::WebContents* web_contents);

  void SetReported();
  bool IsFirstReport() { return is_first_report_; }
  void OnStatisticsAfterDocumentLoad(
      const mojom::DocumentLoadStatistics& statistics);
  mojom::DocumentLoadStatistics getAggregatedDocumentStatistics();

  void OnUserDocumentLoadStatistics(
      const mojom::DocumentLoadStatistics& statistics);
  void OnUserStatisticsAfterDocumentLoad(
      const mojom::DocumentLoadStatistics& statistics);
  mojom::DocumentLoadStatistics getUserAggregatedDocumentStatistics();
#endif  // OHOS_ARKWEB_ADBLOCK

  void OnDocumentLoadStatistics(
      const mojom::DocumentLoadStatistics& statistics);
  void OnDidFinishLoad();

 private:
  mojom::ActivationState activation_state_;

  // Statistics about subresource loads, aggregated across all frames of the
  // current page.
  mojom::DocumentLoadStatistics aggregated_document_statistics_;

#ifdef OHOS_ARKWEB_ADBLOCK
  friend class WebContentsImpl;
  void ClearStatistics(mojom::DocumentLoadStatistics& statistics);
  void NotifyAndResetStatistics();
  mojom::DocumentLoadStatistics aggregated_document_after_load_statistics_;
  bool is_first_report_;
  raw_ptr<content::WebContents> web_contents_ = nullptr;

  void UserNotifyAndResetStatistics();
  mojom::DocumentLoadStatistics user_aggregated_document_statistics_;
  mojom::DocumentLoadStatistics user_aggregated_document_after_load_statistics_;
  base::WeakPtrFactory<PageLoadStatistics> weak_ptr_factory_{this};
#endif  // OHOS_ARKWEB_ADBLOCK
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_PAGE_LOAD_STATISTICS_H_
