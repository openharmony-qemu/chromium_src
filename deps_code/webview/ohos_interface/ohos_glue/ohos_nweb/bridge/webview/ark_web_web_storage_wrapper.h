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

#ifndef ARK_WEB_WEB_STORAGE_WRAPPER_H_
#define ARK_WEB_WEB_STORAGE_WRAPPER_H_
#pragma once

#include "include/nweb_web_storage.h"
#include "ohos_nweb/include/ark_web_web_storage.h"

namespace OHOS::ArkWeb {

class ArkWebWebStorageWrapper : public OHOS::NWeb::NWebWebStorage {
public:
    ArkWebWebStorageWrapper(ArkWebRefPtr<ArkWebWebStorage> ark_web_web_storage);
    ~ArkWebWebStorageWrapper() = default;

    std::vector<std::shared_ptr<OHOS::NWeb::NWebWebStorageOrigin>> GetOrigins() override;

    void GetOrigins(std::shared_ptr<OHOS::NWeb::NWebWebStorageOriginVectorValueCallback> callback) override;

    long GetOriginQuota(const std::string& origin) override;

    void GetOriginQuota(
        const std::string& origin, std::shared_ptr<OHOS::NWeb::NWebLongValueCallback> callback) override;

    long GetOriginUsage(const std::string& origin) override;

    void GetOriginUsage(
        const std::string& origin, std::shared_ptr<OHOS::NWeb::NWebLongValueCallback> callback) override;

    int DeleteOrigin(const std::string& origin) override;

    void DeleteAllData(bool incognito_mode) override;

private:
    ArkWebRefPtr<ArkWebWebStorage> ark_web_web_storage_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_WEB_STORAGE_WRAPPER_H_
