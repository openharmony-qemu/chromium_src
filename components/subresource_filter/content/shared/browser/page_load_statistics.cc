// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/subresource_filter/content/shared/browser/page_load_statistics.h"

#include <string_view>

#include "base/check.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/not_fatal_until.h"
#include "base/strings/strcat.h"
#include "components/subresource_filter/core/common/time_measurements.h"
#include "components/subresource_filter/core/mojom/subresource_filter.mojom.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/task/thread_pool.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

namespace subresource_filter {

#if BUILDFLAG(ARKWEB_ADBLOCK)
PageLoadStatistics::PageLoadStatistics(const mojom::ActivationState& state,
                                       content::WebContents* web_contents)
    : activation_state_(state), is_first_report_(true) {
  web_contents_ = web_contents;
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

PageLoadStatistics::PageLoadStatistics(const mojom::ActivationState& state,
                                       std::string_view uma_filter_tag)
    : activation_state_(state), uma_filter_tag_(uma_filter_tag) {}

PageLoadStatistics::~PageLoadStatistics() = default;

void PageLoadStatistics::OnDocumentLoadStatistics(
    const mojom::DocumentLoadStatistics& statistics) {
  // Note: Chances of overflow are negligible.
  aggregated_document_statistics_.num_loads_total += statistics.num_loads_total;
  aggregated_document_statistics_.num_loads_evaluated +=
      statistics.num_loads_evaluated;
  aggregated_document_statistics_.num_loads_matching_rules +=
      statistics.num_loads_matching_rules;
  aggregated_document_statistics_.num_loads_disallowed +=
      statistics.num_loads_disallowed;

  aggregated_document_statistics_.evaluation_total_wall_duration +=
      statistics.evaluation_total_wall_duration;
  aggregated_document_statistics_.evaluation_total_cpu_duration +=
      statistics.evaluation_total_cpu_duration;

#if BUILDFLAG(ARKWEB_ADBLOCK)
  // 需要将每个Document 拦截到的url 进行累加
  for (auto& pair : statistics.loads_disallowed_url_map) {
    aggregated_document_statistics_.loads_disallowed_url_map.insert(pair);
  }
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
void PageLoadStatistics::SetReported() {
  is_first_report_ = false;
}

void PageLoadStatistics::ClearStatistics(
    mojom::DocumentLoadStatistics& statistics) {
  statistics.num_loads_total = 0;
  statistics.num_loads_evaluated = 0;
  statistics.num_loads_matching_rules = 0;
  statistics.num_loads_disallowed = 0;
  statistics.loads_disallowed_url_map.clear();
}

void PageLoadStatistics::NotifyAndResetStatistics() {
  if (web_contents_ &&
      content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    std::map<std::string, int32_t> subresource_map;
    for (auto& pair :
         aggregated_document_after_load_statistics_.loads_disallowed_url_map) {
      subresource_map.insert(pair);
    }
    if (subresource_map.size() > 0) {
      LOG(INFO)
          << "[AdBlock] On url blocked reported after loaded finish, size:"
          << subresource_map.size();
      web_contents_->OnAdsBlocked(web_contents_->GetLastCommittedURL().spec(),
                                  subresource_map, IsFirstReport());
      if (IsFirstReport()) {
        SetReported();
      }
    }

    ClearStatistics(aggregated_document_after_load_statistics_);
  }
}

void PageLoadStatistics::OnStatisticsAfterDocumentLoad(
    const mojom::DocumentLoadStatistics& statistics) {
  // Note: Chances of overflow are negligible.
  aggregated_document_after_load_statistics_.num_loads_total +=
      statistics.num_loads_total;
  aggregated_document_after_load_statistics_.num_loads_evaluated +=
      statistics.num_loads_evaluated;
  aggregated_document_after_load_statistics_.num_loads_matching_rules +=
      statistics.num_loads_matching_rules;
  aggregated_document_after_load_statistics_.num_loads_disallowed +=
      statistics.num_loads_disallowed;

  aggregated_document_after_load_statistics_.evaluation_total_wall_duration +=
      statistics.evaluation_total_wall_duration;
  aggregated_document_after_load_statistics_.evaluation_total_cpu_duration +=
      statistics.evaluation_total_cpu_duration;

  // 需要将每个Document拦到url进行加
  for (auto& pair : statistics.loads_disallowed_url_map) {
    aggregated_document_after_load_statistics_.loads_disallowed_url_map.insert(
        pair);
  }

  if (aggregated_document_after_load_statistics_.loads_disallowed_url_map
          .size() != 0) {
    NotifyAndResetStatistics();
  }
}

mojom::DocumentLoadStatistics
PageLoadStatistics::getAggregatedDocumentStatistics() {
  return aggregated_document_statistics_;
}

void PageLoadStatistics::OnUserDocumentLoadStatistics(
    const mojom::DocumentLoadStatistics& statistics) {
  // Note: Chances of overflow are negligible.
  user_aggregated_document_statistics_.num_loads_total +=
      statistics.num_loads_total;
  user_aggregated_document_statistics_.num_loads_evaluated +=
      statistics.num_loads_evaluated;
  user_aggregated_document_statistics_.num_loads_matching_rules +=
      statistics.num_loads_matching_rules;
  user_aggregated_document_statistics_.num_loads_disallowed +=
      statistics.num_loads_disallowed;
  user_aggregated_document_statistics_.evaluation_total_wall_duration +=
      statistics.evaluation_total_wall_duration;
  user_aggregated_document_statistics_.evaluation_total_cpu_duration +=
      statistics.evaluation_total_cpu_duration;

  for (auto& pair : statistics.loads_disallowed_url_map) {
    user_aggregated_document_statistics_.loads_disallowed_url_map.insert(pair);
  }
}

void PageLoadStatistics::UserNotifyAndResetStatistics() {
  if (web_contents_ &&
      content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    std::map<std::string, int32_t> subresource_map;
    for (auto& pair : user_aggregated_document_after_load_statistics_
                          .loads_disallowed_url_map) {
      subresource_map.insert(pair);
    }
    if (subresource_map.size() > 0) {
      LOG(INFO) << "[User AdBlock] On url blocked reported after loaded "
                   "finish, size:"
                << subresource_map.size();
      web_contents_->OnAdsBlocked(web_contents_->GetLastCommittedURL().spec(),
                                  subresource_map, IsFirstReport());
      if (IsFirstReport()) {
        SetReported();
      }
    }
    ClearStatistics(user_aggregated_document_after_load_statistics_);
  }
}

void PageLoadStatistics::OnUserStatisticsAfterDocumentLoad(
    const mojom::DocumentLoadStatistics& statistics) {
  // Note: Chances of overflow are negligible.
  user_aggregated_document_after_load_statistics_.num_loads_total +=
      statistics.num_loads_total;
  user_aggregated_document_after_load_statistics_.num_loads_evaluated +=
      statistics.num_loads_evaluated;
  user_aggregated_document_after_load_statistics_.num_loads_matching_rules +=
      statistics.num_loads_matching_rules;
  user_aggregated_document_after_load_statistics_.num_loads_disallowed +=
      statistics.num_loads_disallowed;
  user_aggregated_document_after_load_statistics_
      .evaluation_total_wall_duration +=
      statistics.evaluation_total_wall_duration;
  user_aggregated_document_after_load_statistics_
      .evaluation_total_cpu_duration +=
      statistics.evaluation_total_cpu_duration;

  for (auto& pair : statistics.loads_disallowed_url_map) {
    user_aggregated_document_after_load_statistics_.loads_disallowed_url_map
        .insert(pair);
  }

  if (user_aggregated_document_after_load_statistics_.loads_disallowed_url_map
          .size() != 0) {
    UserNotifyAndResetStatistics();
  }
}

mojom::DocumentLoadStatistics
PageLoadStatistics::getUserAggregatedDocumentStatistics() {
  return user_aggregated_document_statistics_;
}
#endif

void PageLoadStatistics::OnDidFinishLoad() {
  if (activation_state_.activation_level != mojom::ActivationLevel::kDisabled) {
    base::UmaHistogramCounts1000(
        base::StrCat({uma_filter_tag_, ".PageLoad.NumSubresourceLoads.Total"}),
        aggregated_document_statistics_.num_loads_total);
    base::UmaHistogramCounts1000(
        base::StrCat(
            {uma_filter_tag_, ".PageLoad.NumSubresourceLoads.Evaluated"}),
        aggregated_document_statistics_.num_loads_evaluated);
    base::UmaHistogramCounts1000(
        base::StrCat(
            {uma_filter_tag_, ".PageLoad.NumSubresourceLoads.MatchedRules"}),
        aggregated_document_statistics_.num_loads_matching_rules);
    base::UmaHistogramCounts1000(
        base::StrCat(
            {uma_filter_tag_, ".PageLoad.NumSubresourceLoads.Disallowed"}),
        aggregated_document_statistics_.num_loads_disallowed);
  }

  if (activation_state_.measure_performance) {
    CHECK(
        activation_state_.activation_level != mojom::ActivationLevel::kDisabled,
        base::NotFatalUntil::M129);
    base::UmaHistogramCustomTimes(
        base::StrCat({uma_filter_tag_,
                      ".PageLoad.SubresourceEvaluation.TotalWallDuration"}),
        aggregated_document_statistics_.evaluation_total_wall_duration,
        base::Microseconds(1), base::Seconds(10), 50);

    base::UmaHistogramCustomTimes(
        base::StrCat({uma_filter_tag_,
                      ".PageLoad.SubresourceEvaluation.TotalCPUDuration"}),
        aggregated_document_statistics_.evaluation_total_cpu_duration,
        base::Microseconds(1), base::Seconds(10), 50);
  }
  // Theoretically, we should be able to add an else case that CHECK()s that the
  // evaluation durations are zero. However, this causes crashes as the renderer
  // and browser appear to sometimes get out of sync. See crbug.com/372883698.
}

}  // namespace subresource_filter
