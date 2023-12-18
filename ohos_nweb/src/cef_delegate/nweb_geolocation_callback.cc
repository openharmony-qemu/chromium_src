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

#include "ohos_nweb/src/cef_delegate/nweb_geolocation_callback.h"
#include "base/logging.h"

namespace OHOS::NWeb {
NWebGeolocationCallback::NWebGeolocationCallback(CefRefPtr<CefBrowser> browser)
    : browser_(browser) {}

void NWebGeolocationCallback::GeolocationCallbackInvoke(
    const std::string& origin,
    bool allow,
    bool retain,
    bool incognito) {
  if (!browser_) {
    LOG(ERROR) << "GeolocationCallbackInvoke browser_ is nullptr";
    return;
  }

  if (retain) {
    if (allow) {
      browser_->GetGeolocationPermissions()->Enabled(origin, incognito);
    } else {
      browser_->GetGeolocationPermissions()->Disabled(origin, incognito);
    }
  }
  browser_->GetPermissionRequestDelegate()->NotifyGeolocationPermission(allow,
                                                                        origin);
}
}  // namespace OHOS::NWeb
                          