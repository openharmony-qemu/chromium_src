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

#include "base/i18n/rtl.h"
#include "base/run_loop.h"
#include "media/mojo/mojom/media_player.mojom-blink.h"
#include "services/media_session/public/mojom/media_session.mojom-blink.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#define protected public
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_popup_menu_element.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"
#undef private
#undef protected
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/dom/shadow_root.h"
#include "third_party/blink/renderer/core/dom/text.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/fullscreen/fullscreen.h"
#include "third_party/blink/renderer/core/geometry/dom_rect.h"
#include "third_party/blink/renderer/core/html/html_hr_element.h"
#include "third_party/blink/renderer/core/html/html_span_element.h"
#include "third_party/blink/renderer/core/html/media/html_audio_element.h"
#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#include "third_party/blink/renderer/core/loader/empty_clients.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/core/testing/internals.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_popup_menu_element_utils.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/testing/empty_web_media_player.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "ui/base/resource/resource_bundle.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Return;

namespace blink {

namespace {

class MockWebMediaPlayer : public EmptyWebMediaPlayer {
 public:
  MOCK_METHOD0(OnTimeUpdate, void());
  MOCK_CONST_METHOD0(Seekable, WebTimeRanges());
  MOCK_METHOD0(OnFrozen, void());
  MOCK_CONST_METHOD0(HasAudio, bool());
  MOCK_CONST_METHOD0(HasVideo, bool());
  MOCK_CONST_METHOD0(Duration, double());
  MOCK_CONST_METHOD0(CurrentTime, double());
  MOCK_CONST_METHOD0(IsEnded, bool());
  MOCK_CONST_METHOD0(GetNetworkState, NetworkState());
  MOCK_CONST_METHOD0(WouldTaintOrigin, bool());
  MOCK_METHOD1(SetLatencyHint, void(double));
  MOCK_METHOD1(SetWasPlayedWithUserActivationAndHighMediaEngagement,
               void(bool));
  MOCK_METHOD1(EnabledAudioTracksChanged, void(const WebVector<TrackId>&));
  MOCK_METHOD1(SelectedVideoTrackChanged, void(std::optional<TrackId>));
  MOCK_METHOD4(
      Load,
      WebMediaPlayer::LoadTiming(LoadType load_type,
                                 const blink::WebMediaPlayerSource& source,
                                 CorsMode cors_mode,
                                 bool is_cache_disabled));
  MOCK_CONST_METHOD0(DidLazyLoad, bool());
  MOCK_METHOD0(GetSrcAfterRedirects, GURL());
};

class FullscreenMockChromeClient : public EmptyChromeClient {
 public:
  void EnterFullscreen(LocalFrame& frame,
                       const FullscreenOptions*,
                       bool,
                       FullscreenRequestType
#if BUILDFLAG(ARKWEB_FULLSCREEN)
                       ,
                       const absl::optional<gfx::Size>&
#endif
                       ) override {
    Fullscreen::DidResolveEnterFullscreenRequest(*frame.GetDocument(), true);
  }
  void ExitFullscreen(LocalFrame& frame) override {
    Fullscreen::DidExitFullscreen(*frame.GetDocument());
  }
};

class WebMediaStubLocalFrameClient : public EmptyLocalFrameClient {
 public:
  explicit WebMediaStubLocalFrameClient(std::unique_ptr<WebMediaPlayer> player)
      : player_(std::move(player)) {}

  std::unique_ptr<WebMediaPlayer> CreateWebMediaPlayer(
      HTMLMediaElement&,
      const WebMediaPlayerSource&,
      WebMediaPlayerClient* client) override {
    DCHECK(player_) << " Empty injected player - already used?";
    return std::move(player_);
  }

 private:
  std::unique_ptr<WebMediaPlayer> player_;
};

class TestMediaPlayerObserver final
    : public media::mojom::blink::MediaPlayerObserver {
 public:
  struct OnMetadataChangedResult {
    bool has_audio;
    bool has_video;
    media::MediaContentType media_content_type;
  };
  void WaitUntilReceivedMessage() {
    run_loop_ = std::make_unique<base::RunLoop>();
    run_loop_->Run();
    run_loop_.reset();
  }

  void UpdateLayerRect(const ::gfx::Rect& rect) override {}
  void FullscreenChanged(bool is_fullscreen) override {}

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  void OnGetMediaTitle(const WTF::String& data) override {}
  void OnGetVideoPoster(const WTF::String& data) override {}
  void OnInitMediaTitle() override {}
  void OnInitVideoPoster() override {}
#endif
  void OnMediaPlaying() override {
    received_media_playing_ = true;
    run_loop_->Quit();
  }
  void OnMediaPaused(bool stream_ended) override {
    received_media_paused_stream_ended_ = stream_ended;
    run_loop_->Quit();
  }
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  void OnEndAVSession(bool is_hidden) override {}
#endif  // ARKWEB_MEDIA_AVSESSION
  void OnMutedStatusChanged(bool muted) override {
    received_muted_status_type_ = muted;
    run_loop_->Quit();
  }
#if BUILDFLAG(ARKWEB_ACTIVITY_STATE) || BUILDFLAG(IS_OHOS)
  void OnMediaPlayerGone() override {}
#endif
  void OnMediaMetadataChanged(bool has_audio,
                              bool has_video,
                              media::MediaContentType content_type) override {
    received_metadata_changed_result_ =
        OnMetadataChangedResult{has_audio, has_video, content_type};
    run_loop_->Quit();
  }
  void OnMediaPositionStateChanged(
      ::media_session::mojom::blink::MediaPositionPtr) override {}
  void OnMediaEffectivelyFullscreenChanged(
      blink::WebFullscreenVideoStatus status) override {}
  void OnMediaSizeChanged(const gfx::Size& size) override {
    received_media_size_ = size;
    run_loop_->Quit();
  }
  void OnPictureInPictureAvailabilityChanged(bool available) override {}
  void OnAudioOutputSinkChanged(const WTF::String& hashed_device_id) override {}
  void OnUseAudioServiceChanged(bool uses_audio_service) override {
    received_uses_audio_service_ = uses_audio_service;
    run_loop_->Quit();
  }
  void OnAudioOutputSinkChangingDisabled() override {}
  void OnRemotePlaybackMetadataChange(
      media_session::mojom::blink::RemotePlaybackMetadataPtr
          remote_playback_metadata) override {
    received_remote_playback_metadata_ = std::move(remote_playback_metadata);
    run_loop_->Quit();
  }
  void OnVideoVisibilityChanged(bool meets_visibility_threshold) override {}
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void OnVideoPlaying(media::mojom::blink::VideoAttributesForVASTPtr
                          video_attributes) override {}
  void OnUpdateVideoAttributes(media::mojom::blink::VideoAttributesForVASTPtr
                                   video_attributes) override {}
  void OnVideoDestroyed() override {}
  void OnFullScreenOverlayEnter(
      media::mojom::blink::MediaInfoForVASTPtr media_info) override {}
  void OnAVCastStarted(
      media::mojom::blink::MediaInfoForVASTPtr media_info) override {}
  void UpdatePlayStateOverlay(uint32_t playState) override {}
  void MutedChangedOverlay(bool muted) override {}
  void PlaybackRateChangedOverlay(double playback_rate) override {}
  void DurationChangedOverlay(double duration) override {}
  void TimeUpdateOverlay(double current_time) override {}
  void BufferedEndTimeChangedOverlay(double buffered_end_time) override {}
  void EndedOverlay() override {}
  void FullscreenChangedOverlay(bool fullscreen) override {}
  void SeekingOverlay() override {}
  void SeekingFinishedOverlay() override {}
  void ErrorOverlay(int32_t error_code, const WTF::String& error_msg) override {
  }
  void VideoSizeChangedOverlay(int32_t width, int32_t height) override {}
  void FullscreenOverlayChanged(bool fullscreen_overlay,
                                const WTF::String& decoder_name) override {}
  void OnVolumeChanged(double volume) override {}
#endif  // ARKWEB_VIDEO_ASSISTANT
  bool received_media_playing() const { return received_media_playing_; }
  const std::optional<bool>& received_media_paused_stream_ended() const {
    return received_media_paused_stream_ended_;
  }
  const std::optional<bool>& received_muted_status() const {
    return received_muted_status_type_;
  }
  const std::optional<OnMetadataChangedResult>&
  received_metadata_changed_result() const {
    return received_metadata_changed_result_;
  }
  gfx::Size received_media_size() const { return received_media_size_; }
  bool received_use_audio_service_changed(bool uses_audio_service) const {
    return received_uses_audio_service_.value() == uses_audio_service;
  }
  bool received_remote_playback_metadata(
      media_session::mojom::blink::RemotePlaybackMetadataPtr
          remote_playback_metadata) const {
    return received_remote_playback_metadata_ == remote_playback_metadata;
  }
#if defined(ARKWEB_MEDIA_AVSESSION)
  void OnGetMediaTitle(const WTF::String& data) override {}

  void OnGetVideoPoster(const WTF::String& data) override {}
#endif  // ARKWEB_MEDIA_AVSESSION
#if BUILDFLAG(ARKWEB_PIP)
  void OnPictureInPictureStateChanged(uint32_t state,
                                      int32_t width,
                                      int32_t height) override {}
#endif
#if BUILDFLAG(ARKWEB_UNITTESTS)
  void OnMediaCastEnter() override {}
  void OnNotifyMeidaCastUri(const WTF::String& media_uri) override {}
  void HandleStopMediaCast() override {}
  void SetPauseByAvcast(bool pause_avcast) override {}
  void UpdateRemotePlayState(bool is_playing) override {}
  void UpdateRemotePlayPosition(int64_t position) override {}
#endif // ARKWEB_UNITTESTS
 private:
  std::unique_ptr<base::RunLoop> run_loop_;
  bool received_media_playing_{false};
  std::optional<bool> received_media_paused_stream_ended_;
  std::optional<bool> received_muted_status_type_;
  std::optional<OnMetadataChangedResult> received_metadata_changed_result_;
  gfx::Size received_media_size_{0, 0};
  std::optional<bool> received_uses_audio_service_;
  media_session::mojom::blink::RemotePlaybackMetadataPtr
      received_remote_playback_metadata_;
};

class TestMediaPlayerHost final : public media::mojom::blink::MediaPlayerHost {
 public:
  void WaitForPlayer() { run_loop_.Run(); }
  void OnMediaPlayerAdded(
      mojo::PendingAssociatedRemote<media::mojom::blink::MediaPlayer>,
      mojo::PendingAssociatedReceiver<media::mojom::blink::MediaPlayerObserver>
          media_player_observer,
      int32_t player_id) override {
    receiver_.Bind(std::move(media_player_observer));
    run_loop_.Quit();
  }
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void RequestVideoAssistantConfig(
      RequestVideoAssistantConfigCallback callback) {}
#endif  // ARKWEB_VIDEO_ASSISTANT
  TestMediaPlayerObserver& observer() { return observer_; }

 private:
  TestMediaPlayerObserver observer_;
  mojo::AssociatedReceiver<media::mojom::blink::MediaPlayerObserver> receiver_{
      &observer_};
  base::RunLoop run_loop_;
};
enum class MediaTestParam { kAudio, kVideo };

}  // namespace

class MediaControlPopupMenuElementUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
    ui::ResourceBundle::InitSharedInstanceWithLocale(
        "en-US", nullptr, ui::ResourceBundle::DO_NOT_LOAD_COMMON_RESOURCES);
    auto mock_media_player = std::make_unique<MockWebMediaPlayer>();
    EXPECT_CALL(*mock_media_player, Seekable())
        .WillRepeatedly(Return(WebTimeRanges()));
    EXPECT_CALL(*mock_media_player, HasAudio()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_media_player, HasVideo()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_media_player, Duration()).WillRepeatedly(Return(1.0));
    EXPECT_CALL(*mock_media_player, CurrentTime()).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock_media_player, Load(_, _, _, _))
        .Times(AnyNumber())
        .WillRepeatedly(Return(WebMediaPlayer::LoadTiming::kImmediate));
    EXPECT_CALL(*mock_media_player, DidLazyLoad).WillRepeatedly(Return(false));
    EXPECT_CALL(*mock_media_player, WouldTaintOrigin)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_media_player, GetNetworkState)
        .WillRepeatedly(Return(WebMediaPlayer::kNetworkStateIdle));
    EXPECT_CALL(*mock_media_player, SetLatencyHint(_)).Times(AnyNumber());
    chrome_client_ = MakeGarbageCollected<FullscreenMockChromeClient>();
    dummy_page_holder_ = std::make_unique<DummyPageHolder>(
        gfx::Size(), chrome_client_,
        MakeGarbageCollected<WebMediaStubLocalFrameClient>(
            std::move(mock_media_player)));
    media_ = MakeGarbageCollected<HTMLVideoElement>(
        dummy_page_holder_->GetDocument());
    media_->SetMediaPlayerHostForTesting(
        media_player_host_receiver_.BindNewEndpointAndPassDedicatedRemote());
    controls_ = MakeGarbageCollected<MediaControlsImpl>(*media_);
    elem_ = MakeGarbageCollected<MediaControlPopupMenuElement>(*controls_);
    UpdateLifecyclePhases();
  }

  void UpdateLifecyclePhases() {
    dummy_page_holder_->GetFrameView().UpdateAllLifecyclePhasesForTest();
  }
  uint32_t IsMediaMuted() { return media_->IsMediaMuted(); }
  test::TaskEnvironment task_environment_;
  Persistent<HTMLMediaElement> media_;
  Persistent<FullscreenMockChromeClient> chrome_client_;
  std::unique_ptr<DummyPageHolder> dummy_page_holder_;
  TestMediaPlayerHost media_player_host_;
  mojo::AssociatedReceiver<media::mojom::blink::MediaPlayerHost>
      media_player_host_receiver_{&media_player_host_};
  Persistent<MediaControlsImpl> controls_;
  Persistent<MediaControlPopupMenuElement> elem_;
};

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(MediaControlPopupMenuElementUtilsTest, ShouldSetPopupAnchorHM) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  media_->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  auto* rect = MakeGarbageCollected<DOMRect>(0, 0, 100, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.ShouldSetPopupAnchorHM(rect, win));
  media_->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  ASSERT_NO_FATAL_FAILURE(elem_utils_.ShouldSetPopupAnchorHM(rect, win));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM1) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-overflow-menu-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(0, 0, 100, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("top").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM2) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(0, -400, 100, 50);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("top").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM3) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(0, 0, 100, 50);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("max-height").Contains("px"));
  EXPECT_TRUE(elem_->style()->getPropertyValue("top").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM4) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(0, 400, 100, 50);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("top").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM5) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(0, 0, 300, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("left").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM6) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(-300, 0, 400, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("left").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM7) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(0, 100, 100, 50);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("max-height").Contains("px"));
  EXPECT_TRUE(elem_->style()->getPropertyValue("top").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM8) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(-300, 0, 100, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("left").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM9) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(-100, 0, 100, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("left").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM10) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(-300, 0, 600, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("left").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM11) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(0, 0, 100, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("left").Contains("px"));
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetPopupAnchorHM12) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* internals = MakeGarbageCollected<Internals>(doc->GetExecutionContext());
  ShadowRoot* ua_root = internals->createUserAgentShadowRoot(elem_);
  elem_->setAttribute(
      html_names::kPseudoAttr,
      AtomicString("-internal-media-controls-playback-speed-list"));
  base::i18n::SetRTLForTesting(true);
  UpdateLifecyclePhases();
  auto* rect = DOMRect::Create(-232, 0, 500, 100);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetPopupAnchorHM(rect, win));
  EXPECT_TRUE(elem_->style()->getPropertyValue("left").Contains("px"));
  base::i18n::SetRTLForTesting(false);
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetOverflowPopupAnchorHM1) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* win = media_->GetDocument().domWindow();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetOverflowPopupAnchorHM(nullptr, win));
  auto* rect = DOMRect::Create(0, 0, 100, 100);
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetOverflowPopupAnchorHM(rect, nullptr));
  auto utils = MediaControlPopupMenuElementUtils(nullptr);
  ASSERT_NO_FATAL_FAILURE(utils.SetOverflowPopupAnchorHM(rect, win));
  ASSERT_NO_FATAL_FAILURE(
      elem_utils_.SetOverflowPopupAnchorHM(nullptr, nullptr));
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetOverflowPopupAnchorHM(rect, win));
  EXPECT_FALSE(elem_->style()->getPropertyValue("top").IsNull());
}

TEST_F(MediaControlPopupMenuElementUtilsTest, SetOverflowPopupAnchorHM2) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  auto* doc = &media_->GetDocument();
  auto* win = doc->domWindow();
  auto* rect = DOMRect::Create(10, 0, 100, 100);
  base::i18n::SetRTLForTesting(true);
  UpdateLifecyclePhases();
  elem_->style()->removeProperty("left", ASSERT_NO_EXCEPTION);
  ASSERT_NO_FATAL_FAILURE(elem_utils_.SetOverflowPopupAnchorHM(rect, win));
  WTF::String expected_left = WTF::String::Number(rect->left()) + "px";
  EXPECT_EQ(elem_->style()->getPropertyValue("left"), expected_left);
  base::i18n::SetRTLForTesting(false);
}

TEST_F(MediaControlPopupMenuElementUtilsTest, ShouldPlaybackSpeedButton1) {
  MediaControlPopupMenuElementUtils elem_utils_(elem_);
  media_->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  media_->SetBooleanAttribute(html_names::kControlsAttr, true);
  media_->GetDocument().body()->AppendChild(media_);
  controls_->InitializeControls();
  ASSERT_NO_FATAL_FAILURE(elem_utils_.ShouldPlaybackSpeedButton());
}
#endif  // ARKWEB_VIDEO_ASSISTANT

}  // namespace blink