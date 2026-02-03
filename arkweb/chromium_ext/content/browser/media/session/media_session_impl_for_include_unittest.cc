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

#include "base/functional/bind.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/media/session/media_session_ohos.h"
#include "content/browser/media/session/mock_media_session_player_observer.h"
#include "content/browser/picture_in_picture/video_picture_in_picture_window_controller_impl.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "media/base/media_content_type.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::_;
using ::testing::Return;

namespace content {

class VideoMockObserver : public content::MockMediaSessionPlayerObserver {
 public:
  using content::MockMediaSessionPlayerObserver::MockMediaSessionPlayerObserver;
  bool HasVideo(int player_id) const override { return true; }
};

class MediaSessionImplForIncludeTest : public RenderViewHostTestHarness {
 public:
  MediaSessionImplForIncludeTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}
  ~MediaSessionImplForIncludeTest() override = default;

  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    media_session_ = MediaSessionImpl::Get(web_contents());
    player_observer_ = std::make_unique<MockMediaSessionPlayerObserver>(
        main_rfh(), media::MediaContentType::kPersistent);
  }

  void TearDown() override {
    player_observer_.reset();
    RenderViewHostTestHarness::TearDown();
  }

  raw_ptr<MediaSessionImpl> media_session_;
  std::unique_ptr<MockMediaSessionPlayerObserver> player_observer_;
};

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
TEST_F(MediaSessionImplForIncludeTest, NWebGetState1) {
  EXPECT_EQ(media_session_->NWebGetState(),
            MediaSessionImpl::NWebPlaybackState::NONE);
}

TEST_F(MediaSessionImplForIncludeTest, NWebGetState2) {
  int player_id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), player_id);
  media_session_->Suspend(MediaSession::SuspendType::kSystem);
  EXPECT_EQ(media_session_->NWebGetState(),
            MediaSessionImpl::NWebPlaybackState::PAUSED);
  media_session_->RemovePlayer(player_observer_.get(), player_id);
}

TEST_F(MediaSessionImplForIncludeTest, NWebGetState3) {
  int player_id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), player_id);
  player_observer_->SetPlaying(player_id, true);
  EXPECT_TRUE(media_session_->IsActive());
  EXPECT_FALSE(media_session_->IsSuspended());
  EXPECT_EQ(media_session_->NWebGetState(),
            MediaSessionImpl::NWebPlaybackState::PLAYING);
  media_session_->RemovePlayer(player_observer_.get(), player_id);
}

TEST_F(MediaSessionImplForIncludeTest, IsEndOfMedia1) {
  EXPECT_FALSE(media_session_->IsEndOfMedia());
}

TEST_F(MediaSessionImplForIncludeTest, IsEndOfMedia2) {
  media_session::MediaPosition mock_position(1.0, base::Seconds(100),
                                             base::Seconds(50), true);
  media_session_->position_ = mock_position;

  EXPECT_TRUE(media_session_->IsEndOfMedia());
}

TEST_F(MediaSessionImplForIncludeTest, IsEndOfMedia3) {
  media_session::MediaPosition mock_position(1.0, base::Seconds(100),
                                             base::Seconds(50), false);
  media_session_->session_ohos_->SetEndOfMedia(true);

  media_session_->position_ = mock_position;

  EXPECT_TRUE(media_session_->IsEndOfMedia());
}

TEST_F(MediaSessionImplForIncludeTest, IsEndOfMedia4) {
  media_session::MediaPosition mock_position(1.0, base::Seconds(100),
                                             base::Seconds(50), false);
  media_session_->session_ohos_->SetEndOfMedia(false);
  media_session_->position_ = mock_position;
  EXPECT_FALSE(media_session_->IsEndOfMedia());
}

TEST_F(MediaSessionImplForIncludeTest, IsEndOfMedia5) {
  media_session_->session_ohos_.reset();
  media_session_->position_ = std::nullopt;
  EXPECT_TRUE(media_session_->IsEndOfMedia());
}

TEST_F(MediaSessionImplForIncludeTest, SetEndOfMedia) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  media_session_->SetEndOfMedia(true);
  EXPECT_TRUE(media_session_->session_ohos_->IsEndOfMedia());
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->SetEndOfMedia(true));
}

TEST_F(MediaSessionImplForIncludeTest, IsPlayingAudio) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  EXPECT_FALSE(media_session_->IsPlayingAudio());
  media_session_->session_ohos_.reset();
  EXPECT_FALSE(media_session_->IsPlayingAudio());
}

TEST_F(MediaSessionImplForIncludeTest, IsPauseByAvsession) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  media_session_->session_ohos_->SetPauseByAvsession(true);
  EXPECT_TRUE(media_session_->IsPauseByAvsession());
  media_session_->session_ohos_->SetPauseByAvsession(false);
  media_session_->session_ohos_.reset();
  EXPECT_FALSE(media_session_->IsPauseByAvsession());
}

TEST_F(MediaSessionImplForIncludeTest, SetPauseByAvsession) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  media_session_->SetPauseByAvsession(true);
  EXPECT_TRUE(media_session_->session_ohos_->IsPauseByAvsession());
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->SetPauseByAvsession(true));
}

TEST_F(MediaSessionImplForIncludeTest, SetWebviewShow) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  ASSERT_NO_FATAL_FAILURE(media_session_->SetWebviewShow(true, false));
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->SetWebviewShow(true, false));
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
TEST_F(MediaSessionImplForIncludeTest, PutWebMediaAVSessionEnabled1) {
  media_session_->session_ohos_.reset();
  media_session_->PutWebMediaAVSessionEnabled(true);
  EXPECT_NE(media_session_->session_ohos_, nullptr);
}

TEST_F(MediaSessionImplForIncludeTest, PutWebMediaAVSessionEnabled2) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  auto* old_ptr = media_session_->session_ohos_.get();
  media_session_->PutWebMediaAVSessionEnabled(true);
  EXPECT_NE(media_session_->session_ohos_, nullptr);
  EXPECT_EQ(old_ptr, media_session_->session_ohos_.get());
}

TEST_F(MediaSessionImplForIncludeTest, PutWebMediaAVSessionEnabled3) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableMediaAvsession);
  media_session_->session_ohos_.reset();
  media_session_->PutWebMediaAVSessionEnabled(true);
  EXPECT_EQ(media_session_->session_ohos_, nullptr);
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableMediaAvsession);
}

TEST_F(MediaSessionImplForIncludeTest, PutWebMediaAVSessionEnabled4) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableMediaAvsession);
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  auto* old_ptr = media_session_->session_ohos_.get();
  media_session_->PutWebMediaAVSessionEnabled(true);
  EXPECT_NE(media_session_->session_ohos_, nullptr);
  EXPECT_EQ(old_ptr, media_session_->session_ohos_.get());
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableMediaAvsession);
}

TEST_F(MediaSessionImplForIncludeTest, PutWebMediaAVSessionEnabled5) {
  if (!media_session_->session_ohos_) {
    media_session_->PutWebMediaAVSessionEnabled(true);
    ASSERT_NE(media_session_->session_ohos_, nullptr);
  }
  media_session_->PutWebMediaAVSessionEnabled(false);
  EXPECT_EQ(media_session_->session_ohos_, nullptr);
}

TEST_F(MediaSessionImplForIncludeTest, PutWebMediaAVSessionEnabled6) {
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->PutWebMediaAVSessionEnabled(false));
  EXPECT_EQ(media_session_->session_ohos_, nullptr);
}

TEST_F(MediaSessionImplForIncludeTest, PutWebMediaAVSessionEnabled7) {
  base::CommandLine::Reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->PutWebMediaAVSessionEnabled(false));
  EXPECT_EQ(media_session_->session_ohos_, nullptr);
  base::CommandLine::Init(0, nullptr);
}

TEST_F(MediaSessionImplForIncludeTest, EndSessionWhenHide1) {
  media_session_->focused_ = true;
  ASSERT_NO_FATAL_FAILURE(media_session_->EndSessionWhenHide());
}

TEST_F(MediaSessionImplForIncludeTest, EndSessionWhenHide2) {
  media_session_->focused_ = false;
  ASSERT_NO_FATAL_FAILURE(media_session_->EndSessionWhenHide());
}

TEST_F(MediaSessionImplForIncludeTest, EndSessionWhenHide3) {
  media_session_->focused_ = true;
  int id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), id);
  player_observer_->SetPlaying(id, true);
  player_observer_->SetHasSufficientlyVisibleVideo(id, false);
  ASSERT_NO_FATAL_FAILURE(media_session_->EndSessionWhenHide());
}

TEST_F(MediaSessionImplForIncludeTest, EndSessionWhenHide4) {
  media_session_->focused_ = false;
  int id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), id);
  player_observer_->SetPlaying(id, true);
  player_observer_->SetHasSufficientlyVisibleVideo(id, false);
  ASSERT_NO_FATAL_FAILURE(media_session_->EndSessionWhenHide());
}

TEST_F(MediaSessionImplForIncludeTest, EndSessionWhenHide5) {
  media_session_->focused_ = true;
  VideoMockObserver vobs(main_rfh(), media::MediaContentType::kPersistent);
  int id = vobs.StartNewPlayer();
  media_session_->AddPlayer(&vobs, id);
  vobs.SetPlaying(id, true);
  ASSERT_NO_FATAL_FAILURE(media_session_->EndSessionWhenHide());
}

TEST_F(MediaSessionImplForIncludeTest, EndSessionWhenHide6) {
  ASSERT_TRUE(media_session_->session_ohos_);
  media_session_->focused_ = false;  // C=true
  VideoMockObserver vobs(main_rfh(), media::MediaContentType::kPersistent);
  int id = vobs.StartNewPlayer();
  media_session_->AddPlayer(&vobs, id);
  vobs.SetPlaying(id, true);
  ASSERT_NO_FATAL_FAILURE(media_session_->EndSessionWhenHide());
}

TEST_F(MediaSessionImplForIncludeTest, EndSessionWhenHide7) {
  media_session_->focused_ = false;
  int id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), id);
  player_observer_->SetPlaying(id, true);
  player_observer_->SetHasSufficientlyVisibleVideo(id, false);
  ASSERT_NO_FATAL_FAILURE(media_session_->EndSessionWhenHide());
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_AVSESSION)

TEST_F(MediaSessionImplForIncludeTest, CreateSessionOhos1) {
  media_session_->session_ohos_.reset();
  base::CommandLine::Reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->CreateSessionOhos());
  EXPECT_EQ(media_session_->session_ohos_, nullptr);
  base::CommandLine::Init(0, nullptr);
}

TEST_F(MediaSessionImplForIncludeTest, CreateSessionOhos2) {
  media_session_->session_ohos_.reset();
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableMediaAvsession);
  ASSERT_NO_FATAL_FAILURE(media_session_->CreateSessionOhos());
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableMediaAvsession);
  EXPECT_EQ(media_session_->session_ohos_, nullptr);
}

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(MediaSessionImplForIncludeTest, OnPictureInPictureStateChanged1) {
  auto* controller = content::VideoPictureInPictureWindowControllerImpl::
      GetOrCreateForWebContents(web_contents());
  ASSERT_NE(controller, nullptr);

  content::MediaPlayerId id(main_rfh()->GetGlobalId(), 1);

  ASSERT_NO_FATAL_FAILURE(
      media_session_->OnPictureInPictureStateChanged(id, 2, 1, 1));
}

TEST_F(MediaSessionImplForIncludeTest, OnPictureInPictureStateChanged2) {
  content::RenderFrameHost* rfh = main_rfh();
  ASSERT_NE(rfh, nullptr);
  content::MediaPlayerId id(rfh->GetGlobalId(), 2);
  ASSERT_NO_FATAL_FAILURE(
      media_session_->OnPictureInPictureStateChanged(id, 1, 2, 2));
}
#endif  // BUILDFLAG(ARKWEB_PIP)

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
TEST_F(MediaSessionImplForIncludeTest, UpdateMediaPlayersMuteState_NoPlayers) {
  std::vector<content::MediaSessionImpl::PlayerIdentifier> ids;
  media_session_->ForAllPlayers(base::BindRepeating(
      [](std::vector<content::MediaSessionImpl::PlayerIdentifier>* out,
         const content::MediaSessionImpl::PlayerIdentifier& p) {
        out->push_back(p);
      },
      &ids));
  for (const auto& p : ids) {
    media_session_->RemovePlayer(p.observer, p.player_id);
  }
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateMediaPlayersMuteState(1, true));
}

TEST_F(MediaSessionImplForIncludeTest, UpdateMediaPlayersMuteState_IdNotMatch) {
  int id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), id);
  ASSERT_NO_FATAL_FAILURE(
      media_session_->UpdateMediaPlayersMuteState(id + 1, true));
  ASSERT_NO_FATAL_FAILURE(media_session_->GetMediaPlayerMuteState());
  media_session_->RemovePlayer(player_observer_.get(), id);
}

TEST_F(MediaSessionImplForIncludeTest, UpdateMediaPlayersMuteState_IdMatch) {
  int id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), id);
  media_session_->UpdateMediaPlayersMuteState(id, true);
  ASSERT_NO_FATAL_FAILURE(media_session_->GetMediaPlayerMuteState());
  media_session_->RemovePlayer(player_observer_.get(), id);
}

TEST_F(MediaSessionImplForIncludeTest,
       GetMediaPlayerMuteState_NoPlayers_ReturnsIsMutedTrue) {
  media_session_->is_muted_ = true;
  std::vector<content::MediaSessionImpl::PlayerIdentifier> ids;
  media_session_->ForAllPlayers(base::BindRepeating(
      [](std::vector<content::MediaSessionImpl::PlayerIdentifier>* out,
         const content::MediaSessionImpl::PlayerIdentifier& p) {
        out->push_back(p);
      },
      &ids));
  for (const auto& p : ids) {
    media_session_->RemovePlayer(p.observer, p.player_id);
  }
  EXPECT_TRUE(media_session_->GetMediaPlayerMuteState());
}

TEST_F(MediaSessionImplForIncludeTest,
       GetMediaPlayerMuteState_HasPlayer_EntryFalse_ReturnsFalse) {
  int id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), id);
  media_session_->UpdateMediaPlayersMuteState(id, false);
  EXPECT_FALSE(media_session_->GetMediaPlayerMuteState());
  media_session_->RemovePlayer(player_observer_.get(), id);
}

TEST_F(MediaSessionImplForIncludeTest,
       GetMediaPlayerMuteState_HasPlayer_EntryTrue_ReturnsIsMuted) {
  int id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), id);
  media_session_->UpdateMediaPlayersMuteState(id, true);

  media_session_->is_muted_ = true;
  EXPECT_TRUE(media_session_->GetMediaPlayerMuteState());
  media_session_->is_muted_ = false;
  EXPECT_FALSE(media_session_->GetMediaPlayerMuteState());
  media_session_->RemovePlayer(player_observer_.get(), id);
}

TEST_F(MediaSessionImplForIncludeTest,
       GetMediaPlayerMuteState_HasPlayer_NoEntry_ReturnsIsMuted) {
  int id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), id);
  media_session_->is_muted_ = false;
  EXPECT_FALSE(media_session_->GetMediaPlayerMuteState());
  media_session_->is_muted_ = true;
  EXPECT_TRUE(media_session_->GetMediaPlayerMuteState());
  media_session_->RemovePlayer(player_observer_.get(), id);
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
TEST_F(MediaSessionImplForIncludeTest, OnNotifyMeidaCastUri) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  std::string media_uri = "http://www.example.com/"; 
  ASSERT_NO_FATAL_FAILURE(media_session_->OnNotifyMeidaCastUri(media_uri));
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->OnNotifyMeidaCastUri(media_uri));
}

TEST_F(MediaSessionImplForIncludeTest, CreateAVCastAdapter) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  std::string media_uri = "http://www.example.com/"; 
  ASSERT_NO_FATAL_FAILURE(media_session_->CreateAVCastAdapter());
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->CreateAVCastAdapter());
}

TEST_F(MediaSessionImplForIncludeTest, HandleStopMediaCast) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  std::string media_uri = "http://www.example.com/"; 
  ASSERT_NO_FATAL_FAILURE(media_session_->HandleStopMediaCast());
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->HandleStopMediaCast());
}

TEST_F(MediaSessionImplForIncludeTest, UpdateRemotePlayState) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  bool is_playing = true; 
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateRemotePlayState(is_playing));
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateRemotePlayState(is_playing));
}

TEST_F(MediaSessionImplForIncludeTest, UpdateRemotePlayPosition) {
  ASSERT_NE(media_session_->session_ohos_, nullptr);
  int64_t position = 12345; 
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateRemotePlayPosition(position));
  media_session_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateRemotePlayPosition(position));
}

TEST_F(MediaSessionImplForIncludeTest, GetMediaCastCurrentTime) {
  std::vector<content::MediaSessionImpl::PlayerIdentifier> ids;
  media_session_->ForAllPlayers(base::BindRepeating(
      [](std::vector<content::MediaSessionImpl::PlayerIdentifier>* out,
         const content::MediaSessionImpl::PlayerIdentifier& p) {
        out->push_back(p);
      },
      &ids));
  for (const auto& p : ids) {
    media_session_->RemovePlayer(p.observer, p.player_id);
  }
  EXPECT_EQ(media_session_->GetMediaCastCurrentTime(), 0);
}

TEST_F(MediaSessionImplForIncludeTest, PullUpCastBackGround_NoPlayers) {
  std::vector<content::MediaSessionImpl::PlayerIdentifier> ids;
  media_session_->ForAllPlayers(base::BindRepeating(
      [](std::vector<content::MediaSessionImpl::PlayerIdentifier>* out,
         const content::MediaSessionImpl::PlayerIdentifier& p) {
        out->push_back(p);
      },
      &ids));
  for (const auto& p : ids) {
    media_session_->RemovePlayer(p.observer, p.player_id);
  }
  ASSERT_NO_FATAL_FAILURE(media_session_->PullUpCastBackGround("device1"));
}

TEST_F(MediaSessionImplForIncludeTest, PullUpCastBackGround_WithPlayer) {
  int player_id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), player_id);
  ASSERT_NO_FATAL_FAILURE(media_session_->PullUpCastBackGround("test_device"));
  media_session_->RemovePlayer(player_observer_.get(), player_id);
}

TEST_F(MediaSessionImplForIncludeTest, MediaCastStopped_NoPlayers) {
  std::vector<content::MediaSessionImpl::PlayerIdentifier> ids;
  media_session_->ForAllPlayers(base::BindRepeating(
      [](std::vector<content::MediaSessionImpl::PlayerIdentifier>* out,
         const content::MediaSessionImpl::PlayerIdentifier& p) {
        out->push_back(p);
      },
      &ids));
  for (const auto& p : ids) {
    media_session_->RemovePlayer(p.observer, p.player_id);
  }
  ASSERT_NO_FATAL_FAILURE(media_session_->MediaCastStopped());
}

TEST_F(MediaSessionImplForIncludeTest, MediaCastStopped_WithPlayer) {
  int player_id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), player_id);
  ASSERT_NO_FATAL_FAILURE(media_session_->MediaCastStopped());
  media_session_->RemovePlayer(player_observer_.get(), player_id);
}

TEST_F(MediaSessionImplForIncludeTest, UpdateUiPlayState_NoPlayers) {
  std::vector<content::MediaSessionImpl::PlayerIdentifier> ids;
  media_session_->ForAllPlayers(base::BindRepeating(
      [](std::vector<content::MediaSessionImpl::PlayerIdentifier>* out,
         const content::MediaSessionImpl::PlayerIdentifier& p) {
        out->push_back(p);
      },
      &ids));
  for (const auto& p : ids) {
    media_session_->RemovePlayer(p.observer, p.player_id);
  }
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayState(true));
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayState(false));
}

TEST_F(MediaSessionImplForIncludeTest, UpdateUiPlayState_WithPlayer) {
  int player_id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), player_id);
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayState(true));
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayState(false));
  media_session_->RemovePlayer(player_observer_.get(), player_id);
}

TEST_F(MediaSessionImplForIncludeTest, UpdateUiPlayPosition_NoPlayers) {
  std::vector<content::MediaSessionImpl::PlayerIdentifier> ids;
  media_session_->ForAllPlayers(base::BindRepeating(
      [](std::vector<content::MediaSessionImpl::PlayerIdentifier>* out,
         const content::MediaSessionImpl::PlayerIdentifier& p) {
        out->push_back(p);
      },
      &ids));
  for (const auto& p : ids) {
    media_session_->RemovePlayer(p.observer, p.player_id);
  }
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayPosition(1000));
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayPosition(0));
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayPosition(-1));
}

TEST_F(MediaSessionImplForIncludeTest, UpdateUiPlayPosition_WithPlayer) {
  int player_id = player_observer_->StartNewPlayer();
  media_session_->AddPlayer(player_observer_.get(), player_id);
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayPosition(5000));
  ASSERT_NO_FATAL_FAILURE(media_session_->UpdateUiPlayPosition(0));
  media_session_->RemovePlayer(player_observer_.get(), player_id);
}

TEST_F(MediaSessionImplForIncludeTest, IsPageBackground) {
  std::vector<content::MediaSessionImpl::PlayerIdentifier> ids;
  media_session_->ForAllPlayers(base::BindRepeating(
      [](std::vector<content::MediaSessionImpl::PlayerIdentifier>* out,
         const content::MediaSessionImpl::PlayerIdentifier& p) {
        out->push_back(p);
      },
      &ids));
  for (const auto& p : ids) {
    media_session_->RemovePlayer(p.observer, p.player_id);
  }
  ASSERT_NO_FATAL_FAILURE(media_session_->IsPageBackground()); 
}

TEST_F(MediaSessionImplForIncludeTest, SetPauseByAvcast) {
  ASSERT_NO_FATAL_FAILURE(media_session_->SetPauseByAvcast(true));
  ASSERT_NO_FATAL_FAILURE(media_session_->SetPauseByAvcast(false));
  ASSERT_NO_FATAL_FAILURE(media_session_->SetPauseByAvcast(true));
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_CAST)
}  // namespace content