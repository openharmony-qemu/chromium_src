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

#include "ohos_bfcache_policy.h"

#include "base/functional/bind.h"
#include "base/memory/memory_pressure_listener.h"
#include "base/notreached.h"
#include "base/task/task_traits.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "components/performance_manager/public/features.h"
#include "components/performance_manager/public/graph/frame_node.h"
#include "components/performance_manager/public/graph/page_node.h"
#include "components/performance_manager/public/web_contents_proxy.h"
#include "content/public/browser/back_forward_cache.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"

#include "base/logging.h"

namespace performance_manager::policies {
bool PageMightHaveFramesInBFCache(const PageNode* page_node) {
  // TODO(crbug.com/1211368): Use PageState when that actually works.
  auto main_frame_nodes = page_node->GetMainFrameNodes();
  if (main_frame_nodes.size() == 1)
    return false;
  for (const auto* main_frame_node : main_frame_nodes) {
    if (!main_frame_node->IsCurrent())
      return true;
  }
  return false;
}

using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;

void MaybeFlushBFCacheOnUIThread(const WebContentsProxy& contents_proxy,
                                 MemoryPressureLevel memory_pressure_level) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  content::WebContents* const content = contents_proxy.Get();
  if (!content)
    return;

  int cache_size = -1;
  switch (memory_pressure_level) {
    case MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE:
      LOG(DEBUG) << "OHOSBFCache::MaybeFlushBFCacheOnUIThread" << " The value of memory pressure level is: MEMORY_PRESSURE_LEVEL_MODERATE";
      cache_size = 1;
      break;
    case MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_CRITICAL:
      LOG(DEBUG) << "OHOSBFCache::MaybeFlushBFCacheOnUIThread" << " The value of memory pressure level is: MEMORY_PRESSURE_LEVEL_CRITICAL";
      cache_size = 0;
      break;
    default:
      NOTREACHED();
  }
  // Do not flush BFCache if cache_size is negative (such as -1).
  if (cache_size < 0)
    return;

  // Do not flush the BFCache if there's a pending navigation as this could stop
  // it.
  // TODO(sebmarchand): Check if this is really needed.
  auto& navigation_controller = content->GetController();
  if (!navigation_controller.GetPendingEntry()) {
    LOG(DEBUG) << "OHOSBFCache::MaybeFlushBFCacheOnUIThread" << " Start to prune cache is: " << cache_size;
    navigation_controller.GetBackForwardCache().Prune(cache_size);
  }
}

void OHOSBFCachePolicy::MaybeFlushBFCache(
    const PageNode* page_node,
    MemoryPressureLevel memory_pressure_level) {
  DCHECK(page_node);
  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&MaybeFlushBFCacheOnUIThread,
                     page_node->GetContentsProxy(), memory_pressure_level));
}

void OHOSBFCachePolicy::OnPassedToGraph(Graph* graph) {
  DCHECK(graph->HasOnlySystemNode());
  graph_ = graph;
  graph_->AddSystemNodeObserver(this);
}

void OHOSBFCachePolicy::OnTakenFromGraph(Graph* graph) {
  graph_->RemoveSystemNodeObserver(this);
  graph_ = nullptr;
}

void OHOSBFCachePolicy::OnMemoryPressure(MemoryPressureLevel new_level) {
  // This shouldn't happen but add the check anyway in case the API changes.
  if (new_level == MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_NONE) {
    return;
  }

  // Apply the cache limit to all pages.
  for (auto* page_node : graph_->GetAllPageNodes()) {
    if (page_node->GetPageState() == PageNode::PageState::kActive &&
        PageMightHaveFramesInBFCache(page_node)) {
      MaybeFlushBFCache(page_node, new_level);
    }
  }
}

}  // namespace performance_manager::policies
