/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "base/i18n/time_formatting.h"
#include "nweb_cookie_impl.h"

namespace OHOS::NWeb {

void NWebCookieImpl::SetCookieAttribute(const CefCookie& cookie) {
    samesite_policy_ = cookie.same_site;
    name_ = CefString(&cookie.name).ToString();
    if (cookie.has_expires) {
      expires_date_ = base::UnlocalizedTimeFormatWithPattern(
        CefBaseTime(cookie.expires), "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
      is_session_cookie_ = false;
    } else {
      is_session_cookie_ = true;
    }
    value_ = CefString(&cookie.value).ToString();
    path_ = CefString(&cookie.path).ToString();
    is_http_only_ = cookie.httponly;
    is_secure_ = cookie.secure;
    domain_ = CefString(&cookie.domain).ToString();
}

int NWebCookieImpl::GetSamesitePolicy() {
  return samesite_policy_;
}

std::string NWebCookieImpl::GetExpiresDate() {
  return expires_date_;
}

std::string NWebCookieImpl::GetName() {
  return name_;
}

bool NWebCookieImpl::GetIsSessionCookie() {
  return is_session_cookie_;
}

std::string NWebCookieImpl::GetValue() {
  return value_;
}

std::string NWebCookieImpl::GetPath() {
  return path_;
}

bool NWebCookieImpl::GetIsHttpOnly() {
  return is_http_only_;
}

bool NWebCookieImpl::GetIsSecure() {
  return is_secure_;
}

std::string NWebCookieImpl::GetDomain() {
  return domain_;
}

}  // namespace OHOS::NWeb
