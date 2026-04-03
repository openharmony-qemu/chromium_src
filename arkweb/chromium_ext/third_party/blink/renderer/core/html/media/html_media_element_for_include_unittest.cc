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

namespace blink {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
TEST_P(HTMLMediaElementTest, IncludeTestVideoIsMuted) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  EXPECT_EQ(Media()->IsMediaMuted(), 2);
}

TEST_P(HTMLMediaElementTest, IncludeTestAudioIsMuted) {
  if (GetParam() != MediaTestParam::kAudio) {
    return;
  }
  Media()->SetSrc(SrcSchemeToURL(TestURLScheme::kHttp));
  test::RunPendingTasks();

  MockWebMediaPlayer* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  EXPECT_CALL(*mock_wmpi, HasAudio()).WillRepeatedly(Return(true));
  Media()->setMuted(true);
  EXPECT_EQ(Media()->IsMediaMuted(), 0);
  Media()->setMuted(false);
  EXPECT_EQ(Media()->IsMediaMuted(), 1);
}

TEST_P(HTMLMediaElementTest, IncludeTestIsCustomVideoPlayerEnabled) {
  Media()->should_create_custom_renderer_ = false;
  Media()->GetDocument().GetSettings()->SetCustomVideoPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomVideoPlayerEnabled(), false);

  Media()->should_create_custom_renderer_ = false;
  Media()->GetDocument().GetSettings()->SetCustomVideoPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomVideoPlayerEnabled(), false);

  Media()->should_create_custom_renderer_ = true;
  Media()->GetDocument().GetSettings()->SetCustomVideoPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomVideoPlayerEnabled(), true);

  Media()->should_create_custom_renderer_ = true;
  Media()->GetDocument().GetSettings()->SetCustomVideoPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomVideoPlayerEnabled(), false);
}

TEST_P(HTMLMediaElementTest, IncludeTestShouldCustomVideoPlayerOverlay) {
  Media()->GetDocument().GetSettings()->SetCustomVideoPlayerOverlay(true);
  EXPECT_EQ(Media()->ShouldCustomVideoPlayerOverlay(), true);
  Media()->GetDocument().GetSettings()->SetCustomVideoPlayerOverlay(false);
  EXPECT_EQ(Media()->ShouldCustomVideoPlayerOverlay(), false);
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos1) {
  Media()->load_state_ = HTMLMediaElement::kWaitingForSource;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_TRUE(source_infos.empty());
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  EXPECT_EQ(Media()->childNodes()->length(), 0);
  source_infos = Media()->GetRemainSourceInfos();
  EXPECT_TRUE(source_infos.empty());
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos2) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* source1 =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source1->setAttribute(html_names::kSrcAttr, AtomicString("http://test1/"));
  source1->setAttribute(html_names::kTypeAttr, AtomicString("video/test1"));
  Media()->AppendChild(source1);
  auto* source2 =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source2->setAttribute(html_names::kSrcAttr, AtomicString("http://test2/"));
  source2->setAttribute(html_names::kTypeAttr, AtomicString("video/test2"));
  Media()->AppendChild(source2);
  Media()->next_child_node_to_consider_ = source2;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_EQ(source_infos.size(), 1u);
  EXPECT_EQ(source_infos[0].media_source, "http://test2/");
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos3) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* source =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source->setAttribute(html_names::kSrcAttr, AtomicString("http://test1/"));
  source->setAttribute(html_names::kTypeAttr, AtomicString("video/test1"));
  Media()->AppendChild(source);
  Media()->next_child_node_to_consider_ = source;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_EQ(source_infos.size(), 1u);
  EXPECT_EQ(source_infos[0].media_source, "http://test1/");
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos4) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* source1 =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source1->setAttribute(html_names::kSrcAttr, AtomicString("http://test1/"));
  source1->setAttribute(html_names::kTypeAttr, AtomicString("video/test1"));
  Media()->AppendChild(source1);

  auto* source2 =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source2->setAttribute(html_names::kSrcAttr, AtomicString("http://test2/"));
  source2->setAttribute(html_names::kTypeAttr, AtomicString("video/test2"));
  Media()->AppendChild(source2);
  Media()->next_child_node_to_consider_ = source1;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_EQ(source_infos.size(), 2u);
  EXPECT_EQ(source_infos[0].media_source, "http://test1/");
  EXPECT_EQ(source_infos[1].media_source, "http://test2/");
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos5) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* source =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source->setAttribute(html_names::kSrcAttr, AtomicString("http://test1/"));
  source->setAttribute(html_names::kTypeAttr, AtomicString("video/test1"));
  Media()->AppendChild(source);
  Media()->next_child_node_to_consider_ = source;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_EQ(source_infos.size(), 1u);
  EXPECT_EQ(source_infos[0].media_source, "http://test1/");
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos6) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* div = MakeGarbageCollected<HTMLDivElement>(Media()->GetDocument());
  Media()->AppendChild(div);
  Media()->next_child_node_to_consider_ = div;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_TRUE(source_infos.empty());
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos7) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* span = MakeGarbageCollected<HTMLSpanElement>(Media()->GetDocument());
  Media()->AppendChild(span);
  Media()->next_child_node_to_consider_ = span;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_TRUE(source_infos.empty());
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos8) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* parent_div =
      MakeGarbageCollected<HTMLDivElement>(Media()->GetDocument());
  Media()->AppendChild(parent_div);
  auto* source =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source->setAttribute(html_names::kSrcAttr, AtomicString("http://test1/"));
  source->setAttribute(html_names::kTypeAttr, AtomicString("video/test1"));
  parent_div->AppendChild(source);
  Media()->next_child_node_to_consider_ = parent_div;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_TRUE(source_infos.empty());
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos9) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* source =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source->setAttribute(html_names::kSrcAttr, AtomicString("http://test1/"));
  source->setAttribute(html_names::kTypeAttr, AtomicString("video/test1"));
  Media()->AppendChild(source);
  Media()->next_child_node_to_consider_ = source;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_EQ(source_infos.size(), 1u);
  EXPECT_EQ(source_infos[0].media_source, "http://test1/");
}

TEST_P(HTMLMediaElementTest, IncludeTestGetRemainSourceInfos10) {
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  auto* source =
      MakeGarbageCollected<HTMLSourceElement>(Media()->GetDocument());
  source->setAttribute(html_names::kSrcAttr, AtomicString(""));
  source->setAttribute(html_names::kTypeAttr, AtomicString("video/test1"));
  Media()->AppendChild(source);
  Media()->next_child_node_to_consider_ = source;
  auto source_infos = Media()->GetRemainSourceInfos();
  EXPECT_TRUE(source_infos.empty());
}

TEST_P(HTMLMediaElementTest, IncludeTestEmptyControlsList) {
  auto controls_list = Media()->GetMediaControlsList();
  EXPECT_TRUE(controls_list.empty());
  Media()->setAttribute(html_names::kControlslistAttr, AtomicString("test1"));
  controls_list = Media()->GetMediaControlsList();
  EXPECT_EQ(controls_list.size(), 1u);
  EXPECT_EQ(controls_list[0], "test1");
}

TEST_P(HTMLMediaElementTest, IncludeTestGetElementAttributes) {
  auto attributes_map = Media()->GetElementAttributes();
  EXPECT_TRUE(attributes_map.empty());
  Media()->setAttribute(html_names::kSrcAttr, AtomicString("http://test1/"));
  attributes_map = Media()->GetElementAttributes();
  EXPECT_EQ(attributes_map.size(), 1u);
  EXPECT_EQ(attributes_map["src"], "http://test1/");
}

TEST_P(HTMLMediaElementTest, IncludeTestGetOutgoingReferrerString) {
  auto str = Media()->GetOutgoingReferrerString();
  EXPECT_EQ(str.length(), 0u);
}

TEST_P(HTMLMediaElementTest, IncludeTestUpdatePlaybackStatus) {
  Media()->paused_ = true;
  uint32_t status = 0;
  ASSERT_NO_FATAL_FAILURE(Media()->UpdatePlaybackStatus(status));
  EXPECT_TRUE(Media()->paused_);
  status = 1;
  ASSERT_NO_FATAL_FAILURE(Media()->UpdatePlaybackStatus(status));
  Media()->paused_ = false;
  status = 0;
  ASSERT_NO_FATAL_FAILURE(Media()->UpdatePlaybackStatus(status));
}

TEST_P(HTMLMediaElementTest, IncludeTestUpdateVolume) {
  Media()->volume_ = 0.5;
  double volume = 0.5;
  ASSERT_NO_FATAL_FAILURE(Media()->UpdateVolume(volume));
  volume = 0.6;
  ASSERT_NO_FATAL_FAILURE(Media()->UpdateVolume(volume));
  EXPECT_EQ(Media()->volume_, volume);
}

TEST_P(HTMLMediaElementTest, IncludeTestUpdatePlaybackRate) {
  Media()->playback_rate_ = 0.5;
  double playback_rate = 0.5;
  ASSERT_NO_FATAL_FAILURE(Media()->UpdatePlaybackRate(playback_rate));
  playback_rate = 0.6;
  ASSERT_NO_FATAL_FAILURE(Media()->UpdatePlaybackRate(playback_rate));
  EXPECT_EQ(Media()->playback_rate_, playback_rate);
}

TEST_P(HTMLMediaElementTest, IncludeTestGetVideoRect1) {
  gfx::Rect expected_rect(100, 200, 300, 400);
  Media()->layer_rect_ = expected_rect;
  gfx::Rect result = Media()->GetVideoRect();
  EXPECT_EQ(result, expected_rect);
}

TEST_P(HTMLMediaElementTest, IncludeTestGetVideoRect2) {
  Media()->layer_rect_ = gfx::Rect();
  gfx::Rect result = Media()->GetVideoRect();
  EXPECT_GT(result.width(), 0);
  EXPECT_GT(result.height(), 0);
  EXPECT_EQ(result.x(), 0);
  EXPECT_EQ(result.y(), 0);
}

TEST_P(HTMLMediaElementTest, IncludeTestGetVideoRect3) {
  Media()->layer_rect_ = gfx::Rect();
  Media()->setAttribute(
      html_names::kStyleAttr,
      AtomicString(
          "display: block; width: 500px; height: 400px; position: absolute;"));
  Media()->setAttribute(html_names::kSrcAttr,
                        AtomicString("http://test1/video.mp4"));
  Media()->GetDocument().body()->AppendChild(Media());
  Media()->GetDocument().UpdateStyleAndLayoutTree();
  Media()->GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);
  gfx::Rect result = Media()->GetVideoRect();
  EXPECT_GT(result.width(), 0);
  EXPECT_GT(result.height(), 0);
}

TEST_P(HTMLMediaElementTest, IncludeTestGetVideoRect4) {
  Media()->layer_rect_ = gfx::Rect();
  if (Media()->parentNode()) {
    Media()->parentNode()->removeChild(Media());
  }
  gfx::Rect result = Media()->GetVideoRect();
  EXPECT_GT(result.width(), 0);
  EXPECT_GT(result.height(), 0);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnLayerRectChange1) {
  gfx::Rect test_rect(100, 200, 300, 400);
  Media()->layer_rect_ = test_rect;
  ASSERT_NO_FATAL_FAILURE(Media()->OnLayerRectChange(test_rect));
  EXPECT_EQ(Media()->layer_rect_, test_rect);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnLayerRectChange2) {
  gfx::Rect initial_rect(100, 200, 300, 400);
  Media()->layer_rect_ = initial_rect;
  gfx::Rect new_rect(400, 300, 200, 100);
  ASSERT_NO_FATAL_FAILURE(Media()->OnLayerRectChange(new_rect));
  EXPECT_EQ(Media()->layer_rect_, new_rect);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnLayerRectChange3) {
  WaitForPlayer();
  gfx::Rect initial_rect(100, 200, 300, 400);
  Media()->layer_rect_ = initial_rect;
  gfx::Rect new_rect(400, 300, 200, 100);
  ASSERT_NO_FATAL_FAILURE(Media()->OnLayerRectChange(new_rect));
  EXPECT_EQ(Media()->layer_rect_, new_rect);
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
TEST_P(HTMLMediaElementTest, IncludeTestGetMediaTitle) {
  Media()->media_title_ = String();
  String result = Media()->GetMediaTitle();
  EXPECT_TRUE(result.empty());
  String test_title = "test";
  Media()->media_title_ = test_title;
  result = Media()->GetMediaTitle();
  EXPECT_EQ(result, test_title);
}

TEST_P(HTMLMediaElementTest, IncludeTestGetVideoPoster) {
  Media()->video_poster_ = String();
  String result = Media()->GetVideoPoster();
  EXPECT_TRUE(result.empty());
  String test_poster = "test";
  Media()->video_poster_ = test_poster;
  result = Media()->GetVideoPoster();
  EXPECT_EQ(result, test_poster);
}

TEST_P(HTMLMediaElementTest, IncludeTestDidEndAVSession) {
  ASSERT_NO_FATAL_FAILURE(Media()->DidEndAVSession(false));
  WaitForPlayer();
  ASSERT_NO_FATAL_FAILURE(Media()->DidEndAVSession(false));
}
#endif  // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_P(HTMLMediaElementTest, IncludeTestOnLayerBoundsChange1) {
  Media()->has_been_seen_playing_once_ = true;
  gfx::Rect old_rect(10, 10, 20, 20);
  Media()->video_rect_ = gfx::RectF(old_rect);
  gfx::Rect new_bounds(100, 100, 300, 200);
  Media()->OnLayerBoundsChange(new_bounds);
  EXPECT_EQ(Media()->video_rect_, gfx::RectF(old_rect));
}

TEST_P(HTMLMediaElementTest, IncludeTestOnLayerBoundsChange2) {
  WaitForPlayer();
  auto* page = Media()->GetDocument().GetPage();
  auto* view = Media()->GetDocument().View();
  ASSERT_NE(page, nullptr);
  ASSERT_NE(view, nullptr);
  view->Resize(gfx::Size(800, 600));
  page->GetVisualViewport().SetSize(gfx::Size(800, 600));
  UpdateLifecyclePhases();
  gfx::Rect in_view_bounds(0, 0, 320, 240);
  Media()->has_been_seen_playing_once_ = false;
  Media()->has_notified_playing_ = true;

  Media()->OnLayerBoundsChange(in_view_bounds);
  EXPECT_FALSE(Media()->video_rect_.IsEmpty());
  EXPECT_EQ(Media()->video_rect_, gfx::RectF(in_view_bounds));
  EXPECT_TRUE(Media()->has_been_seen_playing_once_);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnLayerBoundsChange3) {
  Media()->has_been_seen_playing_once_ = false;
  Media()->has_notified_playing_ = true;
  gfx::Rect out_of_view_bounds(100000, 100000, 100, 100);
  Media()->OnLayerBoundsChange(out_of_view_bounds);
  EXPECT_TRUE(Media()->video_rect_.IsEmpty());
  EXPECT_FALSE(Media()->has_been_seen_playing_once_);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnLayerBoundsChange4) {
  Media()->has_been_seen_playing_once_ = false;
  Media()->has_notified_playing_ = false;
  gfx::Rect out_of_view_bounds(100000, 100000, 50, 50);
  Media()->OnLayerBoundsChange(out_of_view_bounds);
  EXPECT_TRUE(Media()->video_rect_.IsEmpty());
}

TEST_P(HTMLMediaElementTest, IncludeTestNotifyVideoPlayingInternal1) {
  Media()->video_assistant_enabled_ = false;
  Media()->video_assistant_.reset();
  ASSERT_NO_FATAL_FAILURE(Media()->NotifyVideoPlayingInternal());
  EXPECT_FALSE(Media()->notify_video_playing_timer_.IsActive());
}

TEST_P(HTMLMediaElementTest, IncludeTestNotifyVideoPlayingInternal2) {
  Media()->video_assistant_enabled_ = true;
  Media()->video_assistant_ = true;
  Media()->notify_video_playing_timer_.StartOneShot(base::Milliseconds(100),
                                                    FROM_HERE);
  ASSERT_NO_FATAL_FAILURE(Media()->NotifyVideoPlayingInternal());
  EXPECT_TRUE(Media()->notify_video_playing_timer_.IsActive());
}

TEST_P(HTMLMediaElementTest, IncludeTestNotifyVideoPlayingInternal3) {
  Media()->video_assistant_enabled_ = true;
  Media()->video_assistant_ = true;
  ASSERT_NO_FATAL_FAILURE(Media()->NotifyVideoPlayingInternal());
  EXPECT_TRUE(Media()->notify_video_playing_timer_.IsActive());
}

TEST_P(HTMLMediaElementTest, IncludeTestNotifyVideoDestroyed1) {
  Media()->video_assistant_enabled_ = false;
  Media()->video_assistant_.reset();
  ASSERT_NO_FATAL_FAILURE(Media()->NotifyVideoDestroyed());
}

TEST_P(HTMLMediaElementTest, IncludeTestNotifyVideoDestroyed2) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  Media()->video_assistant_enabled_ = true;
  Media()->video_assistant_ = true;
  ASSERT_NO_FATAL_FAILURE(Media()->NotifyVideoDestroyed());
}

TEST_P(HTMLMediaElementTest, IncludeTestNotifyVideoDestroyed3) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  WaitForPlayer();
  Media()->video_assistant_enabled_ = true;
  Media()->video_assistant_ = true;
  ASSERT_NO_FATAL_FAILURE(Media()->NotifyVideoDestroyed());
}

TEST_P(HTMLMediaElementTest, IncludeTestNotifyVideoDestroyed4) {
  if (GetParam() != MediaTestParam::kAudio) {
    return;
  }
  Media()->video_assistant_enabled_ = true;
  Media()->video_assistant_ = true;
  ASSERT_NO_FATAL_FAILURE(Media()->NotifyVideoDestroyed());
}

TEST_P(HTMLMediaElementTest, IncludeTestCollectMediaInfoAttributesForVAST1) {
  auto info = Media()->CollectMediaInfoAttributesForVAST();
  EXPECT_EQ(info.get(), nullptr);
}

TEST_P(HTMLMediaElementTest, IncludeTestCollectMediaInfoAttributesForVAST2) {
  WaitForPlayer();
  auto* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  ASSERT_NE(mock_wmpi, nullptr);
  EXPECT_CALL(*mock_wmpi, SupportVideoSurface())
      .WillRepeatedly(testing::Return(true));

  auto info = Media()->CollectMediaInfoAttributesForVAST();
  ASSERT_NE(info.get(), nullptr);
  EXPECT_TRUE(info->fullscreen_overlay);
}

TEST_P(HTMLMediaElementTest, IncludeTestCollectMediaInfoAttributesForVAST3) {
  WaitForPlayer();
  auto* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  ASSERT_NE(mock_wmpi, nullptr);
  EXPECT_CALL(*mock_wmpi, SupportVideoSurface())
      .WillRepeatedly(testing::Return(false));

  auto info = Media()->CollectMediaInfoAttributesForVAST();
  ASSERT_NE(info.get(), nullptr);
  EXPECT_FALSE(info->fullscreen_overlay);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnVideoAssistantConfigReceived1) {
  bool result = false;
  auto cb = base::BindOnce([](bool* flag) { *flag = true; }, &result);
  auto config = media::mojom::blink::VideoAssistantConfig::New();
  config->video_assistant = false;

  ASSERT_NO_FATAL_FAILURE(Media()->OnVideoAssistantConfigReceived(
      std::move(cb), std::move(config)));
  EXPECT_FALSE(result);
  EXPECT_FALSE(Media()->video_assistant_.value_or(true));
}

TEST_P(HTMLMediaElementTest, IncludeTestOnVideoAssistantConfigReceived2) {
  bool result = false;
  auto cb = base::BindOnce([](bool* flag) { *flag = true; }, &result);
  auto config = media::mojom::blink::VideoAssistantConfig::New();
  config->video_assistant = true;

  ASSERT_NO_FATAL_FAILURE(Media()->OnVideoAssistantConfigReceived(
      std::move(cb), std::move(config)));
  EXPECT_TRUE(result);
  EXPECT_TRUE(Media()->video_assistant_.value_or(false));
}

TEST_P(HTMLMediaElementTest, IncludeTestOnNotifyVideoPlayingTimerFired1) {
  if (GetParam() != MediaTestParam::kAudio) {
    return;
  }
  Media()->has_notified_playing_ = false;
  Media()->has_been_seen_playing_once_ = false;
  ASSERT_NO_FATAL_FAILURE(Media()->OnNotifyVideoPlayingTimerFired(nullptr));
  EXPECT_TRUE(Media()->has_notified_playing_);
  EXPECT_FALSE(Media()->has_been_seen_playing_once_);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnNotifyVideoPlayingTimerFired2) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }

  Media()->has_notified_playing_ = false;
  Media()->has_been_seen_playing_once_ = false;
  Media()->video_rect_ = gfx::RectF();
  ASSERT_NO_FATAL_FAILURE(Media()->OnNotifyVideoPlayingTimerFired(nullptr));
  EXPECT_TRUE(Media()->has_notified_playing_);
  EXPECT_FALSE(Media()->has_been_seen_playing_once_);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnNotifyVideoPlayingTimerFired3) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  WaitForPlayer();
  Media()->has_notified_playing_ = false;
  Media()->has_been_seen_playing_once_ = true;
  Media()->video_rect_ = gfx::RectF(0, 0, 10, 10);
  ASSERT_NO_FATAL_FAILURE(Media()->OnNotifyVideoPlayingTimerFired(nullptr));
  EXPECT_TRUE(Media()->has_notified_playing_);
}

TEST_P(HTMLMediaElementTest, IncludeTestIsCustomMediaPlayerEnabled1) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_TRUE(Media()->IsCustomMediaPlayerEnabled());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_FALSE(Media()->IsCustomMediaPlayerEnabled());
}

TEST_P(HTMLMediaElementTest, IncludeTestIsCustomMediaPlayerEnabled2) {
  if (GetParam() != MediaTestParam::kAudio) {
    return;
  }
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_FALSE(Media()->IsCustomMediaPlayerEnabled());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_FALSE(Media()->IsCustomMediaPlayerEnabled());
}

TEST_P(HTMLMediaElementTest, IncludeTestOnSupportVideoSurfaceChanged1) {
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  ASSERT_NO_FATAL_FAILURE(
      Media()->OnSupportVideoSurfaceChanged(true, "decoder"));
  EXPECT_FALSE(Media()->user_wants_controls_visible_.has_value());
}

TEST_P(HTMLMediaElementTest, IncludeTestOnSupportVideoSurfaceChanged2) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  ASSERT_NO_FATAL_FAILURE(
      Media()->OnSupportVideoSurfaceChanged(true, "decoder"));
  EXPECT_FALSE(Media()->user_wants_controls_visible_.has_value());
}

TEST_P(HTMLMediaElementTest, IncludeTestOnSupportVideoSurfaceChanged3) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  WaitForPlayer();
  ASSERT_NO_FATAL_FAILURE(
      Media()->OnSupportVideoSurfaceChanged(true, "decoderX"));
  EXPECT_FALSE(Media()->user_wants_controls_visible_.has_value());
}

TEST_P(HTMLMediaElementTest, IncludeTestOnSupportVideoSurfaceChanged4) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  video->SetSrc(SrcSchemeToURL(TestURLScheme::kHttp));
  test::RunPendingTasks();
  SetReadyState(HTMLMediaElement::kHaveEnoughData);
  video->Play();
  ASSERT_NE(VideoVisibilityTracker(), nullptr);
  SimulateEnterFullscreen(video);
  ASSERT_NO_FATAL_FAILURE(
      video->OnSupportVideoSurfaceChanged(false, "decoder"));
  EXPECT_TRUE(Media()->user_wants_controls_visible_.has_value());
  EXPECT_FALSE(Media()->user_wants_controls_visible_.value());
  SimulateExitFullscreen(video);
}

TEST_P(HTMLMediaElementTest, IncludeTestOnSupportVideoSurfaceChanged5) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  ASSERT_NO_FATAL_FAILURE(
      Media()->OnSupportVideoSurfaceChanged(true, "decoder"));
  EXPECT_FALSE(Media()->user_wants_controls_visible_.has_value());
}

TEST_P(HTMLMediaElementTest, IncludeTestOnSupportVideoSurfaceChanged6) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  ASSERT_NO_FATAL_FAILURE(
      Media()->OnSupportVideoSurfaceChanged(false, "decoder"));
  EXPECT_FALSE(Media()->user_wants_controls_visible_.has_value());
}

TEST_P(HTMLMediaElementTest, IncludeTestOnSupportVideoSurfaceChanged7) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  video->SetSrc(SrcSchemeToURL(TestURLScheme::kHttp));
  test::RunPendingTasks();
  SetReadyState(HTMLMediaElement::kHaveEnoughData);
  video->Play();
  ASSERT_NE(VideoVisibilityTracker(), nullptr);
  SimulateEnterFullscreen(video);
  ASSERT_NO_FATAL_FAILURE(
      Media()->OnSupportVideoSurfaceChanged(true, "decoder"));

  EXPECT_FALSE(Media()->user_wants_controls_visible_.has_value());
  SimulateExitFullscreen(video);
}

TEST_P(HTMLMediaElementTest, IncludeTestSetVideoSurface) {
  ASSERT_NO_FATAL_FAILURE(Media()->SetVideoSurface(1));
  WaitForPlayer();
  auto* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  ASSERT_NE(mock_wmpi, nullptr);
  EXPECT_CALL(*mock_wmpi, SetVideoSurface(1)).Times(1);
  ASSERT_NO_FATAL_FAILURE(Media()->SetVideoSurface(1));
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_PIP)
TEST_P(HTMLMediaElementTest, IncludeTestPipEnable) {
  ASSERT_NO_FATAL_FAILURE(Media()->PipEnable(false));
  WaitForPlayer();
  auto* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  ASSERT_NE(mock_wmpi, nullptr);
  EXPECT_CALL(*mock_wmpi, PipEnable(1)).Times(1);
  ASSERT_NO_FATAL_FAILURE(Media()->PipEnable(true));
}
#endif  // ARKWEB_PIP

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_P(HTMLMediaElementTest, IncludeTestRecycleDmaBuffer) {
  ASSERT_NO_FATAL_FAILURE(Media()->RecycleDmaBuffer());
  WaitForPlayer();
  auto* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  ASSERT_NE(mock_wmpi, nullptr);
  EXPECT_CALL(*mock_wmpi, RecycleDmaBuffer()).Times(1);
  ASSERT_NO_FATAL_FAILURE(Media()->RecycleDmaBuffer());
}

TEST_P(HTMLMediaElementTest, IncludeTestResumeDmaBuffer) {
  ASSERT_NO_FATAL_FAILURE(Media()->ResumeDmaBuffer());
  WaitForPlayer();
  auto* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  ASSERT_NE(mock_wmpi, nullptr);
  EXPECT_CALL(*mock_wmpi, ResumeDmaBuffer()).Times(1);
  ASSERT_NO_FATAL_FAILURE(Media()->ResumeDmaBuffer());
}
#endif  // ARKWEB_MEDIA_DMABUF

#if BUILDFLAG(ARKWEB_BFCACHE)
TEST_P(HTMLMediaElementTest, IncludeTestIsMediaResumeFromBFCachePage) {
  EXPECT_EQ(Media()->IsMediaResumeFromBFCachePage(), true);
}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_P(HTMLMediaElementTest, IncludeTestVideoId) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  ASSERT_NO_FATAL_FAILURE(Media()->videoId());
}

TEST_P(HTMLMediaElementTest, VideoLoadOpt_IncludeTestHbsMediaPreloadTime) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  EXPECT_EQ(Media()->hbsMediaPreloadTime(), 4);
}

TEST_P(HTMLMediaElementTest, VideoLoadOpt_IncludeTestHbsMediaMinCacheTime) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  EXPECT_EQ(Media()->hbsMediaMinCacheTime(), 2);
}

TEST_P(HTMLMediaElementTest, VideoLoadOpt_IncludeTestHbsMediaMaxCacheTime) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  EXPECT_EQ(Media()->hbsMediaMaxCacheTime(), 6);
}

TEST_P(HTMLMediaElementTest, VideoLoadOpt_IncludeTestHbsMediaBitrate) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  EXPECT_EQ(Media()->hbsMediaBitrate(), 2000);
}

TEST_P(HTMLMediaElementTest, VideoLoadOpt_IncludeTestHbsMediaMoovSize) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  EXPECT_EQ(Media()->hbsMediaMoovSize(), 512);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
TEST_P(HTMLMediaElementTest, DFX_TESTReportVideoExperience001) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->is_logger_export_ = false;
  ASSERT_NO_FATAL_FAILURE(video->ReportVideoExperienceToBI());
}

TEST_P(HTMLMediaElementTest, DFX_TESTReportVideoExperience002) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->is_logger_export_ = true;
  ASSERT_NO_FATAL_FAILURE(video->ReportVideoExperienceToBI());
}

TEST_P(HTMLMediaElementTest, DFX_TESTReportVideoExperience003) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->is_logger_export_ = true;
  video->SetVideoExperienceMojo();
  ASSERT_NO_FATAL_FAILURE(video->ReportVideoExperienceToBI());
}

TEST_P(HTMLMediaElementTest, DFX_TestGetMediaPlayerType001) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  video->SetSrc(SrcSchemeToURL(TestURLScheme::kHttp));
  test::RunPendingTasks();
  ASSERT_NO_FATAL_FAILURE(video->GetMediaPlayerType());
}

TEST_P(HTMLMediaElementTest, DFX_TestGetMediaPlayerType002) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  video->SetSrc(SrcSchemeToURL(TestURLScheme::kData));
  test::RunPendingTasks();
  ASSERT_NO_FATAL_FAILURE(video->GetMediaPlayerType());
}

TEST_P(HTMLMediaElementTest, DFX_TestGetMediaPlayerType003) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  video->GetDocument().body()->AppendChild(video);
  video->SetSrc(SrcSchemeToURL(TestURLScheme::kBlob));
  test::RunPendingTasks();
  ASSERT_NO_FATAL_FAILURE(video->GetMediaPlayerType());
}

TEST_P(HTMLMediaElementTest, DFX_TestScheduleVideoFreezeEvent) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  int32_t usage_scenario = 5;
  Media()->GetDocument().GetSettings()->SetUsageScenario(usage_scenario);
  auto* video = To<HTMLVideoElement>(Media());
  ASSERT_NO_FATAL_FAILURE(video->ScheduleVideoFreezeEvent());
}

TEST_P(HTMLMediaElementTest, DFX_TestFreezeTime001) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  double result = video->freezeTime();
  ASSERT_EQ(result, 0.0);
}

TEST_P(HTMLMediaElementTest, DFX_TestFreezeTime002) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  int32_t usage_scenario = 5;
  Media()->GetDocument().GetSettings()->SetUsageScenario(usage_scenario);
  auto* video = To<HTMLVideoElement>(Media());
  double result = video->freezeTime();
  ASSERT_EQ(result, 0.0);
}

TEST_P(HTMLMediaElementTest, DFX_TestPlayedTime001) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  auto* video = To<HTMLVideoElement>(Media());
  double result = video->playedTime();
  ASSERT_EQ(result, 0.0);
}

TEST_P(HTMLMediaElementTest, DFX_TestPlayedTime002) {
  if (GetParam() != MediaTestParam::kVideo) {
    return;
  }
  int32_t usage_scenario = 5;
  Media()->GetDocument().GetSettings()->SetUsageScenario(usage_scenario);
  auto* video = To<HTMLVideoElement>(Media());
  double result = video->playedTime();
  ASSERT_EQ(result, 0.0);
}
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_P(HTMLMediaElementTest, DownloadHLS_001) {
  Media()->GetDocument().GetSettings()->SetHideDownloadUI(true);
  Media()->SetSrc(SrcSchemeToURL(TestURLScheme::kHttp));
  test::RunPendingTasks();
  EXPECT_EQ(Media()->SupportVideoAssistantDownload(), false);
}

TEST_P(HTMLMediaElementTest, DownloadHLS_002) {
  Media()->GetDocument().GetSettings()->SetHideDownloadUI(false);
  Media()->SetSrc(AtomicString(""));
  test::RunPendingTasks();
  EXPECT_EQ(Media()->SupportVideoAssistantDownload(), false);
}

TEST_P(HTMLMediaElementTest, DownloadHLS_003) {
  Media()->GetDocument().GetSettings()->SetHideDownloadUI(false);
  Media()->SetSrc(SrcSchemeToURL(TestURLScheme::kHttp));
  Media()->SetNetworkState(HTMLMediaElement::kNetworkEmpty);
  EXPECT_EQ(Media()->SupportVideoAssistantDownload(), false);
}

TEST_P(HTMLMediaElementTest, DownloadHLS_004) {
  Media()->GetDocument().GetSettings()->SetHideDownloadUI(false);
  Media()->SetSrc(SrcSchemeToURL(TestURLScheme::kFile));
  Media()->SetNetworkState(HTMLMediaElement::kNetworkLoading);
  test::RunPendingTasks();
  EXPECT_EQ(Media()->SupportVideoAssistantDownload(), false);
}

TEST_P(HTMLMediaElementTest, DownloadHLS_005) {
  Media()->GetDocument().GetSettings()->SetHideDownloadUI(false);
  Media()->SetSrc(SrcSchemeToURL(TestURLScheme::kHttp));
  Media()->SetNetworkState(HTMLMediaElement::kNetworkLoading);
  Media()->DurationChanged(std::numeric_limits<double>::infinity(), false);
  test::RunPendingTasks();
  EXPECT_EQ(Media()->SupportVideoAssistantDownload(), false);
}

TEST_P(HTMLMediaElementTest, DownloadHLS_006) {
  Media()->GetDocument().GetSettings()->SetHideDownloadUI(false);
  Media()->SetSrc(SrcSchemeToURL(TestURLScheme::kHttp));
  Media()->SetNetworkState(HTMLMediaElement::kNetworkLoading);
  double duration = 30.0;
  Media()->DurationChanged(duration, false);
  test::RunPendingTasks();
  EXPECT_EQ(Media()->SupportVideoAssistantDownload(), true);
}
#endif  // ARKWEB_VIDEO_ASSISTANT
}  // namespace blink