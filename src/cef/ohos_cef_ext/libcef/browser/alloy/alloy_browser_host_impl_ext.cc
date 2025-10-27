// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/libcef/browser/alloy/alloy_browser_host_impl.h"

#include <memory>
#include <string>
#include <utility>

#include "arkweb/build/features/features.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/trace_event/trace_event.h"
#include "cef/libcef/browser/alloy/browser_platform_delegate_alloy.h"
#include "cef/libcef/browser/audio_capturer.h"
#include "cef/libcef/browser/browser_context.h"
#include "cef/libcef/browser/browser_guest_util.h"
#include "cef/libcef/browser/browser_info.h"
#include "cef/libcef/browser/browser_info_manager.h"
#include "cef/libcef/browser/browser_platform_delegate.h"
#include "cef/libcef/browser/context.h"
#include "cef/libcef/browser/hang_monitor.h"
#include "cef/libcef/browser/media_access_query.h"
#include "cef/libcef/browser/osr/osr_util.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/libcef/common/cef_switches.h"
#include "cef/libcef/common/drag_data_impl.h"
#include "cef/libcef/common/frame_util.h"
#include "cef/libcef/common/net/url_util.h"
#include "cef/libcef/common/request_impl.h"
#include "cef/libcef/common/values_impl.h"
#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"
#endif

#include "arkweb/build/features/features.h"
#include "chrome/browser/file_select_helper.h"
#include "chrome/browser/picture_in_picture/picture_in_picture_window_manager.h"
#include "chrome/common/webui_url_constants.h"
#include "components/input/native_web_keyboard_event.h"
#include "components/printing/browser/print_composite_client.h"
#include "components/zoom/page_zoom.h"
#include "content/browser/gpu/compositor_util.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/public/browser/desktop_media_id.h"
#include "content/public/browser/file_select_listener.h"
#include "content/public/browser/keyboard_event_processing_result.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/url_constants.h"
#include "extensions/common/constants.h"
#include "extensions/browser/extension_prefs.h"
#include "libcef/browser/alloy/render_process_state_handler.h"
#include "net/base/net_errors.h"
#include "third_party/blink/public/common/frame/frame_owner_element_type.h"
#include "ui/events/base_event_utils.h"
#if BUILDFLAG(ARKWEB_CLIPBOARD)
#include "libcef/browser/osr/touch_selection_controller_client_osr.h"
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
#include "content/public/common/content_switches.h"
#include "include/arkweb_client_ext.h"
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#endif  // BUILDFLAG(ARKWEB_VIDEO_LTPO)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "arkweb/chromium_ext/gpu/ipc/common/gpu_surface_id_tracker.h"
#include "cef/ohos_cef_ext/include/arkweb_render_handler_ext.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
#include "base/command_line.h"
#include "components/prefs/pref_service.h"
#include "components/zoom/page_zoom.h"
#include "content/public/common/content_switches.h"
#include "net/base/url_util.h"
#include "third_party/blink/public/common/page/page_zoom.h"
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "arkweb/ohos_nweb/src/nweb_inputmethod_handler.h"
#include "cef/ohos_cef_ext/libcef/browser/alloy/cef_extension_keybinding_registry_views.h"
#include "chrome/browser/extensions/extension_commands_global_registry.h"
#include "chrome/browser/extensions/extension_keybinding_registry.h"
#include "libcef/browser/browser_event_util.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/content_accelerators/accelerator_util.h"
#include "ui/events/types/event_type.h"
#include "cef/ohos_cef_ext/libcef/browser/arkweb_browser_platform_delegate_ext.h"
#endif
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
#include "base/ohos/ltpo/include/sliding_observer.h"
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
#include "content/public/common/content_switches.h"
#endif
#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
#include "cef/ohos_cef_ext/include/arkweb_dialog_handler_ext.h"
#include "content/browser/ohos/date_time_chooser_ohos.h"
#endif  // ARKWEB_CSS_INPUT_TIME

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "arkweb/chromium_ext/content/public/browser/custom_media_info.h"
#include "arkweb/chromium_ext/content/public/browser/custom_media_player_listener.h"
#include "cef/ohos_cef_ext/include/cef_media_player_listener.h"
#include "cef/ohos_cef_ext/libcef/browser/alloy/custom_media_player_proxy.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

using content::KeyboardEventProcessingResult;



#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
#include "content/browser/renderer_host/render_widget_host_impl.h"
#endif

#if BUILDFLAG(ARKWEB_PERMISSION)
#include "cef/ohos_cef_ext/libcef/browser/permission/alloy_access_request.h"
#include "third_party/blink/public/mojom/mediastream/media_stream.mojom.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "chrome/browser/extensions/api/tabs/tabs_windows_api.h"
#include "chrome/browser/extensions/extension_action_dispatcher.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "extensions/browser/view_type_utils.h"
#include "extensions/browser/extension_web_contents_observer.h"
#include "libcef/browser/extensions/window_extensions_util.h"
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "base/json/json_writer.h"
#include "base/values.h"
#include "content/public/browser/media_player_controller.h"
#include "content/public/browser/media_player_listener.h"
#include "content/browser/media/video_assistant/video_assistant.h"
#include "libcef/browser/alloy/cef_media_player_controller_impl.h"
#include "libcef/browser/alloy/media_player_listener_proxy.h"
#include "media/mojo/mojom/media_player.mojom.h"
#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_engine_cloud_config.h"
#include "ohos_nweb_ex/overrides/cef/libcef/browser/video_assistant/media_player_controller_impl.h"
#include "ohos_nweb_ex/overrides/cef/libcef/browser/video_assistant/video_assistant.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_tabs_dispatcher.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_manager_dispatcher.h"
#endif  // ARKWEB_NWEB_EX
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
#include "content/browser/media/session/media_session_impl.h"
#endif // defined(OHOS_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "include/cef_media_player_controller.h"
#include "include/cef_media_player_listener_for_vast.h"
#endif // ARKWEB_VIDEO_ASSISTANT

#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_utils.h"

#if BUILDFLAG(IS_OHOS)
int32_t AlloyBrowserHostImplExt::ltpo_strategy_ = -1;
#endif

namespace {
#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
class CefDateTimeChooserCallbackImpl : public CefDateTimeChooserCallback {
 public:
  explicit CefDateTimeChooserCallbackImpl(
      base::WeakPtr<content::DateTimeChooserOHOS> date_time_chooser)
      : date_time_chooser_(date_time_chooser) {}

  void Continue(bool success, double value) override {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(
        &content::DateTimeChooserOHOS::NotifyResult,
        date_time_chooser_, success, value));
  }

 private:
  base::WeakPtr<content::DateTimeChooserOHOS> date_time_chooser_;
  IMPLEMENT_REFCOUNTING(CefDateTimeChooserCallbackImpl);
};
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
class CefMediaPlayerListenerImpl : public CefMediaPlayerListener {
 public:
  explicit CefMediaPlayerListenerImpl(
      std::unique_ptr<content::CustomMediaPlayerListener> listener)
      : listener_(std::move(listener)) {}
  void OnStatusChanged(uint32_t status) override {
    if (listener_) {
      listener_->OnStatusChanged(status);
    }
  }
  void OnVolumeChanged(double volume) override {
    if (listener_) {
      listener_->OnVolumeChanged(volume);
    }
  }
  void OnMutedChanged(bool muted) override {
    if (listener_) {
      listener_->OnMutedChanged(muted);
    }
  }
  void OnPlaybackRateChanged(double playback_rate) override {
    if (listener_) {
      listener_->OnPlaybackRateChanged(playback_rate);
    }
  }
  void OnDurationChanged(double duration) override {
    if (listener_) {
      listener_->OnDurationChanged(duration);
    }
  }
  void OnTimeUpdate(double current_time) override {
    if (listener_) {
      listener_->OnTimeUpdate(current_time);
    }
  }
  void OnBufferedEndTimeChanged(double buffered_time) override {
    if (listener_) {
      listener_->OnBufferedEndTimeChanged(buffered_time);
    }
  }
  void OnEnded() override {
    if (listener_) {
      listener_->OnEnded();
    }
  }
  void OnNetworkStateChanged(uint32_t state) override {
    if (listener_) {
      listener_->OnNetworkStateChanged(state);
    }
  }
  void OnReadyStateChanged(uint32_t state) override {
    if (listener_) {
      listener_->OnReadyStateChanged(state);
    }
  }
  void OnFullscreenChanged(bool fullscreen) override {
    if (listener_) {
      listener_->OnFullscreenChanged(fullscreen);
    }
  }
  void OnSeeking() override {
    if (listener_) {
      listener_->OnSeeking();
    }
  }
  void OnSeekFinished() override {
    if (listener_) {
      listener_->OnSeekFinished();
    }
  }
  void OnError(uint32_t error_code, const CefString& error_msg) override {
    if (listener_) {
      listener_->OnError(error_code, error_msg);
    }
  }
  void OnVideoSizeChanged(int width, int height) override {
    if (listener_) {
      listener_->OnVideoSizeChanged(width, height);
    }
  }
 private:
  IMPLEMENT_REFCOUNTING(CefMediaPlayerListenerImpl);
  std::unique_ptr<content::CustomMediaPlayerListener> listener_;
};
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#if BUILDFLAG(ARKWEB_NWEB_EX)
std::string BuildMediaInfo(
    const media::mojom::MediaInfoForVASTPtr& media_info,
    const media::mojom::VideoAssistantConfigPtr& config) {
  base::Value::Dict root;
  root.Set("id", media_info->id);
  root.Set("title", media_info->title);
  root.Set("duration", media_info->duration);
  root.Set("curTime", media_info->current_time);
  root.Set("playbackrate", media_info->playback_rate);
  root.Set("width", media_info->video_width);
  root.Set("height", media_info->video_height);
  root.Set("muted", media_info->isMuted);
  root.Set("isPlaying", media_info->isPlaying);
  root.Set("downloadBtn", media_info->show_download_button);
  root.Set("playbackrateBtn", media_info->isShowPlaybackSpeed);
  root.Set("volume", media_info->volume);
  // cloud control config
  base::Value::Dict policy;
  policy.Set("downloadButton", static_cast<int>(config->download_button));
  policy.Set("playbackRateButton", config->playback_rate);
  root.Set("policy", std::move(policy));

  root.Set("fullscreenoverlay", media_info->fullscreen_overlay);

  auto json = base::WriteJson(root);
  return json ? json.value() : std::string();
}
#endif // ARKWEB_NWEB_EX
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_NWEB_EX)
content::BrowserContext* GetIncognitoContext(
    content::BrowserContext* browser_context) {
  if (!browser_context) {
    return nullptr;
  }
 
  extensions::ExtensionsBrowserClient* browser_client =
      extensions::ExtensionsBrowserClient::Get();
  if (!browser_client->HasOffTheRecordContext(browser_context)) {
    return nullptr;
  }
 
  return browser_client->GetOffTheRecordContext(browser_context);
}
#endif // ARKWEB_NWEB_EX

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
const char* kBlockedWebUIHostsInIncognito[] = {
  chrome::kChromeUIExtensionsHost,
};
 
bool IsBlockedWebUIHostInIncognito(const std::string_view& host) {
  for (auto& blocked_host : kBlockedWebUIHostsInIncognito) {
    if (base::EqualsCaseInsensitiveASCII(blocked_host, host)) {
      return true;
    }
  }
  return false;
}
#endif // ARKWEB_ARKWEB_EXTENSIONS

}


AlloyBrowserHostImplExt::AlloyBrowserHostImplExt(
      const CefBrowserSettings& settings,
      CefRefPtr<CefClient> client,
      content::WebContents* web_contents,
      scoped_refptr<CefBrowserInfo> browser_info,
      CefRefPtr<AlloyBrowserHostImpl> opener,
      CefRefPtr<CefRequestContextImpl> request_context,
      std::unique_ptr<CefBrowserPlatformDelegate> platform_delegate)
      : CefBrowserHostBase(settings,
                         client,
                         std::move(platform_delegate),
                         browser_info,
                         request_context),
        AlloyBrowserHostImpl(settings,
                            client,
                            web_contents,
                            browser_info,
                            opener,
                            request_context,
                            std::move(platform_delegate)) {
  platform_delegate_->BrowserCreated(this);
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
void AlloyBrowserHostImplExt::ShowDevToolsWith(
    CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
    CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate,
    const CefPoint& inspect_element_at) {
  LOG(INFO) << "ShowDevToolsWith";
  CEF_REQUIRE_UIT();
  if (!EnsureDevToolsProtocolManager()) {
    return;
  }
  devtools_protocol_manager_->ShowDevToolsWith(
      frontend_browser, delegate, inspect_element_at);
}
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void AlloyBrowserHostImplExt::WasOccluded(bool occluded) {
  LOG(DEBUG) << "web was occluded:" << occluded;
  if (!IsWindowless()) {
    NOTREACHED() << "Window rendering is not disabled";
  }

  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&AlloyBrowserHostImplExt::WasOccluded, this, occluded));
    return;
  }

  is_hidden_ = occluded;
  ReportWindowStatus(false);

  if (platform_delegate_)
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->WasOccluded(occluded);
}

void AlloyBrowserHostImplExt::SetEnableLowerFrameRate(bool enabled) {
  LOG(DEBUG) << "SetEnableLowerFrameRate:" << enabled;
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&AlloyBrowserHostImplExt::SetEnableLowerFrameRate, this, enabled));
    return;
  }

  if (web_contents() == nullptr) {
    return;
  }

  auto rvh = web_contents()->GetRenderViewHost();
  if (rvh && rvh->GetWidget()) {
    ArkWebRenderWidgetHostViewOSRExt* view =
        static_cast<ArkWebRenderWidgetHostViewOSRExt*>(rvh->GetWidget()->GetView());
    if (view) {
      view->SetEnableLowerFrameRate(enabled);
    }
  }
}
#endif


#if BUILDFLAG(IS_ARKWEB)
void AlloyBrowserHostImplExt::GetRootBrowserAccessibilityManager(
    void** manager) {
  LOG(INFO) << "dsf AlloyBrowserHostImplExt::GetRootBrowserAccessibilityManager start";
  if (!platform_delegate_)
    return;
  LOG(INFO) << "dsf AlloyBrowserHostImplExt::GetRootBrowserAccessibilityManager middle";
  *manager = static_cast<void*>(
        platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->GetRootBrowserAccessibilityManager());
  LOG(INFO) << "wulonghui AlloyBrowserHostImplExt::GetRootBrowserAccessibilityManager end";
}
void AlloyBrowserHostImplExt::SetWakeLockHandler(int32_t windowId, CefRefPtr<CefSetLockCallback> callback) {
  LOG(DEBUG) << "SetWakeLockHandler enter, callback =" << static_cast<bool>(callback);
  if (!web_contents()) {
    LOG(ERROR) << "set screen lock error, web_contents is nullptr";
    return;
  }
  SetKeepScreenOn handler = nullptr;
  if (callback) {
    handler = [callback](bool key) {
      if (callback) {
        callback->Handle(key);
      }
    };
  }

  web_contents()->SetWakeLockHandler(windowId, std::move(handler));
}
#endif


#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
bool AlloyBrowserHostImplExt::IsIframe()
{
  if (web_contents() && web_contents()->GetFocusedFrame()) {
    return web_contents()->GetFocusedFrame()->GetFrameOwnerElementType() == blink::FrameOwnerElementType::kIframe;
  }
  return false;
}

void AlloyBrowserHostImplExt::ReloadFocusedFrame()
{
  if (web_contents()) {
    web_contents()->ReloadFocusedFrame();
  }
}
#endif

#if BUILDFLAG(ARKWEB_PDF)
void AlloyBrowserHostImplExt::OnPdfScrollAtBottom(const std::string& url)
{
  if (platform_delegate_) {
    platform_delegate_->OnPdfScrollAtBottom(url);
  }
}

void AlloyBrowserHostImplExt::OnPdfLoadEvent(int32_t result, const std::string& url)
{
  if (platform_delegate_) {
    platform_delegate_->OnPdfLoadEvent(result, url);
  }
}
#endif  // BUILDFLAG(ARKWEB_PDF)


#if BUILDFLAG(ARKWEB_SAME_LAYER)
void AlloyBrowserHostImplExt::OnNativeEmbedStatusUpdate(
    const content::NativeEmbedInfo& native_embed_info,
    content::NativeEmbedInfo::TagState state) {
  if (!platform_delegate_) {
    return;
  }

  ArkWebRenderHandlerExt::CefNativeEmbedData data_info;
  data_info.status = static_cast<ArkWebRenderHandlerExt::CefEmbedLifeStatus>(state);
  content::GpuProcessHost* host = content::GpuProcessHost::Get();
  data_info.surfaceId = host->gpu_host()->GetSurfaceId(native_embed_info.native_embed_id);
  data_info.embedId = std::to_string(native_embed_info.native_embed_id);
  data_info.info.id = native_embed_info.embed_element_id;
  data_info.info.x = native_embed_info.rect.x();
  data_info.info.y = native_embed_info.rect.y();
  data_info.info.width = native_embed_info.rect.width();
  data_info.info.height = native_embed_info.rect.height();
  data_info.info.type = native_embed_info.native_type;
  data_info.info.src = native_embed_info.native_source;
  data_info.info.url = native_embed_info.url.path();
  data_info.info.tag = native_embed_info.tag;
  data_info.info.params = native_embed_info.params;

  platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnNativeEmbedLifecycleChange(data_info);
}

void AlloyBrowserHostImplExt::OnNativeEmbedFirstFramePaint(
    int32_t native_embed_id, const std::string& embed_id_attribute) {
  if (!platform_delegate_) {
    return;
  }
  content::GpuProcessHost* host = content::GpuProcessHost::Get();
  content::NativeEmbedFirstPaintEvent event;
  event.embed_id = std::to_string(native_embed_id);
  event.surface_id = host->gpu_host()->GetSurfaceId(native_embed_id);
  event.embed_id_attribute = embed_id_attribute;
  platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnNativeEmbedFirstFramePaint(event);
}

void AlloyBrowserHostImplExt::OnLayerRectVisibilityChange(const std::string& embed_id, bool visibility) {
  if (!platform_delegate_) {
    return;
  }

  platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnNativeEmbedVisibilityChange(embed_id, visibility);
}

void AlloyBrowserHostImplExt::OnNativeEmbedObjectParamChange(
    const content::NativeEmbedParamDataInfo& native_param_info) {
  if (!platform_delegate_) {
    return;
  }

  ArkWebRenderHandlerExt::CefNativeParamData native_param_data;
  native_param_data.embedId = std::to_string(native_param_info.embed_id);
  native_param_data.objectAttributeId = native_param_info.object_attribute_id;
  for (const auto& param_item : native_param_info.param_items) {
    ArkWebRenderHandlerExt::CefNativeParamItem native_param_item;
    switch (param_item.status) {
      case content::NativeEmbedParamStatus::kAdd:
        native_param_item.status = ArkWebRenderHandlerExt::CefNativeParamStatus::PARAM_ADD;
        break;
      case content::NativeEmbedParamStatus::kUpdate:
        native_param_item.status = ArkWebRenderHandlerExt::CefNativeParamStatus::PARAM_UPDATE;
        break;
      case content::NativeEmbedParamStatus::kDelete:
        native_param_item.status = ArkWebRenderHandlerExt::CefNativeParamStatus::PARAM_DELETE;
        break;
    }
    native_param_item.id = param_item.id;
    native_param_item.name = param_item.name;
    native_param_item.value = param_item.value;
    native_param_data.paramItems.push_back(native_param_item);
  }

  platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnNativeEmbedObjectParamChange(native_param_data);
}
#endif


#if BUILDFLAG(ARKWEB_FOCUS)
void AlloyBrowserHostImplExt::ActivateContents(content::WebContents* contents) {
  LOG(INFO) << "AlloyBrowserHostImplExt::ActivateContents";
  OnSetFocus(FOCUS_SOURCE_SYSTEM);
#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
    contents_delegate_.OnActivateContent();
#endif
}
#endif  // BUILDFLAG(ARKWEB_FOCUS)


#if BUILDFLAG(ARKWEB_SAME_LAYER)
void AlloyBrowserHostImplExt::SetNativeEmbedMode(bool flag) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetNativeEmbedMode(flag);
  }
}

void AlloyBrowserHostImplExt::SetNativeInnerWeb(bool isInnerWeb) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetNativeInnerWeb(isInnerWeb);
  }
}

void AlloyBrowserHostImplExt::SetEnableCustomVideoPlayer(bool flag) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetEnableCustomVideoPlayer(flag);
  }
}
#endif


#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
void AlloyBrowserHostImplExt::OnFormEditingStateChanged(bool state, uint64_t form_id) {
  LOG(INFO) << "AlloyBrowserHostImplExt::OnFormEditingStateChanged state: " << state;
  if (client_.get() && client_->AsArkWebClient()->GetFormHandler().get())
    client_->AsArkWebClient()->GetFormHandler()->OnFormEditingStateChanged(this, state, form_id);
}

void AlloyBrowserHostImplExt::MediaStartedPlaying(
    const content::WebContentsObserver::MediaPlayerInfo& video_type,
    const content::MediaPlayerId& id) {
  LOG(INFO) << "AlloyBrowserHostImplExt::MediaStartedPlaying, is_video: " << video_type.has_video;
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  content::RenderFrameHost* main_frame = web_contents()->GetPrimaryMainFrame();
  content::RenderProcessHost* host = main_frame->GetProcess();
  if (host && video_type.has_video) {
    LOG(DEBUG) << "AlloyBrowserHostImplExt::MediaStartedPlaying, pid: " << host->GetProcess().Pid()
        << ", video_stream_cnt: " << video_stream_cnt_;
    if (video_stream_cnt_ == 0) {
        OHOS::NWeb::ResSchedClientAdapter::ReportVideoPlaying(
            OHOS::NWeb::ResSchedStatusAdapter::VIDEO_PLAYING_START, host->GetProcess().Pid());
    }
    ++video_stream_cnt_;
  }
#endif
  start_play_ = true;
  cef_media_type_t type = video_type.has_video ? cef_media_type_t::VIDEO : cef_media_type_t::AUDIO;
  if (client_.get() && client_->AsArkWebClient()->GetMediaHandler().get()) {
    client_->AsArkWebClient()->GetMediaHandler()->OnMediaStateChanged(this, type, cef_media_playing_state_t::PLAYING);
  }

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  if (type == cef_media_type_t::VIDEO && !set_lower_frame_rate_) {
    has_video_playing_ = true;
    has_touch_event_ = false;
    CEF_POST_DELAYED_TASK(CEF_UIT, base::BindOnce(&AlloyBrowserHostImplExt::UpdateVSyncFrequency,
                                                    this), WAIT_TOUCH_EVENT_DELAY_TIME);
  }
#endif
}

void AlloyBrowserHostImplExt::MediaStoppedPlaying(
    const content::WebContentsObserver::MediaPlayerInfo& video_type,
    const content::MediaPlayerId& id,
    content::WebContentsObserver::MediaStoppedReason reason) {
  LOG(INFO) << "AlloyBrowserHostImplExt::MediaStoppedPlaying, is_video: " << video_type.has_video << " stopped reason: " << static_cast<int>(reason);

  if (!start_play_) {
    return;
  }

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  content::RenderFrameHost* main_frame = web_contents()->GetPrimaryMainFrame();
  content::RenderProcessHost* host = main_frame->GetProcess();
  if (host && video_type.has_video) {
    LOG(DEBUG) << "AlloyBrowserHostImplExt::MediaStartedPlaying, pid: " << host->GetProcess().Pid()
        << ", video_stream_cnt: " << video_stream_cnt_;
    --video_stream_cnt_;
    if (video_stream_cnt_ == 0) {
        OHOS::NWeb::ResSchedClientAdapter::ReportVideoPlaying(
            OHOS::NWeb::ResSchedStatusAdapter::VIDEO_PLAYING_STOP, host->GetProcess().Pid());
    }
    video_stream_cnt_ = std::max(video_stream_cnt_, 0);
  }
#endif

  cef_media_type_t type = video_type.has_video ? cef_media_type_t::VIDEO : cef_media_type_t::AUDIO;
  cef_media_playing_state_t state;

  switch(reason)
  {
    case content::WebContentsObserver::MediaStoppedReason::kReachedEndOfStream:
      state = cef_media_playing_state_t::END_OF_STREAM;
      break;
    case content::WebContentsObserver::MediaStoppedReason::kUnspecified:
      state = cef_media_playing_state_t::PAUSE;
      break;
  }

  if (client_.get() && client_->AsArkWebClient()->GetMediaHandler().get()) {
    client_->AsArkWebClient()->GetMediaHandler()->OnMediaStateChanged(this, type, state);
  }

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  if (type == cef_media_type_t::VIDEO) {
    has_video_playing_ = false;
    if (set_lower_frame_rate_) {
      ResetVSyncFrequency();
    }
  }
#endif
}
#endif



#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void AlloyBrowserHostImplExt::MediaPlayerGone(
    const content::WebContentsObserver::MediaPlayerInfo& video_type,
    const content::MediaPlayerId& id) {
  LOG(INFO) << "AlloyBrowserHostImplExt::MediaPlayerGone, is_video: " << video_type.has_video;
  cef_media_type_t type = video_type.has_video ? cef_media_type_t::VIDEO : cef_media_type_t::AUDIO;
  if (client_.get() && client_->AsArkWebClient()->GetMediaHandler().get()) {
    client_->AsArkWebClient()->GetMediaHandler()->OnMediaStateChanged(this, type, cef_media_playing_state_t::PLAYER_GONE);
  }
}
#endif


#if BUILDFLAG(ARKWEB_PRINT)

void AlloyBrowserHostImplExt::SetToken(void* token) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetToken(token);
  }
}

void AlloyBrowserHostImplExt::CreateWebPrintDocumentAdapter(const CefString& jobName,
                                                         void** webPrintDocumentAdapter) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->CreateWebPrintDocumentAdapter(jobName, webPrintDocumentAdapter);
  }
}

void AlloyBrowserHostImplExt::CreateWebPrintDocumentAdapterV2(
    const CefString& jobName, void** adapter) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->
      CreateWebPrintDocumentAdapterV2(jobName, adapter);
  }
}

void AlloyBrowserHostImplExt::SetPrintBackground(bool enable) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetPrintBackground(enable);
  }
}

bool AlloyBrowserHostImplExt::GetPrintBackground() {
  if (platform_delegate_) {
     return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->GetPrintBackground();
  }
  return false;
}
#endif // BUILDFLAG(ARKWEB_PRINT)


#if BUILDFLAG(ARKWEB_DISCARD)
bool AlloyBrowserHostImplExt::Discard() {
  if (!CEF_CURRENTLY_ON_UIT()) {
    LOG(ERROR)
        << "AlloyBrowserHostImplExt::Discard failed, called on invalid thread";
    return false;
  }
  if (!web_contents()) {
    LOG(ERROR)
        << "AlloyBrowserHostImplExt::Discard failed, WebContents is nullptr";
    return false;
  }

  content::RenderFrameHost* main_frame = web_contents()->GetPrimaryMainFrame();
  content::RenderProcessHost* render_process = main_frame->GetProcess();
  if (!render_process) {
    LOG(ERROR)
        << "AlloyBrowserHostImplExt::Discard failed, RenderProcessHost is nullptr";
    return false;
  }

  bool fast_shutdown_success = render_process->FastShutdownIfPossible(1u, true);
  if (fast_shutdown_success) {
    web_contents()->SetWasDiscarded(true);
  }

#if BUILDFLAG(ARKWEB_BFCACHE)
  content::NavigationController& controller = web_contents()->GetController();
  controller.GetBackForwardCache().Flush();
#endif

  return fast_shutdown_success;
}

bool AlloyBrowserHostImplExt::Restore() {
  if (!CEF_CURRENTLY_ON_UIT()) {
    LOG(ERROR)
        << "AlloyBrowserHostImplExt::Restore failed, called on invalid thread";
    return false;
  }
  if (!web_contents()) {
    LOG(ERROR)
        << "AlloyBrowserHostImplExt::Restore failed, WebContents is nullptr";
    return false;
  }

  web_contents()->GetController().SetNeedsReload();
  web_contents()->GetController().LoadIfNecessary();
  web_contents()->Focus();

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  needs_reload_ = false;
#endif

  return true;
}
#endif  // BUILDFLAG(ARKWEB_DISCARD)


#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void AlloyBrowserHostImplExt::OnShowToast(double duration,
                                       const std::string& toast) {
  if (!client_) {
    LOG(WARNING) << "client is nullptr when notify to show toast";
    return;
  }

  client_->OnShowToast(duration, toast);
}

void AlloyBrowserHostImplExt::OnShowVideoAssistant(
    const std::string& videoAssistantItems) {
  if (!client_) {
    LOG(WARNING) << "client is nullptr when notify to show video assistant";
    return;
  }

  client_->OnShowVideoAssistant(videoAssistantItems);
}

void AlloyBrowserHostImplExt::OnReportStatisticLog(const std::string& content) {
  if (!client_) {
    LOG(WARNING) << "client is nullptr when notify to report statistic log";
    return;
  }

  client_->OnReportStatisticLog(content);
}
#endif  // BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)


#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void AlloyBrowserHostImplExt::ShowRepostFormWarningDialog(
    content::WebContents* source) {
  contents_delegate_.ShowRepostFormWarningDialog(source);
}
#endif // ARKWEB_NETWORK_BASE

#if BUILDFLAG(ARKWEB_ADBLOCK)
void AlloyBrowserHostImplExt::OnAdsBlocked(
    const std::string& main_frame_url,
    const std::map<std::string, int32_t>& subresource_blocked,
    bool is_site_first_report) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnAdsBlocked(main_frame_url, subresource_blocked,
                                     is_site_first_report);
  }
}

bool AlloyBrowserHostImplExt::TrigAdBlockEnabledForSiteFromUi(
    const std::string& main_frame_url,
    int main_frame_tree_node_id) {
  if (platform_delegate_) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->TrigAdBlockEnabledForSiteFromUi(
        main_frame_url, main_frame_tree_node_id);
  }

  return false;
}
#endif


#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
bool AlloyBrowserHostImplExt::WebExtensionCheck(
    const std::string functionName,
    content::BrowserContext*& browser_context,
    content::WebContents*& web_contents) {
  web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << functionName << " get contents failed.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << functionName << " get contents failed.";
#endif
    return false;
  }

  browser_context = web_contents->GetBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << functionName << " get context failed.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << functionName << " get context failed.";
#endif
    return false;
  }

  // we need to get the original context to make sure we take the right context.
  browser_context = GetOriginalContext(browser_context);

  return true;
}

void AlloyBrowserHostImplExt::WebExtensionRegisterZoomObserver() {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("WebExtensionRegisterZoomObserver", browser_context, web_contents)) {
    return;
  }

  extensions::TabsWindowsAPI::Get(browser_context)->RegisterTabZoomObserver(web_contents);
}

void AlloyBrowserHostImplExt::WebExtensionUnregisterZoomObserver() {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("WebExtensionUnregisterZoomObserver", browser_context, web_contents)) {
    return;
  }

  extensions::TabsWindowsAPI::Get(browser_context)->UnregisterTabZoomObserver(web_contents);
}

void AlloyBrowserHostImplExt::WebExtensionTabUpdated(
    int tab_id,
    std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
    std::unique_ptr<NWebExtensionTab> tab) {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("TabUpdated", browser_context, web_contents)) {
    return;
  }

  extensions::TabsWindowsAPI::Get(browser_context)->TabUpdated(
      tab_id, web_contents, std::move(changeInfo), std::move(tab));
}

void AlloyBrowserHostImplExt::ExtensionSetTabId(int32_t tab_id) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    LOG(ERROR)
        << "AlloyBrowserHostImplExt::ExtensionSetTabId failed, called on invalid thread";
    return;
  }
 
  if (tab_id != tab_id_) {
    LOG(INFO) << "ExtensionSetTabId nweb_id_=" << nweb_id_
              << "; tab_id_=" << tab_id_ << "; tab_id=" << tab_id;
    tab_id_ = tab_id;
  }
}

int32_t AlloyBrowserHostImplExt::ExtensionGetTabId() {
  if (!CEF_CURRENTLY_ON_UIT()) {
    LOG(ERROR)
        << "AlloyBrowserHostImplExt::ExtensionGetTabId failed, called on invalid thread";
    return -1;
  }

  if (tab_id_ != -1) {
    return tab_id_;
  }

  int nweb_id = GetNWebId();
  if (nweb_id <= 0) {
    LOG(ERROR) << "ExtensionGetTabId error nweb_id=" << nweb_id;
    return -1;
  }
#if BUILDFLAG(ARKWEB_NWEB_EX)
  int32_t tab_id = NWebExtensionTabDispatcher::GetTabIdByNwebId(nweb_id);
  LOG(INFO) << "ExtensionGetTabId tab_id_=" << tab_id_
            << "; tab_id=" << tab_id;
  tab_id_ = tab_id;
#endif
  return tab_id_;
}

void AlloyBrowserHostImplExt::WebExtensionTabRemoved(
    int tab_id,
    bool isWindowClosing,
    int windowId) {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("TabRemoved", browser_context, web_contents)) {
    return;
  }

  extensions::TabsWindowsAPI::Get(browser_context)
      ->TabRemoved(tab_id, isWindowClosing, windowId, web_contents);
}

void AlloyBrowserHostImplExt::WebExtensionTabAttached(
    int tab_id,
    int new_position,
    int new_window_id) {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("TabAttached", browser_context, web_contents)) {
    return;
  }

  extensions::TabsWindowsAPI::Get(browser_context)
      ->TabAttached(tab_id, web_contents, new_position, new_window_id);
}

void AlloyBrowserHostImplExt::WebExtensionTabDetached(
    int tab_id,
    const std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("TabDetached", browser_context, web_contents)) {
    return;
  }

  extensions::TabsWindowsAPI::Get(browser_context)
      ->TabDetached(web_contents, tab_id, detachInfo->oldPosition,
      detachInfo->oldWindowId);
}

void AlloyBrowserHostImplExt::WebExtensionTabHighlighted(NWebExtensionTabHighlightInfo& highlightInfo) {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("TabHighlighted", browser_context, web_contents)) {
    return;
  }

  extensions::TabsWindowsAPI::Get(browser_context)
      ->TabHighlighted(web_contents, highlightInfo);
}

void AlloyBrowserHostImplExt::WebExtensionTabMoved(
    int tab_id,
    const std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("TabMoved", browser_context, web_contents)) {
    return;
  }

  std::unique_ptr<NWebExtensionTabMoveInfo> params = std::make_unique<NWebExtensionTabMoveInfo>(*moveInfo);
  extensions::TabsWindowsAPI::Get(browser_context)
      ->TabMoved(web_contents, tab_id, std::move(params));
}

void AlloyBrowserHostImplExt::WebExtensionTabReplaced(int32_t addedTabId, int32_t removedTabId) {
  content::BrowserContext* browser_context = nullptr;
  content::WebContents* web_contents = nullptr;
  if (!WebExtensionCheck("TabReplaced", browser_context, web_contents)) {
    return;
  }

  extensions::TabsWindowsAPI::Get(browser_context)
      ->TabReplaced(web_contents, addedTabId, removedTabId);
}

content::BrowserContext* GetGlobalBrowserContext() {
  CefRefPtr<CefRequestContext> request_context = CefRequestContext::GetGlobalContext();
  if (!request_context) {
    LOG(ERROR) << "request context is null";
    return nullptr;
  }

  CefRequestContextImpl* request_context_impl =
    static_cast<CefRequestContextImpl*>(request_context.get());
  CefBrowserContext* cef_browser_context = request_context_impl->GetBrowserContext();
  if (!cef_browser_context) {
    LOG(ERROR) << "cef browser context is null";
    return nullptr;
  }
  content::BrowserContext* browser_context = cef_browser_context->AsBrowserContext();
  return browser_context;
}

void AlloyBrowserHostImplExt::WebExtensionActionClicked(
    std::string extensionId,
    const NWebExtensionTab* tab) {
  content::BrowserContext* context = GetGlobalBrowserContext();
  if (!context) {
    LOG(ERROR) << "WebExtensionActionClicked context is null";
    return;
  }

#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (tab && tab->incognito) {
    content::BrowserContext* incognito_context = GetIncognitoContext(context);
    if (incognito_context) {
      context = incognito_context;
    } else {
      LOG(ERROR) << "get incognito context failed";
      return;
    }
  }
#endif // ARKWEB_NWEB_EX

  extensions::ExtensionActionDispatcher::Get(context)
      ->DispatchExtensionActionClickedWithCustomArgs(context, extensionId,
                                                     tab);
}

void AlloyBrowserHostImplExt::UpdatePinnedExtensionIds(
    content::BrowserContext* browser_context,
    std::string extensionId,
    bool isPinned) {
  auto storedPinnedIds =
      extensions::ExtensionPrefs::Get(browser_context)->GetPinnedExtensions();
 
  if (isPinned) {
    if (!base::Contains(storedPinnedIds, extensionId)) {
      storedPinnedIds.push_back(extensionId);
    }
  } else {
    auto iter = base::ranges::find(storedPinnedIds, extensionId);
    if (iter != storedPinnedIds.end()) {
      storedPinnedIds.erase(iter);
    }
  }
 
  extensions::ExtensionPrefs::Get(browser_context)->SetPinnedExtensions(storedPinnedIds);
}

void AlloyBrowserHostImplExt::WebExtensionActionPinnedStateChanged(
    std::string extensionId, bool isPinned) {
  content::BrowserContext* context = GetGlobalBrowserContext();
  if (!context) {
    LOG(ERROR) << "WebExtensionActionPinnedStateChanged context is null";
    return;
  }

#if BUILDFLAG(ARKWEB_NWEB_EX)
  UpdatePinnedExtensionIds(context, extensionId, isPinned);
#endif
    
  extensions::ExtensionActionDispatcher::Get(context)
      ->OnActionPinnedStateChanged(extensionId, isPinned);
}

void AlloyBrowserHostImplExt::WebExtensionActionShowPopup(
    int tabId, std::string extensionId) {
  content::BrowserContext* context = GetGlobalBrowserContext();
  if (!context) {
    LOG(ERROR) << "WebExtensionActionShowPopup context is null";
    return;
  }
 
  extensions::ExtensionActionDispatcher::Get(context)
      ->WebExtensionActionShowPopup(context, tabId, extensionId);
}

void AlloyBrowserHostImplExt::WebExtensionSetViewType(int32_t type) {
  content::WebContents* web_contents = GetWebContents();
  extensions::SetViewType(web_contents, static_cast<extensions::mojom::ViewType>(type));

  int windowsId = extensions::GetCurrentWindowId(web_contents, extension_misc::kCurrentWindowId);
  if (auto* ewco = extensions::ExtensionWebContentsObserver::GetForWebContents(web_contents)) {
      web_contents->ForEachRenderFrameHost([ewco, windowsId](content::RenderFrameHost* frame_host) {
      if (auto local_frame = ewco->GetLocalFrame(frame_host)) {
          local_frame->UpdateBrowserWindowId(windowsId);
      }
    });
  }
}

content::BrowserContext* AlloyBrowserHostImplExt::GetOriginalContext(
    content::BrowserContext* browser_context) {
  if (!browser_context->IsOffTheRecord()) {
    return browser_context;
  }

  extensions::ExtensionsBrowserClient* browser_client =
      extensions::ExtensionsBrowserClient::Get();
  content::BrowserContext* original_browser_context =
      browser_client->GetOriginalContext(browser_context);
  if (!original_browser_context) {
    LOG(ERROR) << "get original_browser_context failed.";
    return browser_context;
  }

  if (!extensions::TabsWindowsAPI::Get(original_browser_context)) {
    LOG(ERROR) << "get TabsWindowsAPI form original_browser_context failed.";
    return browser_context;
  }

  return original_browser_context;
}
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)


#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
void AlloyBrowserHostImplExt::OnBeforeUnloadFired(bool proceed) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnBeforeUnloadFired(proceed);
  }
}
#endif // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void AlloyBrowserHostImplExt::OnShareFile(const std::string& filePath,
    const std::string& utdTypeId) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnShareFile(filePath, utdTypeId);
  }
}

void AlloyBrowserHostImplExt::NavigationStateChanged(
    content::WebContents* source,
    content::InvalidateTypes changed_flags) {
  contents_delegate_.NavigationStateChanged(source, changed_flags);
}

content::NavigationController::UserAgentOverrideOption
AlloyBrowserHostImplExt::ShouldOverrideUserAgentForPrerender2() {
  // For WebView, always use the user agent override, which is set every time
  // the user agent in AwSettings is modified.
  return content::NavigationController::UA_OVERRIDE_TRUE;
}

bool AlloyBrowserHostImplExt::ShouldAllowPartialParamMismatchOfPrerender2(
    content::NavigationHandle& navigation_handle) {
  // We relax initiator checks on WebView first, but continue to discuss.
  //
  // TODO(https://crbug.com/340416082): Relax initiator check for all platforms.

  // `ui::PAGE_TRANSITION_FROM_API` bit distinguishes that the activation
  // navigation is triggered by `WebView.loadUrl()`.
  bool ret =
      navigation_handle.GetPageTransition() & ui::PAGE_TRANSITION_FROM_API;
  if (ret) {
    CHECK(!navigation_handle.GetInitiatorFrameToken().has_value());
    CHECK(!navigation_handle.GetInitiatorOrigin().has_value());
  }
  return ret;
}
#endif


#if BUILDFLAG(ARKWEB_DATALIST)
void AlloyBrowserHostImplExt::OnShowAutofillPopup(
    const gfx::RectF& element_bounds,
    bool is_rtl,
    const std::vector<autofill::Suggestion>& suggestions,
    bool is_password_popup_type) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnShowAutofillPopup(element_bounds, is_rtl, suggestions,
                                            is_password_popup_type);
  }
}

void AlloyBrowserHostImplExt::OnHideAutofillPopup() {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnHideAutofillPopup();
  }
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void AlloyBrowserHostImplExt::RequestPointerLock(
    content::WebContents* web_contents,
    bool user_gesture,
    bool last_unlocked_by_target) {
  contents_delegate_.RequestPointerLock(web_contents, user_gesture, last_unlocked_by_target);
}

void AlloyBrowserHostImplExt::LostPointerLock() {
  contents_delegate_.LostPointerLock();
}
#endif


#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
void AlloyBrowserHostImplExt::OpenDateTimeChooser() {
  content::DateTimeChooserOHOS* date_time_chooser =
      content::DateTimeChooserOHOS::FromWebContents(web_contents());
  if (date_time_chooser && client_) {
    if (auto handler = client_->GetDialogHandler()) {
      blink::mojom::DateTimeDialogValuePtr& date_time_dialog_ptr =
          date_time_chooser->GetDialogValue();
      if (!date_time_dialog_ptr) {
        LOG(ERROR) << "OpenDateTime chooser failed";
        date_time_chooser->NotifyResult(false, 0);
        return;
      }
      CefDateTimeChooser chooser(
          static_cast<cef_text_input_type_t>(date_time_dialog_ptr->dialog_type),
          date_time_dialog_ptr->dialog_value, date_time_dialog_ptr->minimum,
          date_time_dialog_ptr->maximum, date_time_dialog_ptr->step);
      CefRefPtr<CefDateTimeChooserCallback> callback =
          new CefDateTimeChooserCallbackImpl(date_time_chooser->GetWeakPtr());
      std::vector<CefDateTimeSuggestion> suggestions;
      for (size_t index = 0; index < date_time_dialog_ptr->suggestions.size();
           index++) {
        CefDateTimeSuggestion sug;
        CefString label;
        CefString localized_value;
        label.FromString16(date_time_dialog_ptr->suggestions[index]->label);
        localized_value.FromString16(
            date_time_dialog_ptr->suggestions[index]->localized_value);
        sug.value = date_time_dialog_ptr->suggestions[index]->value;
        cef_string_set(label.c_str(), label.length(), &(sug.label), true);
        cef_string_set(localized_value.c_str(), localized_value.length(),
                       &(sug.localized_value), true);
        suggestions.push_back(sug);
      }
      handler->OnDateTimeChooserPopup(this, chooser, suggestions, callback);
    }
  }
}

void AlloyBrowserHostImplExt::CloseDateTimeChooser() {
  if (client_) {
    if (auto handler = client_->GetDialogHandler()) {
      handler->OnDateTimeChooserClose();
    }
  }
}
#endif  // #if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
std::unique_ptr<content::CustomMediaPlayer>
AlloyBrowserHostImplExt::CreateCustomMediaPlayer(
    std::unique_ptr<content::CustomMediaPlayerListener> listener,
    const content::MediaInfo& media_info) {
  CefOwnPtr<CefCustomMediaPlayerDelegate> delegate;
  static_assert(sizeof(CefCustomMediaInfo) == sizeof(content::MediaInfo));
  CefCustomMediaInfo cef_media_info;

  cef_media_info.embed_id = media_info.embed_id;
  cef_media_info.media_type = static_cast<uint32_t>(media_info.media_type);
  cef_media_info.media_src_list.reserve(media_info.media_src_list.size());
  for (const auto& info : media_info.media_src_list) {
    cef_media_info.media_src_list.push_back({
        static_cast<uint32_t>(info.source_type),
        info.media_source, info.media_format});
  }
  cef_media_info.surface_info.id = media_info.surface_info.id;
  cef_media_info.surface_info.x = media_info.surface_info.x;
  cef_media_info.surface_info.y = media_info.surface_info.y;
  cef_media_info.surface_info.width = media_info.surface_info.width;
  cef_media_info.surface_info.height = media_info.surface_info.height;

  cef_media_info.controls = media_info.controls;
  cef_media_info.controlslist.reserve(media_info.controlslist.size());
  for (const auto& item : media_info.controlslist) {
    cef_media_info.controlslist.push_back(item);
  }

  cef_media_info.muted = media_info.muted;
  cef_media_info.poster_url = media_info.poster_url;
  cef_media_info.preload = static_cast<uint32_t>(media_info.preload);
  cef_media_info.https_headers = media_info.https_headers;
  cef_media_info.attributes = media_info.attributes;

  if (!client_ || !client_->AsArkWebClient()) {
    LOG(ERROR) << "client is null, CreateCustomMediaPlayer failed";
    return nullptr;
  }
  delegate = client_->AsArkWebClient()->OnCreateCustomMediaPlayer(
      CefOwnPtr<CefMediaPlayerListenerImpl>(
          new CefMediaPlayerListenerImpl(std::move(listener))),
      cef_media_info);
  if (!delegate) {
    LOG(ERROR) << "CreateCustomMediaPlayer, no media player";
    return nullptr;
  }
  return std::make_unique<CustomMediaPlayerProxy>(std::move(delegate));

}
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void AlloyBrowserHostImplExt::UpdateVSyncFrequency() {
  if (!base::ohos::IsMobileDevice()) {
    LOG(DEBUG) << " VSync  adjustment is only available for mobile deive";
    return;
  }

  if (!has_video_playing_) {
    LOG(DEBUG) << "UpdateVSyncFrequency Fail due to no video playing";
    return;
  }

  if (has_touch_event_) {
    LOG(DEBUG) << "UpdateVSyncFrequency Fail due to touch event";
    has_touch_event_ = false;
    return;
  }

  auto wc = web_contents();
  if (!wc) {
    LOG(ERROR) << "UpdateVSyncFrequency Fail due to no WebContents";
    return;
  }

  auto rvh = wc->GetRenderViewHost();
  if (rvh && rvh->GetWidget()) {
    ArkWebRenderWidgetHostViewOSRExt* view =
        static_cast<ArkWebRenderWidgetHostViewOSRExt*>(rvh->GetWidget()->GetView());
    if(view) {
      LOG(INFO) << "AlloyBrowserHostImplExt::UpdateVSyncFrequency";
      view->AsArkWebRenderWidgetHostViewOSRExt()->UpdateVSyncFrequency();
      set_lower_frame_rate_ = true;
    }
  }
}

void AlloyBrowserHostImplExt::ResetVSyncFrequency() {
  if (!base::ohos::IsMobileDevice()) {
    LOG(DEBUG) << "VSync adjustment is only available for mobile deive";
    return;
  }

  auto wc = web_contents();
  if (!wc) {
    LOG(ERROR) << "ResetVSyncFrequency Fail due to no WebContents";
    return;
  }

  auto rvh = wc->GetRenderViewHost();
  if (rvh && rvh->GetWidget()) {
    ArkWebRenderWidgetHostViewOSRExt* view =
        static_cast<ArkWebRenderWidgetHostViewOSRExt*>(rvh->GetWidget()->GetView());
    if(view) {
      LOG(INFO) << "AlloyBrowserHostImplExt::ResetVSyncFrequency";
      view->AsArkWebRenderWidgetHostViewOSRExt()->ResetVSyncFrequency();
      has_touch_event_ = false;
      set_lower_frame_rate_ = false;
    }
  }
}
#endif


#if BUILDFLAG(ARKWEB_CLIPBOARD)
bool AlloyBrowserHostImplExt::HandleContextMenu(
    content::RenderFrameHost& render_frame_host,
    const content::ContextMenuParams& params) {
  // This bool value is only used for touch insert handle quick menu.
  auto rvh = web_contents()->GetRenderViewHost();
  CefRenderWidgetHostViewOSR* view =
      static_cast<CefRenderWidgetHostViewOSR*>(rvh->GetWidget()->GetView());
  if (view) {
    CefTouchSelectionControllerClientOSR* touch_client =
        static_cast<CefTouchSelectionControllerClientOSR*>(
            view->selection_controller_client());
    if (touch_client && touch_client->HandleContextMenu(params)) {
      return true;
    }
  }

  if (!menu_manager_.get() && platform_delegate_) {
    menu_manager_.reset(
        new CefMenuManager(this, platform_delegate_->CreateMenuRunner()));
  }

  if (menu_manager_->CreateContextMenu(params)) {
    view->ResetGestureDetection(true);
    return true;
  }
  return false;
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)


#if BUILDFLAG(ARKWEB_HTML_SELECT)
void AlloyBrowserHostImplExt::ShowPopupMenu(
    mojo::PendingRemote<blink::mojom::PopupMenuClient> popup_client,
    const gfx::Rect& bounds,
    int item_height,
    double item_font_size,
    int selected_item,
    std::vector<blink::mojom::MenuItemPtr> menu_items,
    bool right_aligned,
    bool allow_multiple_selection) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->ShowPopupMenu(std::move(popup_client), bounds,
                                      item_height, item_font_size,
                                      selected_item, std::move(menu_items),
                                      right_aligned, allow_multiple_selection);
  }
}
#endif  // #if BUILDFLAG(ARKWEB_HTML_SELECT)


#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void AlloyBrowserHostImplExt::WasKeyboardResized() {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&AlloyBrowserHostImplExt::WasKeyboardResized, this));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->WasKeyboardResized();
  }
}

void AlloyBrowserHostImplExt::SetDrawRect(int x, int y, int width, int height) {
  if (platform_delegate_)
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetDrawRect(x, y, width, height);
}

void AlloyBrowserHostImplExt::SetDrawMode(int mode) {
  if (platform_delegate_)
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetDrawMode(mode);
}

void AlloyBrowserHostImplExt::SetFitContentMode(int mode) {
  if (platform_delegate_)
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetFitContentMode(mode);
}

bool AlloyBrowserHostImplExt::GetPendingSizeStatus() {
  if (platform_delegate_) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->GetPendingSizeStatus();
  }
  return false;
}

void AlloyBrowserHostImplExt::SetShouldFrameSubmissionBeforeDraw(bool should) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(
                      &AlloyBrowserHostImplExt::SetShouldFrameSubmissionBeforeDraw,
                      this, should));
    return;
  }

  if (platform_delegate_)
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetShouldFrameSubmissionBeforeDraw(should);
}

std::string AlloyBrowserHostImplExt::GetCurrentLanguage() {
  if (!platform_delegate_) {
    return "";
  }
  return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->GetCurrentLanguage();
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)


#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
std::unique_ptr<content::VideoAssistant>
AlloyBrowserHostImplExt::CreateVideoAssistant() {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  return std::make_unique<nweb_ex::VideoAssistant>(this);
#else
  return content::WebContentsDelegate::CreateVideoAssistant();
#endif // ARKWEB_NWEB_EX
}

void AlloyBrowserHostImplExt::PopluateVideoAssistantConfig(
    const std::string& url,
    media::mojom::VideoAssistantConfigPtr& config) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  auto cloud_control = nweb_ex::AlloyBrowserEngineCloudConfig::GetInstance();
  nweb_ex::BrowserEngineCloudControlConfig cloud_config;
  bool match = cloud_control->GetControlConfigByUrl(url, cloud_config);
  if (!match) {
    config->video_assistant = true;
    config->playback_rate = true;
    config->download_button =
        media::mojom::VideoAssistantDownloadButton::kDownloadPerPage;
    return;
  }
  config->video_assistant = cloud_config.videoAssistant;
  config->playback_rate = cloud_config.playbackRate;
  switch (cloud_config.downloadBtn) {
    case nweb_ex::DownloadBtn::kDownloadPerPage:
      config->download_button =
          media::mojom::VideoAssistantDownloadButton::kDownloadPerPage;
      break;
    case nweb_ex::DownloadBtn::kDownloadForceShow:
      config->download_button =
          media::mojom::VideoAssistantDownloadButton::kDownloadForceShow;
      break;
    case nweb_ex::DownloadBtn::kDownloadForceHide:
      config->download_button =
          media::mojom::VideoAssistantDownloadButton::kDownloadForceHide;
      break;
  }
#endif // ARKWEB_NWEB_EX
}
#endif // ARKWEB_VIDEO_ASSISTANT


void AlloyBrowserHostImplExt::OnWindowShow() {
  TRACE_EVENT0("base", "AlloyBrowserHostImplExt::OnWindowShow");
  LOG(DEBUG) << "AlloyBrowserHostImplExt::OnWindowShow";
#if BUILDFLAG(ARKWEB_REPORT_RENDER_STATE)
  RenderProcessStateHandler::GetInstance()->UpdateRenderProcessState(implUtils->GetRenderProcessId(), nweb_id_, false);
#endif
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  SetVisible(nweb_id_, true);
#endif
}

void AlloyBrowserHostImplExt::OnWindowHide() {
  TRACE_EVENT0("base", "AlloyBrowserHostImplExt::OnWindowHide");
  LOG(DEBUG) << "AlloyBrowserHostImplExt::OnWindowHide";
#if BUILDFLAG(ARKWEB_REPORT_RENDER_STATE)
  RenderProcessStateHandler::GetInstance()->UpdateRenderProcessState(implUtils->GetRenderProcessId(), nweb_id_, true);
#endif
  SetVisible(nweb_id_, false);
}


void AlloyBrowserHostImplExt::OnOnlineRenderToForeground() {
}

void AlloyBrowserHostImplExt::SetWindowId(int window_id, int nweb_id) {
  window_id_ = window_id;
  nweb_id_ = nweb_id;
  OHOS::NWeb::ResSchedClientAdapter::ReportWindowId(static_cast<int32_t>(window_id), static_cast<int32_t>(nweb_id));
  ReportWindowStatus(false);
}

void AlloyBrowserHostImplExt::RenderViewReady() {
  RenderProcessStateHandler::GetInstance()->InitRenderProcessState(implUtils->GetRenderProcessId(), nweb_id_);
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&AlloyBrowserHostImplExt::ReportWindowStatus, this, true));
    return;
  }
  ReportWindowStatus(true);
#if BUILDFLAG(ARKWEB_ZOOM)
  implUtils->UpdateZoomSupportEnabled();
#endif
}

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void AlloyBrowserHostImplExt::ReportWindowStatus(bool first_view_ready) {
  using namespace OHOS::NWeb;
  if (first_view_ready && is_hidden_) {
    LOG(INFO) << "no need to report render view ready because the view is hidden";
    return;
  }

  content::WebContents* contents = web_contents();
  if (!contents) {
    LOG(ERROR) << "AlloyBrowserHostImplExt::ReportWindowStatus web_contents is null";
    return;
  }

  if (auto render_view_host = contents->GetRenderViewHost()) {
    auto render_process_host = render_view_host->GetProcess();
    if (!render_process_host) {
      LOG(ERROR) << "AlloyBrowserHostImplExt::ReportWindowStatus render_process_host is null";
      return;
    }

    ResSchedStatusAdapter status = is_hidden_
                                       ? ResSchedStatusAdapter::WEB_INACTIVE
                                       : ResSchedStatusAdapter::WEB_ACTIVE;
    base::ProcessId process_id = render_process_host->GetProcess().Pid();
    InactiveUnloadOldProcess(process_id);
    ResSchedClientAdapter::ReportWindowStatus(status, process_id, window_id_,
                                              nweb_id_);
    if (!is_hidden_) {
      ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                         ResSchedSceneAdapter::VISIBLE,
                                         nweb_id_);
    }
  } else {
    LOG(ERROR) << "AlloyBrowserHostImplExt::ReportWindowStatus render_view_host is null";
    return;
  }
}
#endif

#if BUILDFLAG(IS_OHOS)
void AlloyBrowserHostImplExt::SetVisible(int32_t nweb_id, bool visible)
{
  auto strategy = -1;
  if (visible && ltpo_strategy_ < 0) {
    strategy = OHOS::NWeb::OhosAdapterHelper::GetInstance().
      GetSystemPropertiesInstance().GetLTPOStrategy();
    ltpo_strategy_ = strategy;
  }
  if (auto* host = content::GpuProcessHost::Get()) {
    if (auto* host_impl = host->gpu_host()) {
      if (strategy >= 0) {
        host_impl->SetLTPOStrategy(ltpo_strategy_);
      }
      TRACE_EVENT2("base", "AlloyBrowserHostImplExt::SetVisible", "nweb_id", nweb_id, "visible", visible);
      LOG(INFO) << "AlloyBrowserHostImplExt::SetVisible nweb_id: " << nweb_id << ", visible " << visible;
      host_impl->SetVisible(nweb_id, visible);
    }
  }
}

void AlloyBrowserHostImplExt::InactiveUnloadOldProcess(base::ProcessId pid) {
  using namespace OHOS::NWeb;
  if(pid != last_pid_ && last_pid_ != -1) {
    ResSchedClientAdapter::ReportWindowStatus(ResSchedStatusAdapter::WEB_INACTIVE,
                                              last_pid_, window_id_, nweb_id_);
  }
  last_pid_ = pid;
}
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
void AlloyBrowserHostImplExt::OnDumpJavaScriptStackCallback(
    int pid,
    content::RendererIsUnresponsiveReason reason,
    const std::string& stack) {
  if (client_) {
    if (auto handler = client_->GetRequestHandler()) {
      int anr_reason = static_cast<int>(
          reason != content::RendererIsUnresponsiveReason::kOnInputEventAckTimeout
              ? content::RendererIsUnresponsiveReason::
                    kNavigationRequestCommitTimeout
              : content::RendererIsUnresponsiveReason::kOnInputEventAckTimeout);
      handler->AsCefRequestHandlerExt()->OnRenderProcessNotResponding(
          this, stack, pid, anr_reason);
    }
  }
}
#endif


#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
content::WebContents* AlloyBrowserHostImplExt::GetWebContentsForExtension() {
  return web_contents();
}

void AlloyBrowserHostImplExt::AcceleratorPressedUI(
    const ui::Accelerator& accelerator,
    content::BrowserContext* browser_context) {
  CefExtensionKeybindingRegistryViews cef_key_view(browser_context, this);
  cef_key_view.AcceleratorPressed(accelerator);
}

bool AlloyBrowserHostImplExt::WebHandleKeyboardEvent(
    content::WebContents* source,
    const input::NativeWebKeyboardEvent& event) {
  // Check to see if event should be ignored.
  if (event.skip_if_unhandled) {
    return false;
  }

  if (contents_delegate_.HandleKeyboardEvent(source, event)) {
    return true;
  }

  if (source) {
    content::BrowserContext* browser_context = source->GetBrowserContext();
    extensions::ExtensionCommandsGlobalRegistry* registry =
        extensions::ExtensionCommandsGlobalRegistry::Get(browser_context);
    if (registry) {
      registry->registry_for_active_window();
    }
 
    bool run_accelerator_flag = true;
    ui::Accelerator accelerator =
        ui::GetAcceleratorFromNativeWebKeyboardEvent(event);
    ui::KeyEvent key_event = accelerator.ToKeyEvent();
    const ui::EventType type = key_event.type();
    if (run_accelerator_flag && (type == ui::EventType::kKeyPressed &&
        event.GetType() != blink::WebKeyboardEvent::Type::kChar)) {
      run_accelerator_flag = true;
    } else {
      run_accelerator_flag = false;
    }
 
    if (run_accelerator_flag) {
      if (!CEF_CURRENTLY_ON_UIT()) {
        CEF_POST_TASK(
            CEF_UIT,
            base::BindOnce(&AlloyBrowserHostImplExt::AcceleratorPressedUI, this,
                           accelerator, browser_context));
      } else {
        AcceleratorPressedUI(accelerator, browser_context);
      }
    }
  }
  
  if (platform_delegate_ && platform_delegate_->HandleKeyboardEvent(event)) {
    return true;
  }

  bool zoom_in;
  if (IsNeedZoomChange(event, zoom_in)) {
    LOG(DEBUG) << "ContentsZoomChange when HandleKeyboardEvent";
    ContentsZoomChange(zoom_in);
    return true;
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
void AlloyBrowserHostImplExt::NotifyScreenInfoChangedV2()
{
  if (!IsWindowless()) {
    DCHECK(false) << "Window rendering is not disabled";
    return;
  }
 
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&AlloyBrowserHostImplExt::NotifyScreenInfoChangedV2, this));
    return;
  }
 
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->NotifyScreenInfoChangedV2();
  }
#if BUILDFLAG(IS_OHOS)
  base::ohos::SlidingObserver::GetInstance().OnDisplayInfoChange();
#endif
}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)

void AlloyBrowserHostImplExt::SetAudioResumeInterval(int resumeInterval) {
  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(web_contents());
  if (!mediaSession) {
    LOG(ERROR) << "AlloyBrowserHostImpl::SetAudioResumeInterval get mediaSession failed.";

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "AlloyBrowserHostImpl::SetAudioResumeInterval get mediaSession failed.";
#endif

    return;
  }
  mediaSession->audioResumeInterval_ = resumeInterval;
}

void AlloyBrowserHostImplExt::SetAudioExclusive(bool audioExclusive) {
  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(web_contents());
  if (!mediaSession) {
    LOG(ERROR) << "AlloyBrowserHostImplExt::SetAudioExclusive get mediaSession failed.";
 
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "AlloyBrowserHostImplExt::SetAudioExclusive get mediaSession failed.";
#endif
 
    return;
  }
  mediaSession->audioExclusive_ = audioExclusive;
}

void AlloyBrowserHostImplExt::SetAudioSessionType(int audioSessionType) {
  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(web_contents());
  if (!mediaSession) {
    LOG(ERROR) << "AlloyBrowserHostImpl::SetAudioSessionType get mediaSession failed.";
    return;
  }
  mediaSession->audioSessionType_ = audioSessionType;
}
#endif // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
bool AlloyBrowserHostImplExt::OnStartBackgroundTask(
    int32_t type,
    const std::string& message) {
  if (platform_delegate_ &&
      platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()
        ->OnStartBackgroundTask(type, message);
  }
  return true;
}
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
std::unique_ptr<content::MediaPlayerListener>
AlloyBrowserHostImplExt::OnFullScreenOverlayEnter(
    media::mojom::MediaInfoForVASTPtr media_info_ptr,
    const content::MediaPlayerId& media_player_id) {
  if (!client_) {
    LOG(WARNING) << "client is null, OnFullScreenOverlayEnter failed";
    return nullptr;
  }

  if (!GetWebContents()) {
    return nullptr;
  }

  std::unique_ptr<CefMediaPlayerListenerForVAST> listener;
#if BUILDFLAG(ARKWEB_NWEB_EX)
  auto config = media::mojom::VideoAssistantConfig::New(true, true,
      media::mojom::VideoAssistantDownloadButton::kDownloadPerPage);
  auto url =
      GetWebContents()->GetLastCommittedURL().DeprecatedGetOriginAsURL().spec();
  PopluateVideoAssistantConfig(url, config);

  auto media_info = BuildMediaInfo(media_info_ptr, config);
  auto media_player_controller =
      std::make_unique<nweb_ex::MediaPlayerControllerImpl>(
          this, media_player_id, std::move(media_info_ptr), std::move(config));

  listener = client_->OnFullScreenOverlayEnter(
      std::make_unique<CefMediaPlayerControllerImpl>(
          std::move(media_player_controller)),
      media_info);
#endif // ARKWEB_NWEB_EX
  if (!listener) {
    return nullptr;
  }
  return std::make_unique<MediaPlayerListenerProxy>(std::move(listener));
}
#endif  // BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)

#if BUILDFLAG(ARKWEB_READER_MODE)
void AlloyBrowserHostImplExt::OnIsPageDistillable(int page_type,
                                            const std::string& distillable_page_url, const std::string& title) {
  if (!client_ || !client_->AsArkWebClient()) {
    LOG(ERROR) << "client is null, OnIsPageDistillable failed";
    return nullptr;
  }
  client_->AsArkWebClient()->OnIsPageDistillable(page_type, distillable_page_url, title);
}

bool AlloyBrowserHostImplExt::IsForDistillerPage() {
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
bool AlloyBrowserHostImplExt::IsURLBlockedInIncognito(
    bool is_guest_view,
    const content::OpenURLParams& params) {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableNwebEx)) {
    return false;
  }
 
  if (!is_guest_view &&
      (params.url.SchemeIs(content::kChromeUIScheme)
      || params.url.SchemeIs(content::kArkWebUIScheme)) &&
      web_contents()->GetBrowserContext() &&
      web_contents()->GetBrowserContext()->IsOffTheRecord() &&
      IsBlockedWebUIHostInIncognito(params.url.host_piece())) {
    // Block navigation to non-allowlisted WebUI pages.
    LOG(WARNING) << "Navigation is blocked in Alloy-style browser.";
    return true;
  }
  
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  std::string AlloyBrowserHostImplExt::OnRewriteUrlForNavigation(const std::string& original_url,
                                                                 const std::string& referrer) {
  if (!client_ || !client_->AsArkWebClient()) {
    LOG(ERROR) << "client is null, OnRewriteUrlForNavigation failed";
    return "";
  }
  return client_->AsArkWebClient()->OnRewriteUrlForNavigation(original_url, referrer);
}
#endif