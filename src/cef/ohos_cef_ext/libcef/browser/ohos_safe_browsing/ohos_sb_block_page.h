// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_BLOCK_PAGE_H_
#define CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_BLOCK_PAGE_H_

#include <memory>

#include "components/security_interstitials/content/security_interstitial_controller_client.h"
#include "components/security_interstitials/content/security_interstitial_page.h"
#include "components/security_interstitials/core/metrics_helper.h"
#include "content/public/browser/web_contents.h"
#include "libcef/browser/ohos_safe_browsing/ohos_sb_controller_client.h"
#include "libcef/browser/ohos_safe_browsing/ohos_sb_threat_type.h"
#include "url/gurl.h"

namespace ohos_safe_browsing {

using security_interstitials::MetricsHelper;
using security_interstitials::SecurityInterstitialControllerClient;
using security_interstitials::SecurityInterstitialPage;

class SbBlockPage : public SecurityInterstitialPage {
 public:
  SbBlockPage(
      content::WebContents* web_contents,
      const GURL& request_url,
      int policy,
      OHSBThreatType block_type,
      std::unique_ptr<SecurityInterstitialControllerClient> controller_client);
  ~SbBlockPage() override;

  // DISALLOW_COPY_AND_ASSIGN
  SbBlockPage(const SbBlockPage&) = delete;
  SbBlockPage& operator=(const SbBlockPage&) = delete;

  std::string GetHTMLContents() override;
  void OnInterstitialClosing() override;

  bool ShouldDisplayURL() const override;

  void CommandReceived(const std::string& command) override;

  void PopulateInterstitialStrings(base::Value::Dict& load_time_data) override;

  void PopulateUrlTrustListInterstitialStrings(
      base::Value::Dict& load_time_data);

  std::string GetUrlTrustListErrorHTMLContents();

 private:
  static std::unique_ptr<MetricsHelper> GetMetricsHelper(const GURL& url);

  void HandleCommand(
      security_interstitials::SecurityInterstitialCommand command);

  OHSBThreatType block_type_;
  int policy_;
};

}  // namespace ohos_safe_browsing

#endif  // CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_BLOCK_PAGE_H_
