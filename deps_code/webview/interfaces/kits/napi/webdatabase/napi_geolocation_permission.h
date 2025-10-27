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

#ifndef NWEB_NAPI_GEOLOCATION_PERMISSION_H
#define NWEB_NAPI_GEOLOCATION_PERMISSION_H

#include <cstddef>
#include <iosfwd>
#include <string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NWeb {
class NapiGeolocationPermission {
public:
    NapiGeolocationPermission() {}

    ~NapiGeolocationPermission() = default;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value ProcessActionByType(napi_env env, napi_callback_info info, int32_t operationType);

    static napi_value JsAllowGeolocation(napi_env env, napi_callback_info info);

    static napi_value JsDeleteGeolocation(napi_env env, napi_callback_info info);

    static napi_value JsDeleteAllGeolocation(napi_env env, napi_callback_info info);

    static void GetPermissionStateComplete(napi_env env, napi_status status, void *data);

    static void GetPermissionStatePromiseComplete(napi_env env, napi_status status, void *data);

    static void ExecuteGetPermissionState(napi_env env, void *data);

    static napi_value GetPermissionStateAsync(napi_env env, napi_value *argv,
        const std::string& origin, bool incognitoMode);

    static napi_value GetPermissionStatePromise(napi_env env, napi_value *argv,
        const std::string& origin, bool incognitoMode);

    static napi_value JsGetAccessibleGeolocation(napi_env env, napi_callback_info info);

    static void GetOriginComplete(napi_env env, napi_status status, void *data);

    static void GetOriginsPromiseComplete(napi_env env, napi_status status, void *data);

    static void ExecuteGetOrigins(napi_env env, void *data);

    static napi_value GetOriginsAsync(napi_env env, napi_value *argv,
        bool incognitoMode);

    static napi_value GetOriginsPromise(napi_env env, bool incognitoMode);

    static napi_value JsGetStoredGeolocation(napi_env env, napi_callback_info info);

    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static bool GetStringPara(napi_env env, napi_value argv, std::string& outValue);

    static bool GetBooleanPara(napi_env env, napi_value argv, bool& outValue);
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_GEOLOCATION_PERMISSION_H
