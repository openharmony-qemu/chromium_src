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

#ifndef NWEB_COOKIE_MANAGER_H
#define NWEB_COOKIE_MANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "nweb_export.h"
#include "nweb_value_callback.h"

namespace OHOS::NWeb {
class NWebWebStorageOrigin;
using NWebGetOriginsCallback = NWebValueCallback<std::vector<NWebWebStorageOrigin>>;
class OHOS_NWEB_EXPORT NWebWebStorage {
public:
    NWebWebStorage() = default;

    virtual ~NWebWebStorage() = default;

    virtual void DeleteAllData() = 0;
    virtual int DeleteOrigin(const std::string& origin) = 0;
    virtual void GetOrigins(std::shared_ptr<NWebGetOriginsCallback> callback) = 0;
    virtual std::vector<NWebWebStorageOrigin> GetOrigins() = 0;
    virtual void GetOriginQuota(const std::string& origin,
                              std::shared_ptr<NWebValueCallback<long>> callback) = 0;
    virtual long GetOriginQuota(const std::string& origin) = 0;
    virtual void GetOriginUsage(const std::string& origin,
                              std::shared_ptr<NWebValueCallback<long>> callback) = 0;
    virtual long GetOriginUsage(const std::string& origin) = 0;
};

class OHOS_NWEB_EXPORT NWebWebStorageOrigin {
public:
    NWebWebStorageOrigin() = default;
    ~NWebWebStorageOrigin() = default;
    void SetOrigin(const std::string& origin) { origin_ = origin; }
    void SetQuota(long quota) { quota_ = quota; }
    void SetUsage(long usage) { usage_ = usage; }
    std::string GetOrigin() { return origin_; }
    long GetQuota() { return quota_; }
    long GetUsage() { return usage_;}

private:
    std::string origin_;
    int64_t quota_;
    int64_t usage_;
};
}

#endif  // NWebWebStorage
