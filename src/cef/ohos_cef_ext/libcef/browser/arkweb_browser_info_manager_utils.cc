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

#include "cef/ohos_cef_ext/libcef/browser/arkweb_browser_info_manager_utils.h"

#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/web_contents.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "cef/libcef/browser/browser_info_manager.h"

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "content/public/common/url_constants.h"
#include "extensions/browser/view_type_utils.h"
#include "extensions/common/constants.h"
#endif

ArkwebBrowserInfoManagerUtils::ArkwebBrowserInfoManagerUtils(
    CefBrowserInfoManager* cef_browser_info_manager)
    : cef_browser_info_manager_(cef_browser_info_manager) {
  DCHECK(cef_browser_info_manager);
}

ArkwebBrowserInfoManagerUtils::~ArkwebBrowserInfoManagerUtils() = default;

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
bool ArkwebBrowserInfoManagerUtils::CanCreateWindow(
    content::RenderFrameHost* opener,
    const GURL& target_url,
    WindowOpenDisposition disposition,
    bool user_gesture,
    CefRefPtr<CefCallback> callback) {
  CEF_REQUIRE_UIT();
  content::Referrer referrer;
  content::OpenURLParams params(target_url, referrer, disposition,
                                ui::PAGE_TRANSITION_LINK,
                                /*is_renderer_initiated=*/true);
  params.user_gesture = user_gesture;

  CefRefPtr<CefBrowserHostBase> browser;
  if (!cef_browser_info_manager_->MaybeAllowNavigation(opener, params,
                                                       browser) ||
      !browser) {
    LOG(INFO) << "not allow popup, cancel the popup";
    callback->Cancel();
    return false;
  }

  CefRefPtr<CefClient> client = browser->GetClient();
  bool allow = true;
  if (client.get()) {
    CefRefPtr<CefLifeSpanHandler> handler = client->GetLifeSpanHandler();
    if (handler.get()) {
      CefRefPtr<CefFrame> opener_frame = browser->GetFrameForHost(opener);
      DCHECK(opener_frame);
      allow = !handler->OnPreBeforePopup(
          browser.get(), opener_frame, target_url.spec(),
          static_cast<cef_window_open_disposition_t>(disposition), user_gesture,
          callback);
    }
  }
  if (!allow) {
    callback->Cancel();
  }
  return allow;
}
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
bool ArkwebBrowserInfoManagerUtils::IsExtensionsOptionsUiFrame(
    const content::GlobalRenderFrameHostToken& global_token) {
  constexpr char kExtensionsHost[] = "extensions";

  auto* rfh = content::RenderFrameHost::FromFrameToken(global_token);
  auto* web_contents = content::WebContents::FromRenderFrameHost(rfh);

  bool is_extensions_options_ui_frame =
      web_contents &&
      (web_contents->GetURL().SchemeIs(extensions::kExtensionScheme) ||
       web_contents->GetURL().SchemeIs(extensions::kArkwebExtensionScheme)) &&
      web_contents->GetResponsibleWebContents() &&
      web_contents->GetResponsibleWebContents()->GetURL().SchemeIs(
          content::kArkWebUIScheme) &&
      web_contents->GetResponsibleWebContents()->GetURL().host() ==
          kExtensionsHost;
  return is_extensions_options_ui_frame;
}

bool ArkwebBrowserInfoManagerUtils::IsExtensionsOffscreenFrame(
    const content::GlobalRenderFrameHostToken& global_token) {
  auto* rfh = content::RenderFrameHost::FromFrameToken(global_token);
  auto* web_contents = content::WebContents::FromRenderFrameHost(rfh);

  if (web_contents &&
      (web_contents->GetURL().SchemeIs(extensions::kExtensionScheme) ||
       web_contents->GetURL().SchemeIs(extensions::kArkwebExtensionScheme)) &&
      (extensions::GetViewType(web_contents) ==
       extensions::mojom::ViewType::kOffscreenDocument)) {
    LOG(INFO) << "need to exclude GetNewBrowserInfo for offscreen document";
    return true;
  }
  return false;
}
#endif  // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
bool ArkwebBrowserInfoManagerUtils::IsPrerendering(
    const content::GlobalRenderFrameHostToken& global_token) {
  std::vector<CefBrowserContext*> browser_context_all =
      CefBrowserContext::GetAll();
  if (browser_context_all.size() == 0) {
    return false;
  }

  auto* rfh = content::RenderFrameHost::FromFrameToken(global_token);
  content::WebContents* web_contents =
      content::WebContents::FromRenderFrameHost(rfh);
  if (!web_contents) {
    return false;
  }

  for (auto& context : CefBrowserContext::GetAll()) {
    prerender::NoStatePrefetchManager* no_state_prefetch_manager =
        prerender::NoStatePrefetchManagerFactory::GetForBrowserContext(
            context->AsBrowserContext());
    if (no_state_prefetch_manager) {
      auto* no_state_prefetch_content =
          no_state_prefetch_manager->GetNoStatePrefetchContents(web_contents);
      if (no_state_prefetch_content) {
        return true;
      }
    }
  }

  return false;
}

void ArkwebBrowserInfoManagerUtils::CancelForPrerendering(
    const content::GlobalRenderFrameHostToken& global_token,
    int timeout_id) {
  CEF_REQUIRE_UIT();
  LOG(INFO) << "cancel for prerendering";
  CefBrowserInfoManager* cef_browser_info_manager =
      CefBrowserInfoManager::GetInstance();
  if (!cef_browser_info_manager) {
    return;
  }

  if (!IsPrerendering(global_token)) {
    return;
  }

  cef_browser_info_manager->TimeoutNewBrowserInfoResponse(global_token,
                                                          timeout_id);
}
#endif
