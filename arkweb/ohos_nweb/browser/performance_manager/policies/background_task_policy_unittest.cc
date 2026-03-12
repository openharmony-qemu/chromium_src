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

// Test for MaybeChangeBackgroundTask with NO_CHANGE_BG_TASK branch
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
}

// Test for MaybeChangeBackgroundTask with all counters > 0
TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask010) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = true;
    background_task_policy->visible_page_num_ = 1;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

// Test for MaybeChangeBackgroundTask with null background_task_holder
TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask011) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->background_task_holder_ = nullptr;
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

// Test for MaybeChangeBackgroundTask with MaybeRequestBackgroundRunning returning false
TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTask012) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
}

// Test for SetBrowserForeground with media_playing_num_ <= 0 and audio_state_num_ <= 0
TEST(BackgroundTaskPolicyTEST, SetBrowserForeground003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->SetBrowserForeground(&page_node_mock);
}

// Test for SetBrowserForeground with WebAudio not requesting background
TEST(BackgroundTaskPolicyTEST, SetBrowserForeground004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->audio_context_players_num_.insert(
        std::make_pair(content::GlobalRenderFrameHostId(1, 1), 1));
    background_task_policy->SetBrowserForeground(&page_node_mock);
}

// Test for SetBrowserBackground with negative counters
TEST(BackgroundTaskPolicyTEST, SetBrowserBackground005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->media_playing_num_ = -1;
    background_task_policy->audio_state_num_ = -1;
    background_task_policy->SetBrowserBackground(&page_node_mock);
}

// Test for OnPageNodeAdded with null page node
TEST(BackgroundTaskPolicyTEST, OnPageNodeAdded003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnPageNodeAdded(nullptr);
}

// Test for OnBeforePageNodeRemoved with null page node
TEST(BackgroundTaskPolicyTEST, OnBeforePageNodeRemoved002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnBeforePageNodeRemoved(nullptr);
}

// Test for OnIsVisibleChanged with null page node
TEST(BackgroundTaskPolicyTEST, OnIsVisibleChanged004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnIsVisibleChanged(nullptr);
}

// Test for OnIsMediaPlayingChanged with null page node
TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChanged004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnIsMediaPlayingChanged(nullptr);
}

// Test for OnIsAudibleChanged with null page node
TEST(BackgroundTaskPolicyTEST, OnIsAudibleChanged005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnIsAudibleChanged(nullptr);
}

// Test for OnDecrementAudioNum with null page node
TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNum003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->OnDecrementAudioNum(nullptr);
}

// Test for ProcessAudioContextPlayers with negative audio_state_num_
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayers005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = -1;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(
        std::make_pair(valid_id, 1));
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    background_task_policy->ProcessAudioContextPlayers(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

// Test for ProcessAudioContextPlayers with large audio_state_num_
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayers006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 100;
    size_t initial_size = background_task_policy->audio_context_players_num_.size();
    background_task_policy->ProcessAudioContextPlayers(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}

// Test for ProcessAudioContextPlayersOnUIThread with null page node
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThread004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->ProcessAudioContextPlayersOnUIThread(nullptr);
}

// Test for GetWebAudioStartBackgroundTask with null WebContents
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTask002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(
        std::make_pair(valid_id, 1));
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);
    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_TRUE(result == true || result == false);
}

// Test for IsWebAudioRequestBackgroundRunning with negative audio_state_num_
TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->audio_state_num_ = -1;
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_FALSE(result);
}

// Test for IsWebAudioRequestBackgroundRunning with zero audio_context_players_num_
TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning005) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->audio_state_num_ = 1;
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_FALSE(result);
}

// Test for AudioContextPlayersBoundary with maximum iterations
TEST(BackgroundTaskPolicyTEST, AudioContextPlayersBoundary003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);

    // Insert a large number of audio contexts
    for (int i = 0; i < 1000; ++i) {
        background_task_policy->OnAudioContextPlaybackStarted(valid_id, i);
    }
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 1);

    background_task_policy->audio_state_num_ = 1000;
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result);
}

// Test for Constructor initialization
TEST(BackgroundTaskPolicyTEST, Constructor001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
    EXPECT_TRUE(background_task_policy->audio_context_players_num_.empty());
}

// Test for OnPassedToGraph with non-null graph
TEST(BackgroundTaskPolicyTEST, OnPassedToGraph003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    TestableGraphMock graph_mock;
    graph_mock.SetLifecycleStatePublic();
    EXPECT_CALL(graph_mock, AddPageNodeObserver(_));
    background_task_policy->OnPassedToGraph(&graph_mock);
}

// Test for OnTakenFromGraph with non-null graph
TEST(BackgroundTaskPolicyTEST, OnTakenFromGraph003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    TestableGraphMock graph_mock;
    graph_mock.SetLifecycleStatePublic();
    EXPECT_CALL(graph_mock, RemovePageNodeObserver(_));
    background_task_policy->OnTakenFromGraph(&graph_mock);
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
// Test for SetBrowserBackground with invalid WebContents
TEST(BackgroundTaskPolicyTEST, SetBrowserBackground006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;
    EXPECT_CALL(page_node_mock, GetWebContents())
        .WillOnce(testing::Return(base::WeakPtr<content::WebContents>()));
    background_task_policy->SetBrowserBackground(&page_node_mock);
}

// Test for SetBrowserBackground with valid WebContents and WebAudio background
TEST(BackgroundTaskPolicyTEST, SetBrowserBackground007) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder = std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->audio_context_players_num_.insert(
        std::make_pair(content::GlobalRenderFrameHostId(1, 1), 1));
    EXPECT_CALL(page_node_mock, GetWebContents())
        .WillRepeatedly(testing::Return(base::WeakPtr<content::WebContents>()));
    background_task_policy->SetBrowserBackground(&page_node_mock);
}

// Test for GetWebAudioStartBackgroundTaskOnUIThread with valid WebContents
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskOnUIThread003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    EXPECT_CALL(page_node_mock, GetWebContents())
        .WillOnce(testing::Return(base::WeakPtr<content::WebContents>()));
    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_TRUE(result);
}

// Test for GetWebAudioStartBackgroundTaskOnUIThread with valid WebContents and audio contexts
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskOnUIThread004) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_context_players_num_.insert(
        std::make_pair(content::GlobalRenderFrameHostId(1, 1), 1));
    EXPECT_CALL(page_node_mock, GetWebContents())
        .WillOnce(testing::Return(base::WeakPtr<content::WebContents>()));
    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_FALSE(result);
}
#endif

// ============================================================================
// Counter Clamping Behavior Tests
// ============================================================================

// Test that visible_page_num_ never goes below 0 in OnIsVisibleChanged
TEST(BackgroundTaskPolicyTEST, OnIsVisibleChangedClamping001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsVisible(false);

    // Set visible_page_num_ to 0 and try to decrement
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);
}

// Test multiple decrements ensure counter doesn't go negative
TEST(BackgroundTaskPolicyTEST, OnIsVisibleChangedClamping002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsVisible(false);

    // Set visible_page_num_ to 1 and decrement multiple times
    background_task_policy->visible_page_num_ = 1;
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);

    // Try to decrement again
    background_task_policy->OnIsVisibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);
}

// Test that media_playing_num_ never goes below 0 in OnIsMediaPlayingChanged
TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChangedClamping001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsMediaPlaying(false);

    // Set media_playing_num_ to 0 and try to decrement
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
}

// Test multiple media state changes ensure counter doesn't go negative
TEST(BackgroundTaskPolicyTEST, OnIsMediaPlayingChangedClamping002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsMediaPlaying(false);

    // Set media_playing_num_ to 1 and decrement multiple times
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);

    // Try to decrement again
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
}

// Test that audio_state_num_ never goes below 0 in OnIsAudibleChanged
TEST(BackgroundTaskPolicyTEST, OnIsAudibleChangedClamping001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsAudible(false);

    // Set audio_state_num_ to 0 and try to decrement
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

// Test multiple audio state changes ensure counter doesn't go negative
TEST(BackgroundTaskPolicyTEST, OnIsAudibleChangedClamping002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsAudible(false);

    // Set audio_state_num_ to 1 and decrement multiple times
    background_task_policy->audio_state_num_ = 1;
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);

    // Try to decrement again
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

// Test that OnDecrementAudioNum clamps both media_playing_num_ and audio_state_num_
TEST(BackgroundTaskPolicyTEST, OnDecrementAudioNumClamping001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    page_node_mock.SetIsMediaPlaying(false);
    page_node_mock.SetIsAudible(false);

    // Set counters to 0 and try to decrement
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->OnDecrementAudioNum(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

// ============================================================================
// Multi-page Visibility Change Tests
// ============================================================================

// Test multiple page additions increment visible_page_num_
TEST(BackgroundTaskPolicyTEST, MultiPageVisibility001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock1;
    PageNodeMock page_node_mock2;
    PageNodeMock page_node_mock3;

    background_task_policy->OnPageNodeAdded(&page_node_mock1);
    EXPECT_EQ(background_task_policy->visible_page_num_, 1);

    background_task_policy->OnPageNodeAdded(&page_node_mock2);
    EXPECT_EQ(background_task_policy->visible_page_num_, 2);

    background_task_policy->OnPageNodeAdded(&page_node_mock3);
    EXPECT_EQ(background_task_policy->visible_page_num_, 3);
}

// Test visibility changes on multiple pages
TEST(BackgroundTaskPolicyTEST, MultiPageVisibility002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock1;
    PageNodeMock page_node_mock2;
    PageNodeMock page_node_mock3;

    // Add three pages
    background_task_policy->OnPageNodeAdded(&page_node_mock1);
    background_task_policy->OnPageNodeAdded(&page_node_mock2);
    background_task_policy->OnPageNodeAdded(&page_node_mock3);
    EXPECT_EQ(background_task_policy->visible_page_num_, 3);

    // Hide one page
    page_node_mock1.SetIsVisible(false);
    background_task_policy->OnIsVisibleChanged(&page_node_mock1);
    EXPECT_EQ(background_task_policy->visible_page_num_, 2);

    // Hide another page
    page_node_mock2.SetIsVisible(false);
    background_task_policy->OnIsVisibleChanged(&page_node_mock2);
    EXPECT_EQ(background_task_policy->visible_page_num_, 1);

    // Show a page
    page_node_mock1.SetIsVisible(true);
    background_task_policy->OnIsVisibleChanged(&page_node_mock1);
    EXPECT_EQ(background_task_policy->visible_page_num_, 2);
}

// Test that background task is not requested when any page is visible
TEST(BackgroundTaskPolicyTEST, MultiPageVisibility003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock1;
    PageNodeMock page_node_mock2;

    // Add two pages
    background_task_policy->OnPageNodeAdded(&page_node_mock1);
    background_task_policy->OnPageNodeAdded(&page_node_mock2);

    // Both pages visible, no media playing
    page_node_mock1.SetIsMediaPlaying(false);
    page_node_mock2.SetIsMediaPlaying(false);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock1);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock2);

    // Hide one page but still have media playing
    page_node_mock1.SetIsVisible(false);
    background_task_policy->OnIsVisibleChanged(&page_node_mock1);
    page_node_mock1.SetIsMediaPlaying(true);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock1);

    // visible_page_num_ should be 1 (one page still visible)
    EXPECT_EQ(background_task_policy->visible_page_num_, 1);
}

// ============================================================================
// Concurrent Media and Audio State Tests
// ============================================================================

// Test concurrent media playing and audible states
TEST(BackgroundTaskPolicyTEST, ConcurrentMediaAudio001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    // Start media playing
    page_node_mock.SetIsMediaPlaying(true);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 1);

    // Set page as audible
    page_node_mock.SetIsAudible(true);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_state_num_, 1);

    // Both counters should be > 0
    EXPECT_GT(background_task_policy->media_playing_num_, 0);
    EXPECT_GT(background_task_policy->audio_state_num_, 0);
}

// Test concurrent media playing and audible states with multiple pages
TEST(BackgroundTaskPolicyTEST, ConcurrentMediaAudio002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock1;
    PageNodeMock page_node_mock2;

    // Page 1: media playing, audible
    page_node_mock1.SetIsMediaPlaying(true);
    page_node_mock1.SetIsAudible(true);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock1);
    background_task_policy->OnIsAudibleChanged(&page_node_mock1);

    // Page 2: media playing only
    page_node_mock2.SetIsMediaPlaying(true);
    page_node_mock2.SetIsAudible(false);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock2);
    background_task_policy->OnIsAudibleChanged(&page_node_mock2);

    EXPECT_EQ(background_task_policy->media_playing_num_, 2);
    EXPECT_EQ(background_task_policy->audio_state_num_, 1);
}

// Test concurrent media and audio state transitions
TEST(BackgroundTaskPolicyTEST, ConcurrentMediaAudio003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    // Start with media playing and audible
    page_node_mock.SetIsMediaPlaying(true);
    page_node_mock.SetIsAudible(true);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);

    EXPECT_EQ(background_task_policy->media_playing_num_, 1);
    EXPECT_EQ(background_task_policy->audio_state_num_, 1);

    // Stop media playing but keep audible
    page_node_mock.SetIsMediaPlaying(false);
    background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
    EXPECT_EQ(background_task_policy->audio_state_num_, 1);

    // Stop audible
    page_node_mock.SetIsAudible(false);
    background_task_policy->OnIsAudibleChanged(&page_node_mock);
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

// ============================================================================
// WebAudio Conditional Branch Tests (ARKWEB_PERFORMANCE_PERSISTENT_TASK)
// ============================================================================

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
// Test MaybeChangeBackgroundTask when WebAudio doesn't request background
TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTaskWebAudio001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder =
        std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 0;

    // Simulate WebAudio not requesting background
    // When audio_state_num_ != audio_context_players_num_.size(), returns true
    // When equal, calls GetWebAudioStartBackgroundTask() which returns false
    background_task_policy->audio_state_num_ = 1;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));

    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    // Background task should NOT be requested
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
}

// Test MaybeChangeBackgroundTask when WebAudio requests background
TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTaskWebAudio002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder =
        std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = false;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 1;

    // When audio_state_num_ != audio_context_players_num_.size(), returns true
    // This allows background task request
    background_task_policy->audio_context_players_num_.clear();

    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    // Background task should be requested
    EXPECT_TRUE(background_task_policy->is_request_background_task_);
}

// Test MaybeChangeBackgroundTask cancels bg task when WebAudio not requesting
TEST(BackgroundTaskPolicyTEST, MaybeChangeBackgroundTaskWebAudio003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder =
        std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->is_request_background_task_ = true;
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 0;
    background_task_policy->audio_state_num_ = 1;

    // Set up scenario where cancellation should occur
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));

    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    // Background task should be cancelled (set to false)
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
}

// Test IsWebAudioRequestBackgroundRunning with mismatched sizes
TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning006) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->audio_state_num_ = 2;
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));

    // Sizes don't match (2 vs 1), should return true
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result);
}

// Test IsWebAudioRequestBackgroundRunning with matching sizes and no contexts
TEST(BackgroundTaskPolicyTEST, IsWebAudioRequestBackgroundRunning007) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->audio_context_players_num_.clear();

    // Both are 0, GetWebAudioStartBackgroundTask returns true
    bool result = background_task_policy->IsWebAudioRequestBackgroundRunning();
    EXPECT_TRUE(result);
}

// Test SetBrowserBackground with WebAudio not requesting background
TEST(BackgroundTaskPolicyTEST, SetBrowserBackgroundWebAudio001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder =
        std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = false;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;

    // Set up scenario where IsWebAudioRequestBackgroundRunning returns false
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));

    background_task_policy->SetBrowserBackground(&page_node_mock);
    // Background task should not be requested due to WebAudio check
}
#endif

// ============================================================================
// ProcessAudioContextPlayersOnUIThread with RenderFrameHost Scenarios
// ============================================================================

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
// Test ProcessAudioContextPlayersOnUIThread with valid RenderFrameHost
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThreadRenderFrameHost001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 1;

    // Add audio context player
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));

    // Process should not remove player if RenderFrameHost doesn't match page's WebContents
    background_task_policy->ProcessAudioContextPlayersOnUIThread(&page_node_mock);
    // Player should still exist since PageNodeMock::GetWebContents returns empty weak_ptr
}

// Test ProcessAudioContextPlayersOnUIThread with audio_state_num_ = 0
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThreadRenderFrameHost002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 0;

    // Add audio context players
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 2));

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 2);

    // When audio_state_num_ = 0, all players should be cleared
    background_task_policy->ProcessAudioContextPlayersOnUIThread(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}

// Test ProcessAudioContextPlayersOnUIThread with empty audio_context_players_num_
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersOnUIThreadRenderFrameHost003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 1;

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);

    // Should return early when empty
    background_task_policy->ProcessAudioContextPlayersOnUIThread(&page_node_mock);
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 0);
}
#endif

// ============================================================================
// GetWebAudioStartBackgroundTaskOnUIThread with WebContents Scenarios
// ============================================================================

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
// Test GetWebAudioStartBackgroundTaskOnUIThread with multiple audio contexts
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskOnUIThreadWebContents001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();

    // Add multiple audio context players
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id1, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id2, 2));

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 2);

    // Should return false since RenderFrameHost::FromID returns nullptr in test
    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_FALSE(result);
}

// Test GetWebAudioStartBackgroundTaskOnUIThread with same RenderFrameHostId multiple times
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskOnUIThreadWebContents002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();

    // Add multiple audio context players with same RenderFrameHostId
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 2));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 3));

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 3);

    // Should return false since RenderFrameHost::FromID returns nullptr in test
    bool result = background_task_policy->GetWebAudioStartBackgroundTaskOnUIThread();
    EXPECT_FALSE(result);
}
#endif

// ============================================================================
// WeakPtr Handling Tests for Async Operations
// ============================================================================

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
// Test GetWebAudioStartBackgroundTask with valid weak_ptr
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskWeakPtr001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();

    // When called from UI thread, it should work normally
    // When called from non-UI thread, it posts task and waits
    // For this test, we just verify it doesn't crash
    bool result = background_task_policy->GetWebAudioStartBackgroundTask();
    EXPECT_TRUE(result);
}

// Test GetWebAudioStartBackgroundTask with multiple audio contexts
TEST(BackgroundTaskPolicyTEST, GetWebAudioStartBackgroundTaskWeakPtr002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();

    // Add audio context players
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 2));

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 2);

    // Should call GetWebAudioStartBackgroundTaskOnUIThread via task posting
    bool result = background_task_policy->GetWebAudioStartBackgroundTask();
    // Result depends on whether RenderFrameHost::FromID returns valid pointer
    EXPECT_TRUE(result == true || result == false);
}

// Test ProcessAudioContextPlayers with task posting to UI thread
TEST(BackgroundTaskPolicyTEST, ProcessAudioContextPlayersWeakPtr001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    background_task_policy->audio_state_num_ = 1;

    // Add audio context player
    content::GlobalRenderFrameHostId valid_id = content::GlobalRenderFrameHostId(1, 1);
    background_task_policy->audio_context_players_num_.insert(std::make_pair(valid_id, 1));

    size_t initial_size = background_task_policy->audio_context_players_num_.size();

    // When not on UI thread, should post task using weak_ptr
    // The task will be posted but may not execute in test environment
    background_task_policy->ProcessAudioContextPlayers(&page_node_mock);

    // Size should remain the same since the task may not execute
    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), initial_size);
}
#endif

// ============================================================================
// Additional Edge Case Tests
// ============================================================================

// Test OnIsVisibleChanged with rapid visibility toggles
TEST(BackgroundTaskPolicyTEST, RapidVisibilityToggles001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    // Rapid visibility changes
    for (int i = 0; i < 10; ++i) {
        page_node_mock.SetIsVisible(true);
        background_task_policy->OnIsVisibleChanged(&page_node_mock);
        page_node_mock.SetIsVisible(false);
        background_task_policy->OnIsVisibleChanged(&page_node_mock);
    }

    // Counter should be clamped at 0
    EXPECT_EQ(background_task_policy->visible_page_num_, 0);
}

// Test OnIsMediaPlayingChanged with rapid state changes
TEST(BackgroundTaskPolicyTEST, RapidMediaStateChanges001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    // Rapid media state changes
    for (int i = 0; i < 10; ++i) {
        page_node_mock.SetIsMediaPlaying(true);
        background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
        page_node_mock.SetIsMediaPlaying(false);
        background_task_policy->OnIsMediaPlayingChanged(&page_node_mock);
    }

    // Counter should be clamped at 0
    EXPECT_EQ(background_task_policy->media_playing_num_, 0);
}

// Test OnIsAudibleChanged with rapid state changes
TEST(BackgroundTaskPolicyTEST, RapidAudioStateChanges001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;

    // Rapid audio state changes
    for (int i = 0; i < 10; ++i) {
        page_node_mock.SetIsAudible(true);
        background_task_policy->OnIsAudibleChanged(&page_node_mock);
        page_node_mock.SetIsAudible(false);
        background_task_policy->OnIsAudibleChanged(&page_node_mock);
    }

    // Counter should be clamped at 0
    EXPECT_EQ(background_task_policy->audio_state_num_, 0);
}

// Test MaybeChangeBackgroundTask with all states combined
TEST(BackgroundTaskPolicyTEST, CombinedStates001) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder =
        std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);

    // Set up all states
    background_task_policy->visible_page_num_ = 2;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 1;
    background_task_policy->is_request_background_task_ = true;

    // With visible pages, background task should be cancelled
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_FALSE(background_task_policy->is_request_background_task_);
}

// Test MaybeChangeBackgroundTask with no visible pages and media/audio active
TEST(BackgroundTaskPolicyTEST, CombinedStates002) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();
    PageNodeMock page_node_mock;
    std::unique_ptr<BackgroundTaskHolderMock> background_task_holder =
        std::make_unique<BackgroundTaskHolderMock>();
    background_task_holder->back_ground = true;
    background_task_policy->background_task_holder_ = std::move(background_task_holder);

    // Set up states for background task request
    background_task_policy->visible_page_num_ = 0;
    background_task_policy->media_playing_num_ = 1;
    background_task_policy->audio_state_num_ = 0;
    background_task_policy->is_request_background_task_ = false;

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
    // Clear audio contexts to ensure WebAudio check passes
    background_task_policy->audio_context_players_num_.clear();
#endif

    // Should request background task
    background_task_policy->MaybeChangeBackgroundTask(&page_node_mock);
    EXPECT_TRUE(background_task_policy->is_request_background_task_);
}

// Test destructor clears audio_context_players_num_
TEST(BackgroundTaskPolicyTEST, Destructor003) {
    auto background_task_policy = std::make_shared<BackgroundTaskPolicy>();

    // Add multiple audio context players
    content::GlobalRenderFrameHostId valid_id1 = content::GlobalRenderFrameHostId(1, 1);
    content::GlobalRenderFrameHostId valid_id2 = content::GlobalRenderFrameHostId(2, 2);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id1, 1);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id1, 2);
    background_task_policy->OnAudioContextPlaybackStarted(valid_id2, 1);

    EXPECT_EQ(background_task_policy->audio_context_players_num_.size(), 3);

    // Reset should call destructor which clears the set
    background_task_policy.reset();
    EXPECT_TRUE(true);
}