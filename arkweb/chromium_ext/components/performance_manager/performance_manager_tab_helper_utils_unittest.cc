/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "base/test/scoped_feature_list.h"
#include "base/test/task_environment.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/media_player_id.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/common/content_features.h"
#include "content/public/test/web_contents_tester.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_render_frame_host.h"
#include "content/test/test_web_contents.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "components/performance_manager/test_support/performance_manager_test_harness.h"
#include "components/performance_manager/graph/page_node_impl.h"
#include "components/performance_manager/graph/frame_node_impl.h"
#include "components/performance_manager/performance_manager_impl.h"
#include "components/performance_manager/public/features.h"
#include "components/performance_manager/public/graph/page_node.h"
#include "components/performance_manager/performance_manager_tab_helper.h"

using namespace performance_manager;

class PerformanceManagerTabHelperUtilsTest
    : public PerformanceManagerTestHarness {
 public:
  PerformanceManagerTabHelperUtilsTest() {
    scoped_feature_list_.InitWithFeatureState(
        performance_manager::features::kSeamlessRenderFrameSwap, true);
  }

  void TearDown() override {
    DeleteContents();
    PerformanceManagerTestHarness::TearDown();
  }

 protected:
  base::test::ScopedFeatureList scoped_feature_list_;
};

TEST_F(PerformanceManagerTabHelperUtilsTest, MediaStartedPlaying001) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(true, true);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
}

TEST_F(PerformanceManagerTabHelperUtilsTest, MediaStartedPlaying002) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(true, false);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
}

TEST_F(PerformanceManagerTabHelperUtilsTest, MediaStartedPlaying003) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(false, true);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
}

TEST_F(PerformanceManagerTabHelperUtilsTest, MediaStoppedPlaying001) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(true, true);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStoppedPlaying(
      video_type, media_player_id, content::WebContentsObserver::MediaStoppedReason::kUnspecified));
  is_media_playing = page_node->is_media_playing();
  EXPECT_FALSE(is_media_playing);
}

TEST_F(PerformanceManagerTabHelperUtilsTest, MediaStoppedPlaying002) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(true, true);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStoppedPlaying(
      video_type, media_player_id, content::WebContentsObserver::MediaStoppedReason::kReachedEndOfStream));
  is_media_playing = page_node->is_media_playing();
  EXPECT_FALSE(is_media_playing);
}

TEST_F(PerformanceManagerTabHelperUtilsTest, OneShotMediaPlayerStopped001) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(true, true);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
  ASSERT_NO_FATAL_FAILURE(tab_helper->OneShotMediaPlayerStopped());
}

TEST_F(PerformanceManagerTabHelperUtilsTest, AudioContextPlaybackStarted001) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(true, true);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
  content::RenderFrameHost* rfh = web_contents()->GetPrimaryMainFrame();
  int audio_context_id = 123;
  content::WebContentsObserver::AudioContextId audio_context_id_pair;
  audio_context_id_pair.first = rfh;
  audio_context_id_pair.second = audio_context_id;
  EXPECT_TRUE(audio_context_id_pair.first);
  ASSERT_NO_FATAL_FAILURE(tab_helper->AudioContextPlaybackStarted(audio_context_id_pair));
}

TEST_F(PerformanceManagerTabHelperUtilsTest, AudioContextPlaybackStarted002) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(true, true);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
  int audio_context_id = 123;
  content::WebContentsObserver::AudioContextId audio_context_id_pair;
  audio_context_id_pair.first = nullptr;
  audio_context_id_pair.second = audio_context_id;
  ASSERT_NO_FATAL_FAILURE(tab_helper->AudioContextPlaybackStarted(audio_context_id_pair));
}

TEST_F(PerformanceManagerTabHelperUtilsTest, AudioContextPlaybackStopped001) {
  SetContents(CreateTestWebContents());
  auto* tab_helper = performance_manager::PerformanceManagerTabHelper::FromWebContents(web_contents());
  ASSERT_TRUE(tab_helper);
  auto page_node = tab_helper->primary_page_node();
  ASSERT_TRUE(page_node);
  content::MediaPlayerId media_player_id(web_contents()->GetPrimaryMainFrame()->GetGlobalId(), 1);
  performance_manager::PerformanceManagerTabHelper::MediaPlayerInfo video_type(true, true);
  ASSERT_NO_FATAL_FAILURE(tab_helper->MediaStartedPlaying(video_type, media_player_id));
  bool is_media_playing = false;
  is_media_playing = page_node->is_media_playing();
  EXPECT_TRUE(is_media_playing);
  content::RenderFrameHost* rfh = web_contents()->GetPrimaryMainFrame();
  int audio_context_id = 123;
  content::WebContentsObserver::AudioContextId audio_context_id_pair;
  audio_context_id_pair.first = rfh;
  audio_context_id_pair.second = audio_context_id;
  EXPECT_TRUE(audio_context_id_pair.first);
  tab_helper->AudioContextPlaybackStarted(audio_context_id_pair);
  ASSERT_NO_FATAL_FAILURE(tab_helper->AudioContextPlaybackStopped(audio_context_id_pair));
}
