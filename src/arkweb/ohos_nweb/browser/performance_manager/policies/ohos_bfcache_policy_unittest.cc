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

class PageNodeMock : public PageNode {
public:

    bool is_visible = false;

    bool is_audible = false;

    bool is_media_playing = false;

    int frame_node_flag = 0;

    int main_frame_nodes_count = 0;

    PageNodeMock() { }

    static const char* ToString(PageNode::EmbeddingType embedding_type) { }

    static const char* ToString(performance_manager::PageType type) { }
    static const char* ToString(PageNode::LoadingState loading_state) { }

    static constexpr NodeTypeEnum Type() { return NodeTypeEnum::kPage; }

    const std::string& GetBrowserContextID() const { }

    const FrameNode* GetOpenerFrameNode() const { }

    const FrameNode* GetEmbedderFrameNode() const { }

    resource_attribution::PageContext GetResourceContext() const { }

    EmbeddingType GetEmbeddingType() const { }

    performance_manager::PageType GetType() const { }

    bool IsFocused() const { }

    void SetIsVisible(bool visibility) {
        is_visible = visibility;
    }

    bool IsVisible() const { 
        return is_visible;
    }

    base::TimeDelta GetTimeSinceLastVisibilityChange() const { }

    void SetIsAudible(bool audible) {
        is_audible = audible;
    }

    bool IsAudible() const { 
        return is_audible;
    }

    std::optional<base::TimeDelta> GetTimeSinceLastAudibleChange()
        const { }

    bool HasPictureInPicture() const { }

    bool IsOffTheRecord() const { }

    #if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
    void SetIsMediaPlaying(bool media_playing) {
        is_media_playing = media_playing;
    }

    bool IsMediaPlaying() const { 
        return is_media_playing;
    }
    #endif

    LoadingState GetLoadingState() const { }

    ukm::SourceId GetUkmSourceID() const { }

    LifecycleState GetLifecycleState() const { }

    bool IsHoldingWebLock() const { }

    bool IsHoldingIndexedDBLock() const { }

    bool UsesWebRTC() const { }

    int64_t GetNavigationID() const { }

    const std::string& GetContentsMimeType() const { }

    std::optional<blink::mojom::PermissionStatus>
    GetNotificationPermissionStatus() const { }

    base::TimeDelta GetTimeSinceLastNavigation() const { }

    const FrameNode* GetMainFrameNode() const { }

    NodeSetView<const FrameNode*> GetMainFrameNodes() const {
    }

    const GURL& GetMainFrameUrl() const { }

    uint64_t EstimateMainFramePrivateFootprintSize() const { }

    bool HadFormInteraction() const { }

    bool HadUserEdits() const { }

    base::WeakPtr<content::WebContents> GetWebContents() const { }

    uint64_t EstimateResidentSetSize() const { }

    uint64_t EstimatePrivateFootprintSize() const { }

    const void* GetImpl() const { }

    uintptr_t GetImplType() const { }

    NodeState GetNodeState() const { }

    Graph* GetGraph() const { }
};

TEST(OHOS_BFCACHE_POLICY_TEST, OnPassedToGraph) {
  auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
  auto graph_mock = std::make_shared<GraphMock>();
  graph_mock->SetLifecycleState();
  ohos_bfcache_policy->OnPassedToGraph(graph_mock.get());
}

TEST(OHOS_BFCACHE_POLICY_TEST, OnPassedToGraph001) {
  auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
  std::unique_ptr<GraphImpl> graph = std::make_unique<GraphImpl>();
  
  graph->SetUp();
  ohos_bfcache_policy->OnPassedToGraph(graph.get());
  graph->TearDown();
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

TEST(OHOS_BFCACHE_POLICY_TEST, OnMemoryPressure001) {
  auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
  std::unique_ptr<GraphImpl> graph = std::make_unique<GraphImpl>();
  graph->SetUp();
  
  PageNodeMock page_node_mock;
  const size_t page_index = static_cast<size_t>(PageNodeMock::Type());

  SEQUENCE_CHECKER(sequence_checker_);

  base::SequenceChecker sequence_checker_;
  sequence_checker_.DetachFromSequence();
  
  {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    
    graph->nodes_[page_index].insert(&page_node_mock);
    
    ASSERT_FALSE(graph->nodes_[page_index].empty());
  }
  
  ohos_bfcache_policy->OnMemoryPressure(MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE);

  graph->TearDown();
}

TEST(OHOS_BFCACHE_POLICY_TEST, OnMemoryPressure002) {
  auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
  std::unique_ptr<GraphImpl> graph = std::make_unique<GraphImpl>();
  graph->SetUp();

  PageNodeMock page_node_mock;
  const size_t page_index = static_cast<size_t>(PageNodeMock::Type());

  SEQUENCE_CHECKER(sequence_checker_);

  base::SequenceChecker sequence_checker_;
  sequence_checker_.DetachFromSequence();
  
  {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    
    graph->nodes_[page_index].insert(&page_node_mock);
    
    ASSERT_FALSE(graph->nodes_[page_index].empty());
  }

  ohos_bfcache_policy->OnMemoryPressure(MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE);
  graph->TearDown();
}

TEST(OHOS_BFCACHE_POLICY_TEST, OnMemoryPressure003) {
  auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
  std::unique_ptr<GraphImpl> graph = std::make_unique<GraphImpl>();
  graph->SetUp();

  PageNodeMock pageA;
  PageNodeMock pageB;

  const size_t page_index = static_cast<size_t>(PageNodeMock::Type());

  SEQUENCE_CHECKER(sequence_checker_);

  base::SequenceChecker sequence_checker_;
  sequence_checker_.DetachFromSequence();
  
  {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    
    graph->nodes_[page_index].insert(&pageA);
    
    ASSERT_FALSE(graph->nodes_[page_index].empty());
  }

  ohos_bfcache_policy->OnMemoryPressure(MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE);
  graph->TearDown();
}

TEST(OHOS_BFCACHE_POLICY_TEST, OnMemoryPressure004) {
  auto ohos_bfcache_policy = std::make_shared<OHOSBFCachePolicy>();
  std::unique_ptr<GraphImpl> graph = std::make_unique<GraphImpl>();
  graph->SetUp();

  PageNodeMock pageA;
  PageNodeMock pageB;
  PageNodeMock pageC;

  const size_t page_index = static_cast<size_t>(PageNodeMock::Type());

  SEQUENCE_CHECKER(sequence_checker_);

  base::SequenceChecker sequence_checker_;
  sequence_checker_.DetachFromSequence();
  
  {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    
    graph->nodes_[page_index].insert(&pageA);
    
    ASSERT_FALSE(graph->nodes_[page_index].empty());
  }

  ohos_bfcache_policy->OnMemoryPressure(MemoryPressureLevel::MEMORY_PRESSURE_LEVEL_MODERATE);
  graph->TearDown();
}
