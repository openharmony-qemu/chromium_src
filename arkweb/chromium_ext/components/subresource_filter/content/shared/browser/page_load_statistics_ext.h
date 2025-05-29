/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_SHARED_BROWSER_PAGE_LOAD_STATISTICS_EXT_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_SHARED_BROWSER_PAGE_LOAD_STATISTICS_EXT_H_

#include "components/subresource_filter/content/shared/browser/page_load_statistics.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "content/public/browser/web_contents_observer.h"

namespace content {
class WebContents;
}  // namespace content
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

namespace subresource_filter {

// This class is notified of metrics recorded for individual (sub-)documents of
// a page, aggregates them, and logs the aggregated metrics to UMA histograms
// when the page load is complete (at the load event).
class PageLoadStatisticsExt : public PageLoadStatistics {
 public:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  PageLoadStatisticsExt(const mojom::ActivationState& state,
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
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

  PageLoadStatisticsExt* AsPageLoadStatisticsExt() override { return this; }

 private:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  friend class WebContentsImpl;
  void ClearStatistics(mojom::DocumentLoadStatistics& statistics);
  void NotifyAndResetStatistics();
  mojom::DocumentLoadStatistics aggregated_document_after_load_statistics_;
  bool is_first_report_;
  raw_ptr<content::WebContents> web_contents_ = nullptr;

  void UserNotifyAndResetStatistics();
  mojom::DocumentLoadStatistics user_aggregated_document_statistics_;
  mojom::DocumentLoadStatistics user_aggregated_document_after_load_statistics_;
  base::WeakPtrFactory<PageLoadStatisticsExt> weak_ptr_factory_{this};
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_SHARED_BROWSER_PAGE_LOAD_STATISTICS_EXT_H_
