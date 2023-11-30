/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef BACKGROUND_TASK_HOLDER_H_
#define BACKGROUND_TASK_HOLDER_H_

#include "background_task_adapter.h"

namespace performance_manager::mechanism {
class BackgroundTaskHolder {
 public:
  BackgroundTaskHolder() = default;
  BackgroundTaskHolder(const BackgroundTaskHolder& other) = delete;
  BackgroundTaskHolder& operator=(const BackgroundTaskHolder&) = delete;
  ~BackgroundTaskHolder() = default;

  bool MaybeRequestBackgroundRunning(bool running,
                                     OHOS::NWeb::BackgroundModeAdapter bgMode);
};
}  // namespace performance_manager::mechanism

#endif  // BACKGROUND_TASK_HOLDER_H_
