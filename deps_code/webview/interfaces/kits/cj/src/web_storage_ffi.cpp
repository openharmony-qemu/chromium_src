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

#include "web_storage_ffi.h"

#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_storage.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
extern "C" {
// web_storage
int32_t FfiWebStorageDeleteOrigin(char* corigin)
{
    std::string origin(corigin);
    return OHOS::NWeb::WebStorage::CJdeleteOrigin(origin);
}

void FfiWebStorageDeleteAllData(bool incognito)
{
    OHOS::NWeb::WebStorage::CJdeleteAllData(incognito);
}

int64_t FfiWebStorageGetOriginQuota(char* corigin, int32_t* errCode)
{
    std::string origin(corigin);
    return OHOS::NWeb::WebStorage::CjGetOriginUsageOrQuota(origin, errCode, true);
}

int64_t FfiWebStorageGetOriginUsage(char* corigin, int32_t* errCode)
{
    std::string origin(corigin);
    return OHOS::NWeb::WebStorage::CjGetOriginUsageOrQuota(origin, errCode, false);
}

CArrWebStorageOrigin FfiWebStorageGetOrigins(int32_t* errCode)
{
    return OHOS::NWeb::WebStorage::CjGetOrigins(errCode);
}
}
} // namespace Webview
} // namespace OHOS