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

#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_utils.h"

#include "content/browser/web_contents/web_contents_impl.h"

#include "content/public/common/content_switches.h"

#if BUILDFLAG(ARKWEB_I18N)
#include "base/ohos/locale_utils.h"
#include "ui/base/ui_base_switches.h"
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_utils.h"
#endif

namespace content {

// LCOV_EXCL_START
WebContentsImplUtils::WebContentsImplUtils(WebContentsImpl* impl) {
  this->webContentsImpl = impl;
}
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_I18N)
void WebContentsImplUtils::UpdateRenderAcceptLanguageIfNeed(
    const std::string& old_accept_language) {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (!command_line.HasSwitch(::switches::kLang)) {
    return;
  }
  std::string lang = command_line.GetSwitchValueASCII(::switches::kLang);
  std::regex pattern("-");
  std::smatch match;
  std::string current_accept_language;
  if (std::regex_search(lang, match, pattern)) {
    std::string region = match.suffix();
    current_accept_language = base::ohos::ComputeLanguageByRegion(region);
  } else {
    current_accept_language = base::ohos::ComputeLanguageByRegion("");
  }
  if (current_accept_language != "" &&
      current_accept_language != old_accept_language) {
    webContentsImpl->renderer_preferences_.accept_languages =
        current_accept_language;
  }
}
#endif  // #if BUILDFLAG(ARKWEB_I18N)

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
void WebContentsImplUtils::renderProcessShareInit(
    const WebContents::CreateParams& params,
    scoped_refptr<SiteInstanceImpl> site_instance) {
  if (!params.shared_render_process_token.empty()) {
    webContentsImpl->AsWebContentsImplExt()->shared_render_process_token_ =
        params.shared_render_process_token;
    RenderProcessHost* render_process =
        ArkwebRenderProcessHostImplUtils::GetProcessForSharedToken(
            webContentsImpl->AsWebContentsImplExt()->shared_render_process_token_);
    if (render_process) {
      site_instance->ReuseExistingProcessIfPossible(render_process);
    } else {
      ArkwebRenderProcessHostImplUtils::RegisteProcessForSharedToken(
          webContentsImpl->AsWebContentsImplExt()->shared_render_process_token_,
          site_instance->GetProcess());
    }
  }
}
#endif
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
void WebContentsImplUtils::UpdateMainFrameLoadingControlsState(FrameTreeNode* frame_tree_node, bool should_show_loading_ui) {
  if (frame_tree_node->IsMainFrame()) {
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kEnableNwebExTopControls) &&
        should_show_loading_ui) {
      webContentsImpl->UpdateBrowserControlsState(webContentsImpl->AsWebContentsImplExt()->browser_controls_state_,
                                 cc::BrowserControlsState::kShown, false,
                                 std::nullopt);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_USERAGENT) || BUILDFLAG(ARKWEB_EXT_UA)
void WebContentsImplUtils::UpdateUserAgentOverride(const blink::UserAgentOverride& ua_override){
  webContentsImpl->AsWebContentsImplExt()->user_agent_ = ua_override.ua_string_override;
  webContentsImpl->UpdateOverridingUserAgent();

  // DTS2023022711784
  // 子进程打开新窗口时，会先创建delayed_load_url_params_，等到加载url时直接使用
  // delayed_load_url_params_的值创建NavigationRequest。其override_user_agent默认值是
  // UA_OVERRIDE_FALSE，故这里也要更新。
  if (webContentsImpl->delayed_load_url_params_) {
    webContentsImpl->delayed_load_url_params_->override_user_agent =
        NavigationController::UA_OVERRIDE_TRUE;
  }
}
#endif

#if BUILDFLAG(ARKWEB_PDF)
void WebContentsImplUtils::JudgeIsPdfPageVisibilityChanged(Visibility visibility) {
  if (visibility == Visibility::VISIBLE && !webContentsImpl->did_first_set_visible_) {
    GURL url = webContentsImpl->GetVisibleURL();
    url::Origin url_origin = url::Origin::Create(url);
    bool is_pdf = IsPdfExtensionOrigin(url_origin);
    base::ohos::SlidingObserver::GetInstance().SetIsPdf(is_pdf);
  }
}
#endif

#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
void WebContentsImplUtils::EvictFrameBackBuffersWhenNWebWasHidden() {
  DCHECK(!webContentsImpl->IsBeingDestroyed());
  PageVisibilityState page_visibility = webContentsImpl->CalculatePageVisibilityState(Visibility::HIDDEN);

  bool view_is_visible = !webContentsImpl->IsCrashed() && page_visibility != PageVisibilityState::kHidden;
  if (auto* view = webContentsImpl->GetRenderWidgetHostView()) {
    if (!view_is_visible) {
#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
      view->EvictFrameBackBuffers();
#endif
    }
  }
}

void WebContentsImplUtils::SetIsOfflineWebComponent() {
  DCHECK(!webContentsImpl->IsBeingDestroyed());
  is_offline_component_ = true;
}

void WebContentsImplUtils::SetIsOfflineWebComponentInactive(Visibility new_visibility) {
  DCHECK(!webContentsImpl->IsBeingDestroyed());
  if (auto* view = webContentsImpl->GetRenderWidgetHostView()) {
    if (is_offline_component_) {
      if (new_visibility == Visibility::VISIBLE) {
        view->SetIsOfflineWebComponentInactive(false);
        is_offline_component_ = false;
      } else {
        view->SetIsOfflineWebComponentInactive(true);
      }
    }
  }
}
#endif
}  // namespace content
