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

#ifndef CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_INTERFACE_H_
#define CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_INTERFACE_H_

#include "base/supports_user_data.h"
#include "content/public/browser/web_contents.h"
#include "url/gurl.h"

namespace ohos_safe_browsing {
enum class UrlTrustCheckResult { RESULT_ALLOW, RESULT_DENY };

enum class UrlListSetResult : int {
  INIT_ERROR = -2,
  PARAM_ERROR = -1,
  SET_OK = 0,
};

class UrlTrustListInterface : public base::SupportsUserData::Data {
 public:
  static char interfaceKey;
  virtual ~UrlTrustListInterface() = default;
  virtual UrlTrustCheckResult CheckUrlTrustList(const GURL& url) {
    return UrlTrustCheckResult::RESULT_ALLOW;
  }
};
}  // namespace ohos_safe_browsing

#endif  // CEF_LIBCEF_BROWSER_OHOS_SAFE_BROWSING_OHOS_URL_TRUST_LIST_INTERFACE_H_
