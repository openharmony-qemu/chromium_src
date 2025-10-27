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
#include "libcef/browser/alloy/render_process_state_handler.h"
#include "net/base/net_errors.h"
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

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
#include "content/browser/site_instance_impl.h"
#endif
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
#endif  // ARKWEB_NWEB_EX
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
#include "content/browser/media/session/media_session_impl.h"
#endif // defined(OHOS_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_IMMEDIATE_DESTORY)
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/browser/site_info.h"
#endif

#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_utils.h"
#if BUILDFLAG(ARKWEB_CLIPBOARD)
void AlloyBrowserHostImplUtils::UpdateZoomSupportEnabled() {
  auto rvh = alloyBrowserHostImpl->web_contents()->GetRenderViewHost();
  ArkWebRenderWidgetHostViewOSRExt* view =
        static_cast<ArkWebRenderWidgetHostViewOSRExt*>(rvh->GetWidget()->GetView());
  if (view) {
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetDoubleTapSupportEnabled(alloyBrowserHostImpl->settings_.supports_double_tap_zoom);
    view->AsArkWebRenderWidgetHostViewOSRExt()->SetMultiTouchZoomSupportEnabled(alloyBrowserHostImpl->settings_.supports_multi_touch_zoom);
  }
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)


#if BUILDFLAG(ARKWEB_REPORT_RENDER_STATE)
base::ProcessId AlloyBrowserHostImplUtils::GetRenderProcessId() {
  content::WebContents* contents = alloyBrowserHostImpl->web_contents();
  if (!contents) {
    LOG(ERROR) << "AlloyBrowserHostImplUtils::GetRenderProcessId web_contents is null";
    return 0;
  }

  if (auto render_view_host = contents->GetRenderViewHost()) {
    auto render_process_host = render_view_host->GetProcess();
    if (!render_process_host) {
      LOG(ERROR) << "AlloyBrowserHostImplUtils::GetRenderProcessId render_process_host is null";
      return 0;
    }
    return render_process_host->GetProcess().Pid();
  } else {
    LOG(ERROR) << "AlloyBrowserHostImplUtils::GetRenderProcessId render_view_host is null";
    return 0;
  }
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void AlloyBrowserHostImplUtils::manageVSyncFrequencyOnTouchEvent(const CefTouchEvent& event){
  if (event.type == CEF_TET_PRESSED) {
    alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()->has_touch_event_ = true;
    if (alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()->set_lower_frame_rate_) {
      alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()->ResetVSyncFrequency();
    }
  }

  if (event.type == CEF_TET_RELEASED) {
    if (alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()->has_video_playing_ && !alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()->set_lower_frame_rate_) {
      CEF_POST_DELAYED_TASK(CEF_UIT, base::BindOnce(&AlloyBrowserHostImplExt::UpdateVSyncFrequency,
                                                    alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()), alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()->WAIT_TOUCH_EVENT_DELAY_TIME);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
void AlloyBrowserHostImplUtils::commitPendingZoomLevelPreferences() {
  if (alloyBrowserHostImpl->web_contents()) {
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kEnableNwebExGetZoomLevel)) {
      auto* browser_context = CefBrowserContext::FromBrowserContext(
          alloyBrowserHostImpl->web_contents()->GetBrowserContext());
      if (browser_context) {
        DCHECK(browser_context->AsProfile()->GetPrefs());
        browser_context->AsProfile()->GetPrefs()->CommitPendingWrite();
      }
    }
    // WebContentsDestroyed();
  }
}
#endif


#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
int AlloyBrowserHostImplUtils::handleZoomEventWithInput(bool zoom_in) {
  if (alloyBrowserHostImpl && !alloyBrowserHostImpl->settings_.zoom_control_access) {
    return -1;
  }
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx)) {
    content::PageZoom zoomType = zoom_in ? content::PageZoom::PAGE_ZOOM_IN
                                         : content::PageZoom::PAGE_ZOOM_OUT;
    zoom::PageZoom::Zoom(alloyBrowserHostImpl->web_contents(), zoomType);
    return -1;
  }

  double curFactor = alloyBrowserHostImpl->GetZoomLevel();
  double tempZoomFactor = zoom_in ? curFactor + 2.0 : curFactor - 2.0;
  if (tempZoomFactor > 10.0 || tempZoomFactor < -10.0) {
    LOG(ERROR) << "The mouse wheel event can no longer be zoomed in or out.";
    return -1;
  }
  alloyBrowserHostImpl->SetZoomLevel(tempZoomFactor);

  return 0;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
void AlloyBrowserHostImplUtils::enableZoomLevelController(content::WebContents* web_contents) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExGetZoomLevel)) {
    zoom::ZoomController::CreateForWebContents(web_contents);
  }
}
#endif


#if BUILDFLAG(ARKWEB_WEBRTC) && BUILDFLAG(ARKWEB_PERMISSION)
int AlloyBrowserHostImplUtils::conditionalHandleMediaStreamPermissionRequest(
  content::MediaResponseCallback callback,
  const content::MediaStreamRequest& request) {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableMediaStream)) {
    // Cancel the request.
    std::move(callback).Run(blink::mojom::StreamDevicesSet(),
                            blink::mojom::MediaStreamRequestResult::PERMISSION_DENIED,
                            std::unique_ptr<content::MediaStreamUI>());
    return -1;
  }
  bool microphone_requested =
      (request.audio_type ==
       blink::mojom::MediaStreamType::DEVICE_AUDIO_CAPTURE);
  bool webcam_requested = (request.video_type ==
                           blink::mojom::MediaStreamType::DEVICE_VIDEO_CAPTURE);
  bool screen_requested = (request.video_type ==
                           blink::mojom::MediaStreamType::GUM_DESKTOP_VIDEO_CAPTURE) ||
                          (request.video_type ==
                           blink::mojom::MediaStreamType::DISPLAY_VIDEO_CAPTURE);
  LOG(INFO) << "RequestMediaAccessPermission screen_requested: " << screen_requested;
  AlloyPermissionRequestHandler* permission_handler = alloyBrowserHostImpl->GetPermissionRequestHandler();
  if (!permission_handler) {
    // Cancel the request.
    std::move(callback).Run(blink::mojom::StreamDevicesSet(),
                            blink::mojom::MediaStreamRequestResult::PERMISSION_DENIED,
                            std::unique_ptr<content::MediaStreamUI>());
    return -1;
  }
  if (!screen_requested && (microphone_requested || webcam_requested)) {
    permission_handler->SendRequest(new AlloyMediaAccessRequest(alloyBrowserHostImpl, request, std::move(callback)));
  } else {
    permission_handler->SendScreenCaptureRequest(new AlloyScreenCaptureAccessRequest(alloyBrowserHostImpl, request, std::move(callback)));
  }

  return 0;
}
#endif // BUILDFLAG(ARKWEB_WEBRTC) && BUILDFLAG(ARKWEB_PERMISSION)


#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool AlloyBrowserHostImplUtils::handleAndReDispatchKeyboardEvent(content::WebContents* source, const input::NativeWebKeyboardEvent& event) {
  bool isUsed = alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()->WebHandleKeyboardEvent(source, event);
  CefRefPtr<CefKeyboardHandler> handler;
  CefKeyEvent cef_event;
  GetCefKeyEvent(event, cef_event);
  if (alloyBrowserHostImpl->client_) {
    handler = alloyBrowserHostImpl->client_->GetKeyboardHandler();
  }
  if (handler) {
    handler->KeyboardReDispatch(cef_event, isUsed);
  }
  return isUsed;
}
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
int AlloyBrowserHostImplUtils::handleRendererUnresponsive(content::WebContents* source, content::RendererIsUnresponsiveReason reason) {
  LOG(INFO) << "AlloyBrowserHostImplUtils::RendererUnresponsive";
  content::RenderProcessHost* host =
      source->GetPrimaryMainFrame()->GetProcess();
  if (!host->IsReady() || !source->GetPrimaryMainFrame()->IsRenderFrameLive()) {
    LOG(INFO) << "AlloyBrowserHostImplUtils::RendererUnresponsive not readey for render dump";
    alloyBrowserHostImpl->AsAlloyBrowserHostImplExt()->OnDumpJavaScriptStackCallback(host->GetProcess().Pid(), reason, "");
    return -1;
  }
  host->InvokeRenderCrashDump();
  host->dumpCurrentJavaScriptStackInMainThread(
      base::BindOnce(&AlloyBrowserHostImplExt::OnDumpJavaScriptStackCallback, alloyBrowserHostImpl->AsAlloyBrowserHostImplExt(),
                     host->GetProcess().Pid(), reason));
  return 0;
}
#endif

#if BUILDFLAG(ARKWEB_IMMEDIATE_DESTORY)
void AlloyBrowserHostImplUtils::handleSingleRenderDelayShutdown(content::WebContents* source) {
  if (content::RenderProcessHost::render_process_mode() ==
    content::RenderProcessMode::SINGLE_MODE) {
    auto mainFrame = source->GetPrimaryMainFrame();
    auto process = static_cast<content::RenderProcessHostImpl*>(
      mainFrame->GetProcess());
    auto siteInstance = static_cast<content::SiteInstanceImpl*>(mainFrame->GetSiteInstance());
    if (process && siteInstance) {
      LOG(INFO) << "delay shut down render process in single mode";
      process->DelayProcessShutdown(base::Seconds(1), base::TimeDelta(), siteInstance->GetSiteInfo());
      return ;
    }
  }
  // Try to fast shutdown the associated process.
  source->GetPrimaryMainFrame()->GetProcess()->FastShutdownIfPossible(1, false);
}
#endif
