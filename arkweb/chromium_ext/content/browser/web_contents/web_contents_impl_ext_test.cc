// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_ext.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string.h>

#include <memory>

#include "base/command_line.h"
#include "content/browser/browser_main_loop.h"
#include "content/browser/media/media_web_contents_observer.h"
#include "content/browser/media/ohos/native_web_contents_observer.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/browser/renderer_host/render_frame_host_manager.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/wake_lock/wake_lock_context_host.h"
#include "content/browser/web_contents/web_contents_view.h"
#include "content/public/browser/custom_media_player_listener.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/common/content_switches_ext.h"
#include "content/public/common/main_function_params.h"
#include "content/public/test/test_utils.h"
#include "content/test/test_render_frame_host.h"
#include "content/test/test_web_contents.h"
#include "third_party/blink/public/mojom/input/input_handler.mojom.h"

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

class MockWebContentsDelegateExtended : public WebContentsDelegate {
 public:
  MockWebContentsDelegateExtended() = default;
  ~MockWebContentsDelegateExtended() = default;

  MOCK_METHOD(bool,
              TrigAdBlockEnabledForSiteFromUi,
              (const std::string& main_frame_url, int main_frame_tree_node_id),
              ());

  MOCK_METHOD(std::unique_ptr<VideoAssistant>,
              CreateVideoAssistant,
              (),
              (override));
};

class MockRenderViewHostDelegateView : public RenderViewHostDelegateView {
 public:
  MockRenderViewHostDelegateView() = default;

  MockRenderViewHostDelegateView(const MockRenderViewHostDelegateView&) =
      delete;
  MockRenderViewHostDelegateView& operator=(
      const MockRenderViewHostDelegateView&) = delete;

  ~MockRenderViewHostDelegateView() override = default;

  int start_dragging_count() const { return start_dragging_count_; }

  void StartDragging(const DropData& drop_data,
                     const url::Origin& source_origin,
                     blink::DragOperationsMask allowed_ops,
                     const gfx::ImageSkia& image,
                     const gfx::Vector2d& cursor_offset,
                     const gfx::Rect& drag_obj_rect,
                     const blink::mojom::DragEventSourceInfo& event_info,
                     RenderWidgetHostImpl* source_rwh) override {
    ++start_dragging_count_;
  }

 private:
  int start_dragging_count_ = 0;
};

class MockWebContentsView : public WebContentsView {
 public:
  MockWebContentsView() = default;
  ~MockWebContentsView() override = default;

  MOCK_METHOD(gfx::NativeView, GetNativeView, (), (const, override));
  MOCK_METHOD(gfx::NativeView, GetContentNativeView, (), (const, override));
  MOCK_METHOD(gfx::NativeWindow,
              GetTopLevelNativeWindow,
              (),
              (const, override));
  MOCK_METHOD(gfx::Rect, GetContainerBounds, (), (const, override));
  MOCK_METHOD(gfx::Rect, GetViewBounds, (), (const, override));
  MOCK_METHOD(void, Focus, (), (override));
  MOCK_METHOD(void, SetInitialFocus, (), (override));
  MOCK_METHOD(void, StoreFocus, (), (override));
  MOCK_METHOD(void, RestoreFocus, (), (override));
  MOCK_METHOD(void, FocusThroughTabTraversal, (bool reverse), (override));
  MOCK_METHOD(DropData*, GetDropData, (), (const, override));
  MOCK_METHOD(void, CreateView, (gfx::NativeView context), (override));
  MOCK_METHOD(RenderWidgetHostViewBase*,
              CreateViewForWidget,
              (RenderWidgetHost * render_widget_host),
              (override));
  MOCK_METHOD(RenderWidgetHostViewBase*,
              CreateViewForChildWidget,
              (RenderWidgetHost * render_widget_host),
              (override));
  MOCK_METHOD(void, SetPageTitle, (const std::u16string& title), (override));
  MOCK_METHOD(void, RenderViewReady, (), (override));
  MOCK_METHOD(void,
              RenderViewHostChanged,
              (RenderViewHost * old_host, RenderViewHost* new_host),
              (override));
  MOCK_METHOD(void, SetOverscrollControllerEnabled, (bool enabled), (override));
  MOCK_METHOD(void, OnCapturerCountChanged, (), (override));
  MOCK_METHOD(void, FullscreenStateChanged, (bool is_fullscreen), (override));
  MOCK_METHOD(void,
              UpdateWindowControlsOverlay,
              (const gfx::Rect& bounding_rect),
              (override));
  MOCK_METHOD(BackForwardTransitionAnimationManager*,
              GetBackForwardTransitionAnimationManager,
              (),
              (override));
  MOCK_METHOD(void,
              DestroyBackForwardTransitionAnimationManager,
              (),
              (override));

#if BUILDFLAG(IS_MAC)
  MOCK_METHOD(bool, CloseTabAfterEventTrackingIfNeeded, (), (override));
#endif

  MOCK_METHOD(void, DidStopRefresh, (), (override));

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  MOCK_METHOD(void,
              UpdateBrowserControlsHeight,
              (int height, bool animate),
              (override));
#endif
};

class MockDelegate : public RenderFrameHostManager::Delegate {
 public:
  MockDelegate() = default;
  ~MockDelegate() override = default;

  MOCK_METHOD(bool,
              CreateRenderViewForRenderManager,
              (RenderViewHost * render_view_host,
               const std::optional<blink::FrameToken>& opener_frame_token,
               RenderFrameProxyHost* proxy_host),
              (override));

  MOCK_METHOD(void,
              CreateRenderWidgetHostViewForRenderManager,
              (RenderViewHost * render_view_host),
              (override));

  MOCK_METHOD(void,
              BeforeUnloadFiredFromRenderManager,
              (bool proceed, bool* proceed_to_fire_unload),
              (override));

  MOCK_METHOD(void, CancelModalDialogsForRenderManager, (), (override));

  MOCK_METHOD(void,
              NotifySwappedFromRenderManager,
              (RenderFrameHostImpl * old_frame, RenderFrameHostImpl* new_frame),
              (override));

  MOCK_METHOD(void,
              NotifySwappedFromRenderManagerWithoutFallbackContent,
              (RenderFrameHostImpl * new_frame),
              (override));

  MOCK_METHOD(void,
              NotifyMainFrameSwappedFromRenderManager,
              (RenderFrameHostImpl * old_frame, RenderFrameHostImpl* new_frame),
              (override));

  MOCK_METHOD(bool, FocusLocationBarByDefault, (), (override));
  MOCK_METHOD(void, ReattachOuterDelegateIfNeeded, (), (override));
  MOCK_METHOD(void,
              OnFrameTreeNodeDestroyed,
              (FrameTreeNode * node),
              (override));
  MOCK_METHOD(const std::string&, SharedRenderProcessToken, (), (override));
};

class MockRenderFrameHostManager : public RenderFrameHostManager {
 public:
  MockRenderFrameHostManager(FrameTreeNode* frame_tree_node, Delegate* delegate)
      : RenderFrameHostManager(frame_tree_node, delegate) {}
  virtual ~MockRenderFrameHostManager() = default;

  MOCK_METHOD(RenderFrameHostImpl*,
              speculative_frame_host,
              (),
              (const, override));
  MOCK_METHOD(RenderFrameHostImpl*, current_frame_host, (), (const, override));
};

class TestWebContentsImplExt : public TestWebContents {
 public:
  explicit TestWebContentsImplExt(BrowserContext* browser_context)
      : TestWebContents(browser_context) {}

  virtual ~TestWebContentsImplExt() {}
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

  void UpdataMap(gfx::Rect rect) {
    ExtendContent()->native_embed_rect_info_map_.insert(
        std::make_pair("1", rect));
  }

  void UpdateObserver() {
    ExtendContent()->native_web_contents_observer_ = std::move(nullptr);
  }

  void UpdateDelegate() {
    ExtendContent()->video_assistant_ = std::move(nullptr);
  }

  void SetPipStatus(bool flag) {
    ExtendContent()->pip_status_ = flag;
  }

  void UpdateDeleteView(RenderViewHostDelegateView* view) {
    ExtendContent()->render_view_host_delegate_view_ = view;
  }

  void SetView(std::unique_ptr<WebContentsView> view) {
    ExtendContent()->view_ = std::move(view);
  }

  void SetContextHost(WebContents* content) {
    ExtendContent()->wake_lock_context_host_ =
        std::make_unique<WakeLockContextHost>(content);
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

#if BUILDFLAG(ARKWEB_AI)
TEST_F(WebContentsImplExtTest, OnDataDetectorSelectText001) {
  ExtendContent()->OnDataDetectorSelectText();
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

TEST_F(WebContentsImplExtTest, OnLayerRectVisibilityChange001) {
  std::string embed_id = "abc";
  bool visibility = false;
  ExtendContent()->OnLayerRectVisibilityChange(embed_id, visibility);
}

TEST_F(WebContentsImplExtTest, SetDelegate001) {
  auto delegate = std::make_unique<MockWebContentsDelegateExtended>();
  ExtendContent()->SetDelegate(delegate.get());
}

TEST_F(WebContentsImplExtTest, SetDelegate002) {
  auto delegate = std::make_unique<MockWebContentsDelegateExtended>();
  EXPECT_CALL(*delegate, CreateVideoAssistant())
      .WillOnce(testing::Return(std::make_unique<VideoAssistant>()));
  ExtendContent()->SetDelegate(delegate.get());
}

TEST_F(WebContentsImplExtTest, SetDelegate003) {
  auto delegate = std::make_unique<MockWebContentsDelegateExtended>();
  EXPECT_CALL(*delegate, CreateVideoAssistant())
      .WillOnce(testing::Return(nullptr));
  ExtendContent()->SetDelegate(delegate.get());
}

TEST_F(WebContentsImplExtTest, EnableAdsBlock003) {
  ExtendContent()->SetTestFlag(true);
  bool enable = true;
  ExtendContent()->EnableAdsBlock(enable);
}

TEST_F(WebContentsImplExtTest, EnableAdsBlock004) {
  ExtendContent()->SetTestFlag(true);
  bool enable = false;
  ExtendContent()->EnableAdsBlock(enable);
}

TEST_F(WebContentsImplExtTest, IsAdsBlockEnabled002) {
  ExtendContent()->SetTestFlag(true);
  auto flag = ExtendContent()->IsAdsBlockEnabled();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, TrigAdBlockEnabledForSiteFromUi002) {
  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);
  std::string main_frame_url = "";
  ExtendContent()->TrigAdBlockEnabledForSiteFromUi(main_frame_url);
}

TEST_F(WebContentsImplExtTest, TrigAdBlockEnabledForSiteFromUi003) {
  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);
  ExtendContent()->SetDelegate(nullptr);
  std::string main_frame_url = "";
  ExtendContent()->TrigAdBlockEnabledForSiteFromUi(main_frame_url);
}

TEST_F(WebContentsImplExtTest, TrigAdBlockEnabledForSiteFromUi004) {
  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);

  auto delegate = std::make_unique<MockWebContentsDelegateExtended>();
  ExtendContent()->SetDelegate(delegate.get());
  std::string main_frame_url = "";
  ExtendContent()->TrigAdBlockEnabledForSiteFromUi(main_frame_url);
}

TEST_F(WebContentsImplExtTest, TrigAdBlockEnabledForSiteFromUi005) {
  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);

  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  EXPECT_CALL(delegate_extend,
              TrigAdBlockEnabledForSiteFromUi(testing::_, testing::_))
      .WillOnce(testing::Return(true));
  std::string main_frame_url = "";
  ExtendContent()->TrigAdBlockEnabledForSiteFromUi(main_frame_url);
}

TEST_F(WebContentsImplExtTest, TrigAdBlockEnabledForSiteFromUi006) {
  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);

  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  EXPECT_CALL(delegate_extend,
               TrigAdBlockEnabledForSiteFromUi(testing::_, testing::_))
      .WillOnce(testing::Return(false));
  std::string main_frame_url = "";
  ExtendContent()->TrigAdBlockEnabledForSiteFromUi(main_frame_url);
}

TEST_F(WebContentsImplExtTest, OnAdsBlocked002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  std::string main_frame_url = "";
  std::map<std::string, int32_t> subresource_blocked;
  bool is_site_first_report = false;
  ExtendContent()->OnAdsBlocked(main_frame_url, subresource_blocked,
                                is_site_first_report);
}

TEST_F(WebContentsImplExtTest, OnNativeEmbedFirstFramePaint002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int32_t native_embed_id = 0;
  std::string embed_id_attribute = "";
  ExtendContent()->OnNativeEmbedFirstFramePaint(native_embed_id,
                                                embed_id_attribute);
}

TEST_F(WebContentsImplExtTest, OnLayerRectVisibilityChange002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  std::string embed_id = "";
  bool visibility = false;
  ExtendContent()->OnLayerRectVisibilityChange(embed_id, visibility);
}

TEST_F(WebContentsImplExtTest, ExtensionGetTabId002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  auto id = ExtendContent()->ExtensionGetTabId();
  EXPECT_EQ(id, -1);
}

TEST_F(WebContentsImplExtTest, ClearContextMenu002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  ExtendContent()->ClearContextMenu();
}

TEST_F(WebContentsImplExtTest, FullScreenChanged000) {
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  bool is_fullscreen = true;
  ExtendContent()->FullScreenChanged(media_player_id, is_fullscreen);
}

TEST_F(WebContentsImplExtTest, OnNativeEmbedStatusUpdate002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  NativeEmbedInfo::TagState state = NativeEmbedInfo::TagState::TAG_STATE_CREATE;
  NativeEmbedInfo native_embed_info;
  ExtendContent()->OnNativeEmbedStatusUpdate(native_embed_info, state);
}

TEST_F(WebContentsImplExtTest, OnNativeEmbedStatusUpdate003) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  gfx::Rect rect(0, 0);
  UpdataMap(rect);

  NativeEmbedInfo::TagState state = NativeEmbedInfo::TagState::TAG_STATE_CREATE;
  NativeEmbedInfo native_embed_info;
  native_embed_info.embed_element_id = "1";

  ExtendContent()->OnNativeEmbedStatusUpdate(native_embed_info, state);
}

TEST_F(WebContentsImplExtTest, OnNativeEmbedStatusUpdate004) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  gfx::Rect rect(50, 50);
  UpdataMap(rect);

  NativeEmbedInfo::TagState state = NativeEmbedInfo::TagState::TAG_STATE_CREATE;
  NativeEmbedInfo native_embed_info;
  native_embed_info.embed_element_id = "1";
  native_embed_info.rect = gfx::Rect(50, 50);

  ExtendContent()->OnNativeEmbedStatusUpdate(native_embed_info, state);
}

TEST_F(WebContentsImplExtTest, OnRenderFrameHostEnterBackForwardCache002) {
  GlobalRenderFrameHostId host_id(0, 0);
  UpdateObserver();
  ExtendContent()->OnRenderFrameHostEnterBackForwardCache(host_id);
}

TEST_F(WebContentsImplExtTest, OnRenderFrameHostLeaveBackForwardCache002) {
  GlobalRenderFrameHostId host_id(0, 0);
  UpdateObserver();
  ExtendContent()->OnRenderFrameHostLeaveBackForwardCache(host_id);
}

TEST_F(WebContentsImplExtTest, MouseSelectMenuShow002) {
  auto ptr_view = std::make_unique<MockRenderViewHostDelegateView>();
  UpdateDeleteView(ptr_view.get());
  bool flag = true;
  ExtendContent()->MouseSelectMenuShow(flag);
}

TEST_F(WebContentsImplExtTest, ChangeVisibilityOfQuickMenu002) {
  auto ptr_view = std::make_unique<MockRenderViewHostDelegateView>();
  UpdateDeleteView(ptr_view.get());
  ExtendContent()->ChangeVisibilityOfQuickMenu();
}

TEST_F(WebContentsImplExtTest, CloseImageOverlaySelection002) {
  auto ptr_view = std::make_unique<MockRenderViewHostDelegateView>();
  UpdateDeleteView(ptr_view.get());
  ExtendContent()->CloseImageOverlaySelection();
}

TEST_F(WebContentsImplExtTest, OnOverlayZoomChanged002) {
  auto ptr_view = std::make_unique<MockRenderViewHostDelegateView>();
  UpdateDeleteView(ptr_view.get());
  ExtendContent()->OnOverlayZoomChanged();
}

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
TEST_F(WebContentsImplExtTest, UpdateBrowserControlsHeight002) {
  auto ptr_webview = std::make_unique<MockWebContentsView>();
  SetView(std::move(ptr_webview));
  int height = 0;
  bool animate = false;
  ExtendContent()->UpdateBrowserControlsHeight(height, animate);
}
#endif

TEST_F(WebContentsImplExtTest, CreateCustomMediaPlayer002) {
  std::unique_ptr<MockCustomMediaPlayerListener> listener =
      std::make_unique<MockCustomMediaPlayerListener>();
  MediaInfo media_info;
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  auto rst =
      ExtendContent()->CreateCustomMediaPlayer(std::move(listener), media_info);
  EXPECT_EQ(rst, nullptr);
}

TEST_F(WebContentsImplExtTest, SetDelegate004) {
  ExtendContent()->SetDelegate(nullptr);
}

TEST_F(WebContentsImplExtTest, SetDelegate005) {
  UpdateDelegate();
  ExtendContent()->SetDelegate(nullptr);
}

TEST_F(WebContentsImplExtTest, MediaDestroyed002) {
  UpdateDelegate();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->MediaDestroyed(media_player_id);
}

TEST_F(WebContentsImplExtTest, EnableVideoAssistant002) {
  bool enable = true;
  ExtendContent()->EnableVideoAssistant(enable);
}

TEST_F(WebContentsImplExtTest, OnShowToast002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  double duration = 0.0;
  std::string toast = "abc";
  ExtendContent()->OnShowToast(duration, toast);
}

TEST_F(WebContentsImplExtTest, OnReportStatisticLog002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  std::string content = "abc";
  ExtendContent()->OnReportStatisticLog(content);
}

TEST_F(WebContentsImplExtTest, PopluateVideoAssistantConfig002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  media::mojom::VideoAssistantConfigPtr config;
  ExtendContent()->PopluateVideoAssistantConfig(config);
}

TEST_F(WebContentsImplExtTest, OnFullScreenOverlayEnter002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  media::mojom::MediaInfoForVASTPtr media_info =
      media::mojom::MediaInfoForVAST::New();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  ExtendContent()->OnFullScreenOverlayEnter(std::move(media_info),
                                            media_player_id);
}

TEST_F(WebContentsImplExtTest, ShowAutofillPopup002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);

  gfx::RectF element_bounds;
  bool is_rtl = false;
  std::vector<autofill::Suggestion> suggestions;
  bool is_password_popup_type = false;
  ExtendContent()->ShowAutofillPopup(element_bounds, is_rtl, suggestions,
                                     is_password_popup_type);
}

TEST_F(WebContentsImplExtTest, HideAutofillPopup002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  ExtendContent()->HideAutofillPopup();
}

TEST_F(WebContentsImplExtTest, OnShareFile002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  std::string filePath = "";
  std::string utdTypeId = "";
  ExtendContent()->OnShareFile(filePath, utdTypeId);
}

TEST_F(WebContentsImplExtTest, OnBeforeUnloadFired002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  bool proceed = false;
  ExtendContent()->OnBeforeUnloadFired(proceed);
}

TEST_F(WebContentsImplExtTest, ExitFullscreenMode001) {
  bool will_cause_resize = false;
  ExtendContent()->ExitFullscreenMode(will_cause_resize);
}

TEST_F(WebContentsImplExtTest, RenderViewReady001) {
  int render_process_id = 0;
  int render_view_id = 0;
  RenderViewHost* host =
      RenderViewHost::FromID(render_process_id, render_view_id);
  ExtendContent()->RenderViewReady(host);
}

TEST_F(WebContentsImplExtTest, OnPip004) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int status = 0;
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int width = 0;
  int height = 0;
  ExtendContent()->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                         height);
}

TEST_F(WebContentsImplExtTest, OnPip005) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int status = 7;
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int width = 0;
  int height = 0;
  ExtendContent()->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                         height);
}

TEST_F(WebContentsImplExtTest, OnPip006) {
  MockWebContentsDelegateExtended delegate_extend;
  contents()->SetDelegate(&delegate_extend);
  int status = 1;
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int width = 0;
  int height = 0;
  ExtendContent()->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                         height);
}

TEST_F(WebContentsImplExtTest, OnPip007) {
  MockWebContentsDelegateExtended delegate_extend;
  contents()->SetDelegate(&delegate_extend);
  int status = 11;
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int width = 0;
  int height = 0;
  ExtendContent()->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                         height);
}

TEST_F(WebContentsImplExtTest, OnPipEvent002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 0;
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent003) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 7;
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent004) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  SetPipStatus(true);
  int event = 1;
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent005) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 0;
  SetPipStatus(true);
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent006) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 7;
  SetPipStatus(true);
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent007) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 1;
  SetPipStatus(true);
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent008) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 1;
  SetPipStatus(false);
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent009) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 8;
  SetPipStatus(false);
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent0010) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 8;
  SetPipStatus(true);
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, OnPipEvent0011) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int event = 1;
  SetPipStatus(true);
  ExtendContent()->OnPipEvent(event);
}

TEST_F(WebContentsImplExtTest, UpdateAdBlockEnabledToRender002) {
  auto mock_delegate = std::make_unique<MockDelegate>();
  auto& frame_tree = ExtendContent()->GetPrimaryFrameTree();
  auto ptr_tree_node = frame_tree.root();
  auto manager = std::make_unique<MockRenderFrameHostManager>(
      ptr_tree_node, mock_delegate.get());
  ExtendContent()->SetRenderManagerForTesting(manager.get());
  EXPECT_CALL(*manager, speculative_frame_host()).Times(1);
  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);
  ExtendContent()->UpdateAdBlockEnabledToRender(false);
}

TEST_F(WebContentsImplExtTest, GetAdblockEnabledForSite002) {
  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);

  auto flag = ExtendContent()->GetAdblockEnabledForSite();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, SetAdBlockEnabledForSite003) {
  bool is_adblock_enabled = false;
  int main_frame_tree_node_id = 0;

  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);
  ExtendContent()->SetAdBlockEnabledForSite(is_adblock_enabled,
                                            main_frame_tree_node_id);
}

TEST_F(WebContentsImplExtTest, SetWakeLockHandler002) {
  int32_t window_id = 0;
  SetKeepScreenOn handler;
  int render_process_id = 0;
  int render_frame_id = 0;
  auto host = RenderFrameHost::FromID(render_process_id, render_frame_id);
  auto tents = WebContents::FromRenderFrameHost(host);
  SetContextHost(tents);
  ExtendContent()->SetWakeLockHandler(window_id, handler);
}

TEST_F(WebContentsImplExtTest, CollapseAllFramesSelection001) {
  ExtendContent()->CollapseAllFramesSelection();
}

TEST_F(WebContentsImplExtTest, OnPip008) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int status = 0;
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int width = 0;
  int height = 0;
  ExtendContent()->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                         height);
}

TEST_F(WebContentsImplExtTest, OnPip009) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int status = 7;
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int width = 0;
  int height = 0;
  ExtendContent()->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                         height);
}

TEST_F(WebContentsImplExtTest, OnPip0010) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int status = 9;
  int delegate_id = 0;
  int child_id = 0;
  int frame_routing_id = 0;
  int width = 0;
  int height = 0;
  ExtendContent()->OnPip(status, delegate_id, child_id, frame_routing_id, width,
                         height);
}

TEST_F(WebContentsImplExtTest, OnPdfScrollAtBottom002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  std::string url = "abc";
  ExtendContent()->OnPdfScrollAtBottom(url);
}

TEST_F(WebContentsImplExtTest, OnPdfLoadEvent002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int32_t result = 0;
  std::string url = "abc";
  ExtendContent()->OnPdfLoadEvent(result, url);
}

TEST_F(WebContentsImplExtTest, DelMediaPlayerAudibleCount002) {
  ExtendContent()->DelMediaPlayerAudibleCount();
  auto flag = ExtendContent()->GetMediaPlayerCurrentAudible();
  EXPECT_FALSE(flag);
}

TEST_F(WebContentsImplExtTest, EnableSafeBrowsingDetection004) {
  bool enable = true;
  bool strictMode = true;
  ExtendContent()->EnableSafeBrowsingDetection(enable, strictMode);
  auto flag = ExtendContent()->IsSafeBrowsingDetectionStrict();
  EXPECT_TRUE(flag);
}

TEST_F(WebContentsImplExtTest, GetRenderViewHost003) {
  auto mock_delegate = std::make_unique<MockDelegate>();
  auto& frame_tree = ExtendContent()->GetPrimaryFrameTree();
  auto ptr_tree_node = frame_tree.root();
  auto manager = std::make_unique<MockRenderFrameHostManager>(
      ptr_tree_node, mock_delegate.get());
  ExtendContent()->SetRenderManagerForTesting(manager.get());
  RenderFrameHostImpl* rfh = ExtendContent()->GetPrimaryMainFrame();
  EXPECT_CALL(*manager, current_frame_host())
      .Times(1)
      .WillOnce(::testing::Return(rfh));
  auto impl = ExtendContent()->GetRenderViewHost();
  EXPECT_NE(impl, nullptr);
}

TEST_F(WebContentsImplExtTest, StartCamera002) {
  int n_Web_id = 0;
  // first para
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  MainFunctionParams main_func_para(currentProcess);
  // second para
  std::unique_ptr<base::ThreadPoolInstance::ScopedExecutionFence>
      scoped_execution_fence =
          std::make_unique<base::ThreadPoolInstance::ScopedExecutionFence>();
  std::unique_ptr<BrowserMainLoop> browser_context =
      std::make_unique<BrowserMainLoop>(std::move(main_func_para),
                                        std::move(scoped_execution_fence));
  ExtendContent()->StartCamera(n_Web_id);
}

TEST_F(WebContentsImplExtTest, StopCamera001) {
  int n_Web_id = 0;
  // first para
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  MainFunctionParams main_func_para(currentProcess);
  // second para
  std::unique_ptr<base::ThreadPoolInstance::ScopedExecutionFence>
      scoped_execution_fence =
          std::make_unique<base::ThreadPoolInstance::ScopedExecutionFence>();
  std::unique_ptr<BrowserMainLoop> browser_context =
      std::make_unique<BrowserMainLoop>(std::move(main_func_para),
                                        std::move(scoped_execution_fence));
  ExtendContent()->StopCamera(n_Web_id);
}

TEST_F(WebContentsImplExtTest, CloseCamera001) {
  int n_Web_id = 0;
  // first para
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  MainFunctionParams main_func_para(currentProcess);
  // second para
  std::unique_ptr<base::ThreadPoolInstance::ScopedExecutionFence>
      scoped_execution_fence =
          std::make_unique<base::ThreadPoolInstance::ScopedExecutionFence>();
  std::unique_ptr<BrowserMainLoop> browser_context =
      std::make_unique<BrowserMainLoop>(std::move(main_func_para),
                                        std::move(scoped_execution_fence));
  ExtendContent()->CloseCamera(n_Web_id);
}

TEST_F(WebContentsImplExtTest, StopScreenCapture002) {
  // first para
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  MainFunctionParams main_func_para(currentProcess);
  // second para
  std::unique_ptr<base::ThreadPoolInstance::ScopedExecutionFence>
      scoped_execution_fence =
          std::make_unique<base::ThreadPoolInstance::ScopedExecutionFence>();
  std::unique_ptr<BrowserMainLoop> browser_context =
      std::make_unique<BrowserMainLoop>(std::move(main_func_para),
                                        std::move(scoped_execution_fence));
  int32_t nweb_id = 0;
  std::string session_id = "";
  ExtendContent()->StopScreenCapture(nweb_id, session_id);
}

TEST_F(WebContentsImplExtTest, SetScreenCapturePickerShow002) {
  // first para
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  MainFunctionParams main_func_para(currentProcess);
  // second para
  std::unique_ptr<base::ThreadPoolInstance::ScopedExecutionFence>
      scoped_execution_fence =
          std::make_unique<base::ThreadPoolInstance::ScopedExecutionFence>();
  std::unique_ptr<BrowserMainLoop> browser_context =
      std::make_unique<BrowserMainLoop>(std::move(main_func_para),
                                        std::move(scoped_execution_fence));
  ExtendContent()->SetScreenCapturePickerShow();
}

TEST_F(WebContentsImplExtTest, DisableSessionReuse002) {
  // first para
  auto currentProcess = base::CommandLine::ForCurrentProcess();
  MainFunctionParams main_func_para(currentProcess);
  // second para
  std::unique_ptr<base::ThreadPoolInstance::ScopedExecutionFence>
      scoped_execution_fence =
          std::make_unique<base::ThreadPoolInstance::ScopedExecutionFence>();
  std::unique_ptr<BrowserMainLoop> browser_context =
      std::make_unique<BrowserMainLoop>(std::move(main_func_para),
                                        std::move(scoped_execution_fence));
  ExtendContent()->DisableSessionReuse();
}

TEST_F(WebContentsImplExtTest, EnterFullscreenMode002) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      "enable-nweb-ex-top-controls");
  int render_process_id = 0;
  int render_frame_id = 0;
  auto ptr_host = RenderFrameHost::FromID(render_process_id, render_frame_id);
  auto ptr_host_impl = RenderFrameHostImpl::From(ptr_host);
  blink::mojom::FullscreenOptions options;
  EXPECT_TRUE(base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableNwebExTopControls));
  ExtendContent()->EnterFullscreenMode(ptr_host_impl, options);
}

TEST_F(WebContentsImplExtTest, ExitFullscreenMode002) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      "enable-nweb-ex-top-controls");
  EXPECT_TRUE(base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableNwebExTopControls));
  bool will_cause_resize = false;
  ExtendContent()->ExitFullscreenMode(will_cause_resize);
}

TEST_F(WebContentsImplExtTest, RenderViewReady002) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      "enable-nweb-ex-top-controls");
  EXPECT_TRUE(base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableNwebExTopControls));
  int render_process_id = 0;
  int render_view_id = 0;
  RenderViewHost* view_host =
      RenderViewHost::FromID(render_process_id, render_view_id);
  ExtendContent()->RenderViewReady(view_host);
}

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
TEST_F(WebContentsImplExtTest, OnDocumentEndReady001) {
  FrameInfos frameInfo;
  ExtendContent()->OnDocumentEndReady(frameInfo);
}
#endif

TEST_F(WebContentsImplExtTest, OnDocumentEndReady002) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  FrameInfos frameInfo;
  ExtendContent()->OnDocumentEndReady(frameInfo);
}

TEST_F(WebContentsImplExtTest, OnMediaCastEnter001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  ExtendContent()->OnMediaCastEnter();
}

TEST_F(WebContentsImplExtTest, NotifyRemoteExitFullScreen001) {
  ExtendContent()->NotifyRemoteExitFullScreen();
}

TEST_F(WebContentsImplExtTest, OnSafeBrowsingCheckDetail001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int code = 1;
  int policy = 2;
  int threat = 3;
  ExtendContent()->OnSafeBrowsingCheckDetail(code, policy, threat);
}

TEST_F(WebContentsImplExtTest, DetectBlankScreen001) {
  std::string url = "http://test.com";
  ExtendContent()->DetectBlankScreen(url);
}

TEST_F(WebContentsImplExtTest, SetBlankScreenDetectionConfig001) {
  bool enable = true;
  std::vector<double> detectionTiming = {1.0, 2.0};
  std::vector<int32_t> detectionMethods = {1, 2};
  int32_t contentfulNodesCountThreshold = 100;
  ExtendContent()->SetBlankScreenDetectionConfig(enable, detectionTiming, 
                                                  detectionMethods, contentfulNodesCountThreshold);
}

TEST_F(WebContentsImplExtTest, OnStartBackgroundTask001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int32_t type = 1;
  std::string message = "test message";
  auto result = ExtendContent()->OnStartBackgroundTask(type, message);
  EXPECT_TRUE(result);
}

TEST_F(WebContentsImplExtTest, OnStartBackgroundTask002) {
  int32_t type = 1;
  std::string message = "test message";
  auto result = ExtendContent()->OnStartBackgroundTask(type, message);
  EXPECT_TRUE(result);
}

TEST_F(WebContentsImplExtTest, GetOverScrollOffset001) {
  float offset_x = 0.0f;
  float offset_y = 0.0f;
  ExtendContent()->GetOverScrollOffset(&offset_x, &offset_y);
  EXPECT_EQ(offset_x, 0.0f);
  EXPECT_EQ(offset_y, 0.0f);
}

TEST_F(WebContentsImplExtTest, OnOverScrollOffsetChanged001) {
  float offset_x = 10.0f;
  float offset_y = 20.0f;
  ExtendContent()->OnOverScrollOffsetChanged(offset_x, offset_y);

  float get_offset_x = 0.0f;
  float get_offset_y = 0.0f;
  ExtendContent()->GetOverScrollOffset(&get_offset_x, &get_offset_y);
  EXPECT_EQ(get_offset_x, 10.0f);
  EXPECT_EQ(get_offset_y, 20.0f);
}

TEST_F(WebContentsImplExtTest, OnRewriteUrlForNavigation001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  std::string original_url = "http://test.com";
  std::string referrer = "http://referrer.com";
  int transition_type = 1;
  bool is_key_request = true;
  auto result = ExtendContent()->OnRewriteUrlForNavigation(original_url, referrer, 
                                                              transition_type, is_key_request);
}

TEST_F(WebContentsImplExtTest, OnRewriteUrlForNavigation002) {
  std::string original_url = "http://test.com";
  std::string referrer = "http://referrer.com";
  int transition_type = 1;
  bool is_key_request = true;
  auto result = ExtendContent()->OnRewriteUrlForNavigation(original_url, referrer, 
                                                              transition_type, is_key_request);
  EXPECT_EQ(result, "");
}

TEST_F(WebContentsImplExtTest, NotifyNavigationRewriteUrl001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  std::string original_url = "http://test.com";
  std::string referrer = "http://referrer.com";
  int transition_type = 1;
  bool is_key_request = true;
  auto result = ExtendContent()->NotifyNavigationRewriteUrl(original_url, referrer, 
                                                             transition_type, is_key_request);
}

TEST_F(WebContentsImplExtTest, NotifyNavigationRewriteUrl002) {
  std::string original_url = "http://test.com";
  std::string referrer = "http://referrer.com";
  int transition_type = 1;
  bool is_key_request = true;
  auto result = ExtendContent()->NotifyNavigationRewriteUrl(original_url, referrer, 
                                                             transition_type, is_key_request);
  EXPECT_EQ(result, "");
}

#if BUILDFLAG(ARKWEB_READER_MODE)
TEST_F(WebContentsImplExtTest, GetAllFrameInfos001) {
  std::map<std::string, std::string> frameinfos;
  ExtendContent()->GetAllFrameInfos(frameinfos);
}
#endif

TEST_F(WebContentsImplExtTest, OnAudioStateChangedExt002) {
  bool is_currently_audible = false;
  bool is_ohos_currently_audible = false;
  auto result = ExtendContent()->OnAudioStateChangedExt(is_currently_audible, 
                                                          is_ohos_currently_audible);
  EXPECT_FALSE(result);
}

TEST_F(WebContentsImplExtTest, OnAudioStateChangedExtSetAudible001) {
  bool is_ohos_currently_audible = true;
  ExtendContent()->OnAudioStateChangedExtSetAudible(is_ohos_currently_audible);
}

TEST_F(WebContentsImplExtTest, OnAVCastStarted001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  media::mojom::MediaInfoForVASTPtr media_info =
      media::mojom::MediaInfoForVAST::New();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  auto result = ExtendContent()->OnAVCastStarted(std::move(media_info), media_player_id);
}

TEST_F(WebContentsImplExtTest, OnAVCastStarted002) {
  media::mojom::MediaInfoForVASTPtr media_info =
      media::mojom::MediaInfoForVAST::New();
  GlobalRenderFrameHostId host_id(0, 0);
  MediaPlayerId media_player_id(host_id, 0);
  auto result = ExtendContent()->OnAVCastStarted(std::move(media_info), media_player_id);
  EXPECT_EQ(result, nullptr);
}

TEST_F(WebContentsImplExtTest, EnableVideoAssistantAVCast001) {
  bool enable = true;
  ExtendContent()->EnableVideoAssistantAVCast(enable);
}

TEST_F(WebContentsImplExtTest, EnableVideoAssistantAVCast002) {
  bool enable = false;
  ExtendContent()->EnableVideoAssistantAVCast(enable);
}

TEST_F(WebContentsImplExtTest, GetUserAgentMetadata001) {
  std::string user_agent = "test_agent";
  auto metadata = ExtendContent()->GetUserAgentMetadata(user_agent);
}

TEST_F(WebContentsImplExtTest, GetUserAgentMetadata002) {
  std::string user_agent = "";
  auto metadata = ExtendContent()->GetUserAgentMetadata(user_agent);
}

TEST_F(WebContentsImplExtTest, isSameUserAgent001) {
  blink::UserAgentOverride ua_override;
  ua_override.ua_string_override = "test_agent";
  auto result = ExtendContent()->isSameUserAgent(ua_override);
}

TEST_F(WebContentsImplExtTest, isSameUserAgent002) {
  blink::UserAgentOverride ua_override;
  auto result = ExtendContent()->isSameUserAgent(ua_override);
}

TEST_F(WebContentsImplExtTest, SetAdBlockEnabledForSite002) {
  bool is_adblock_enabled = true;
  int main_frame_tree_node_id = 0;

  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);
  ExtendContent()->SetAdBlockEnabledForSite(is_adblock_enabled, main_frame_tree_node_id);
}

TEST_F(WebContentsImplExtTest, SetAdBlockEnabledForSite004) {
  bool is_adblock_enabled = true;
  int main_frame_tree_node_id = 1;

  auto rst = main_test_rfh();
  ExtendContent()->SetPrimaryMainFrame(rst);
  ExtendContent()->SetAdBlockEnabledForSite(is_adblock_enabled, main_frame_tree_node_id);
}

TEST_F(WebContentsImplExtTest, ProcessForPdfType001) {
  NavigationHandle* navigation_handle = nullptr;
  ExtendContent()->ProcessForPdfType(navigation_handle);
}

TEST_F(WebContentsImplExtTest, OnNativeEmbedObjectParamChange001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  NativeEmbedParamDataInfo param_info;
  ExtendContent()->OnNativeEmbedObjectParamChange(param_info);
}

TEST_F(WebContentsImplExtTest, OnNativeEmbedObjectParamChange002) {
  NativeEmbedParamDataInfo param_info;
  ExtendContent()->OnNativeEmbedObjectParamChange(param_info);
}

TEST_F(WebContentsImplExtTest, SetNWebId002) {
  int id = 100;
  ExtendContent()->SetNWebId(id);
  auto web_id = ExtendContent()->GetNWebId();
  EXPECT_EQ(web_id, 100);
}

TEST_F(WebContentsImplExtTest, OnCameraCaptureStateChanged001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int original_state = 0;
  int new_state = 1;
  ExtendContent()->OnCameraCaptureStateChanged(original_state, new_state);
}

TEST_F(WebContentsImplExtTest, OnCameraCaptureStateChanged002) {
  int original_state = 0;
  int new_state = 1;
  ExtendContent()->OnCameraCaptureStateChanged(original_state, new_state);
}

TEST_F(WebContentsImplExtTest, OnMicrophoneCaptureStateChanged001) {
  MockWebContentsDelegateExtended delegate_extend;
  ExtendContent()->SetDelegate(&delegate_extend);
  int original_state = 0;
  int new_state = 1;
  ExtendContent()->OnMicrophoneCaptureStateChanged(original_state, new_state);
}

TEST_F(WebContentsImplExtTest, OnMicrophoneCaptureStateChanged002) {
  int original_state = 0;
  int new_state = 1;
  ExtendContent()->OnMicrophoneCaptureStateChanged(original_state, new_state);
}

TEST_F(WebContentsImplExtTest, ResumeMicrophone001) {
  int nWebID = -1;
  ExtendContent()->ResumeMicrophone(nWebID);
}

TEST_F(WebContentsImplExtTest, StopMicrophone001) {
  int nWebID = -1;
  ExtendContent()->StopMicrophone(nWebID);
}

TEST_F(WebContentsImplExtTest, PauseMicrophone001) {
  int nWebID = -1;
  ExtendContent()->PauseMicrophone(nWebID);
}

TEST_F(WebContentsImplExtTest, IsQuickMenuShow001) {
  auto result = ExtendContent()->IsQuickMenuShow();
  EXPECT_FALSE(result);
}

TEST_F(WebContentsImplExtTest, IsQuickMenuShow002) {
  auto ptr_view = std::make_unique<MockRenderViewHostDelegateView>();
  UpdateDeleteView(ptr_view.get());
  auto result = ExtendContent()->IsQuickMenuShow();
  EXPECT_FALSE(result);
}

TEST_F(WebContentsImplExtTest, DelVideoAssistant001) {
  ExtendContent()->DelVideoAssistant();
}

}  // namespace content
