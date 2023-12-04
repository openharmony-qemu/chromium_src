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

  static NWebCookieManager* GetCookieManagerInstance();
  bool IsAcceptCookieAllowed() const override;
  void PutAcceptCookieEnabled(bool accept) override;
  bool IsThirdPartyCookieAllowed() const override;
  void PutAcceptThirdPartyCookieEnabled(bool accept) override;
  bool IsFileURLSchemeCookiesAllowed() const override;
  void PutAcceptFileURLSchemeCookiesEnabled(bool allow) override;
  void ReturnCookie(
      const std::string& url,
      std::shared_ptr<NWebValueCallback<std::string>> callback) override;
  std::string ReturnCookie(const std::string& url, bool &is_valid) override;
  void SetCookie(const std::string& url,
                 const std::string& value,
                 std::shared_ptr<NWebValueCallback<bool>> callback) override;
  int SetCookie(const std::string& url, const std::string& value) override;
  void ExistCookies(std::shared_ptr<NWebValueCallback<bool>> callback) override;
  bool ExistCookies() override;
  void Store(std::shared_ptr<NWebValueCallback<bool>> callback) override;
  bool Store() override;
  void DeleteSessionCookies(
      std::shared_ptr<NWebValueCallback<bool>> callback) override;
  void DeleteCookieEntirely(
      std::shared_ptr<NWebValueCallback<bool>> callback) override;

 private:
  std::shared_ptr<NWebCookieManagerDelegateInterface> delegate_;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_COOKIE_MANAGER_IMPL_H
