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

#include "cef/libcef/browser/browser_host_base.h"
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "cef/ohos_cef_ext/libcef/browser/arkweb_browser_host_ext.h"
#endif

#include <tuple>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "cef/libcef/browser/browser_info_manager.h"
#include "cef/libcef/browser/browser_platform_delegate.h"
#include "cef/libcef/browser/context.h"
//#include "cef/libcef/browser/extensions/browser_extensions_util.h"
#include "cef/libcef/browser/hang_monitor.h"
#include "cef/libcef/browser/image_impl.h"
#include "cef/libcef/browser/navigation_entry_impl.h"
#include "cef/libcef/browser/printing/print_util.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/libcef/common/frame_util.h"
#include "cef/libcef/common/net/url_util.h"
#include "chrome/browser/platform_util.h"
#include "chrome/browser/spellchecker/spellcheck_factory.h"
#include "chrome/browser/spellchecker/spellcheck_service.h"
#include "chrome/browser/ui/browser_commands.h"
#include "components/favicon/core/favicon_url.h"
#include "components/spellcheck/common/spellcheck_features.h"
#include "components/zoom/page_zoom.h"
#include "components/zoom/zoom_controller.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/download_request_utils.h"
#include "content/public/browser/file_select_listener.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#if BUILDFLAG(IS_ARKWEB)
#include "cef/ohos_cef_ext/libcef/browser/javascript/oh_javascript_injector.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "ohos_cef_ext/libcef/browser/autofill/oh_autofill_client.h"
#endif
#include "libcef/browser/arkweb_browser_host_ext.h"
#include "ui/base/resource/resource_scale_factor.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/shell_dialogs/select_file_policy.h"
#if BUILDFLAG(IS_ARKWEB)
#include "libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"
#endif
#include "cef/ohos_cef_ext/libcef/browser/arkweb_received_slice_helper_ext.h"
#include "components/download/public/common/download_item.h"
#include "components/search_engines/template_url_data.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "chrome/browser/preloading/prefetch/no_state_prefetch/no_state_prefetch_manager_factory.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_handle.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_manager.h"
#endif  // ARKWEB_NO_STATE_PREFETCH

#if BUILDFLAG(ARKWEB_ITP)
#include "cef/ohos_cef_ext/libcef/browser/anti_tracking/third_party_cookie_access_policy.h"
#endif

#if BUILDFLAG(ARKWEB_I18N)
#include "chrome/browser/browser_process.h"
#include "third_party/blink/public/common/renderer_preferences/renderer_preferences.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_switches.h"
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "cef/ohos_cef_ext/libcef/browser/adblock/adblock_config_bridge.h"
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#include "libcef/browser/subresource_filter/adblock_list.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "base/files/file_util.h"
#include "base/strings/string_number_conversions.h"
#include "cef/include/cef_parser.h"
#include "content/public/common/mhtml_generation_params.h"
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include <chrono>

#include "arkweb/ohos_nweb/src/nweb_inputmethod_handler.h"
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
#include "libcef/browser/ohos_safe_browsing/ohos_url_trust_list_manager.h"
#include "libcef/browser/ohos_safe_browsing/ohos_url_trust_list_navigation_throttle.h"
#endif

#if BUILDFLAG(ARKWEB_MSGPORT)
#include "base/json/json_writer.h"
#include "base/rand_util.h"
#include "content/browser/web_contents/web_contents_impl.h"
#endif

#if BUILDFLAG(ARKWEB_SECURITY_STATE)
#include "components/security_state/content/content_utils.h"
#include "components/security_state/core/security_state.h"
#endif

#if BUILDFLAG(ARKWEB_PRINT)
#include "chrome/browser/printing/print_view_manager_common.h"
#include "libcef/browser/printing/ohos_print_manager.h"
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
#include "chrome/browser/password_manager/chrome_password_manager_client.h"
using OhPasswordManagerClient = ChromePasswordManagerClient;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "content/public/common/url_constants.h"
#include "net/base/mime_util.h"
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "components/embedder_support/user_agent_utils.h"
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
#include "content/browser/media/session/media_session_impl.h"
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_PERMISSION)
#include "ohos_cef_ext/libcef/browser/permission/alloy_access_request.h"
#include "ohos_cef_ext/libcef/browser/permission/alloy_geolocation_access.h"
#endif  // BUILDFLAG(ARKWEB_PERMISSION)

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
#include "chrome/browser/profiles/profile.h"
#include "components/zoom/zoom_controller.h"
#include "third_party/blink/public/common/page/page_zoom.h"
#endif
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "content/public/browser/browsing_data_remover.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "chrome/browser/extensions/api/tabs/tabs_windows_api.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "ohos_nweb/include/nweb_errors.h"
#include "ohos_nweb/src/capi/arkweb_error_code.h"
#endif

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include "cef/ohos_cef_ext/libcef/browser/cef_download_item_impl_ext.h"
#include "ohos_cef_ext/libcef/browser/permission/alloy_access_request.h"
#include "ohos_cef_ext/libcef/browser/permission/alloy_geolocation_access.h"
#if BUILDFLAG(ARKWEB_DEVTOOLS)
#include "cef/ohos_cef_ext/libcef/browser/devtools/devtools_manager_delegate.h"
#endif

#include "arkweb/chromium_ext/components/js_injection/js_communication_host_utils.h"

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "libcef/browser/dom_distiller/oh_dom_distiller_manager.h"
#include "libcef/browser/dom_distiller/oh_self_deleting_request_delegate.h"
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
#include "arkweb/chromium_ext/chrome/browser/ssl/ohos_https_upgrades_helper.h"
#endif

#include "arkweb/ohos_adapter_ndk/inputmethodframework_adapter/imf_adapter_impl.h"

const char kNWebId[] = "nweb_id";
#endif
namespace {

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
static float DEFAULT_MIN_ZOOM_FACTOR = 0.01f;
static float DEFAULT_MAX_ZOOM_FACTOR = 100.0f;

enum class WebScrollType : int { UNKNOWN = -1, EVENT = 0 };
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

const int32_t kFrameIdMinLength = 3;

bool CheckUrlPath(base::FilePath url_path, CefString& path) {
    auto file_path =
        base::MakeAbsoluteFilePathNoResolveSymbolicLinks(base::FilePath(path))
            .value_or(base::FilePath());
    if (file_path.empty()) {
      return false;
    }
    if (file_path == url_path || file_path.IsParent(url_path)) {
      return true;
    }
    return false;
}

}  // namespace

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
const std::string WEB_ARCHIVE_EXTENSION = ".mht";

CefString GenerateArchiveAutoNamePath(const GURL& url,
                                      const CefString& base_name) {
  if (!url.is_empty()) {
    std::string file_name = url.ExtractFileName();
    if (file_name.empty()) {
      file_name = "index";
    }

    std::string test_name =
        base_name.ToString() + file_name + WEB_ARCHIVE_EXTENSION;

    if (!base::PathExists(base::FilePath(test_name))) {
      return test_name;
    }

    for (int i = 0; i < 100; ++i) {
      test_name = base_name.ToString() + file_name + "-" +
                  base::NumberToString(i) + WEB_ARCHIVE_EXTENSION;
      if (!base::PathExists(base::FilePath(test_name))) {
        return test_name;
      }
    }
  }
  return "";
}
#endif

bool ParaseRenderFrameHostId(const std::string& frameId, int* childId, int* routingId) {
  if (frameId.size() < kFrameIdMinLength) {
    return false;
  }
 
  const size_t pos = frameId.find('_');
  if (pos == std::string::npos || pos == 0 || pos == frameId.size() - 1) {
    return false;
  }
 
  if (!base::StringToInt(frameId.substr(0, pos), childId) ||
      !base::StringToInt(frameId.substr(pos + 1), routingId)) {
    return false;
  }
 
  return true;
}

ArkWebBrowserHostExtImpl::ArkWebBrowserHostExtImpl()
    : weak_ptr_factory_(this) {}

#if BUILDFLAG(ARKWEB_NAVIGATION)
#include "cef/ohos_cef_ext/libcef/browser/arkweb_navigation_state_serializer_ext.h"
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_FULLSCREEN)
void ArkWebBrowserHostExtImpl::ExitFullScreen() {
  base::AutoLock lock_scope(state_lock_);
  auto wc = GetWebContents();
  if (wc == nullptr) {
    LOG(ERROR) << "getWebContents falied, wc is null";
    return;
  }

  if (!is_fullscreen_) {
    LOG(ERROR) << "not full-screen state";
    return;
  }
  wc->GetPrimaryMainFrame()->AllowInjectingJavaScript();
  std::string jscode(
      "{if(document.fullscreenElement){document.exitFullscreen()}}");
  wc->GetPrimaryMainFrame()->ExecuteJavaScript(base::UTF8ToUTF16(jscode),
                                               base::NullCallback());
}
#endif  // BUILDFLAG(ARKWEB_FULLSCREEN)

#if BUILDFLAG(IS_OHOS)
void ArkWebBrowserHostExtImpl::SetBrowserUserAgentString(
    const CefString& user_agent) {
  auto callback =
      base::BindOnce(&ArkWebBrowserHostExtImpl::ReloadOriginalUrl, this);
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT, std::move(callback));
    return;
  }
  PutUserAgent(user_agent, true);
}

void ArkWebBrowserHostExtImpl::DeleteHistory() {
  auto callback =
      base::BindOnce(&ArkWebBrowserHostExtImpl::DeleteHistory, this);
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT, std::move(callback));
    return;
  }
  auto wc = GetWebContents();
  if (wc && wc->GetController().CanPruneAllButLastCommitted()) {
    wc->GetController().PruneAllButLastCommitted();
  }
}

void ArkWebBrowserHostExtImpl::WasOccluded(bool occluded) {
  // TODO(ohos): please impl the function and remove this comment.
  LOG(INFO) << "ArkWebBrowserHostExtImpl::WasOccluded";
}

// TODO(ohos): please impl the function and remove this comment.
void ArkWebBrowserHostExtImpl::OnWindowShow() {
  // TODO(ohos): please impl the function and remove this comment.
  LOG(INFO) << "ArkWebBrowserHostExtImpl::OnWindowShow";
}

void ArkWebBrowserHostExtImpl::OnWindowHide() {
  // TODO(ohos): please impl the function and remove this comment.
}

void ArkWebBrowserHostExtImpl::OnOnlineRenderToForeground() {
  // TODO(ohos): please impl the function and remove this comment.
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebBrowserHostExtImpl::AdvanceFocusForIME(int focusType) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT, base::BindOnce(&AlloyBrowserHostImpl::AdvanceFocusForIME, this,
                                focusType));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->AdvanceFocusForIME(focusType);
  }
}

void ArkWebBrowserHostExtImpl::SendTouchEventList(
    const std::vector<CefTouchEvent>& event_list) {
  if (!IsWindowless()) {
    DCHECK(false) << "Window rendering is not disabled";
    return;
  }

  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&ArkWebBrowserHostExtImpl::SendTouchEventList,
                                 this, event_list));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SendTouchEventList(event_list);
  }
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

void ArkWebBrowserHostExtImpl::WasKeyboardResized() {
  // TODO(ohos): please impl the function and remove this comment.
}

void ArkWebBrowserHostExtImpl::SetWindowId(int window_id, int nweb_id) {
  // TODO(ohos): please impl the function and remove this comment.
}

void ArkWebBrowserHostExtImpl::SetWakeLockHandler(
    int32_t windowId,
    CefRefPtr<CefSetLockCallback> callback) {
  // TODO(ohos): please impl the function and remove this comment.
}

#if BUILDFLAG(ARKWEB_PRINT)
void ArkWebBrowserHostExtImpl::SetToken(void* token) {
  // TODO(ohos): please impl the function and remove this comment.
}

#if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
void ArkWebBrowserHostExtImpl::SetVirtualPixelRatio(float ratio) {
  if (std::fabs(ratio - virtual_pixel_ratio_) >
      std::numeric_limits<float>::epsilon()) {
    UpdatePixelRatio(ratio);
  }
  virtual_pixel_ratio_ = ratio;
}

float ArkWebBrowserHostExtImpl::GetVirtualPixelRatio() {
  return virtual_pixel_ratio_;
}

void ArkWebBrowserHostExtImpl::UpdatePixelRatio(float ratio) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<CefFrameHostImpl*>(frame.get())->UpdatePixelRatio(ratio);
  } else {
    LOG(ERROR) << "main frame is invalid";
  }
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void ArkWebBrowserHostExtImpl::SetIsFling(bool is_fling) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())->SetIsFling(is_fling);
  } else {
    LOG(ERROR) << "main frame is invalid";
  }
}
#endif

void ArkWebBrowserHostExtImpl::ShowFreeCopyMenu() {
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  if (!GetWebContents()) {
    return;
  }
  LOG(DEBUG) << "select and copy invoke";
  GetWebContents()->ShowFreeCopyMenu();
#endif
}

std::string ArkWebBrowserHostExtImpl::GetSelectedTextFromContextParam() {
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  if (GetWebContents()) {
    auto rvh = GetWebContents()->GetRenderViewHost();
    if (rvh && rvh->GetWidget() && rvh->GetWidget()->GetView()) {
      ArkWebRenderWidgetHostViewOSRExt* view =
          static_cast<ArkWebRenderWidgetHostViewOSRExt*>(rvh->GetWidget()->GetView());
      if (view && !view->GetLastSelectedTextFromContextParam().empty()) {
        return base::UTF16ToUTF8(view->GetLastSelectedTextFromContextParam());
      }
    }
  }
  return std::string();
#else
  return std::string();
#endif
}

bool ArkWebBrowserHostExtImpl::ShouldShowFreeCopyMenu() {
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  if (!GetWebContents()) {
    return false;
  }
  return GetWebContents()->ShouldShowFreeCopyMenu();
#else
  return false;
#endif
}

void ArkWebBrowserHostExtImpl::CreateWebPrintDocumentAdapter(
    const CefString& jobName,
    void** webPrintDocumentAdapter) {
  content::RenderFrameHost* rfh_to_use =
      printing::OhosPrintManager::GetRenderFrameHostToUse(GetWebContents());
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return;
  }

  ohos_print_manager->CreateWebPrintDocumentAdapter(jobName,
                                                    webPrintDocumentAdapter);
}

void ArkWebBrowserHostExtImpl::CreateWebPrintDocumentAdapterV2(
    const CefString& jobName,
    void** adapter) {
  content::RenderFrameHost* rfh_to_use =
      printing::OhosPrintManager::GetRenderFrameHostToUse(GetWebContents());
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return;
  }

  ohos_print_manager->CreateWebPrintDocumentAdapterV2(jobName, adapter);
}

void ArkWebBrowserHostExtImpl::SetPrintBackground(bool enabled) {
  content::RenderFrameHost* rfh_to_use =
      printing::GetFrameToPrint(GetWebContents());
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return;
  }

  ohos_print_manager->SetPrintBackground(enabled);
}

bool ArkWebBrowserHostExtImpl::GetPrintBackground() {
  content::RenderFrameHost* rfh_to_use =
      printing::GetFrameToPrint(GetWebContents());
  if (!rfh_to_use) {
    LOG(ERROR) << "rfh_to_use is nullptr";
    return false;
  }
  auto* ohos_print_manager = printing::OhosPrintManager::FromWebContents(
      content::WebContents::FromRenderFrameHost(rfh_to_use));
  if (!ohos_print_manager) {
    LOG(ERROR) << "ohos_print_manager is nullptr";
    return false;
  }

  return ohos_print_manager->GetPrintBackground();
}
#endif  // BUILDFLAG(ARKWEB_PRINT)

void ArkWebBrowserHostExtImpl::SetEnableLowerFrameRate(bool enabled) {
  // TODO(ohos): please impl the function and remove this comment.
}

void ArkWebBrowserHostExtImpl::SetEnableHalfFrameRate(bool enabled) {
  // TODO(ohos): please impl the function and remove this comment.
}

#if BUILDFLAG(ARKWEB_PDF)
void ArkWebBrowserHostExtImpl::CreateToPDF(
    const CefPdfPrintSettings& settings,
    CefRefPtr<CefPdfValueCallback> callback) {
  auto web_contents = GetWebContents();
  if (!web_contents || !callback) {
    LOG(ERROR) << "CefBrowserHostBase::CreateToPDF "
                  "callback is nullptr or web_contents is null";
    return;
  }

  print_util::CreateToPDF(web_contents, settings, callback);
}
#endif  // BUILDFLAG(ARKWEB_PDF)

void ArkWebBrowserHostExtImpl::PostTaskToUIThread(CefRefPtr<CefTask> task) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&ArkWebBrowserHostExtImpl::PostTaskToUIThread,
                                 this, task));
    return;
  }
  std::move(task)->Execute();
}

void ArkWebBrowserHostExtImpl::SetWebPreferences(
    const CefBrowserSettings& browser_settings) {
  UpdateBrowserSettings(browser_settings);
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  GetWebContents()->OnWebPreferencesChanged(settings_.usage_scenario);
#else
  GetWebContents()->OnWebPreferencesChanged();
#endif
}

void ArkWebBrowserHostExtImpl::OnWebPreferencesChanged() {
  GetWebContents()->OnWebPreferencesChanged();
}

#define SETTINGS_STRING_SET(src, target) \
  cef_string_set(src.str, src.length, &target, true)

void ArkWebBrowserHostExtImpl::UpdateBrowserSettings(
    const CefBrowserSettings& browser_settings) {
  /* font family */
  SETTINGS_STRING_SET(browser_settings.standard_font_family,
                      settings_.standard_font_family);
  SETTINGS_STRING_SET(browser_settings.fixed_font_family,
                      settings_.fixed_font_family);
  SETTINGS_STRING_SET(browser_settings.serif_font_family,
                      settings_.serif_font_family);
  SETTINGS_STRING_SET(browser_settings.sans_serif_font_family,
                      settings_.sans_serif_font_family);
  SETTINGS_STRING_SET(browser_settings.cursive_font_family,
                      settings_.cursive_font_family);
  SETTINGS_STRING_SET(browser_settings.fantasy_font_family,
                      settings_.fantasy_font_family);

  /* font size*/
  settings_.default_font_size = browser_settings.default_font_size;
  settings_.default_fixed_font_size = browser_settings.default_fixed_font_size;
  settings_.minimum_font_size = browser_settings.minimum_font_size;
  settings_.minimum_logical_font_size =
      browser_settings.minimum_logical_font_size;
  SETTINGS_STRING_SET(browser_settings.default_encoding,
                      settings_.default_encoding);
  settings_.javascript = browser_settings.javascript;
  settings_.image_loading = browser_settings.image_loading;
  settings_.local_storage = browser_settings.local_storage;
#if !CEF_API_ADDED(13800)
  settings_.databases = browser_settings.databases;
#endif

  /* ohos webview add*/
  settings_.force_dark_mode_enabled = browser_settings.force_dark_mode_enabled;
#if BUILDFLAG(ARKWEB_DARKMODE)
  settings_.dark_prefer_color_scheme_enabled =
      browser_settings.dark_prefer_color_scheme_enabled;
#endif
  settings_.javascript_can_open_windows_automatically =
      browser_settings.javascript_can_open_windows_automatically;
  settings_.loads_images_automatically =
      browser_settings.loads_images_automatically;
  settings_.text_size_percent = browser_settings.text_size_percent;
  settings_.allow_running_insecure_content =
      browser_settings.allow_running_insecure_content;
  settings_.strict_mixed_content_checking =
      browser_settings.strict_mixed_content_checking;
  settings_.allow_mixed_content_upgrades =
      browser_settings.allow_mixed_content_upgrades;
  settings_.geolocation_enabled = browser_settings.geolocation_enabled;
  settings_.supports_double_tap_zoom =
      browser_settings.supports_double_tap_zoom;
  settings_.supports_multi_touch_zoom =
      browser_settings.supports_multi_touch_zoom;
  settings_.zoom_control_access =
      browser_settings.zoom_control_access;
  settings_.initialize_at_minimum_page_scale =
      browser_settings.initialize_at_minimum_page_scale;
  settings_.viewport_meta_enabled = browser_settings.viewport_meta_enabled;
  settings_.user_gesture_required = browser_settings.user_gesture_required;
  settings_.pinch_smooth_mode = browser_settings.pinch_smooth_mode;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  settings_.hide_vertical_scrollbars =
      browser_settings.hide_vertical_scrollbars;
  settings_.hide_horizontal_scrollbars =
      browser_settings.hide_horizontal_scrollbars;
  settings_.scroll_enabled = browser_settings.scroll_enabled;
  settings_.blur_enabled = browser_settings.blur_enabled;
#endif  // defined(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  settings_.border_radius_top_left = browser_settings.border_radius_top_left;
  settings_.border_radius_top_right = browser_settings.border_radius_top_right;
  settings_.border_radius_bottom_left =
      browser_settings.border_radius_bottom_left;
  settings_.border_radius_bottom_right =
      browser_settings.border_radius_bottom_right;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER
#if BUILDFLAG(ARKWEB_MENU)
  settings_.touch_handle_exist = browser_settings.touch_handle_exist;
  settings_.viewport_scale = browser_settings.viewport_scale;
#endif  // BUILDFLAG(ARKWEB_MENU)
#if BUILDFLAG(IS_OHOS)
  settings_.draw_mode = browser_settings.draw_mode;
  settings_.text_autosizing_enabled = browser_settings.text_autosizing_enabled;
  settings_.force_zero_layout_height =
      browser_settings.force_zero_layout_height;
#endif  // BUILDFLAG(IS_OHOS)
#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  settings_.background_color = browser_settings.background_color;
#endif  // ARKWEB_BACKGROUND_COLOR
#if BUILDFLAG(ARKWEB_CSS_FONT)
  settings_.font_weight_scale = browser_settings.font_weight_scale;
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  settings_.native_embed_mode_enabled =
      browser_settings.native_embed_mode_enabled;
  settings_.intrinsic_size_enabled =
      browser_settings.intrinsic_size_enabled;
  settings_.css_display_change_enabled =
      browser_settings.css_display_change_enabled;
  SETTINGS_STRING_SET(browser_settings.embed_tag, settings_.embed_tag);
  SETTINGS_STRING_SET(browser_settings.embed_tag_type,
                      settings_.embed_tag_type);
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  settings_.scrollbar_color = browser_settings.scrollbar_color;
#endif  // ARKWEB_SCROLLBAR

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  settings_.contextmenu_customization_enabled =
      browser_settings.contextmenu_customization_enabled;
#endif

#if BUILDFLAG(ARKWEB_COPY_OPTION)
  settings_.copy_option = browser_settings.copy_option;
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_FOCUS)
  settings_.gesture_focus_mode = browser_settings.gesture_focus_mode;
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  settings_.custom_video_player_enable =
      browser_settings.custom_video_player_enable;
  settings_.custom_video_player_overlay =
      browser_settings.custom_video_player_overlay;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  settings_.supports_multiple_windows =
      browser_settings.supports_multiple_windows;
#endif  // ARKWEB_MULTI_WINDOW

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  settings_.record_whole_document = browser_settings.record_whole_document;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  settings_.usage_scenario = browser_settings.usage_scenario;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  settings_.universal_access_from_file_urls =
      browser_settings.universal_access_from_file_urls;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
  settings_.enable_media_network_traffic_prompt =
      browser_settings.enable_media_network_traffic_prompt;
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  settings_.delay_for_background_tab_freezing =
      browser_settings.delay_for_background_tab_freezing;
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  settings_.record_whole_document = browser_settings.record_whole_document;
#endif
#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  settings_.error_page_enabled = browser_settings.error_page_enabled;
#endif
}

void ArkWebBrowserHostExtImpl::SetDrawRect(int x,
                                           int y,
                                           int width,
                                           int height) {
  // todo(ohos):impl this function then remove todo
}
#if BUILDFLAG(ARKWEB_DISCARD)
bool ArkWebBrowserHostExtImpl::Discard() {
  if (!CEF_CURRENTLY_ON_UIT()) {
    LOG(ERROR)
        << "ArkWebBrowserHostExtImpl::Discard failed, called on invalid thread";
    return false;
  }
  if (!GetWebContents()) {
    LOG(ERROR)
        << "ArkWebBrowserHostExtImpl::Discard failed, WebContents is nullptr";
    return false;
  }

  content::RenderFrameHost* main_frame =
      GetWebContents()->GetPrimaryMainFrame();
  content::RenderProcessHost* render_process = main_frame->GetProcess();
  if (!render_process) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::Discard failed, RenderProcessHost "
                  "is nullptr";
    return false;
  }

  bool fast_shutdown_success = render_process->FastShutdownIfPossible(1u, true);
  if (fast_shutdown_success) {
    GetWebContents()->SetWasDiscarded(true);
  }

  return fast_shutdown_success;
}

bool ArkWebBrowserHostExtImpl::Restore() {
  if (!CEF_CURRENTLY_ON_UIT()) {
    LOG(ERROR)
        << "ArkWebBrowserHostExtImpl::Restore failed, called on invalid thread";
    return false;
  }
  if (!GetWebContents()) {
    LOG(ERROR)
        << "ArkWebBrowserHostExtImpl::Restore failed, WebContents is nullptr";
    return false;
  }

  GetWebContents()->GetController().SetNeedsReload();
  GetWebContents()->GetController().LoadIfNecessary();
  GetWebContents()->Focus();

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  needs_reload_ = false;
#endif

  return true;
}
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
void ArkWebBrowserHostExtImpl::JavaScriptOnDocumentStart(
    const CefString& script,
    const std::vector<CefString>& script_rules,
    bool is_transfer_finished) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto* host = GetJsCommunicationHost();
  if (host) {
    std::string stdScript = script.ToString();
    std::vector<std::string> scriptRules;
    for (CefString rule : script_rules) {
      scriptRules.push_back(rule.ToString());
    }
    if (!script.empty()) {
      js_injection::JsCommunicationHost::AddScriptResult result =
          host->js_communication_host_utils_->AddDocumentStartPendingJavaScript(script, scriptRules);
    }
    if (is_transfer_finished) {
        host->js_communication_host_utils_->CommitPendingJavascriptsAtDocumentStart();
    }
  }
}

void ArkWebBrowserHostExtImpl::RemoveJavaScriptOnDocumentStart() {}

js_injection::JsCommunicationHost*
ArkWebBrowserHostExtImpl::GetJsCommunicationHost() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (!js_communication_host_.get()) {
    js_communication_host_ =
        std::make_unique<js_injection::JsCommunicationHost>(GetWebContents());
  }
  return js_communication_host_.get();
}

void ArkWebBrowserHostExtImpl::JavaScriptOnDocumentEnd(
    const CefString& script,
    const std::vector<CefString>& script_rules,
    bool is_transfer_finished) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto* host = GetJsCommunicationHost();
  if (host) {
    std::string stdScript = script.ToString();
    std::vector<std::string> scriptRules;
    for (CefString rule : script_rules) {
      scriptRules.push_back(rule.ToString());
    }
    if (!script.empty()) {
      js_injection::JsCommunicationHost::AddScriptResult result =
          host->js_communication_host_utils_->AddDocumentEndPendingJavaScript(script, scriptRules);
    }
    if (is_transfer_finished) {
        host->js_communication_host_utils_->CommitPendingJavascriptsAtDocumentEnd();
    }
  }
}

void ArkWebBrowserHostExtImpl::RemoveJavaScriptOnDocumentEnd() {}

void ArkWebBrowserHostExtImpl::JavaScriptOnHeadReady(
    const CefString& script,
    const std::vector<CefString>& script_rules,
    bool is_transfer_finished) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto* host = GetJsCommunicationHost();
  if (host) {
    std::string stdScript = script.ToString();
    std::vector<std::string> scriptRules;
    for (CefString rule : script_rules) {
      scriptRules.push_back(rule.ToString());
    }
    if (!script.empty()) {
      js_injection::JsCommunicationHost::AddScriptResult result =
          host->js_communication_host_utils_->AddHeadReadyPendingJavaScript(script, scriptRules);
    }
    if (is_transfer_finished) {
        host->js_communication_host_utils_->CommitPendingJavascriptsAtHeadReady();
    }
  }
}

void ArkWebBrowserHostExtImpl::RemoveJavaScriptOnHeadReady() {}
#endif  // BUILDFLAG(ARKWEB_JSPROXY)

void ArkWebBrowserHostExtImpl::SetNWebId(int NWebID) {
#if BUILDFLAG(ARKWEB_WEBRTC)
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "GetWebContents null";
#endif
    return;
  }
  web_contents->SetNWebId(NWebID);
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}

#if BUILDFLAG(IS_ARKWEB)
void ArkWebBrowserHostExtImpl::EnableAppLinking(bool enable) {
  is_arkweb_applinking_enabled_ = enable;
}
 
bool ArkWebBrowserHostExtImpl::IsAppLinkingEnabled() const {
  return is_arkweb_applinking_enabled_;
}
#endif // BUILDFLAG(IS_ARKWEB)

int ArkWebBrowserHostExtImpl::GetNWebId() {
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  if (browser_info() && browser_info()->extra_info()) {
    auto nweb_id_value = browser_info()->extra_info()->GetValue(kNWebId);
    if (nweb_id_value) {
      return nweb_id_value->GetInt();
    }
  }
  return -1;
#else
  return -1;
#endif
}

std::string ArkWebBrowserHostExtImpl::GetDataURI(const std::string& data) {
  return CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
      .ToString();
}

void ArkWebBrowserHostExtImpl::LoadWithDataAndBaseUrl(
    const CefString& baseUrl,
    const CefString& data,
    const CefString& mimeType,
    const CefString& encoding,
    const CefString& historyUrl) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&ArkWebBrowserHostExtImpl::LoadWithDataAndBaseUrl, this,
                       baseUrl, data, mimeType, encoding, historyUrl));
    return;
  }
  std::string dataBase = data.empty() ? "" : data;
  std::string mimeTypeBase = mimeType.empty() ? "text/html" : mimeType;
  std::string url = baseUrl.empty() ? "about:blank" : baseUrl;
  std::string historyUrlBase = historyUrl.empty() ? "about:blank" : historyUrl;

  std::string buildData = "data:";
  buildData.append(mimeTypeBase);
  if (!encoding.empty()) {
    buildData.append(";charset=");
    buildData.append(encoding);
  }
  buildData.append(";base64");
  buildData.append(",");
  std::string emtry_data_url = buildData;
  dataBase = GetDataURI(dataBase);
  buildData.append(dataBase);
  GURL data_url = GURL(buildData);
  content::NavigationController::LoadURLParams loadUrlParams(data_url);
  if (data_url.spec().size() > url::kMaxURLChars) {
    loadUrlParams.url = GURL(emtry_data_url);
    loadUrlParams.data_url_as_string =
        base::MakeRefCounted<base::RefCountedString>(std::move(buildData));
  }

  if (!(url.find("data:") == 0)) {
    loadUrlParams.virtual_url_for_special_cases = GURL(historyUrlBase);
    loadUrlParams.base_url_for_data_url = GURL(url);
  }

  loadUrlParams.load_type = content::NavigationController::LOAD_TYPE_DATA;
  loadUrlParams.transition_type = ui::PAGE_TRANSITION_TYPED;
  loadUrlParams.override_user_agent =
      content::NavigationController::UA_OVERRIDE_TRUE;
  loadUrlParams.can_load_local_resources = true;
  auto web_contents = GetWebContents();
  if (web_contents) {
    LOG(DEBUG) << "load data with BaseUrl";
    web_contents->GetController().LoadURLWithParams(loadUrlParams);
  }
}

void ArkWebBrowserHostExtImpl::LoadWithData(const CefString& data,
                                            const CefString& mimeType,
                                            const CefString& encoding) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&ArkWebBrowserHostExtImpl::LoadWithData, this,
                                 data, mimeType, encoding));
    return;
  }
  std::string dataBase = data.empty() ? "" : data;
  std::string mimeTypeBase = mimeType.empty() ? "text/html" : mimeType;

  std::string buildData = "data:";
  buildData.append(mimeTypeBase);
  if (encoding.ToString() == "base64") {
    buildData.append(";base64");
  }
  buildData.append(",");
  std::string emtry_data_url = buildData;
  buildData.append(dataBase);
  GURL data_url = GURL(buildData);
  content::NavigationController::LoadURLParams loadUrlParams(data_url);
  if (data_url.spec().size() > url::kMaxURLChars) {
    loadUrlParams.url = GURL(emtry_data_url);
    loadUrlParams.data_url_as_string =
        base::MakeRefCounted<base::RefCountedString>(std::move(buildData));
  }

  auto web_contents = GetWebContents();
  if (web_contents) {
    LOG(DEBUG) << "load data";
    web_contents->GetController().LoadURLWithParams(loadUrlParams);
  }
}

void ArkWebDealWithPostData(const std::string& post_data, 
                           content::NavigationController::LoadURLParams* params) {
  if (post_data.empty()) {
      params->post_data = new network::ResourceRequestBody();
  } else {
      params->post_data = network::ResourceRequestBody::CreateFromBytes(
          std::vector<uint8_t>(post_data.begin(), post_data.end()));
  }
}

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
void ArkWebBrowserHostExtImpl::LoadUrlWithParams(const std::string& url,
                                                 const LoadUrlType load_type,
                                                 const std::string& refer,
                                                 const std::string& headers,
                                                 const std::string& post_data,
                                                 const bool allow_https_upgrade,
                                                 int32_t transition_type) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&ArkWebBrowserHostExtImpl::LoadUrlWithParams,
                                 this, std::string(url), load_type,
                                 std::string(refer), std::string(headers),
                                 std::string(post_data), allow_https_upgrade,
                                 transition_type));
    return;
  }
  GURL new_url = GURL(url);
  content::NavigationController::LoadURLParams loadUrlParams(new_url);
 
  loadUrlParams.url = GURL(new_url);
  loadUrlParams.load_type = static_cast<content::NavigationController::LoadURLType>(load_type);
  
  loadUrlParams.referrer = content::Referrer(GURL(static_cast<std::optional<std::string>>(refer).value_or("")), 
                           network::mojom::ReferrerPolicy::kDefault);
  
  loadUrlParams.extra_headers = headers;
  ArkWebDealWithPostData(post_data, &loadUrlParams);
  loadUrlParams.force_no_https_upgrade = !allow_https_upgrade;

  loadUrlParams.transition_type = static_cast<ui::PageTransition>(
      ui::PAGE_TRANSITION_TYPED | ui::PAGE_TRANSITION_FROM_ADDRESS_BAR);
  if (ui::IsValidPageTransitionType(transition_type)) {
    loadUrlParams.transition_type =
        static_cast<ui::PageTransition>(transition_type);
  }
  // if url_typed_with_http_scheme == true, it means user dont want to upgrade.
  loadUrlParams.url_typed_with_http_scheme = !allow_https_upgrade;
                                                       
  if (auto web_contents = GetWebContents()) {
    LOG(DEBUG) << "load Url With Params";
    web_contents->GetController().LoadURLWithParams(loadUrlParams);
  }
}
#endif

void ArkWebBrowserHostExtImpl::SetNativeWindow(cef_native_window_t window) {
  widget_ = NWebNativeWindowTracker::GetInstance()->AddNativeWindow(window);
}

cef_accelerated_widget_t ArkWebBrowserHostExtImpl::GetAcceleratedWidget(
    bool isPopup) {
  if (!isPopup) {
    return widget_;
  }
  return popup_widget_;
}

void ArkWebBrowserHostExtImpl::SetPopupWindow(cef_native_window_t window) {
  popup_widget_ =
      NWebNativeWindowTracker::GetInstance()->AddNativeWindow(window);
}

bool ArkWebBrowserHostExtImpl::GetPendingSizeStatus() {
  // todo(ohos):impl this function then remove todo
  return false;
}

void ArkWebBrowserHostExtImpl::SetFitContentMode(int mode) {
  // todo(ohos):impl this function then remove todo
}

void ArkWebBrowserHostExtImpl::SetDrawMode(int mode) {
  // todo(ohos):impl this function then remove todo
}

#endif  // BUILDFLAG(IS_OHOS)

#if BUILDFLAG(ARKWEB_AUTOFILL)
void ArkWebBrowserHostExtImpl::SetAutofillCallback(
    CefRefPtr<CefWebMessageReceiver> callback) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "GetWebContents null";
#endif
    return;
  }

  autofill::OhAutofillClient* autofill_client =
      autofill::OhAutofillClient::FromWebContents(web_contents);
  if (autofill_client) {
    autofill_client->SetOnMessageCallback(callback);
  }
}

void ArkWebBrowserHostExtImpl::FillAutofillData(CefRefPtr<CefValue> message) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "GetWebContents null";
#endif
    return;
  }
  autofill::OhAutofillClient* autofill_client =
      autofill::OhAutofillClient::FromWebContents(web_contents);
  if (autofill_client) {
    autofill_client->FillData(message);
  }
}
#endif

#if BUILDFLAG(IS_ARKWEB)
void ArkWebBrowserHostExtImpl::UpdateZoomSupportEnabled() {
  auto rvh = GetWebContents()->GetRenderViewHost();
  ArkWebRenderWidgetHostViewOSRExt* view =
      static_cast<ArkWebRenderWidgetHostViewOSRExt*>(
          rvh->GetWidget()->GetView());

  if (view) {
    view->SetDoubleTapSupportEnabled(settings_.supports_double_tap_zoom);
    view->SetMultiTouchZoomSupportEnabled(settings_.supports_multi_touch_zoom);
  }
}
#endif

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
SkColor ArkWebBrowserHostExtImpl::GetBackgroundColor() const {
  return base_background_color_;
}

void ArkWebBrowserHostExtImpl::SetBackgroundColor(int color) {
  if (color == base_background_color_) {
    return;
  }

  base_background_color_ = color;
  OnWebPreferencesChanged();
  UpdateBackgroundColor(color);
}

void ArkWebBrowserHostExtImpl::UpdateBackgroundColor(int color) {
  auto rvh = GetWebContents()->GetRenderViewHost();

  if (rvh->GetWidget()->GetView()) {
    rvh->GetWidget()->GetView()->SetBackgroundColor(color);
  }

  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->UpdateBackgroundColor(color);
  }
}
#endif  // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebBrowserHostExtImpl::ScrollFocusedEditableNodeIntoView() {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(
            &ArkWebBrowserHostExtImpl::ScrollFocusedEditableNodeIntoView,
            this));
    return;
  }
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->ScrollFocusedEditableNodeIntoView();
  }
}

void ArkWebBrowserHostExtImpl::ScaleGestureChangeV2(int type,
                                                    float scale,
                                                    float originScale,
                                                    float centerX,
                                                    float centerY) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->ScaleGestureChangeV2(type, scale, originScale, centerX,
                                             centerY);
  }
}

void ArkWebBrowserHostExtImpl::GoBackOrForward(int num_steps) {
  auto callback = base::BindOnce(&ArkWebBrowserHostExtImpl::GoBackOrForward,
                                 this, num_steps);
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT, std::move(callback));
    return;
  }

  if (browser_info_->IsNavigationLocked(std::move(callback))) {
    return;
  }

  auto wc = GetWebContents();
  if (wc && wc->GetController().CanGoToOffset(num_steps)) {
    wc->GetController().GoToOffset(num_steps);
  }
}

void ArkWebBrowserHostExtImpl::SetInitialScale(float scale) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->SetInitialScale(scale / (100 / virtual_pixel_ratio_));
  }
}

void ArkWebBrowserHostExtImpl::SetFocusOnWeb() {
  if (settings_.blur_enabled) {
    auto web_contents = GetWebContents();
    if (web_contents) {
      LOG(INFO)
          << "ArkWebBrowserHostExtImpl::SetFocusOnWeb: ClearFocusedElement";
      web_contents->ClearFocusedElement();
    }
  }
}

void ArkWebBrowserHostExtImpl::SetImeShow(bool visible) {
  if (client_) {
    CefRefPtr<CefKeyboardHandler> handler = client_->GetKeyboardHandler();
    if (handler) {
      LOG(INFO) << "ArkWebBrowserHostExtImpl::SetImeShow visible=" << visible;
      handler->SetImeShow(visible);
    }
  }
}

void ArkWebBrowserHostExtImpl::UpdateSecurityLayer(bool isNeedSecurityLayer) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->UpdateSecurityLayer(isNeedSecurityLayer);
  }
}

void ArkWebBrowserHostExtImpl::UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->UpdateTextFieldStatus(isShowKeyboard, isAttachIME);
  }
}

void ArkWebBrowserHostExtImpl::SetHasComposition(bool has_composition) {
  has_composition_ = has_composition;
}

bool ArkWebBrowserHostExtImpl::GetHasComposition() {
  return has_composition_;
}
#endif  // ARKWEB_INPUT_EVENTS

bool ArkWebBrowserHostExtImpl::CanGoBackOrForward(int num_steps) {
  auto wc = GetWebContents();
  if (wc != nullptr) {
    return wc->GetController().CanGoToOffset(num_steps);
  }
  return false;
}

CefString ArkWebBrowserHostExtImpl::Title() {
  auto web_contents = GetWebContents();
  if (web_contents) {
    return web_contents->GetTitle();
  }
  return "";
}
int ArkWebBrowserHostExtImpl::PageLoadProgress() {
  auto web_contents = GetWebContents();
  if (web_contents) {
    if (!web_contents->IsLoading()) {
      return 100;
    }
    return round(100 * web_contents->GetLoadProgress());
  }
  return 0;
}

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
void ArkWebBrowserHostExtImpl::StoreWebArchiveInternal(
    CefRefPtr<CefStoreWebArchiveResultCallback> callback,
    const CefString& path) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    callback->OnStoreWebArchiveDone("");
    return;
  }

  web_contents->GenerateMHTML(
      content::MHTMLGenerationParams(base::FilePath(path)),
      base::BindOnce(
          [](const CefString& path,
             CefRefPtr<CefStoreWebArchiveResultCallback> callback,
             int64_t file_size) {
            CEF_REQUIRE_UIT();
            callback->OnStoreWebArchiveDone(file_size < 0 ? "" : path);
          },
          path, callback));
}

void ArkWebBrowserHostExtImpl::StoreWebArchive(
    const CefString& base_name,
    bool auto_name,
    CefRefPtr<CefStoreWebArchiveResultCallback> callback) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&ArkWebBrowserHostExtImpl::StoreWebArchive,
                                 this, base_name, auto_name, callback));
    return;
  }

  if (!callback) {
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    callback->OnStoreWebArchiveDone("");
    return;
  }

  if (auto_name) {
    base::ThreadPool::PostTaskAndReplyWithResult(
        FROM_HERE,
        {base::MayBlock(), base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN},
        base::BindOnce(&GenerateArchiveAutoNamePath, web_contents->GetURL(),
                       base_name),
        base::BindOnce(&ArkWebBrowserHostExtImpl::StoreWebArchiveInternal,
                       weak_ptr_factory_.GetWeakPtr(), std::move(callback)));
  } else {
    StoreWebArchiveInternal(std::move(callback), base_name);
  }
}
#endif

#if BUILDFLAG(ARKWEB_NAVIGATION)
CefRefPtr<CefBinaryValue> ArkWebBrowserHostExtImpl::GetWebState() {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    return nullptr;
  }

  return NavigationStateSerializer::WriteNavigationStatus(*web_contents);
}

bool ArkWebBrowserHostExtImpl::RestoreWebState(
    const CefRefPtr<CefBinaryValue> state) {
  auto web_contents = GetWebContents();
  if (!web_contents || !state) {
    return false;
  }
  return NavigationStateSerializer::RestoreNavigationStatus(*web_contents,
                                                            state);
}
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebBrowserHostExtImpl::ScrollTo(float x, float y) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())->ScrollTo(x, y);
  }
}

void ArkWebBrowserHostExtImpl::ScrollBy(float delta_x, float delta_y) {
  // By calling cc interface SetSynchronousInputHandlerRootScrollOffset,
  // sliding can be realized without waiting for rendering to be completed.
  if (!scrollable_ && scrollType_ != static_cast<int>(WebScrollType::EVENT)) {
    return;
  }
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->ScrollBy(delta_x, delta_y);
  }
}

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void ArkWebBrowserHostExtImpl::SetBypassVsyncCondition(int32_t condition) {
  LOG(INFO) << "ArkWebBrowserHostExtImpl::SetBypassVsyncCondition condition:"
            << condition;
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetBypassVsyncCondition(condition);
  }
}
#endif

void ArkWebBrowserHostExtImpl::SlideScroll(float vx, float vy) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid() && scrollable_) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())->SlideScroll(vx, vy);
  }
}

void ArkWebBrowserHostExtImpl::ZoomBy(float delta, float width, float height) {
  if (delta < DEFAULT_MIN_ZOOM_FACTOR || delta > DEFAULT_MAX_ZOOM_FACTOR) {
    LOG(ERROR) << "invalid zommby delta";
    return;
  }
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->ZoomBy(delta, width, height);
  }
}

void ArkWebBrowserHostExtImpl::GetHitData(int& type, CefString& extra_data) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->GetHitData(type, extra_data);
  }
}

void ArkWebBrowserHostExtImpl::GetLastHitData(int& type, CefString& extra_data) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->GetLastHitData(type, extra_data);
  }
}

uint64_t ArkWebBrowserHostExtImpl::GetCurrentTimestamp() {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             now.time_since_epoch())
      .count();
}

void ArkWebBrowserHostExtImpl::SetScrollable(bool enable, int scrollType) {
  LOG(DEBUG) << "set scrollable: " << enable << ", scrollType = " << scrollType;
  scrollable_ = enable;
  scrollType_ = scrollType;

  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetScrollable(enable);
  }

  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    if (scrollType == static_cast<int>(WebScrollType::UNKNOWN)) {
      static_cast<CefFrameHostImpl*>(frame.get())->SetScrollable(enable);
    } else if (scrollType == static_cast<int>(WebScrollType::EVENT)) {
      static_cast<CefFrameHostImpl*>(frame.get())->SetScrollable(true);
    }
  }
}

void ArkWebBrowserHostExtImpl::SetOverscrollMode(int overscrollMode) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->SetOverscrollMode(overscrollMode);
  }
}
#endif  // defined(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
CefString ArkWebBrowserHostExtImpl::GetOriginalUrl() {
  auto web_contents = GetWebContents();
  if (web_contents) {
    return web_contents->GetController().GetOriginalUrl();
  }
  return base::EmptyString();
}

void ArkWebBrowserHostExtImpl::PutNetworkAvailable(bool available) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<CefFrameHostImpl*>(frame.get())
        ->AsArkWebFrame()
        ->SetJsOnlineProperty(available);
  }
}

void ArkWebBrowserHostExtImpl::SetFileAccess(bool flag) {
  base::AutoLock lock_scope(state_lock_);
  if (file_access_ == flag) {
    return;
  }
  file_access_ = flag;
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(GetWebContents());
  if (!mediaSession || !GetWebContents()) {
    LOG(ERROR) << "CefBrowserHostBase::SetGrantFileAccessDirs get mediaSession or webContents failed.";
    return;
  }
  mediaSession->fileAccess_ = flag;
#endif // ARKWEB_MEDIA_POLICY
}

void ArkWebBrowserHostExtImpl::SetBlockNetwork(bool flag) {
  base::AutoLock lock_scope(state_lock_);
  if (network_blocked_ == flag) {
    return;
  }
  network_blocked_ = flag;
}

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
void ArkWebBrowserHostExtImpl::SetDisallowSandboxFileAccessFromFileUrl(bool flag) {
  base::AutoLock lock_scope(state_lock_);
  if (disallow_sandbox_file_access_from_file_url_ == flag) {
    return;
  }
  disallow_sandbox_file_access_from_file_url_ = flag;
}
#endif

void ArkWebBrowserHostExtImpl::SetCacheMode(int flag) {
  base::AutoLock lock_scope(state_lock_);
  if (cache_mode_ == flag) {
    return;
  }
  cache_mode_ = flag;
}

void ArkWebBrowserHostExtImpl::SetGrantFileAccessDirs(
    const std::vector<CefString>& dir_list,
    const std::vector<CefString>& excluded_dir_list) {
  base::AutoLock lock_scope(state_lock_);
  file_access_dirs_list_ = dir_list;
  file_excluded_dirs_list_ = excluded_dir_list;
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(GetWebContents());
  if (!mediaSession || !GetWebContents()) {
    LOG(ERROR) << "CefBrowserHostBase::SetGrantFileAccessDirs get mediaSession or webContents failed.";
    return;
  }
  for (auto& dir: file_access_dirs_list_) {
    mediaSession->grantMediaFileAccessDirs_.emplace_back(dir.ToString());
  }
#endif // ARKWEB_MEDIA_POLICY
}

bool ArkWebBrowserHostExtImpl::GetFileAccess() {
  base::AutoLock lock_scope(state_lock_);
  return file_access_;
}

bool ArkWebBrowserHostExtImpl::GetBlockNetwork() {
  base::AutoLock lock_scope(state_lock_);
  return network_blocked_;
}

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
bool ArkWebBrowserHostExtImpl::GetDisallowSandboxFileAccessFromFileUrl() {
  base::AutoLock lock_scope(state_lock_);
  return disallow_sandbox_file_access_from_file_url_;
}
#endif

int ArkWebBrowserHostExtImpl::GetCacheMode() {
  base::AutoLock lock_scope(state_lock_);
  return cache_mode_;
}

std::vector<std::string> ArkWebBrowserHostExtImpl::GetGrantFileAccessDirs() {
  base::AutoLock lock_scope(state_lock_);
  std::vector<std::string> dir_list;
  for (auto& dir : file_access_dirs_list_) {
    dir_list.emplace_back(dir.ToString());
  }
  return dir_list;
}

net_service::FileAccessType ArkWebBrowserHostExtImpl::IsInFileAccessList(const GURL& url) {
  if (file_access_dirs_list_.empty()) {
    return net_service::FileAccessType::kFileAccessEmpty;
  }

  if (!url.is_valid() || !url.SchemeIsFile() || !url.has_path()) {
    return net_service::FileAccessType::kFileAccessBlock;
  }

  auto url_path = base::MakeAbsoluteFilePathNoResolveSymbolicLinks(
      base::FilePath(url.path())).value_or(base::FilePath());
  if (url_path.empty()) {
    return net_service::FileAccessType::kFileAccessBlock;
  }

  for (auto& path : file_excluded_dirs_list_) {
    if (CheckUrlPath(url_path, path)) {
      LOG(WARNING) << "IsInFileAccessList excluded";
      return net_service::FileAccessType::kFileAccessBlock;
    }
  }

  for (auto& path : file_access_dirs_list_) {
    if (CheckUrlPath(url_path, path)) {
      return net_service::FileAccessType::kFileAccessPass;
    }
  }
  return net_service::FileAccessType::kFileAccessBlock;
}

void ArkWebBrowserHostExtImpl::GetSettingOfNetHelper(const GURL& url, struct net_service::NetHelperSetting& setting) {
  setting.file_access = GetFileAccess();
  setting.block_network = GetBlockNetwork();
  setting.cache_mode = GetCacheMode();
#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  setting.disallow_sandbox_file_access_from_file_url = GetDisallowSandboxFileAccessFromFileUrl();
#endif
  setting.file_access_status = IsInFileAccessList(url);
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
int ArkWebBrowserHostExtImpl::PrerenderPage(const CefString& url,
                                            const CefString& additional_headers) {
  content::WebContents* web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "WebContents has not initialized.";
    return ARKWEB_INIT_ERROR;
  }

  for (auto it = prerender_handles_.begin(); it != prerender_handles_.end();
       ++it) {
    const std::unique_ptr<content::PrerenderHandle>& handle = *it;

    // If the handle is not equivalent but has the same prerendering URL, cancel
    // it to start a new one.
    if (handle->GetInitialPrerenderingUrl() == url.ToString()) {
      prerender_handles_.erase(it);
      break;
    }
  }

  // Cancel existing prerendering before starting a new one to avoid hitting the
  // limit.
  // Erase the oldest prerendering to free up the capacity for the new
  // attempt. If the handles are already empty, other embedder triggers should
  // be running. In that case, there is no way to trigger. Let this request
  // fail eventually.
  if (!prerender_handles_.empty()) {
    prerender_handles_.pop_front();
  }

  // This is the same as the page transition of WebView.loadUrl().
  auto page_transition = ui::PageTransitionFromInt(
      ui::PAGE_TRANSITION_TYPED | ui::PAGE_TRANSITION_FROM_API);

  std::string new_additional_headers =
      additional_headers.ToString() + "Sec-Purpose: prefetch;prerender\r\n";

  GURL gurl = GURL(url.ToString());
  if (gurl.is_empty() || !gurl.is_valid()) {
    LOG(DEBUG) << "PrerenderPage url is invalid, skip.";
    return OHOS::NWeb::NWEB_INVALID_URL;
  }
  if (!gurl.SchemeIsHTTPOrHTTPS()) {
    LOG(DEBUG) << "Prerenderring does not support the scheme of the URL, skip.";
    return OHOS::NWeb::NWEB_INVALID_URL;
  }

  // TODO(https://crbug.com/41490450): Do the following:
  // - Pass a valid PreloadingAttempt.
  // - Pass a valid navigation handle callback.
  // - Run multiple prerendering in a sequential manner, not in parallel.
  std::unique_ptr<content::PrerenderHandle> prerender_handle =
      web_contents->StartPrerendering(
          gurl, content::PreloadingTriggerType::kEmbedder,
          "ArkWeb",
          net::HttpRequestHeaders(),
          std::nullopt,
          page_transition,
          /*should_warm_up_compositor*/true, /*should_prepare_paint_tree*/true,
          content::PreloadingHoldbackStatus::kAllowed,
          /*preload_pipeline_info=*/nullptr,
          /*preloading_attempt=*/nullptr,
          /*url_match_predicate=*/{},
          /*prerender_navigation_handle_callback*/{},
          /*allow_reuse=*/false,
          /*extra_headers=*/new_additional_headers.c_str()
          );

  if (prerender_handle) {
    prerender_handles_.push_back(std::move(prerender_handle));
    LOG(DEBUG) << "PrerenderPage successfully.";
    return OHOS::NWeb::NWEB_OK;
  } else {
    LOG(DEBUG) << "PrerenderPage failed.";
    return ARKWEB_INIT_ERROR;
  }
}
 
void ArkWebBrowserHostExtImpl::CancelAllPrerendering() {
  prerender_handles_.clear();
}
#endif // BUILDFLAG(ARKWEB_NETWORK_LOAD)

#if BUILDFLAG(ARKWEB_I18N)
void ArkWebBrowserHostExtImpl::UpdateLocale(const CefString& locale) {
  std::string update_locale = locale.ToString();

  // need to notify renderer preference to change accepted_language.
  if (!GetWebContents()) {
    return;
  }

  auto prefs = GetWebContents()->GetMutableRendererPrefs();
  if (prefs->accept_languages.compare(update_locale)) {
    prefs->accept_languages = update_locale;
    GetWebContents()->SyncRendererPrefs();
  }

  if (!ui::ResourceBundle::HasSharedInstance() ||
      !ui::ResourceBundle::LocaleDataPakExists(update_locale, ui::ResourceBundle::Gender::kDefault)) {
    LOG(ERROR) << "CefBrowserHostBase::UpdateLocale !HasSharedInstance";
    return;
  }

  std::string origin_locale =
      ui::ResourceBundle::GetSharedInstance().GetLoadedLocale();
  LOG(DEBUG) << "UpdateLocale origin locale:" << origin_locale
             << ", update locale:" << update_locale;
  if (origin_locale == update_locale) {
    LOG(WARNING) << "CefBrowserHostBase::UpdateLocale no need to update locale";
    return;
  }

  // each render process may need to update locale.
  content::RenderProcessHost* host =
      GetWebContents()->GetPrimaryMainFrame()->GetProcess();
  if (host) {
    host->OnLocaleChangedToRenderer(update_locale);
  }

  const std::string result =
      ui::ResourceBundle::GetSharedInstance().ReloadLocaleResources(
          update_locale);

  if (result.empty()) {
    LOG(ERROR) << "browser host update locale failed";
    return;
  }
  g_browser_process->SetApplicationLocale(result);

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (!command_line->HasSwitch(::switches::kLang)) {
    return;
  }

  std::string current_lang =
      command_line->GetSwitchValueASCII(::switches::kLang);
  if (current_lang != result) {
    command_line->AppendSwitchASCII(switches::kLang, result);
  }
}
#endif  // BUILDFLAG(ARKWEB_I18N)

#if BUILDFLAG(ARKWEB_AI)
void ArkWebBrowserHostExtImpl::OnTextSelected(bool flag) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnTextSelected(flag);
  }
}

void ArkWebBrowserHostExtImpl::OnDestroyImageAnalyzerOverlay() {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnDestroyImageAnalyzerOverlay();
  }
}

void ArkWebBrowserHostExtImpl::OnFoldStatusChanged(uint32_t foldStatus) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnFoldStatusChanged(foldStatus);
  }
}

float ArkWebBrowserHostExtImpl::GetPageScaleFactor() {
  if (platform_delegate_) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->GetPageScaleFactor();
  }
  return 1;
}

std::string ArkWebBrowserHostExtImpl::GetDataDetectorSelectText() {
  if (platform_delegate_) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->GetDataDetectorSelectText();
  }
  return std::string();
}

void ArkWebBrowserHostExtImpl::OnDataDetectorSelectText() {
  auto web_contents = GetWebContents();
  if (web_contents && platform_delegate_) {
    web_contents->SetShowingContextMenu(false);
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnDataDetectorSelectText();
  }
}
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void ArkWebBrowserHostExtImpl::OnSafeInsetsChange(int left,
                                                  int top,
                                                  int right,
                                                  int bottom) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&ArkWebBrowserHostExtImpl::OnSafeInsetsChange,
                                 this, left, top, right, bottom));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->OnSafeInsetsChange(left, top, right, bottom);
  }
}
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void ArkWebBrowserHostExtImpl::GetImageForContextNode(CefRefPtr<CefFrame> frame, int command_id) {
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->GetImageForContextNode(command_id);
  }
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

void ArkWebBrowserHostExtImpl::GetImageFromCache(const CefString& url,
                                                 int command_id) {
  LOG(INFO) << "ArkWebBrowserHostExtImpl::GetImageFromCache";
  auto web_contents = GetWebContents();
  auto frame = GetMainFrame();
  if (web_contents && frame && frame->IsValid()) {
    content::RenderFrameHost* rfh = web_contents->GetPrimaryMainFrame();
    if (rfh) {
      LOG(INFO) << "ArkWebBrowserHostExtImpl::GetImageFromCache";
      rfh->GetImageFromCache(
          url.ToString(),
          base::BindOnce(&ArkwebFrameHostExtImpl::OnGetImageFromCache,
                         static_cast<ArkwebFrameHostExtImpl*>(frame.get()),
                         url.ToString(), command_id));
    }
  }
}

void ArkWebBrowserHostExtImpl::GetImageFromCacheEx(const CefString& url,
                                                   int command_id) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  LOG(INFO) << "CefBrowserHostBase::GetImageFromCacheEx";
  auto web_contents = GetWebContents();
  auto frame = GetMainFrame();
  if (web_contents && frame && frame->IsValid()) {
    content::RenderFrameHost* rfh = web_contents->GetPrimaryMainFrame();
   if (rfh) {
      LOG(INFO) << "ArkWebBrowserHostExtImpl::GetImageFromCacheEx";
      rfh->GetImageFromCache(
          url.ToString(),
          base::BindOnce(&ArkwebFrameHostExtImpl::OnGetImageFromCacheEx,
                         static_cast<ArkwebFrameHostExtImpl*>(frame.get()),
                         url.ToString(), command_id));
    }
   return;
  }
#endif
}

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
void ArkWebBrowserHostExtImpl::UpdateDrawRect() {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    frame.get()->AsArkWebFrame()->UpdateDrawRect();
  }
}
#endif

void ArkWebBrowserHostExtImpl::ScrollToWithAnime(float x,
                                                 float y,
                                                 int32_t duration) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->ScrollToWithAnime(x, y, duration);
  }
}

void ArkWebBrowserHostExtImpl::ScrollByWithAnime(float delta_x,
                                                 float delta_y,
                                                 int32_t duration) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->ScrollByWithAnime(delta_x, delta_y, duration);
  }
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void ArkWebBrowserHostExtImpl::GetOverScrollOffset(float* offset_x,
                                                   float* offset_y) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->GetOverScrollOffset(offset_x, offset_y);
  }
}
#endif

void ArkWebBrowserHostExtImpl::SetForceEnableZoom(bool forceEnableZoom) {
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
  if (!GetWebContents()) {
    return;
  }
  GetWebContents()->SetForceEnableZoom(forceEnableZoom);
#endif
}

bool ArkWebBrowserHostExtImpl::ShouldShowLoadingUI() {
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  auto wc = GetWebContents();
  if (wc) {
    return wc->ShouldShowLoadingUI();
  }
  return false;
#else
  return false;
#endif
}

bool ArkWebBrowserHostExtImpl::GetForceEnableZoom() {
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
  if (!GetWebContents()) {
    return false;
  }
  return GetWebContents()->GetForceEnableZoom();
#else
  return false;
#endif  // #if defined (ARKWEB_EXT_FORCE_ZOOM)
}
#if BUILDFLAG(ARKWEB_MSGPORT)
// in the std::map<std::pair<uint64_t, uint64_t>, std::pair<WebMessagePort,
// WebMessagePort>> portMap_; first is the paif of port_handles. second is the
// WebMessagePort of the pipe.
void ArkWebBrowserHostExtImpl::CreateWebMessagePorts(
    std::vector<CefString>& ports) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CHECK(false) << "called on invalid thread";
    return;
  }
  base::AutoLock msg_lock(web_message_lock_);
  auto web_contents = GetWebContents();
  if (web_contents) {
    int retry_times = 0;
    constexpr int MAX_RETRY_TIMES = 5;
    do {
      std::vector<blink::WebMessagePort> portArr;
      web_contents->CreateWebMessagePorts(portArr);
      if (portArr.size() != 2) {
        LOG(ERROR) << "CreateWebMessagePorts size wrong";
        return;
      }
      uint64_t pointer0 = base::RandUint64();
      if (pointer0 == ULLONG_MAX || (pointer0 + 1) == ULLONG_MAX) {
        retry_times++;
        continue;
      }
      pointer0 = ((pointer0 % 2) == 0) ? (pointer0 + 1) : pointer0;
      uint64_t pointer1 = pointer0 + 1;
      auto iter = portMap_.find(std::make_pair(pointer0, pointer1));
      if (iter == portMap_.end()) {
        portMap_[std::make_pair(pointer0, pointer1)] =
            std::make_pair(std::move(portArr[0]), std::move(portArr[1]));
        ports.emplace_back(std::to_string(pointer0));
        ports.emplace_back(std::to_string(pointer1));
        return;
      }
      retry_times++;
    } while (retry_times < MAX_RETRY_TIMES);
  } else {
    LOG(ERROR) << "CreateWebMessagePorts web_contents its null";
  }
}

// in the std::map<std::pair<uint64_t, uint64_t>, std::pair<WebMessagePort,
// WebMessagePort>> portMap_; first is the paif of port_handles. second is the
// WebMessagePort of the pipe.
void ArkWebBrowserHostExtImpl::PostWebMessage(
    CefString& message,
    std::vector<CefString>& port_handles,
    CefString& targetUri) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CHECK(false) << "called on invalid thread";
    return;
  }
  base::AutoLock msg_lock(web_message_lock_);
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "PostWebMessage web_contents its null";
    return;
  }

  std::string msg = message.empty() ? "" : message.ToString();
  std::string uri = targetUri.empty() ? "" : targetUri.ToString();
  // check whether the port is already send to html5.
  for (CefString port : port_handles) {
    auto found = postedPorts_.find(port.ToString());
    if (found != postedPorts_.end()) {
      LOG(ERROR) << "This port has alrady send to h5, can not post again.";
      return;
    }
  }

  // find the WebMessagePort by port_handle, and send to html5
  std::vector<blink::WebMessagePort> sendPorts;
  for (CefString port : port_handles) {
    LOG(DEBUG) << "PostWebMessage port:" << port.ToString();
    for (auto iter = portMap_.begin(); iter != portMap_.end(); ++iter) {
      if (port.ToString().compare(std::to_string(iter->first.first)) == 0) {
        postedPorts_.insert(port.ToString());
        sendPorts.emplace_back(std::move(iter->second.first));
      } else if (port.ToString().compare(std::to_string(iter->first.second)) ==
                 0) {
        postedPorts_.insert(port.ToString());
        sendPorts.emplace_back(std::move(iter->second.second));
      }
    }
  }

  // send to html5 main frame
  if (sendPorts.size() >= 1) {
    web_contents->PostWebMessage(msg, sendPorts, uri);
  }
}

// in the std::map<std::pair<uint64_t, uint64_t>, std::pair<WebMessagePort,
// WebMessagePort>> portMap_; first is the paif of port_handles. second is the
// WebMessagePort of the pipe.
void ArkWebBrowserHostExtImpl::ClosePortInternal(const CefString& portHandle) {
  base::AutoLock msg_lock(web_message_lock_);
  LOG(DEBUG) << "ClosePort Start";

  // find port and close, then erase the item in map
  for (auto iter = portMap_.begin(); iter != portMap_.end(); ++iter) {
    if (portHandle.ToString().compare(std::to_string(iter->first.first)) == 0 ||
        portHandle.ToString().compare(std::to_string(iter->first.second)) == 0) {
      iter->second.first.Close();
      iter->second.second.Close();
      portMap_.erase(iter);
      break;
    }
  }

  for (auto iter = receiverMap_.begin(); iter != receiverMap_.end(); ++iter) {
    if (portHandle.ToString().compare(iter->first) == 0) {
      receiverMap_.erase(iter);
      break;
    }
  }

  postedPorts_.erase(portHandle.ToString());
  LOG(DEBUG) << "ClosePort end";
  Release();
}

void ArkWebBrowserHostExtImpl::ClosePort(const CefString& portHandle) {
  AddRef();
  sequenced_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&ArkWebBrowserHostExtImpl::ClosePortInternal,
                                this, portHandle));
}

bool ArkWebBrowserHostExtImpl::ConvertCefValueToBlinkMsg(
    CefRefPtr<CefValue>& original,
    blink::WebMessagePort::Message& message) {
  LOG(DEBUG) << "ConvertCefValueToBlinkMsg type:" << (int)original->GetType();
  switch (original->GetType()) {
    case VTYPE_STRING: {
      message = blink::WebMessagePort::Message(
          base::UTF8ToUTF16(original->GetString().ToString()));
      message.type_ = blink::WebMessagePort::Message::MessageType::STRING;
      break;
    }
    case VTYPE_BINARY: {
      CefRefPtr<CefBinaryValue> binValue = original->GetBinary();
      size_t len = binValue->GetSize();
      std::vector<uint8_t> arr(len);
      binValue->GetData(&arr[0], len, 0);
      message = blink::WebMessagePort::Message(std::move(arr));
      message.type_ = blink::WebMessagePort::Message::MessageType::BINARY;
      break;
    }
    case VTYPE_BOOL: {
      message.bool_value_ = original->GetBool();
      message.type_ = blink::WebMessagePort::Message::MessageType::BOOLEAN;
      break;
    }
    case VTYPE_INT: {
      message.int64_value_ = original->GetInt();
      message.type_ = blink::WebMessagePort::Message::MessageType::INTEGER;
      break;
    }
    case VTYPE_DOUBLE: {
      message.double_value_ = original->GetDouble();
      message.type_ = blink::WebMessagePort::Message::MessageType::DOUBLE;
      break;
    }
    case VTYPE_LIST: {
      CefRefPtr<CefListValue> value = original->GetList();
      CefValueType type = value->GetType(0);
      size_t len = value->GetSize();
      switch (type) {
        case VTYPE_STRING: {
          std::vector<std::u16string> msg_arr;
          for (size_t i = 0; i < len; i++) {
            msg_arr.push_back(
                base::UTF8ToUTF16(value->GetString(i).ToString()));
          }
          message.string_arr_ = std::move(msg_arr);
          message.type_ =
              blink::WebMessagePort::Message::MessageType::STRINGARRAY;
          break;
        }
        case VTYPE_BOOL: {
          std::vector<bool> msg_arr;
          for (size_t i = 0; i < len; i++) {
            msg_arr.push_back(value->GetBool(i));
          }
          message.bool_arr_ = std::move(msg_arr);
          message.type_ =
              blink::WebMessagePort::Message::MessageType::BOOLEANARRAY;
          break;
        }
        case VTYPE_INT: {
          std::vector<int64_t> msg_arr;
          for (size_t i = 0; i < len; i++) {
            msg_arr.push_back(value->GetInt(i));
          }
          message.int64_arr_ = std::move(msg_arr);
          message.type_ =
              blink::WebMessagePort::Message::MessageType::INT64ARRAY;
          break;
        }
        case VTYPE_DOUBLE: {
          std::vector<double> msg_arr;
          for (size_t i = 0; i < len; i++) {
            msg_arr.push_back(value->GetDouble(i));
          }
          message.double_arr_ = std::move(msg_arr);
          message.type_ =
              blink::WebMessagePort::Message::MessageType::DOUBLEARRAY;
          break;
        }
        default:
          LOG(ERROR) << "Only support string, bool, int, double";
          break;
      }
      break;
    }
    case VTYPE_DICTIONARY: {
      CefRefPtr<CefDictionaryValue> dict = original->GetDictionary();
      std::u16string err_name = u"";
      std::u16string err_msg = u"";
      if (dict->HasKey("Error.name")) {
        err_name = base::UTF8ToUTF16(dict->GetString("Error.name").ToString());
      }
      if (dict->HasKey("Error.message")) {
        err_msg =
            base::UTF8ToUTF16(dict->GetString("Error.message").ToString());
      }
      message.err_name_ = std::move(err_name);
      message.err_msg_ = std::move(err_msg);
      message.type_ = blink::WebMessagePort::Message::MessageType::ERROR;
      break;
    }
    default: {
      LOG(ERROR) << "Not support type:" << (int)original->GetType();
      break;
    }
  }
  return true;
}

void ArkWebBrowserHostExtImpl::PostPortMessageInternal(
    const CefString& portHandle,
    CefRefPtr<CefValue> data) {
  base::ScopedAllowBlocking scoped_allow_blocking;
  base::AutoLock msg_lock(web_message_lock_);
  // construct blink message
  blink::WebMessagePort::Message message;
  if (!ConvertCefValueToBlinkMsg(data, message)) {
    LOG(ERROR) << "Post meessage: convert cef value to blink message failed";
    return;
  }

  // find the WebMessagePort in map
  for (auto iter = portMap_.begin(); iter != portMap_.end(); ++iter) {
    if (portHandle.ToString().compare(std::to_string(iter->first.first)) == 0) {
      if (iter->second.first.CanPostMessage()) {
        iter->second.first.PostMessage(std::move(message));
      } else {
        LOG(ERROR) << "port can not post messsage";
      }
      break;
    } else if (portHandle.ToString().compare(
                   std::to_string(iter->first.second)) == 0) {
      if (iter->second.second.CanPostMessage()) {
        iter->second.second.PostMessage(std::move(message));
      } else {
        LOG(ERROR) << "port can not post messsage";
      }
      break;
    }
  }
}

void ArkWebBrowserHostExtImpl::PostPortMessage(const CefString& portHandle,
                                               CefRefPtr<CefValue> data) {
  sequenced_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&ArkWebBrowserHostExtImpl::PostPortMessageInternal, this,
                     portHandle, data));
}

void ArkWebBrowserHostExtImpl::SetPortMessageCallbackInternal(
    const CefString& portHandle,
    CefRefPtr<CefWebMessageReceiver> callback) {
  base::ScopedAllowBlocking scoped_allow_blocking;
  base::AutoLock msg_lock(web_message_lock_);
  std::string pointer0 = portHandle.ToString();

  // get web message receiver instance
  std::shared_ptr<WebMessageReceiverImpl> webMsgReceiver;
  auto receive_it = receiverMap_.find(pointer0);
  if (receive_it != receiverMap_.end()) {
    webMsgReceiver = receive_it->second;
  } else {
    webMsgReceiver = std::make_shared<WebMessageReceiverImpl>();
  }

  webMsgReceiver->SetOnMessageCallback(callback);

  // find the port set message callback
  for (auto iter = portMap_.begin(); iter != portMap_.end(); ++iter) {
    if (portHandle.ToString().compare(std::to_string(iter->first.first)) == 0) {
      if (iter->second.first.HasReceiver()) {
        iter->second.first.ClearReceiver();
      }
      iter->second.first.SetReceiver(webMsgReceiver.get(),
                                     sequenced_task_runner_);
      break;
    } else if (portHandle.ToString().compare(
                   std::to_string(iter->first.second)) == 0) {
      if (iter->second.second.HasReceiver()) {
        iter->second.second.ClearReceiver();
      }
      iter->second.second.SetReceiver(webMsgReceiver.get(),
                                      sequenced_task_runner_);
      break;
    }
  }

  // save in map
  receiverMap_[pointer0] = webMsgReceiver;
}

// in the std::map<std::pair<uint64_t, uint64_t>, std::pair<WebMessagePort,
// WebMessagePort>> portMap_; first is the paif of port_handles. second is the
// WebMessagePort of the pipe.
void ArkWebBrowserHostExtImpl::SetPortMessageCallback(
    const CefString& portHandle,
    CefRefPtr<CefWebMessageReceiver> callback) {
  sequenced_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&ArkWebBrowserHostExtImpl::SetPortMessageCallbackInternal,
                     this, portHandle, callback));
}

void ArkWebBrowserHostExtImpl::DestroyAllWebMessagePorts() {
  base::AutoLock msg_lock(web_message_lock_);
  LOG(INFO) << "clear all message ports";
  for (auto& port : portMap_) {
    CefString handleCef;
    handleCef.FromString(std::to_string(port.first.first));
    ClosePort(handleCef);
  }
}
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

#if BUILDFLAG(ARKWEB_MSGPORT)
WebMessageReceiverImpl::~WebMessageReceiverImpl() {
  LOG(DEBUG) << "WebMessageReceiverImpl destrory";
}

void WebMessageReceiverImpl::SetOnMessageCallback(
    CefRefPtr<CefWebMessageReceiver> callback) {
  LOG(DEBUG) << "WebMessageReceiverImpl::SetOnMessageCallback ";
  callback_ = callback;
}

void WebMessageReceiverImpl::ConvertBlinkMsgToCefValue(
    blink::WebMessagePort::Message& message,
    CefRefPtr<CefValue> data) {
  switch (message.type_) {
    case blink::WebMessagePort::Message::MessageType::STRING: {
      data->SetString(base::UTF16ToUTF8(message.data));
      break;
    }
    case blink::WebMessagePort::Message::MessageType::BINARY: {
      std::vector<uint8_t> vecBinary = message.array_buffer;
      CefRefPtr<CefBinaryValue> value =
          CefBinaryValue::Create(vecBinary.data(), vecBinary.size());
      data->SetBinary(value);
      break;
    }
    case blink::WebMessagePort::Message::MessageType::DOUBLE: {
      data->SetDouble((message.double_value_));
      break;
    }
    case blink::WebMessagePort::Message::MessageType::BOOLEAN: {
      data->SetBool((message.bool_value_));
      break;
    }
    case blink::WebMessagePort::Message::MessageType::INTEGER: {
      data->SetInt((message.int64_value_));
      break;
    }
    case blink::WebMessagePort::Message::MessageType::STRINGARRAY: {
      CefRefPtr<CefListValue> value = CefListValue::Create();
      for (size_t i = 0; i < message.string_arr_.size(); i++) {
        value->SetString(i, base::UTF16ToUTF8(message.string_arr_[i]));
      }
      data->SetList(value);
      break;
    }
    case blink::WebMessagePort::Message::MessageType::BOOLEANARRAY: {
      CefRefPtr<CefListValue> value = CefListValue::Create();
      for (size_t i = 0; i < message.bool_arr_.size(); i++) {
        value->SetBool(i, message.bool_arr_[i]);
      }
      data->SetList(value);
      break;
    }
    case blink::WebMessagePort::Message::MessageType::DOUBLEARRAY: {
      CefRefPtr<CefListValue> value = CefListValue::Create();
      for (size_t i = 0; i < message.double_arr_.size(); i++) {
        value->SetDouble(i, message.double_arr_[i]);
      }
      data->SetList(value);
      break;
    }
    case blink::WebMessagePort::Message::MessageType::INT64ARRAY: {
      CefRefPtr<CefListValue> value = CefListValue::Create();
      for (size_t i = 0; i < message.int64_arr_.size(); i++) {
        value->SetInt(i, message.int64_arr_[i]);
      }
      data->SetList(value);
      break;
    }
    case blink::WebMessagePort::Message::MessageType::ERROR: {
      std::u16string err_name = message.err_name_;
      std::u16string err_msg = message.err_msg_;
      CefRefPtr<CefDictionaryValue> dict = CefDictionaryValue::Create();
      dict->SetString("Error.name", err_name);
      dict->SetString("Error.message", err_msg);
      data->SetDictionary(dict);
      break;
    }
    default:
      break;
  }
}

// this will receive message from html5
bool WebMessageReceiverImpl::OnMessage(blink::WebMessagePort::Message message) {
  LOG(DEBUG) << "OnMessage start";
  // Pass the message on to the receiver.
  if (callback_) {
    CefRefPtr<CefValue> data = CefValue::Create();
    ConvertBlinkMsgToCefValue(message, data);
    callback_->OnMessage(data);
  } else {
    LOG(ERROR) << "u should set callback to receive message";
  }
  return true;
}
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

#if BUILDFLAG(ARKWEB_MSGPORT)
// static
bool ValidateResultType(base::Value::Type type) {
  if (type == base::Value::Type::STRING || type == base::Value::Type::DOUBLE ||
      type == base::Value::Type::INTEGER ||
      type == base::Value::Type::BOOLEAN) {
    return true;
  }
  return false;
}

void ArkWebBrowserHostExtImpl::ExecuteJSCallback(
    CefRefPtr<CefJavaScriptResultCallback> callback,
    base::Value dict_result) {
  LOG(DEBUG) << "javascript result callback enter";
  const auto& dict = dict_result.GetIfDict();
  if (!dict || !dict->Find("result")) {
    return;
  }

  base::Value result = std::move(*dict->Find("result"));
  std::string json;
  base::JSONWriter::Write(result, &json);
  if (callback != nullptr) {
    CefRefPtr<CefValue> data = CefValue::Create();
    data->SetString(json);
    callback->OnJavaScriptExeResult(data);
  }
}

void ArkWebBrowserHostExtImpl::ExecuteExtensionJSCallback(
    CefRefPtr<CefJavaScriptResultCallback> callback,
    base::Value dict_result) {
  const auto& dict = dict_result.GetIfDict();
  if (!dict || !dict->Find("result")) {
    return;
  }

  std::string* exception = dict->FindString("exception");
  base::Value result = std::move(*dict->Find("result"));
  std::optional<bool> isObject = dict->FindBool("isObject");
  LOG(DEBUG) << "javascript result callback enter, type:"
             << result.GetTypeName(result.type());
  std::string json;
  base::JSONWriter::Write(result, &json);
  CefRefPtr<CefValue> data = CefValue::Create();
  switch (result.type()) {
    case base::Value::Type::STRING: {
      data->SetString(json);
      break;
    }
    case base::Value::Type::DOUBLE: {
      data->SetDouble(result.GetDouble());
      break;
    }
    case base::Value::Type::INTEGER: {
      data->SetDouble(result.GetInt());
      break;
    }
    case base::Value::Type::BOOLEAN: {
      data->SetBool(result.GetBool());
      break;
    }
    case base::Value::Type::BINARY: {
      std::vector<uint8_t> vec = result.GetBlob();
      CefRefPtr<CefBinaryValue> value =
          CefBinaryValue::Create(vec.data(), vec.size());
      data->SetBinary(value);
      break;
    }
    case base::Value::Type::LIST: {
      int len = result.GetList().size();
      CefRefPtr<CefListValue> value = CefListValue::Create();
      base::Value::Type typeFirst = base::Value::Type::NONE;
      base::Value::Type typeCur = base::Value::Type::NONE;
      bool support = true;
      for (int i = 0; i < len; i++) {
        base::Value list_ele = std::move(result.GetList()[i]);
        typeCur = list_ele.type();
        if (!ValidateResultType(typeCur)) {
          data->SetString(
              "This type not support, only string/number/boolean "
              "is supported for array elements");
          support = false;
          break;
        }
        if (i == 0) {
          typeFirst = typeCur;
        }
        if (typeCur != typeFirst) {
          support = false;
          data->SetString(
              "This type not support, The elements in the array "
              "must be the same.");
          break;
        }
        switch (list_ele.type()) {
          case base::Value::Type::STRING: {
            CefString msgCef;
            msgCef.FromString(list_ele.GetString());
            value->SetString(i, msgCef);
            break;
          }
          case base::Value::Type::DOUBLE: {
            value->SetDouble(i, list_ele.GetDouble());
            break;
          }
          case base::Value::Type::INTEGER: {
            value->SetInt(i, list_ele.GetInt());
            break;
          }
          case base::Value::Type::BOOLEAN: {
            value->SetBool(i, list_ele.GetBool());
            break;
          }
          default: {
            LOG(ERROR) << "Not support type";
            support = false;
            data->SetString(
                "This type not support, only "
                "string/number/boolean is supported for array "
                "elements");
            break;
          }
        }
      }
      if (support) {
        data->SetList(value);
      }
      break;
    }
    default: {
      LOG(ERROR) << "base::Value not support type:" << result.type();
      if (exception) {
        callback->SetErrorDescription(*exception);
      } else if (isObject.value_or(false)) {
        callback->SetErrorDescription(json);
      }
      data->SetString(
          "This type not support, only "
          "string/number/boolean/arraybuffer/array is supported");
      break;
    }
  }

  if (callback != nullptr) {
    callback->OnJavaScriptExeResult(data);
  }
}

void ArkWebBrowserHostExtImpl::ExecuteJavaScript(
    const std::string& code,
    CefRefPtr<CefJavaScriptResultCallback> callback,
    bool extention) {
  auto web_contents = GetWebContents();
  // enable inject javaScript
  LOG(DEBUG) << "ExecuteJavaScript with callback enter";
  if (web_contents && web_contents->GetPrimaryMainFrame()) {
    LOG(DEBUG) << "ExecuteJavaScript with callback";
    web_contents->GetPrimaryMainFrame()->AllowInjectingJavaScript();
    if (!extention) {
      web_contents->GetPrimaryMainFrame()->ExecuteJavaScript(
          base::UTF8ToUTF16(code),
          base::BindOnce(&ArkWebBrowserHostExtImpl::ExecuteJSCallback, this,
                         callback));
    } else {
      web_contents->GetPrimaryMainFrame()->ExecuteJavaScript(
          base::UTF8ToUTF16(code),
          base::BindOnce(&ArkWebBrowserHostExtImpl::ExecuteExtensionJSCallback,
                         this, callback));
    }
  }
}

void ArkWebBrowserHostExtImpl::ExecuteJavaScriptExt(
    const int fd,
    const uint64_t scriptLength,
    CefRefPtr<CefJavaScriptResultCallback> callback,
    bool extention) {
  auto web_contents = GetWebContents();
  // enable inject javaScript
  LOG(DEBUG) << "ExecuteJavaScriptExt with callback enter";
  if (web_contents && web_contents->GetPrimaryMainFrame()) {
    LOG(DEBUG) << "ExecuteJavaScriptExt with callback";
    web_contents->GetPrimaryMainFrame()->AllowInjectingJavaScript();
    if (!extention) {
      web_contents->GetPrimaryMainFrame()->ExecuteJavaScriptExt(
          fd, scriptLength,
          base::BindOnce(&ArkWebBrowserHostExtImpl::ExecuteJSCallback, this,
                         callback));
    } else {
      web_contents->GetPrimaryMainFrame()->ExecuteJavaScriptExt(
          fd, scriptLength,
          base::BindOnce(&ArkWebBrowserHostExtImpl::ExecuteExtensionJSCallback,
                         this, callback));
    }
  }
}
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
CefString ArkWebBrowserHostExtImpl::GetLastJavascriptProxyCallingFrameUrl() {
  if (!GetWebContents()) {
    return base::EmptyString();
  }

  NWEB::OhJavascriptInjector* javascriptInjector =
      NWEB::OhJavascriptInjector::FromWebContents(GetWebContents());
  if (!javascriptInjector) {
    return base::EmptyString();
  }
  // return javascriptInjector->GetLastCallingFrameUrl();
  return "";
}
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
void ArkWebBrowserHostExtImpl::SetNeedsReload(bool needs_reload) {
  if (is_hidden_ && needs_reload) {
    GetWebContents()->GetController().SetNeedsReload();
  }
  LOG(INFO) << "Set needs reload: " << needs_reload;
  needs_reload_ = needs_reload;
}

bool ArkWebBrowserHostExtImpl::NeedsReload() {
  return needs_reload_;
}
#endif  // BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
int ArkWebBrowserHostExtImpl::SetUrlTrustListWithErrMsg(
    const CefString& urlTrustList,
    CefString& detailErrMsg) {
  std::string urlTrustListUpdated = urlTrustList.ToString();
  content::WebContents* webContents = GetWebContents();
  std::string detailErrMsgUpdated;
  if (!webContents) {
    LOG(ERROR) << "SetUrlTrustListWithErrMsg failed, web contents is error.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR)
        << "SetUrlTrustListWithErrMsg failed, web contents is error.";
#endif
    return static_cast<int>(ohos_safe_browsing::UrlListSetResult::INIT_ERROR);
  }
  ohos_safe_browsing::UrlTrustListManager* manager =
      reinterpret_cast<ohos_safe_browsing::UrlTrustListManager*>(
          webContents->GetUserData(
              &ohos_safe_browsing::UrlTrustListInterface::interfaceKey));
  if (!manager) {
    manager = new ohos_safe_browsing::UrlTrustListManager();
    if (!manager) {
      LOG(ERROR) << "SetUrlTrustListWithErrMsg failed, new UrlTrustListManager "
                    "failed.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
      LOG_FEEDBACK(ERROR) << "SetUrlTrustListWithErrMsg failed, new "
                             "UrlTrustListManager failed.";
#endif
      return static_cast<int>(ohos_safe_browsing::UrlListSetResult::INIT_ERROR);
    }
    webContents->SetUserData(
        &ohos_safe_browsing::UrlTrustListInterface::interfaceKey,
        std::unique_ptr<base::SupportsUserData::Data>(manager));
  }
  int res = static_cast<int>(manager->SetUrlTrustListWithErrMsg(
      urlTrustListUpdated, detailErrMsgUpdated));
  detailErrMsg.FromString(detailErrMsgUpdated);
  return res;
}
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void ArkWebBrowserHostExtImpl::ProcessAutofillCancel(
    const CefString& fillContent) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "GetWebContents null";
#endif
    return;
  }

  OhPasswordManagerClient* password_manager_client =
      OhPasswordManagerClient::FromWebContents(web_contents);
  if (password_manager_client) {
    password_manager_client->AsChromePasswordManagerClientExt()->ProcessAutofillCancel(fillContent);
  }
}

void ArkWebBrowserHostExtImpl::AutoFillWithIMFEvent(bool is_username,
                                                    bool is_other_account,
                                                    bool is_new_password,
                                                    const CefString& content) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "GetWebContents null";
#endif
    return;
  }

  OhPasswordManagerClient* password_manager_client =
      OhPasswordManagerClient::FromWebContents(web_contents);
  if (password_manager_client) {
    password_manager_client->AsChromePasswordManagerClientExt()->AutoFillWithIMFEvent(is_username, is_other_account,
                                                                                      is_new_password, content);
  }
}
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool ArkWebBrowserHostExtImpl::IsNeedZoomChange(
    const input::NativeWebKeyboardEvent& event,
    bool& zoom_in) {
  if (!(event.GetModifiers() & blink::WebKeyboardEvent::kControlKey) ||
      (event.GetType() != blink::WebKeyboardEvent::Type::kRawKeyDown)) {
    LOG(DEBUG) << "not control key down";
    return false;
  }
  int32_t native_key_code = event.native_key_code;
  if (native_key_code == OHOS::NWeb::ScanKeyCode::EQUAL_SCAN_CODE ||
      native_key_code == OHOS::NWeb::ScanKeyCode::NUMPADADD_SCAN_CODE) {
    zoom_in = true;
    return true;
  }
  if (native_key_code == OHOS::NWeb::ScanKeyCode::MINUS_SCAN_CODE ||
      native_key_code == OHOS::NWeb::ScanKeyCode::NUMPADSUBTRACT_SCAN_CODE) {
    zoom_in = false;
    return true;
  }
  return false;
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkWebBrowserHostExtImpl::SendMouseClickEvent(const CefMouseEvent& event,
                                                   MouseButtonType type,
                                                   bool mouseUp,
                                                   int clickCount) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&CefBrowserHostBase::SendMouseClickEvent, this,
                                 event, type, mouseUp, clickCount));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->SendMouseClickEvent(event, type, mouseUp, clickCount);
  }

  if (mouseUp) {
    return;
  }

  float ratio = GetVirtualPixelRatio();
  if (ratio <= 0) {
    LOG(ERROR) << "get ratio invalid: " << ratio;
    return;
  }
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<CefFrameHostImpl*>(frame.get())
        ->SendHitEvent(event.x * ratio, event.y * ratio, 0, 0);
  }
}

void ArkWebBrowserHostExtImpl::ResumeDownload(
    const CefString& url,
    const CefString& full_path,
    int64_t received_bytes,
    int64_t total_bytes,
    const CefString& etag,
    const CefString& mime_type,
    const CefString& last_modified,
    const CefString& received_slices_string) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  LOG(DEBUG) << "ArkWebBrowserHostExtImpl::ResumeDownload";
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&ArkWebBrowserHostExtImpl::ResumeDownload, this, url,
                       full_path, received_bytes, total_bytes, etag, mime_type,
                       last_modified, received_slices_string));
    return;
  }
  GURL gurl = GURL(url.ToString());
  if (gurl.is_empty() || !gurl.is_valid()) {
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    return;
  }

  auto browser_context = web_contents->GetBrowserContext();
  if (!browser_context) {
    return;
  }

  content::DownloadManager* manager = browser_context->GetDownloadManager();
  if (!manager) {
    return;
  }

  base::FilePath file_full_path = base::FilePath::FromUTF8Unsafe(
      full_path.ToString());  // FILE_PATH_LITERAL()

  std::vector<download::DownloadItem::ReceivedSlice> received_slices =
      arkweb_received_slice_helper_ext::FromString(
          received_slices_string.ToString());
  manager->GetNextId(
      base::BindOnce(&ArkWebBrowserHostExtImpl::ResumeDownloadWithId,
                     weak_ptr_factory_.GetWeakPtr(), std::move(gurl),
                     std::move(file_full_path), received_bytes, total_bytes,
                     etag, mime_type, last_modified, received_slices));
#endif
}

#if BUILDFLAG(ARKWEB_TERMINATE_RENDER)
bool ArkWebBrowserHostExtImpl::TerminateRenderProcess() {
  bool result = false;
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->TerminateRenderProcess(result);
    LOG(INFO) << "TerminateRenderProcess result:" << result;
  }
  return result;
}
#endif

void ArkWebBrowserHostExtImpl::ResumeDownloadWithId(
    const GURL& gurl,
    const base::FilePath& full_path,
    int64_t received_bytes,
    int64_t total_bytes,
    const std::string& etag,
    const std::string& mime_type,
    const std::string& last_modified,
    std::vector<download::DownloadItem::ReceivedSlice> received_slices,
    uint32_t next_id) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  LOG(DEBUG) << "ArkWebBrowserHostExtImpl::ResumeDownloadWithId url:"
             << gurl.spec();
  auto web_contents = GetWebContents();
  if (!web_contents) {
    return;
  }

  auto browser_context = web_contents->GetBrowserContext();
  if (!browser_context) {
    return;
  }

  content::DownloadManager* manager = browser_context->GetDownloadManager();
  if (!manager) {
    return;
  }
  std::vector<GURL> url_chain;
  url_chain.push_back(gurl);
  auto download_item = manager->CreateDownloadItem(
      std::string("") /*base::GenerateGUID()*/,     /*guid*/
      next_id,                                      /*id*/
      full_path,                                    /*current_path*/
      full_path,                                    /*target_path*/
      url_chain,                                    /*url_chain*/
      GURL(),                                       /*referrer_url*/
      content::StoragePartitionConfig(),            /*storage_partition_config*/
      GURL(),                                       /*tab_url*/
      GURL(),                                       /*tab_referrer_url*/
      url::Origin(),                                /*request_initiator*/
      mime_type,                                    /*mime_type*/
      mime_type,                                    /*original_mime_type*/
      base::Time::Now(),                            /*start_time*/
      base::Time::Now(),                            /*end_time*/
      etag,                                         /*etag*/
      last_modified,                                /*last_modified*/
      received_bytes,                               /*received_bytes*/
      total_bytes,                                  /*total_bytes*/
      std::string(),                                /*hash*/
      download::DownloadItem::INTERRUPTED,          /*state*/
      download::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS, /*danger_type*/
      download::DOWNLOAD_INTERRUPT_REASON_NETWORK_FAILED, /*interrupt_reason*/
      false,                                              /*opened*/
      base::Time(),                                       /*last_access_time*/
      false,                                              /*transient*/
      received_slices                                     /*received_slices*/
  );
  if (download_item) {
    auto browser = GetBrowser();
    if (browser) {
      int nweb_id = GetNWebId();
      download_item->SetUserData(
          kNWebId,
          std::make_unique<download::ArkWebDownloadItemImplExt::NWebIdData>(
              nweb_id));
      download_item->Resume(true /*is_user_resume*/);
      return;
    }
  }
#endif  //  ARKWEB_EX_DOWNLOAD
}

void ArkWebBrowserHostExtImpl::SetVirtualKeyBoardArg(int32_t width,
                                                     int32_t height,
                                                     double keyboard) {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SetVirtualKeyBoardArg(width, height, keyboard);
  }
}

bool ArkWebBrowserHostExtImpl::ShouldVirtualKeyboardOverlay() {
  if (platform_delegate_) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->ShouldVirtualKeyboardOverlay();
  }
  return false;
}

#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
bool ArkWebBrowserHostExtImpl::WebPageSnapshot(
    const char* id,
    int width,
    int height,
    cef_web_snapshot_callback_t callback) {
  if (platform_delegate_) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->WebPageSnapshot(id, width, height,
                                               std::move(callback));
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_OPTIMIZE_PARSER_BUDGET)
void ArkWebBrowserHostExtImpl::SetOptimizeParserBudgetEnabled(bool enable) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<CefFrameHostImpl*>(frame.get())
        ->SetOptimizeParserBudgetEnabled(enable);
  }
}
#endif

#if BUILDFLAG(ARKWEB_TOUCHPAD_FLING)
void ArkWebBrowserHostExtImpl::SendTouchpadFlingEvent(
    const CefMouseEvent& event,
    double vx,
    double vy) {
  if (vx == 0 && vy == 0) {
    // Nothing to do.
    return;
  }

  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&ArkWebBrowserHostExtImpl::SendTouchpadFlingEvent, this,
                       event, vx, vy));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->SendTouchpadFlingEvent(event, vx, vy);
  }
}
#endif
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
void ArkWebBrowserHostExtImpl::PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) {
  if (!GetWebContents()) {
    return;
  }

  prerender::NoStatePrefetchManager* no_state_prefetch_manager =
      prerender::NoStatePrefetchManagerFactory::GetForBrowserContext(
          GetWebContents()->GetBrowserContext());
  if (!no_state_prefetch_manager) {
    return;
  }
  content::SessionStorageNamespace* session_storage_namespace =
      GetWebContents()->GetController().GetDefaultSessionStorageNamespace();
  gfx::Size size = GetWebContents()->GetContainerBounds().size();

  std::string prefetch_url = prefetch_options.url_cef;
  std::string additional_http_headers = prefetch_options.additional_http_headers_cef;
  no_state_prefetch_manager->SetMinTimeBetweenPrefetchesMs(
    prefetch_options.min_time_between_prefetches);
  no_state_prefetch_manager->SetIgnoreCacheControlNoStore(
    prefetch_options.ignore_cache_control_no_store);
  no_state_prefetch_manager->StartOhPrefetchingFromOmnibox(
      GURL(prefetch_url), session_storage_namespace, size, nullptr,
      additional_http_headers);
}
#endif  // defined(ARKWEB_NO_STATE_PREFETCH)

#if BUILDFLAG(ARKWEB_ADBLOCK)
void ArkWebBrowserHostExtImpl::UpdateAdblockEasyListRules(
    long adBlockEasyListVersion) {
  adblock::UpdateAdblockEasyListRules(adBlockEasyListVersion);
}

void ArkWebBrowserHostExtImpl::EnableAdsBlock(bool enable) {
  if (!GetWebContents()) {
    return;
  }
  GetWebContents()->EnableAdsBlock(enable);

  LOG(INFO) << "web adblock enabled : " << enable;

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "web adblock enabled :" << enable;
#endif

  OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
  if (!OHOS::adblock::AdBlockConfig::GetInstance()
           ->GetUserEasylistReplaceSwitch() &&
      enable) {
    LOG(INFO) << "[Adblock] enable cloud control for easylist";

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "[Adblock] enable cloud control for easylist";
#endif

    ohos_adblock::AdblockConfigBridge::GetInstance()->EnableAdsBlock(
        GetBrowserContext(), true);
  } else {
    ohos_adblock::AdblockConfigBridge::GetInstance()->EnableAdsBlock(
        GetBrowserContext(), false);
  }
}

bool ArkWebBrowserHostExtImpl::IsAdsBlockEnabled() {
  if (!GetWebContents()) {
    return false;
  }

  return GetWebContents()->IsAdsBlockEnabled();
}

bool ArkWebBrowserHostExtImpl::IsAdsBlockEnabledForCurPage() {
  if (!GetWebContents()) {
    return false;
  }

  return GetWebContents()->IsAdsBlockEnabledForCurPage();
}

void ArkWebBrowserHostExtImpl::SetAdBlockEnabledForSite(
    bool is_adblock_enabled,
    int main_frame_tree_node_id) {
  if (!GetWebContents()) {
    LOG(ERROR) << "[AdBlock] SetAdBlockEnabledForSite get webcontents null";
    return;
  }

  GetWebContents()->SetAdBlockEnabledForSite(is_adblock_enabled,
                                             main_frame_tree_node_id);
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
void ArkWebBrowserHostExtImpl::RegisterNativeJSProxy(
    const CefString& object_name,
    const std::vector<CefString>& method_list,
    const int32_t object_id,
    bool is_async,
    const CefString& permission) {
  NWEB::OhJavascriptInjector* javascriptInjector =
      NWEB::OhJavascriptInjector::FromWebContents(GetWebContents());
  if (!javascriptInjector) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::RegisterNativeJSProxy "
                  "javascriptInjector is null";
    return;
  }
  std::vector<std::string> method_vector;
  for (CefString method : method_list) {
    method_vector.push_back(method.ToString());
  }
  // javascriptInjector->AddNativeInterface(object_name.ToString(), method_vector,
  //                                        object_id, is_async, permission);
}

void ArkWebBrowserHostExtImpl::RegisterArkJSfunction(
    const CefString& object_name,
    const std::vector<CefString>& method_list,
    const std::vector<CefString>& async_method_list,
    const int32_t object_id,
    const CefString& permission) {
  NWEB::OhJavascriptInjector* javascriptInjector =
      NWEB::OhJavascriptInjector::FromWebContents(GetWebContents());
  if (!javascriptInjector) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::RegisterArkJSfunction "
                  "javascriptInjector is null";
    return;
  }
  std::vector<std::string> method_vector;
  for (CefString method : method_list) {
    method_vector.push_back(method.ToString());
  }
  std::vector<std::string> async_method_vector;
  for (CefString async_method : async_method_list) {
    async_method_vector.push_back(async_method.ToString());
  }
  // javascriptInjector->AddInterface(object_name.ToString(), method_vector,
  //                                  async_method_vector, object_id, permission);
}

void ArkWebBrowserHostExtImpl::UnregisterArkJSfunction(
    const CefString& object_name,
    const std::vector<CefString>& method_list) {
  std::vector<std::string> method_vector;
  for (CefString method : method_list) {
    method_vector.push_back(method.ToString());
  }
  if (GetWebContents() == nullptr) {
    return;
  }
  if (auto* javascriptInjector =
          NWEB::OhJavascriptInjector::FromWebContents(GetWebContents())) {
    // javascriptInjector->RemoveInterface(object_name.ToString(), method_vector);
  }
}

void ArkWebBrowserHostExtImpl::CallH5Function(
    int32_t routing_id,
    int32_t h5_object_id,
    const CefString& h5_method_name,
    const std::vector<CefRefPtr<CefValue>>& args) {
  NWEB::OhJavascriptInjector* javascriptInjector =
      NWEB::OhJavascriptInjector::FromWebContents(GetWebContents());
  if (!javascriptInjector || h5_object_id <= 0) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::CallH5Function "
                  "javascriptInjector is null or h5_object_id = "
               << h5_object_id;
    return;
  }

  std::string name = h5_method_name.ToString();
  // javascriptInjector->DoCallH5Function(routing_id, h5_object_id, name, args);
}
#endif  // BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)

#if BUILDFLAG(ARKWEB_PERMISSION)
CefRefPtr<CefBrowserPermissionRequestDelegate>
ArkWebBrowserHostExtImpl::GetPermissionRequestDelegate() {
  return this;
}
CefRefPtr<CefGeolocationAcess>
ArkWebBrowserHostExtImpl::GetGeolocationPermissions() {
  if (geolocation_permissions_ == nullptr) {
    geolocation_permissions_ = new AlloyGeolocationAccess();
  }
  return geolocation_permissions_;
}

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void ArkWebBrowserHostExtImpl::AskClipboardReadWritePermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  permission_request_handler_->SendRequest(new AlloyAccessRequest(
      origin, AlloyAccessRequest::Resources::CLIPBOARD_READ_WRITE,
      std::move(callback)));
}

void ArkWebBrowserHostExtImpl::AbortAskClipboardReadWritePermission(
    const CefString& origin) {
  permission_request_handler_->CancelRequest(
      origin, AlloyAccessRequest::Resources::CLIPBOARD_READ_WRITE);
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
#endif  // BUILDFLAG(ARKWEB_PERMISSION)

#if BUILDFLAG(ARKWEB_ITP)
void ArkWebBrowserHostExtImpl::EnableIntelligentTrackingPrevention(
    bool enable) {
  {
    base::AutoLock locker(lock_);
    intelligent_tracking_prevention_cookies_enabled_ = enable;
  }
  LOG(INFO) << "Intelligent tracking prevention cookies enabled " << enable;
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "Intelligent tracking prevention cookies enabled "
                     << enable;
#endif
  ohos_anti_tracking::ThirdPartyCookieAccessPolicy::GetInstance()
      ->EnableIntelligentTrackingPrevention(GetBrowserContext(), enable);
}

bool ArkWebBrowserHostExtImpl::IsIntelligentTrackingPreventionEnabled() {
  base::AutoLock locker(lock_);
  return intelligent_tracking_prevention_cookies_enabled_;
}

GURL ArkWebBrowserHostExtImpl::GetLastCommittedURL() {
  GURL url;
  if (!GetWebContents()) {
    return url;
  }
  url = GetWebContents()->GetLastCommittedURL();
  return url;
}
#endif

#if BUILDFLAG(ARKWEB_PRECOMPILE)
void ArkWebBrowserHostExtImpl::PrecompileJavaScript(
    const std::string& url,
    const std::string& script,
    CefRefPtr<CefCacheOptions> cacheOptions,
    CefRefPtr<CefPrecompileCallback> callback) {
  std::map<std::string, std::string> responseHeaders;

  cef_string_map_t cefResponseHeaders = cacheOptions->GetResponseHeaders();
  CefString key;
  CefString value;
  size_t size = cef_string_map_size(cefResponseHeaders);
  for (size_t i = 0; i < size; i++) {
    cef_string_map_key(cefResponseHeaders, i, key.GetWritableStruct());
    cef_string_map_value(cefResponseHeaders, i, value.GetWritableStruct());
    responseHeaders.emplace(key.ToString(), value.ToString());
  }

  auto options = std::make_shared<oh_code_cache::CacheOptions>(responseHeaders);

  oh_code_cache::TaskRunner::GetTaskRunner()->PostTaskAndReplyWithResult(
      FROM_HERE,
      base::BindOnce(&ArkWebBrowserHostExtImpl::WriteResponseCache, this, url,
                     script, options),
      base::BindOnce(&ArkWebBrowserHostExtImpl::OnDidWriteResponseCache, this,
                     url, script, options, std::move(callback)));
}

oh_code_cache::NextOp ArkWebBrowserHostExtImpl::WriteResponseCache(
    const std::string& url,
    const std::string& script,
    std::shared_ptr<oh_code_cache::CacheOptions> cacheOptions) {
  auto response_cache = oh_code_cache::ResponseCache::CreateResponseCache(url);

  if (!response_cache) {
    LOG(DEBUG) << "Create response cache error.";
    return oh_code_cache::NextOp::THROW_ERROR;
  }

  return response_cache->Write(cacheOptions->response_headers_, script);
}

void ArkWebBrowserHostExtImpl::OnDidWriteResponseCache(
    const std::string& url,
    const std::string& script,
    std::shared_ptr<oh_code_cache::CacheOptions> cacheOptions,
    CefRefPtr<CefPrecompileCallback> callback,
    oh_code_cache::NextOp nextOp) {
  switch (nextOp) {
    case oh_code_cache::NextOp::WRITE_CODE_CACHE:
      GenerateCodeCache(url, script, cacheOptions, std::move(callback));
      break;
    case oh_code_cache::NextOp::THROW_ERROR:
      callback->OnPrecompileFinished(
          static_cast<int32_t>(oh_code_cache::CacheError::INTERNAL_ERROR));
      break;
    default:
      callback->OnPrecompileFinished(
          static_cast<int32_t>(oh_code_cache::CacheError::NO_ERROR));
      break;
  }
}

void ArkWebBrowserHostExtImpl::GenerateCodeCache(
    const std::string& url,
    const std::string& script,
    std::shared_ptr<oh_code_cache::CacheOptions> cacheOptions,
    CefRefPtr<CefPrecompileCallback> callback) {
  auto wc = GetWebContents();
  if (wc == nullptr) {
    callback->OnPrecompileFinished(
        static_cast<int32_t>(oh_code_cache::CacheError::INTERNAL_ERROR));
    return;
  }

  wc->GetPrimaryMainFrame()->GenerateCodeCache(
      url, script, cacheOptions,
      base::BindOnce(&ArkWebBrowserHostExtImpl::OnDidGenerateCodeCache, this,
                     std::move(callback)));
}

void ArkWebBrowserHostExtImpl::OnDidGenerateCodeCache(
    CefRefPtr<CefPrecompileCallback> callback,
    int32_t result) {
  callback->OnPrecompileFinished(result);
}
#endif

void ArkWebBrowserHostExtImpl::StartCamera() {
#if BUILDFLAG(ARKWEB_WEBRTC)
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "GetWebContents null";
#endif
    return;
  }
  web_contents->StartCamera(web_contents->GetNWebId());
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}

void ArkWebBrowserHostExtImpl::StopCamera() {
#if BUILDFLAG(ARKWEB_WEBRTC)
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "GetWebContents null";
#endif
    return;
  }
  web_contents->StopCamera(web_contents->GetNWebId());
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}

void ArkWebBrowserHostExtImpl::CloseCamera() {
#if BUILDFLAG(ARKWEB_WEBRTC)
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "GetWebContents null";
#endif
    return;
  }
  web_contents->CloseCamera(web_contents->GetNWebId());
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}
int ArkWebBrowserHostExtImpl::GetSecurityLevel() {
  if (!GetWebContents()) {
    return static_cast<int>(security_state::SecurityLevel::NONE);
  }

  auto web_contents = GetWebContents();
  std::unique_ptr<security_state::VisibleSecurityState> state =
      security_state::GetVisibleSecurityState(web_contents);
  DCHECK(state);

  security_state::SecurityLevel securityValue =
      security_state::GetSecurityLevel(*state);
  return static_cast<int>(securityValue);
}

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
void ArkWebBrowserHostExtImpl::ScrollPageUpDown(bool is_up,
                                                bool is_half,
                                                float view_height) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid() && scrollable_) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->ScrollPageUpDown(is_up, is_half, view_height);
  }
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void ArkWebBrowserHostExtImpl::GetScrollOffset(float* offset_x,
                                               float* offset_y) {
  auto frame = GetMainFrame();
  if (frame && frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->GetScrollOffset(offset_x, offset_y);
  }
}
#endif
#endif  // BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
int ArkWebBrowserHostExtImpl::InsertBackForwardEntry(int index,
                                                     const CefString& url) {
  if (!GetWebContents()) {
    return static_cast<int>(
        content::NavigationController::NavigationEntryUpdateError::ERR_OTHER);
  }
  GURL gurl = GURL(url.ToString());
  if (gurl.is_empty() || !gurl.is_valid()) {
    return static_cast<int>(
        content::NavigationController::NavigationEntryUpdateError::ERR_OTHER);
  }
  return static_cast<int>(
      GetWebContents()->GetController().InsertBackForwardEntry(index, gurl));
}

int ArkWebBrowserHostExtImpl::UpdateNavigationEntryUrl(int index,
                                                       const CefString& url) {
  if (!GetWebContents()) {
    return static_cast<int>(
        content::NavigationController::NavigationEntryUpdateError::ERR_OTHER);
  }
  GURL gurl = GURL(url.ToString());
  if (gurl.is_empty() || !gurl.is_valid()) {
    return static_cast<int>(
        content::NavigationController::NavigationEntryUpdateError::ERR_OTHER);
  }
  return static_cast<int>(
      GetWebContents()->GetController().UpdateNavigationEntryUrl(index, gurl));
}

void ArkWebBrowserHostExtImpl::ClearForwardList() {
  if (!GetWebContents()) {
    return;
  }

  GetWebContents()->GetController().PruneForwardEntries();
}
#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

void ArkWebBrowserHostExtImpl::UpdateBrowserControlsState(int constraints,
                                                          int current,
                                                          bool animate) {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (!GetWebContents()) {
    return;
  }

  cc::BrowserControlsState constraints_state =
      static_cast<cc::BrowserControlsState>(constraints);
  cc::BrowserControlsState current_state =
      static_cast<cc::BrowserControlsState>(current);

  GetWebContents()->UpdateBrowserControlsState(constraints_state, current_state,
                                               animate, std::nullopt);
#endif
}

void ArkWebBrowserHostExtImpl::UpdateBrowserControlsHeight(int height,
                                                           bool animate) {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (!GetWebContents()) {
    return;
  }

  GetWebContents()->UpdateBrowserControlsHeight(height, animate);
#endif
}

int ArkWebBrowserHostExtImpl::GetTopControlsOffset() {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (platform_delegate_) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->GetTopControlsOffset();
  }
  return 0;
#else
  return 0;
#endif
}

int ArkWebBrowserHostExtImpl::GetShrinkViewportHeight() {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (platform_delegate_) {
    return platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->GetShrinkViewportHeight();
  }
  return 0;
#else
  return 0;
#endif
}

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
bool ArkWebBrowserHostExtImpl::IsSafeBrowsingEnabled() {
  return settings_.is_safe_browsing_enable;
}

void ArkWebBrowserHostExtImpl::EnableSafeBrowsing(bool enable) {
  if (settings_.is_safe_browsing_enable != enable) {
    LOG(INFO) << "enable safe browsing" << enable;
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "enable safe browsing" << enable;
#endif
    settings_.is_safe_browsing_enable = enable;
  }
}

void ArkWebBrowserHostExtImpl::EnableSafeBrowsingDetection(bool enable,
                                                           bool strictMode) {
  if (!GetWebContents()) {
    return;
  }
  GetWebContents()->EnableSafeBrowsingDetection(enable, strictMode);
}

bool ArkWebBrowserHostExtImpl::IsSafeBrowsingDetectionDisabled() const {
  if (!GetWebContents()) {
    return true;
  }

  return GetWebContents()->IsSafeBrowsingDetectionDisabled();
}

void ArkWebBrowserHostExtImpl::HandleSafeBrowsingDetection(
    const std::string& url) {
  if (client_) {
      client_->HandleSafeBrowsingDetection(GetSafeBrowsingDetectionMode(),
                                           GetSafeBrowsingDetectionSwitch(),
                                           CefString(url));
  }
}

int ArkWebBrowserHostExtImpl::GetSafeBrowsingDetectionMode() const {
  if (!GetWebContents()) {
    return 0;
  }

  return GetWebContents()->IsSafeBrowsingDetectionStrict() ? 1 : 0;
}

enum DetectSwitch {
  DETECT_SWITCH_DEFAULT = 0,
  DETECT_SWITCH_ENABLE = 1,
  DETECT_SWITCH_DISABLE = 2
};

int ArkWebBrowserHostExtImpl::GetSafeBrowsingDetectionSwitch() const {
  if (!GetWebContents()) {
    return DETECT_SWITCH_DEFAULT;
  }

  if (!GetWebContents()->IsSafeBrowsingDetectionConfig()) {
    return DETECT_SWITCH_DEFAULT;
  }

  return GetWebContents()->IsSafeBrowsingDetectionDisabled()
             ? DETECT_SWITCH_DISABLE
             : DETECT_SWITCH_ENABLE;
}

void ArkWebBrowserHostExtImpl::SetSafeBrowsingDetectionCallback(
    CefRefPtr<CefSafeBrowsingDetectionCallback> callback) {
  if (client_) {
      client_->SetSafeBrowsingDetectionCallback(callback);
  }
}
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
bool ArkWebBrowserHostExtImpl::CanStoreWebArchive() {
  if (!CEF_CURRENTLY_ON_UIT()) {
    return false;
  }

  auto web_contents = GetWebContents();
  std::string mime_type = web_contents->GetContentsMimeType();
  GURL url = web_contents->GetURL();
  content::NavigationEntry* entry =
      web_contents->GetController().GetLastCommittedEntry();
  if (entry && (entry->GetPageType() != content::PAGE_TYPE_NORMAL ||
                entry->GetBaseURLForDataURL().spec() ==
                    content::kUnreachableWebDataURL)) {
    return false;
  }

  return url.SchemeIsHTTPOrHTTPS() &&
         (net::MatchesMimeType(mime_type, "text/html") ||
          net::MatchesMimeType(mime_type, "application/xhtml+xml"));
}
#endif

void ArkWebBrowserHostExtImpl::ReloadOriginalUrl() {
  auto callback =
      base::BindOnce(&ArkWebBrowserHostExtImpl::ReloadOriginalUrl, this);
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT, std::move(callback));
    return;
  }

  if (browser_info_->IsNavigationLocked(std::move(callback))) {
    return;
  }

  auto wc = GetWebContents();
  if (wc) {
    wc->GetController().LoadOriginalRequestURL();
  }
}

#if BUILDFLAG(ARKWEB_BFCACHE)
void ArkWebBrowserHostExtImpl::SetBackForwardCacheOptions(int32_t size,
                                                          int32_t timeToLive) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "SetBackForwardCacheOptions failed to get web contents in "
                  "CefBrowserHostBase.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "SetBackForwardCacheOptions failed to get web "
                           "contents in CefBrowserHostBase.";
#endif
    return;
  }

  LOG(INFO) << "SetBackForwardCacheOptions size: " << size
            << " timeToLive: " << timeToLive;
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "SetBackForwardCacheOptions size: " << size
                     << " timeToLive: " << timeToLive;
#endif
  content::NavigationController& controller = web_contents->GetController();
  controller.GetBackForwardCache().SetCacheSize(size);
  controller.GetBackForwardCache().SetTimeToLive(timeToLive);
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
gfx::Rect ArkWebBrowserHostExtImpl::GetVisibleRectToWeb() {
  if (!GetClient() || !GetClient()->GetRenderHandler()) {
    return gfx::Rect();
  }
  CefRefPtr<CefRenderHandler> handler = GetClient()->GetRenderHandler();
  int visible_x = 0;
  int visible_y = 0;
  int visible_width = 0;
  int visible_height = 0;
  handler->AsArkWebRenderHandler()->GetVisibleRectToWeb(
      visible_x, visible_y, visible_width, visible_height);
  return gfx::Rect(visible_x, visible_y, visible_width, visible_height);
}
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
void ArkWebBrowserHostExtImpl::PutUserAgent(const CefString& ua, bool from_app) {
  if (!GetWebContents()) {
    return;
  }
  custom_user_agent_ = ua;
  if (from_app) {
    GetWebContents()->SetCustomUA(custom_user_agent_);
  }

#if BUILDFLAG(ARKWEB_EXT_UA)
  std::string user_agent = ua;
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kEnableNwebExUa) &&
      (ua.empty() || ua.length() == 0)) {
    user_agent = DefaultUserAgent();
  }
  blink::UserAgentOverride user_agent_override =
      blink::UserAgentOverride::UserAgentOnly(user_agent);
  user_agent_override.from_app = true;
  GetWebContents()->SetUserAgentOverride(user_agent_override, true);
#else
  GetWebContents()->SetUserAgentOverride(
      blink::UserAgentOverride::UserAgentOnly(ua), true);
#endif

  content::NavigationController& controller = GetWebContents()->GetController();
  for (int i = 0; i < controller.GetEntryCount(); ++i) {
    controller.GetEntryAtIndex(i)->SetIsOverridingUserAgent(true);
  }
}

CefString ArkWebBrowserHostExtImpl::DefaultUserAgent() {
  return embedder_support::GetUserAgent();
}

CefString ArkWebBrowserHostExtImpl::GetCustomUserAgent() {
  return custom_user_agent_;
}
#endif  // BUILDFLAG(ARKWEB_USERAGENT)

#if BUILDFLAG(ARKWEB_FIND_IN_PAGE)
void ArkWebBrowserHostExtImpl::FindEx(const CefString& searchText,
                                      bool forward,
                                      bool matchCase,
                                      bool findNext,
                                      bool newSession) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT, base::BindOnce(&ArkWebBrowserHostExtImpl::FindEx,
                                          this, searchText, forward, matchCase,
                                          findNext, newSession));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->Find(searchText, forward, matchCase, findNext,
                             newSession);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
CefRefPtr<CefDownloadItem> ArkWebBrowserHostExtImpl::GetDownloadItem(
    uint32_t item_id) {
  LOG(DEBUG) << "CefBrowserHostBase::GetDownloadItem item_id: " << item_id;
  auto web_contents = GetWebContents();
  if (!web_contents) {
    return nullptr;
  }

  auto browser_context = web_contents->GetBrowserContext();
  if (!browser_context) {
    return nullptr;
  }

  content::DownloadManager* manager = browser_context->GetDownloadManager();
  if (!manager) {
    return nullptr;
  }

  download::DownloadItem* item = manager->GetDownload(item_id);
  CefRefPtr<CefDownloadItemImpl> download_item(
      new CefDownloadItemImplExt(item));
  return download_item.get();
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void ArkWebBrowserHostExtImpl::CloseMedia() {
  if (is_fullscreen_) {
    ExitFullScreen();
    StopMedia();
  }
}

void ArkWebBrowserHostExtImpl::ResumeMedia() {
  content::MediaSessionImpl* mediaSession =
      content::MediaSessionImpl::Get(GetWebContents());
  if (!mediaSession || !GetWebContents()) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::ResumeMedia get mediaSession or "
                  "webContents failed.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "CefBrowserHostBase::ResumeMedia get mediaSession "
                           "or webContents failed.";
#endif
    return;
  }
  mediaSession->Resume(content::MediaSession::SuspendType::kSystem);
  GetWebContents()->SetHtmlPlayEnabled(true);
}

void ArkWebBrowserHostExtImpl::PauseMedia() {
  content::MediaSessionImpl* mediaSession =
      content::MediaSessionImpl::Get(GetWebContents());
  if (!mediaSession || !GetWebContents()) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::PauseMedia get mediaSession or "
                  "webContents failed.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "CefBrowserHostBase::PauseMedia get mediaSession or "
                           "webContents failed.";
#endif
    return;
  }
  mediaSession->Suspend(content::MediaSession::SuspendType::kSystem);
  GetWebContents()->SetHtmlPlayEnabled(false);
}

void ArkWebBrowserHostExtImpl::StopMedia() {
  content::MediaSessionImpl* mediaSession =
      content::MediaSessionImpl::Get(GetWebContents());
  if (!mediaSession) {
    LOG(ERROR)
        << "ArkWebBrowserHostExtImpl::StopMedia get mediaSession failed.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR)
        << "CefBrowserHostBase::StopMedia get mediaSession failed.";
#endif
    return;
  }
  mediaSession->Stop(content::MediaSession::SuspendType::kSystem);
}

int ArkWebBrowserHostExtImpl::GetMediaPlaybackState() {
  content::MediaSessionImpl* mediaSession =
      content::MediaSessionImpl::Get(GetWebContents());
  if (!mediaSession || !GetWebContents()) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::GetMediaPlaybackState get "
                  "mediaSession or webContents failed.";
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(ERROR) << "CefBrowserHostBase::GetMediaPlaybackState get "
                           "mediaSession or webContents failed.";
#endif
    return static_cast<int>(content::MediaSessionImpl::NWebPlaybackState::NONE);
  }
  if (!GetWebContents()->IsHtmlPlayEnabled()) {
    return static_cast<int>(
        content::MediaSessionImpl::NWebPlaybackState::PAUSED);
  }
  content::MediaSessionImpl::NWebPlaybackState playbackState =
      mediaSession->NWebGetState();
  return static_cast<int>(playbackState);
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

void ArkWebBrowserHostExtImpl::SetBrowserZoomLevel(double zoom_factor) {
#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
  if (CEF_CURRENTLY_ON_UIT()) {
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kEnableNwebExGetZoomLevel) &&
        GetWebContents()) {
      zoom::ZoomController* zoom_controller =
          zoom::ZoomController::FromWebContents(GetWebContents());
      if (!zoom_controller) {
        LOG(ERROR) << "SetBrowserZoomLevel has no zoom controller.";
        return;
      }
      zoom_controller->SetZoomLevel(blink::ZoomFactorToZoomLevel(zoom_factor));
    }
  } else {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&AlloyBrowserHostImpl::SetBrowserZoomLevel,
                                 this, zoom_factor));
  }
#endif
}

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
void ArkWebBrowserHostExtImpl::OnZoomChanged(
    const zoom::ZoomController::ZoomChangedEventData& data) {
  if (data.web_contents != GetWebContents()) {
    return;
  }
  if (client_) {
    CefRefPtr<ArkWebDisplayHandlerExt> handler = client_->GetDisplayHandler();
    if (handler) {
      handler->OnContentsBrowserZoomChange(
          blink::ZoomLevelToZoomFactor(data.new_zoom_level),
          data.can_show_bubble);
    }
  }
  if (GetWebContents()) {
    auto* browser_context = CefBrowserContext::FromBrowserContext(
        GetWebContents()->GetBrowserContext());
    if (browser_context) {
      DCHECK(browser_context->AsProfile()->GetPrefs());
      browser_context->AsProfile()->GetPrefs()->CommitPendingWrite();
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void ArkWebBrowserHostExtImpl::MaximizeResize() {
  if (platform_delegate_) {
    platform_delegate_->AsArkWebCefBrowserPlatformDelegateExt()->MaximizeResize();
  }
}
#endif  // ARKWEB_MAXIMIZE_RESIZE

void ArkWebBrowserHostExtImpl::RemoveCache(bool include_disk_files) {
  auto frame = GetMainFrame();
  if (!frame) {
    LOG(ERROR) << "browser host remove cache failed, frame is null";
    return;
  }
  if (frame->IsValid()) {
    static_cast<ArkwebFrameHostExtImpl*>(frame.get())
        ->RemoveCache(include_disk_files);
  } else {
    LOG(ERROR) << "browser host remove cache failed, frame is not valid";
    return;
  }
}

#if BUILDFLAG(ARKWEB_DATALIST)
void ArkWebBrowserHostExtImpl::PasswordSuggestionSelected(int list_index) {
  if (!GetWebContents()) {
    return;
  }
  autofill::OhAutofillClient* autofill_client =
      autofill::OhAutofillClient::FromWebContents(GetWebContents());
  if (autofill_client) {
    autofill_client->SuggestionSelected(list_index);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void ArkWebBrowserHostExtImpl::PutWebMediaAVSessionEnabled(bool enable) {
  content::MediaSessionImpl* mediaSession =
      content::MediaSessionImpl::Get(GetWebContents());
  if (!mediaSession || !GetWebContents()) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::PutWebMediaAVSessionEnabled "
                  "failed, mediaSession or web contents error.";
    return;
  }
  mediaSession->PutWebMediaAVSessionEnabled(enable);
}
#endif  // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_DEVTOOLS)
void ArkWebBrowserHostExtImpl::SetWebDebuggingAccess(bool isEnableDebug) {
  base::AutoLock locker(lock_);
  if (is_web_debugging_access_ == isEnableDebug) {
    return;
  }

  if (isEnableDebug) {
    CefDevToolsManagerDelegate::StartHttpHandler(GetBrowserContext());
  } else {
    CefDevToolsManagerDelegate::StopHttpHandler();
  }
  is_web_debugging_access_ = isEnableDebug;
}

bool ArkWebBrowserHostExtImpl::GetWebDebuggingAccess() {
  base::AutoLock locker(lock_);

  return is_web_debugging_access_;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool ArkWebBrowserHostExtImpl::SetFocusByPosition(float x, float y) {
    auto frame = GetMainFrame();
    if (frame && frame->IsValid()) {
      return static_cast<ArkwebFrameHostExtImpl*>(frame.get())->SetFocusByPosition(x, y);
    }
    return false;
  }
#endif // BUILDFLAG(ARKWEB_INPUT_EVENTS)

void ArkWebBrowserHostExtImpl::RunJavaScriptInFrames(const std::string& jsString, FrameInfos rootFrame,
                                                     bool recursive, IsolatedWorld world,
                                                     CefRefPtr<CefJavaScriptResultCallback> callback) {
  LOG(DEBUG) << "RunJavaScriptInFrames enter";
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
    return;
  }
 
  int childId = 0;
  int routingId = 0;
  content::RenderFrameHost* targetFrame = nullptr;
  if (rootFrame.id.empty()) {
    targetFrame = web_contents->GetPrimaryMainFrame();
  } else if(!ParaseRenderFrameHostId(rootFrame.id, &childId, &routingId)) {
    LOG(ERROR) << "rootFrame id is invalid.";
    CefRefPtr<CefValue> data = CefValue::Create();
    std::u16string err_name = base::UTF8ToUTF16(std::string("RunJavaScriptInFrames"));
    std::u16string err_msg = base::UTF8ToUTF16(std::string("rootFrame id is invalid."));
    CefRefPtr<CefDictionaryValue> dict = CefDictionaryValue::Create();
    dict->SetString("Error.name", err_name);
    dict->SetString("Error.message", err_msg);
    data->SetDictionary(dict);
    callback->OnJavaScriptExeResult(data);
    return;
  } else {
    targetFrame = static_cast<content::WebContentsImpl*>(web_contents)->AsWebContentsImplExt()
                  ->GetTargetFramesIncludingPending(routingId);
  }

  if (!targetFrame) {
    LOG(ERROR) << "rootFrame id can not find frame.";
    CefRefPtr<CefValue> data = CefValue::Create();
    std::u16string err_name = base::UTF8ToUTF16(std::string("RunJavaScriptInFrames"));
    std::u16string err_msg = base::UTF8ToUTF16(std::string("rootFrame id can not find frame."));
    CefRefPtr<CefDictionaryValue> dict = CefDictionaryValue::Create();
    dict->SetString("Error.name", err_name);
    dict->SetString("Error.message", err_msg);
    data->SetDictionary(dict);
    callback->OnJavaScriptExeResult(data);
    return;
  }

  LOG(DEBUG) << "RunJavaScriptInFrames";
  targetFrame->AllowInjectingJavaScript();
  targetFrame->ExecuteJavaScriptInFrames(base::UTF8ToUTF16(jsString), recursive, world.name,
                    base::BindOnce(&ArkWebBrowserHostExtImpl::ExecuteExtensionJSCallback,
                                   this, callback));
}

#if BUILDFLAG(ARKWEB_BGTASK)
void ArkWebBrowserHostExtImpl::OnBrowserForeground() {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
    return;
  }
  LOG(INFO) << "ArkWebBrowserHostExtImpl::OnBrowserForeground";
  web_contents->OnBrowserForeground();
}

void ArkWebBrowserHostExtImpl::OnBrowserBackground() {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
    return;
  }
  LOG(INFO) << "ArkWebBrowserHostExtImpl::OnBrowserBackground";
  web_contents->OnBrowserBackground();
}
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
void ArkWebBrowserHostExtImpl::Distill(const std::string& guid, const DistillOptions& distill_options,
  CefRefPtr<CefDistillCallback> callback) {
  auto web_contents = GetWebContents();
  if (!web_contents || !callback) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::Distill "
                  "callback is nullptr or web_contents is null";
    return;
  }
  oh_dom_distiller::DistillResultCallback distill_result_callback =
    base::BindOnce(
      [](CefRefPtr<CefDistillCallback> callback,
        const std::string& guid, const std::string& content) {
        if (callback) {
          callback->OnDistillCallback(guid, content);
        }
      },
      callback, guid);
  oh_dom_distiller::OhDomDistillerManager::GetInstance()->DistillCurrentPage(
    web_contents, distill_options, std::move(distill_result_callback));
}

void ArkWebBrowserHostExtImpl::AbortDistill() {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::AbortDistill, web_contents is null";
    return;
  }
  oh_dom_distiller::OhDomDistillerManager::GetInstance()->AbortDistill(web_contents);
}
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void ArkWebBrowserHostExtImpl::GetFocusedFrameInfo(int32_t& frame_id,
                                                   CefString& frame_url) {
  frame_id = -1;
  frame_url.clear();

  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
    return;
  }

  auto frame = web_contents->GetFocusedFrame();
  if (!frame) {
    LOG(ERROR) << "web_contents GetFocusedFrame null";
    return;
  }

  // Extension API frame ID of the top-level frame.
  constexpr int32_t kTopFrameId = 0;
  frame_id = frame->IsInPrimaryMainFrame()
                 ? kTopFrameId
                 : frame->GetFrameTreeNodeId().value();
  frame_url = frame->GetLastCommittedURL().spec();
}
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
void ArkWebBrowserHostExtImpl::EnableHttpsUpgrades(bool enable) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::EnableHttpsUpgrades, web_contents is null";
    return;
  }
  OhosHttpsUpgradesHelper::CreateForWebContents(web_contents);
  auto* https_helper = OhosHttpsUpgradesHelper::FromWebContents(web_contents);
  if (https_helper) {
    https_helper->set_is_arkweb_https_upgrades_enable(enable);
  } else {
    LOG(ERROR) << "ArkWebBrowserHostExtImpl::EnableHttpsUpgrades, https_helper is null";
  }
}
#endif

void ArkWebBrowserHostExtImpl::HandleInputMethodExtendAction(int32_t action) {
  auto* web_contents = static_cast<content::WebContentsImpl*>(GetWebContents());
  if (web_contents == nullptr) {
    LOG(ERROR) << __FUNCTION__ << " web_contents is nullptr, " << action;
    return;
  }
  if (action ==
      static_cast<int32_t>(OHOS::NWeb::IMFAdapterExtendAction::SELECT_ALL)) {
    web_contents->SelectAll();
  } else if (action ==
             static_cast<int32_t>(OHOS::NWeb::IMFAdapterExtendAction::CUT)) {
    web_contents->Cut();
  } else if (action ==
             static_cast<int32_t>(OHOS::NWeb::IMFAdapterExtendAction::COPY)) {
    web_contents->Copy();
  } else if (action ==
             static_cast<int32_t>(OHOS::NWeb::IMFAdapterExtendAction::PASTE)) {
    web_contents->Paste();
  } else {
    LOG(ERROR) << __FUNCTION__ << " Unsupported action " << action;
  }
}

void ArkWebBrowserHostExtImpl::StopFling() {
  auto rwhv = GetWebContents()->GetRenderWidgetHostView();
  if (rwhv && rwhv->GetRenderWidgetHost()) {
    content::RenderWidgetHostImpl::From(rwhv->GetRenderWidgetHost())->StopFling();
  }
}
