// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/apps/platform_apps/chrome_apps_browser_api_provider.h"

#include "arkweb/build/features/features.h"
#include "build/build_config.h"

#if !BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "chrome/browser/apps/platform_apps/api/generated_api_registration.h"
#endif

namespace chrome_apps {

ChromeAppsBrowserAPIProvider::ChromeAppsBrowserAPIProvider() = default;
ChromeAppsBrowserAPIProvider::~ChromeAppsBrowserAPIProvider() = default;

void ChromeAppsBrowserAPIProvider::RegisterExtensionFunctions(
    ExtensionFunctionRegistry* registry) {
#if !BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  api::ChromeAppsGeneratedFunctionRegistry::RegisterAll(registry);
#endif
}

}  // namespace chrome_apps
