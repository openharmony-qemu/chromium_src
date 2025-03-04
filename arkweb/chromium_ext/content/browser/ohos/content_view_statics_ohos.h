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

#ifndef CONTENT_CONTENT_VIEW_STATICS_OHOS_H_
#define CONTENT_CONTENT_VIEW_STATICS_OHOS_H_

#include "base/check.h"
#include "base/lazy_instance.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_process_host_observer.h"

namespace content {
class SuspendedProcessWatcherOHOS : public content::RenderProcessHostObserver {
 public:
  SuspendedProcessWatcherOHOS() = default;
  ~SuspendedProcessWatcherOHOS() = default;

  static std::shared_ptr<SuspendedProcessWatcherOHOS>
  GetShradWebKitTimersInstance();

  void PauseWebKitShardTimersFromOHOS();
  void ResumeWebKitShardTimersFromOHOS();

 private:
  std::set<int /* RenderProcessHost id */> pause_processes_;
};
}  // namespace content
#endif
