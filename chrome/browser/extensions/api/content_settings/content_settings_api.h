// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_API_CONTENT_SETTINGS_CONTENT_SETTINGS_API_H_
#define CHROME_BROWSER_EXTENSIONS_API_CONTENT_SETTINGS_CONTENT_SETTINGS_API_H_

#include <vector>

#include "base/gtest_prod_util.h"
#include "content/public/common/buildflags.h"
#include "extensions/browser/extension_function.h"
#include "extensions/browser/browser_context_keyed_api_factory.h"
#include "extensions/browser/event_router.h"


#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "ohos_nweb/src/capi/browser_service/nweb_extension_content_settings_types.h"
#endif  // ARKWEB_ARKWEB_EXTENSIONS

namespace content {
struct WebPluginInfo;
}

namespace base {
class FilePath;
}

namespace extensions {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/content_settings/content_settings_api_for_include_file.cc"
#else

class ContentSettingsContentSettingClearFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("contentSettings.clear", CONTENTSETTINGS_CLEAR)

 protected:
  ~ContentSettingsContentSettingClearFunction() override = default;

  // ExtensionFunction:
  ResponseAction Run() override;
};

class ContentSettingsContentSettingGetFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("contentSettings.get", CONTENTSETTINGS_GET)

 protected:
  ~ContentSettingsContentSettingGetFunction() override = default;

  // ExtensionFunction:
  ResponseAction Run() override;
};

class ContentSettingsContentSettingSetFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("contentSettings.set", CONTENTSETTINGS_SET)

 protected:
  ~ContentSettingsContentSettingSetFunction() override = default;

  // ExtensionFunction:
  ResponseAction Run() override;
};
#endif  // ARKWEB_ARKWEB_EXTENSIONS

class ContentSettingsContentSettingGetResourceIdentifiersFunction
    : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("contentSettings.getResourceIdentifiers",
                             CONTENTSETTINGS_GETRESOURCEIDENTIFIERS)

 protected:
  ~ContentSettingsContentSettingGetResourceIdentifiersFunction() override =
      default;

  // ExtensionFunction:
  ResponseAction Run() override;

 private:
  FRIEND_TEST_ALL_PREFIXES(ExtensionApiTest,
                           ContentSettingsGetResourceIdentifiers);

#if BUILDFLAG(ENABLE_PLUGINS)
  // Callback method that gets executed when |plugins|
  // are asynchronously fetched.
  void OnGotPlugins(const std::vector<content::WebPluginInfo>& plugins);
#endif
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_CONTENT_SETTINGS_CONTENT_SETTINGS_API_H_
