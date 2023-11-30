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
#include "nweb_hilog.h"

using namespace OHOS::NWeb;

extern "C" OHOS_NWEB_EXPORT NWebCookieManager* GetCookieManager() {
  WVLOG_I("GetCookieManager");
  return NWebCookieManagerImpl::GetCookieManagerInstance();
}

namespace OHOS::NWeb {

NWebCookieManager* NWebCookieManagerImpl::GetCookieManagerInstance() {
  static NWebCookieManagerImpl cookie_manager;
  return &cookie_manager;
}

NWebCookieManagerImpl::NWebCookieManagerImpl() {
#if defined(USE_CEF)
  delegate_ = std::make_shared<NWebCookieManagerDelegate>();
#endif
}

bool NWebCookieManagerImpl::IsAcceptCookieAllowed() const {
  if (delegate_ != nullptr) {
    return delegate_->IsAcceptCookieAllowed();
  }
  return false;
}

void NWebCookieManagerImpl::PutAcceptCookieEnabled(bool accept) {
  if (delegate_ != nullptr) {
    delegate_->PutAcceptCookieEnabled(accept);
  }
}

bool NWebCookieManagerImpl::IsThirdPartyCookieAllowed() const {
  if (delegate_ != nullptr) {
    return delegate_->IsThirdPartyCookieAllowed();
  }
  return false;
}

void NWebCookieManagerImpl::PutAcceptThirdPartyCookieEnabled(bool accept) {
  if (delegate_ != nullptr) {
    delegate_->PutAcceptThirdPartyCookieEnabled(accept);
  }
}

bool NWebCookieManagerImpl::IsFileURLSchemeCookiesAllowed() const {
  if (delegate_ != nullptr) {
    return delegate_->IsFileURLSchemeCookiesAllowed();
  }
  return false;
}

void NWebCookieManagerImpl::PutAcceptFileURLSchemeCookiesEnabled(bool allow) {
  if (delegate_ != nullptr) {
    delegate_->PutAcceptFileURLSchemeCookiesEnabled(allow);
  }
}

void NWebCookieManagerImpl::SetCookie(
    const std::string& url,
    const std::string& value,
    std::shared_ptr<NWebValueCallback<bool>> callback) {
  if (delegate_ != nullptr) {
    delegate_->SetCookie(url, value, callback);
  }
}

int NWebCookieManagerImpl::SetCookie(const std::string& url,
                                     const std::string& value) {
  if (delegate_ != nullptr) {
    return delegate_->SetCookie(url, value);
  }
  return NWEB_ERR;
}

void NWebCookieManagerImpl::ReturnCookie(
    const std::string& url,
    std::shared_ptr<NWebValueCallback<std::string>> callback) {
  if (delegate_ != nullptr) {
    delegate_->ReturnCookie(url, callback);
  }
}

std::string NWebCookieManagerImpl::ReturnCookie(const std::string& url, bool &is_valid) {

  if (delegate_ != nullptr) {
    return delegate_->ReturnCookie(url, is_valid);
  }
  return "";
}

void NWebCookieManagerImpl::ExistCookies(
    std::shared_ptr<NWebValueCallback<bool>> callback) {
  if (delegate_ != nullptr) {
    delegate_->ExistCookies(callback);
  }
}

bool NWebCookieManagerImpl::ExistCookies() {
  if (delegate_ != nullptr) {
    return delegate_->ExistCookies();
  }
  return false;
}

void NWebCookieManagerImpl::Store(
    std::shared_ptr<NWebValueCallback<bool>> callback) {
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
    std::shared_ptr<NWebValueCallback<bool>> callback) {
  if (delegate_ != nullptr) {
    delegate_->DeleteSessionCookies(callback);
  }
}

void NWebCookieManagerImpl::DeleteCookieEntirely(
    std::shared_ptr<NWebValueCallback<bool>> callback) {
  if (delegate_ != nullptr) {
    delegate_->DeleteCookieEntirely(callback);
  }
}
}  // namespace OHOS::NWeb
                          