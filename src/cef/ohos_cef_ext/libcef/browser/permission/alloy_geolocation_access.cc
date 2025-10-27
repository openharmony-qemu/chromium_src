// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/permission/alloy_geolocation_access.h"

#include "base/logging.h"
#include "cef/ohos_cef_ext/libcef/browser/net_database/cef_data_base_impl.h"

bool AlloyGeolocationAccess::ContainOrigin(const CefString& origin) {
  auto dataBase = CefDataBase::GetGlobalDataBase();
  if (dataBase == nullptr) {
    return false;
  }
  return dataBase->ExistPermissionByOrigin(origin,
                                           CefDataBaseImpl::GEOLOCATION_TYPE);
}

bool AlloyGeolocationAccess::IsOriginAccessEnabled(const CefString& origin) {
  auto dataBase = CefDataBase::GetGlobalDataBase();
  if (dataBase == nullptr) {
    return false;
  }
  bool result = false;
  dataBase->GetPermissionResultByOrigin(
      origin, CefDataBaseImpl::GEOLOCATION_TYPE, result);
  return result;
}

void AlloyGeolocationAccess::Enabled(const CefString& origin, bool incognito) {
  auto dataBase = incognito ? CefDataBase::GetGlobalIncognitoDataBase()
                            : CefDataBase::GetGlobalDataBase();
  if (dataBase == nullptr) {
    return;
  }
  dataBase->SetPermissionByOrigin(origin, CefDataBaseImpl::GEOLOCATION_TYPE,
                                  true);
}

void AlloyGeolocationAccess::Disabled(const CefString& origin, bool incognito) {
  auto dataBase = incognito ? CefDataBase::GetGlobalIncognitoDataBase()
                            : CefDataBase::GetGlobalDataBase();
  if (dataBase == nullptr) {
    return;
  }
  dataBase->SetPermissionByOrigin(origin, CefDataBaseImpl::GEOLOCATION_TYPE,
                                  false);
}

std::string AlloyGeolocationAccess::GetOriginKey(std::string& origin) {
  return pref_prefix_ + origin;
}
