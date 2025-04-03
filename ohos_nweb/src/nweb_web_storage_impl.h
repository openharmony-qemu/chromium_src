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

#ifndef NWEB_WEB_STORAGE_IMPL_H
#define NWEB_WEB_STORAGE_IMPL_H

#include "build/build_config.h"
#include "capi/nweb_storage_extension_callback.h"
#include "nweb_errors.h"
#include "nweb_web_storage.h"
#include "nweb_web_storage_delegate_interface.h"

namespace OHOS::NWeb {
class NWebWebStorageImpl : public NWebWebStorage {
 public:
  NWebWebStorageImpl();
  ~NWebWebStorageImpl() = default;

  void DeleteAllData(bool incognito_mode) override;
  int DeleteOrigin(const std::string& origin) override;
  void GetOrigins(
      std::shared_ptr<NWebWebStorageOriginVectorValueCallback> callback) override;
  std::vector<std::shared_ptr<NWebWebStorageOrigin>> GetOrigins() override;
  void GetOriginQuota(
      const std::string& origin,
      std::shared_ptr<NWebLongValueCallback> callback) override;
  long GetOriginQuota(const std::string& origin) override;
  void GetOriginUsage(
      const std::string& origin,
      std::shared_ptr<NWebLongValueCallback> callback) override;
  long GetOriginUsage(const std::string& origin) override;

#if BUILDFLAG(IS_OHOS)
  // #ifdef OHOS_EX_PASSWORD
  void PutWebStorageCallback(std::shared_ptr<NWebStorageExtensionCallback>
                                 web_storage_extension_callback);
  std::string GetPassword(const std::string& url,
                          const std::string& username,
                          int callback_id);
  void GetSavedPasswords(int callback_id);
  void MigratePasswords();
  void ClearPassword();
  void RemovePassword(const std::string& url, const std::string& username);
  void RemovePasswordByUrl(const std::string& url);
  void ModifyPassword(const std::string& url,
                      const std::string& old_username,
                      const std::string& new_username,
                      const std::string& new_password);
  // #endif OHOS_EX_PASSWORD
#endif
 private:
  std::shared_ptr<NWebWebStorageDelegateInterface> delegate_;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_WEB_STORAGE_IMPL_H
