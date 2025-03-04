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

#ifndef NWEB_COOKIE_MANAGER_DELEGATE_H
#define NWEB_COOKIE_MANAGER_DELEGATE_H

#include "arkweb/build/features/features.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_cookie.h"
#include "nweb_cookie_manager_delegate_interface.h"
#include "nweb_errors.h"
#include "nweb_value_callback.h"

namespace OHOS::NWeb {
class NWebCookieManagerDelegate : public NWebCookieManagerDelegateInterface {
 public:
  NWebCookieManagerDelegate() = default;
  ~NWebCookieManagerDelegate() = default;

  bool IsAcceptCookieAllowed() override;
  void PutAcceptCookieEnabled(bool accept) override;
  bool IsThirdPartyCookieAllowed() override;
  void PutAcceptThirdPartyCookieEnabled(bool accept) override;
  bool IsFileURLSchemeCookiesAllowed() override;
  void PutAcceptFileURLSchemeCookiesEnabled(bool allow) override;
  void ReturnCookie(const std::string& url,
                    bool incognitoMode,
                    std::shared_ptr<NWebStringValueCallback> callback) override;
  std::string ReturnCookie(const std::string& url,
                           bool& is_valid,
                           bool incognito_mode) override;
#if BUILDFLAG(ARKWEB_COOKIE_NDK)
  std::string ReturnCookieWithHttpOnly(const std::string& url,
                                       bool& is_valid,
                                       bool incognito_mode,
                                       bool includeHttpOnly) override;
#endif
  void SetCookie(const std::string& url,
                 const std::string& value,
                 std::shared_ptr<NWebBoolValueCallback> callback) override;
  int SetCookie(const std::string& url,
                const std::string& value,
                bool incognito_mode,
                bool includeHttpOnly) override;
#if BUILDFLAG(ARKWEB_COOKIE_NDK)
  int SetCookieWithHttpOnly(const std::string& url,
                            const std::string& value,
                            bool incognito_mode,
                            bool includeHttpOnly) override;
#endif
  void ExistCookies(std::shared_ptr<NWebBoolValueCallback> callback) override;
  bool ExistCookies(bool incognito_mode) override;
  void Store(std::shared_ptr<NWebBoolValueCallback> callback) override;
  bool Store() override;
  void DeleteSessionCookies(
      std::shared_ptr<NWebBoolValueCallback> callback) override;
  void DeleteCookieEntirely(std::shared_ptr<NWebBoolValueCallback> callback,
                            bool incognito_mode) override;
  void ConfigCookie(const std::string& url,
                    const std::string& value,
                    bool incognitoMode,
                    bool includeHttpOnly,
                    std::shared_ptr<NWebLongValueCallback> callback) override;

 private:
  CefRefPtr<CefCookieManager> GetGlobalCookieManager();
  CefRefPtr<CefCookieManager> cookie_manager_ = nullptr;
  const int ENOUGH_WAITED_TIME = 500;

  CefRefPtr<CefCookieManager> GetGlobalIncognitoCookieManager();
  CefRefPtr<CefCookieManager> incognito_cookie_manager_ = nullptr;
};
}  // namespace OHOS::NWeb
#endif
