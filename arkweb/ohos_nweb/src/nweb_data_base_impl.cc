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

#include "nweb_data_base_impl.h"

#include "nweb_data_base_delegate.h"
#include "nweb_hilog.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
NWebDataBaseImpl::NWebDataBaseImpl() {
#if defined(USE_CEF)
  delegate_ = std::make_shared<NWebDataBaseDelegate>();
#endif
}

bool NWebDataBaseImpl::ExistHttpAuthCredentials() {
  if (delegate_ != nullptr) {
    return delegate_->ExistHttpAuthCredentials();
  }
  return false;
}

void NWebDataBaseImpl::DeleteHttpAuthCredentials() {
  if (delegate_ != nullptr) {
    delegate_->DeleteHttpAuthCredentials();
  }
}

void NWebDataBaseImpl::SaveHttpAuthCredentials(const std::string& host,
                                               const std::string& realm,
                                               const std::string& username,
                                               const char* password) {
  if (delegate_ != nullptr) {
    delegate_->SaveHttpAuthCredentials(host, realm, username, password);
  }
}

void NWebDataBaseImpl::GetHttpAuthCredentials(const std::string& host,
                                              const std::string& realm,
                                              std::string& username,
                                              char* password,
                                              uint32_t passwordSize) {
  if (delegate_ != nullptr) {
    delegate_->GetHttpAuthCredentials(host, realm, username, password,
                                      passwordSize);
  }
}

bool NWebDataBaseImpl::ExistPermissionByOrigin(const std::string& origin,
                                               int type,
                                               bool incognito) {
  if (delegate_ != nullptr) {
    return delegate_->ExistPermissionByOrigin(origin, type, incognito);
  }
  return false;
}

bool NWebDataBaseImpl::GetPermissionResultByOrigin(const std::string& origin,
                                                   int type,
                                                   bool& result,
                                                   bool incognito) {
  if (delegate_ != nullptr) {
    return delegate_->GetPermissionResultByOrigin(origin, type, result,
                                                  incognito);
  }
  return false;
}

int NWebDataBaseImpl::SetPermissionByOrigin(const std::string& origin,
                                            int type,
                                            bool result,
                                            bool incognito) {
  if (delegate_ != nullptr) {
    return delegate_->SetPermissionByOrigin(origin, type, result, incognito);
  }
  return NWEB_ERR;
}

int NWebDataBaseImpl::ClearPermissionByOrigin(const std::string& origin,
                                              int type,
                                              bool incognito) {
  if (delegate_ != nullptr) {
    return delegate_->ClearPermissionByOrigin(origin, type, incognito);
  }
  return NWEB_ERR;
}

void NWebDataBaseImpl::ClearAllPermission(int type, bool incognito) {
  if (delegate_ != nullptr) {
    delegate_->ClearAllPermission(type, incognito);
  }
}

std::vector<std::string> NWebDataBaseImpl::GetOriginsByPermission(
    int type,
    bool incognito) {
  if (delegate_ != nullptr) {
    return delegate_->GetOriginsByPermission(type, incognito);
  }
  return {};
}

bool NWebDataBaseImpl::ExistFormData() const {
  if (delegate_ != nullptr) {
    return delegate_->ExistFormData();
  }
  return false;
}

void NWebDataBaseImpl::ClearFormData() {
  if (delegate_ != nullptr) {
    return delegate_->ClearFormData();
  }
  return;
}

}  // namespace OHOS::NWeb
