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
}  // namespace content