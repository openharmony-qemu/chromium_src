// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_ext.h"

#include "base/command_line.h"
#include "content/browser/browser_main_loop.h"
#include "content/browser/media/media_web_contents_observer.h"
#include "content/public/browser/custom_media_player_listener.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/common/main_function_params.h"
#include "content/public/test/test_utils.h"
#include "content/test/test_web_contents.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string.h>
#include <memory>

namespace content {
class MockCustomMediaPlayerListener : public CustomMediaPlayerListener {
 public:
  MockCustomMediaPlayerListener() = default;
  ~MockCustomMediaPlayerListener() = default;
  MOCK_METHOD(void, OnStatusChanged, (uint32_t status), (override));
  MOCK_METHOD(void, OnVolumeChanged, (double volume), (override));
  MOCK_METHOD(void, OnMutedChanged, (bool muted), (override));
  MOCK_METHOD(void, OnPlaybackRateChanged, (double playback_rate), (override));
  MOCK_METHOD(void, OnDurationChanged, (double duration), (override));
  MOCK_METHOD(void, OnTimeUpdate, (double current_time), (override));
  MOCK_METHOD(void,
              OnBufferedEndTimeChanged,
              (double buffered_time),
              (override));
  MOCK_METHOD(void, OnEnded, (), (override));
  MOCK_METHOD(void, OnNetworkStateChanged, (uint32_t state), (override));
  MOCK_METHOD(void, OnReadyStateChanged, (uint32_t state), (override));
  MOCK_METHOD(void, OnFullscreenChanged, (bool fullscreen), (override));
  MOCK_METHOD(void, OnSeeking, (), (override));
  MOCK_METHOD(void, OnSeekFinished, (), (override));
  MOCK_METHOD(void,
              OnError,
              (uint32_t error_code, const std::string& error_msg),
              (override));
  MOCK_METHOD(void, OnVideoSizeChanged, (int width, int height), (override));
};

class TestWebContentsImplExt : public TestWebContents {
 public:
  TestWebContentsImplExt(BrowserContext* browser_context)
      : TestWebContents(browser_context) {}

  virtual ~TestWebContentsImplExt() {}

  void SetRenderManagerForTesting(RenderFrameHostManager* manager) {
    test_manager_ = manager;
  }

  RenderFrameHostManager* GetRenderManager() {
    if (test_manager_) {
      return test_manager_;
    }
    return WebContentsImpl::GetRenderManager();
  }

 private:
  RenderFrameHostManager* test_manager_ = nullptr;
};

class WebContentsImplExtTest : public RenderViewHostImplTestHarness {
 public:
  void SetUp() override {
    RenderViewHostImplTestHarness::SetUp();

    if (IsIsolatedOriginRequiredToGuaranteeDedicatedProcess()) {
      ChildProcessSecurityPolicyImpl::GetInstance()->AddFutureIsolatedOrigins(
          {url::Origin::Create(isolated_cross_site_url())},
          ChildProcessSecurityPolicy::IsolatedOriginSource::TEST,
          browser_context());
      SetContents(CreateTestWebContents());
    }
  }

  TestWebContentsImplExt* ExtendContent() {
    return static_cast<TestWebContentsImplExt*>(web_contents());
  }

  bool has_audio_wake_lock() {
    return ExtendContent()
        ->media_web_contents_observer()
        ->has_audio_wake_lock_for_testing();
  }

  GURL isolated_cross_site_url() const {
    return GURL("http://isolated-cross-site.com");
  }

 private:
#if BUILDFLAG(IS_CHROMEOS_LACROS)
  chromeos::ScopedLacrosServiceTestHelper scoped_lacros_service_test_helper_;
#endif
};

TEST_F(WebContentsImplExtTest, GetNWebId001) {
  auto web_id = ExtendContent()->GetNWebId();
  EXPECT_EQ(web_id, 0);
}

TEST_F(WebContentsImplExtTest, SetNWebId001) {
  int id = 0;
  ExtendContent()->SetNWebId(id);
  auto web_id = ExtendContent()->GetNWebId();
  EXPECT_EQ(web_id, 0);
}

TEST_F(WebContentsImplExtTest, AddMediaPlayerAudibleCount001) {
  ExtendContent()->AddMediaPlayerAudibleCount();
  auto flag = ExtendContent()->GetMediaPlayerCurrentAudible();
  EXPECT_TRUE(flag);
}

TEST_F(WebContentsImplExtTest, DelMediaPlayerAudibleCount001) {
  ExtendContent()->AddMediaPlayerAudibleCount();
  ExtendContent()->DelMediaPlayerAudibleCount();
  auto flag = ExtendContent()->GetMediaPlayerCurrentAudible();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, CreateWebMessagePorts001) {
  std::vector<blink::WebMessagePort> ports;
  ExtendContent()->CreateWebMessagePorts(ports);
  auto size = ports.size();
  EXPECT_EQ(size, 2);
}

TEST_F(WebContentsImplExtTest, PostWebMessage001) {
  std::string message = "";
  std::vector<blink::WebMessagePort> ports;
  std::string target_uri = "abc";
  ExtendContent()->PostWebMessage(message, ports, target_uri);
  EXPECT_EQ(ports.size(), 0);
}

TEST_F(WebContentsImplExtTest, PostWebMessage002) {
  std::string message = "";
  std::vector<blink::WebMessagePort> ports;
  std::string target_uri = "*";
  ExtendContent()->PostWebMessage(message, ports, target_uri);
  EXPECT_EQ(ports.size(), 0);
}

TEST_F(WebContentsImplExtTest, OpenDateTimeChooser001) {
  ExtendContent()->OpenDateTimeChooser();
}

TEST_F(WebContentsImplExtTest, CloseDateTimeChooser001) {
  ExtendContent()->CloseDateTimeChooser();
}

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
TEST_F(WebContentsImplExtTest, SetForceEnableZoom001) {
  ExtendContent()->SetForceEnableZoom(false);
  auto flag = ExtendContent()->GetForceEnableZoom();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, SetForceEnableZoom002) {
  ExtendContent()->SetForceEnableZoom(true);
  auto flag = ExtendContent()->GetForceEnableZoom();
  EXPECT_TRUE(flag);
}

TEST_F(WebContentsImplExtTest, EnableAdsBlock001) {
  ExtendContent()->EnableAdsBlock(false);
  auto flag = ExtendContent()->GetForceEnableZoom();
  EXPECT_FALSE(flag);
  DeleteContents();
}

TEST_F(WebContentsImplExtTest, EnableAdsBlock002) {
  ExtendContent()->EnableAdsBlock(true);
  auto flag = ExtendContent()->GetForceEnableZoom();
  EXPECT_FALSE(flag);
}
#endif

TEST_F(WebContentsImplExtTest, IsAdsBlockEnabled001) {
  auto flag = ExtendContent()->IsAdsBlockEnabled();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, IsAdsBlockEnabledForCurPage001) {
  auto flag = ExtendContent()->IsAdsBlockEnabledForCurPage();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, TrigAdBlockEnabledForSiteFromUi001) {
  std::string main_frame_url = "";
  ExtendContent()->TrigAdBlockEnabledForSiteFromUi(main_frame_url);
}

TEST_F(WebContentsImplExtTest, OnAdsBlocked001) {
  std::string main_frame_url = "";
  std::map<std::string, int32_t> subresource_blocked;
  bool is_site_first_report = false;
  ExtendContent()->OnAdsBlocked(main_frame_url, subresource_blocked,
                                is_site_first_report);
}

TEST_F(WebContentsImplExtTest, GetAdblockEnabledForSite001) {
  auto flag = ExtendContent()->GetAdblockEnabledForSite();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, SetAdBlockEnabledForSite001) {
  bool is_adblock_enabled = false;
  int main_frame_tree_node_id = 0;
  ExtendContent()->SetAdBlockEnabledForSite(is_adblock_enabled,
                                            main_frame_tree_node_id);
}

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
TEST_F(WebContentsImplExtTest, NotifyContextMenuWillShow001) {
  ExtendContent()->NotifyContextMenuWillShow();
  bool flag = ExtendContent()->IsShowingContextMenu();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, ShowFreeCopyMenu001) {
  ExtendContent()->ShowFreeCopyMenu();
}
#endif

TEST_F(WebContentsImplExtTest, GetTargetFramesIncludingPending001) {
  int routing_id = 0;
  auto ptr = ExtendContent()->GetTargetFramesIncludingPending(routing_id);
  EXPECT_EQ(ptr, nullptr);
}

TEST_F(WebContentsImplExtTest, SetHtmlPlayEnabled001) {
  bool enabled = false;
  ExtendContent()->SetHtmlPlayEnabled(enabled);
  auto flag = ExtendContent()->IsHtmlPlayEnabled();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, OnNativeEmbedStatusUpdated01) {
  NativeEmbedInfo::TagState state = NativeEmbedInfo::TagState::TAG_STATE_CREATE;
  NativeEmbedInfo native_embed_info;
  ExtendContent()->OnNativeEmbedStatusUpdate(native_embed_info, state);
}

TEST_F(WebContentsImplExtTest, OnNativeEmbedFirstFramePaint001) {
  int32_t native_embed_id = 0;
  std::string embed_id_attribute = "abc";
  ExtendContent()->OnNativeEmbedFirstFramePaint(native_embed_id,
                                                embed_id_attribute);
}

TEST_F(WebContentsImplExtTest, OnRenderFrameHostEnterBackForwardCache001) {
  int child_id = 0;
  int frame_routing_id = 0;
  GlobalRenderFrameHostId id(child_id, frame_routing_id);
  ExtendContent()->OnRenderFrameHostEnterBackForwardCache(id);
}

TEST_F(WebContentsImplExtTest, OnRenderFrameHostLeaveBackForwardCache001) {
  int child_id = 0;
  int frame_routing_id = 0;
  GlobalRenderFrameHostId id(child_id, frame_routing_id);
  ExtendContent()->OnRenderFrameHostLeaveBackForwardCache(id);
}

TEST_F(WebContentsImplExtTest, SetWakeLockHandler001) {
  int32_t window_id = 0;
  SetKeepScreenOn handler;
  ExtendContent()->SetWakeLockHandler(window_id, handler);
}

TEST_F(WebContentsImplExtTest, MouseSelectMenuShow001) {
  bool show = false;
  ExtendContent()->MouseSelectMenuShow(show);
}

TEST_F(WebContentsImplExtTest, ChangeVisibilityOfQuickMenu001) {
  ExtendContent()->ChangeVisibilityOfQuickMenu();
}

TEST_F(WebContentsImplExtTest, CloseImageOverlaySelection001) {
  ExtendContent()->CloseImageOverlaySelection();
}

TEST_F(WebContentsImplExtTest, OnOverlayZoomChanged001) {
  ExtendContent()->OnOverlayZoomChanged();
}

TEST_F(WebContentsImplExtTest, ExtensionGetTabId001) {
  auto rst = ExtendContent()->ExtensionGetTabId();
  EXPECT_EQ(rst, -1);
}

TEST_F(WebContentsImplExtTest, OnFormEditingStateChanged001) {
  uint64_t form_id = 0;
  bool did_submit = false;
  ExtendContent()->OnFormEditingStateChanged(form_id, did_submit);
}

TEST_F(WebContentsImplExtTest, MediaPlayerGone001) {
  WebContentsObserver::MediaPlayerInfo info(false, false);
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->MediaPlayerGone(info, media_player_id);
}

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
TEST_F(WebContentsImplExtTest, UpdateBrowserControlsHeight001) {
  int height = 0;
  bool animate = false;
  ExtendContent()->UpdateBrowserControlsHeight(height, animate);
}
#endif

TEST_F(WebContentsImplExtTest, ClearContextMenu001) {
  ExtendContent()->ClearContextMenu();
}

TEST_F(WebContentsImplExtTest, EnableSafeBrowsingDetection001) {
  bool enable = true;
  bool strictMode = false;
  ExtendContent()->EnableSafeBrowsingDetection(enable, strictMode);
  auto flag = ExtendContent()->IsSafeBrowsingDetectionConfig();
  EXPECT_TRUE(flag);
}

TEST_F(WebContentsImplExtTest, EnableSafeBrowsingDetection002) {
  bool enable = false;
  bool strictMode = false;
  ExtendContent()->EnableSafeBrowsingDetection(enable, strictMode);
  auto flag = ExtendContent()->IsSafeBrowsingDetectionDisabled();
  EXPECT_TRUE(flag);
}

TEST_F(WebContentsImplExtTest, EnableSafeBrowsingDetection003) {
  bool enable = false;
  bool strictMode = true;
  ExtendContent()->EnableSafeBrowsingDetection(enable, strictMode);
  auto flag = ExtendContent()->IsSafeBrowsingDetectionStrict();
  EXPECT_TRUE(flag);
}

TEST_F(WebContentsImplExtTest, CreateCustomMediaPlayer001) {
  std::unique_ptr<MockCustomMediaPlayerListener> listener =
      std::make_unique<MockCustomMediaPlayerListener>();
  MediaInfo media_info;
  auto rst =
      ExtendContent()->CreateCustomMediaPlayer(std::move(listener), media_info);
  EXPECT_EQ(rst, nullptr);
}

TEST_F(WebContentsImplExtTest, AddCustomMediaPlayer001) {
  std::unique_ptr<CustomMediaPlayer> player =
      std::make_unique<CustomMediaPlayer>();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->AddCustomMediaPlayer(media_player_id, player.get());
}

TEST_F(WebContentsImplExtTest, RemoveCustomMediaPlayer001) {
  std::unique_ptr<CustomMediaPlayer> player =
      std::make_unique<CustomMediaPlayer>();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->RemoveCustomMediaPlayer(media_player_id, player.get());
}

TEST_F(WebContentsImplExtTest, RemoveCustomMediaPlayer002) {
  std::unique_ptr<CustomMediaPlayer> player =
      std::make_unique<CustomMediaPlayer>();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->AddCustomMediaPlayer(media_player_id, player.get());
  ExtendContent()->RemoveCustomMediaPlayer(media_player_id, player.get());
}

TEST_F(WebContentsImplExtTest, UpdateLayerRect001) {
  gfx::Rect rect(100, 100);
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->UpdateLayerRect(media_player_id, rect);
}

TEST_F(WebContentsImplExtTest, UpdateLayerRect002) {
  gfx::Rect rect(100, 100);
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  std::unique_ptr<CustomMediaPlayer> player =
      std::make_unique<CustomMediaPlayer>();
  ExtendContent()->AddCustomMediaPlayer(media_player_id, player.get());
  ExtendContent()->UpdateLayerRect(media_player_id, rect);
}

TEST_F(WebContentsImplExtTest, FullScreenChanged001) {
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  bool is_fullscreen = false;
  ExtendContent()->FullScreenChanged(media_player_id, is_fullscreen);
}

TEST_F(WebContentsImplExtTest, FullScreenChanged002) {
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);

  std::unique_ptr<CustomMediaPlayer> player =
      std::make_unique<CustomMediaPlayer>();
  ExtendContent()->AddCustomMediaPlayer(media_player_id, player.get());

  bool is_fullscreen = false;
  ExtendContent()->FullScreenChanged(media_player_id, is_fullscreen);
}

TEST_F(WebContentsImplExtTest, FullScreenChanged003) {
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);

  std::unique_ptr<CustomMediaPlayer> player =
      std::make_unique<CustomMediaPlayer>();
  ExtendContent()->AddCustomMediaPlayer(media_player_id, player.get());

  bool is_fullscreen = true;
  ExtendContent()->FullScreenChanged(media_player_id, is_fullscreen);
}

TEST_F(WebContentsImplExtTest, RequestEnterFullscreen001) {
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->RequestEnterFullscreen(media_player_id);
  auto rst = ExtendContent()->media_web_contents_observer();
  EXPECT_NE(rst, nullptr);
}

TEST_F(WebContentsImplExtTest, RequestExitFullscreen001) {
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->RequestExitFullscreen(media_player_id);
  auto rst = ExtendContent()->media_web_contents_observer();
  EXPECT_NE(rst, nullptr);
}

TEST_F(WebContentsImplExtTest, SharedRenderProcessToken001) {
  auto str = ExtendContent()->SharedRenderProcessToken();
  EXPECT_EQ(str, "");
}

TEST_F(WebContentsImplExtTest, MediaDestroyed001) {
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->MediaDestroyed(media_player_id);
}

TEST_F(WebContentsImplExtTest, EnableVideoAssistant001) {
  bool enable = false;
  ExtendContent()->EnableVideoAssistant(enable);
}

TEST_F(WebContentsImplExtTest, ExecuteVideoAssistantFunction001) {
  std::string cmd_id = "";
  ExtendContent()->ExecuteVideoAssistantFunction(cmd_id);
}

TEST_F(WebContentsImplExtTest, OnShowToast001) {
  double duration = 0.0;
  std::string toast = "";
  ExtendContent()->OnShowToast(duration, toast);
}

TEST_F(WebContentsImplExtTest, OnReportStatisticLog001) {
  std::string toast = "";
  ExtendContent()->OnReportStatisticLog(toast);
}

TEST_F(WebContentsImplExtTest, CustomWebMediaPlayer001) {
  bool enable = false;
  ExtendContent()->CustomWebMediaPlayer(enable);
}

TEST_F(WebContentsImplExtTest, CustomWebMediaPlayer002) {
  bool enable = true;
  ExtendContent()->CustomWebMediaPlayer(enable);
}

TEST_F(WebContentsImplExtTest, PopluateVideoAssistantConfig001) {
  media::mojom::VideoAssistantConfigPtr config;
  ExtendContent()->PopluateVideoAssistantConfig(config);
}

TEST_F(WebContentsImplExtTest, OnVideoPlaying001) {
  media::mojom::VideoAttributesForVASTPtr video_attributes =
      media::mojom::VideoAttributesForVAST::New();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->OnVideoPlaying(std::move(video_attributes), media_player_id);
}

TEST_F(WebContentsImplExtTest, OnUpdateVideoAttributes001) {
  media::mojom::VideoAttributesForVASTPtr video_attributes =
      media::mojom::VideoAttributesForVAST::New();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->OnUpdateVideoAttributes(std::move(video_attributes), media_player_id);
}

TEST_F(WebContentsImplExtTest, OnVideoDestroyed001) {
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->OnVideoDestroyed(media_player_id);
}

TEST_F(WebContentsImplExtTest, OnFullScreenOverlayEnter001) {
  media::mojom::MediaInfoForVASTPtr media_info =
      media::mojom::MediaInfoForVAST::New();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->OnFullScreenOverlayEnter(std::move(media_info), media_player_id);
}

TEST_F(WebContentsImplExtTest, DelAllVideoSurfaces001) {
  ExtendContent()->DelAllVideoSurfaces();
}

TEST_F(WebContentsImplExtTest, ReportVideoDecoderName001) {
  std::string decoder_name = "";
  ExtendContent()->ReportVideoDecoderName(decoder_name);
}

TEST_F(WebContentsImplExtTest, ShowAutofillPopup001) {
  gfx::RectF element_bounds;
  bool is_rtl = false;
  std::vector<autofill::Suggestion> suggestions;
  bool is_password_popup_type = false;
  ExtendContent()->ShowAutofillPopup(element_bounds, is_rtl, suggestions,
                                     is_password_popup_type);
}

TEST_F(WebContentsImplExtTest, HideAutofillPopup001) {
  ExtendContent()->HideAutofillPopup();
}

TEST_F(WebContentsImplExtTest, OnShareFile001) {
  std::string file_path = "";
  std::string utd_type_id = "";
  ExtendContent()->OnShareFile(file_path, utd_type_id);
}

TEST_F(WebContentsImplExtTest, SelectRangeV2001) {
  gfx::Point position;
  bool is_base = false;
  ExtendContent()->SelectRangeV2(position, is_base);
}

TEST_F(WebContentsImplExtTest, SetCustomUA001) {
  std::string custom_user_agent = "abc";
  ExtendContent()->SetCustomUA(custom_user_agent);
  auto str = ExtendContent()->GetCustomUA();
  EXPECT_EQ(str, "abc");
}

TEST_F(WebContentsImplExtTest, OneShotMediaPlayerStopped001) {
  ExtendContent()->OneShotMediaPlayerStopped();
}

TEST_F(WebContentsImplExtTest, OnBeforeUnloadFired001) {
  bool proceed = false;
  ExtendContent()->OnBeforeUnloadFired(proceed);
}

TEST_F(WebContentsImplExtTest, StopScreenCapture001) {
  int32_t nweb_id = 0;
  std::string session_id = "";
  ExtendContent()->StopScreenCapture(nweb_id, session_id);
}

TEST_F(WebContentsImplExtTest, DisableSessionReuse001) {
  ExtendContent()->DisableSessionReuse();
}

TEST_F(WebContentsImplExtTest, EnterFullscreenMode001) {
  int render_process_id = 0;
  int render_frame_id = 0;
  auto ptr_host = RenderFrameHost::FromID(render_process_id, render_frame_id);
  auto ptr_host_impl = RenderFrameHostImpl::From(ptr_host);
  blink::mojom::FullscreenOptions options;
  ExtendContent()->EnterFullscreenMode(ptr_host_impl, options);
}

TEST_F(WebContentsImplExtTest, GetRenderViewHost001) {
  auto impl = ExtendContent()->GetRenderViewHost();
  EXPECT_NE(impl, nullptr);
}

TEST_F(WebContentsImplExtTest, UpdateBrowserControlsState001) {
  cc::BrowserControlsState constraints = cc::BrowserControlsState::kShown;
  cc::BrowserControlsState current = cc::BrowserControlsState::kShown;
  bool animate = false;
  std::optional<cc::BrowserControlsOffsetTagsInfo> offset_tags_info;
  ExtendContent() -> UpdateBrowserControlsState(constraints, current, animate,
                                                offset_tags_info);
}

TEST_F(WebContentsImplExtTest, GetMediaPlayerId001) {
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  bool status = false;
  auto id = ExtendContent()->GetMediaPlayerId(delegate_id, child_id,
                                              frame_routing_id, status);
  EXPECT_EQ(id.delegate_id, 0);
}

TEST_F(WebContentsImplExtTest, OnPip001) {
  int status = 0;
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int width = 0;
  int height = 0;
  ExtendContent()->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                         height);
}

TEST_F(WebContentsImplExtTest, OnPipEvent001) {
  int event = 0;
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, SetUpdateSurface001) {
  bool state = false;
  ExtendContent()->SetUpdateSurface(state);
  auto flag = ExtendContent()->IsUpdateSurface();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, OnPdfScrollAtBottom001) {
  std::string url = "abc";
  ExtendContent()->OnPdfScrollAtBottom(url);
}

TEST_F(WebContentsImplExtTest, OnPdfLoadEvent001) {
  int32_t result = 0;
  std::string url = "abc";
  ExtendContent()->OnPdfLoadEvent(result, url);
}

TEST_F(WebContentsImplExtTest, SetMediaResumeFromBFCachePage001) {
  bool resume = true;
  ExtendContent()->SetMediaResumeFromBFCachePage(resume);
}

TEST_F(WebContentsImplExtTest, SetMediaResumeFromBFCachePage002) {
  bool resume = false;
  ExtendContent()->SetMediaResumeFromBFCachePage(resume);
}

#if BUILDFLAG(ARKWEB_BGTASK)
TEST_F(WebContentsImplExtTest, OnBrowserForeground001) {
  ExtendContent()->OnBrowserForeground();
}

TEST_F(WebContentsImplExtTest, OnBrowserBackground001) {
  ExtendContent()->OnBrowserBackground();
}
#endif

TEST_F(WebContentsImplExtTest, SetScreenCapturePickerShow001) {
  ExtendContent()->SetScreenCapturePickerShow();
}
}  // namespace content