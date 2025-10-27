// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_CONTROLLER_CLIENT_H_
#define CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_CONTROLLER_CLIENT_H_

#include <memory>
#include <string>

#include "components/prefs/pref_service.h"
#include "components/security_interstitials/content/security_interstitial_controller_client.h"
#include "content/browser/renderer_host/navigation_entry_impl.h"
#include "url/gurl.h"
#include "base/memory/raw_ptr.h"

namespace content {
class WebContents;
}  // namespace content

namespace security_interstitials {
class MetricsHelper;
class SettingsPageHelper;
}  // namespace security_interstitials

using security_interstitials::MetricsHelper;
using security_interstitials::SecurityInterstitialControllerClient;

class SbControllerClient : public SecurityInterstitialControllerClient {
 public:
  SbControllerClient(content::WebContents* web_contents,
                     PrefService* prefs,
                     const GURL& url,
                     const std::string& app_locale,
                     bool incognito_mode);

  ~SbControllerClient() override;

  void Proceed() override;
  void Reload() override;
  void GoBack() override;
  PrefService* GetPrefService() override;
  const std::string& GetApplicationLocale() const override;

 protected:
  const std::string GetExtendedReportingPrefName() const override;
  raw_ptr<content::WebContents> web_contents_;

 private:
  static std::unique_ptr<MetricsHelper> GetMetricsHelper(const GURL& url);

  raw_ptr<PrefService> prefs_;
  const std::string app_locale_;
  GURL url_;
  bool incognito_mode_{false};
};

#endif  // CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_CONTROLLER_CLIENT_H_
