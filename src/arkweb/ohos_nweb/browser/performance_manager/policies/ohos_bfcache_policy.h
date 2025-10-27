// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on ohos_bfcache_policy.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#ifndef OHOS_BFCACHE_POLICY_H_
#define OHOS_BFCACHE_POLICY_H_

#include "base/time/time.h"
#include "base/timer/timer.h"
#include "components/performance_manager/public/graph/graph.h"
#include "components/performance_manager/public/graph/system_node.h"

namespace performance_manager::policies {

// Policies that automatically flush the BFCache of pages when the system is
// under memory pressure.
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
};

}  // namespace performance_manager::policies

#endif  // BOHOS_BFCACHE_POLICY_H_
