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

#ifndef NWEB_COOKIE_IMPL_H
#define NWEB_COOKIE_IMPL_H

#include "cef/include/cef_base.h"
#include "nweb_cookie_manager.h"

namespace OHOS::NWeb {

class NWebCookieImpl : public NWebCookie {
public:
  NWebCookieImpl() = default;
  ~NWebCookieImpl() = default;

  int GetSamesitePolicy() override;
  std::string GetExpiresDate() override;
  std::string GetName() override;
  bool GetIsSessionCookie() override;
  std::string GetValue() override;
  std::string GetPath() override;
  bool GetIsHttpOnly() override;
  bool GetIsSecure() override;
  std::string GetDomain() override;

  void SetCookieAttribute(const CefCookie& cookie);

private:
  int samesite_policy_;
  std::string expires_date_;
  std::string name_;
  bool is_session_cookie_;
  std::string value_;
  std::string path_;
  bool is_http_only_;
  bool is_secure_;
  std::string domain_;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_COOKIE_IMPL_H
