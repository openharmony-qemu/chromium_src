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

#ifndef CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_NAVIGATION_THROTTLE_H_
#define CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_NAVIGATION_THROTTLE_H_

#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/navigation_throttle.h"

namespace ohos_safe_browsing {
class UrlTrustListNavigationThrottle : public content::NavigationThrottle {
 public:
  static std::unique_ptr<content::NavigationThrottle> Create(
      content::NavigationHandle* handle);

  UrlTrustListNavigationThrottle(content::NavigationHandle* navigation_handle);
  ~UrlTrustListNavigationThrottle() override;

  ThrottleCheckResult WillStartRequest() override;
  const char* GetNameForLogging() override;
};
}  // namespace ohos_safe_browsing
#endif  // CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_NAVIGATION_THROTTLE_H_
