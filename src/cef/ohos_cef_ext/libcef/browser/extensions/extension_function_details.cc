// Copyright 2017 the Chromium Embedded Framework Authors. Portions copyright
// 2014 The Chromium Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/extensions/extension_function_details.h"

#include "libcef/browser/browser_context.h"
#include "ohos_cef_ext/libcef/browser/extensions/browser_extensions_util.h"
#include "libcef/browser/thread_util.h"

#include "base/strings/utf_string_conversions.h"
#include "base/task/thread_pool.h"
#include "chrome/browser/extensions/api/tabs/tabs_constants.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/favicon_status.h"
#include "content/public/browser/navigation_entry.h"
#include "extensions/browser/extension_function.h"
#include "extensions/browser/extension_function_dispatcher.h"
#include "extensions/common/error_utils.h"
#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_ext.h"

using content::RenderViewHost;
using content::WebContents;

namespace extensions {

namespace keys = extensions::tabs_constants;

CefExtensionFunctionDetails::CefExtensionFunctionDetails(
    ExtensionFunction* function)
    : function_(function) {}

CefExtensionFunctionDetails::~CefExtensionFunctionDetails() {}

Profile* CefExtensionFunctionDetails::GetProfile() const {
  return Profile::FromBrowserContext(function_->browser_context());
}

CefRefPtr<AlloyBrowserHostImpl> CefExtensionFunctionDetails::GetSenderBrowser()
    const {
  content::WebContents* web_contents = function_->GetSenderWebContents();
  if (web_contents) {
    return AlloyBrowserHostImpl::GetBrowserForContents(web_contents);
  }
  return nullptr;
}

CefRefPtr<AlloyBrowserHostImpl> CefExtensionFunctionDetails::GetCurrentBrowser()
    const {
  return GetSenderBrowser();
}

bool CefExtensionFunctionDetails::CanAccessBrowser(
    CefRefPtr<AlloyBrowserHostImpl> target) const {
  DCHECK(target);

  // Start with the browser hosting the extension.
  CefRefPtr<AlloyBrowserHostImpl> browser = GetSenderBrowser();
  if (browser == target) {
    // A sender can always access itself.
    return true;
  }

  // if (browser && browser->client()) {
  //   CefRefPtr<CefExtensionHandler> handler = GetCefExtension()->GetHandler();
  //   if (handler) {
  //     return handler->CanAccessBrowser(
  //         GetCefExtension(), browser.get(),
  //         function_->include_incognito_information(), target);
  //   }
  // }

  // Default to allowing access.
  return true;
}

api::tabs::Tab CefExtensionFunctionDetails::CreateTabObject(
    CefRefPtr<AlloyBrowserHostImpl> new_browser,
    int opener_browser_id,
    bool active,
    int index) {
  content::WebContents* contents = new_browser->web_contents();

  bool is_loading = contents->IsLoading();
  api::tabs::Tab tab_object;
  tab_object.id = new_browser->AsAlloyBrowserHostImplExt()->ExtensionGetTabId();
  tab_object.index = index;
  tab_object.window_id = *tab_object.id;
  tab_object.status = is_loading ? api::tabs::TabStatus::kLoading
                                 : api::tabs::TabStatus::kComplete;
  tab_object.active = active;
  tab_object.selected = true;
  tab_object.highlighted = true;
  tab_object.pinned = false;
  // TODO(extensions): Use RecentlyAudibleHelper to populate |audible|.
  tab_object.discarded = false;
  tab_object.auto_discardable = false;
  tab_object.muted_info = CreateMutedInfo(contents);
  tab_object.incognito = false;
  gfx::Size contents_size = contents->GetContainerBounds().size();
  tab_object.width = contents_size.width();
  tab_object.height = contents_size.height();
  tab_object.url = contents->GetURL().spec();
  tab_object.title = base::UTF16ToUTF8(contents->GetTitle());

  content::NavigationEntry* entry = contents->GetController().GetVisibleEntry();
  if (entry && entry->GetFavicon().valid) {
    tab_object.fav_icon_url = entry->GetFavicon().url.spec();
  }

  if (opener_browser_id >= 0) {
    tab_object.opener_tab_id = opener_browser_id;
  }

  return tab_object;
}

// static
api::tabs::MutedInfo CefExtensionFunctionDetails::CreateMutedInfo(
    content::WebContents* contents) {
  DCHECK(contents);
  api::tabs::MutedInfo info;
  info.muted = contents->IsAudioMuted();
  // TODO(cef): Maybe populate |info.reason|.
  return info;
}

}  // namespace extensions
