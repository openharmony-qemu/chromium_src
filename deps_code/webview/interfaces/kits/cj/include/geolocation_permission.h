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

#ifndef GEOLOCATION_PERMISSION_H
#define GEOLOCATION_PERMISSION_H

#include <cstddef>
#include <iosfwd>
#include <string>

namespace OHOS {
namespace NWeb {
class GeolocationPermission {
public:
    GeolocationPermission() {}

    ~GeolocationPermission() = default;

    static void CjAllowGeolocation(std::string origin, bool incognitoMode, int32_t *errCode);

    static void CjDeleteGeolocation(std::string origin, bool incognitoMode, int32_t *errCode);

    static bool CjGetAccessibleGeolocation(std::string origin, bool incognitoMode, int32_t *errCode);

    static std::vector<std::string> CjGetStoredGeolocation(bool incognitoMode, int32_t *errCode);

    static void CjDeleteAllGeolocation(bool incognitoMode, int32_t *errCode);

private:
    static void ProcessActionByType(std::string origin, bool incognitoMode,
        int32_t operationType, int32_t *errCode);

    static bool ExecuteGetPermissionState(std::string origin, bool incognitoMode, int32_t *errCode);

    static std::vector<std::string> ExecuteGetOrigins(bool incognitoMode, int32_t *errCode);
};
} // namespace NWeb
} // namespace OHOS

#endif // GEOLOCATION_PERMISSION_H
