// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ALLOY_GEOLOCATION_PERMISSIONS_H_
#define CEF_LIBCEF_BROWSER_ALLOY_GEOLOCATION_PERMISSIONS_H_

#include <string>

#include "base/memory/weak_ptr.h"
#include "include/cef_permission_request.h"

class AlloyGeolocationAccess : public CefGeolocationAcess {
 public:
  AlloyGeolocationAccess(const AlloyGeolocationAccess&) = delete;
  AlloyGeolocationAccess& operator=(const AlloyGeolocationAccess&) = delete;

  AlloyGeolocationAccess() = default;
  ~AlloyGeolocationAccess() = default;

  bool ContainOrigin(const CefString& origin) override;
  bool IsOriginAccessEnabled(const CefString& origin) override;
  void Enabled(const CefString& origin, bool incognito) override;
  void Disabled(const CefString& origin, bool incognito) override;

 private:
  std::string GetOriginKey(std::string& origin);
  const std::string pref_prefix_ = "AlloyGeolocationAccess%";

  IMPLEMENT_REFCOUNTING(AlloyGeolocationAccess);
};

#endif  // CEF_LIBCEF_BROWSER_ALLOY_GEOLOCATION_PERMISSIONS_H_
