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

#include "nweb_web_storage_impl.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "nweb_hilog.h"
#include "nweb_web_storage_delegate.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
NWebWebStorageImpl::NWebWebStorageImpl() {
#if defined(USE_CEF)
  delegate_ = std::make_shared<NWebWebStorageDelegate>();
#endif
}

void NWebWebStorageImpl::DeleteAllData(bool incognito_mode) {
  if (delegate_ != nullptr) {
    delegate_->DeleteAllData(incognito_mode);
  }
}

int NWebWebStorageImpl::DeleteOrigin(const std::string& origin) {
  if (delegate_ != nullptr) {
    return delegate_->DeleteOrigin(origin);
  }
  return NWEB_ERR;
}

void NWebWebStorageImpl::GetOrigins(
    std::shared_ptr<NWebWebStorageOriginVectorValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->GetOrigins(callback);
  }
}

std::vector<std::shared_ptr<NWebWebStorageOrigin>>
NWebWebStorageImpl::GetOrigins() {
  std::vector<std::shared_ptr<NWebWebStorageOrigin>> origins;
  if (delegate_ != nullptr) {
    delegate_->GetOrigins(origins);
  }
  return origins;
}

void NWebWebStorageImpl::GetOriginQuota(
    const std::string& origin,
    std::shared_ptr<NWebLongValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->GetOriginQuota(origin, callback);
  }
}

long NWebWebStorageImpl::GetOriginQuota(const std::string& origin) {
  if (delegate_ != nullptr) {
    return delegate_->GetOriginQuota(origin);
  }
  return NWEB_ERR;
}

void NWebWebStorageImpl::GetOriginUsage(
    const std::string& origin,
    std::shared_ptr<NWebLongValueCallback> callback) {
  if (delegate_ != nullptr) {
    delegate_->GetOriginUsage(origin, callback);
  }
}

long NWebWebStorageImpl::GetOriginUsage(const std::string& origin) {
  if (delegate_ != nullptr) {
    return delegate_->GetOriginUsage(origin);
  }
  return NWEB_ERR;
}

#if BUILDFLAG(IS_OHOS)
void NWebWebStorageImpl::PutWebStorageCallback(
    std::shared_ptr<NWebStorageExtensionCallback>
        web_storage_extension_callback) {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (delegate_ != nullptr) {
    delegate_->RegisterWebStorageExtensionCallback(
        web_storage_extension_callback);
  }
#endif  // ARKWEB_EXT_PASSWORD
}

std::string NWebWebStorageImpl::GetPassword(const std::string& url,
                                            const std::string& username,
                                            int callback_id) {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (delegate_ != nullptr) {
    return delegate_->GetPassword(url, username, callback_id);
  }
  return "";
#else
  return "";
#endif
}

void NWebWebStorageImpl::GetSavedPasswords(int callback_id) {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (delegate_ != nullptr) {
    return delegate_->GetSavedPasswordsInfo(callback_id);
  }
#endif  // ARKWEB_EXT_PASSWORD
}

void NWebWebStorageImpl::ClearPassword() {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (delegate_ != nullptr) {
    delegate_->ClearPassword();
  }
#endif  // ARKWEB_EXT_PASSWORD
}

void NWebWebStorageImpl::RemovePassword(const std::string& url,
                                        const std::string& username) {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (delegate_ != nullptr) {
    delegate_->RemovePassword(url, username);
  }
#endif  // ARKWEB_EXT_PASSWORD
}
void NWebWebStorageImpl::ModifyPassword(const std::string& url,
                                        const std::string& old_username,
                                        const std::string& new_username,
                                        const std::string& new_password) {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (delegate_ != nullptr) {
    delegate_->ModifyPassword(url, old_username, new_username, new_password);
  }
#endif  // ARKWEB_EXT_PASSWORD
}

void NWebWebStorageImpl::RemovePasswordByUrl(const std::string& url) {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (delegate_ != nullptr) {
    delegate_->RemovePasswordByUrl(url);
  }
#endif  // ARKWEB_EXT_PASSWORD
}
#endif
}  // namespace OHOS::NWeb
