// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cef/libcef/browser/chrome/extensions/chrome_extension_util.h"
#include "cef/ohos_cef_ext/libcef/browser/chrome/extensions/arkweb_chrome_extension_util_ext.h"

#include "cef/libcef/browser/browser_host_base.h"
#include "chrome/browser/profiles/profile.h"
#include "components/sessions/content/session_tab_helper.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/command_line.h"
#include "cef/libcef/browser/browser_info_manager.h"
#include "cef/libcef/browser/alloy/alloy_browser_host_impl.h"
#include "content/public/common/content_switches.h"
#endif

#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_ext.h"

namespace cef {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
int GetTabIdForWebContents(const content::WebContents* web_contents) {
  if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
      switches::kEnableNwebEx) && base::ohos::IsPcDevice()) {
    auto browser = AlloyBrowserHostImpl::GetBrowserForContents(web_contents);
    if (!browser) {
      return -1;
    }
    int tab_id = browser->AsAlloyBrowserHostImplExt()->ExtensionGetTabId();
    if (tab_id >= 0) {
      return tab_id;
    } else {
      LOG(INFO) << "GetTabIdForWebContents error tabId=" << tab_id;
      return -1;
    }
  }

  return sessions::SessionTabHelper::IdForTab(web_contents).id();
}

content::WebContents* GetWebContentByTabId(int tab_id) {
  CefRefPtr<AlloyBrowserHostImpl> browser = nullptr;
  for (const auto& browser_info :
       CefBrowserInfoManager::GetInstance()->GetBrowserInfoList()) {
    CefRefPtr<AlloyBrowserHostImpl> current_browser =
        browser_info->browser()->AsAlloyBrowserHostImpl();
    if (current_browser && current_browser->AsAlloyBrowserHostImplExt()->ExtensionGetTabId() == tab_id &&
        tab_id >= 0) {
      browser = current_browser;
      break;
    }
  }

  if (browser) {
    return browser->web_contents();
  } else {
    return nullptr;
  }
}

content::WebContents* GetWebContentBySessionId(int session_id) {
  CefRefPtr<AlloyBrowserHostImpl> browser = nullptr;
  for (const auto& browser_info :
      CefBrowserInfoManager::GetInstance()->GetBrowserInfoList()) {
    CefRefPtr<AlloyBrowserHostImpl> current_browser = browser_info->browser()->AsAlloyBrowserHostImpl();
    if (current_browser &&
        (sessions::SessionTabHelper::IdForTab(current_browser->web_contents()).id()
         == session_id) && session_id > 0) {
      browser = current_browser;
      break;
    }
  }

  if (browser) {
    return browser->web_contents();
  } else {
    return nullptr;
  }
}

content::WebContents* GetWebContentByTabIdOrSessionId(int id) {
  if (id < 0) {
    LOG(INFO) << "GetWebContentByTabIdOrBrowserId error: id=" << id;
    return nullptr;
  }

  if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
      switches::kEnableNwebEx) && base::ohos::IsPcDevice()) {
    return GetWebContentByTabId(id);
  }

  return GetWebContentBySessionId(id);
}

bool ArkWebExtensionIsNotTabId(content::WebContents* web_contents, int tab_id) {
  if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
      switches::kEnableNwebEx) && base::ohos::IsPcDevice()) {
    if (web_contents->ExtensionGetTabId() != tab_id) {
      return true;
    }
  } else {
    if (sessions::SessionTabHelper::IdForTab(web_contents).id() != tab_id) {
      return true;
    }
  }
  return false;
}

bool GetAlloyTabById(int tab_id,
                     Profile* profile,
                     bool include_incognito,
                     content::WebContents** contents) {
  for (auto rph_iterator = content::RenderProcessHost::AllHostsIterator();
       !rph_iterator.IsAtEnd(); rph_iterator.Advance()) {
    content::RenderProcessHost* rph = rph_iterator.GetCurrentValue();
 
    // Ignore renderers that aren't ready.
    if (!rph->IsInitializedAndNotDead()) {
      continue;
    }
    // Ignore renderers that aren't from a valid profile. This is either the
    // same profile or the incognito profile if `include_incognito` is true.
    Profile* process_profile =
        Profile::FromBrowserContext(rph->GetBrowserContext());
    if (process_profile != profile &&
        !(include_incognito && profile->IsSameOrParent(process_profile))) {
      continue;
    }
 
    bool found_contents = false;
    rph->ForEachRenderFrameHost([&contents, tab_id,
                                 &found_contents](content::RenderFrameHost* rfh) {
      CHECK(rfh);
      auto* web_contents = content::WebContents::FromRenderFrameHost(rfh);
      CHECK(web_contents);
      if (ArkWebExtensionIsNotTabId(web_contents, tab_id)) {
        return;
      }
 
      // We only consider Alloy style CefBrowserHosts in this loop. Otherwise,
      // we could end up returning a WebContents that shouldn't be exposed to
      // extensions.
      auto browser = CefBrowserHostBase::GetBrowserForContents(web_contents);
      if (!browser || !browser->IsAlloyStyle()) {
        return;
      }
 
      if (contents) {
        *contents = web_contents;
      }
      found_contents = true;
    });
 
    if (found_contents) {
      return true;
    }
  }
 
  return false;
}
#endif // ARKWEB_ARKWEB_EXTENSIONS

}  // namespace cef
