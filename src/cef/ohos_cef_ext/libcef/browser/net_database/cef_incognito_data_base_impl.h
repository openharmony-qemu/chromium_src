// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_CEF_EXT_CEF_LIBCEF_BROWSER_NET_DATA_BASE_INCOGNITO_IMPL_H_
#define OHOS_CEF_EXT_CEF_LIBCEF_BROWSER_NET_DATA_BASE_INCOGNITO_IMPL_H_

#include "include/cef_data_base.h"
#include "libcef/browser/browser_context.h"

class CefIncognitoDataBaseImpl : public CefDataBase {
 public:
  enum CefPermissionType { GEOLOCATION_TYPE };
  CefIncognitoDataBaseImpl() = default;

  CefIncognitoDataBaseImpl(const CefIncognitoDataBaseImpl&) = delete;
  CefIncognitoDataBaseImpl& operator=(const CefIncognitoDataBaseImpl&) = delete;

  bool ExistHttpAuthCredentials() override;

  void DeleteHttpAuthCredentials() override;

  void SaveHttpAuthCredentials(const CefString& host,
                               const CefString& realm,
                               const CefString& username,
                               const char* password) override;

  void GetHttpAuthCredentials(const CefString& host,
                              const CefString& realm,
                              CefString& username,
                              char* password,
                              uint32_t passwordSize) override;

  bool ExistPermissionByOrigin(const CefString& origin, int type) override;

  bool GetPermissionResultByOrigin(const CefString& origin,
                                   int type,
                                   bool& result) override;

  void SetPermissionByOrigin(const CefString& origin,
                             int type,
                             bool result) override;

  void ClearPermissionByOrigin(const CefString& origin, int type) override;

  void ClearAllPermission(int type) override;

  void GetOriginsByPermission(int type,
                              std::vector<CefString>& origins) override;

 private:
  std::map<CefString, bool> geolocation_permission_map_;
  IMPLEMENT_REFCOUNTING(CefIncognitoDataBaseImpl);
};
#endif  // OHOS_CEF_EXT_CEF_LIBCEF_BROWSER_NET_DATA_BASE_INCOGNITO_IMPL_H_
