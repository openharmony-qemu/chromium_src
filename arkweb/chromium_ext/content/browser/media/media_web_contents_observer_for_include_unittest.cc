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

#include "base/test/task_environment.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/media_player_listener.h"
#include "content/public/test/test_renderer_host.h"
#include "media/base/media_content_type.h"
#define private public
#include "content/browser/media/media_web_contents_observer.h"
#undef private
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/rect.h"

namespace content {
class MockMediaPlayerListener : public content::MediaPlayerListener {
 public:
  MOCK_METHOD1(OnStatusChanged, void(uint32_t));
  MOCK_METHOD1(OnMutedChanged, void(bool));
  MOCK_METHOD1(OnPlaybackRateChanged, void(double));
  MOCK_METHOD1(OnDurationChanged, void(double));
  MOCK_METHOD1(OnTimeUpdate, void(double));
  MOCK_METHOD1(OnBufferedEndTimeChanged, void(double));
  MOCK_METHOD0(OnEnded, void());
  MOCK_METHOD1(OnFullscreenChanged, void(bool));
  MOCK_METHOD0(OnSeeking, void());
  MOCK_METHOD0(OnSeekFinished, void());
  MOCK_METHOD2(OnError, void(uint32_t, const std::string&));
  MOCK_METHOD2(OnVideoSizeChanged, void(int32_t, int32_t));
  MOCK_METHOD1(OnVolumeChanged, void(double));
  MOCK_METHOD1(OnFullscreenOverlayChanged, void(bool));
};

class TestMediaPlayer : public media::mojom::MediaPlayer {
 public:
  enum class PauseRequestType {
    kNone,
    kTriggeredByUser,
    kNotTriggeredByUser,
  };

  TestMediaPlayer(MediaWebContentsObserver* media_web_contents_observer,
                  const MediaPlayerId& player_id) {
    mojo::AssociatedRemote<media::mojom::MediaPlayerHost> player_host;
    media_web_contents_observer->BindMediaPlayerHost(
        player_id.frame_routing_id,
        player_host.BindNewEndpointAndPassDedicatedReceiver());

    mojo::PendingAssociatedRemote<media::mojom::MediaPlayer> player;
    receiver_.Bind(player.InitWithNewEndpointAndPassReceiver());

    mojo::PendingAssociatedRemote<media::mojom::MediaPlayerObserver>
        dummy_player_observer;
    player_host->OnMediaPlayerAdded(
        std::move(player),
        dummy_player_observer.InitWithNewEndpointAndPassReceiver(),
        player_id.delegate_id);
    player_host.FlushForTesting();
  }

  void WaitUntilReceivedMessage() {
    run_loop_ = std::make_unique<base::RunLoop>();
    run_loop_->Run();
    run_loop_.reset();
  }

  void WaitUntilVolumeChanged() {
    run_loop_for_volume_ = std::make_unique<base::RunLoop>();
    run_loop_for_volume_->Run();
    run_loop_for_volume_.reset();
  }

  void RequestPlay() override {
    received_play_ = true;
    run_loop_->Quit();
  }

  void RequestPause(bool triggered_by_user) override {
    received_pause_type_ = triggered_by_user
                               ? PauseRequestType::kTriggeredByUser
                               : PauseRequestType::kNotTriggeredByUser;
    run_loop_->Quit();
  }

  void RequestSeekForward(base::TimeDelta seek_time) override {
    received_seek_forward_time_ = seek_time;
    run_loop_->Quit();
  }

  void RequestSeekBackward(base::TimeDelta seek_time) override {
    received_seek_backward_time_ = seek_time;
    run_loop_->Quit();
  }

  void RequestSeekTo(base::TimeDelta seek_time) override {
    received_seek_to_time_ = seek_time;
    run_loop_->Quit();
  }

  void RequestEnterPictureInPicture() override {}

  void RequestMute(bool mute) override {}

  void SetVolumeMultiplier(double multiplier) override {
    received_volume_multiplier_ = multiplier;
    if (run_loop_for_volume_) {
      run_loop_for_volume_->Quit();
    }
  }

  void SetPersistentState(bool persistent) override {}

  void SetPowerExperimentState(bool enabled) override {}

  void SetAudioSinkId(const std::string& sink_id) override {
    received_set_audio_sink_id_ = sink_id;
    run_loop_->Quit();
  }

  void SuspendForFrameClosed() override {}

  void RequestMediaRemoting() override {}

  void RequestVisibility(
      RequestVisibilityCallback request_visibility_callback) override {
    std::move(request_visibility_callback).Run(expected_visibility_);
    if (run_loop_) {
      run_loop_->Quit();
    }
  }

  void SetHtmlPlayEnabled(bool enabled) override {}

  void RequestEnterFullscreen() override {}

  void RequestExitFullscreen() override {}

  void SetPlaybackRate(double playback_rate) override {}

  void RequestDownloadUrl() override {}

  void HidePlaybackSpeedList() override {}

  void SetVideoSurface(int32_t widget_id) override {}

  void PipEnable(bool enable) override {}

  void PipDown(bool enable) override {}

  void RequestExitPictureInPicture() override {}

  void NotifyPipResize() override {}

  void PipRequestPlay() override {}

  void SetVolume(double volume) override {}

  void GetVolume(GetVolumeCallback callback) override {}

  void SetExpectedVisibility(bool expected_visibility) {
    expected_visibility_ = expected_visibility;
  }

  bool received_play() const { return received_play_; }

  PauseRequestType received_pause() const { return received_pause_type_; }

  const base::TimeDelta& received_seek_forward_time() const {
    return received_seek_forward_time_;
  }

  const base::TimeDelta& received_seek_backward_time() const {
    return received_seek_backward_time_;
  }

  const base::TimeDelta& received_seek_to_time() const {
    return received_seek_to_time_;
  }

  double received_volume_multiplier() const {
    return received_volume_multiplier_;
  }

  const std::string& received_set_audio_sink_id() const {
    return received_set_audio_sink_id_;
  }

 private:
  std::unique_ptr<base::RunLoop> run_loop_;
  std::unique_ptr<base::RunLoop> run_loop_for_volume_;
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver_{this};

  bool received_play_{false};
  double received_volume_multiplier_{0};
  PauseRequestType received_pause_type_{PauseRequestType::kNone};
  base::TimeDelta received_seek_forward_time_;
  base::TimeDelta received_seek_backward_time_;
  base::TimeDelta received_seek_to_time_;
  std::string received_set_audio_sink_id_;
  bool expected_visibility_ = false;
};

class MediaWebContentsObserverForIncludeTest
    : public RenderViewHostTestHarness {
 public:
  MediaWebContentsObserverForIncludeTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  ~MediaWebContentsObserverForIncludeTest() override = default;

  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    observer_ = std::make_unique<MediaWebContentsObserver>(
        static_cast<WebContentsImpl*>(web_contents()));
    player_id_.emplace(main_rfh()->GetGlobalId(), 1);
  }

  void TearDown() override {
    observer_.reset();
    RenderViewHostTestHarness::TearDown();
  }

 protected:
  std::unique_ptr<MediaWebContentsObserver> observer_;
  std::optional<MediaPlayerId> player_id_;
};

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
TEST_F(MediaWebContentsObserverForIncludeTest, OnMediaPlayerGone1) {
  ASSERT_EQ(observer_->player_info_map_.count(*player_id_), 0u);
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->OnMediaPlayerGone());
  EXPECT_EQ(observer_->player_info_map_.count(*player_id_), 0u);
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnMediaPlayerGone2) {
  observer_->OnMediaMetadataChanged(*player_id_, false, false,
                                    media::MediaContentType::kTransient);
  ASSERT_EQ(observer_->player_info_map_.count(*player_id_), 1u);
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->OnMediaPlayerGone());
  EXPECT_EQ(observer_->player_info_map_.count(*player_id_), 1u);
}
#endif  // BUILDFLAG(ARKWEB_ACTIVITY_STATE)

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
TEST_F(MediaWebContentsObserverForIncludeTest, SetHtmlPlayEnabled) {
  ASSERT_NE(observer_->session_controllers_manager_, nullptr);
  ASSERT_NO_FATAL_FAILURE(observer_->SetHtmlPlayEnabled(false));
  observer_->session_controllers_manager_.reset();
  ASSERT_EQ(observer_->session_controllers_manager_, nullptr);
  ASSERT_NO_FATAL_FAILURE(observer_->SetHtmlPlayEnabled(false));
}

TEST_F(MediaWebContentsObserverForIncludeTest,
       IsPlayerIdInMediaPlayerRemotesMap) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());
  EXPECT_EQ(observer_->IsPlayerIdInMediaPlayerRemotesMap(*player_id_), true);
  observer_->media_player_remotes_.clear();
  EXPECT_EQ(observer_->IsPlayerIdInMediaPlayerRemotesMap(*player_id_), false);
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER) || BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(MediaWebContentsObserverForIncludeTest, IsWebContentsAvailable1) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  EXPECT_EQ(host->IsWebContentsAvailable(), true);
}

TEST_F(MediaWebContentsObserverForIncludeTest, IsWebContentsAvailable2) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  EXPECT_EQ(host->IsWebContentsAvailable(), false);
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
TEST_F(MediaWebContentsObserverForIncludeTest, UpdateLayerRect1) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->UpdateLayerRect(gfx::Rect(1, 2, 3, 4)));
}

TEST_F(MediaWebContentsObserverForIncludeTest, UpdateLayerRect2) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  ASSERT_NO_FATAL_FAILURE(host->UpdateLayerRect(gfx::Rect(1, 2, 3, 4)));
}

TEST_F(MediaWebContentsObserverForIncludeTest, FullscreenChanged1) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->FullscreenChanged(true));
  ASSERT_NO_FATAL_FAILURE(host->FullscreenChanged(false));
}

TEST_F(MediaWebContentsObserverForIncludeTest, FullscreenChanged2) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  ASSERT_NO_FATAL_FAILURE(host->FullscreenChanged(true));
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestEnterFullscreen1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());
  ASSERT_NO_FATAL_FAILURE(observer_->RequestEnterFullscreen(*player_id_));
  observer_->media_player_remotes_.clear();
  ASSERT_NO_FATAL_FAILURE(observer_->RequestEnterFullscreen(*player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestEnterFullscreen2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  mojo::AssociatedRemote<media::mojom::MediaPlayer> remote;
  auto pending_receiver = remote.BindNewEndpointAndPassReceiver();

  TestMediaPlayer test_media_player(observer_.get(), *player_id_);
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver(
      &test_media_player);
  receiver.Bind(std::move(pending_receiver));
  observer_->media_player_remotes_.emplace(*player_id_, std::move(remote));
  ASSERT_NO_FATAL_FAILURE(observer_->RequestEnterFullscreen(*player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestExitFullscreen1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());
  ASSERT_NO_FATAL_FAILURE(observer_->RequestExitFullscreen(*player_id_));
  observer_->media_player_remotes_.clear();
  ASSERT_NO_FATAL_FAILURE(observer_->RequestExitFullscreen(*player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestExitFullscreen2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  mojo::AssociatedRemote<media::mojom::MediaPlayer> remote;
  auto pending_receiver = remote.BindNewEndpointAndPassReceiver();
  TestMediaPlayer test_media_player(observer_.get(), *player_id_);
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver(
      &test_media_player);
  receiver.Bind(std::move(pending_receiver));
  observer_->media_player_remotes_.emplace(*player_id_, std::move(remote));
  ASSERT_NO_FATAL_FAILURE(observer_->RequestExitFullscreen(*player_id_));
}
#endif  // BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
TEST_F(MediaWebContentsObserverForIncludeTest, OnGetMediaTitle) {
  ASSERT_TRUE(player_id_.has_value());
  auto host_true =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host_true->OnGetMediaTitle("test"));

  auto host_false =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  ASSERT_NO_FATAL_FAILURE(host_false->OnGetMediaTitle("test"));
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnGetVideoPoster) {
  ASSERT_TRUE(player_id_.has_value());
  auto host_true =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host_true->OnGetVideoPoster("test"));
  auto host_false =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  ASSERT_NO_FATAL_FAILURE(host_false->OnGetVideoPoster("test"));
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnInitMediaTitle) {
  ASSERT_TRUE(player_id_.has_value());
  auto host_true =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host_true->OnInitMediaTitle());

  auto host_false =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  ASSERT_NO_FATAL_FAILURE(host_false->OnInitMediaTitle());
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnInitVideoPoster) {
  ASSERT_TRUE(player_id_.has_value());
  auto host_true =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host_true->OnInitVideoPoster());
  auto host_false =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  ASSERT_NO_FATAL_FAILURE(host_false->OnInitVideoPoster());
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnEndAVSession) {
  ASSERT_TRUE(player_id_.has_value());
  auto host_true =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host_true->OnEndAVSession(true));

  auto host_false =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  ASSERT_NO_FATAL_FAILURE(host_false->OnEndAVSession(true));
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_AVSESSION)

TEST_F(MediaWebContentsObserverForIncludeTest, IsMediaPlaying1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_.reset();
  observer_ = std::make_unique<MediaWebContentsObserver>(
      static_cast<WebContentsImpl*>(web_contents()));
  EXPECT_EQ(observer_->IsMediaPlaying(*player_id_), false);
}

TEST_F(MediaWebContentsObserverForIncludeTest, IsMediaPlaying2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_.reset();
  observer_ = std::make_unique<MediaWebContentsObserver>(
      static_cast<WebContentsImpl*>(web_contents()));
  observer_->OnMediaMetadataChanged(*player_id_, false, false,
                                    media::MediaContentType::kTransient);
  EXPECT_EQ(observer_->IsMediaPlaying(*player_id_), false);
}

TEST_F(MediaWebContentsObserverForIncludeTest, IsMediaPlaying3) {
  ASSERT_TRUE(player_id_.has_value());
  observer_.reset();
  observer_ = std::make_unique<MediaWebContentsObserver>(
      static_cast<WebContentsImpl*>(web_contents()));
  observer_->OnMediaMetadataChanged(*player_id_, false, false,
                                    media::MediaContentType::kTransient);
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  host->OnMediaPlaying();
  EXPECT_EQ(observer_->IsMediaPlaying(*player_id_), true);
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(MediaWebContentsObserverForIncludeTest, SetPlaybackRate1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();

  mojo::AssociatedRemote<media::mojom::MediaPlayer> remote;
  auto pending_receiver = remote.BindNewEndpointAndPassReceiver();

  TestMediaPlayer test_media_player(observer_.get(), *player_id_);
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver(
      &test_media_player);
  receiver.Bind(std::move(pending_receiver));
  observer_->media_player_remotes_.emplace(*player_id_, std::move(remote));
  ASSERT_NO_FATAL_FAILURE(observer_->SetPlaybackRate(0.0, *player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, SetPlaybackRate2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());

  ASSERT_NO_FATAL_FAILURE(observer_->SetPlaybackRate(0.0, *player_id_));

  observer_->media_player_remotes_.clear();
  ASSERT_NO_FATAL_FAILURE(observer_->SetPlaybackRate(0.0, *player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, SetVolume1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  mojo::AssociatedRemote<media::mojom::MediaPlayer> remote;
  auto pending_receiver = remote.BindNewEndpointAndPassReceiver();
  TestMediaPlayer test_media_player(observer_.get(), *player_id_);
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver(
      &test_media_player);
  receiver.Bind(std::move(pending_receiver));
  observer_->media_player_remotes_.emplace(*player_id_, std::move(remote));
  ASSERT_NO_FATAL_FAILURE(observer_->SetVolume(0.0, *player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, SetVolume2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());
  ASSERT_NO_FATAL_FAILURE(observer_->SetVolume(0.0, *player_id_));
  observer_->media_player_remotes_.clear();
  ASSERT_NO_FATAL_FAILURE(observer_->SetVolume(0.0, *player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, GetVolume1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  mojo::AssociatedRemote<media::mojom::MediaPlayer> remote;
  auto pending_receiver = remote.BindNewEndpointAndPassReceiver();
  TestMediaPlayer test_media_player(observer_.get(), *player_id_);
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver(
      &test_media_player);
  receiver.Bind(std::move(pending_receiver));
  observer_->media_player_remotes_.emplace(*player_id_, std::move(remote));
  EXPECT_EQ(observer_->GetVolume(*player_id_), -1.0);
}

TEST_F(MediaWebContentsObserverForIncludeTest, GetVolume2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  EXPECT_EQ(observer_->GetVolume(*player_id_), -1.0);
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());
  EXPECT_EQ(observer_->GetVolume(*player_id_), -1.0);
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestFullScreen1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());
  ASSERT_NO_FATAL_FAILURE(observer_->RequestFullScreen(false, *player_id_));
  observer_->media_player_remotes_.clear();
  ASSERT_NO_FATAL_FAILURE(observer_->RequestFullScreen(false, *player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestFullScreen2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  mojo::AssociatedRemote<media::mojom::MediaPlayer> remote;
  auto pending_receiver = remote.BindNewEndpointAndPassReceiver();
  TestMediaPlayer test_media_player(observer_.get(), *player_id_);
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver(
      &test_media_player);
  receiver.Bind(std::move(pending_receiver));
  observer_->media_player_remotes_.emplace(*player_id_, std::move(remote));
  ASSERT_NO_FATAL_FAILURE(observer_->RequestFullScreen(false, *player_id_));
  ASSERT_NO_FATAL_FAILURE(observer_->RequestFullScreen(true, *player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestDownloadUrl1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());
  ASSERT_NO_FATAL_FAILURE(observer_->RequestDownloadUrl(*player_id_));
  observer_->media_player_remotes_.clear();
  ASSERT_NO_FATAL_FAILURE(observer_->RequestDownloadUrl(*player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestDownloadUrl2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  mojo::AssociatedRemote<media::mojom::MediaPlayer> remote;
  auto pending_receiver = remote.BindNewEndpointAndPassReceiver();
  TestMediaPlayer test_media_player(observer_.get(), *player_id_);
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver(
      &test_media_player);
  receiver.Bind(std::move(pending_receiver));
  observer_->media_player_remotes_.emplace(*player_id_, std::move(remote));
  ASSERT_NO_FATAL_FAILURE(observer_->RequestDownloadUrl(*player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, HidePlaybackSpeedList1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  observer_->media_player_remotes_.emplace(
      *player_id_, mojo::AssociatedRemote<media::mojom::MediaPlayer>());
  ASSERT_NO_FATAL_FAILURE(observer_->HidePlaybackSpeedList(*player_id_));
  observer_->media_player_remotes_.clear();
  ASSERT_NO_FATAL_FAILURE(observer_->HidePlaybackSpeedList(*player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, HidePlaybackSpeedList2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();
  mojo::AssociatedRemote<media::mojom::MediaPlayer> remote;
  auto pending_receiver = remote.BindNewEndpointAndPassReceiver();
  TestMediaPlayer test_media_player(observer_.get(), *player_id_);
  mojo::AssociatedReceiver<media::mojom::MediaPlayer> receiver(
      &test_media_player);
  receiver.Bind(std::move(pending_receiver));
  observer_->media_player_remotes_.emplace(*player_id_, std::move(remote));
  ASSERT_NO_FATAL_FAILURE(observer_->HidePlaybackSpeedList(*player_id_));
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestVideoAssistantConfig1) {
  bool called = false;
  MediaWebContentsObserver::MediaPlayerHostImpl host(main_rfh()->GetGlobalId(),
                                                     observer_.get());
  host.RequestVideoAssistantConfig(base::BindOnce(
      [](bool* flag, media::mojom::VideoAssistantConfigPtr) { *flag = true; },
      &called));
  EXPECT_EQ(called, true);
}

TEST_F(MediaWebContentsObserverForIncludeTest, RequestVideoAssistantConfig2) {
  bool called = false;
  MediaWebContentsObserver::MediaPlayerHostImpl host(main_rfh()->GetGlobalId(),
                                                     nullptr);
  host.RequestVideoAssistantConfig(base::BindOnce(
      [](bool* flag, media::mojom::VideoAssistantConfigPtr) { *flag = true; },
      &called));
  EXPECT_EQ(called, false);
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnVideoDestroyed1) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  auto attrs = media::mojom::VideoAttributesForVAST::New();
  ASSERT_NO_FATAL_FAILURE(host->OnVideoPlaying(std::move(attrs)));
  ASSERT_NO_FATAL_FAILURE(host->OnUpdateVideoAttributes(std::move(attrs)));
  ASSERT_NO_FATAL_FAILURE(host->OnVideoDestroyed());
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnVideoDestroyed2) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  auto attrs = media::mojom::VideoAttributesForVAST::New();
  ASSERT_NO_FATAL_FAILURE(host->OnVideoPlaying(std::move(attrs)));
  ASSERT_NO_FATAL_FAILURE(host->OnUpdateVideoAttributes(std::move(attrs)));
  ASSERT_NO_FATAL_FAILURE(host->OnVideoDestroyed());
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnFullScreenOverlayEnter1) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->OnFullScreenOverlayEnter(nullptr));
  auto info = media::mojom::MediaInfoForVAST::New();
  ASSERT_NO_FATAL_FAILURE(host->OnFullScreenOverlayEnter(std::move(info)));
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnFullScreenOverlayEnter2) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  auto info = media::mojom::MediaInfoForVAST::New();
  ASSERT_NO_FATAL_FAILURE(host->OnFullScreenOverlayEnter(std::move(info)));
}

TEST_F(MediaWebContentsObserverForIncludeTest, UpdatePlayStateOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->UpdatePlayStateOverlay(1));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnStatusChanged(1u)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->UpdatePlayStateOverlay(1));
}

TEST_F(MediaWebContentsObserverForIncludeTest, MutedChangedOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->MutedChangedOverlay(false));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnMutedChanged(false)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->MutedChangedOverlay(false));
}

TEST_F(MediaWebContentsObserverForIncludeTest, PlaybackRateChangedOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->PlaybackRateChangedOverlay(0.0));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnPlaybackRateChanged(0.0)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->PlaybackRateChangedOverlay(0.0));
}

TEST_F(MediaWebContentsObserverForIncludeTest, DurationChangedOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->DurationChangedOverlay(0.0));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnDurationChanged(0.0)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->DurationChangedOverlay(0.0));
}

TEST_F(MediaWebContentsObserverForIncludeTest, TimeUpdateOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->TimeUpdateOverlay(0.0));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnTimeUpdate(0.0)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->TimeUpdateOverlay(0.0));
}

TEST_F(MediaWebContentsObserverForIncludeTest, BufferedEndTimeChangedOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->BufferedEndTimeChangedOverlay(0.0));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnBufferedEndTimeChanged(0.0)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->BufferedEndTimeChangedOverlay(0.0));
}

TEST_F(MediaWebContentsObserverForIncludeTest, EndedOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->EndedOverlay());
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnEnded()).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->EndedOverlay());
}

TEST_F(MediaWebContentsObserverForIncludeTest, FullscreenChangedOverlay1) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->FullscreenChangedOverlay(true));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnFullscreenChanged(true)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->FullscreenChangedOverlay(true));
}

TEST_F(MediaWebContentsObserverForIncludeTest, FullscreenChangedOverlay2) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);
  EXPECT_CALL(*listener, OnFullscreenChanged(false)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->FullscreenChangedOverlay(false));
}

TEST_F(MediaWebContentsObserverForIncludeTest, SeekingOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->SeekingOverlay());
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnSeeking()).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->SeekingOverlay());
}

TEST_F(MediaWebContentsObserverForIncludeTest, SeekingFinishedOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->SeekingFinishedOverlay());
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnSeekFinished()).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->SeekingFinishedOverlay());
}

TEST_F(MediaWebContentsObserverForIncludeTest, ErrorOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->ErrorOverlay(1, "err"));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnError(1, "err")).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->ErrorOverlay(1, "err"));
}

TEST_F(MediaWebContentsObserverForIncludeTest, VideoSizeChangedOverlay) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->VideoSizeChangedOverlay(1, 1));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnVideoSizeChanged(1, 1)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->VideoSizeChangedOverlay(1, 1));
}

TEST_F(MediaWebContentsObserverForIncludeTest, OnVolumeChanged) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->OnVolumeChanged(0.0));
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnVolumeChanged(0.0)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->OnVolumeChanged(0.0));
}

TEST_F(MediaWebContentsObserverForIncludeTest, FullscreenOverlayChanged1) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());

  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  host->media_player_listener_ = std::move(listener_uptr);

  EXPECT_CALL(*listener, OnFullscreenOverlayChanged(true)).Times(1);
  ASSERT_NO_FATAL_FAILURE(host->FullscreenOverlayChanged(true, "avc1"));
}

TEST_F(MediaWebContentsObserverForIncludeTest, FullscreenOverlayChanged2) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  EXPECT_CALL(*listener, OnFullscreenOverlayChanged(true)).Times(0);
  ASSERT_NO_FATAL_FAILURE(host->FullscreenOverlayChanged(true, "avc1"));
}

TEST_F(MediaWebContentsObserverForIncludeTest, FullscreenOverlayChanged3) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  auto listener_uptr =
      std::make_unique<testing::NiceMock<MockMediaPlayerListener>>();
  auto* listener = listener_uptr.get();
  EXPECT_CALL(*listener, OnFullscreenOverlayChanged(true)).Times(0);
  ASSERT_NO_FATAL_FAILURE(host->FullscreenOverlayChanged(true, "avc1"));
}
#endif  // BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(MediaWebContentsObserverForIncludeTest, GetMediaPlayerId1) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();

  const int delegate_id = 11;
  const auto gid = main_rfh()->GetGlobalId();
  bool status = true;
  content::MediaPlayerId id = observer_->GetMediaPlayerId(
      delegate_id, gid.child_id, gid.frame_routing_id, status);

  EXPECT_FALSE(status);
  EXPECT_EQ(id.delegate_id, delegate_id);
  EXPECT_EQ(id.frame_routing_id.child_id, gid.child_id);
  EXPECT_EQ(id.frame_routing_id.frame_routing_id, gid.frame_routing_id);
}

TEST_F(MediaWebContentsObserverForIncludeTest, GetMediaPlayerId2) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();

  const int delegate_id = 22;
  const auto gid = main_rfh()->GetGlobalId();
  content::MediaPlayerId existing_id(
      content::GlobalRenderFrameHostId(gid.child_id, gid.frame_routing_id),
      delegate_id);

  observer_->media_player_remotes_.emplace(
      existing_id, mojo::AssociatedRemote<media::mojom::MediaPlayer>());

  bool status = false;
  content::MediaPlayerId id = observer_->GetMediaPlayerId(
      delegate_id, gid.child_id, gid.frame_routing_id, status);

  EXPECT_TRUE(status);
  EXPECT_EQ(id, existing_id);
}

TEST_F(MediaWebContentsObserverForIncludeTest, GetMediaPlayerId3) {
  ASSERT_TRUE(player_id_.has_value());
  observer_->media_player_remotes_.clear();

  const auto gid = main_rfh()->GetGlobalId();

  content::MediaPlayerId other_id(
      content::GlobalRenderFrameHostId(gid.child_id, gid.frame_routing_id),
      999);
  observer_->media_player_remotes_.emplace(
      other_id, mojo::AssociatedRemote<media::mojom::MediaPlayer>());

  const int query_delegate_id = 111;
  bool status = true;
  content::MediaPlayerId ret = observer_->GetMediaPlayerId(
      query_delegate_id, gid.child_id, gid.frame_routing_id, status);

  EXPECT_FALSE(status);
  EXPECT_EQ(ret.delegate_id, query_delegate_id);
  EXPECT_EQ(ret.frame_routing_id.child_id, gid.child_id);
  EXPECT_EQ(ret.frame_routing_id.frame_routing_id, gid.frame_routing_id);
  EXPECT_NE(ret, other_id);
}

TEST_F(MediaWebContentsObserverForIncludeTest,
       OnPictureInPictureStateChanged1) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, observer_.get());
  ASSERT_NO_FATAL_FAILURE(host->OnPictureInPictureStateChanged(1, 2, 3));
}

TEST_F(MediaWebContentsObserverForIncludeTest,
       OnPictureInPictureStateChanged2) {
  ASSERT_TRUE(player_id_.has_value());
  auto host =
      std::make_unique<MediaWebContentsObserver::MediaPlayerObserverHostImpl>(
          *player_id_, nullptr);
  ASSERT_NO_FATAL_FAILURE(host->OnPictureInPictureStateChanged(1, 2, 3));
}
#endif  // BUILDFLAG(ARKWEB_PIP)
}  // namespace content