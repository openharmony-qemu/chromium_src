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
#include "media/base/mock_filters.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "net/cookies/site_for_cookies.h"
#include "services/network/public/mojom/restricted_cookie_manager.mojom.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"
#include "url/origin.h"
#define private public
#include "content/browser/media/ohos/ohos_custom_media_player_renderer.h"
#define ARKWEB_TEST_INCLUDE
#include "content/browser/media/ohos/ohos_custom_media_player_renderer.cc"
#undef private

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StrictMock;

namespace content {
class MockOHOSCustomMediaPlayerRenderer : public OHOSCustomMediaPlayerRenderer {
 public:
  MockOHOSCustomMediaPlayerRenderer()
      : OHOSCustomMediaPlayerRenderer(
            0,
            0,
            0,
            nullptr,
            mojo::PendingReceiver<media::mojom::MediaPlayerRendererExtension>(),
            mojo::PendingRemote<
                media::mojom::CustomMediaPlayerRendererClientExtension>()) {}
  MOCK_METHOD1(OnMediaDurationChanged, void(base::TimeDelta duration));
  MOCK_METHOD0(OnPlaybackComplete, void());
  MOCK_METHOD1(OnError, void(int error));
  MOCK_METHOD2(OnVideoSizeChanged, void(int width, int height));

  base::WeakPtr<OHOSCustomMediaPlayerRenderer> GetWeakPtr() {
    return weak_factory_.GetWeakPtr();
  }

 private:
  base::WeakPtrFactory<OHOSCustomMediaPlayerRenderer> weak_factory_{this};
};

class MockCustomMediaPlayer : public content::CustomMediaPlayer {
 public:
  MockCustomMediaPlayer() = default;
  ~MockCustomMediaPlayer() override = default;

  MOCK_METHOD1(Initialize, int32_t(const std::string& surface_id));
  MOCK_METHOD4(UpdateLayerRect, void(int x, int y, int width, int height));
  MOCK_METHOD0(Play, void());
  MOCK_METHOD0(Pause, void());
  MOCK_METHOD1(Seek, void(double target_time));
  MOCK_METHOD1(SetVolume, void(double volume));
  MOCK_METHOD1(SetMuted, void(bool muted));
  MOCK_METHOD1(SetPlaybackRate, void(double rate));
  MOCK_METHOD0(Release, void());
  MOCK_METHOD0(EnterFullscreen, void());
  MOCK_METHOD0(ExitFullscreen, void());
  MOCK_METHOD0(ResumeMediaPlayer, void());
  MOCK_METHOD1(SuspendMediaPlayer, void(int suspend_type));
};

class MockClientExtension
    : public media::mojom::CustomMediaPlayerRendererClientExtension {
 public:
  void OnDurationChange(base::TimeDelta) override {}
  void OnVideoSizeChange(const gfx::Size&) override {}
  void UpdatePlaybackStatus(uint32_t) override {}
  void UpdateVolume(double) override {}
  void UpdateMuted(bool) override {}
  void UpdatePlaybackRate(double) override {}
  void UpdateBufferedEndTime(double) override {}

  mojo::Receiver<media::mojom::CustomMediaPlayerRendererClientExtension>
      receiver{this};
};

class OHOSCustomMediaPlayerRendererTest : public RenderViewHostTestHarness {
 public:
  OHOSCustomMediaPlayerRendererTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    web_contents_ = static_cast<WebContentsImpl*>(web_contents());
    ASSERT_NE(web_contents_, nullptr);
    mock_resource_ = std::make_unique<media::MockMediaResource>();

    test_url_ = GURL("https://example");
    test_origin_ = url::Origin::Create(test_url_);
    test_site_for_cookies_ = net::SiteForCookies::FromUrl(test_url_);
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
    mojo::PendingRemote<media::mojom::CustomMediaPlayerRendererClientExtension>
        client_remote;

    renderer_ = std::make_unique<OHOSCustomMediaPlayerRenderer>(
        process_id, routing_id, 1, web_contents_, std::move(renderer_receiver),
        std::move(client_remote));
  }

  void OnInitCallback(media::PipelineStatus status) {
    callback_called_ = true;
    callback_status_ = status;
  }

  void BindClientExtension(OHOSCustomMediaPlayerRenderer* renderer,
                           MockClientExtension& fake) {
    mojo::PendingRemote<media::mojom::CustomMediaPlayerRendererClientExtension>
        remote;
    auto receiver = remote.InitWithNewPipeAndPassReceiver();
    renderer->client_extension_.Bind(std::move(remote));
    fake.receiver.Bind(std::move(receiver));
  }

  WebContentsImpl* web_contents_ = nullptr;
  std::unique_ptr<OHOSCustomMediaPlayerRenderer> renderer_;
  StrictMock<media::MockRendererClient> mock_client_;
  std::unique_ptr<media::MockMediaResource> mock_resource_;
  bool callback_called_ = false;
  media::PipelineStatus callback_status_;
  GURL test_url_;
  url::Origin test_origin_;
  net::SiteForCookies test_site_for_cookies_;
};

TEST_F(OHOSCustomMediaPlayerRendererTest,
       GetRestrictedCookieManagerForContext1) {
  mojo::PendingRemote<network::mojom::RestrictedCookieManager> result =
      GetRestrictedCookieManagerForContext(
          nullptr, test_url_, test_site_for_cookies_, test_origin_,
          static_cast<RenderFrameHostImpl*>(main_rfh()));
  EXPECT_FALSE(result.is_valid());
}

TEST_F(OHOSCustomMediaPlayerRendererTest,
       GetRestrictedCookieManagerForContext2) {
  BrowserContext* browser_context = web_contents_->GetBrowserContext();
  ASSERT_NE(browser_context, nullptr);
  mojo::PendingRemote<network::mojom::RestrictedCookieManager> result =
      GetRestrictedCookieManagerForContext(browser_context, test_url_,
                                           test_site_for_cookies_, test_origin_,
                                           nullptr);
  EXPECT_TRUE(result.is_valid());
}

TEST_F(OHOSCustomMediaPlayerRendererTest,
       GetRestrictedCookieManagerForContext3) {
  BrowserContext* browser_context = web_contents_->GetBrowserContext();
  ASSERT_NE(browser_context, nullptr);

  mojo::PendingRemote<network::mojom::RestrictedCookieManager> result =
      GetRestrictedCookieManagerForContext(
          browser_context, test_url_, test_site_for_cookies_, test_origin_,
          static_cast<RenderFrameHostImpl*>(main_rfh()));
  EXPECT_TRUE(result.is_valid());
}

TEST_F(OHOSCustomMediaPlayerRendererTest, ConvertTo) {
  EXPECT_EQ(ConvertTo(0), MediaInfo::Preload::NONE);
  EXPECT_EQ(ConvertTo(1), MediaInfo::Preload::METADATA);
  EXPECT_EQ(ConvertTo(2), MediaInfo::Preload::AUTO);
  EXPECT_EQ(ConvertTo(3), MediaInfo::Preload::AUTO);
  EXPECT_EQ(ConvertTo(100), MediaInfo::Preload::AUTO);
  EXPECT_EQ(ConvertTo(UINT32_MAX), MediaInfo::Preload::AUTO);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, Initialize1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  callback_called_ = false;
  auto init_cb =
      base::BindOnce(&OHOSCustomMediaPlayerRendererTest::OnInitCallback,
                     base::Unretained(this));

  ASSERT_NO_FATAL_FAILURE(renderer_->Initialize(
      nullptr, &mock_client_, media::RequestSurfaceCB(),
      media::VideoDecoderChangedCB(), std::move(init_cb)));

  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(callback_status_, media::PIPELINE_ERROR_INITIALIZATION_FAILED);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, Initialize2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  EXPECT_CALL(*mock_resource_, GetType())
      .WillOnce(Return(media::MediaResource::Type::kStream));

  callback_called_ = false;
  auto init_cb =
      base::BindOnce(&OHOSCustomMediaPlayerRendererTest::OnInitCallback,
                     base::Unretained(this));

  ASSERT_NO_FATAL_FAILURE(renderer_->Initialize(
      mock_resource_.get(), &mock_client_, media::RequestSurfaceCB(),
      media::VideoDecoderChangedCB(), std::move(init_cb)));

  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(callback_status_, media::PIPELINE_ERROR_INITIALIZATION_FAILED);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, Initialize3) {
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
  auto init_cb =
      base::BindOnce(&OHOSCustomMediaPlayerRendererTest::OnInitCallback,
                     base::Unretained(this));

  ASSERT_NO_FATAL_FAILURE(renderer_->Initialize(
      mock_resource_.get(), &mock_client_, media::RequestSurfaceCB(),
      media::VideoDecoderChangedCB(), std::move(init_cb)));

  EXPECT_NE(renderer_->media_url_params_, nullptr);
  EXPECT_FALSE(renderer_->init_cb_.is_null());
  EXPECT_EQ(renderer_->renderer_client_, &mock_client_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, GetCookies1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->media_url_params_.reset();
  renderer_->cookies_.reset();

  ASSERT_NO_FATAL_FAILURE(renderer_->GetCookies());
  base::RunLoop().RunUntilIdle();

  EXPECT_FALSE(renderer_->cookies_.has_value());
}

TEST_F(OHOSCustomMediaPlayerRendererTest, GetCookies2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->media_url_params_ =
      std::make_unique<media::MediaUrlParams>(url_params);

  int unauth_child = -12345;
  renderer_->media_player_id_.frame_routing_id.child_id = unauth_child;

  renderer_->cookies_.reset();
  ASSERT_NO_FATAL_FAILURE(renderer_->GetCookies());
  base::RunLoop().RunUntilIdle();

  ASSERT_TRUE(renderer_->cookies_.has_value());
  EXPECT_TRUE(renderer_->cookies_->empty());
}

TEST_F(OHOSCustomMediaPlayerRendererTest, GetCookies3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->media_url_params_ =
      std::make_unique<media::MediaUrlParams>(url_params);

  const url::Origin origin = url::Origin::Create(url_params.media_url);
  auto* policy = ChildProcessSecurityPolicyImpl::GetInstance();

  int ghost_child = -99999;
  policy->GrantCommitOrigin(ghost_child, origin);
  policy->GrantRequestOrigin(ghost_child, origin);

  renderer_->media_player_id_.frame_routing_id.child_id = ghost_child;
  renderer_->cookies_.reset();
  ASSERT_NO_FATAL_FAILURE(renderer_->GetCookies());
  base::RunLoop().RunUntilIdle();

  ASSERT_TRUE(renderer_->cookies_.has_value());
  EXPECT_TRUE(renderer_->cookies_->empty());
}

TEST_F(OHOSCustomMediaPlayerRendererTest, GetCookies4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  renderer_->media_url_params_ =
      std::make_unique<media::MediaUrlParams>(url_params);

  int good_child = main_rfh()->GetProcess()->GetID();
  renderer_->media_player_id_.frame_routing_id.child_id = good_child;

  const url::Origin origin = url::Origin::Create(url_params.media_url);
  auto* policy = ChildProcessSecurityPolicyImpl::GetInstance();
  policy->GrantCommitOrigin(good_child, origin);
  policy->GrantRequestOrigin(good_child, origin);

  renderer_->cookies_.reset();
  ASSERT_NO_FATAL_FAILURE(renderer_->GetCookies());
  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(renderer_->cookies_.has_value());
}

TEST_F(OHOSCustomMediaPlayerRendererTest, TryCreateMediaPlayer1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->surface_id_ = -1;
  renderer_->cookies_ = std::string("k=v");

  renderer_->media_player_.reset();
  ASSERT_NO_FATAL_FAILURE(renderer_->TryCreateMediaPlayer());
  base::RunLoop().RunUntilIdle();

  EXPECT_EQ(renderer_->media_player_, nullptr);
  EXPECT_FALSE(renderer_->initialized_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, TryCreateMediaPlayer2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->surface_id_ = 123;
  renderer_->cookies_.reset();

  renderer_->media_player_.reset();
  ASSERT_NO_FATAL_FAILURE(renderer_->TryCreateMediaPlayer());
  base::RunLoop().RunUntilIdle();

  EXPECT_EQ(renderer_->media_player_, nullptr);
  EXPECT_FALSE(renderer_->initialized_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, TryCreateMediaPlayer3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);

  renderer_->surface_id_ = 456;
  renderer_->cookies_ = std::string("a=b");
  renderer_->media_url_params_ =
      std::make_unique<media::MediaUrlParams>(url_params);
  renderer_->init_cb_.Reset();

  renderer_->media_player_.reset();
  ASSERT_NO_FATAL_FAILURE(renderer_->TryCreateMediaPlayer());
  base::RunLoop().RunUntilIdle();

  EXPECT_EQ(renderer_->media_player_, nullptr);
  EXPECT_FALSE(renderer_->initialized_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, TryCreateMediaPlayer4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);

  renderer_->surface_id_ = 789;
  renderer_->cookies_ = std::string("c=d");
  renderer_->media_url_params_ =
      std::make_unique<media::MediaUrlParams>(url_params);

  renderer_->initialized_ = true;

  callback_called_ = false;
  renderer_->init_cb_ =
      base::BindOnce(&OHOSCustomMediaPlayerRendererTest::OnInitCallback,
                     base::Unretained(this));

  ASSERT_NO_FATAL_FAILURE(renderer_->TryCreateMediaPlayer());
  base::RunLoop().RunUntilIdle();

  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(callback_status_, media::PIPELINE_ERROR_INITIALIZATION_FAILED);
  EXPECT_TRUE(renderer_->initialized_);
  EXPECT_EQ(renderer_->media_player_, nullptr);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, TryCreateMediaPlayer5) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->surface_id_ = 111;
  renderer_->cookies_ = std::string("x=y");
  renderer_->media_url_params_.reset();

  callback_called_ = false;
  renderer_->init_cb_ =
      base::BindOnce(&OHOSCustomMediaPlayerRendererTest::OnInitCallback,
                     base::Unretained(this));
  ASSERT_NO_FATAL_FAILURE(renderer_->TryCreateMediaPlayer());
  base::RunLoop().RunUntilIdle();

  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(callback_status_, media::PIPELINE_ERROR_INITIALIZATION_FAILED);
  EXPECT_EQ(renderer_->media_player_, nullptr);
  EXPECT_FALSE(renderer_->initialized_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, TryCreateMediaPlayer6) {
  mojo::PendingReceiver<media::mojom::MediaPlayerRendererExtension> rr;
  mojo::PendingRemote<media::mojom::CustomMediaPlayerRendererClientExtension>
      cr;
  auto r = std::make_unique<OHOSCustomMediaPlayerRenderer>(
      -12345, 0, 1, nullptr, std::move(rr), std::move(cr));
  ASSERT_NE(r, nullptr);

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);

  r->surface_id_ = 100;
  r->cookies_ = std::string("k=v");
  r->media_url_params_ = std::make_unique<media::MediaUrlParams>(url_params);

  callback_called_ = false;
  r->init_cb_ =
      base::BindOnce(&OHOSCustomMediaPlayerRendererTest::OnInitCallback,
                     base::Unretained(this));

  ASSERT_NO_FATAL_FAILURE(r->TryCreateMediaPlayer());
  base::RunLoop().RunUntilIdle();

  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(callback_status_, media::PIPELINE_ERROR_INITIALIZATION_FAILED);
  EXPECT_EQ(r->media_player_, nullptr);
  EXPECT_FALSE(r->initialized_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, BuildMediaInfo1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->surface_id_ = 42;
  renderer_->is_audio_ = false;
  renderer_->source_infos_.clear();
  renderer_->source_infos_.push_back({"src1.mp4", "video/mp4"});
  renderer_->source_infos_.push_back({"src2.webm", "video/webm"});
  renderer_->video_rect_ = gfx::Rect(10, 20, 640, 360);
  renderer_->show_media_controls_ = false;
  renderer_->controls_list_.clear();
  renderer_->muted_ = true;
  renderer_->poster_url_.clear();
  renderer_->attributes_.clear();
  renderer_->cookies_ = std::string("");
  renderer_->referrer_.clear();

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  url_params.custom_media_url_params.media_source_type = 0;
  url_params.custom_media_url_params.preload_type = 0;

  renderer_->media_url_params_ =
      std::make_unique<media::MediaUrlParams>(url_params);

  auto info = renderer_->BuildMediaInfo("surf-42");

  EXPECT_EQ(info.embed_id, "42");
  EXPECT_EQ(info.media_type, MediaInfo::MediaType::VIDEO);
  ASSERT_EQ(info.media_src_list.size(), 2u);
  EXPECT_EQ(info.media_src_list[0].media_source, "src1.mp4");
  EXPECT_EQ(info.media_src_list[0].media_format, "video/mp4");
  EXPECT_EQ(info.media_src_list[1].media_source, "src2.webm");
  EXPECT_EQ(info.media_src_list[1].media_format, "video/webm");
  EXPECT_EQ(info.surface_info.id, "surf-42");
  EXPECT_EQ(info.surface_info.x, 10);
  EXPECT_EQ(info.surface_info.y, 20);
  EXPECT_EQ(info.surface_info.width, 640);
  EXPECT_EQ(info.surface_info.height, 360);
  EXPECT_FALSE(info.controls);
  EXPECT_TRUE(info.controlslist.empty());
  EXPECT_TRUE(info.muted);
  EXPECT_TRUE(info.poster_url.empty());
  EXPECT_EQ(info.preload, ConvertTo(0));
}

TEST_F(OHOSCustomMediaPlayerRendererTest, BuildMediaInfo2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->surface_id_ = 7;
  renderer_->is_audio_ = true;
  renderer_->source_infos_.clear();
  renderer_->source_infos_.push_back({"audio.m4a", "audio/mp4"});
  renderer_->video_rect_ = gfx::Rect(0, 0, 0, 0);
  renderer_->show_media_controls_ = true;
  renderer_->controls_list_ = {"play", "seek", "mute"};
  renderer_->muted_ = false;
  renderer_->poster_url_ = "https://example";
  renderer_->attributes_.clear();
  renderer_->attributes_.insert({"k1", "v1"});
  renderer_->attributes_.insert({"k2", "v2"});

  renderer_->cookies_ = std::string("sid=abc");
  renderer_->referrer_ = "https://example";

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, true, false);
  url_params.custom_media_url_params.media_source_type = 2;
  url_params.custom_media_url_params.preload_type = 2;

  renderer_->media_url_params_ =
      std::make_unique<media::MediaUrlParams>(url_params);

  auto info = renderer_->BuildMediaInfo("surf-7");

  EXPECT_EQ(info.embed_id, "7");
  EXPECT_EQ(info.media_type, MediaInfo::MediaType::AUDIO);
  ASSERT_EQ(info.media_src_list.size(), 1u);
  EXPECT_EQ(info.media_src_list[0].media_source, "audio.m4a");
  EXPECT_EQ(info.media_src_list[0].media_format, "audio/mp4");
  EXPECT_EQ(info.surface_info.id, "surf-7");
  EXPECT_EQ(info.controls, true);
  ASSERT_EQ(info.controlslist.size(), 3u);
  EXPECT_FALSE(info.muted);
  EXPECT_EQ(info.poster_url, "https://example");
}

TEST_F(OHOSCustomMediaPlayerRendererTest, BuildMediaInfo3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->surface_id_ = 42;
  renderer_->is_audio_ = false;
  renderer_->source_infos_.clear();
  renderer_->video_rect_ = gfx::Rect(10, 20, 640, 360);
  renderer_->show_media_controls_ = false;
  renderer_->controls_list_.clear();
  renderer_->muted_ = true;
  renderer_->poster_url_.clear();
  renderer_->attributes_.clear();
  renderer_->cookies_ = std::string("");
  renderer_->referrer_.clear();

  static media::MediaUrlParams url_params(
      GURL("https://example"), net::SiteForCookies(), url::Origin(),
      net::StorageAccessApiStatus::kNone, false, false);
  url_params.custom_media_url_params.media_source_type = 0;
  url_params.custom_media_url_params.preload_type = 0;

  renderer_->media_url_params_ =
      std::make_unique<media::MediaUrlParams>(url_params);

  auto info = renderer_->BuildMediaInfo("surf-42");

  EXPECT_EQ(info.embed_id, "42");
  EXPECT_EQ(info.media_type, MediaInfo::MediaType::VIDEO);
  EXPECT_TRUE(info.media_src_list.empty());
  EXPECT_EQ(info.surface_info.id, "surf-42");
  EXPECT_EQ(info.surface_info.x, 10);
  EXPECT_EQ(info.surface_info.y, 20);
  EXPECT_EQ(info.surface_info.width, 640);
  EXPECT_EQ(info.surface_info.height, 360);
  EXPECT_FALSE(info.controls);
  EXPECT_TRUE(info.controlslist.empty());
  EXPECT_TRUE(info.muted);
  EXPECT_TRUE(info.poster_url.empty());
  EXPECT_EQ(info.preload, ConvertTo(0));
}

TEST_F(OHOSCustomMediaPlayerRendererTest, StartPlayingFrom1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  renderer_->media_player_.reset();
  renderer_->is_playing_ = false;
  EXPECT_CALL(mock_client_,
              OnBufferingStateChange(media::BUFFERING_HAVE_ENOUGH,
                                     media::BUFFERING_CHANGE_REASON_UNKNOWN))
      .Times(1);
  ASSERT_NO_FATAL_FAILURE(renderer_->StartPlayingFrom(base::Seconds(5.0)));
  EXPECT_TRUE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, StartPlayingFrom2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, Seek(5.0)).Times(1);
  EXPECT_CALL(*mock_player, Play()).Times(1);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_playing_ = false;
  EXPECT_CALL(mock_client_,
              OnBufferingStateChange(media::BUFFERING_HAVE_ENOUGH,
                                     media::BUFFERING_CHANGE_REASON_UNKNOWN))
      .Times(1);
  ASSERT_NO_FATAL_FAILURE(renderer_->StartPlayingFrom(base::Seconds(5.0)));
  EXPECT_TRUE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, StartPlayingFrom3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, Seek(10.5)).Times(1);
  EXPECT_CALL(*mock_player, Play()).Times(0);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_playing_ = true;
  EXPECT_CALL(mock_client_,
              OnBufferingStateChange(media::BUFFERING_HAVE_ENOUGH,
                                     media::BUFFERING_CHANGE_REASON_UNKNOWN))
      .Times(1);
  ASSERT_NO_FATAL_FAILURE(renderer_->StartPlayingFrom(base::Seconds(10.5)));
  EXPECT_TRUE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetPlaybackRate1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  renderer_->media_player_.reset();
  renderer_->is_playing_ = true;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(1.5));
  EXPECT_TRUE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetPlaybackRate2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, Pause()).Times(1);
  EXPECT_CALL(*mock_player, SetPlaybackRate(_)).Times(0);
  EXPECT_CALL(*mock_player, Play()).Times(0);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_playing_ = true;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(0.0));
  EXPECT_FALSE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetPlaybackRate3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, Pause()).Times(0);
  EXPECT_CALL(*mock_player, SetPlaybackRate(_)).Times(0);
  EXPECT_CALL(*mock_player, Play()).Times(0);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_playing_ = false;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(0.0));
  EXPECT_FALSE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetPlaybackRate4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, SetPlaybackRate(1.5)).Times(1);
  EXPECT_CALL(*mock_player, Play()).Times(1);
  EXPECT_CALL(*mock_player, Pause()).Times(0);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_playing_ = false;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(1.5));
  EXPECT_TRUE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetPlaybackRate5) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, SetPlaybackRate(2.0)).Times(1);
  EXPECT_CALL(*mock_player, Play()).Times(0);
  EXPECT_CALL(*mock_player, Pause()).Times(0);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_playing_ = true;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRate(2.0));
  EXPECT_TRUE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, UpdateVolume1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  renderer_->media_player_.reset();
  renderer_->volume_ = 0.8f;
  renderer_->web_contents_muted_ = false;
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateVolume());
}

TEST_F(OHOSCustomMediaPlayerRendererTest, UpdateVolume2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();

  EXPECT_CALL(*mock_player, SetVolume(_)).Times(1);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->web_contents_muted_ = false;
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateVolume());
}

TEST_F(OHOSCustomMediaPlayerRendererTest, UpdateVolume3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();

  EXPECT_CALL(*mock_player, SetVolume(0.0)).Times(1);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->volume_ = 0.8f;
  renderer_->web_contents_muted_ = true;
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateVolume());
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnMediaDurationChanged1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  MockClientExtension fake;
  BindClientExtension(renderer_.get(), fake);
  renderer_->duration_ = base::Seconds(10);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnMediaDurationChanged(base::Seconds(0)));
  EXPECT_EQ(renderer_->duration_, media::kInfiniteDuration);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnMediaDurationChanged2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  MockClientExtension fake;
  BindClientExtension(renderer_.get(), fake);
  renderer_->duration_ = base::Seconds(5);
  base::TimeDelta new_duration = base::Seconds(15);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnMediaDurationChanged(new_duration));
  EXPECT_EQ(renderer_->duration_, new_duration);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnMediaDurationChanged3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  MockClientExtension fake;
  BindClientExtension(renderer_.get(), fake);
  base::TimeDelta same_duration = base::Seconds(10);
  renderer_->duration_ = same_duration;
  ASSERT_NO_FATAL_FAILURE(renderer_->OnMediaDurationChanged(same_duration));
  EXPECT_EQ(renderer_->duration_, same_duration);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnMediaDurationChanged4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  renderer_->duration_ = base::Seconds(5);
  base::TimeDelta new_duration = base::Seconds(15);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnMediaDurationChanged(new_duration));
  EXPECT_EQ(renderer_->duration_, new_duration);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnError1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  renderer_->has_error_ = false;
  EXPECT_CALL(mock_client_, OnError(::testing::_)).Times(0);

  ASSERT_NO_FATAL_FAILURE(renderer_->OnError(
      media::OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_INVALID_CODE));
  EXPECT_FALSE(renderer_->has_error_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnError2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  renderer_->has_error_ = false;

  EXPECT_CALL(mock_client_, OnError(::testing::_)).Times(1);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnError(123));
  EXPECT_TRUE(renderer_->has_error_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnVideoSizeChanged1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  gfx::Size same_size = gfx::Size(640, 360);
  renderer_->video_size_ = same_size;

  ASSERT_NO_FATAL_FAILURE(renderer_->OnVideoSizeChanged(640, 360));
  EXPECT_EQ(renderer_->video_size_, same_size);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnVideoSizeChanged2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  renderer_->video_size_ = gfx::Size(320, 180);

  ASSERT_NO_FATAL_FAILURE(renderer_->OnVideoSizeChanged(1280, 720));
  EXPECT_EQ(renderer_->video_size_, gfx::Size(1280, 720));
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnVideoSizeChanged3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  renderer_->video_size_ = gfx::Size(320, 180);
  MockClientExtension mock_client_extension;
  BindClientExtension(renderer_.get(), mock_client_extension);
  ASSERT_NO_FATAL_FAILURE(renderer_->OnVideoSizeChanged(1280, 720));
  EXPECT_EQ(renderer_->video_size_, gfx::Size(1280, 720));
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetMediaPlayerState1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  renderer_->media_player_.reset();
  renderer_->is_media_player_suspend_ = false;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetMediaPlayerState(true, 1));
  EXPECT_FALSE(renderer_->is_media_player_suspend_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetMediaPlayerState2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, SuspendMediaPlayer(2)).Times(1);
  EXPECT_CALL(*mock_player, ResumeMediaPlayer()).Times(0);
  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_media_player_suspend_ = false;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetMediaPlayerState(true, 2));
  EXPECT_TRUE(renderer_->is_media_player_suspend_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetMediaPlayerState3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, SuspendMediaPlayer(_)).Times(0);
  EXPECT_CALL(*mock_player, ResumeMediaPlayer()).Times(0);

  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_media_player_suspend_ = true;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetMediaPlayerState(true, 1));
  EXPECT_TRUE(renderer_->is_media_player_suspend_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetMediaPlayerState4) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, SuspendMediaPlayer(_)).Times(0);
  EXPECT_CALL(*mock_player, ResumeMediaPlayer()).Times(1);

  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_media_player_suspend_ = true;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetMediaPlayerState(false, 0));
  EXPECT_FALSE(renderer_->is_media_player_suspend_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetMediaPlayerState5) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, SuspendMediaPlayer(_)).Times(0);
  EXPECT_CALL(*mock_player, ResumeMediaPlayer()).Times(0);

  renderer_->media_player_ = std::move(mock_player);
  renderer_->is_media_player_suspend_ = false;
  ASSERT_NO_FATAL_FAILURE(renderer_->SetMediaPlayerState(false, 0));
  EXPECT_FALSE(renderer_->is_media_player_suspend_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetAttributes1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  base::flat_map<std::string, std::string> test_attributes;
  test_attributes["key1"] = "value1";
  test_attributes["key2"] = "value2";

  ASSERT_NO_FATAL_FAILURE(renderer_->SetAttributes(std::move(test_attributes)));
  EXPECT_EQ(renderer_->attributes_.size(), 2u);
  EXPECT_EQ(renderer_->attributes_["key1"], "value1");
  EXPECT_EQ(renderer_->attributes_["key2"], "value2");
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetAttributes2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  renderer_->attributes_["existing"] = "value";
  base::flat_map<std::string, std::string> empty_attributes;
  ASSERT_NO_FATAL_FAILURE(
      renderer_->SetAttributes(std::move(empty_attributes)));
  EXPECT_EQ(renderer_->attributes_.size(), 0u);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetPlaybackRateWithReason1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, SetPlaybackRate(1.5)).Times(1);
  renderer_->media_player_ = std::move(mock_player);
  ASSERT_NO_FATAL_FAILURE(
      renderer_->SetPlaybackRateWithReason(1.5, media::ActionReason::kNormal));
}

TEST_F(OHOSCustomMediaPlayerRendererTest, SetPlaybackRateWithReason2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  auto mock_player = std::make_unique<MockCustomMediaPlayer>();
  EXPECT_CALL(*mock_player, SetPlaybackRate(_)).Times(0);
  renderer_->media_player_ = std::move(mock_player);
  ASSERT_NO_FATAL_FAILURE(renderer_->SetPlaybackRateWithReason(
      1.5, media::ActionReason::kCustomRenderer));
}

TEST_F(OHOSCustomMediaPlayerRendererTest, UpdateClientExtensionMethods1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdatePlaybackStatus(1));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateVolume(0.8));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateMuted(true));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdatePlaybackRate(2.0));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateBufferedEndTime(10.5));
  EXPECT_TRUE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, UpdateClientExtensionMethods2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());

  MockClientExtension mock_client_extension;
  BindClientExtension(renderer_.get(), mock_client_extension);

  ASSERT_NO_FATAL_FAILURE(renderer_->UpdatePlaybackStatus(1));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateVolume(0.8));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateMuted(true));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdatePlaybackRate(2.0));
  ASSERT_NO_FATAL_FAILURE(renderer_->UpdateBufferedEndTime(10.5));
  EXPECT_TRUE(renderer_->is_playing_);
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnFullscreenChanged1) {
  const int process_id = main_rfh()->GetProcess()->GetID();
  const int routing_id = main_rfh()->GetRoutingID();
  mojo::PendingReceiver<media::mojom::MediaPlayerRendererExtension>
      renderer_receiver;
  mojo::PendingRemote<media::mojom::CustomMediaPlayerRendererClientExtension>
      client_remote;
  auto test_renderer = std::make_unique<OHOSCustomMediaPlayerRenderer>(
      process_id, routing_id, 1, nullptr, std::move(renderer_receiver),
      std::move(client_remote));

  test_renderer->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                            media::VideoDecoderChangedCB(), base::DoNothing());
  ASSERT_NO_FATAL_FAILURE(test_renderer->OnFullscreenChanged(true));
  ASSERT_NO_FATAL_FAILURE(test_renderer->OnFullscreenChanged(false));
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnFullscreenChanged2) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  ASSERT_NO_FATAL_FAILURE(renderer_->OnFullscreenChanged(true));
}

TEST_F(OHOSCustomMediaPlayerRendererTest, OnFullscreenChanged3) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);
  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  ASSERT_NO_FATAL_FAILURE(renderer_->OnFullscreenChanged(false));
}

TEST_F(OHOSCustomMediaPlayerRendererTest,
       CustomMediaPlayerListenerImplMethods1) {
  CreateRenderer();
  ASSERT_NE(renderer_, nullptr);

  renderer_->Initialize(nullptr, &mock_client_, media::RequestSurfaceCB(),
                        media::VideoDecoderChangedCB(), base::DoNothing());
  CustomMediaPlayerListenerImpl listener(nullptr);
  ASSERT_NO_FATAL_FAILURE(listener.OnStatusChanged(1));
  ASSERT_NO_FATAL_FAILURE(listener.OnVolumeChanged(0.8));
  ASSERT_NO_FATAL_FAILURE(listener.OnMutedChanged(true));
  ASSERT_NO_FATAL_FAILURE(listener.OnPlaybackRateChanged(2.0));
  ASSERT_NO_FATAL_FAILURE(listener.OnDurationChanged(10.5));
  ASSERT_NO_FATAL_FAILURE(listener.OnTimeUpdate(5.0));
  ASSERT_NO_FATAL_FAILURE(listener.OnBufferedEndTimeChanged(8.0));
  ASSERT_NO_FATAL_FAILURE(listener.OnEnded());
  ASSERT_NO_FATAL_FAILURE(listener.OnReadyStateChanged(static_cast<uint32_t>(
      CustomMediaPlayerListenerImpl::ReadyState::HAVE_NOTHING)));
  ASSERT_NO_FATAL_FAILURE(listener.OnFullscreenChanged(true));
  ASSERT_NO_FATAL_FAILURE(listener.OnError(123, "test error"));
  ASSERT_NO_FATAL_FAILURE(listener.OnVideoSizeChanged(1280, 720));
}

TEST_F(OHOSCustomMediaPlayerRendererTest,
       CustomMediaPlayerListenerImplMethods2) {
  auto mock_renderer = std::make_unique<MockOHOSCustomMediaPlayerRenderer>();
  base::WeakPtr<OHOSCustomMediaPlayerRenderer> weak_renderer =
      mock_renderer->GetWeakPtr();
  CustomMediaPlayerListenerImpl listener(weak_renderer);
  EXPECT_CALL(*mock_renderer, OnMediaDurationChanged(::testing::_)).Times(1);
  EXPECT_CALL(*mock_renderer, OnPlaybackComplete()).Times(1);
  EXPECT_CALL(*mock_renderer, OnError(::testing::_)).Times(1);
  EXPECT_CALL(*mock_renderer, OnVideoSizeChanged(::testing::_, ::testing::_))
      .Times(1);

  ASSERT_NO_FATAL_FAILURE(listener.OnStatusChanged(1));
  ASSERT_NO_FATAL_FAILURE(listener.OnVolumeChanged(0.8));
  ASSERT_NO_FATAL_FAILURE(listener.OnMutedChanged(true));
  ASSERT_NO_FATAL_FAILURE(listener.OnPlaybackRateChanged(2.0));
  ASSERT_NO_FATAL_FAILURE(listener.OnDurationChanged(10.5));
  ASSERT_NO_FATAL_FAILURE(listener.OnTimeUpdate(5.0));
  ASSERT_NO_FATAL_FAILURE(listener.OnBufferedEndTimeChanged(8.0));
  ASSERT_NO_FATAL_FAILURE(listener.OnEnded());
  ASSERT_NO_FATAL_FAILURE(listener.OnFullscreenChanged(true));
  ASSERT_NO_FATAL_FAILURE(listener.OnError(123, "test error"));
  ASSERT_NO_FATAL_FAILURE(listener.OnVideoSizeChanged(1280, 720));
}

}  // namespace content