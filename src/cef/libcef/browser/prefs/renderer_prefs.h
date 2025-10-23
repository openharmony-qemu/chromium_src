// Copyright (c) 2010 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PREFS_RENDERER_PREFS_H_
#define CEF_LIBCEF_BROWSER_PREFS_RENDERER_PREFS_H_
#pragma once

#include <string>

#include "arkweb/build/features/features.h"

#include "cef/include/internal/cef_types_wrappers.h"

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "content/public/browser/web_contents.h"
#endif

namespace blink::web_pref {
struct WebPreferences;
}

namespace renderer_prefs {

// Set default values based on CEF command-line flags for preferences that are
// not available via the PrefService. Chromium command-line flags should not
// exist for these preferences.
void SetDefaultPrefs(blink::web_pref::WebPreferences& web);

// Set preferences based on CefBrowserSettings.
void SetCefPrefs(const CefBrowserSettings& cef,
                 blink::web_pref::WebPreferences& web);

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
void SetJsDefaultContent(content::WebContents* web_contents,
                         blink::web_pref::WebPreferences& web);
#endif

}  // namespace renderer_prefs

#endif  // CEF_LIBCEF_BROWSER_PREFS_RENDERER_PREFS_H_
