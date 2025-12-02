/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "ohos_url_trust_list_navigation_throttle.h"

#include "base/i18n/rtl.h"
#include "components/security_interstitials/content/security_interstitial_tab_helper.h"
#include "content/public/browser/web_contents.h"
#include "ohos_safe_browsing_response.h"
#include "ohos_sb_block_page.h"
#include "ohos_sb_controller_client.h"
#include "ohos_sb_threat_type.h"
#include "ohos_url_trust_list_interface.h"

using NavigationThrottle = content::NavigationThrottle;
using ThrottleCheckResult = content::NavigationThrottle::ThrottleCheckResult;

namespace ohos_safe_browsing {
std::unique_ptr<NavigationThrottle> UrlTrustListNavigationThrottle::Create(
    content::NavigationThrottleRegistry& registry) {
  return base::WrapUnique(new UrlTrustListNavigationThrottle(registry));
}

UrlTrustListNavigationThrottle::UrlTrustListNavigationThrottle(
    content::NavigationThrottleRegistry& registry)
    : NavigationThrottle(registry) {}

UrlTrustListNavigationThrottle::~UrlTrustListNavigationThrottle() = default;

ThrottleCheckResult UrlTrustListNavigationThrottle::WillStartRequest() {
  content::NavigationHandle* handle = navigation_handle();
  if (!handle || !handle->IsInMainFrame() ||
      handle->GetNetErrorCode() == net::ERR_BLOCKED_BY_CLIENT) {
    return PROCEED;
  }

  content::WebContents* webContents = handle->GetWebContents();
  if (!webContents) {
    return PROCEED;
  }
  auto manager = reinterpret_cast<UrlTrustListInterface*>(
      webContents->GetUserData(&UrlTrustListInterface::interfaceKey));
  if (!manager) {
    return PROCEED;
  }
  if (manager->CheckUrlTrustList(handle->GetURL()) !=
      UrlTrustCheckResult::RESULT_ALLOW) {
    auto gurl = handle->GetURL();
    auto locale = base::i18n::GetConfiguredLocale();
    auto controller = std::make_unique<SbControllerClient>(webContents, nullptr,
                                                           gurl, locale, false);
    std::unique_ptr<SbBlockPage> blocking_page = std::make_unique<SbBlockPage>(
        webContents, gurl, OHSBPolicyType::POLICY_URL_TRUST_LIST,
        OHSBThreatType::THREAT_URL_TRUST_LIST, std::move(controller));
    std::string html = blocking_page->GetUrlTrustListErrorHTMLContents();
    security_interstitials::SecurityInterstitialTabHelper::
        AssociateBlockingPage(handle, std::move(blocking_page));
    return {BLOCK_REQUEST, net::ERR_BLOCKED_BY_CLIENT, html};
  }
  return PROCEED;
}

const char* UrlTrustListNavigationThrottle::GetNameForLogging() {
  return "UrlTrustListNavigationThrottle";
}
}  // namespace ohos_safe_browsing
