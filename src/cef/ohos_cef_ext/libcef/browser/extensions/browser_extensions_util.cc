// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/extensions/browser_extensions_util.h"
#include "cef/libcef/browser/browser_info_manager.h"
#include "cef/ohos_cef_ext/libcef/browser/alloy/alloy_browser_host_impl_ext.h"

namespace extensions {

CefRefPtr<AlloyBrowserHostImpl> GetBrowserByTabIdForExtension(
    int tab_id,
    content::BrowserContext* browser_context) {
  // REQUIRE_ALLOY_RUNTIME();
  CEF_REQUIRE_UIT();
  DCHECK(browser_context);
  if (tab_id < 0 || !browser_context) {
    return nullptr;
  }

  auto cef_browser_context =
      CefBrowserContext::FromBrowserContext(browser_context);

  for (const auto& browser_info :
       CefBrowserInfoManager::GetInstance()->GetBrowserInfoList()) {
    CefRefPtr<AlloyBrowserHostImpl> current_browser = browser_info->browser()->AsAlloyBrowserHostImpl();
        // static_cast<AlloyBrowserHostImpl*>(browser_info->browser().get());
    if (current_browser && current_browser->AsAlloyBrowserHostImplExt()->ExtensionGetTabId() == tab_id) {
      // Make sure we're operating in the same CefBrowserContext.
      if (CefBrowserContext::FromBrowserContext(
              current_browser->GetBrowserContext()) == cef_browser_context) {
        return current_browser;
      } else {
        LOG(WARNING) << "[Extension] Browser with tabId " << tab_id
                     << " cannot be accessed because is uses a different "
                        "CefRequestContext";
        break;
      }
    }
  }

  return nullptr;
}

} // namespace extensions