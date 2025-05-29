// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PERMISSIONS_CONTEXTS_GEOLOCATION_PERMISSION_CONTEXT_OHOS_H_
#define COMPONENTS_PERMISSIONS_CONTEXTS_GEOLOCATION_PERMISSION_CONTEXT_OHOS_H_

#include "base/memory/weak_ptr.h"
#include "components/permissions/contexts/geolocation_permission_context.h"
#include "components/permissions/permission_request_id.h"

namespace permissions {

class GeolocationPermissionContextOHOS : public GeolocationPermissionContext {
 public:
  GeolocationPermissionContextOHOS(content::BrowserContext* browser_context,
                                   std::unique_ptr<Delegate> delegate);

  GeolocationPermissionContextOHOS(const GeolocationPermissionContextOHOS&) =
      delete;
  GeolocationPermissionContextOHOS& operator=(
      const GeolocationPermissionContextOHOS&) = delete;

  ~GeolocationPermissionContextOHOS() override;

 private:
  void RequestPermission(PermissionRequestData request_data,
                         BrowserPermissionCallback callback) override;

  void RequestReply(PermissionRequestData request_data,
                    BrowserPermissionCallback callback,
                    bool reply_success);
  base::WeakPtrFactory<GeolocationPermissionContextOHOS> weak_factory_{this};
};

}  // namespace permissions

#endif  // COMPONENTS_PERMISSIONS_CONTEXTS_GEOLOCATION_PERMISSION_CONTEXT_OHOS_H_
