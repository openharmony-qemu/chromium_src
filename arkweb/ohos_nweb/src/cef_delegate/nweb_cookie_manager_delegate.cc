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

#include "nweb_cookie_manager_delegate.h"

#include <memory>
#include <thread>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "base/synchronization/waitable_event.h"
#include "base/time/time.h"
#include "cef/include/cef_waitable_event.h"
#include "cef/libcef/common/net_service/net_service_util.h"
#include "cef/libcef/common/time_util.h"
#include "net/cookies/canonical_cookie.h"
#include "nweb_cookie_impl.h"
#include "url/gurl.h"

#if BUILDFLAG(ARKWEB_COOKIE)
#include "nweb_impl.h"
#endif

using namespace OHOS::NWeb;
using base::WaitableEvent;

namespace {
class CookieCompletionCallback : public CefCompletionCallback {
 public:
  explicit CookieCompletionCallback(
      std::shared_ptr<WaitableEvent> event,
      std::shared_ptr<NWebBoolValueCallback> callback)
      : event_(event), callback_(callback) {}
  void OnComplete() override {
    if (event_ != nullptr) {
      event_->Signal();
    }
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(true);
    }
  }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebBoolValueCallback> callback_;
  IMPLEMENT_REFCOUNTING(CookieCompletionCallback);
};

class CookieSetCallback : public CefSetCookieCallback {
 public:
  explicit CookieSetCallback(std::shared_ptr<WaitableEvent> event,
                             std::shared_ptr<NWebBoolValueCallback> callback)
      : event_(event), callback_(callback), set_success_(false) {}
  void OnComplete(bool success) override {
    set_success_ = success;
    if (event_ != nullptr) {
      event_->Signal();
    }
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(set_success_);
    }
  }

  bool IsSetSuccess() const { return set_success_; }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebBoolValueCallback> callback_;
  bool set_success_;

  IMPLEMENT_REFCOUNTING(CookieSetCallback);
};

class CookieConfigCallback : public CefSetCookieCallback {
 public:
  explicit CookieConfigCallback(std::shared_ptr<WaitableEvent> event,
                                std::shared_ptr<NWebLongValueCallback> callback)
      : event_(event), callback_(callback), set_success_(false) {}
  void OnComplete(bool success) override {
    set_success_ = success;
    long set_callback_success_ = success ? 1 : NWEB_INVALID_COOKIE_VALUE;
    if (event_ != nullptr) {
      event_->Signal();
    }
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(set_callback_success_);
    }
  }

  void OnErrorCode(long error_code) {
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(error_code);
    }
  }

  bool IsSetSuccess() const { return set_success_; }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebLongValueCallback> callback_;
  bool set_success_;

  IMPLEMENT_REFCOUNTING(CookieConfigCallback);
};

class HasCookieVisitor : public CefCookieVisitor {
 public:
  HasCookieVisitor() = delete;
  explicit HasCookieVisitor(std::shared_ptr<WaitableEvent> event,
                            std::shared_ptr<NWebBoolValueCallback> callback)
      : event_(event), callback_(callback) {}
  ~HasCookieVisitor() = default;

  bool Visit(const CefCookie& cookie,
             int count,
             int total,
             bool& deleteCookie) override {
    total_cookies_number = total;
    if (event_ != nullptr) {
      event_->Signal();
    }
    if (total == 0) {
      if (callback_ != nullptr) {
        callback_->OnReceiveValue(false);
      }
    }
    return true;
  }

  void SetCookieLine(const CefString& cookieLine) override {
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(true);
    }
  }

  bool IsExistCookies() const {
    return total_cookies_number == 0 ? false : true;
  }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebBoolValueCallback> callback_;
  IMPLEMENT_REFCOUNTING(HasCookieVisitor);
  int total_cookies_number;
};

class ReturnCookieVisitor : public CefCookieVisitor {
 public:
  ReturnCookieVisitor() = delete;
  explicit ReturnCookieVisitor(
      std::shared_ptr<WaitableEvent> event,
      std::shared_ptr<NWebStringValueCallback> callback)
      : event_(event),
        cookie_line_(std::string()),
        cookies_(std::vector<CefCookie>()),
        callback_(callback) {}
  ~ReturnCookieVisitor() = default;

  bool Visit(const CefCookie& cookie,
             int count,
             int total,
             bool& deleteCookie) override {
    if (total == 0) {
      if (callback_ != nullptr) {
        callback_->OnReceiveValue(cookie_line_);
      }
      if (event_ != nullptr) {
        event_->Signal();
      }
      return true;
    }
    cookies_.push_back(cookie);
    return true;
  }

  void SetCookieLine(const CefString& cookieLine) override {
    cookie_line_ = cookieLine.ToString();

    if (callback_ != nullptr) {
      callback_->OnReceiveValue(cookie_line_);
    }
    if (event_ != nullptr) {
      event_->Signal();
    }
  }

  void ReturnCookieError() {
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(std::to_string(NWEB_INVALID_URL));
    }
  }

  std::vector<CefCookie>& GetVisitorCookies() { return cookies_; }

  std::string ReturnCookieLine() const { return cookie_line_; }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::string cookie_line_;
  std::vector<CefCookie> cookies_;
  std::shared_ptr<NWebStringValueCallback> callback_;
  IMPLEMENT_REFCOUNTING(ReturnCookieVisitor);
};

class CookieDeleteCallback : public CefDeleteCookiesCallback {
 public:
  CookieDeleteCallback() = delete;
  CookieDeleteCallback(std::shared_ptr<WaitableEvent> event,
                       std::shared_ptr<NWebBoolValueCallback> callback)
      : event_(event), callback_(callback), num_deleted_(0) {}
  explicit CookieDeleteCallback(std::shared_ptr<NWebBoolValueCallback> callback)
      : event_(nullptr), callback_(callback), num_deleted_(0) {}
  explicit CookieDeleteCallback(std::shared_ptr<WaitableEvent> event)
      : event_(event), callback_(nullptr), num_deleted_(0) {}

  void OnComplete(int num_deleted) override {
    num_deleted_ = num_deleted;
    if (callback_ != nullptr) {
      if (num_deleted_ > 0) {
        callback_->OnReceiveValue(true);
      } else {
        callback_->OnReceiveValue(false);
      }
    }
    if (event_ != nullptr) {
      event_->Signal();
    }
  }

  int GetNumDeleted() const { return num_deleted_; }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebBoolValueCallback> callback_;
  int num_deleted_;

  IMPLEMENT_REFCOUNTING(CookieDeleteCallback);
};
}  // namespace

namespace OHOS::NWeb {
CefRefPtr<CefCookieManager>
NWebCookieManagerDelegate::GetGlobalCookieManager() {
  if (!cookie_manager_) {
#if BUILDFLAG(ARKWEB_COOKIE)
    if (NWebImpl::ShouldLazyInitWebEngine()) {
      if (!uninitialized_cookie_manager_) {
        uninitialized_cookie_manager_ = GetUninitializedCookieManagerExt(false);
        LOG_FEEDBACK(INFO, kNetwork) << "CreateUninitializedCookieManager";
      }
      return uninitialized_cookie_manager_;
    }
    cookie_manager_ = CefCookieManager::GetGlobalManager(nullptr);
    LOG_FEEDBACK(INFO, kNetwork) << "CreateCookieManager";
#endif
  }
  return cookie_manager_;
}

CefRefPtr<CefCookieManager>
NWebCookieManagerDelegate::GetGlobalIncognitoCookieManager() {
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  if (!incognito_cookie_manager_) {
#if BUILDFLAG(ARKWEB_COOKIE)
    if (NWebImpl::ShouldLazyInitWebEngine()) {
      if (!uninitialized_incognito_cookie_manager_) {
        uninitialized_incognito_cookie_manager_ =
            GetUninitializedCookieManagerExt(true);
        LOG_FEEDBACK(INFO, kNetwork) << "CreateUninitializedIncognitoCookieManager";
      }
      return uninitialized_incognito_cookie_manager_;
    }
    incognito_cookie_manager_ =
        CefCookieManager::GetGlobalIncognitoManager(nullptr);
    LOG_FEEDBACK(INFO, kNetwork) << "CreateIncognitoCookieManager";
#endif
  }
  return incognito_cookie_manager_;
#else
  return nullptr;
#endif
}

bool NWebCookieManagerDelegate::IsAcceptCookieAllowed() {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return false;
  }
  return cookie_manager->AsCefCookieManagerExt()->IsAcceptCookieAllowed();
}

void NWebCookieManagerDelegate::PutAcceptCookieEnabled(bool accept) {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return;
  }
  cookie_manager->AsCefCookieManagerExt()->PutAcceptCookieEnabled(accept);
}

bool NWebCookieManagerDelegate::IsThirdPartyCookieAllowed() {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return false;
  }
  return cookie_manager->AsCefCookieManagerExt()->IsThirdPartyCookieAllowed();
}

void NWebCookieManagerDelegate::PutAcceptThirdPartyCookieEnabled(bool accept) {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return;
  }
  cookie_manager->AsCefCookieManagerExt()->PutAcceptThirdPartyCookieEnabled(
      accept);
}

bool NWebCookieManagerDelegate::IsFileURLSchemeCookiesAllowed() {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return false;
  }
  return cookie_manager->AsCefCookieManagerExt()
      ->IsFileURLSchemeCookiesAllowed();
}

void NWebCookieManagerDelegate::PutAcceptFileURLSchemeCookiesEnabled(
    bool allow) {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return;
  }
  cookie_manager->AsCefCookieManagerExt()->PutAcceptFileURLSchemeCookiesEnabled(
      allow);
}

void NWebCookieManagerDelegate::ReturnCookie(
    const std::string& url,
    bool incognitoMode,
    std::shared_ptr<NWebStringValueCallback> callback) {
  CefRefPtr<CefCookieManager> cookie_manager =
      incognitoMode ? GetGlobalIncognitoCookieManager()
                    : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    if (callback != nullptr) {
      callback->OnReceiveValue(std::to_string(NWEB_INVALID_URL));
    }
    return;
  }
  CefRefPtr<ReturnCookieVisitor> visitor =
      new ReturnCookieVisitor(nullptr, callback);
  if (!cookie_manager->VisitUrlCookies(CefString(url), false, visitor, false,
                                       false)) {
    LOG(ERROR) << "VisitUrlCookies failed";
    visitor->ReturnCookieError();
    return;
  }
}

std::string NWebCookieManagerDelegate::ReturnCookie(const std::string& url,
                                                    bool& is_valid,
                                                    bool incognito_mode) {
  CefRefPtr<CefCookieManager> cookie_manager =
      incognito_mode ? GetGlobalIncognitoCookieManager()
                     : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return "";
  }
  CefRefPtr<ReturnCookieVisitor> visitor =
      new ReturnCookieVisitor(nullptr, nullptr);
  is_valid = true;
  if (!cookie_manager->VisitUrlCookies(CefString(url), false, visitor, true,
                                       false)) {
    LOG(ERROR) << "VisitUrlCookies failed";
    is_valid = false;
    return "";
  }
  return visitor->ReturnCookieLine();
}

#if BUILDFLAG(ARKWEB_COOKIE_NDK)
std::string NWebCookieManagerDelegate::ReturnCookieWithHttpOnly(
    const std::string& url,
    bool& is_valid,
    bool incognito_mode,
    bool includeHttpOnly) {
  CefRefPtr<CefCookieManager> cookie_manager =
      incognito_mode ? GetGlobalIncognitoCookieManager()
                     : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return "";
  }
  CefRefPtr<ReturnCookieVisitor> visitor =
      new ReturnCookieVisitor(nullptr, nullptr);
  is_valid = true;
  if (!cookie_manager->VisitUrlCookies(CefString(url), includeHttpOnly, visitor,
                                       true, true)) {
    LOG(ERROR) << "VisitUrlCookies failed";
    is_valid = false;
    return "";
  }
  return visitor->ReturnCookieLine();
}
#endif

bool FixInvalidGurl(const CefString& url, GURL& gurl) {
  if (!gurl.is_valid()) {
    GURL fixedGurl = GURL("https://" + url.ToString());
    if (fixedGurl.is_valid() && fixedGurl.host() == url.ToString()) {
      gurl = fixedGurl;
      return true;
    }
    return false;
  }
  return true;
}

void NWebCookieManagerDelegate::ConfigCookie(
    const std::string& url,
    const std::string& value,
    bool incognitoMode,
    bool includeHttpOnly,
    std::shared_ptr<NWebLongValueCallback> callback) {
  CefRefPtr<CookieConfigCallback> cookie_config_callback(
      new CookieConfigCallback(nullptr, callback));
  CefRefPtr<CefCookieManager> cookie_manager =
      incognitoMode ? GetGlobalIncognitoCookieManager()
                    : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    cookie_config_callback->OnErrorCode(NWEB_ERR);
    return;
  }
  GURL gurl = GURL(url);
  if (!FixInvalidGurl(url, gurl)) {
    LOG(ERROR) << "FixInvalidGurl failed";
    cookie_config_callback->OnErrorCode(NWEB_INVALID_URL);
    return;
  }
  CefCookie cef_cookie;
  if (!CefCookieManager::CreateCefCookie(CefString(gurl.spec()), CefString(value),
#if BUILDFLAG(ARKWEB_COOKIE)
                                         /*block_truncated=*/false,
#endif // BUILDFLAG(ARKWEB_COOKIE)
                                         cef_cookie)) {
    LOG(ERROR) << "CreateCefCookie failed";
    cookie_config_callback->OnErrorCode(NWEB_INVALID_COOKIE_VALUE);
    return;
  }

  if (!cookie_manager->SetCookie(CefString(gurl.spec()), cef_cookie,
                                 cookie_config_callback, false,
                                 CefString(value), includeHttpOnly)) {
    LOG(ERROR) << "SetCookie error";
    cookie_config_callback->OnErrorCode(NWEB_INVALID_URL);
    return;
  }
}

void NWebCookieManagerDelegate::SetCookie(
    const std::string& url,
    const std::string& value,
    std::shared_ptr<NWebBoolValueCallback> callback) {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    if (callback != nullptr) {
      callback->OnReceiveValue(false);
    }
    return;
  }
  GURL gurl = GURL(url);
  if (!FixInvalidGurl(url, gurl)) {
    if (callback != nullptr) {
      callback->OnReceiveValue(false);
    }
    return;
  }
  CefCookie cef_cookie;
  if (!CefCookieManager::CreateCefCookie(CefString(gurl.spec()), CefString(value),
#if BUILDFLAG(ARKWEB_COOKIE)
                                         /*block_truncated=*/false,
#endif // BUILDFLAG(ARKWEB_COOKIE)
                                         cef_cookie)) {
    LOG(ERROR) << "CreateCefCookie failed";
    if (callback != nullptr) {
      callback->OnReceiveValue(false);
    }
    return;
  }

  if (!cookie_manager->SetCookie(CefString(gurl.spec()), cef_cookie,
                                 new CookieSetCallback(nullptr, callback),
                                 false, CefString(value), false)) {
    LOG(ERROR) << "SetCookie error";
    if (callback != nullptr) {
      callback->OnReceiveValue(false);
    }
    return;
  }
}

int NWebCookieManagerDelegate::SetCookie(const std::string& url,
                                         const std::string& value,
                                         bool incognito_mode,
                                         bool includeHttpOnly) {
  CefRefPtr<CefCookieManager> cookie_manager =
      incognito_mode ? GetGlobalIncognitoCookieManager()
                     : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return NWEB_ERR;
  }
  CefCookie cef_cookie;
  GURL gurl = GURL(url);
  if (!FixInvalidGurl(url, gurl)) {
    return NWEB_INVALID_URL;
  }
  if (!CefCookieManager::CreateCefCookie(CefString(gurl.spec()), CefString(value),
#if BUILDFLAG(ARKWEB_COOKIE)
                                         /*block_truncated=*/false,
#endif // BUILDFLAG(ARKWEB_COOKIE)
                                         cef_cookie)) {
    LOG(ERROR) << "CreateCefCookie failed";
    return NWEB_INVALID_COOKIE_VALUE;
  }
  CefRefPtr<CookieSetCallback> callback(
      new CookieSetCallback(nullptr, nullptr));
  if (!cookie_manager->SetCookie(CefString(gurl.spec()), cef_cookie, callback,
                                 true, CefString(value), includeHttpOnly)) {
    LOG(ERROR) << "SetCookie error";
    return NWEB_INVALID_URL;
  }
  return callback->IsSetSuccess() ? NWEB_OK : NWEB_ERR;
}

#if BUILDFLAG(ARKWEB_COOKIE_NDK)
int NWebCookieManagerDelegate::SetCookieWithHttpOnly(const std::string& url,
                                                     const std::string& value,
                                                     bool incognito_mode,
                                                     bool includeHttpOnly) {
  CefRefPtr<CefCookieManager> cookie_manager =
      incognito_mode ? GetGlobalIncognitoCookieManager()
                     : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return NWEB_ERR;
  }
  CefCookie cef_cookie;
  GURL gurl = GURL(url);
  if (!FixInvalidGurl(url, gurl)) {
    return NWEB_INVALID_URL;
  }
  if (!CefCookieManager::CreateCefCookie(CefString(gurl.spec()), CefString(value),
#if BUILDFLAG(ARKWEB_COOKIE)
                                         /*block_truncated=*/false,
#endif // BUILDFLAG(ARKWEB_COOKIE)
                                         cef_cookie)) {
    LOG(ERROR) << "CreateCefCookie failed";
    return NWEB_INVALID_COOKIE_VALUE;
  }
  CefRefPtr<CookieSetCallback> callback(
      new CookieSetCallback(nullptr, nullptr));
  if (!cookie_manager->SetCookie(CefString(gurl.spec()), cef_cookie, callback,
                                 true, CefString(value), includeHttpOnly)) {
    LOG(ERROR) << "SetCookie error";
    return NWEB_INVALID_URL;
  }
  return callback->IsSetSuccess() ? NWEB_OK : NWEB_ERR;
}
#endif

void NWebCookieManagerDelegate::ExistCookies(
    std::shared_ptr<NWebBoolValueCallback> callback) {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    if (callback) {
      callback->OnReceiveValue(false);
    }
    return;
  }
  CefRefPtr<HasCookieVisitor> visitor = new HasCookieVisitor(nullptr, callback);
  if (!cookie_manager->VisitAllCookies(visitor, false)) {
    LOG(INFO) << "VisitAllCookies failed";
    if (callback) {
      callback->OnReceiveValue(false);
    }
    return;
  }
}

bool NWebCookieManagerDelegate::ExistCookies(bool incognito_mode) {
  CefRefPtr<CefCookieManager> cookie_manager =
      incognito_mode ? GetGlobalIncognitoCookieManager()
                     : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return false;
  }
  CefRefPtr<HasCookieVisitor> visitor = new HasCookieVisitor(nullptr, nullptr);
  cookie_manager->VisitAllCookies(visitor, true);
  return visitor->IsExistCookies();
}

void NWebCookieManagerDelegate::Store(
    std::shared_ptr<NWebBoolValueCallback> callback) {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    if (callback) {
      callback->OnReceiveValue(false);
    }
    return;
  }
  CefRefPtr<CefCompletionCallback> complete =
      new CookieCompletionCallback(nullptr, callback);
  if (!cookie_manager->FlushStore(complete)) {
    LOG(ERROR) << "Store Failed";
    if (callback) {
      callback->OnReceiveValue(false);
    }
  }
}

bool NWebCookieManagerDelegate::Store() {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    return false;
  }
  std::shared_ptr<base::WaitableEvent> event =
      std::make_shared<base::WaitableEvent>(
          base::WaitableEvent::ResetPolicy::AUTOMATIC,
          base::WaitableEvent::InitialState::NOT_SIGNALED);
  CefRefPtr<CookieCompletionCallback> complete =
      new CookieCompletionCallback(event, nullptr);
  if (!cookie_manager->FlushStore(complete)) {
    LOG(ERROR) << "Store Failed";
    return false;
  }
  event->TimedWait(base::Milliseconds(ENOUGH_WAITED_TIME));
  return true;
}

void NWebCookieManagerDelegate::DeleteSessionCookies(
    std::shared_ptr<NWebBoolValueCallback> callback) {
  CefRefPtr<CefCookieManager> cookie_manager = GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    if (callback) {
      callback->OnReceiveValue(false);
    }
    return;
  }
  CefRefPtr<CefDeleteCookiesCallback> delete_callback =
      new CookieDeleteCallback(callback);
  if (!cookie_manager->DeleteCookies(CefString(std::string()),
                                     CefString(std::string()), true,
                                     delete_callback, true)) {
    LOG(ERROR) << "DeleteCookieEntirely Failed";
    if (callback) {
      callback->OnReceiveValue(false);
    }
  }
}

void NWebCookieManagerDelegate::DeleteCookieEntirely(
    std::shared_ptr<NWebBoolValueCallback> callback,
    bool incognito_mode) {
  CefRefPtr<CefCookieManager> cookie_manager =
      incognito_mode ? GetGlobalIncognitoCookieManager()
                     : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    if (callback) {
      callback->OnReceiveValue(false);
    }
    return;
  }
  CefRefPtr<CefDeleteCookiesCallback> delete_callback =
      new CookieDeleteCallback(callback);
  if (!cookie_manager->DeleteCookies(CefString(std::string()),
                                     CefString(std::string()), false,
                                     delete_callback, true)) {
    LOG(ERROR) << "DeleteCookieEntirely Failed";
    if (callback) {
      callback->OnReceiveValue(false);
    }
  }
}

void NWebCookieManagerDelegate::GetAllCookies(
    bool incognito_mode,
    std::vector<std::shared_ptr<NWebCookie>>& cookies) {
  CefRefPtr<CefCookieManager> cookie_manager =
      incognito_mode ? GetGlobalIncognitoCookieManager()
                     : GetGlobalCookieManager();
  if (cookie_manager == nullptr) {
    LOG(ERROR) << "GetGlobalCookieManager failed";
    return;
  }
  CefRefPtr<ReturnCookieVisitor> visitor = new ReturnCookieVisitor(nullptr, nullptr);
  if (!cookie_manager->VisitAllCookies(visitor, true)) {
    LOG(ERROR) << "VisitAllCookies failed";
    return;
  }
  std::vector<CefCookie> cef_cookies = visitor->GetVisitorCookies();
  for (auto cef_cookie : cef_cookies) {
    std::shared_ptr<NWebCookieImpl> cookie = std::make_shared<NWebCookieImpl>();
    cookie->SetCookieAttribute(cef_cookie);
    cookies.push_back(cookie);
  }
}

#if BUILDFLAG(ARKWEB_COOKIE)
CefRefPtr<CefCookieManagerExt>
NWebCookieManagerDelegate::GetUninitializedCookieManagerExt(
    bool support_incognito) {
  CefRefPtr cookie_manager =
      CefCookieManagerImplExt::GetInstance(support_incognito);
  return cookie_manager.get();
}
#endif

}  // namespace OHOS::NWeb
