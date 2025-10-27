// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ARKWEB_USERAGENT_UTILS_H_
#define CEF_LIBCEF_BROWSER_ARKWEB_USERAGENT_UTILS_H_
#pragma once

#include "content/public/browser/navigation_handle.h"

namespace arkweb_useragent_utils {
  void MaybeOverrideUserAgentOnStartNavigation(content::NavigationHandle* navigation);
  void MaybeOverrideUserAgentOnRedirectNavigation(content::NavigationHandle* navigation);
  void CheckRedirectChainForDuplicates(content::NavigationHandle* navigation);
  bool ShouldUpdateErrorPageUrl(const std::vector<std::string>& redirect_chain,
                                const std::string& current_url);
} // namespace arkweb_useragent_utils

#endif // CEF_LIBCEF_BROWSER_ARKWEB_USERAGENT_UTILS_H_