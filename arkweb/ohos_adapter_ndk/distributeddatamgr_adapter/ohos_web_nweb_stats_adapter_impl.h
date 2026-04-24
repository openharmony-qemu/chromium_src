/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_WEB_NWEB_STATS_ADAPTER_IMPL_H
#define OHOS_WEB_NWEB_STATS_ADAPTER_IMPL_H

#include <string>

#include <database/rdb/relational_store.h>
#include <database/rdb/relational_store_error_code.h>
#include <AbilityKit/ability_runtime/context_constant.h>
#include <AbilityKit/ability_runtime/ability_runtime_common.h>

namespace OHOS::NWeb {

class OhosWebNWebStatsAdapterImpl {
public:
    static OhosWebNWebStatsAdapterImpl& GetInstance();
    uint32_t GetMaxInstanceCount();
    void UpdateMaxInstanceCount(uint32_t count);

private:
    ~OhosWebNWebStatsAdapterImpl();
    OhosWebNWebStatsAdapterImpl();
    OhosWebNWebStatsAdapterImpl(const OhosWebNWebStatsAdapterImpl&) = delete;
    OhosWebNWebStatsAdapterImpl& operator=(const OhosWebNWebStatsAdapterImpl&) = delete;
    OhosWebNWebStatsAdapterImpl(OhosWebNWebStatsAdapterImpl&&) = delete;
    OhosWebNWebStatsAdapterImpl& operator=(OhosWebNWebStatsAdapterImpl&&) = delete;

    bool InitDatabase();
    Rdb_SecurityArea GetAreaMode(AbilityRuntime_AreaMode areaMode);
    int64_t QueryInt64Value(const std::string& key);
    void StoreInt64Value(const std::string& key, int64_t value);
    int64_t GetResetIntervalSeconds();

    OH_Rdb_Store *rdbStore_ = nullptr;
    bool dbInitialized_ = false;
};

} // namespace OHOS::NWeb

#endif // OHOS_WEB_NWEB_STATS_ADAPTER_IMPL_H
