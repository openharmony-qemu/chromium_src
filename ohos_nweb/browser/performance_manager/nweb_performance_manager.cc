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

#include "nweb_performance_manager.h"
#include <memory>
#include "ohos_nweb/browser/performance_manager/policies/background_task_policy.h"
#include "ohos_nweb/browser/performance_manager/policies/ohos_bfcache_policy.h"

namespace OHOS::NWeb {
// static
void NwebPerformanceManager::CreatePoliciesAndDecorators(
    performance_manager::Graph* graph) {
  graph->PassToGraph(
      std::make_unique<performance_manager::policies::BackgroundTaskPolicy>());
  graph->PassToGraph(
      std::make_unique<performance_manager::policies::OHOSBFCachePolicy>());
}
}  // namespace OHOS::NWeb