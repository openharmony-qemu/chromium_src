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

#include "web_storage.h"
#include "web_errors.h"
#include "nweb_helper.h"
#include "nweb_web_storage.h"

using namespace OHOS::Webview;

namespace OHOS {
namespace NWeb {
constexpr int32_t INTERFACE_ERROR = -1;
constexpr int32_t INTERFACE_OK = 0;

int32_t WebStorage::CJdeleteOrigin(const std::string &origin)
{
    int32_t errCode = NWebError::NO_ERROR;
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> web_storage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (web_storage) {
        if (web_storage->DeleteOrigin(origin) == NWebError::INVALID_ORIGIN) {
            errCode = NWebError::INVALID_ORIGIN;
        }
    } else {
        errCode = NWebError::NWEB_ERROR;
    }
    return errCode;
}

void WebStorage::CJdeleteAllData(bool incognito)
{
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> web_storage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (web_storage) {
        web_storage->DeleteAllData(incognito);
    }
}

int64_t WebStorage::CjGetOriginUsageOrQuota(const std::string &origin, int32_t *errCode, bool isQuata)
{
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> web_storage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!web_storage) {
        *errCode = INTERFACE_ERROR;
        return 0;
    }
    if (isQuata) {
        auto ret = static_cast<int64_t>(web_storage->GetOriginQuota(origin));
        if (ret == INTERFACE_ERROR) {
            *errCode = INTERFACE_ERROR;
        } else if (ret == NWebError::INVALID_ORIGIN) {
            *errCode = NWebError::INVALID_ORIGIN;
        } else {
            *errCode = INTERFACE_OK;
        }
        return ret;
    } else {
        auto ret = static_cast<int64_t>(web_storage->GetOriginUsage(origin));
        if (ret == INTERFACE_ERROR) {
            *errCode = INTERFACE_ERROR;
        } else if (ret == NWebError::INVALID_ORIGIN) {
            *errCode = NWebError::INVALID_ORIGIN;
        } else {
            *errCode = INTERFACE_OK;
        }
        return ret;
    }
}

CArrWebStorageOrigin WebStorage::CjGetOrigins(int32_t *errCode)
{
    auto ret = CArrWebStorageOrigin { .cWebStorageOrigin = nullptr, .size = 0 };
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> web_storage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!web_storage) {
        *errCode = INTERFACE_ERROR;
        return ret;
    }
    std::vector<std::shared_ptr<NWebWebStorageOrigin>> origins = web_storage->GetOrigins();
    if (origins.empty()) {
        *errCode = NWebError::NO_WEBSTORAGE_ORIGIN;
        return ret;
    }
    CWebStorageOrigin* head = static_cast<CWebStorageOrigin*>(malloc(sizeof(CWebStorageOrigin) * origins.size()));
    if (head == nullptr) {
        *errCode = NWebError::NEW_OOM;
        return ret;
    }
    int32_t i = 0;
    for (auto origin : origins) {
        Webview::CWebStorageOrigin ffiOrigin;
        ffiOrigin.origin = MallocCString(origin->GetOrigin());
        ffiOrigin.quota = static_cast<uint32_t>(origin->GetQuota());
        ffiOrigin.usage = static_cast<uint32_t>(origin->GetUsage());
        head[i] = ffiOrigin;
        i++;
    }
    ret.cWebStorageOrigin = head;
    ret.size = static_cast<int64_t>(origins.size());
    return ret;
}
}
}