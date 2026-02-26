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

#include "base/run_loop.h"
#include "media/mojo/mojom/media_player.mojom-blink.h"
#include "services/media_session/public/mojom/media_session.mojom-blink.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define protected public
#define private public
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#undef private
#undef protected

#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/fullscreen/fullscreen.h"
#include "third_party/blink/renderer/core/html/media/html_audio_element.h"

#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#include "third_party/blink/renderer/core/loader/empty_clients.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/testing/empty_web_media_player.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "third_party/blink/public/platform/web_media_player_source.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/core/html/media/media_error.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Return;

namespace blink {

namespace {

enum class TestURLScheme {
  kHttp,
  kHttps,
  kFtp,
  kFile,
  kData,
  kBlob,
};

AtomicString SrcSchemeToURL(TestURLScheme scheme) {
  switch (scheme) {
    case TestURLScheme::kHttp:
      return AtomicString("http://example.com/foo.mp4");
    case TestURLScheme::kHttps:
      return AtomicString("https://example.com/foo.mp4");
    case TestURLScheme::kFtp:
      return AtomicString("ftp://example.com/foo.mp4");
    case TestURLScheme::kFile:
      return AtomicString("file:///foo/bar.mp4");
    case TestURLScheme::kData:
      return AtomicString("data:video/mp4;base64,XXXXXXX");
    case TestURLScheme::kBlob:
      return AtomicString(
          "blob:http://example.com/00000000-0000-0000-0000-000000000000");
    default:
      NOTREACHED();
  }
}

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
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  MOCK_CONST_METHOD0(IsMediaPlayerShown, bool());
#endif
};

class FullscreenMockChromeClient : public EmptyChromeClient {
 public:
  void EnterFullscreen(LocalFrame& frame,
                       const FullscreenOptions*,
                       bool,
                       FullscreenRequestType
                       ,
                       const absl::optional<gfx::Size>&
                       ) override {
    Fullscreen::DidResolveEnterFullscreenRequest(*frame.GetDocument(),
                                                 true /* granted */);
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
#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  void OnMediaCastEnter() override {}
  void OnNotifyMeidaCastUri(const WTF::String& media_uri) override {}
  void HandleStopMediaCast() override {}
  void SetPauseByAvcast(bool pause_avcast) override {}
  void UpdateRemotePlayState(bool is_playing) override {}
  void UpdateRemotePlayPosition(int64_t position) override {}
#endif // ARKWEB_MEDIA_CAST
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

class HTMLMediaElementUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
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

    media_ = MakeGarbageCollected<HTMLAudioElement>(
        dummy_page_holder_->GetDocument());

    Media()->SetMediaPlayerHostForTesting(
        media_player_host_receiver_.BindNewEndpointAndPassDedicatedRemote());

    UpdateLifecyclePhases();
  }

  void TearDown() override {}
  void UpdateLifecyclePhases() {
    dummy_page_holder_->GetFrameView().UpdateAllLifecyclePhasesForTest();
  }

  HTMLMediaElement* Media() const { return media_.Get(); }

  test::TaskEnvironment task_environment_;
  Persistent<HTMLMediaElement> media_;
  Persistent<FullscreenMockChromeClient> chrome_client_;
  std::unique_ptr<DummyPageHolder> dummy_page_holder_;
  TestMediaPlayerHost media_player_host_;
  mojo::AssociatedReceiver<media::mojom::blink::MediaPlayerHost>
      media_player_host_receiver_{&media_player_host_};
};

class MockWebMediaPlayerClient : public MediaPlayerClient {
 public:
  MockWebMediaPlayerClient() = default;

  MockWebMediaPlayerClient(const MockWebMediaPlayerClient&) = delete;
  MockWebMediaPlayerClient& operator=(const MockWebMediaPlayerClient&) = delete;

  MOCK_METHOD0(NetworkStateChanged, void());
  MOCK_METHOD0(ReadyStateChanged, void());
  MOCK_METHOD0(TimeChanged, void());
  MOCK_METHOD0(Repaint, void());
  MOCK_METHOD0(DurationChanged, void());
  MOCK_METHOD0(SizeChanged, void());
  MOCK_METHOD1(SetCcLayer, void(cc::Layer*));
  MOCK_METHOD1(AddMediaTrack, void(const media::MediaTrack& track));
  MOCK_METHOD1(RemoveMediaTrack, void(const media::MediaTrack&));
  MOCK_METHOD1(MediaSourceOpened, void(std::unique_ptr<WebMediaSource>));
  MOCK_METHOD2(RemotePlaybackCompatibilityChanged, void(const KURL&, bool));
  MOCK_METHOD0(WasAlwaysMuted, bool());
  MOCK_METHOD0(HasSelectedVideoTrack, bool());
  MOCK_METHOD0(GetSelectedVideoTrackId, WebMediaPlayer::TrackId());
  MOCK_METHOD0(HasNativeControls, bool());
  MOCK_METHOD0(IsAudioElement, bool());
  MOCK_CONST_METHOD0(GetDisplayType, DisplayType());
  MOCK_CONST_METHOD0(IsInAutoPIP, bool());
  MOCK_METHOD1(MediaRemotingStarted, void(const WebString&));
  MOCK_METHOD1(MediaRemotingStopped, void(int));
  MOCK_METHOD0(PictureInPictureStopped, void());
  MOCK_METHOD0(OnPictureInPictureStateChange, void());
#if BUILDFLAG(ARKWEB_PIP)
  MOCK_METHOD0(UpdatePictureInPictureSurface, void());
#endif
  MOCK_CONST_METHOD0(CouldPlayIfEnoughData, bool());
  MOCK_METHOD0(ResumePlayback, void());
  MOCK_METHOD1(PausePlayback, void(MediaPlayerClient::PauseReason));
  MOCK_METHOD0(DidPlayerStartPlaying, void());
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  MOCK_METHOD1(DidEndAVSession, void(bool));
#endif  // ARKWEB_MEDIA_AVSESSION
  MOCK_METHOD1(DidPlayerPaused, void(bool));
  MOCK_METHOD0(DidPlayerGone, void());
  MOCK_METHOD1(DidPlayerMutedStatusChange, void(bool));
  MOCK_METHOD6(DidMediaMetadataChange,
               void(bool,
                    bool,
                    media::AudioCodec,
                    media::VideoCodec,
                    media::MediaContentType,
                    bool));
  MOCK_METHOD4(DidPlayerMediaPositionStateChange,
               void(double,
                    base::TimeDelta,
                    base::TimeDelta position,
                    bool end_of_media));
  MOCK_METHOD0(DidDisableAudioOutputSinkChanges, void());
  MOCK_METHOD1(DidUseAudioServiceChange, void(bool uses_audio_service));
  MOCK_METHOD1(DidPlayerSizeChange, void(const gfx::Size&));
  MOCK_METHOD1(OnRemotePlaybackDisabled, void(bool));
  MOCK_METHOD0(DidBufferUnderflow, void());
  MOCK_METHOD0(DidSeek, void());
  MOCK_METHOD2(OnFirstFrame, void(base::TimeTicks, size_t));
  MOCK_METHOD0(OnRequestVideoFrameCallback, void());
  MOCK_METHOD0(GetElementId, int());
  MOCK_METHOD0(ScheduleVideoFreezeEvent, void());
#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  MOCK_METHOD0(videoId, std::string());
  MOCK_METHOD0(hbsMediaPreloadTime, uint16_t());
  MOCK_METHOD0(hbsMediaMaxCacheTime, uint16_t());
  MOCK_METHOD0(hbsMediaMinCacheTime, uint16_t());
  MOCK_METHOD0(hbsMediaBitrate, uint16_t());
  MOCK_METHOD0(hbsMediaMoovSize, uint16_t());
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION
};

TEST_F(HTMLMediaElementUtilsTest, TestDidPlayerMutedStatusChangeExt) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  Media()->media_title_ = "1234";
  ASSERT_NO_FATAL_FAILURE(element_utils_.DidPlayerMutedStatusChangeExt(true));
  Media()->video_poster_ = "1234";
  ASSERT_NO_FATAL_FAILURE(element_utils_.DidPlayerMutedStatusChangeExt(true));
}

TEST_F(HTMLMediaElementUtilsTest, TestScheduleNamedEventUtils) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  ASSERT_NO_FATAL_FAILURE(
      element_utils_.ScheduleNamedEventUtils(event_type_names::kTimeupdate));
  ASSERT_NO_FATAL_FAILURE(element_utils_.ScheduleNamedEventUtils(
      event_type_names::kDurationchange));
  ASSERT_NO_FATAL_FAILURE(
      element_utils_.ScheduleNamedEventUtils(event_type_names::kEnded));
  ASSERT_NO_FATAL_FAILURE(
      element_utils_.ScheduleNamedEventUtils(event_type_names::kSeeking));
  ASSERT_NO_FATAL_FAILURE(
      element_utils_.ScheduleNamedEventUtils(event_type_names::kSeeked));
  ASSERT_NO_FATAL_FAILURE(
      element_utils_.ScheduleNamedEventUtils(event_type_names::kPlaying));
  ASSERT_NO_FATAL_FAILURE(
      element_utils_.ScheduleNamedEventUtils(event_type_names::kWaiting));
  ASSERT_NO_FATAL_FAILURE(
      element_utils_.ScheduleNamedEventUtils(event_type_names::kError));
  ASSERT_NO_FATAL_FAILURE(
      element_utils_.ScheduleNamedEventUtils(event_type_names::kVolumechange));
#endif  // ARKWEB_VIDEO_ASSISTANT
}

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
TEST_F(HTMLMediaElementUtilsTest, TestResetMediaPlayerAndMediaSourceUtils) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->load_state_ = HTMLMediaElement::kLoadingFromSourceElement;
  ASSERT_NO_FATAL_FAILURE(element_utils_.ResetMediaPlayerAndMediaSourceUtils());
}
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
TEST_F(HTMLMediaElementUtilsTest, TestStartRecord) {
  HTMLMediaElementUtils::Recorder* recorder_ =
      new HTMLMediaElementUtils::Recorder();
  recorder_->start_time_ = base::TimeTicks::Now() + base::Milliseconds(1000);
  ASSERT_NO_FATAL_FAILURE(recorder_->StartRecord());
}

TEST_F(HTMLMediaElementUtilsTest, TestStopRecord) {
  HTMLMediaElementUtils::Recorder* recorder_ =
      new HTMLMediaElementUtils::Recorder();
  recorder_->total_duration_ = base::TimeDelta() + base::Milliseconds(1000);
  ASSERT_NO_FATAL_FAILURE(recorder_->StopRecord());
}

TEST_F(HTMLMediaElementUtilsTest, DFX_TESTReportVideoExperience001) {
  media_ =
    MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  auto report_param = element_utils_.ReportVideoExperienceToBI();
  EXPECT_EQ(report_param.get(), nullptr);
}

TEST_F(HTMLMediaElementUtilsTest, DFX_TESTReportVideoExperience002) {
  media_ =
    MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  int32_t usage_scenario = 5;
  Media()->GetDocument().GetSettings()->SetUsageScenario(usage_scenario);
  auto report_param = element_utils_.ReportVideoExperienceToBI();
  EXPECT_EQ(report_param.get(), nullptr);
}

TEST_F(HTMLMediaElementUtilsTest, DFX_TESTReportVideoExperience003) {
  media_ =
    MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  int32_t usage_scenario = 1;
  Media()->GetDocument().GetSettings()->SetUsageScenario(usage_scenario);
  element_utils_.has_reported_experience_ = true;
  auto report_param = element_utils_.ReportVideoExperienceToBI();
  EXPECT_EQ(report_param.get(), nullptr);
}

TEST_F(HTMLMediaElementUtilsTest, DFX_TESTReportVideoExperience004) {
  media_ =
    MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  int32_t usage_scenario = 1;
  Media()->GetDocument().GetSettings()->SetUsageScenario(usage_scenario);
  element_utils_.has_reported_experience_ = false;

  auto report_param = element_utils_.ReportVideoExperienceToBI();
  EXPECT_EQ(report_param.get(), nullptr);
}

TEST_F(HTMLMediaElementUtilsTest, DFX_TESTReportVideoExperience005) {
  media_ =
    MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  int32_t usage_scenario = 1;
  Media()->GetDocument().GetSettings()->SetUsageScenario(usage_scenario);
  element_utils_.has_reported_experience_ = false;

  Media()->SetError(MakeGarbageCollected<MediaError>(MediaError::kMediaErrDecode, ""));
  auto report_param = element_utils_.ReportVideoExperienceToBI();
  EXPECT_NE(report_param.get(), nullptr);
}

TEST_F(HTMLMediaElementUtilsTest, DFX_TestGetMainUrl) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_.GetMainUrl());
}

#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

TEST_F(HTMLMediaElementUtilsTest, TestIsMediaPlayerShown) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->web_media_player_ = nullptr;
  bool result = element_utils_.IsMediaPlayerShown();
  ASSERT_EQ(result, false);
  Media()->web_media_player_ = std::make_unique<MockWebMediaPlayer>();
  auto* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  ASSERT_NE(mock_wmpi, nullptr);
  EXPECT_CALL(*mock_wmpi, IsMediaPlayerShown())
      .WillRepeatedly(testing::Return(true));
  result = element_utils_.IsMediaPlayerShown();
  ASSERT_EQ(result, true);
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(HTMLMediaElementUtilsTest, TestCollectVideoAttributesForVAST) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->video_visible_ = true;
  Media()->web_media_player_ = std::make_unique<MockWebMediaPlayer>();
  Media()->video_rect_ = gfx::RectF(10.0, 20.0, 300.0, 200.0);
  auto* mock_wmpi =
      reinterpret_cast<MockWebMediaPlayer*>(Media()->GetWebMediaPlayer());
  ASSERT_NE(mock_wmpi, nullptr);
  EXPECT_CALL(*mock_wmpi, IsMediaPlayerShown())
      .WillRepeatedly(testing::Return(true));
  media::mojom::blink::VideoAttributesForVASTPtr result =
      element_utils_.CollectVideoAttributesForVAST();
  ASSERT_FALSE(result.is_null());
  ASSERT_EQ(result->visible, true);

  Media()->video_visible_ = true;
  Media()->web_media_player_ = std::make_unique<MockWebMediaPlayer>();
  gfx::RectF rect_;
  Media()->video_rect_ = rect_;
  result = element_utils_.CollectVideoAttributesForVAST();
  ASSERT_FALSE(result.is_null());
  ASSERT_EQ(result->visible, false);

  Media()->video_visible_ = true;
  Media()->web_media_player_ = nullptr;
  Media()->video_rect_ = gfx::RectF(10.0, 20.0, 300.0, 200.0);
  result = element_utils_.CollectVideoAttributesForVAST();
  ASSERT_FALSE(result.is_null());
  ASSERT_EQ(result->visible, false);

  Media()->video_visible_ = true;
  Media()->web_media_player_ = nullptr;
  Media()->video_rect_ = rect_;
  result = element_utils_.CollectVideoAttributesForVAST();
  ASSERT_FALSE(result.is_null());
  ASSERT_EQ(result->visible, false);

  Media()->video_visible_ = false;
  Media()->web_media_player_ = std::make_unique<MockWebMediaPlayer>();
  Media()->video_rect_ = gfx::RectF(10.0, 20.0, 300.0, 200.0);
  result = element_utils_.CollectVideoAttributesForVAST();
  ASSERT_FALSE(result.is_null());
  ASSERT_EQ(result->visible, false);

  Media()->video_visible_ = false;
  Media()->web_media_player_ = std::make_unique<MockWebMediaPlayer>();
  Media()->video_rect_ = rect_;
  result = element_utils_.CollectVideoAttributesForVAST();
  ASSERT_FALSE(result.is_null());
  ASSERT_EQ(result->visible, false);

  Media()->video_visible_ = false;
  Media()->web_media_player_ = nullptr;
  Media()->video_rect_ = gfx::RectF(10.0, 20.0, 300.0, 200.0);
  result = element_utils_.CollectVideoAttributesForVAST();
  ASSERT_FALSE(result.is_null());
  ASSERT_EQ(result->visible, false);

  Media()->video_visible_ = false;
  Media()->web_media_player_ = nullptr;
  Media()->video_rect_ = rect_;
  result = element_utils_.CollectVideoAttributesForVAST();
  ASSERT_FALSE(result.is_null());
  ASSERT_EQ(result->visible, false);
}

TEST_F(HTMLMediaElementUtilsTest, TestTryNotifyVideoPlaying) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->video_assistant_enabled_ = true;
  ASSERT_NO_FATAL_FAILURE(element_utils_.TryNotifyVideoPlaying());
  Media()->video_assistant_ = true;
  ASSERT_NO_FATAL_FAILURE(element_utils_.TryNotifyVideoPlaying());
  Media()->video_assistant_ = false;
  ASSERT_NO_FATAL_FAILURE(element_utils_.TryNotifyVideoPlaying());
}

TEST_F(HTMLMediaElementUtilsTest, TestUpdateVideoAssistantAttributes) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->video_assistant_enabled_ = true;
  ASSERT_NO_FATAL_FAILURE(element_utils_.UpdateVideoAssistantAttributes());
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.UpdateVideoAssistantAttributes());
  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  Media()->video_assistant_enabled_ = true;
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.UpdateVideoAssistantAttributes());
}

TEST_F(HTMLMediaElementUtilsTest, TestNotifyVideoVisible) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->video_assistant_enabled_ = true;
  Media()->video_visible_ = true;
  ASSERT_NO_FATAL_FAILURE(element_utils_.NotifyVideoVisible(true));
  ASSERT_NO_FATAL_FAILURE(element_utils_.NotifyVideoVisible(false));
}

TEST_F(HTMLMediaElementUtilsTest, TestEnterFullScreenOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.EnterFullScreenOverlay());
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.EnterFullScreenOverlay());

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.EnterFullScreenOverlay());
}

TEST_F(HTMLMediaElementUtilsTest, TestUpdatePlayStateOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  HTMLMediaElementUtils::PlayState state_ =
      HTMLMediaElementUtils::PlayState::kPause;
  ASSERT_NO_FATAL_FAILURE(element_utils_.UpdatePlayStateOverlay(state_));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.UpdatePlayStateOverlay(state_));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.UpdatePlayStateOverlay(state_));
}

TEST_F(HTMLMediaElementUtilsTest, TestMutedChangedOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.MutedChangedOverlay(true));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.MutedChangedOverlay(true));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.MutedChangedOverlay(true));
}

TEST_F(HTMLMediaElementUtilsTest, TestPlaybackRateChangedOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.PlaybackRateChangedOverlay(1.0));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.PlaybackRateChangedOverlay(1.0));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.PlaybackRateChangedOverlay(1.0));
}

TEST_F(HTMLMediaElementUtilsTest, TestDurationChangedOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.DurationChangedOverlay(1.0));
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.DurationChangedOverlay(1.0));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.DurationChangedOverlay(1.0));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.DurationChangedOverlay(1.0));
}

TEST_F(HTMLMediaElementUtilsTest, TestTimeUpdateOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.TimeUpdateOverlay(1.0));
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.TimeUpdateOverlay(1.0));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.TimeUpdateOverlay(1.0));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.TimeUpdateOverlay(1.0));
}

TEST_F(HTMLMediaElementUtilsTest, TestBufferedEndTimeChangedOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.BufferedEndTimeChangedOverlay(1.0));
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.BufferedEndTimeChangedOverlay(1.0));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.BufferedEndTimeChangedOverlay(1.0));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.BufferedEndTimeChangedOverlay(1.0));
}

TEST_F(HTMLMediaElementUtilsTest, TestCalculateBufferedEndTime) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->duration_ = 0.00 / 0.00;
  double result = element_utils_.CalculateBufferedEndTime();
  ASSERT_EQ(result, 0);
  Media()->duration_ = 3.00 / 0;
  result = element_utils_.CalculateBufferedEndTime();
  ASSERT_EQ(result, 0);
  Media()->duration_ = 0.00;
  result = element_utils_.CalculateBufferedEndTime();
  ASSERT_EQ(result, 0);
  Media()->setCurrentTime(3.00 / 0);
  result = element_utils_.CalculateBufferedEndTime();
  ASSERT_EQ(result, 0);
  Media()->duration_ = 1.00;
  Media()->setCurrentTime(2.00);
  Media()->web_media_player_ = nullptr;
  result = element_utils_.CalculateBufferedEndTime();
  ASSERT_EQ(result, 0);
}

TEST_F(HTMLMediaElementUtilsTest, TestEndedOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.EndedOverlay());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.EndedOverlay());
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.EndedOverlay());

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.EndedOverlay());
}

TEST_F(HTMLMediaElementUtilsTest, TestFullscreenChangedOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.FullscreenChangedOverlay(true));
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.FullscreenChangedOverlay(true));
  ASSERT_NO_FATAL_FAILURE(element_utils_.FullscreenChangedOverlay(false));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.FullscreenChangedOverlay(true));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.FullscreenChangedOverlay(true));
}

TEST_F(HTMLMediaElementUtilsTest, TestSeekingOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.SeekingOverlay());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.SeekingOverlay());
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.SeekingOverlay());

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.SeekingOverlay());
}

TEST_F(HTMLMediaElementUtilsTest, TestSeekingFinishedOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.SeekingFinishedOverlay());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.SeekingFinishedOverlay());
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.SeekingFinishedOverlay());

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.SeekingFinishedOverlay());
}

TEST_F(HTMLMediaElementUtilsTest, TestErrorOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.ErrorOverlay(3, "test errorcode"));
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.ErrorOverlay(3, "test errorcode"));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.ErrorOverlay(3, String()));
  ASSERT_NO_FATAL_FAILURE(element_utils_.ErrorOverlay(3, "test errorcode"));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.ErrorOverlay(3, "test errorcode"));
}

TEST_F(HTMLMediaElementUtilsTest, TestVideoSizeChangedOverlay) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.VideoSizeChangedOverlay(1, 2));
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.VideoSizeChangedOverlay(1, 2));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.VideoSizeChangedOverlay(1, 2));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.VideoSizeChangedOverlay(1, 2));
}

TEST_F(HTMLMediaElementUtilsTest, TestOnVolumeChanged) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_.OnVolumeChanged(1.01));
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_.OnVolumeChanged(1.01));
  Media()->ResetMojoState();
  Media()->StartPlayerLoad();
  ASSERT_NO_FATAL_FAILURE(element_utils_.OnVolumeChanged(1.01));

  media_ =
      MakeGarbageCollected<HTMLAudioElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_one_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_one_.OnVolumeChanged(1.01));

  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_two_(Media());
  element_utils_two_.htmlMediaElement_ = nullptr;
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(false);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), false);
  ASSERT_NO_FATAL_FAILURE(element_utils_two_.OnVolumeChanged(1.01));
  Media()->GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  EXPECT_EQ(Media()->IsCustomMediaPlayerEnabled(), true);
  ASSERT_NO_FATAL_FAILURE(element_utils_two_.OnVolumeChanged(1.01));
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_F(HTMLMediaElementUtilsTest, VideoLoadOpt_IsUseVideoLoadOptimizationTest) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  EXPECT_EQ(element_utils_.IsUseVideoLoadOptimization(), false);
}

TEST_F(HTMLMediaElementUtilsTest, VideoLoadOpt_SetVideoIsPlayingTest) {
  media_ =
      MakeGarbageCollected<HTMLVideoElement>(dummy_page_holder_->GetDocument());
  HTMLMediaElementUtils element_utils_(Media());
  ASSERT_NO_FATAL_FAILURE(element_utils_.SetVideoIsPlaying(true));
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}  // namespace blink