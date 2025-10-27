/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "geolocation_permission_ffi.h"

#include "geolocation_permission.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_errors.h"
#include "webview_utils.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
extern "C" {
// GeolocationPermissions
void FfiOHOSGeolocationAllowGeolocation(char* origin, bool incognito, int32_t* errCode)
{
    std::string originStr = std::string(origin);
    GeolocationPermission::CjAllowGeolocation(originStr, incognito, errCode);
}

void FfiOHOSGeolocationDeleteGeolocation(char* origin, bool incognito, int32_t* errCode)
{
    std::string originStr = std::string(origin);
    GeolocationPermission::CjDeleteGeolocation(originStr, incognito, errCode);
}

bool FfiOHOSGeolocationGetAccessibleGeolocation(char* origin, bool incognito, int32_t* errCode)
{
    std::string originStr = std::string(origin);
    return GeolocationPermission::CjGetAccessibleGeolocation(originStr, incognito, errCode);
}

CArrString FfiOHOSGeolocationGetStoredGeolocation(bool incognito, int32_t* errCode)
{
    std::vector<std::string> origins = GeolocationPermission::CjGetStoredGeolocation(incognito, errCode);
    CArrString arrOrigins = { .head = nullptr, .size = 0 };
    if (errCode && *errCode != 0) {
        return arrOrigins;
    }
    arrOrigins.size = static_cast<int64_t>(origins.size());
    arrOrigins.head = OHOS::Webview::VectorToCArrString(origins);
    return arrOrigins;
}

void FfiOHOSGeolocationDeleteAllGeolocation(bool incognito, int32_t* errCode)
{
    GeolocationPermission::CjDeleteAllGeolocation(incognito, errCode);
}
}
} // namespace Webview
} // namespace OHOS