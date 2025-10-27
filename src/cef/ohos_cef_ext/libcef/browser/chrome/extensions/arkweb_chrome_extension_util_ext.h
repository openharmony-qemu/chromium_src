// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef ARKWEB_LIBCEF_BROWSER_CHROME_EXTENSIONS_CHROME_EXTENSION_UTIL_EXT_H_
#define ARKWEB_LIBCEF_BROWSER_CHROME_EXTENSIONS_CHROME_EXTENSION_UTIL_EXT_H_
#pragma once

#include "arkweb/build/features/features.h"

namespace content {
class WebContents;
}  // namespace content

namespace cef {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
int GetTabIdForWebContents(const content::WebContents* web_contents);

content::WebContents* GetWebContentByTabId(int tab_id);

content::WebContents* GetWebContentBySessionId(int browser_id);

content::WebContents* GetWebContentByTabIdOrSessionId(int id);

bool ArkWebExtensionIsNotTabId(content::WebContents* web_contents, int tab_id);
#endif

}  // namespace cef

#endif  // ARKWEB_LIBCEF_BROWSER_CHROME_EXTENSIONS_CHROME_EXTENSION_UTIL_EXT_H_
