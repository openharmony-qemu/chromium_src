/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_CHROMIUM_EXT_CONTENT_PUBLIC_BROWSER_HTTPS_UPGRADES_POLICY_H_
#define ARKWEB_CHROMIUM_EXT_CONTENT_PUBLIC_BROWSER_HTTPS_UPGRADES_POLICY_H_

namespace content {

// HTTPS upgrade policy used in LoadURLParams.
enum class HttpsUpgradesPolicy {
  // No HTTPS upgrade policy specified.
  NONE,
  // Try to upgrade HTTP to HTTPS.
  TRY_UPGRADES,
  // Don't upgrade due to URL typed with HTTP scheme.
  DONT_UPGRADES_DUE_URL_TYPED_WITH_HTTP_SCHEME
};

}  // namespace content

#endif  // ARKWEB_CHROMIUM_EXT_CONTENT_PUBLIC_BROWSER_HTTPS_UPGRADES_POLICY_H_
