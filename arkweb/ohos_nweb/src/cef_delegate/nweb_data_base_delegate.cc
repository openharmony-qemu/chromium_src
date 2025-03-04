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
#include "nweb_data_base_delegate.h"

#include <vector>

#include "base/logging.h"
#include "url/gurl.h"

namespace OHOS::NWeb {

CefRefPtr<CefDataBase> NWebDataBaseDelegate::GetGlobalCefDataBase() {
  if (data_base_ == nullptr) {
    data_base_ = CefDataBase::GetGlobalDataBase();
  }
  return data_base_;
}

#if BUILDFLAG(IS_OHOS)
CefRefPtr<CefDataBase> NWebDataBaseDelegate::GetGlobalIncognitoCefDataBase() {
  if (incognito_data_base_ == nullptr) {
    incognito_data_base_ = CefDataBase::GetGlobalIncognitoDataBase();
  }
  return incognito_data_base_;
}
#endif

bool NWebDataBaseDelegate::ExistHttpAuthCredentials() {
  CefRefPtr<CefDataBase> data_base = GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return false;
  }
  return data_base->ExistHttpAuthCredentials();
}

void NWebDataBaseDelegate::DeleteHttpAuthCredentials() {
  CefRefPtr<CefDataBase> data_base = GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return;
  }
  data_base->DeleteHttpAuthCredentials();
}

void NWebDataBaseDelegate::SaveHttpAuthCredentials(const std::string& host,
                                                   const std::string& realm,
                                                   const std::string& username,
                                                   const char* password) {
  CefRefPtr<CefDataBase> data_base = GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return;
  }
  data_base->SaveHttpAuthCredentials(host, realm, username, password);
}

void NWebDataBaseDelegate::GetHttpAuthCredentials(const std::string& host,
                                                  const std::string& realm,
                                                  std::string& username,
                                                  char* password,
                                                  uint32_t passwordSize) {
  CefRefPtr<CefDataBase> data_base = GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return;
  }

  CefString usernameCef;
  data_base->GetHttpAuthCredentials(host, realm, usernameCef, password,
                                    passwordSize);
  username = usernameCef;
}

bool NWebDataBaseDelegate::ExistPermissionByOrigin(const std::string& origin,
                                                   int type,
                                                   bool incognito) {
  CefRefPtr<CefDataBase> data_base =
      incognito ? GetGlobalIncognitoCefDataBase() : GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return false;
  }
  return data_base->ExistPermissionByOrigin(origin, type);
}

bool NWebDataBaseDelegate::GetPermissionResultByOrigin(
    const std::string& origin,
    int type,
    bool& result,
    bool incognito) {
  CefRefPtr<CefDataBase> data_base =
      incognito ? GetGlobalIncognitoCefDataBase() : GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return false;
  }
  return data_base->GetPermissionResultByOrigin(origin, type, result);
}

int NWebDataBaseDelegate::SetPermissionByOrigin(const std::string& origin,
                                                int type,
                                                bool result,
                                                bool incognito) {
  GURL gurl = GURL(origin);
  if (gurl.is_empty() || !gurl.is_valid()) {
    return NWEB_INVALID_ORIGIN;
  }
  CefRefPtr<CefDataBase> data_base =
      incognito ? GetGlobalIncognitoCefDataBase() : GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return NWEB_ERR;
  }
  data_base->SetPermissionByOrigin(origin, type, result);
  return NWEB_OK;
}

int NWebDataBaseDelegate::ClearPermissionByOrigin(const std::string& origin,
                                                  int type,
                                                  bool incognito) {
  GURL gurl = GURL(origin);
  if (gurl.is_empty() || !gurl.is_valid()) {
    return NWEB_INVALID_ORIGIN;
  }
  CefRefPtr<CefDataBase> data_base =
      incognito ? GetGlobalIncognitoCefDataBase() : GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return NWEB_ERR;
  }
  data_base->ClearPermissionByOrigin(origin, type);
  return NWEB_OK;
}

void NWebDataBaseDelegate::ClearAllPermission(int type, bool incognito) {
  CefRefPtr<CefDataBase> data_base =
      incognito ? GetGlobalIncognitoCefDataBase() : GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return;
  }
  data_base->ClearAllPermission(type);
}

std::vector<std::string> NWebDataBaseDelegate::GetOriginsByPermission(
    int type,
    bool incognito) {
  CefRefPtr<CefDataBase> data_base =
      incognito ? GetGlobalIncognitoCefDataBase() : GetGlobalCefDataBase();
  if (data_base == nullptr) {
    return {};
  }

  std::vector<CefString> method_vector;
  data_base->GetOriginsByPermission(type, method_vector);

  std::vector<std::string> origins;
  for (std::string value : method_vector) {
    origins.push_back(value);
  }
  return origins;
}

bool NWebDataBaseDelegate::ExistFormData() {
  return false;
}

void NWebDataBaseDelegate::ClearFormData() {
  return;
}

}  // namespace OHOS::NWeb
