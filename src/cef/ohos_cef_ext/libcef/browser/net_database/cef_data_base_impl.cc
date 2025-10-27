// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/net_database/cef_data_base_impl.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

void TransferVector(const std::vector<std::string>& source,
                    std::vector<CefString>& target) {
  if (!target.empty()) {
    target.clear();
  }

  if (!source.empty()) {
    std::vector<std::string>::const_iterator it = source.begin();
    for (; it != source.end(); ++it) {
      target.push_back(*it);
    }
  }
}

CefRefPtr<CefDataBase> CefDataBase::GetGlobalDataBase() {
  CefRefPtr<CefRequestContext> context = CefRequestContext::GetGlobalContext();
  return context ? context->GetDataBase() : nullptr;
}

bool CefDataBaseImpl::ExistHttpAuthCredentials() {
  OHOS::NWeb::OhosWebDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetOhosWebDataBaseAdapterInstance();
  return databaseAdapter.ExistHttpAuthCredentials();
}

void CefDataBaseImpl::DeleteHttpAuthCredentials() {
  OHOS::NWeb::OhosWebDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetOhosWebDataBaseAdapterInstance();
  return databaseAdapter.DeleteHttpAuthCredentials();
}

void CefDataBaseImpl::SaveHttpAuthCredentials(const CefString& host,
                                              const CefString& realm,
                                              const CefString& username,
                                              const char* password) {
  if (host.empty() || username.empty() || password == nullptr) {
    return;
  }
  OHOS::NWeb::OhosWebDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetOhosWebDataBaseAdapterInstance();
  databaseAdapter.SaveHttpAuthCredentials(host, realm, username, password);
}

void CefDataBaseImpl::GetHttpAuthCredentials(const CefString& host,
                                             const CefString& realm,
#if BUILDFLAG(IS_OHOS)
                                             CefString& username,
                                             char* password,
                                             uint32_t passwordSize) {
#else
                                             std::vector<CefString>&
                                                 username_password) {
#endif  // BUILDFLAG(IS_OHOS)
  if (host.empty()) {
    return;
  }

  std::vector<std::string> result;
  std::string usernameStr;
  OHOS::NWeb::OhosWebDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetOhosWebDataBaseAdapterInstance();
#if BUILDFLAG(IS_OHOS)
  databaseAdapter.GetHttpAuthCredentials(host, realm, usernameStr, password,
                                         passwordSize);
  username = usernameStr;
#else
  databaseAdapter.GetHttpAuthCredentials(host, realm, usernameStr, result);
  TransferVector(result, username_password);
#endif
}

static OHOS::NWeb::WebPermissionType TypeToWebPermissionType(int type) {
  if (type == CefDataBaseImpl::CefPermissionType::GEOLOCATION_TYPE) {
    return OHOS::NWeb::WebPermissionType::GEOLOCATION;
  }
  return OHOS::NWeb::WebPermissionType::NONE_TYPE;
}

bool CefDataBaseImpl::ExistPermissionByOrigin(const CefString& origin,
                                              int type) {
  if (origin.empty()) {
    return false;
  }
  OHOS::NWeb::WebPermissionType key = TypeToWebPermissionType(type);
  if (key == OHOS::NWeb::WebPermissionType::NONE_TYPE) {
    return false;
  }
  OHOS::NWeb::OhosWebPermissionDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWebPermissionDataBaseInstance();
  return databaseAdapter.ExistPermissionByOrigin(origin, key);
}

bool CefDataBaseImpl::GetPermissionResultByOrigin(const CefString& origin,
                                                  int type,
                                                  bool& result) {
  if (origin.empty()) {
    return false;
  }
  OHOS::NWeb::WebPermissionType key = TypeToWebPermissionType(type);
  if (key == OHOS::NWeb::WebPermissionType::NONE_TYPE) {
    return false;
  }
  OHOS::NWeb::OhosWebPermissionDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWebPermissionDataBaseInstance();
  return databaseAdapter.GetPermissionResultByOrigin(origin, key, result);
}

void CefDataBaseImpl::SetPermissionByOrigin(const CefString& origin,
                                            int type,
                                            bool result) {
  if (origin.empty()) {
    return;
  }
  OHOS::NWeb::WebPermissionType key = TypeToWebPermissionType(type);
  if (key == OHOS::NWeb::WebPermissionType::NONE_TYPE) {
    return;
  }
  OHOS::NWeb::OhosWebPermissionDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWebPermissionDataBaseInstance();
  databaseAdapter.SetPermissionByOrigin(origin, key, result);
}

void CefDataBaseImpl::ClearPermissionByOrigin(const CefString& origin,
                                              int type) {
  if (origin.empty()) {
    return;
  }
  OHOS::NWeb::WebPermissionType key = TypeToWebPermissionType(type);
  if (key == OHOS::NWeb::WebPermissionType::NONE_TYPE) {
    return;
  }
  OHOS::NWeb::OhosWebPermissionDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWebPermissionDataBaseInstance();
  databaseAdapter.ClearPermissionByOrigin(origin, key);
}

void CefDataBaseImpl::ClearAllPermission(int type) {
  OHOS::NWeb::WebPermissionType key = TypeToWebPermissionType(type);
  if (key == OHOS::NWeb::WebPermissionType::NONE_TYPE) {
    return;
  }
  OHOS::NWeb::OhosWebPermissionDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWebPermissionDataBaseInstance();
  databaseAdapter.ClearAllPermission(key);
}

void CefDataBaseImpl::GetOriginsByPermission(int type,
                                             std::vector<CefString>& origins) {
  OHOS::NWeb::WebPermissionType key = TypeToWebPermissionType(type);
  if (key == OHOS::NWeb::WebPermissionType::NONE_TYPE) {
    return;
  }
  OHOS::NWeb::OhosWebPermissionDataBaseAdapter& databaseAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .GetWebPermissionDataBaseInstance();
  std::vector<std::string> result;
  databaseAdapter.GetOriginsByPermission(key, result);
  TransferVector(result, origins);
}
