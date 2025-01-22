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

#include "cef/include/cef_base.h"
#include "cef/include/cef_web_storage.h"
#include "nweb_errors.h"
#include "nweb_value_callback.h"
#include "nweb_web_storage_delegate_interface.h"

namespace OHOS::NWeb {
class NWebWebStorageDelegate : public NWebWebStorageDelegateInterface {
 public:
  NWebWebStorageDelegate() = default;
  ~NWebWebStorageDelegate() = default;

  void DeleteAllData(bool incognito_mode) override;
  int DeleteOrigin(const std::string& origin) override;
  void GetOrigins(
      std::shared_ptr<NWebWebStorageOriginVectorValueCallback> callback) override;
  void GetOrigins(
      std::vector<std::shared_ptr<NWebWebStorageOrigin>>& origins) override;
  void GetOriginQuota(
      const std::string& origin,
      std::shared_ptr<NWebLongValueCallback> callback) override;
  long GetOriginQuota(const std::string& origin) override;
  void GetOriginUsage(
      const std::string& origin,
      std::shared_ptr<NWebLongValueCallback> callback) override;
  long GetOriginUsage(const std::string& origin) override;

#ifdef OHOS_EX_PASSWORD
  void RegisterWebStorageExtensionCallback(
      std::shared_ptr<NWebStorageExtensionCallback>
          web_storage_extension_callback) override;
  std::string GetPassword(const std::string& url,
                          const std::string& username,
                          int callback_id) override;
  void GetSavedPasswordsInfo(int callback_id) override;
  void MigratePasswordsInfo() override;
  void ClearPassword() override;
  void RemovePassword(const std::string& url,
                      const std::string& username) override;
  void RemovePasswordByUrl(const std::string& url) override;
  void ModifyPassword(const std::string& url,
                      const std::string& old_username,
                      const std::string& new_username,
                      const std::string& new_password) override;
#endif
 private:
  CefRefPtr<CefWebStorage> GetGlobalWebStorage();

  CefRefPtr<CefWebStorage> web_storage_ = nullptr;
#ifdef OHOS_EX_PASSWORD
  std::shared_ptr<NWebStorageExtensionCallback>
      web_storage_extension_callback_ = nullptr;
#endif  // OHOS_EX_PASSWORD

#if BUILDFLAG(IS_OHOS)
  CefRefPtr<CefWebStorage> GetGlobalIncognitoWebStorage();
  CefRefPtr<CefWebStorage> incognito_web_storage_ = nullptr;
#endif

};
}  // namespace OHOS::NWeb
#endif
