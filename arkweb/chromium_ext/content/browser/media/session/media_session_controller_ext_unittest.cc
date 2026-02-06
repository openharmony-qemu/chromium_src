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

#include "content/browser/media/session/media_session_controller_ext.h"

#include "content/browser/web_contents/web_contents_impl.h"
#define private public
#include "content/browser/media/media_web_contents_observer.h"
#undef private

#include "content/browser/media/session/media_session_impl.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "media/base/media_content_type.h"
#include "media/mojo/mojom/media_player.mojom.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::_;
using ::testing::Return;

namespace content {
namespace {
constexpr int kMilliseconds = 1000;
}
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

#if BUILDFLAG(ARKWEB_UNITTESTS)
  void PullUpCastBackGround(const std::string& device_name) override {}
  void UpdateUiPlayState(bool is_playing) override {}
  void UpdateUiPlayPosition(int64_t position) override {}
  void MediaCastStopped() override {}
  void MediaCastStopByNavigation() override {}
  void GetMediaCastCurrentTime(GetMediaCastCurrentTimeCallback callback) override {}
  void NotifyRemoteExitFullScreen() override {}
  void NotifyCastControlShow(bool is_show) override {}
#endif // ARKWEB_UNITTESTS

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

class MediaSessionControllerExtTest : public RenderViewHostTestHarness {
 public:
  MediaSessionControllerExtTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  ~MediaSessionControllerExtTest() override = default;

  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    web_contents_impl_ = static_cast<WebContentsImpl*>(web_contents());

    web_contents_impl_->media_web_contents_observer_.reset(
        new MediaWebContentsObserver(web_contents_impl_));
    media_session_ = MediaSessionImpl::Get(web_contents());

    id_.emplace(main_rfh()->GetGlobalId(), 1);
    controller_ =
        std::make_unique<MediaSessionControllerExt>(*id_, web_contents_impl_);
  }

  void TearDown() override {
    controller_.reset();
    RenderViewHostTestHarness::TearDown();
  }

  WebContentsImpl* web_contents_impl_ = nullptr;
  raw_ptr<MediaSessionImpl> media_session_ = nullptr;
  std::optional<MediaPlayerId> id_;
  std::unique_ptr<MediaSessionControllerExt> controller_;
};

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
TEST_F(MediaSessionControllerExtTest, OnSetHtmlPlayEnabled1) {
  ASSERT_NE(controller_, nullptr);
  auto backup = std::move(web_contents_impl_->media_web_contents_observer_);
  ASSERT_EQ(web_contents_impl_->media_web_contents_observer(), nullptr);
  ASSERT_NO_FATAL_FAILURE(
      controller_->OnSetHtmlPlayEnabled(controller_->player_id_, false));
  web_contents_impl_->media_web_contents_observer_ = std::move(backup);
}

TEST_F(MediaSessionControllerExtTest, OnSetHtmlPlayEnabled2) {
  ASSERT_NE(controller_, nullptr);
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) =
      nullptr;
  ASSERT_NO_FATAL_FAILURE(
      controller_->OnSetHtmlPlayEnabled(controller_->player_id_, true));
}

TEST_F(MediaSessionControllerExtTest, SetHtmlPlayEnabled) {
  ASSERT_NE(controller_, nullptr);
  auto* mwo = web_contents_impl_->media_web_contents_observer();
  ASSERT_NE(mwo, nullptr);
  TestMediaPlayer player(mwo, controller_->id_);

  ASSERT_NO_FATAL_FAILURE(controller_->SetHtmlPlayEnabled(true));
}

TEST_F(MediaSessionControllerExtTest, SetSessionStateIfNeed1) {
  ASSERT_NE(controller_, nullptr);
  controller_->media_content_type_ = media::MediaContentType::kOneShot;
  ASSERT_NO_FATAL_FAILURE(controller_->SetSessionStateIfNeed(true));
}
TEST_F(MediaSessionControllerExtTest, SetSessionStateIfNeed2) {
  ASSERT_NE(controller_, nullptr);
  controller_->media_content_type_ = media::MediaContentType::kPersistent;
  ASSERT_NO_FATAL_FAILURE(controller_->SetSessionStateIfNeed(true));
  ASSERT_NO_FATAL_FAILURE(controller_->SetSessionStateIfNeed(false));
}

TEST_F(MediaSessionControllerExtTest, SetSessionStateIfNeed3) {
  ASSERT_NE(controller_, nullptr);
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) =
      nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->SetSessionStateIfNeed(true));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) =
      media_session_;
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(MediaSessionControllerExtTest, OnPictureInPictureStateChanged1) {
  ASSERT_NE(controller_, nullptr);
  ASSERT_NO_FATAL_FAILURE(
      controller_->OnPictureInPictureStateChanged(controller_->id_, 1, 10, 20));
}

TEST_F(MediaSessionControllerExtTest, OnPictureInPictureStateChanged2) {
  ASSERT_NE(controller_, nullptr);
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) =
      nullptr;
  ASSERT_NO_FATAL_FAILURE(
      controller_->OnPictureInPictureStateChanged(controller_->id_, 1, 10, 20));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) =
      media_session_;
}
#endif  // BUILDFLAG(ARKWEB_PIP)

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
TEST_F(MediaSessionControllerExtTest, OnEndAVSession1) {
  ASSERT_NE(controller_, nullptr);
  ASSERT_NO_FATAL_FAILURE(controller_->OnEndAVSession(controller_->id_, true));
  ASSERT_NO_FATAL_FAILURE(controller_->OnEndAVSession(controller_->id_, false));
  ASSERT_NO_FATAL_FAILURE(controller_->OnEndAVSession(controller_->id_, true));
}

TEST_F(MediaSessionControllerExtTest, OnEndAVSession2) {
  ASSERT_NE(controller_, nullptr);
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) =
      nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->OnEndAVSession(controller_->id_, true));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) =
      media_session_;
}

#endif  // BUILDFLAG(ARKWEB_MEDIA_AVSESSION)

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
TEST_F(MediaSessionControllerExtTest, OnNotifyMeidaCastUri) {
  ASSERT_NE(controller_, nullptr);
  const std::string test_uri = "http://example.com/video.mp4";
  ASSERT_NO_FATAL_FAILURE(controller_->OnNotifyMeidaCastUri(test_uri));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->OnNotifyMeidaCastUri(test_uri));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = media_session_;
}

TEST_F(MediaSessionControllerExtTest, CreateAVCastAdapter) {
  ASSERT_NE(controller_, nullptr);
  ASSERT_NO_FATAL_FAILURE(controller_->CreateAVCastAdapter());
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->CreateAVCastAdapter());
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = media_session_;
}

TEST_F(MediaSessionControllerExtTest, HandleStopMediaCast) {
  ASSERT_NE(controller_, nullptr);
  ASSERT_NO_FATAL_FAILURE(controller_->HandleStopMediaCast());
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->HandleStopMediaCast());
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = media_session_;
}

TEST_F(MediaSessionControllerExtTest, UpdateRemotePlayState) {
  ASSERT_NE(controller_, nullptr);
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateRemotePlayState(true));
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateRemotePlayState(false));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateRemotePlayState(true));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = media_session_;
}

TEST_F(MediaSessionControllerExtTest, UpdateRemotePlayPosition) {
  ASSERT_NE(controller_, nullptr);
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateRemotePlayPosition(123456));
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateRemotePlayPosition(0));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateRemotePlayPosition(987654));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = media_session_;
}

TEST_F(MediaSessionControllerExtTest, GetMediaCastCurrentTime) {
  ASSERT_NE(controller_, nullptr);
  auto* mwo = web_contents_impl_->media_web_contents_observer();
  ASSERT_NE(mwo, nullptr);
  TestMediaPlayer player(mwo, controller_->id_);
  int32_t time = controller_->GetMediaCastCurrentTime(controller_->player_id_);
  LOG(INFO) << "GetMediaCastCurrentTime returned: " << time;
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = nullptr;
  time = controller_->GetMediaCastCurrentTime(controller_->player_id_);
  ASSERT_EQ(time, 0) << "Should return 0 when web_contents_ is null";
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = web_contents_impl_;
  auto backup = std::move(web_contents_impl_->media_web_contents_observer_);
  time = controller_->GetMediaCastCurrentTime(controller_->player_id_);
  ASSERT_EQ(time, 0) << "Should return 0 when media_web_contents_observer is null";
  web_contents_impl_->media_web_contents_observer_ = std::move(backup);
}

TEST_F(MediaSessionControllerExtTest, PullUpCastBackGround) {
  ASSERT_NE(controller_, nullptr);
  auto* mwo = web_contents_impl_->media_web_contents_observer();
  ASSERT_NE(mwo, nullptr);
  TestMediaPlayer player(mwo, controller_->id_);
  const std::string device_name = "Test_Device";
  ASSERT_NO_FATAL_FAILURE(controller_->PullUpCastBackGround(
      controller_->player_id_, device_name));
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->PullUpCastBackGround(
      controller_->player_id_, device_name));
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = web_contents_impl_;
  auto backup = std::move(web_contents_impl_->media_web_contents_observer_);
  ASSERT_NO_FATAL_FAILURE(controller_->PullUpCastBackGround(
      controller_->player_id_, device_name));
  web_contents_impl_->media_web_contents_observer_ = std::move(backup);
}

TEST_F(MediaSessionControllerExtTest, MediaCastStopped) {
  ASSERT_NE(controller_, nullptr);
  auto* mwo = web_contents_impl_->media_web_contents_observer();
  ASSERT_NE(mwo, nullptr);
  TestMediaPlayer player(mwo, controller_->id_);
  ASSERT_NO_FATAL_FAILURE(controller_->MediaCastStopped(controller_->player_id_));
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->MediaCastStopped(controller_->player_id_));
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = web_contents_impl_;
  auto backup = std::move(web_contents_impl_->media_web_contents_observer_);
  ASSERT_NO_FATAL_FAILURE(controller_->MediaCastStopped(controller_->player_id_));
  web_contents_impl_->media_web_contents_observer_ = std::move(backup);
}

TEST_F(MediaSessionControllerExtTest, UpdateUiPlayState) {
  ASSERT_NE(controller_, nullptr);
  auto* mwo = web_contents_impl_->media_web_contents_observer();
  ASSERT_NE(mwo, nullptr);
  TestMediaPlayer player(mwo, controller_->id_);
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateUiPlayState(
      controller_->player_id_, true));
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateUiPlayState(
      controller_->player_id_, false));
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateUiPlayState(
      controller_->player_id_, true));
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = web_contents_impl_;
  auto backup = std::move(web_contents_impl_->media_web_contents_observer_);
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateUiPlayState(
      controller_->player_id_, true));
  web_contents_impl_->media_web_contents_observer_ = std::move(backup);
}

TEST_F(MediaSessionControllerExtTest, UpdateUiPlayPosition) {
  ASSERT_NE(controller_, nullptr);
  auto* mwo = web_contents_impl_->media_web_contents_observer();
  ASSERT_NE(mwo, nullptr);
  TestMediaPlayer player(mwo, controller_->id_);
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateUiPlayPosition(
      controller_->player_id_, 123456789));
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateUiPlayPosition(
      controller_->player_id_, 0));
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateUiPlayPosition(
      controller_->player_id_, 987654321));
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = web_contents_impl_;
  auto backup = std::move(web_contents_impl_->media_web_contents_observer_);
  ASSERT_NO_FATAL_FAILURE(controller_->UpdateUiPlayPosition(
      controller_->player_id_, 555555));
  web_contents_impl_->media_web_contents_observer_ = std::move(backup);
}

TEST_F(MediaSessionControllerExtTest, MediaCastStopByNavigation) {
  ASSERT_NE(controller_, nullptr);
  auto* mwo = web_contents_impl_->media_web_contents_observer();
  ASSERT_NE(mwo, nullptr);
  TestMediaPlayer player(mwo, controller_->id_);
  ASSERT_NO_FATAL_FAILURE(controller_->MediaCastStopByNavigation());
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->MediaCastStopByNavigation());
  const_cast<raw_ptr<content::WebContentsImpl>&>(controller_->web_contents_) = web_contents_impl_;
  auto backup = std::move(web_contents_impl_->media_web_contents_observer_);
  ASSERT_NO_FATAL_FAILURE(controller_->MediaCastStopByNavigation());
  web_contents_impl_->media_web_contents_observer_ = std::move(backup);
}

TEST_F(MediaSessionControllerExtTest, SetPauseByAvcast) {
  ASSERT_NE(controller_, nullptr);
  ASSERT_NO_FATAL_FAILURE(controller_->SetPauseByAvcast(true));
  ASSERT_NO_FATAL_FAILURE(controller_->SetPauseByAvcast(false));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(controller_->SetPauseByAvcast(true));
  const_cast<raw_ptr<content::MediaSessionImpl>&>(controller_->media_session_) = media_session_;
}
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)
}  // namespace content