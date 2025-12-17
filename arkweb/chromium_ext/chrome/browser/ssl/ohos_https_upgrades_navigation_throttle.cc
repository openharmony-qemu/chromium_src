// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/chrome/browser/ssl/ohos_https_upgrades_navigation_throttle.h"

#include "arkweb/chromium_ext/chrome/browser/ssl/ohos_https_upgrades_helper.h"
#include "base/feature_list.h"
#include "base/memory/weak_ptr.h"
#include "base/task/sequenced_task_runner.h"
#include "base/time/time.h"
#include "base/logging.h"
#include "chrome/browser/ssl/https_first_mode_settings_tracker.h"
#include "chrome/browser/ssl/https_only_mode_tab_helper.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "components/security_interstitials/content/security_interstitial_tab_helper.h"
#include "components/security_interstitials/content/stateful_ssl_host_state_delegate.h"
#include "chrome/browser/ssl/stateful_ssl_host_state_delegate_factory.h"
#include "components/security_interstitials/core/https_only_mode_metrics.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/navigation_throttle.h"
#include "content/public/browser/web_contents.h"
#include "net/base/net_errors.h"
#include "ui/base/page_transition_types.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"

namespace {

// Time that the throttle will wait before canceling the upgraded navigation and
// showing the HTTPS-First Mode interstitial.
base::TimeDelta g_fallback_delay = base::Seconds(3);

}  // namespace

// static
std::unique_ptr<OhosHttpsUpgradesNavigationThrottle>
OhosHttpsUpgradesNavigationThrottle::MaybeCreateThrottleFor(
    content::NavigationHandle* handle) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  // HTTPS-First Mode is only relevant for primary main-frame HTTP(S)
  // navigations.
  if (!handle->GetURL().SchemeIsHTTPOrHTTPS() ||
      !handle->IsInPrimaryMainFrame() || handle->IsSameDocument()) {
    return nullptr;
  }
  auto* https_helper = OhosHttpsUpgradesHelper::FromWebContents(handle->GetWebContents());
  if ((!https_helper || !(https_helper->should_upgrade_to_https()))) {
    return nullptr;
  }

  // Ensure that the HttpsOnlyModeTabHelper has been created (this does nothing
  // if it has already been created for the WebContents). There are cases where
  // the tab helper won't get created by the initialization in
  // chrome/browser/ui/tab_helpers.cc but the criteria for adding the throttle
  // are still met (see crbug.com/1233889 for one example).
  HttpsOnlyModeTabHelper::CreateForWebContents(handle->GetWebContents());
  return std::make_unique<OhosHttpsUpgradesNavigationThrottle>(handle);
}

OhosHttpsUpgradesNavigationThrottle::OhosHttpsUpgradesNavigationThrottle(
    content::NavigationHandle* handle)
    : content::NavigationThrottle(handle) {}

OhosHttpsUpgradesNavigationThrottle::~OhosHttpsUpgradesNavigationThrottle() = default;

content::NavigationThrottle::ThrottleCheckResult
OhosHttpsUpgradesNavigationThrottle::WillStartRequest() {
  return content::NavigationThrottle::ThrottleAction::PROCEED;
}

content::NavigationThrottle::ThrottleCheckResult
OhosHttpsUpgradesNavigationThrottle::WillFailRequest() {
  // Fallback to HTTP on navigation failure is handled by
  // HttpsUpgradesInterceptor::MaybeCreateLoaderForResponse().
  return content::NavigationThrottle::PROCEED;
}

content::NavigationThrottle::ThrottleCheckResult
OhosHttpsUpgradesNavigationThrottle::WillRedirectRequest() {
  // If the navigation is doing a fallback redirect to HTTP, trigger the HTTP
  // interstitial (if enabled). The interceptor creates a redirect for the
  // fallback navigation, which will trigger MaybeCreateLoader() in the
  // interceptor for the redirect but *doesn't* trigger WillStartRequest()
  // because it's all part of the same request.
  auto* handle = navigation_handle();
  auto* contents = handle->GetWebContents();
  auto* tab_helper = HttpsOnlyModeTabHelper::FromWebContents(contents);
  if (tab_helper == nullptr) {
    return content::NavigationThrottle::PROCEED;
  }

  if (tab_helper->is_navigation_upgraded()) {
    // Check if the timer is already started, as there may be additional
    // redirects on the navigation after the artificial upgrade redirect.
    navigation_handle()->SetNavigationTimeout(g_fallback_delay);
  }

  return content::NavigationThrottle::PROCEED;
}

content::NavigationThrottle::ThrottleCheckResult
OhosHttpsUpgradesNavigationThrottle::WillProcessResponse() {
  // Clear the status for this navigation as it will successfully commit.
  auto* tab_helper = HttpsOnlyModeTabHelper::FromWebContents(
      navigation_handle()->GetWebContents());
  if (tab_helper == nullptr) {
    return content::NavigationThrottle::PROCEED;
  }

  if (tab_helper->is_navigation_upgraded()) {
    tab_helper->set_is_navigation_upgraded(false);
  }

  // Clear the fallback flag, if set.
  tab_helper->set_is_navigation_fallback(false);

  return content::NavigationThrottle::PROCEED;
}

const char* OhosHttpsUpgradesNavigationThrottle::GetNameForLogging() {
  return "OhosHttpsUpgradesNavigationThrottle";
}

