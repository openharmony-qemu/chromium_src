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

#include "arkweb/chromium_ext/components/subresource_filter/content/shared/browser/page_load_statistics_ext.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/task/thread_pool.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

namespace subresource_filter {

#if BUILDFLAG(ARKWEB_ADBLOCK)
PageLoadStatisticsExt::PageLoadStatisticsExt(
    const mojom::ActivationState& state,
    content::WebContents* web_contents)
    : PageLoadStatistics(state, ""), is_first_report_(true) {
  web_contents_ = web_contents;
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

#if BUILDFLAG(ARKWEB_ADBLOCK)
void PageLoadStatisticsExt::SetReported() {
  is_first_report_ = false;
}

void PageLoadStatisticsExt::ClearStatistics(
    mojom::DocumentLoadStatistics& statistics) {
  statistics.num_loads_total = 0;
  statistics.num_loads_evaluated = 0;
  statistics.num_loads_matching_rules = 0;
  statistics.num_loads_disallowed = 0;
  statistics.loads_disallowed_url_map.clear();
}

void PageLoadStatisticsExt::NotifyAndResetStatistics() {
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

void PageLoadStatisticsExt::OnStatisticsAfterDocumentLoad(
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
PageLoadStatisticsExt::getAggregatedDocumentStatistics() {
  return aggregated_document_statistics_;
}

void PageLoadStatisticsExt::OnUserDocumentLoadStatistics(
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

void PageLoadStatisticsExt::UserNotifyAndResetStatistics() {
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

void PageLoadStatisticsExt::OnUserStatisticsAfterDocumentLoad(
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
PageLoadStatisticsExt::getUserAggregatedDocumentStatistics() {
  return user_aggregated_document_statistics_;
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

}  // namespace subresource_filter
