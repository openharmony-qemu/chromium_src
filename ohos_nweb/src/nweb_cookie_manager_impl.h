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

#ifndef NWEB_COOKIE_MANAGER_IMPL_H
#define NWEB_COOKIE_MANAGER_IMPL_H

#include "nweb_cookie_manager.h"
#include "nweb_cookie_manager_delegate_interface.h"
#include "nweb_errors.h"

namespace OHOS::NWeb {
class NWebCookieManagerImpl : public NWebCookieManager {
 public:
  NWebCookieManagerImpl();
  ~NWebCookieManagerImpl() = default;

  static std::shared_ptr<NWebCookieManager> GetInstance();
  bool IsAcceptCookieAllowed() override;
  void PutAcceptCookieEnabled(bool accept) override;
  bool IsThirdPartyCookieAllowed() override;
  void PutAcceptThirdPartyCookieEnabled(bool accept) override;
  bool IsFileURLSchemeCookiesAllowed() override;
  void PutAcceptFileURLSchemeCookiesEnabled(bool allow) override;
  void ReturnCookie(
      const std::string& url,
      std::shared_ptr<NWebStringValueCallback> callback) override;
  std::string ReturnCookie(const std::string &url,
                           bool &is_valid,
                           bool incognito_mode) override;
  std::string ReturnCookieWithHttpOnly(const std::string& url,
                                       bool& is_valid,
                                       bool incognito_mode,
                                       bool includeHttpOnly) override;
  void SetCookie(const std::string& url,
                 const std::string& value,
                 std::shared_ptr<NWebBoolValueCallback> callback) override;
  int SetCookie(const std::string &url,
                const std::string &value,
                bool incognito_mode) override;
  int SetCookieWithHttpOnly(const std::string &url,
                            const std::string &value,
                            bool incognito_mode,
                            bool includeHttpOnly) override;
  void ExistCookies(std::shared_ptr<NWebBoolValueCallback> callback) override;
  bool ExistCookies(bool incognito_mode) override;
  void Store(std::shared_ptr<NWebBoolValueCallback> callback) override;
  bool Store() override;
  void DeleteSessionCookies(
      std::shared_ptr<NWebBoolValueCallback> callback) override;
  void DeleteCookieEntirely(
      std::shared_ptr<NWebBoolValueCallback> callback,
      bool incognito_mode) override;
  void ConfigCookie(const std::string& url,
                    const std::string& value,
                    std::shared_ptr<NWebLongValueCallback> callback) override;
  void GetCookieAsync(
      const std::string& url,
      bool incognitoMode,
      std::shared_ptr<NWebStringValueCallback> callback) override;
  int SetCookieSync(const std::string& url,
                    const std::string& value,
                    bool incognitoMode,
                    bool includeHttpOnly) override;
  void SetCookieAsync(const std::string& url,
                      const std::string& value,
                      bool incognitoMode,
                      bool includeHttpOnly,
                      std::shared_ptr<NWebLongValueCallback> callback) override;

 private:
  std::shared_ptr<NWebCookieManagerDelegateInterface> delegate_;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_COOKIE_MANAGER_IMPL_H
