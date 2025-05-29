// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/permissions/contexts/geolocation_permission_context_ohos.h"

#include "base/functional/callback.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "ohos/adapter/permission_manager/permission_manager_adapter.h"

namespace permissions {

GeolocationPermissionContextOHOS::GeolocationPermissionContextOHOS(
    content::BrowserContext* browser_context,
    std::unique_ptr<Delegate> delegate)
    : GeolocationPermissionContext(browser_context, std::move(delegate)) {}

GeolocationPermissionContextOHOS::~GeolocationPermissionContextOHOS() = default;

void GeolocationPermissionContextOHOS::RequestPermission(
    PermissionRequestData request_data,
    BrowserPermissionCallback callback) {
  namespace ohos_permission = ohos::adapter::permission;
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock()},
      base::BindOnce(
          &ohos_permission::PermissionManagerAdapter::RequestPermission,
          ohos_permission::OHOSPermissionType::LOCATION),
      base::BindOnce(&GeolocationPermissionContextOHOS::RequestReply,
                     weak_factory_.GetWeakPtr(), std::move(request_data),
                     std::move(callback)));
}

void GeolocationPermissionContextOHOS::RequestReply(
    PermissionRequestData request_data,
    BrowserPermissionCallback callback,
    bool reply_success) {
  if (!reply_success) {
    std::move(callback).Run(CONTENT_SETTING_ASK);
    return;
  }

  GeolocationPermissionContext::RequestPermission(std::move(request_data),
                                                  std::move(callback));
}

}  // namespace permissions
