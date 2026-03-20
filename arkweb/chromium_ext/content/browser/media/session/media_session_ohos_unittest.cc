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
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "services/media_session/public/cpp/media_position.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "arkweb/chromium_ext/content/browser/media/session/media_session_ohos.h"
#undef private

namespace content {
class FakeAVSessionAdapter : public OHOS::NWeb::MediaAVSessionAdapter {
 public:
  explicit FakeAVSessionAdapter(bool create_ret, bool regist_ret)
      : create_ret_(create_ret), regist_ret_(regist_ret) {}

  bool CreateAVSession(OHOS::NWeb::MediaAVSessionType) override {
    create_called_ = true;
    return create_ret_;
  }
  void DestroyAVSession() override {}

  bool RegistCallback(
      std::shared_ptr<OHOS::NWeb::MediaAVSessionCallbackAdapter> cb) override {
    regist_called_ = true;
    callback_ok_ = static_cast<bool>(cb);
    return regist_ret_;
  }

  bool IsActivated() override { return false; }
  bool Activate() override { return false; }
  void DeActivate() override {}

  void SetMetadata(
      const std::shared_ptr<OHOS::NWeb::MediaAVSessionMetadataAdapter> metadata)
      override {
    set_metadata_called_++;
    get_metadata_ = static_cast<bool>(metadata);
  }

  void SetPlaybackPosition(
      const std::shared_ptr<OHOS::NWeb::MediaAVSessionPositionAdapter> position)
      override {
    set_playback_pos_called_ = true;
    last_pos_ = position;
  }
  void SetPlaybackState(OHOS::NWeb::MediaAVSessionPlayState state) override {
    set_playback_called_ = true;
    last_playback_state_ = state;
  }
  void SetMediaCastUri(const std::string& uri) override {
    set_media_cast_uri_called_ = true;
    media_cast_uri_ = uri;
  }

  void PrepareMediaCastDescription() override {
    prepare_media_cast_desc_called_ = true;
  }

  void HandleStopMediaCast() override {
    handle_stop_media_cast_called_ = true;
  }

  void UpdateRemotePlayState(bool is_playing) override {
    update_remote_play_state_called_ = true;
    remote_play_state_ = is_playing;
  }
  bool set_media_cast_uri_called_ = false;
  std::string media_cast_uri_;
  bool prepare_media_cast_desc_called_ = false;
  bool handle_stop_media_cast_called_ = false;
  bool update_remote_play_state_called_ = false;
  bool remote_play_state_ = false;
  bool create_called_ = false;
  bool regist_called_ = false;
  bool callback_ok_ = false;
  int set_metadata_called_ = 0;
  bool get_metadata_ = false;
  bool create_ret_ = false;
  bool regist_ret_ = false;
  bool set_playback_called_ = false;
  bool set_playback_pos_called_ = false;
  std::shared_ptr<OHOS::NWeb::MediaAVSessionPositionAdapter> last_pos_;
  OHOS::NWeb::MediaAVSessionPlayState last_playback_state_{
      OHOS::NWeb::MediaAVSessionPlayState::STATE_INITIAL};
};

class MediaSessionOHOSTest : public RenderViewHostTestHarness {
 public:
  MediaSessionOHOSTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    media_session_impl_ = MediaSessionImpl::Get(web_contents());
    ASSERT_NE(media_session_impl_, nullptr);
    media_session_ohos_ =
        std::make_unique<MediaSessionOHOS>(media_session_impl_);
  }

  void TearDown() override {
    media_session_ohos_.reset();
    RenderViewHostTestHarness::TearDown();
  }

  MediaSessionImpl* media_session_impl_ = nullptr;
  std::unique_ptr<MediaSessionOHOS> media_session_ohos_;
};

TEST_F(MediaSessionOHOSTest, Prepare1) {
  EXPECT_FALSE(media_session_ohos_->is_initialized_);
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID);

  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Prepare(
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID));
  EXPECT_FALSE(media_session_ohos_->is_initialized_);
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID);
  EXPECT_FALSE(media_session_ohos_->task_runner_);
}

TEST_F(MediaSessionOHOSTest, Prepare2) {
  media_session_ohos_->avsession_adapter_.reset();
  EXPECT_FALSE(media_session_ohos_->is_initialized_);
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID);
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Prepare(
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO));
  EXPECT_TRUE(media_session_ohos_->is_initialized_);
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID);
  EXPECT_FALSE(media_session_ohos_->task_runner_);
}

TEST_F(MediaSessionOHOSTest, Prepare3) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  ASSERT_TRUE(media_session_ohos_->av_metadata_);
  EXPECT_TRUE(media_session_ohos_->av_metadata_->GetTitle().empty());

  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Prepare(
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO));
  EXPECT_TRUE(media_session_ohos_->is_initialized_);
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO);
  EXPECT_TRUE(media_session_ohos_->task_runner_);
  EXPECT_TRUE(fake->create_called_);
  EXPECT_TRUE(fake->regist_called_);
  EXPECT_TRUE(fake->callback_ok_);
  EXPECT_EQ(fake->set_metadata_called_, 0);
}

TEST_F(MediaSessionOHOSTest, Prepare4) {
  auto* fake = new FakeAVSessionAdapter(true, false);
  media_session_ohos_->avsession_adapter_.reset(fake);

  ASSERT_TRUE(media_session_ohos_->av_metadata_);
  media_session_ohos_->av_metadata_->SetTitle("title-not-empty");
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Prepare(
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_VIDEO));
  EXPECT_TRUE(media_session_ohos_->is_initialized_);
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_VIDEO);
  EXPECT_TRUE(media_session_ohos_->task_runner_);
  EXPECT_TRUE(fake->create_called_);
  EXPECT_TRUE(fake->regist_called_);
  EXPECT_TRUE(fake->callback_ok_);
  EXPECT_EQ(fake->set_metadata_called_, 1);
  EXPECT_TRUE(fake->get_metadata_);
}

TEST_F(MediaSessionOHOSTest, Prepare5) {
  auto* fake = new FakeAVSessionAdapter(false, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  ASSERT_TRUE(media_session_ohos_->av_metadata_);
  media_session_ohos_->av_metadata_->SetTitle("title-exists");
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Prepare(
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO));
  EXPECT_TRUE(media_session_ohos_->is_initialized_);
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID);
  EXPECT_FALSE(media_session_ohos_->task_runner_);
  EXPECT_TRUE(fake->create_called_);
  EXPECT_EQ(fake->set_metadata_called_, 0);
  EXPECT_FALSE(fake->regist_called_);
}

TEST_F(MediaSessionOHOSTest, Prepare6) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->av_metadata_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Prepare(
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO));

  EXPECT_TRUE(media_session_ohos_->is_initialized_);
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO);
  EXPECT_TRUE(media_session_ohos_->task_runner_);
  EXPECT_TRUE(fake->create_called_);
  EXPECT_TRUE(fake->regist_called_);
  EXPECT_EQ(fake->set_metadata_called_, 0);
}

TEST_F(MediaSessionOHOSTest, GetMediaType1) {
  struct Case {
    std::vector<media_session::mojom::MediaAudioVideoState> states;
    OHOS::NWeb::MediaAVSessionType expected;
  };

  std::vector<Case> cases = {
      {{}, OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID},
      {{media_session::mojom::MediaAudioVideoState::kAudioOnly},
       OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO},
      {{media_session::mojom::MediaAudioVideoState::kDeprecatedUnknown},
       OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID},
      {{media_session::mojom::MediaAudioVideoState::kAudioOnly,
        media_session::mojom::MediaAudioVideoState::kVideoOnly},
       OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_VIDEO},
      {{media_session::mojom::MediaAudioVideoState::kAudioOnly,
        media_session::mojom::MediaAudioVideoState::kAudioVideo},
       OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_VIDEO},
      {{media_session::mojom::MediaAudioVideoState::kDeprecatedUnknown,
        media_session::mojom::MediaAudioVideoState::kAudioOnly},
       OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO},
  };

  for (const auto& c : cases) {
    EXPECT_EQ(media_session_ohos_->GetMediaType(c.states), c.expected);
  }
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged1) {
  media_session_ohos_->avsession_adapter_.reset();
  auto info = media_session::mojom::MediaSessionInfo::New();
  info->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info->playback_state = media_session::mojom::MediaPlaybackState::kPlaying;
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info)));
  EXPECT_FALSE(media_session_ohos_->is_initialized_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged2) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(nullptr));
  EXPECT_FALSE(fake->set_playback_called_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged3) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  auto info = media_session::mojom::MediaSessionInfo::New();
  info->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  info->audio_video_states = std::vector{
      media_session::mojom::MediaAudioVideoState::kDeprecatedUnknown};

  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info)));
  EXPECT_FALSE(media_session_ohos_->is_initialized_);
  EXPECT_FALSE(fake->set_playback_called_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged4) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  auto info = media_session::mojom::MediaSessionInfo::New();
  info->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  info->audio_video_states =
      std::vector{media_session::mojom::MediaAudioVideoState::kAudioOnly};

  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info)));
  EXPECT_TRUE(media_session_ohos_->is_initialized_);
  EXPECT_TRUE(fake->set_playback_called_);
  EXPECT_EQ(fake->last_playback_state_,
            OHOS::NWeb::MediaAVSessionPlayState::STATE_PAUSE);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged5) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  auto info1 = media_session::mojom::MediaSessionInfo::New();
  info1->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info1->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  info1->audio_video_states =
      std::vector{media_session::mojom::MediaAudioVideoState::kAudioOnly};
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info1)));
  EXPECT_TRUE(media_session_ohos_->is_initialized_);

  auto info2 = media_session::mojom::MediaSessionInfo::New();
  info2->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info2->playback_state = media_session::mojom::MediaPlaybackState::kPlaying;
  info2->audio_video_states =
      std::vector{media_session::mojom::MediaAudioVideoState::kAudioVideo};
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info2)));
  EXPECT_TRUE(fake->set_playback_called_);
  EXPECT_EQ(fake->last_playback_state_,
            OHOS::NWeb::MediaAVSessionPlayState::STATE_PLAY);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged6) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  auto info1 = media_session::mojom::MediaSessionInfo::New();
  info1->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info1->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  info1->audio_video_states =
      std::vector{media_session::mojom::MediaAudioVideoState::kAudioOnly};
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info1)));

  auto info2 = media_session::mojom::MediaSessionInfo::New();
  info2->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info2->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info2)));
  EXPECT_TRUE(fake->set_playback_called_);
  EXPECT_EQ(fake->last_playback_state_,
            OHOS::NWeb::MediaAVSessionPlayState::STATE_PAUSE);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged7) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  auto info1 = media_session::mojom::MediaSessionInfo::New();
  info1->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info1->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  info1->audio_video_states =
      std::vector{media_session::mojom::MediaAudioVideoState::kAudioOnly};
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info1)));

  fake->set_playback_called_ = false;
  auto info2 = media_session::mojom::MediaSessionInfo::New();
  info2->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info2->playback_state = media_session::mojom::MediaPlaybackState::kPlaying;
  info2->audio_video_states = std::vector{
      media_session::mojom::MediaAudioVideoState::kDeprecatedUnknown};
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info2)));
  EXPECT_FALSE(fake->set_playback_called_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged8) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  auto info = media_session::mojom::MediaSessionInfo::New();
  info->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info)));
  EXPECT_FALSE(media_session_ohos_->is_initialized_);
  EXPECT_TRUE(fake->set_playback_called_);
  EXPECT_EQ(fake->last_playback_state_,
            OHOS::NWeb::MediaAVSessionPlayState::STATE_PAUSE);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged9) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  auto info1 = media_session::mojom::MediaSessionInfo::New();
  info1->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info1->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  info1->audio_video_states =
      std::vector{media_session::mojom::MediaAudioVideoState::kAudioOnly};
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info1)));
  EXPECT_TRUE(media_session_ohos_->is_initialized_);

  fake->set_playback_called_ = false;
  auto info2 = media_session::mojom::MediaSessionInfo::New();
  info2->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info2->playback_state = media_session::mojom::MediaPlaybackState::kPlaying;
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info2)));
  EXPECT_TRUE(fake->set_playback_called_);
  EXPECT_EQ(fake->last_playback_state_,
            OHOS::NWeb::MediaAVSessionPlayState::STATE_PLAY);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged10) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->is_initialized_ = true;
  auto info = media_session::mojom::MediaSessionInfo::New();
  info->state = media_session::mojom::MediaSessionInfo::SessionState::kActive;
  info->playback_state = media_session::mojom::MediaPlaybackState::kPaused;
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(std::move(info)));
  EXPECT_FALSE(fake->create_called_);
  EXPECT_TRUE(fake->set_playback_called_);
  EXPECT_EQ(fake->last_playback_state_,
            OHOS::NWeb::MediaAVSessionPlayState::STATE_PAUSE);
}

TEST_F(MediaSessionOHOSTest, MediaSessionInfoChanged11) {
  media_session_ohos_->avsession_adapter_.reset();
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionInfoChanged(nullptr));
}

TEST_F(MediaSessionOHOSTest, MediaSessionMetadataChanged1) {
  media_session_ohos_->avsession_adapter_.reset();
  auto md = media_session::MediaMetadata();
  md.title = u"";
  md.artist = u"";
  md.album = u"";
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->MediaSessionMetadataChanged(md));
}

TEST_F(MediaSessionOHOSTest, MediaSessionMetadataChanged2) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionMetadataChanged(std::nullopt));
  EXPECT_EQ(fake->set_metadata_called_, 0);
}

TEST_F(MediaSessionOHOSTest, MediaSessionMetadataChanged3) {
  media_session_ohos_->avsession_adapter_.reset();
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionMetadataChanged(std::nullopt));
}

TEST_F(MediaSessionOHOSTest, MediaSessionMetadataChanged4) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  auto md = media_session::MediaMetadata();
  md.title = u"";
  md.artist = u"artist-x";
  md.album = u"album-x";
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->MediaSessionMetadataChanged(md));
  EXPECT_EQ(fake->set_metadata_called_, 0);
}

TEST_F(MediaSessionOHOSTest, MediaSessionMetadataChanged5) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  auto md = media_session::MediaMetadata();
  md.title = u"title-x";
  md.artist = u"artist-y";
  md.album = u"album-z";
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->MediaSessionMetadataChanged(md));
  EXPECT_EQ(fake->set_metadata_called_, 1);
  EXPECT_TRUE(fake->get_metadata_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged1) {
  media_session_ohos_->avsession_adapter_.reset();
  media_session::MediaPosition pos(1.0, base::Seconds(60), base::Seconds(10),
                                   false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));
}

TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged2) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(std::nullopt));
  EXPECT_FALSE(fake->set_playback_pos_called_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged3) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->av_position_.reset();
  media_session::MediaPosition pos(1.0, base::Seconds(30), base::Seconds(5),
                                   false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));
  EXPECT_FALSE(fake->set_playback_pos_called_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged4) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  ASSERT_TRUE(media_session_ohos_->av_position_);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
  media_session::MediaPosition pos(1.0, base::Seconds(100), base::Seconds(7),
                                   false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));
  EXPECT_FALSE(fake->set_playback_pos_called_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged5) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  ASSERT_TRUE(media_session_ohos_->av_position_);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  media_session::MediaPosition pos(1.0, base::Seconds(120), base::Seconds(30),
                                   false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));
  EXPECT_TRUE(fake->set_playback_pos_called_);
  ASSERT_TRUE(fake->last_pos_);
  EXPECT_EQ(media_session_ohos_->av_position_->GetDuration(), 120000);
  EXPECT_EQ(media_session_ohos_->av_position_->GetElapsedTime(), 30000);
}

TEST_F(MediaSessionOHOSTest, MediaSessionImagesChanged1) {
  media_session_ohos_->avsession_adapter_.reset();
  base::flat_map<media_session::mojom::MediaSessionImageType,
                 std::vector<media_session::MediaImage>>
      images;
  media_session::MediaImage img;
  img.src = GURL("https://example");
  images[media_session::mojom::MediaSessionImageType::kArtwork] = {img};
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionImagesChanged(images));
}

TEST_F(MediaSessionOHOSTest, MediaSessionImagesChanged2) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  base::flat_map<media_session::mojom::MediaSessionImageType,
                 std::vector<media_session::MediaImage>>
      images;
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionImagesChanged(images));
  EXPECT_EQ(fake->set_metadata_called_, 0);
}

TEST_F(MediaSessionOHOSTest, MediaSessionImagesChanged3) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  base::flat_map<media_session::mojom::MediaSessionImageType,
                 std::vector<media_session::MediaImage>>
      images;
  media_session::MediaImage img;
  img.src = GURL("https://example");
  images[media_session::mojom::MediaSessionImageType::kArtwork] = {img};

  ASSERT_TRUE(media_session_ohos_->av_metadata_);
  media_session_ohos_->av_metadata_->SetTitle("");
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionImagesChanged(images));
  EXPECT_EQ(fake->set_metadata_called_, 0);
}

TEST_F(MediaSessionOHOSTest, MediaSessionImagesChanged4) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  base::flat_map<media_session::mojom::MediaSessionImageType,
                 std::vector<media_session::MediaImage>>
      images;
  media_session::MediaImage img;
  img.src = GURL("https://example");
  images[media_session::mojom::MediaSessionImageType::kArtwork] = {img};

  ASSERT_TRUE(media_session_ohos_->av_metadata_);
  media_session_ohos_->av_metadata_->SetTitle("t");

  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionImagesChanged(images));

  EXPECT_EQ(fake->set_metadata_called_, 1);
  EXPECT_TRUE(fake->get_metadata_);
}

TEST_F(MediaSessionOHOSTest, MediaSessionImagesChanged5) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  base::flat_map<media_session::mojom::MediaSessionImageType,
                 std::vector<media_session::MediaImage>>
      images;
  media_session::MediaImage img;
  img.src = GURL("https://example");
  images[media_session::mojom::MediaSessionImageType::kSourceIcon] = {img};
  media_session_ohos_->av_metadata_.reset();
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionImagesChanged(images));
  EXPECT_EQ(fake->set_metadata_called_, 0);
}

TEST_F(MediaSessionOHOSTest, MediaSessionImagesChanged6) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  base::flat_map<media_session::mojom::MediaSessionImageType,
                 std::vector<media_session::MediaImage>>
      images;
  images[media_session::mojom::MediaSessionImageType::kArtwork] = {};
  ASSERT_TRUE(media_session_ohos_->av_metadata_);
  media_session_ohos_->av_metadata_->SetTitle("test");
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionImagesChanged(images));
  EXPECT_EQ(fake->set_metadata_called_, 1);
  EXPECT_TRUE(fake->get_metadata_);
}

TEST_F(MediaSessionOHOSTest, Resume) {
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Resume());
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Resume());
}

TEST_F(MediaSessionOHOSTest, Suspend) {
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Suspend());
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Suspend());
}

TEST_F(MediaSessionOHOSTest, Stop) {
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Stop());
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->Stop());
}

TEST_F(MediaSessionOHOSTest, SeekTo) {
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SeekTo(1000));
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
  media_session_ohos_->is_playing_ = true;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SeekTo(5000));
  media_session_ohos_->is_playing_ = false;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SeekTo(3000));
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SeekTo(-100));
  media_session_ohos_->is_playing_ = true;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SeekTo(0));
}

TEST_F(MediaSessionOHOSTest, SetWebviewShow) {
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SetWebviewShow(true, false));
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_VIDEO;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SetWebviewShow(true, true));
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SetWebviewShow(true, true));
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SetWebviewShow(true, false));
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SetWebviewShow(false, false));
  media_session_ohos_->avsession_adapter_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SetWebviewShow(false, false));
}

TEST_F(MediaSessionOHOSTest, IsPlayingAudio) {
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;
  EXPECT_FALSE(media_session_ohos_->IsPlayingAudio());
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = media_session_impl_;
  media_session_ohos_->avsession_adapter_.reset();
  EXPECT_FALSE(media_session_ohos_->IsPlayingAudio());
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;
  EXPECT_FALSE(media_session_ohos_->IsPlayingAudio());
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = media_session_impl_;
  media_session_ohos_->avsession_adapter_.reset(
      new FakeAVSessionAdapter(true, true));
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
  EXPECT_TRUE(media_session_ohos_->IsPlayingAudio());
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_FALSE(media_session_ohos_->IsPlayingAudio());
}

TEST_F(MediaSessionOHOSTest, CallBackPlay) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->Play());
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->Play());
  base::RunLoop().RunUntilIdle();
}

TEST_F(MediaSessionOHOSTest, CallBackPause) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->Pause());
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->Pause());
  base::RunLoop().RunUntilIdle();
}

TEST_F(MediaSessionOHOSTest, CallBackStop) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->Stop());
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->Stop());
  base::RunLoop().RunUntilIdle();
}

TEST_F(MediaSessionOHOSTest, CallBackSeekTo) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->SeekTo(5000));
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->SeekTo(3000));
  base::RunLoop().RunUntilIdle();
}

TEST_F(MediaSessionOHOSTest, CreateAVCastAdapter) {
  media_session_ohos_->avsession_adapter_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->CreateAVCastAdapter());
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_uri_ = "https://example.com/media.mp4";
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->CreateAVCastAdapter());
  EXPECT_TRUE(fake->set_media_cast_uri_called_);
  EXPECT_EQ(fake->media_cast_uri_, "https://example.com/media.mp4");
  EXPECT_TRUE(fake->prepare_media_cast_desc_called_);
}

TEST_F(MediaSessionOHOSTest, HandleStopMediaCast) {
  media_session_ohos_->avsession_adapter_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->HandleStopMediaCast());
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->HandleStopMediaCast());
  EXPECT_TRUE(fake->handle_stop_media_cast_called_);
}

TEST_F(MediaSessionOHOSTest, GetMediaCastCurrentTime) {
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;
  EXPECT_EQ(media_session_ohos_->GetMediaCastCurrentTime(), 0);
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = media_session_impl_;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->GetMediaCastCurrentTime());
}

TEST_F(MediaSessionOHOSTest, PullUpCastBackGround) {
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->PullUpCastBackGround("device1"));
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = media_session_impl_;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->PullUpCastBackGround("device2"));
}

TEST_F(MediaSessionOHOSTest, MediaCastStopped) {
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->MediaCastStopped());
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = media_session_impl_;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->MediaCastStopped());
}

TEST_F(MediaSessionOHOSTest, SetAvCast) {
  EXPECT_FALSE(media_session_ohos_->is_avcast_);
  media_session_ohos_->SetAvCast(true);
  EXPECT_TRUE(media_session_ohos_->is_avcast_);
  media_session_ohos_->SetAvCast(false);
  EXPECT_FALSE(media_session_ohos_->is_avcast_);
}

TEST_F(MediaSessionOHOSTest, UpdateUiPlayState) {
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateUiPlayState(true));
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateUiPlayState(false));
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = media_session_impl_;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateUiPlayState(true));
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateUiPlayState(false));
}

TEST_F(MediaSessionOHOSTest, UpdateUiPlayPosition) {
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateUiPlayPosition(1000));
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateUiPlayPosition(-1000));
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = media_session_impl_;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateUiPlayPosition(5000));
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateUiPlayPosition(0));
}

TEST_F(MediaSessionOHOSTest, UpdateRemotePlayState) {
  media_session_ohos_->avsession_adapter_.reset();
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateRemotePlayState(true));
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateRemotePlayState(false));
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateRemotePlayState(true));
  EXPECT_TRUE(fake->update_remote_play_state_called_);
  EXPECT_TRUE(fake->remote_play_state_);
  fake->update_remote_play_state_called_ = false;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->UpdateRemotePlayState(false));
  EXPECT_TRUE(fake->update_remote_play_state_called_);
  EXPECT_FALSE(fake->remote_play_state_);
}

TEST_F(MediaSessionOHOSTest, SetWebviewShow_WithAvCast) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ = OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
  media_session_ohos_->is_avcast_ = false;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SetWebviewShow(false, true));
  EXPECT_EQ(media_session_ohos_->media_type_,
            OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_INVALID);
  media_session_ohos_->media_type_ = OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;
  media_session_ohos_->is_avcast_ = true;
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->SetWebviewShow(false, true));
}

TEST_F(MediaSessionOHOSTest, CallBackPullUpCastBackGround) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->PullUpCastBackGround("device1"));
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->PullUpCastBackGround("device2"));
  base::RunLoop().RunUntilIdle();
}

TEST_F(MediaSessionOHOSTest, CallBackMediaCastStopped) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->MediaCastStopped());
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->MediaCastStopped());
  base::RunLoop().RunUntilIdle();
}

TEST_F(MediaSessionOHOSTest, CallBackSetAvCast) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->SetAvCast(true));
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->SetAvCast(true));
  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(media_session_ohos_->is_avcast_);
  ASSERT_NO_FATAL_FAILURE(callback_valid->SetAvCast(false));
  base::RunLoop().RunUntilIdle();
  EXPECT_FALSE(media_session_ohos_->is_avcast_);
}

TEST_F(MediaSessionOHOSTest, CallBackUpdateUiPlayState) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->UpdateUiPlayState(true));
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->UpdateUiPlayState(true));
  base::RunLoop().RunUntilIdle();
}

TEST_F(MediaSessionOHOSTest, CallBackUpdateUiPlayPosition) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->UpdateUiPlayPosition(1000));
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->UpdateUiPlayPosition(5000));
  base::RunLoop().RunUntilIdle();
}

// Tests that position updates are filtered when seek_filter flag is set and playback rate is 0
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_seek_filter_when_rate_zero) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  // Set seek_filter flag to simulate seeking in progress
  media_session_ohos_->is_playing_before_seeking_ = true;

  // Create position with playback_rate = 0 (seeking in progress)
  media_session::MediaPosition pos(0.0, base::Seconds(120), base::Seconds(30),
                                   false);

  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));

  // Expected: set_playback_pos_called_ = false (filtered due to seek_filter + rate=0),
  //          old_position_ updated to store the seeking position
  EXPECT_FALSE(fake->set_playback_pos_called_);
  EXPECT_TRUE(media_session_ohos_->old_position_.has_value());
  EXPECT_EQ(media_session_ohos_->old_position_->playback_rate(), 0.0);

  // Clean up: reset flag for subsequent tests
  media_session_ohos_->is_playing_before_seeking_ = false;
}

// Tests that position updates are NOT filtered when seek_filter flag is set but playback rate is non-zero
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_seek_filter_does_not_apply_when_playing) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  // Set seek_filter flag to test that it doesn't filter when rate is non-zero
  media_session_ohos_->is_playing_before_seeking_ = true;

  // Create position with playback_rate = 1.0 (normal playback after seek)
  media_session::MediaPosition pos(1.0, base::Seconds(120), base::Seconds(30),
                                   false);

  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));

  // Expected: set_playback_pos_called_ = true (filter only applies when rate == 0)
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Clean up: reset flag for subsequent tests
  media_session_ohos_->is_playing_before_seeking_ = false;
}

// Tests that position updates within the same second are filtered
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_subsecond_delta_filtered) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // First call: position at 30 seconds
  media_session::MediaPosition pos1(1.0, base::Seconds(120), base::Seconds(30),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos1));
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Reset the flag for next call
  fake->set_playback_pos_called_ = false;

  // Second call: position at 30.4 seconds
  // Note: InSeconds() truncates to integer seconds, so both 30s and 30.4s map to 30
  media_session::MediaPosition pos2(1.0, base::Seconds(120), base::Seconds(30.4),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos2));

  // Expected: Second call filtered out (both positions have same InSeconds() value of 30)
  EXPECT_FALSE(fake->set_playback_pos_called_);
}

// Tests that position updates with half-second delta are filtered when in same second
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_halfsecond_delta_filtered) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // First call: position at 30 seconds
  media_session::MediaPosition pos1(1.0, base::Seconds(120), base::Seconds(30),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos1));
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Reset the flag for next call
  fake->set_playback_pos_called_ = false;

  // Second call: position at 30.5 seconds
  // Note: InSeconds() truncates to integer seconds, so 30.5s maps to 30 (truncation, not rounding)
  media_session::MediaPosition pos2(1.0, base::Seconds(120), base::Seconds(30.5),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos2));

  // Expected: Second call filtered out (both positions have same InSeconds() value of 30)
  EXPECT_FALSE(fake->set_playback_pos_called_);
}

// Tests that position updates crossing second boundary are processed
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_second_boundary_processed) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // First call: position at 30 seconds
  media_session::MediaPosition pos1(1.0, base::Seconds(120), base::Seconds(30),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos1));
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Reset the flag for next call
  fake->set_playback_pos_called_ = false;

  // Second call: position at 31 seconds
  // Note: InSeconds() returns different values (30 vs 31), triggering update
  media_session::MediaPosition pos2(1.0, base::Seconds(120), base::Seconds(31),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos2));

  // Expected: Second call processed (InSeconds() values differ: 30 != 31)
  EXPECT_TRUE(fake->set_playback_pos_called_);
}

// Tests that playback rate change triggers update regardless of position delta
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_rate_change_triggers_update) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // First call: rate 1.0, position at 30 seconds
  media_session::MediaPosition pos1(1.0, base::Seconds(120), base::Seconds(30),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos1));
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Reset the flag for next call
  fake->set_playback_pos_called_ = false;

  // Second call: rate 2.0, position at 30.3 seconds
  // Rate change should bypass the position comparison and trigger update
  media_session::MediaPosition pos2(2.0, base::Seconds(120), base::Seconds(30.3),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos2));

  // Expected: Second call processed (rate change bypasses position check)
  EXPECT_TRUE(fake->set_playback_pos_called_);
}

// Tests that backward seeks crossing second boundaries are processed
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_backward_seek) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // First call: position at 60 seconds
  media_session::MediaPosition pos1(1.0, base::Seconds(120), base::Seconds(60),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos1));
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Reset the flag for next call
  fake->set_playback_pos_called_ = false;

  // Second call: position at 58 seconds (backward seek)
  // Note: InSeconds() returns different values (60 vs 58), triggering update
  media_session::MediaPosition pos2(1.0, base::Seconds(120), base::Seconds(58),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos2));

  // Expected: Second call processed (InSeconds() values differ: 60 != 58)
  EXPECT_TRUE(fake->set_playback_pos_called_);
}

// Tests that the first position update is always processed
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_initial_position_update) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // Ensure no old_position_ set (first update scenario)
  EXPECT_FALSE(media_session_ohos_->old_position_.has_value());

  // First position update
  media_session::MediaPosition pos(1.0, base::Seconds(120), base::Seconds(30),
                                   false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));

  // Expected: Update processed, old_position_ set to current position
  EXPECT_TRUE(fake->set_playback_pos_called_);
  EXPECT_TRUE(media_session_ohos_->old_position_.has_value());
  EXPECT_EQ(media_session_ohos_->old_position_->GetPosition().InMilliseconds(), 30000);
}

// Tests that minimal delta crossing second boundary is processed
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_cross_second_boundary_minimal) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // First call: position at 30.999 seconds
  media_session::MediaPosition pos1(1.0, base::Seconds(120), base::Seconds(30.999),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos1));
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Reset the flag for next call
  fake->set_playback_pos_called_ = false;

  // Second call: position at 31.0 seconds (1ms delta, but crosses second boundary)
  // Note: InSeconds() returns 30 for 30.999s, and 31 for 31.0s
  media_session::MediaPosition pos2(1.0, base::Seconds(120), base::Seconds(31.0),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos2));

  // Expected: Second call processed (InSeconds() values differ: 30 != 31)
  EXPECT_TRUE(fake->set_playback_pos_called_);
}

// Tests that large delta within same second is filtered
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_large_delta_same_second) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // First call: position at 30.001 seconds
  media_session::MediaPosition pos1(1.0, base::Seconds(120), base::Seconds(30.001),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos1));
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Reset the flag for next call
  fake->set_playback_pos_called_ = false;

  // Second call: position at 30.999 seconds (998ms delta, but within same second)
  // Note: InSeconds() truncates both to 30
  media_session::MediaPosition pos2(1.0, base::Seconds(120), base::Seconds(30.999),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos2));

  // Expected: Second call filtered out (both positions have same InSeconds() value of 30)
  EXPECT_FALSE(fake->set_playback_pos_called_);
}

// Tests that NotifyCastControlShow handles null media_session gracefully
TEST_F(MediaSessionOHOSTest, NotifyCastControlShow_NullMediaSession) {
  // Save original media_session_ for restoration
  auto original_media_session = media_session_ohos_->media_session_;

  // Set media_session_ to nullptr
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = nullptr;

  // Expected: No crash when media_session_ is null
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->NotifyCastControlShow(true));
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->NotifyCastControlShow(false));

  // Restore media_session_
  const_cast<raw_ptr<MediaSessionImpl, DanglingUntriaged>&>(
      media_session_ohos_->media_session_) = original_media_session;
}

// Tests that NotifyCastControlShow works correctly with valid media_session
TEST_F(MediaSessionOHOSTest, NotifyCastControlShow_ValidMediaSession) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);

  // Ensure media_session_ is valid
  EXPECT_NE(media_session_ohos_->media_session_, nullptr);

  // Expected: Call forwarded correctly, no crash
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->NotifyCastControlShow(true));
  ASSERT_NO_FATAL_FAILURE(media_session_ohos_->NotifyCastControlShow(false));
}

// Tests OHOSMediaAVSessionCallback::NotifyCastControlShow with null and valid weak_ptr
TEST_F(MediaSessionOHOSTest, CallBackNotifyCastControlShow) {
  media_session_ohos_->task_runner_ =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  // Test with null weak_ptr
  auto callback_null = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_, base::WeakPtr<MediaSessionOHOS>());
  ASSERT_NO_FATAL_FAILURE(callback_null->NotifyCastControlShow(true));

  // Test with valid weak_ptr
  auto callback_valid = std::make_unique<OHOSMediaAVSessionCallback>(
      media_session_ohos_->task_runner_,
      media_session_ohos_->weak_factory_.GetWeakPtr());
  ASSERT_NO_FATAL_FAILURE(callback_valid->NotifyCastControlShow(true));

  // Expected: Task posted, no crash
  base::RunLoop().RunUntilIdle();
}

// Tests that playback_rate=0 is NOT filtered when seek_filter flag is false
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_zero_playback_rate_without_seek_filter) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // Create position with playback_rate = 0 (paused state)
  media_session::MediaPosition pos(0.0, base::Seconds(120), base::Seconds(30),
                                   false);

  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));

  // Expected: Update processed (filter only applies when seek_filter flag is true AND rate == 0)
  EXPECT_TRUE(fake->set_playback_pos_called_);
}

// Tests that large position jumps crossing multiple second boundaries are processed
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_large_position_jump) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // First call: position at 10 seconds
  media_session::MediaPosition pos1(1.0, base::Seconds(120), base::Seconds(10),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos1));
  EXPECT_TRUE(fake->set_playback_pos_called_);

  // Reset the flag for next call
  fake->set_playback_pos_called_ = false;

  // Second call: position at 60 seconds (50 second jump)
  // Note: InSeconds() returns different values (10 vs 60), triggering update
  media_session::MediaPosition pos2(1.0, base::Seconds(120), base::Seconds(60),
                                    false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos2));

  // Expected: Second update processed (InSeconds() values differ: 10 != 60)
  EXPECT_TRUE(fake->set_playback_pos_called_);
}

// Tests that update_time is correctly set to approximately current system time
TEST_F(MediaSessionOHOSTest, MediaSessionPositionChanged_update_time_correctness) {
  auto* fake = new FakeAVSessionAdapter(true, true);
  media_session_ohos_->avsession_adapter_.reset(fake);
  media_session_ohos_->media_type_ =
      OHOS::NWeb::MediaAVSessionType::MEDIA_TYPE_AUDIO;

  // Reset state to ensure clean test environment
  media_session_ohos_->old_position_.reset();
  media_session_ohos_->is_playing_before_seeking_ = false;

  // Get current time before the call
  auto before_time = std::chrono::system_clock::now();
  auto before_millis = std::chrono::time_point_cast<std::chrono::milliseconds>(before_time);
  int64_t before_time_ms = before_millis.time_since_epoch().count();

  // Call MediaSessionPositionChanged
  media_session::MediaPosition pos(1.0, base::Seconds(120), base::Seconds(30),
                                   false);
  ASSERT_NO_FATAL_FAILURE(
      media_session_ohos_->MediaSessionPositionChanged(pos));

  // Get current time after the call
  auto after_time = std::chrono::system_clock::now();
  auto after_millis = std::chrono::time_point_cast<std::chrono::milliseconds>(after_time);
  int64_t after_time_ms = after_millis.time_since_epoch().count();

  // Expected: update_time is set correctly (approximately current system time)
  // Using 100ms tolerance to account for timing variations
  int64_t update_time = media_session_ohos_->av_position_->GetUpdateTime();
  EXPECT_GE(update_time, before_time_ms);
  EXPECT_LE(update_time, after_time_ms + 100);
}
}  // namespace content