// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on bfcache_policy.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include <gmock/gmock.h>

#include "base/functional/bind.h"
#include "base/memory/memory_pressure_listener.h"
#include "base/memory/weak_ptr.h"
#include "base/notreached.h"
#include "base/task/task_traits.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "components/performance_manager/public/features.h"
#include "components/performance_manager/public/graph/frame_node.h"
#include "components/performance_manager/public/graph/page_node.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/browser/renderer_host/back_forward_cache_impl.h"
#include "content/public/browser/back_forward_cache.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "gtest/gtest.h"

#define protected public
#define private public
#include "ohos_bfcache_policy.h"
#include "components/performance_manager/graph/graph_impl.h"

using namespace performance_manager::policies;
using namespace performance_manager;
using namespace content;
using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;

class GraphMock : public performance_manager::GraphImpl {
public:
  GraphMock() { }

  ~GraphMock() { }

  void RemovePageNodeObserver(PageNodeObserver* observer) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  }
  void RemoveSystemNodeObserver(SystemNodeObserver* observer) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  }

  void AddSystemNodeObserver(SystemNodeObserver* observer) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  }

  void AddPageNodeObserver(PageNodeObserver* observer) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  }

  void SetLifecycleState() {
    lifecycle_state_ = LifecycleState::kTearDownCalled;
  }
};

TEST(OHOS_BFCACHE_POLICY_TEST, OnPassedToGraph) {
    auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
    auto graph_mock = std::make_shared<GraphMock>();
    graph_mock->SetLifecycleState();
    ohos_bfcache_policy->OnPassedToGraph(graph_mock.get());
}

TEST(OHOS_BFCACHE_POLICY_TEST, OnTakenFromGraph) {
  auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
  auto graph_mock = std::make_shared<GraphMock>();
  graph_mock->SetLifecycleState();
  ohos_bfcache_policy->OnTakenFromGraph(graph_mock.get());
}

TEST(OHOS_BFCACHE_POLICY_TEST, OnMemoryPressure) {
  auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
  ohos_bfcache_policy->OnMemoryPressure(MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_NONE);
}


