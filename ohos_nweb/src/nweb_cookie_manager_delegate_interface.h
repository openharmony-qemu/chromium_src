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

#ifndef NWEB_COOKIE_MANAGER_DELEGATE_INTERFACE_H
#define NWEB_COOKIE_MANAGER_DELEGATE_INTERFACE_H

#include <memory>
#include <string>
#include "nweb_value_callback.h"

namespace OHOS::NWeb {
class NWebCookieManagerDelegateInterface {
 public:
  virtual ~NWebCookieManagerDelegateInterface() = default;
  virtual bool IsAcceptCookieAllowed() = 0;
  virtual void PutAcceptCookieEnabled(bool accept) = 0;
  virtual bool IsThirdPartyCookieAllowed() = 0;
  virtual void PutAcceptThirdPartyCookieEnabled(bool accept) = 0;
  virtual bool IsFileURLSchemeCookiesAllowed() = 0;
  virtual void PutAcceptFileURLSchemeCookiesEnabled(bool allow) = 0;
  virtual void ReturnCookie(
      const std::string& url,
      std::shared_ptr<NWebStringValueCallback> callback) = 0;
  virtual std::string ReturnCookie(const std::string &url,
                                   bool &is_valid,
                                   bool incognito_mode) = 0;
  virtual void SetCookie(const std::string& url,
                         const std::string& value,
                         std::shared_ptr<NWebBoolValueCallback> callback) = 0;
  virtual int SetCookie(const std::string &url,
                        const std::string &value,
                        bool incognito_mode) = 0;
  virtual void ExistCookies(
      std::shared_ptr<NWebBoolValueCallback> callback) = 0;
  virtual bool ExistCookies(bool incognito_mode) = 0;
  virtual void Store(std::shared_ptr<NWebBoolValueCallback> callback) = 0;
  virtual bool Store() = 0;
  virtual void DeleteSessionCookies(
      std::shared_ptr<NWebBoolValueCallback> callback) = 0;
  virtual void DeleteCookieEntirely(
      std::shared_ptr<NWebBoolValueCallback> callback,
      bool incognito_mode) = 0;
  virtual void ConfigCookie(const std::string& url,
                            const std::string& value,
                            std::shared_ptr<NWebLongValueCallback> callback) = 0;
};
}  // namespace OHOS::NWeb
#endif
