/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef CEF_INCLUDE_CEF_DATA_BASE_H_
#define CEF_INCLUDE_CEF_DATA_BASE_H_
#pragma once

#include <vector>

#include "include/cef_base.h"

///
/// Class used for managing web data base.
///
/*--cef(source=library,no_debugct_check)--*/
class CefDataBase : public virtual CefBaseRefCounted {
 public:
  ///
  /// Returns the global data base instance.
  ///
  /*--cef()--*/
  static CefRefPtr<CefDataBase> GetGlobalDataBase();

  ///
  /// clear all http auth data
  ///
  /*--cef()--*/
  virtual void DeleteHttpAuthCredentials() = 0;

  ///
  /// get whether there has any http auth data.
  ///
  /*--cef()--*/
  virtual bool ExistHttpAuthCredentials() = 0;

  ///
  /// save http auth data.
  ///
  /*--cef()--*/
  virtual void SaveHttpAuthCredentials(const CefString& host,
                                       const CefString& realm,
                                       const CefString& username,
                                       const char* password) = 0;

  ///
  /// get http auth data by host and realm.
  ///
  /*--cef()--*/
  virtual void GetHttpAuthCredentials(const CefString& host,
                                      const CefString& realm,
                                      CefString& username,
                                      char* password,
                                      uint32_t passwordSize) = 0;

  ///
  /// gets whether the instance holds the specified permissions for the
  /// specified source.
  ///
  /*--cef()--*/
  virtual bool ExistPermissionByOrigin(const CefString& origin, int type) = 0;

  ///
  /// get specifies permission type result by origin.
  ///
  /*--cef()--*/
  virtual bool GetPermissionResultByOrigin(const CefString& origin,
                                           int type,
                                           bool& result) = 0;

  ///
  /// Set specifies permission type by origin.
  ///
  /*--cef()--*/
  virtual void SetPermissionByOrigin(const CefString& origin,
                                     int type,
                                     bool result) = 0;

  ///
  /// Delete specifies permission type by origin.
  ///
  /*--cef()--*/
  virtual void ClearPermissionByOrigin(const CefString& origin, int type) = 0;

  ///
  /// Delete all specifies permission type.
  ///
  /*--cef()--*/
  virtual void ClearAllPermission(int type) = 0;

  ///
  /// Obtains all origins of a specified permission type.
  ///
  /*--cef()--*/
  virtual void GetOriginsByPermission(int type,
                                      std::vector<CefString>& origins) = 0;

  static CefRefPtr<CefDataBase> GetGlobalIncognitoDataBase();
};

#endif  // CEF_INCLUDE_CEF_DATA_BASE_H_
