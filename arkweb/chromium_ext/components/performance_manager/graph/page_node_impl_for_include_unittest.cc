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

#include <optional>
#include <string>
#include "components/performance_manager/graph/page_node_impl.h"
#include "base/containers/contains.h"
#include "base/memory/raw_ptr.h"
#include "components/performance_manager/graph/frame_node_impl.h"
#include "components/performance_manager/graph/graph_impl_operations.h"
#include "components/performance_manager/graph/process_node_impl.h"
#include "components/performance_manager/public/freezing/freezing.h"
#include "components/performance_manager/public/graph/page_node.h"
#include "components/performance_manager/test_support/graph_test_harness.h"
#include "components/performance_manager/test_support/mock_graphs.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"

using namespace performance_manager;

class PageNodeImplExtTest : public GraphTestHarness {
};

using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;

class LenientMockObserver : public PageNodeImpl::Observer {
public:
    LenientMockObserver() = default;
    ~LenientMockObserver() override = default;

    MOCK_METHOD(void, OnPageNodeAdded, (const PageNode *), (override));
    MOCK_METHOD(void, OnBeforePageNodeRemoved, (const PageNode *), (override));
    // Note that opener/embedder functionality is actually tested in the
    // FrameNodeImpl and GraphImpl unittests.
    MOCK_METHOD(void, OnOpenerFrameNodeChanged, (const PageNode *, const FrameNode *), (override));
    MOCK_METHOD(void, OnEmbedderFrameNodeChanged, (const PageNode *, const FrameNode *, EmbeddingType), (override));
    MOCK_METHOD(void, OnTypeChanged, (const PageNode *, PageType), (override));
    MOCK_METHOD(void, OnIsFocusedChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnIsVisibleChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnIsAudibleChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnHasPictureInPictureChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnLoadingStateChanged, (const PageNode *, PageNode::LoadingState), (override));
    MOCK_METHOD(void, OnUkmSourceIdChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnPageLifecycleStateChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnPageIsHoldingWebLockChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnPageIsHoldingIndexedDBLockChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnPageUsesWebRTCChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnMainFrameUrlChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnMainFrameDocumentChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnTitleUpdated, (const PageNode *), (override));
    MOCK_METHOD(void, OnFaviconUpdated, (const PageNode *), (override));
    MOCK_METHOD(void, OnHadFormInteractionChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnHadUserEditsChanged, (const PageNode *), (override));
    MOCK_METHOD(void, OnAboutToBeDiscarded, (const PageNode *, const PageNode *), (override));

#if BUILDFLAG(ARKWEB_BGTASK)
    MOCK_METHOD(void, SetBrowserForeground, (const PageNode *), (override));
    MOCK_METHOD(void, SetBrowserBackground, (const PageNode *), (override));
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
    MOCK_METHOD(void, OnDecrementAudioNum, (const PageNode *), (override));
    MOCK_METHOD(void, OnAudioContextPlaybackStarted, (content::GlobalRenderFrameHostId, int), (override));
    MOCK_METHOD(void, OnAudioContextPlaybackStopped, (content::GlobalRenderFrameHostId, int), (override));
    void SetNotifiedAudioContextId(const AudioContextId &audio_context_id)
    {
        audio_context_id_ = audio_context_id;
    }

    const AudioContextId TakeNotifiedAudioContextId()
    {
        return audio_context_id_;
    }

    void SetNotifiedPageNode(const PageNode *page_node)
    {
        notified_page_node_ = page_node;
    }
    
    const PageNode *TakeNotifiedPageNode()
    {
        const PageNode *node = notified_page_node_;
        notified_page_node_ = nullptr;
        return node;
    }
#endif

private:
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
    AudioContextId audio_context_id_;
#endif
    raw_ptr<const PageNode, DanglingUntriaged> notified_page_node_ = nullptr;
};

using MockObserver = ::testing::StrictMock<LenientMockObserver>;

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
TEST_F(PageNodeImplExtTest, SetIsMediaPlaying001)
{
    auto page_node = CreateNode<PageNodeImpl>();
    page_node.get()->SetIsMediaPlaying(true);
    bool is_media_playing;
    is_media_playing = page_node.get()->is_media_playing();
    EXPECT_TRUE(is_media_playing);

    page_node.get()->SetIsMediaPlaying(false);
    is_media_playing = page_node.get()->is_media_playing();
    EXPECT_FALSE(is_media_playing);
}

TEST_F(PageNodeImplExtTest, AudioContextTest) {
  MockObserver obs;
  graph()->AddPageNodeObserver(&obs);

  // Create a page node and expect a matching call to "OnPageNodeAdded".
  EXPECT_CALL(obs, OnPageNodeAdded(_))
      .WillOnce(Invoke(&obs, &MockObserver::SetNotifiedPageNode));
  auto page_node = CreateNode<PageNodeImpl>();
  const PageNode* raw_page_node = page_node.get();
  EXPECT_EQ(raw_page_node, obs.TakeNotifiedPageNode());

  EXPECT_CALL(obs, OnDecrementAudioNum(_))
      .WillOnce(Invoke(&obs, &MockObserver::SetNotifiedPageNode));
  page_node->OneShotMediaPlayerStopped();
  EXPECT_EQ(raw_page_node, obs.TakeNotifiedPageNode());

  const AudioContextId audio_context_id = std::make_pair(nullptr, 1);
  EXPECT_CALL(obs, OnAudioContextPlaybackStarted(_))
      .WillOnce(Invoke(&obs, &MockObserver::SetNotifiedAudioContextId));
  page_node->AudioContextPlaybackStarted(audio_context_id);
  EXPECT_EQ(audio_context_id, obs.TakeNotifiedAudioContextId());

  EXPECT_CALL(obs, OnAudioContextPlaybackStopped(_))
      .WillOnce(Invoke(&obs, &MockObserver::SetNotifiedAudioContextId));
  page_node->AudioContextPlaybackStopped(audio_context_id);
  EXPECT_EQ(audio_context_id, obs.TakeNotifiedAudioContextId());

  graph()->RemovePageNodeObserver(&obs);
}
#endif