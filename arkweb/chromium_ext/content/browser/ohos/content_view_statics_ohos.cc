/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/content/browser/ohos/content_view_statics_ohos.h"
namespace content {

void SuspendedProcessWatcherOHOS::PauseWebKitShardTimersFromOHOS() {
  for (content::RenderProcessHost::iterator iter(
           content::RenderProcessHost::AllHostsIterator());
       !iter.IsAtEnd(); iter.Advance()) {
    content::RenderProcessHost* host = iter.GetCurrentValue();
    if (!host) {
      LOG(INFO) << "PauseWebKitShardTimersFromOHOS host is nullptr.";
      continue;
    }
    if (pause_processes_.insert(host->GetID()).second) {
      host->AddObserver(this);
      host->GetRendererInterface()->SetWebKitSharedTimersSuspended(true);
    }
  }
}
void SuspendedProcessWatcherOHOS::ResumeWebKitShardTimersFromOHOS() {
  for (auto id : pause_processes_) {
    content::RenderProcessHost* host = content::RenderProcessHost::FromID(id);
    if (!host) {
      LOG(INFO) << "ResumeWebKitShardTimersFromOHOS host is nullptr.";
      continue;
    }
    host->RemoveObserver(this);
    host->GetRendererInterface()->SetWebKitSharedTimersSuspended(false);
  }
  pause_processes_.clear();
}

std::shared_ptr<SuspendedProcessWatcherOHOS>
SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance() {
  static std::shared_ptr<SuspendedProcessWatcherOHOS> content_view_ =
      std::make_shared<SuspendedProcessWatcherOHOS>();
  return content_view_;
}
}  // namespace content
                       