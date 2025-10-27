// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#ifndef CEF_INCLUDE_CEF_PERMISSION_STATUS_QUERY_H_
#define CEF_INCLUDE_CEF_PERMISSION_STATUS_QUERY_H_
#pragma once
 
#include "include/cef_base.h"
#include "include/internal/cef_types.h"
 
///
/// Class used to query permission status.
///
/*--cef(source=library)--*/
class CefAccessQuery : public virtual CefBaseRefCounted {
 public:
  ///
  /// Get the origin that is trying to acess the resource.
  ///
  /*--cef()--*/
  virtual CefString Origin() = 0;
  ///
  /// Get the resource that the origin is trying to acess.
  ///
  /*--cef()--*/
  virtual int ResourceAcessId() = 0;
  ///
  /// Report the permission status of resource.
  ///
  /*--cef()--*/
  virtual void ReportQueryResult(int32_t status) = 0;
};
 
///
/// Implement this interface to handle permission status query.
///
/*--cef(source=library)--*/
class CefPermissionQuery : public virtual CefBaseRefCounted {
 public:
  ///
  /// Get permission status.
  ///
  static void GetPermissionStatusAsync(CefRefPtr<CefAccessQuery>);
};
 
 
#endif // CEF_INCLUDE_CEF_PERMISSION_STATUS_QUERY_H_