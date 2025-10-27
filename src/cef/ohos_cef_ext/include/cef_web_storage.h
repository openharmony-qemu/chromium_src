// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_INCLUDE_CEF_STORAGE_H_
#define CEF_INCLUDE_CEF_STORAGE_H_
#pragma once

#include <vector>

#include "include/cef_base.h"
#include "include/cef_callback.h"

class CefGetOriginsCallback;

///
/// Interface to implement to be notified of asynchronous completion via
/// CefWebStorage::GetSavedPasswords.
///
/*--cef(source=client)--*/
class CefGetSavedPasswordsCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion.
  ///
  /*--cef()--*/
  virtual void OnComplete(const std::vector<CefString>& url,
                          const std::vector<CefString>& username) = 0;
};

///
/// Interface to implement to be notified of asynchronous completion via
/// CefWebStorage::GetOriginQuota.
///
/*--cef(source=client)--*/
class CefGetOriginUsageOrQuotaCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion.
  ///
  /*--cef()--*/
  virtual void OnComplete(int64_t nums) = 0;
};

///
/// Interface to implement to be notified of asynchronous completion via
/// CefWebStorage::GetOrigins().
///
/*--cef(source=client)--*/
class CefGetOriginsCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon Origins completion.
  ///
  /*--cef()--*/
  virtual void OnOrigins(std::vector<CefString>& origins) = 0;

  ///
  /// Method that will be called upon Usages completion.
  ///
  /*--cef()--*/
  virtual void OnUsages(std::vector<CefString>& usages) = 0;

  ///
  /// Method that will be called upon Quotas completion.
  ///
  /*--cef()--*/
  virtual void OnQuotas(std::vector<CefString>& quotas) = 0;

  ///
  /// Method that will be called upon completion.
  ///
  /*--cef()--*/
  virtual void OnComplete() = 0;
};

///
/// Interface to implement to be notified of asynchronous completion via
/// CefWebStorage::GetPassword.
///
/*--cef(source=client)--*/
class CefGetPasswordCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion.
  ///
  /*--cef()--*/
  virtual void OnComplete(const CefString& result) = 0;
};

///
/// Class used for managing storage. The methods of this class may be called on
/// any thread unless otherwise indicated.
///
/*--cef(source=library,no_debugct_check)--*/
class CefWebStorage : public virtual CefBaseRefCounted {
 public:
  ///
  /// Returns the global web storage.
  ///
  /*--cef(optional_param=callback)--*/
  static CefRefPtr<CefWebStorage> GetGlobalManager(
      CefRefPtr<CefCompletionCallback> callback);

  ///
  /// Returns the global web storage in incognito mode.
  ///
  /*--cef(optional_param=callback)--*/
  static CefRefPtr<CefWebStorage> GetGlobalIncognitoManager(
      CefRefPtr<CefCompletionCallback> callback);

  ///
  /// Clears all storage currently being used by the JavaScript storage APIs.
  ///
  /*--cef()--*/
  virtual void DeleteAllData() = 0;

  ///
  /// Clears the storage currently being used by the Web SQL Database APIs by
  /// the given origin.
  ///
  /*--cef()--*/
  virtual void DeleteOrigin(const CefString& origin) = 0;

  ///
  /// Gets the origins currently using the Web SQL Database APIs.
  ///
  /*--cef(optional_param=callback)--*/
  virtual void GetOrigins(CefRefPtr<CefGetOriginsCallback> callback) = 0;

  ///
  /// Gets the storage quota for the Web SQL Database API for the given origin.
  ///
  /*--cef(optional_param=origin,optional_param=callback)--*/
  virtual void GetOriginQuota(
      const CefString& origin,
      CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback) = 0;

  ///
  /// Gets the amount of storage currently being used by the Web SQL Database
  /// APIs by the given origin.
  ///
  /*--cef(optional_param=origin,optional_param=callback)--*/
  virtual void GetOriginUsage(
      const CefString& origin,
      CefRefPtr<CefGetOriginUsageOrQuotaCallback> callback) = 0;

  ///
  /// clear password
  ///
  /*--cef()--*/
  virtual void ClearPassword() = 0;

  ///
  /// remove save pssword by url and username
  ///
  /*--cef()--*/
  virtual void RemovePassword(const CefString& url,
                              const CefString& username) = 0;

  ///
  /// modify save pssword by url
  ///
  /*--cef()--*/
  virtual void ModifyPassword(const CefString& url,
                              const CefString& old_username,
                              const CefString& new_username,
                              const CefString& new_password) = 0;

  ///
  /// remove password by host url
  ///
  /*--cef()--*/
  virtual void RemovePasswordByUrl(const CefString& url) = 0;

  ///
  /// Get password by url and username
  ///
  /*--cef(optional_param=url,optional_param=username,optional_param=callback)--*/
  virtual void GetPassword(const CefString& url,
                           const CefString& username,
                           CefRefPtr<CefGetPasswordCallback> callback) = 0;
  ///
  /// Get password by url and username
  ///
  /*--cef(optional_param=url,optional_param=username,optional_param=callback)--*/
  virtual void GetSavedPasswordsInfo(
      CefRefPtr<CefGetSavedPasswordsCallback> callback) = 0;

  virtual void MigratePasswordsInfo() = 0;
};

#endif  // CEF_INCLUDE_CEF_STORAGE_H_
