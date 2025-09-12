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

#ifndef COMPONENTS_DOM_DISTILLER_CORE_TASK_TRACKER_UTILS_H_
#define COMPONENTS_DOM_DISTILLER_CORE_TASK_TRACKER_UTILS_H_

#include "base/memory/raw_ptr.h"
#include "build/build_config.h"
#include "build/buildflag.h"
#include "arkweb/build/features/features.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace dom_distiller {

class TaskTracker;
class ViewRequestDelegate;

class ViewRequestDelegateOhos {
 public:
#if BUILDFLAG(ARKWEB_READER_MODE)
  virtual void OnArticleAborted() {}
#endif // ARKWEB_READER_MODE
};

class TaskTrackerUtils {
 public:
  TaskTrackerUtils(TaskTracker* tracker);

#if BUILDFLAG(ARKWEB_READER_MODE)
  void AddViewer(ViewRequestDelegate* delegate);
  void AbortDistill();
#endif // ARKWEB_READER_MODE

 private:
  raw_ptr<TaskTracker> tracker_;
#if BUILDFLAG(ARKWEB_READER_MODE)
  std::vector<std::unique_ptr<ViewRequestDelegate>> viewers_list_;
#endif // ARKWEB_READER_MODE
};
}  // namespace dom_distiller
#endif  // COMPONENTS_DOM_DISTILLER_CORE_TASK_TRACKER_UTILS_H_
