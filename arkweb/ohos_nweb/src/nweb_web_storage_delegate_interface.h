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

#ifndef NWEB_WEB_STORAGE_DELEGATE_INTERFACE_H
#define NWEB_WEB_STORAGE_DELEGATE_INTERFACE_H

#include <memory>
#include <string>
#include <vector>
#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "capi/nweb_storage_extension_callback.h"
#include "nweb_value_callback.h"
#include "nweb_web_storage.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace OHOS::NWeb {
class NWebWebStorageDelegateInterface {
 public:
  virtual ~NWebWebStorageDelegateInterface() = default;
  virtual void DeleteAllData(bool incognito_mode) = 0;
  virtual int DeleteOrigin(const std::string& origin) = 0;
  virtual void GetOrigins(
      std::shared_ptr<NWebWebStorageOriginVectorValueCallback> callback) = 0;
  virtual void GetOrigins(
      std::vector<std::shared_ptr<NWebWebStorageOrigin>>& origins) = 0;
  virtual void GetOriginQuota(
      const std::string& origin,
      std::shared_ptr<NWebLongValueCallback> callback) = 0;
  virtual long GetOriginQuota(const std::string& origin) = 0;
  virtual void GetOriginUsage(
      const std::string& origin,
      std::shared_ptr<NWebLongValueCallback> callback) = 0;
  virtual long GetOriginUsage(const std::string& origin) = 0;

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  virtual void RegisterWebStorageExtensionCallback(
      std::shared_ptr<NWebStorageExtensionCallback>
          web_storage_extension_callback) = 0;
  virtual std::string GetPassword(const std::string& url,
                                  const std::string& username,
                                  int callback_id) = 0;
  virtual void GetSavedPasswordsInfo(int callback_id) = 0;
  virtual void MigratePasswordsInfo() = 0;
  virtual void ClearPassword() = 0;
  virtual void RemovePassword(const std::string& url,
                              const std::string& username) = 0;
  virtual void RemovePasswordByUrl(const std::string& url) = 0;
  virtual void ModifyPassword(const std::string& url,
                              const std::string& old_username,
                              const std::string& new_username,
                              const std::string& new_password) = 0;
#endif
};
}  // namespace OHOS::NWeb
#endif
