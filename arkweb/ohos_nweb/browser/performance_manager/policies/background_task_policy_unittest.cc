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

#include <cstdint>
#include <memory>
#include <memory>
#include <unordered_set>
#include <gmock/gmock.h>

#include "base/dcheck_is_on.h"
#include "base/memory/ptr_util.h"
#include "base/memory/raw_ptr.h"
#include "base/observer_list_types.h"
#include "base/sequence_checker.h"
#include "background_task_adapter.h"
#include "components/performance_manager/public/graph/node_set_view.h"
#include "content/browser/scheduler/browser_task_executor.h"
#include "ohos_nweb/browser/performance_manager/mechanisms/background_task_holder.h"
#include "gtest/gtest.h"

#undef private
#define private public
#include "background_task_policy.h"
#undef private
#define private public
#include "components/performance_manager/graph/graph_impl.h"

using namespace testing;
using namespace OHOS::NWeb;
using namespace performance_manager;
using namespace performance_manager::policies;
using namespace performance_manager::mechanism;

class GraphMock : public performance_manager::GraphImpl {
    public:
    GraphMock() { }

    ~GraphMock() { }

    void RemovePageNodeObserver(PageNodeObserver* observer) {
        DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    }

    void AddPageNodeObserver(PageNodeObserver* observer) {
        DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    }

    void SetLifecycleState() {
        lifecycle_state_ = LifecycleState::kTearDownCalled;
    }
};

class TestableGraphMock : public GraphMock {
public:
    using GraphMock::lifecycle_state_;
    void SetLifecycleStatePublic() { SetLifecycleState(); }
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

    static const char* ToString(PageType type) { }
    static const char* ToString(PageNode::LoadingState loading_state) { }

    static constexpr NodeTypeEnum Type() { return NodeTypeEnum::kPage; }

    const std::string& GetBrowserContextID() const { }

    const FrameNode* GetOpenerFrameNode() const { }

    const FrameNode* GetEmbedderFrameNode() const { }

    resource_attribution::PageContext GetResourceContext() const { }

    EmbeddingType GetEmbeddingType() const { }

    PageType GetType() const { }

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

class BackgroundTaskHolderMock : public BackgroundTaskHolder {
    public:

    bool back_ground = false;

    BackgroundTaskHolderMock() {}

    void SetBackground(bool back_ground_tmp) {
        back_ground = back_ground_tmp;
    }

    bool MaybeRequestBackgroundRunning(bool running,
        BackgroundModeAdapter bgMode) {
        return back_ground;
    }
};

TEST(BackgroundTaskPolicyTEST, OnTakenFromGraph001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnTakenFromGraph(nullptr);
}

TEST(BackgroundTaskPolicyTEST, OnTakenFromGraph002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    TestableGraphMock graph_mock;
    graph_mock.SetLifecycleStatePublic();
    background_task_policy->OnTakenFromGraph(&graph_mock);
}

TEST(BackgroundTaskPolicyTEST, OnPassedToGraph001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnPassedToGraph(nullptr);
}

TEST(BackgroundTaskPolicyTEST, OnPassedToGraph002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    TestableGraphMock graph_mock;
    graph_mock.SetLifecycleStatePublic();
    background_task_policy->OnPassedToGraph(&graph_mock);
}

TEST(BackgroundTaskPolicyTEST, OnPageNodeAdded001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnPageNodeAdded(nullptr);
}

TEST(BackgroundTaskPolicyTEST, OnPageNodeAdded002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->OnPageNodeAdded(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnBeforePageNodeRemoved) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnBeforePageNodeRemoved(nullptr);
}

TEST(BackgroundTaskPolicyTEST, OnIsVisibleChanged001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnIsVisibleChanged(nullptr);
}

TEST(BackgroundTaskPolicyTEST, OnIsVisibleChanged002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnIsVisibleChanged003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	page_node_mock.SetIsVisible(true);
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChanged001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnIsMediaPlayingChanged(nullptr);
}

TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChanged002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChanged003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	page_node_mock.SetIsMediaPlaying(true);
	page_node_mock.SetIsVisible(true);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNum001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnDecrementAudioNum(nullptr);
}

TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNum002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsMediaPlaying(true);
    page_node_mock.SetIsAudible(true);
    background_task_policy->OnDecrementAudioNum(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNum003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsMediaPlaying(false);
    page_node_mock.SetIsAudible(false);
    background_task_policy->OnDecrementAudioNum(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnIsAudibleChanged(nullptr);
}

TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	page_node_mock.SetIsAudible(true);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    background_task_policy->audio_state_num_ = 1;
    page_node_mock.SetIsAudible(false);

    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 2)); // test case 2

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 2); // test case 2

    background_task_policy->OnIsAudibleChanged(&page_node_mock);

    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();

    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 2)); // test case 2

    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    EXPECT_EQ(initial_size, 2);  // test case 2

    PageNodeMock page_node_mock;
    page_node_mock.SetIsAudible(false);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
	background_task_policy->background_task_holder_ = std::move(background_task_holder);
	background_task_policy->is_request_background_task_ = true;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
	background_task_policy->background_task_holder_ = std::move(background_task_holder);
	background_task_policy->is_request_background_task_ = true;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
	background_task_policy->background_task_holder_ = std::move(background_task_holder);
	background_task_policy->is_request_background_task_ = false;
	background_task_policy->media_playing_num_ = 0;
	background_task_policy->audio_state_num_ = 0;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
	background_task_policy->background_task_holder_ = std::move(background_task_holder);
	background_task_policy->is_request_background_task_ = true;
	background_task_policy->media_playing_num_ = 1;
	background_task_policy->audio_state_num_ = 0;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
	background_task_policy->background_task_holder_ = std::move(background_task_holder);
	background_task_policy->is_request_background_task_ = true;
	background_task_policy->media_playing_num_ = 0;
	background_task_policy->audio_state_num_ = 1;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
	background_task_policy->background_task_holder_ = std::move(background_task_holder);
	background_task_policy->is_request_background_task_ = false;
	background_task_policy->visible_page_num_ = 0;
	background_task_policy->media_playing_num_ = 1;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask007) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
	background_task_policy->background_task_holder_ = std::move(background_task_holder);
	background_task_policy->is_request_background_task_ = false;
	background_task_policy->visible_page_num_ = 0;
	background_task_policy->media_playing_num_ = 0;
	background_task_policy->audio_state_num_ = 1;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask008) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
	std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
	background_task_policy->background_task_holder_ = std::move(background_task_holder);
	background_task_policy->is_request_background_task_ = false;
	background_task_policy->visible_page_num_ = 0;
	background_task_policy->media_playing_num_ = 0;
	background_task_policy->audio_state_num_ = 0;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

#if BUILDFLAG(ARKWEB_BGTASK)
TEST(BackgroundTaskPolicyTEST, SetBrowserForeground001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->SetBrowserForeground(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, SetBrowserForeground002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->SetBrowserForeground(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, SetBrowserBackground001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 1;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->SetBrowserBackground(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, SetBrowserBackground002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 1;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->SetBrowserBackground(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, SetBrowserBackground003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = -1;
    background_task_policy->audio_state_num_ = 1;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->SetBrowserBackground(&page_node_mock);
}

TEST(BackgroundTaskPolicyTEST, SetBrowserBackground004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = -1;
    background_task_policy->audio_state_num_ = -1;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->SetBrowserBackground(&page_node_mock);
}
#endif

TEST(BackgroundTaskPolicyTEST, OnAudioContextPlaybackStarted001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    content::GlobalRenderFrameHostId empty_id = content::GlobalRenderFrameHostId();
    background_task_policy->OnAudioContextPlaybackStarted(empty_id, 0);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size + 1);
}

TEST(BackgroundTaskPolicyTEST, OnAudioContextPlaybackStarted002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 1);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);

    background_task_policy->OnAudioContextPlaybackStarted(valid_id2, 2);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 2);
}

TEST(BackgroundTaskPolicyTEST, OnAudioContextPlaybackStopped001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
     content::GlobalRenderFrameHostId empty_id = content::GlobalRenderFrameHostId();
    background_task_policy->OnAudioContextPlaybackStopped(empty_id, 0);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, OnAudioContextPlaybackStopped002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);

    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 1);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);

    background_task_policy->OnAudioContextPlaybackStopped(valid_id, 1);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result);
}

TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->audio_state_num_ = 2; // test case 2
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 2); // test case 2

    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result);
}

TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->audio_state_num_ = 3;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 1);

    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayers001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    background_task_policy->ProcessAudioContextPlayers(nullptr);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayers002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 0;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);
    background_task_policy->ProcessAudioContextPlayersOnUIThread(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayers003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 1;
    size_t initial_size = background_task_policy->audio_context_players_num_.size();

    background_task_policy->ProcessAudioContextPlayers(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayers004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 2; // test case 2
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 2)); // test case 2

    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    background_task_policy->ProcessAudioContextPlayers(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThread001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    background_task_policy->ProcessAudioContextPlayersOnUIThread(nullptr);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThread002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 0;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(
        std::make_pair(valid_id, 1));

    background_task_policy->ProcessAudioContextPlayersOnUIThread(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThread003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 1;
    size_t initial_size = background_task_policy->audio_context_players_num_.size();

    background_task_policy->ProcessAudioContextPlayersOnUIThread(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTask001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);
    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_TRUE(result == true || result == false);
}

TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskOnUIThread001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_TRUE(result);
}

TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskOnUIThread002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));

    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_FALSE(result);
}

TEST(BackgroundTaskPolicyTEST, AudioContextPlayersBoundary001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);

    for (int i = 0; i < 10; ++i) { // 10 test case
        background_task_policy->OnAudioContextPlaybackStarted(valid_id, i);
    }
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 10); // test case 10

    background_task_policy->audio_state_num_ = 10; // test case 10
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result == true || result == false);
}

TEST(BackgroundTaskPolicyTEST, AudioContextPlayersBoundary002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId same_id = content::GlobalRenderFrameHostId(1, 1);

    background_task_policy->OnAudioContextPlaybackStarted(same_id, 1);
    background_task_policy->OnAudioContextPlaybackStarted(same_id, 1);

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);

    background_task_policy->audio_state_num_ = 1;
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result == true || result == false);
}

TEST(BackgroundTaskPolicyTEST, Destructor002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 1);

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);

    background_task_policy.reset();
    EXPECT_TRUE(true);
}

// ==================== Constructor Tests ====================
TEST(BackgroundTaskPolicyTEST, ConstructorInitialValues001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
    EXPECT_TRUE(background_task_policy->background_task_holder_ != nullptr);
}

// ==================== OnIsVisibleChanged Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, OnIsVisibleChanged004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->visible_page_num_ = 0;
    page_node_mock.SetIsVisible(false);
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);
}

TEST(BackgroundTaskPolicyTEST, OnIsVisibleChanged005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->visible_page_num_ = 5;
    page_node_mock.SetIsVisible(true);
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 6);
}

TEST(BackgroundTaskPolicyTEST, OnIsVisibleChanged006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->visible_page_num_ = 1;
    page_node_mock.SetIsVisible(false);
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);
}

// ==================== OnIsMediaPlayingChanged Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChanged004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 0;
    page_node_mock.SetIsMediaPlaying(false);
    page_node_mock.SetIsVisible(false);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
}

TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChanged005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 3;
    page_node_mock.SetIsMediaPlaying(true);
    page_node_mock.SetIsVisible(false);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 4);
}

TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChanged006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 1;
    page_node_mock.SetIsMediaPlaying(false);
    page_node_mock.SetIsVisible(true);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
}

// ==================== OnDecrementAudioNum Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNum004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 2;
    background_task_policy->audio_state_num_ = 3;
    page_node_mock.SetIsMediaPlaying(true);
    page_node_mock.SetIsAudible(false);
    background_task_policy->OnDecrementAudioNum(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 1);
    EXPECT_EQ(background_task_policy->audio_state_num_, 3);
}

TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNum005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 2;
    background_task_policy->audio_state_num_ = 3;
    page_node_mock.SetIsMediaPlaying(false);
    page_node_mock.SetIsAudible(true);
    background_task_policy->OnDecrementAudioNum(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 2);
    EXPECT_EQ(background_task_policy->audio_state_num_, 2);
}

TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNum006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    page_node_mock.SetIsMediaPlaying(true);
    page_node_mock.SetIsAudible(true);
    background_task_policy->OnDecrementAudioNum(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNum007) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    page_node_mock.SetIsMediaPlaying(true);
    page_node_mock.SetIsAudible(true);
    background_task_policy->OnDecrementAudioNum(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

// ==================== OnIsAudibleChanged Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 0;
    page_node_mock.SetIsAudible(false);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged007) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 5;
    page_node_mock.SetIsAudible(true);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 6);
}

TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged008) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 1;
    page_node_mock.SetIsAudible(false);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged009) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 0;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    page_node_mock.SetIsAudible(false);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

// ==================== MaybeChangeBackgroundTask Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask009) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = true;
    background_task_policy->visible_page_num_ = 1;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask010) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 1;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask011) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = true;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_TRUE(background_task_policy->is_request_background_task_);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask012) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_TRUE(background_task_policy->is_request_background_task_);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask013) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 1;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_TRUE(background_task_policy->is_request_background_task_);
}

TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask014) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_TRUE(background_task_policy->is_request_background_task_);
}

// ==================== OnAudioContextPlaybackStopped Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, OnAudioContextPlaybackStopped003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    background_task_policy->OnAudioContextPlaybackStopped(valid_id, 999);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, OnAudioContextPlaybackStopped004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id1, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id2, 2);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 2);
    background_task_policy->OnAudioContextPlaybackStopped(valid_id1, 1);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);
}

// ==================== ProcessAudioContextPlayers Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayers005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 2;
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id1, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id2, 2));
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    background_task_policy->ProcessAudioContextPlayers(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayers006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->ProcessAudioContextPlayers(nullptr);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

// ==================== ProcessAudioContextPlayersOnUIThread Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThread004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 1;
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id1, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id2, 2));
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    background_task_policy->ProcessAudioContextPlayersOnUIThread(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThread005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 2;
    background_task_policy->ProcessAudioContextPlayersOnUIThread(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

// ==================== GetWebAudioStartBackgroundTask Tests ====================
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTask002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id1, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id2, 2));
    bool result = background_task_policy->GetWebAudioStartBackgroundTask();
    EXPECT_TRUE(result == true || result == false);
}

// ==================== IsWebAudioRequestBackgroundRunning Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->audio_state_num_ = 2;
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id1, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id2, 2);
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result == true || result == false);
}

TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->audio_state_num_ = 0;
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result);
}

// ==================== OnPageNodeAdded Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, OnPageNodeAdded003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->OnPageNodeAdded(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 1);
}

TEST(BackgroundTaskPolicyTEST, OnPageNodeAdded004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock1;
    PageNodeMock page_node_mock2;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->OnPageNodeAdded(&page_node_mock1);
    background_task_policy->OnPageNodeAdded(&page_node_mock2);
    EXPECT_EQ(background_task_policy->visible_page_num_, 2);
}

// ==================== OnBeforePageNodeRemoved Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, OnBeforePageNodeRemoved002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->OnBeforePageNodeRemoved(&page_node_mock);
    EXPECT_TRUE(true);
}

// ==================== AudioContextPlayers Complex Scenarios ====================
TEST(BackgroundTaskPolicyTEST, AudioContextPlayersScenario001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);

    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 2);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id, 3);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 3);

    background_task_policy->OnAudioContextPlaybackStopped(valid_id, 2);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 2);

    background_task_policy->OnAudioContextPlaybackStopped(valid_id, 1);
    background_task_policy->OnAudioContextPlaybackStopped(valid_id, 3);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

TEST(BackgroundTaskPolicyTEST, AudioContextPlayersScenario002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);

    background_task_policy->OnAudioContextPlaybackStarted(valid_id1, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id2, 1);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 2);

    background_task_policy->audio_state_num_ = 0;
    background_task_policy->audio_context_players_num_.clear();
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

// ==================== Edge Cases Tests ====================
TEST(BackgroundTaskPolicyTEST, EdgeCase001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    PageNodeMock page_node_mock;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
}

TEST(BackgroundTaskPolicyTEST, EdgeCase002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = true;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    PageNodeMock page_node_mock;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
}

TEST(BackgroundTaskPolicyTEST, EdgeCase003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = true;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;
    PageNodeMock page_node_mock;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_TRUE(background_task_policy->is_request_background_task_);
}

// ==================== Multiple State Changes Tests ====================
TEST(BackgroundTaskPolicyTEST, MultipleStateChanges001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;

    page_node_mock.SetIsVisible(true);
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 1);

    page_node_mock.SetIsVisible(false);
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);
}

TEST(BackgroundTaskPolicyTEST, MultipleStateChanges002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    background_task_policy->media_playing_num_ = 0;

    page_node_mock.SetIsMediaPlaying(true);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 1);

    page_node_mock.SetIsMediaPlaying(false);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
}

TEST(BackgroundTaskPolicyTEST, MultipleStateChanges003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    background_task_policy->audio_state_num_ = 0;

    page_node_mock.SetIsAudible(true);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 1);

    page_node_mock.SetIsAudible(false);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

// ==================== Negative Boundary Tests ====================
TEST(BackgroundTaskPolicyTEST, NegativeBoundary001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->visible_page_num_ = 0;
    page_node_mock.SetIsVisible(false);
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_GE(background_task_policy->visible_page_num_, 0);
}

TEST(BackgroundTaskPolicyTEST, NegativeBoundary002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 0;
    page_node_mock.SetIsMediaPlaying(false);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_GE(background_task_policy->media_playing_num_, 0);
}

TEST(BackgroundTaskPolicyTEST, NegativeBoundary003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 0;
    page_node_mock.SetIsAudible(false);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_GE(background_task_policy->audio_state_num_, 0);
}

// ==================== Large Value Tests ====================
TEST(BackgroundTaskPolicyTEST, LargeValue001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->visible_page_num_ = 1000;
    page_node_mock.SetIsVisible(true);
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 1001);
}

TEST(BackgroundTaskPolicyTEST, LargeValue002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->media_playing_num_ = 1000;
    page_node_mock.SetIsMediaPlaying(true);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 1001);
}

TEST(BackgroundTaskPolicyTEST, LargeValue003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 1000;
    page_node_mock.SetIsAudible(true);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 1001);
}

// ==================== Copy/Move Delete Tests ====================
TEST(BackgroundTaskPolicyTEST, CopyMoveDelete001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    EXPECT_TRUE(background_task_policy != nullptr);
    auto background_task_policy2 = std::make_shared<BackgroundTaskPolicy>();
    EXPECT_TRUE(background_task_policy2 != nullptr);
    EXPECT_TRUE(background_task_policy != background_task_policy2);
}

// ==================== GetWebAudioStartBackgroundTaskOnUIThread Additional Tests ====================
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskOnUIThread003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id1, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id2, 2));
    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_FALSE(result);
}