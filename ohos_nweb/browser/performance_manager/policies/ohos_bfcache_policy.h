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

#ifndef OHOS_BFCACHE_POLICY_H_
#define OHOS_BFCACHE_POLICY_H_

#include <memory>

#include "base/memory/raw_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "components/performance_manager/public/graph/graph.h"
#include "components/performance_manager/public/graph/system_node.h"

namespace performance_manager {

namespace mechanism {
class BackgroundTaskHolder;
}  // namespace mechanism
namespace policies {
class OHOSBFCachePolicy : public GraphOwned,
                          public SystemNode::ObserverDefaultImpl {
 public:
  OHOSBFCachePolicy() = default;
  OHOSBFCachePolicy(const OHOSBFCachePolicy&) = delete;
  OHOSBFCachePolicy(OHOSBFCachePolicy&&) = delete;
  OHOSBFCachePolicy& operator=(const OHOSBFCachePolicy&) = delete;
  OHOSBFCachePolicy& operator=(OHOSBFCachePolicy&&) = delete;
  ~OHOSBFCachePolicy() override = default;

 protected:
  using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;

  // Try to flush the BFCache associated with |page_node|. This will be a no-op
  // if there's a pending navigation.
  virtual void MaybeFlushBFCache(const PageNode* page_node,
                                 MemoryPressureLevel memory_pressure_level);

 private:
  // GraphOwned implementation:
  void OnPassedToGraph(Graph* graph) override;
  void OnTakenFromGraph(Graph* graph) override;

  // SystemNodeObserver:
  void OnMemoryPressure(MemoryPressureLevel new_level) override;

  raw_ptr<Graph> graph_;
};
}  // namespace policies
}  // namespace performance_manager
#endif  // BOHOS_BFCACHE_POLICY_H_
