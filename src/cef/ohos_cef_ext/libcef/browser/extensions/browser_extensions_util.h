#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_BROWSER_EXTENSIONS_UTIL_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_BROWSER_EXTENSIONS_UTIL_H_
#pragma once

#include "cef/libcef/browser/alloy/alloy_browser_host_impl.h"
namespace extensions {

// Get browser by tab_id for extension.
CefRefPtr<AlloyBrowserHostImpl> GetBrowserByTabIdForExtension(
    int tab_id,
    content::BrowserContext* browser_context);

}  // namespace extensions

#endif // CEF_LIBCEF_BROWSER_EXTENSIONS_BROWSER_EXTENSIONS_UTIL_H_