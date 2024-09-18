/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_cookie_manager_impl.h"
#include "nweb_cookie_manager_delegate.h"
#include "nweb_default_engine_init_args_impl.h"
#include "nweb_hilog.h"

#if defined(OHOS_COOKIE)
#include "nweb_impl.h"
#endif // defined(OHOS_COOKIE)

namespace OHOS::NWeb {

// static
std::shared_ptr<NWebCookieManager> NWebCookieManagerImpl::GetInstance() {
  static std::shared_ptr<NWebCookieManagerImpl> cookie_manager =
    std::make_shared<NWebCookieManagerImpl>();
  return cookie_manager;
}

NWebCookieManagerImpl::NWebCookieManagerImpl() {
#if defined(USE_CEF)
  delegate_ = std::make_shared<NWebCookieManagerDelegate>();
#endif
#if defined(OHOS_COOKIE)
  std::shared_ptr<NWebEngineInitArgs> init_args = std::make_shared<NWebDefaultEngineInitArgsImpl>();
  (void)NWebImpl::InitializeICUStatic(init_args);
#endif // defined(OHOS_COOKIE)
}

bool NWebCookieManagerImpl::IsAcceptCookieAllowed() {
  if (delegate_ != nullptr) {
    return delegate_->IsAcceptCookieAllowed();
  }
  return false;
}

void NWebCookieManagerImpl::PutAcceptCookieEnabled(bool accept) {
  if (delegate_ != nullptr) {
    WVLOG_I("PutAcceptCookieEnabled accept: %{public}d", accept);
    delegate_->PutAcceptCookieEnabled(accept);
  }
}

bool NWebCookieManagerImpl::IsThirdPartyCookieAllowed() {
  if (delegate_ != nullptr) {
    return delegate_->IsThirdPartyCookieAllowed();
  }
  return false;
}

void NWebCookieManagerImpl::PutAcceptThirdPartyCookieEnabled(bool accept) {
  if (delegate_ != nullptr) {
    WVLOG_I("PutAcceptThirdPartyCookieEnabled accept: %{public}d", accept);
    delegate_->PutAcceptThirdPartyCookieEnabled(accept);
  }
}

bool NWebCookieManagerImpl::IsFileURLSchemeCookiesAllowed() {
  if (delegate_ != nullptr) {
    return delegate_->IsFileURLSchemeCookiesAllowed();
  }
  return false;
}

void NWebCookieManagerImpl::PutAcceptFileURLSchemeCookiesEnabled(bool allow) {
  if (delegate_ != nullptr) {
    WVLOG_I("PutAcceptFileURLSchemeCookiesEnabled allow: %{public}d", allow);
    delegate_->PutAcceptFileURLSchemeCookiesEnabled(allow);
  }
}

void NWebCookieManagerImpl::ConfigCookie(
    const std::string& url,
    const std::string& value,
    std::shared_ptr<NWebLongValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->ConfigCookie(url, value, false, false, callback);
  }
}

void NWebCookieManagerImpl::SetCookie(
    const std::string& url,
    const std::string& value,
    std::shared_ptr<NWebBoolValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->SetCookie(url, value, callback);
  }
}

int NWebCookieManagerImpl::SetCookie(
    const std::string& url, const std::string& value, bool incognito_mode) {
  if (delegate_ != nullptr) {
    return delegate_->SetCookie(url, value, incognito_mode, false);
  }
  return NWEB_ERR;
}

int NWebCookieManagerImpl::SetCookieWithHttpOnly(
    const std::string& url, const std::string& value, bool incognito_mode, bool includeHttpOnly) {
#ifdef OHOS_COOKIE_NDK
  if (delegate_ != nullptr) {
    return delegate_->SetCookieWithHttpOnly(url, value, incognito_mode, includeHttpOnly);
  }
#endif
  return NWEB_ERR;
}

void NWebCookieManagerImpl::ReturnCookie(
    const std::string& url,
    std::shared_ptr<NWebStringValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->ReturnCookie(url, false, callback);
  }
}

std::string NWebCookieManagerImpl::ReturnCookie(
    const std::string& url, bool& is_valid, bool incognito_mode) {
  if (delegate_ != nullptr) {
    return delegate_->ReturnCookie(url, is_valid, incognito_mode);
  }
  return "";
}

std::string NWebCookieManagerImpl::ReturnCookieWithHttpOnly(
    const std::string& url, bool& is_valid, bool incognito_mode, bool includeHttpOnly) {
#ifdef OHOS_COOKIE_NDK
  if (delegate_ != nullptr) {
    return delegate_->ReturnCookieWithHttpOnly(url, is_valid, incognito_mode, includeHttpOnly);
  }
#endif
  return "";
}

void NWebCookieManagerImpl::ExistCookies(
    std::shared_ptr<NWebBoolValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->ExistCookies(callback);
  }
}

bool NWebCookieManagerImpl::ExistCookies(bool incognito_mode) {
  if (delegate_ != nullptr) {
    return delegate_->ExistCookies(incognito_mode);
  }
  return false;
}

void NWebCookieManagerImpl::Store(
    std::shared_ptr<NWebBoolValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->Store(callback);
  }
}

bool NWebCookieManagerImpl::Store() {
  if (delegate_ != nullptr) {
    return delegate_->Store();
  }
  return false;
}

void NWebCookieManagerImpl::DeleteSessionCookies(
    std::shared_ptr<NWebBoolValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->DeleteSessionCookies(callback);
  }
}

void NWebCookieManagerImpl::DeleteCookieEntirely(
    std::shared_ptr<NWebBoolValueCallback> callback, bool incognito_mode) {
  if (delegate_ != nullptr) {
    delegate_->DeleteCookieEntirely(callback, incognito_mode);
  }
}

void NWebCookieManagerImpl::GetCookieAsync(
    const std::string& url,
    bool incognitoMode,
    std::shared_ptr<NWebStringValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->ReturnCookie(url, incognitoMode, callback);
  }
}

int NWebCookieManagerImpl::SetCookieSync(const std::string& url,
                                         const std::string& value,
                                         bool incognitoMode,
                                         bool includeHttpOnly) {
  if (delegate_ != nullptr) {
    return delegate_->SetCookie(url, value, incognitoMode, includeHttpOnly);
  }
  return NWEB_ERR;
}

void NWebCookieManagerImpl::SetCookieAsync(
    const std::string& url,
    const std::string& value,
    bool incognitoMode,
    bool includeHttpOnly,
    std::shared_ptr<NWebLongValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->ConfigCookie(url, value, incognitoMode, includeHttpOnly,
                            callback);
  }
}
}  // namespace OHOS::NWeb
                          
