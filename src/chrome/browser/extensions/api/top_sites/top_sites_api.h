// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_API_TOP_SITES_TOP_SITES_API_H_
#define CHROME_BROWSER_EXTENSIONS_API_TOP_SITES_TOP_SITES_API_H_

#include "components/history/core/browser/history_types.h"
#include "extensions/browser/extension_function.h"
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "ohos_nweb/src/capi/web_extension_top_sites_items.h"
#endif
#include "extensions/buildflags/buildflags.h"

static_assert(BUILDFLAG(ENABLE_EXTENSIONS_CORE));

namespace extensions {

class TopSitesGetFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("topSites.get", TOPSITES_GET)

  TopSitesGetFunction();

 protected:
  ~TopSitesGetFunction() override;

  // ExtensionFunction:
  ResponseAction Run() override;

 private:
  void OnMostVisitedURLsAvailable(const history::MostVisitedURLList& data);
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  static void OnGet(const base::WeakPtr<TopSitesGetFunction>& function,
                    const std::vector<NWebExtensionTopSitesMostVisitedURL>& data,
                    const std::optional<std::string>& error);
  bool call_get_ = false;
  base::WeakPtrFactory<TopSitesGetFunction> weak_ptr_factory_{this};
#endif
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_TOP_SITES_TOP_SITES_API_H_
