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

#include "arkweb/chromium_ext/components/dom_distiller/core/task_tracker_utils.h"
#include "components/dom_distiller/core/task_tracker.h"

#include "base/logging.h"

namespace dom_distiller {

TaskTrackerUtils::TaskTrackerUtils(TaskTracker* tracker) : tracker_(tracker) {}

#if BUILDFLAG(ARKWEB_READER_MODE)
void TaskTrackerUtils::AddViewer(ViewRequestDelegate* delegate) {
  std::unique_ptr<ViewRequestDelegate> delegate_owned(delegate);
  viewers_list_.push_back(std::move(delegate_owned));
}

void TaskTrackerUtils::AbortDistill() {
  for (auto& viewer : tracker_->viewers_) {
    viewer.OnArticleAborted();
  }
}
#endif  // ARKWEB_READER_MODE
}  // namespace dom_distiller