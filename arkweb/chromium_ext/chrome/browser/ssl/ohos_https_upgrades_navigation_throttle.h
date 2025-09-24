// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_BROWSER_SSL_HTTPS_UPGRADES_NAVIGATION_THROTTLE_H_
#define OHOS_BROWSER_SSL_HTTPS_UPGRADES_NAVIGATION_THROTTLE_H_

#include <memory>

#include "components/security_interstitials/content/security_blocking_page_factory.h"
#include "components/security_interstitials/core/https_only_mode_metrics.h"
#include "content/public/browser/navigation_throttle.h"

class Profile;

// OhosHttpsUpgradesNavigationThrottle is responsible for observing HTTPS-First Mode
// navigations that have been upgraded by OhosHttpsUpgradesInterceptor, timing them
// out if they take too long, and catching fallback navigations to HTTP and
// triggering the HTTPS-First Mode interstitial.
//
// Metadata about the navigation state (as it pertains to HTTPS-First Mode)
// shared between OhosHttpsUpgradesInterceptor and OhosHttpsUpgradesNavigationThrottle
// is stored in an HttpsOnlyModeTabHelper set as user-data on the WebContents in
// which the navigation occurs. (Such metadata might ordinarily be added to
// ChromeNavigationUIData, but the Interceptor only receives a clone of the
// data, so it can't be used as a channel between these classes.)
class OhosHttpsUpgradesNavigationThrottle : public content::NavigationThrottle {
 public:
  static std::unique_ptr<OhosHttpsUpgradesNavigationThrottle>
  MaybeCreateThrottleFor(content::NavigationHandle* handle);

  OhosHttpsUpgradesNavigationThrottle(content::NavigationHandle* handle);
  ~OhosHttpsUpgradesNavigationThrottle() override;

  OhosHttpsUpgradesNavigationThrottle(const OhosHttpsUpgradesNavigationThrottle&) =
      delete;
  OhosHttpsUpgradesNavigationThrottle& operator=(
      const OhosHttpsUpgradesNavigationThrottle&) = delete;

  // content::NavigationThrottle:
  content::NavigationThrottle::ThrottleCheckResult WillStartRequest() override;
  content::NavigationThrottle::ThrottleCheckResult WillRedirectRequest()
      override;
  content::NavigationThrottle::ThrottleCheckResult WillFailRequest() override;
  content::NavigationThrottle::ThrottleCheckResult WillProcessResponse()
      override;
  const char* GetNameForLogging() override;
};

#endif  // OHOS_BROWSER_SSL_HTTPS_UPGRADES_NAVIGATION_THROTTLE_H_
