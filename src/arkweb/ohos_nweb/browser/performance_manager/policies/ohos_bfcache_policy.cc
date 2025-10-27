// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on bfcache_policy.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "ohos_bfcache_policy.h"

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
#include "content/public/browser/back_forward_cache.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"

namespace performance_manager::policies {

namespace {

bool PageMightHaveFramesInBFCache(const PageNode* page_node) {
  // TODO(crbug.com/40182881): Use PageState when that actually works.
  auto main_frame_nodes = page_node->GetMainFrameNodes();
  if (main_frame_nodes.size() == 1) {
    return false;
  }
  for (const FrameNode* main_frame_node : main_frame_nodes) {
    if (!main_frame_node->IsCurrent()) {
      return true;
    }
  }
  return false;
}

using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;

void MaybeFlushBFCacheOnUIThread(base::WeakPtr<content::WebContents> contents,
                                 MemoryPressureLevel memory_pressure_level) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (!contents) {
    return;
  }

  int cache_size = -1;
  switch (memory_pressure_level) {
    case MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE:
      LOG(DEBUG) << "OHOSBFCache::MaybeFlushBFCacheOnUIThread"
                 << " The value of memory pressure level is: "
                    "MEMORY_PRESSURE_LEVEL_MODERATE";
      cache_size = 1;
      break;
    case MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_CRITICAL:
      LOG(DEBUG) << "OHOSBFCache::MaybeFlushBFCacheOnUIThread"
                 << " The value of memory pressure level is: "
                    "MEMORY_PRESSURE_LEVEL_CRITICAL";
      cache_size = 0;
      break;
    default:
      NOTREACHED_IN_MIGRATION();
  }
  // Do not flush BFCache if cache_size is negative (such as -1).
  if (cache_size < 0) {
    return;
  }

  // Do not flush the BFCache if there's a pending navigation as this could stop
  // it.
  // TODO(sebmarchand): Check if this is really needed.
  auto& navigation_controller = contents->GetController();
  if (!navigation_controller.GetPendingEntry()) {
    LOG(DEBUG) << "OHOSBFCache::MaybeFlushBFCacheOnUIThread"
               << " Start to prune cache is: " << cache_size;
    navigation_controller.GetBackForwardCache().Prune(cache_size);
  }
}

}  // namespace

void OHOSBFCachePolicy::MaybeFlushBFCache(
    const PageNode* page_node,
    MemoryPressureLevel memory_pressure_level) {
  DCHECK(page_node);
  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&MaybeFlushBFCacheOnUIThread, page_node->GetWebContents(),
                     memory_pressure_level));
}

void OHOSBFCachePolicy::OnPassedToGraph(Graph* graph) {
  DCHECK(graph->HasOnlySystemNode());
  graph->AddSystemNodeObserver(this);
}

void OHOSBFCachePolicy::OnTakenFromGraph(Graph* graph) {
  graph->RemoveSystemNodeObserver(this);
}

void OHOSBFCachePolicy::OnMemoryPressure(MemoryPressureLevel new_level) {
  // This shouldn't happen but add the check anyway in case the API changes.
  if (new_level == MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_NONE) {
    return;
  }

  auto* owningGraph = GetOwningGraph();
  if (!owningGraph) {
    LOG(ERROR) << "GetOwningGraph() returned null";
    return;
  }

  // Apply the cache limit to all pages.
  for (const PageNode* page_node : GetOwningGraph()->GetAllPageNodes()) {
    if (PageMightHaveFramesInBFCache(page_node)) {
      MaybeFlushBFCache(page_node, new_level);
    }
  }
}

}  // namespace performance_manager::policies
