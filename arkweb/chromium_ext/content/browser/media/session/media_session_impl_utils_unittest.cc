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

#include "arkweb/chromium_ext/content/browser/media/session/media_session_impl_utils.h"

#include "base/time/time.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/media/session/mock_media_session_player_observer.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/media_session.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/test/test_renderer_host.h"
#include "media/base/media_content_type.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

using ::testing::_;
using ::testing::NotNull;
using ::testing::Return;

namespace content {

class MediaSessionImplUtilsTest : public RenderViewHostTestHarness {
 public:
  MediaSessionImplUtilsTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}
  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    impl_ = MediaSessionImpl::Get(web_contents());
    utils_ = std::make_unique<MediaSessionImplUtils>(impl_);
    player_observer_ = std::make_unique<MockMediaSessionPlayerObserver>(
        main_rfh(), media::MediaContentType::kPersistent);
  }

  void TearDown() override {
    utils_.reset();
    player_observer_.reset();
    RenderViewHostTestHarness::TearDown();
  }

  raw_ptr<MediaSessionImpl> impl_;
  std::unique_ptr<MediaSessionImplUtils> utils_;
  std::unique_ptr<MockMediaSessionPlayerObserver> player_observer_;
};

TEST_F(MediaSessionImplUtilsTest, CheckPosition) {
  ASSERT_NE(impl_->session_ohos_, nullptr);
  std::optional<media_session::MediaPosition> pos =
      media_session::MediaPosition(1.0, base::Seconds(100), base::Seconds(20),
                                   false);
  ASSERT_NO_FATAL_FAILURE(utils_->CheckPosition(pos));
  impl_->session_ohos_.reset();
  ASSERT_NO_FATAL_FAILURE(utils_->CheckPosition(pos));
}

TEST_F(MediaSessionImplUtilsTest, IsSuspended) {
  EXPECT_FALSE(impl_->IsSuspended());
  ASSERT_NO_FATAL_FAILURE(utils_->IsSuspended());
  int player_id = player_observer_->StartNewPlayer();
  impl_->AddPlayer(player_observer_.get(), player_id);
  impl_->Suspend(MediaSession::SuspendType::kSystem);
  EXPECT_TRUE(impl_->IsSuspended());
  ASSERT_NO_FATAL_FAILURE(utils_->IsSuspended());
  impl_->Resume(MediaSession::SuspendType::kSystem);
  impl_->RemovePlayer(player_observer_.get(), player_id);
}

TEST_F(MediaSessionImplUtilsTest, PushBackMediaImage1) {
  ASSERT_NE(nullptr, impl_->web_contents());
  std::vector<media_session::MediaImage> artwork(2);
  std::string poster = impl_->web_contents()->GetVideoPoster();
  utils_->PushBackMediaImage(artwork);
  ASSERT_EQ(artwork.size(), 3u);
  EXPECT_EQ(GURL(poster), artwork.back().src);
}

TEST_F(MediaSessionImplUtilsTest, PushBackMediaImage2) {
  ASSERT_NE(nullptr, impl_->web_contents());
  std::vector<media_session::MediaImage> artwork;
  artwork.emplace_back();
  static_cast<content::WebContentsObserver*>(impl_)->Observe(nullptr);
  ASSERT_NO_FATAL_FAILURE(utils_->PushBackMediaImage(artwork));
  EXPECT_EQ(artwork.size(), 1);
}

TEST_F(MediaSessionImplUtilsTest, SetMediaTitle1) {
  ASSERT_NE(nullptr, impl_->web_contents());
  media_session::MediaMetadata md;
  static_cast<content::WebContentsImpl*>(impl_->web_contents())->media_title_ =
      "arkweb-title";
  utils_->SetMediaTitle(md);
  ASSERT_FALSE(md.title.empty());
}

TEST_F(MediaSessionImplUtilsTest, SetMediaTitle2) {
  ASSERT_NE(nullptr, impl_->web_contents());
  media_session::MediaMetadata md;
  static_cast<content::WebContentsImpl*>(impl_->web_contents())
      ->media_title_.clear();
  utils_->SetMediaTitle(md);
  EXPECT_TRUE(md.title.empty());
}

TEST_F(MediaSessionImplUtilsTest, SetMediaTitle3) {
  media_session::MediaMetadata md;
  static_cast<content::WebContentsObserver*>(impl_)->Observe(nullptr);
  utils_->SetMediaTitle(md);
  EXPECT_TRUE(md.title.empty());
}

TEST_F(MediaSessionImplUtilsTest, DoEndSessionWhenHide1) {
  ASSERT_NE(impl_->session_ohos_, nullptr);
  impl_->session_ohos_->SetPauseByAvsession(false);
  ASSERT_NO_FATAL_FAILURE(utils_->DoEndSessionWhenHide());
}

TEST_F(MediaSessionImplUtilsTest, DoEndSessionWhenHide2) {
  ASSERT_NE(impl_->session_ohos_, nullptr);
  impl_->session_ohos_->SetPauseByAvsession(false);
  int pid = player_observer_->StartNewPlayer();
  impl_->AddPlayer(player_observer_.get(), pid);
  impl_->Suspend(MediaSession::SuspendType::kSystem);
  ASSERT_TRUE(impl_->IsSuspended());
  ASSERT_NO_FATAL_FAILURE(utils_->DoEndSessionWhenHide());
  impl_->Resume(MediaSession::SuspendType::kSystem);
  impl_->RemovePlayer(player_observer_.get(), pid);
}

TEST_F(MediaSessionImplUtilsTest, DoEndSessionWhenHide3) {
  ASSERT_NE(impl_->session_ohos_, nullptr);
  impl_->session_ohos_->SetPauseByAvsession(true);
  ASSERT_FALSE(impl_->IsSuspended());
  ASSERT_NO_FATAL_FAILURE(utils_->DoEndSessionWhenHide());
}

TEST_F(MediaSessionImplUtilsTest, DoEndSessionWhenHide4) {
  ASSERT_NE(impl_->session_ohos_, nullptr);
  impl_->session_ohos_->SetPauseByAvsession(true);
  int pid = player_observer_->StartNewPlayer();
  impl_->AddPlayer(player_observer_.get(), pid);
  impl_->Suspend(MediaSession::SuspendType::kSystem);
  ASSERT_TRUE(impl_->IsSuspended());
  ASSERT_NO_FATAL_FAILURE(utils_->DoEndSessionWhenHide());
  impl_->Resume(MediaSession::SuspendType::kSystem);
  impl_->RemovePlayer(player_observer_.get(), pid);
}

}  // namespace content
