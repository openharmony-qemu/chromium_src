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

#include "arkweb/chromium_ext/components/dom_distiller/core/dom_distiller_service_utils.h"
#include "components/dom_distiller/core/task_tracker.h"

namespace dom_distiller {

DomDistillerServiceUtils::DomDistillerServiceUtils(DomDistillerService* service)
    : service_(service) {}

#if BUILDFLAG(ARKWEB_READER_MODE)
bool DomDistillerServiceUtils::ShouldViewUrl(
    const GURL& url,
    const std::unique_ptr<DistillerPage>& distiller_page) {
  if (!url.is_valid() || !distiller_page ||
      !distiller_page->GetDistillOptions()) {
    return true;
  }
  return false;
}

bool DomDistillerServiceUtils::ViewUrl(
    TaskTracker*& task_tracker,
    const std::unique_ptr<DistillerPage>& distiller_page) {
  GURL distill_url(distiller_page->GetDistillOptions()->distill_url);
  bool was_created =
      service_->GetOrCreateTaskTrackerForUrl(distill_url, &task_tracker);

  LOG(INFO) << __func__ << " [Distiller] was_created:" << was_created
            << " tasks:" << service_->tasks_.size();
  return was_created;
}

void DomDistillerServiceUtils::AbortDistill() {
  std::vector<TaskTracker*> task_trackers;
  for (auto it = service_->tasks_.begin(); it != service_->tasks_.end(); ++it) {
    task_trackers.push_back((*it).get());
  }
  for (auto* task : task_trackers) {
    if (task) {
      task->utils()->AbortDistill();
    }
  }
}
#endif  // ARKWEB_READER_MODE
}  // namespace dom_distiller