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
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/media/session/mock_media_session_player_observer.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "media/base/mock_filters.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "arkweb/chromium_ext/content/browser/media/ohos/ohos_media_player_renderer.h"
#undef private

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StrictMock;

namespace content {

class MockClientExtension
    : public media::mojom::MediaPlayerRendererClientExtension {
 public:
  void OnDurationChange(base::TimeDelta) override {}
  void OnVideoSizeChange(const gfx::Size&) override {}
  void OnFrameUpdate(media::mojom::OhosSurfaceBufferHandlePtr) override {}
  mojo::Receiver<media::mojom::MediaPlayerRendererClientExtension> receiver{
      this};
};

class OHOSMediaPlayerRendererTest : public RenderViewHostTestHarness {
 public:
  OHOSMediaPlayerRendererTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    web_contents_ = static_cast<WebContentsImpl*>(web_contents());
    ASSERT_NE(web_contents_, nullptr);
    mock_resource_ = std::make_unique<media::MockMediaResource>();
  }

  void TearDown() override {
    renderer_.reset();
    mock_resource_.reset();
    RenderViewHostTestHarness::TearDown();
  }

  void CreateRenderer() {
    const int process_id = main_rfh()->GetProcess()->GetID();
    const int routing_id = main_rfh()->GetRoutingID();
    mojo::PendingReceiver<media::mojom::MediaPlayerRendererExtension>
        renderer_receiver;
    mojo::PendingRemote<media::mojom::MediaPlayerRendererClientExtension>
        client_remote;

    renderer_ = std::make_unique<OHOSMediaPlayerRenderer>(
        process_id, routing_id, web_contents_, std::move(renderer_receiver),
        std::move(client_remote));
  }

  void OnInitCallback(media::PipelineStatus status) {
    callback_called_ = true;
    callback_status_ = status;
  }

  void BindClientExtension(OHOSMediaPlayerRenderer* renderer,
                           MockClientExtension& fake) {
    mojo::PendingRemote<media::mojom::MediaPlayerRendererClientExtension>
        remote;
    auto receiver = remote.InitWithNewPipeAndPassReceiver();
    renderer->client_extension_.Bind(std::move(remote));
    fake.receiver.Bind(std::move(receiver));
  }

  WebContentsImpl* web_contents_ = nullptr;
  std::unique_ptr<OHOSMediaPlayerRenderer> renderer_;
  StrictMock<media::MockRendererClient> mock_client_;
  std::unique_ptr<media::MockMediaResource> mock_resource_;
  bool callback_called_ = false;
  media::PipelineStatus callback_status_;
};

TEST_F(OHOSMediaPlayerRendererTest, Initialize1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  EXPECT_CALL(*mock_resource_, GetType())
      .WillOnce(Return(media::MediaResource::Type::kStream));
  callback_called_ = false;
  auto init_cb = base::BindOnce(&OHOSMediaPlayerRendererTest::OnInitCallback,
                                base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->Initialize(
      mock_resource_.get(), &mock_client_, media::RequestSurfaceCB(),
      media::VideoDecoderChangedCB(), std::move(init_cb)));
  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(callback_status_, media::PIPELINE_ERROR_INITIALIZATION_FAILED);
}

TEST_F(OHOSMediaPlayerRendererTest, Initialize2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);

  EXPECT_CALL(*mock_resource_, GetType())
      .WillOnce(Return(media::MediaResource::Type::KUrl));
  EXPECT_CALL(*mock_resource_, GetMediaUrlParams())
      .WillOnce(ReturnRef(url_params));
  callback_called_ = false;
  auto init_cb = base::BindOnce(&OHOSMediaPlayerRendererTest::OnInitCallback,
                                base::Unretained(this));

  ASSERT_NO_FATAL_FAILURE(renderer_->Initialize(
      mock_resource_.get(), &mock_client_, media::RequestSurfaceCB(),
      media::VideoDecoderChangedCB(), std::move(init_cb)));
  ASSERT_NE(renderer_, nullptr);
}

TEST_F(OHOSMediaPlayerRendererTest, CreateMediaPlayer1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->url_params_.reset();
  callback_called_ = false;
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(callback_status_, media::PIPELINE_ERROR_INITIALIZATION_FAILED);
}

TEST_F(OHOSMediaPlayerRendererTest, CreateMediaPlayer2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  callback_called_ = false;
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(callback_status_, media::PIPELINE_OK);
}

TEST_F(OHOSMediaPlayerRendererTest, StartPlayingFrom1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->has_error_ = true;
  renderer_->media_player_ = nullptr;
  EXPECT_CALL(mock_client_, OnBufferingStateChange(_, _)).Times(0);
  ASSERT_NO_FATAL_FAILURE(renderer_->StartPlayingFrom(base::Seconds(10)));
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(1.0));
}

TEST_F(OHOSMediaPlayerRendererTest, StartPlayingFrom2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->has_error_ = true;
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  EXPECT_CALL(mock_client_, OnBufferingStateChange(_, _)).Times(0);
  ASSERT_NO_FATAL_FAILURE(renderer_->StartPlayingFrom(base::Seconds(10)));
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(1.0));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateVolume());
  EXPECT_EQ(renderer_->GetMediaTime(), base::Milliseconds(0));
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  ASSERT_NO_FATAL_FAILURE(renderer_->SetVideoSurface(1));
#endif
}

TEST_F(OHOSMediaPlayerRendererTest, StartPlayingFrom3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->has_error_ = false;
  renderer_->media_player_ = nullptr;
  EXPECT_CALL(mock_client_, OnBufferingStateChange(_, _)).Times(0);
  ASSERT_NO_FATAL_FAILURE(renderer_->StartPlayingFrom(base::Seconds(10)));
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(1.0));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateVolume());
  EXPECT_EQ(renderer_->GetMediaTime(), base::Milliseconds(0));
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  ASSERT_NO_FATAL_FAILURE(renderer_->SetVideoSurface(1));
#endif
}

TEST_F(OHOSMediaPlayerRendererTest, StartPlayingFrom4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->has_error_ = false;
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);

  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  EXPECT_NE(renderer_->media_player_, nullptr);
  renderer_->renderer_client_ = &mock_client_;
  EXPECT_CALL(mock_client_, OnBufferingStateChange(_, _)).Times(1);
  ASSERT_NO_FATAL_FAILURE(renderer_->StartPlayingFrom(base::Seconds(10)));
}

TEST_F(OHOSMediaPlayerRendererTest, SetPlaybackRate1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  renderer_->renderer_client_ = &mock_client_;

  ASSERT_NE(renderer_->media_player_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(0.0));
}

TEST_F(OHOSMediaPlayerRendererTest, SetPlaybackRate2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  renderer_->renderer_client_ = &mock_client_;

  ASSERT_NE(renderer_->media_player_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(0.5));
}

TEST_F(OHOSMediaPlayerRendererTest, SetPlaybackRate3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  renderer_->renderer_client_ = &mock_client_;

  ASSERT_NE(renderer_->media_player_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(1.8));
}

TEST_F(OHOSMediaPlayerRendererTest, SetPlaybackRate4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  renderer_->renderer_client_ = &mock_client_;

  ASSERT_NE(renderer_->media_player_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(1.1));
}

TEST_F(OHOSMediaPlayerRendererTest, SetPlaybackRate5) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  renderer_->renderer_client_ = &mock_client_;

  ASSERT_NE(renderer_->media_player_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(1.5));
}

TEST_F(OHOSMediaPlayerRendererTest, SetPlaybackRate6) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  renderer_->renderer_client_ = &mock_client_;

  ASSERT_NE(renderer_->media_player_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(2.0));
}

TEST_F(OHOSMediaPlayerRendererTest, TryOrCreateMediaPlayer1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->TryOrCreateMediaPlayer());
  EXPECT_EQ(renderer_->media_player_, nullptr);
}

TEST_F(OHOSMediaPlayerRendererTest, TryOrCreateMediaPlayer2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->SetNativeWindowSurface(123);
  EXPECT_EQ(renderer_->url_params_, nullptr);
  EXPECT_EQ(renderer_->media_player_, nullptr);
}

TEST_F(OHOSMediaPlayerRendererTest, TryOrCreateMediaPlayer3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  callback_called_ = false;
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  renderer_->SetNativeWindowSurface(456);
  base::RunLoop().RunUntilIdle();
  EXPECT_NE(renderer_->media_player_, nullptr);
}

TEST_F(OHOSMediaPlayerRendererTest, GetMediaResourceGetter1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  auto* getter1 = renderer_->GetMediaResourceGetter();
  EXPECT_NE(getter1, nullptr);
  auto* getter2 = renderer_->GetMediaResourceGetter();
  EXPECT_NE(getter2, nullptr);
}

TEST_F(OHOSMediaPlayerRendererTest, GetMediaResourceGetter2) {
  mojo::PendingReceiver<media::mojom::MediaPlayerRendererExtension>
      renderer_receiver;
  mojo::PendingRemote<media::mojom::MediaPlayerRendererClientExtension>
      client_remote;
  auto bad_render = std::make_unique<OHOSMediaPlayerRenderer>(
      -12345, 0, web_contents_, std::move(renderer_receiver),
      std::move(client_remote));
  auto* getter = bad_render->GetMediaResourceGetter();
  EXPECT_EQ(getter, nullptr);
}

TEST_F(OHOSMediaPlayerRendererTest, OnMediaDurationChanged) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  MockClientExtension fake;
  BindClientExtension(renderer_.get(), fake);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnMediaDurationChanged(base::TimeDelta()));
  EXPECT_EQ(renderer_->duration_, media::kInfiniteDuration);
  base::TimeDelta delta = base::Seconds(5);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnMediaDurationChanged(delta));
  EXPECT_EQ(renderer_->duration_, delta);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnMediaDurationChanged(delta));
}

TEST_F(OHOSMediaPlayerRendererTest, OnError1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  EXPECT_CALL(mock_client_, OnError(::testing::_)).Times(0);
  renderer_->renderer_client_ = &mock_client_;
  renderer_->initialized_ = true;
  ASSERT_NO_FATAL_FAILURE(renderer_->OnError(
      media::OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_INVALID_CODE));
  EXPECT_FALSE(renderer_->has_error_);
}

TEST_F(OHOSMediaPlayerRendererTest, OnError2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->renderer_client_ = &mock_client_;
  renderer_->initialized_ = true;
  EXPECT_CALL(mock_client_, OnError(::testing::_)).Times(1);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnError(
      media::OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_FORMAT));
  EXPECT_TRUE(renderer_->has_error_);
}

TEST_F(OHOSMediaPlayerRendererTest, OnError3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->renderer_client_ = &mock_client_;
  renderer_->initialized_ = false;
  EXPECT_CALL(mock_client_, OnError(::testing::_)).Times(0);
  ASSERT_NO_FATAL_FAILURE(
      renderer_->OnError(media::OHOSMediaPlayerBridge::MEDIA_ERROR_FORMAT));
  EXPECT_TRUE(renderer_->has_error_);
}

TEST_F(OHOSMediaPlayerRendererTest, OnVideoSizeChanged) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  MockClientExtension fake;
  BindClientExtension(renderer_.get(), fake);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnVideoSizeChanged(640, 360));
  EXPECT_EQ(renderer_->video_size_, gfx::Size(640, 360));
  ASSERT_NO_FATAL_FAILURE(renderer_->OnVideoSizeChanged(1280, 720));
  EXPECT_EQ(renderer_->video_size_, gfx::Size(1280, 720));
  ASSERT_NO_FATAL_FAILURE(renderer_->OnVideoSizeChanged(1280, 720));
}

TEST_F(OHOSMediaPlayerRendererTest, OnPlayerInterruptEvent1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->web_contents_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_->OnPlayerInterruptEvent(2));
}

TEST_F(OHOSMediaPlayerRendererTest, OnPlayerInterruptEvent2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  mojo::PendingReceiver<media::mojom::MediaPlayerRendererExtension>
      renderer_receiver;
  mojo::PendingRemote<media::mojom::MediaPlayerRendererClientExtension>
      client_remote;
  auto temp = std::make_unique<OHOSMediaPlayerRenderer>(
      -12345, 0, web_contents_, std::move(renderer_receiver),
      std::move(client_remote));
  ASSERT_NE(temp, nullptr);
  ASSERT_NO_FATAL_FAILURE(temp->OnPlayerInterruptEvent(2));
}

TEST_F(OHOSMediaPlayerRendererTest, OnPlayerInterruptEvent3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  auto* ms = MediaSessionImpl::Get(web_contents());
  ASSERT_NE(ms, nullptr);
  ms->audioResumeInterval_ = 1;
  if (ms->IsSuspended()) {
    ms->Resume(MediaSession::SuspendType::kSystem);
  }
  ASSERT_NO_FATAL_FAILURE(renderer_->OnPlayerInterruptEvent(3));
  ASSERT_NO_FATAL_FAILURE(renderer_->OnPlayerInterruptEvent(2));
  ASSERT_NO_FATAL_FAILURE(renderer_->OnPlayerInterruptEvent(0));
}

TEST_F(OHOSMediaPlayerRendererTest, OnPlayerInterruptEvent4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  auto* ms = MediaSessionImpl::Get(web_contents());
  ASSERT_NE(ms, nullptr);
  ms->audioResumeInterval_ = -1;
  if (!ms->IsSuspended()) {
    ms->Suspend(MediaSession::SuspendType::kSystem);
  }
  ASSERT_NO_FATAL_FAILURE(renderer_->OnPlayerInterruptEvent(1));
}

TEST_F(OHOSMediaPlayerRendererTest, OnPlayerInterruptEvent5) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  auto* ms = MediaSessionImpl::Get(web_contents());
  ASSERT_NE(ms, nullptr);
  ms->audioResumeInterval_ = -1;
  StrictMock<MockMediaSessionPlayerObserver> player(
      main_rfh(), media::MediaContentType::kPersistent);
  const int kId = player.StartNewPlayer();
  ms->AddPlayer(&player, kId);
  player.SetPlaying(kId, true);
  ms->Suspend(MediaSession::SuspendType::kSystem);
  base::RunLoop().RunUntilIdle();
  ASSERT_TRUE(ms->IsSuspended());
  renderer_->OnPlayerInterruptEvent(1);
  EXPECT_FALSE(ms->IsSuspended());
  ms->RemovePlayer(&player, kId);
}

TEST_F(OHOSMediaPlayerRendererTest, OnPlayerInterruptEvent6) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  auto* ms = MediaSessionImpl::Get(web_contents());
  ASSERT_NE(ms, nullptr);
  ms->audioResumeInterval_ = -1;
  if (ms->IsSuspended()) {
    ms->Resume(MediaSession::SuspendType::kSystem);
  }
  bool suspended_before = ms->IsSuspended();
  ASSERT_NO_FATAL_FAILURE(renderer_->OnPlayerInterruptEvent(1));
  EXPECT_EQ(ms->IsSuspended(), suspended_before);
}

TEST_F(OHOSMediaPlayerRendererTest, IsNeedResume1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->intervalSinceLastSuspend_ = std::time(nullptr);
  EXPECT_TRUE(renderer_->isNeedResume(-1));
}

TEST_F(OHOSMediaPlayerRendererTest, IsNeedResume2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->intervalSinceLastSuspend_ = std::time(nullptr);
  EXPECT_FALSE(renderer_->isNeedResume(0));
}

TEST_F(OHOSMediaPlayerRendererTest, IsNeedResume3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  time_t now = std::time(nullptr);
  renderer_->intervalSinceLastSuspend_ = now - 3;
  EXPECT_TRUE(renderer_->isNeedResume(5));
}

TEST_F(OHOSMediaPlayerRendererTest, IsNeedResume4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  time_t now = std::time(nullptr);
  renderer_->intervalSinceLastSuspend_ = now - 10;
  EXPECT_FALSE(renderer_->isNeedResume(5));
}

TEST_F(OHOSMediaPlayerRendererTest, IsNeedResume5) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  time_t now = std::time(nullptr);
  renderer_->intervalSinceLastSuspend_ = now - 5;
  EXPECT_TRUE(renderer_->isNeedResume(5));
}

TEST_F(OHOSMediaPlayerRendererTest, OnAudioStateChanged) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnAudioStateChanged(true));
  ASSERT_NO_FATAL_FAILURE(renderer_->OnAudioStateChanged(false));
  renderer_->web_contents_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_->OnAudioStateChanged(true));
}

TEST_F(OHOSMediaPlayerRendererTest, OnPlayerSeekBack) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnPlayerSeekBack(base::TimeDelta()));
  renderer_->web_contents_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_->OnPlayerSeekBack(base::TimeDelta()));
}

TEST_F(OHOSMediaPlayerRendererTest, GetGrantMediaFileAccessDirs1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->web_contents_ = nullptr;
  std::vector<std::string> out;
  ASSERT_NO_FATAL_FAILURE(renderer_->GetGrantMediaFileAccessDirs(out));
  EXPECT_TRUE(out.empty());
}

TEST_F(OHOSMediaPlayerRendererTest, GetGrantMediaFileAccessDirs2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  auto* ms = MediaSessionImpl::Get(web_contents());
  ASSERT_NE(ms, nullptr);
  ms->grantMediaFileAccessDirs_.clear();
  ms->grantMediaFileAccessDirs_.push_back("/data/test/dirA");
  ms->grantMediaFileAccessDirs_.push_back("/data/test/dirB");
  std::vector<std::string> out;
  renderer_->GetGrantMediaFileAccessDirs(out);
  ASSERT_EQ(out.size(), 2u);
  EXPECT_EQ(out[0], "/data/test/dirA");
  EXPECT_EQ(out[1], "/data/test/dirB");
}

TEST_F(OHOSMediaPlayerRendererTest, GetGrantMediaFileAccessDirs3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  auto* ms = MediaSessionImpl::Get(web_contents());
  ASSERT_NE(ms, nullptr);
  ms->grantMediaFileAccessDirs_.clear();
  ms->fileAccess_ = false;
  std::vector<std::string> out;
  renderer_->GetGrantMediaFileAccessDirs(out);
  ASSERT_EQ(out.size(), 1u);
}

TEST_F(OHOSMediaPlayerRendererTest, GetGrantMediaFileAccessDirs4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  auto* ms = MediaSessionImpl::Get(web_contents());
  ASSERT_NE(ms, nullptr);
  ms->grantMediaFileAccessDirs_.clear();
  ms->fileAccess_ = true;
  std::vector<std::string> out;
  renderer_->GetGrantMediaFileAccessDirs(out);
  EXPECT_TRUE(out.empty());
}

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(OHOSMediaPlayerRendererTest, PipEnable) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->has_error_ = true;
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  ASSERT_NO_FATAL_FAILURE(renderer_->PipEnable(true));
  renderer_->media_player_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_->PipEnable(false));
}
#endif  // ARKWEB_PIP

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(OHOSMediaPlayerRendererTest, RecycleDmaBuffer) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->has_error_ = true;
  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->url_params_ = std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_ = base::BindOnce(
      &OHOSMediaPlayerRendererTest::OnInitCallback, base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->CreateMediaPlayer());
  base::RunLoop().RunUntilIdle();
  ASSERT_NO_FATAL_FAILURE(renderer_->RecycleDmaBuffer());
  ASSERT_NO_FATAL_FAILURE(renderer_->ResumeDmaBuffer());
  renderer_->media_player_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(renderer_->RecycleDmaBuffer());
  ASSERT_NO_FATAL_FAILURE(renderer_->ResumeDmaBuffer());
}
#endif  // ARKWEB_MEDIA_DMABUF

TEST_F(OHOSMediaPlayerRendererTest, CtorNoObserver) {
  mojo::PendingReceiver<media::mojom::MediaPlayerRendererExtension>
      renderer_receiver;
  mojo::PendingRemote<media::mojom::MediaPlayerRendererClientExtension>
      client_remote;
  auto result = std::make_unique<OHOSMediaPlayerRenderer>(
      main_rfh()->GetProcess()->GetID(), main_rfh()->GetRoutingID(), nullptr,
      std::move(renderer_receiver), std::move(client_remote));
  ASSERT_NE(result, nullptr);
}

}  // namespace content