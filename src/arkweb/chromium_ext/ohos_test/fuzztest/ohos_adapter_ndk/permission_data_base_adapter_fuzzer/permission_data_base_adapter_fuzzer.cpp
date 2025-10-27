/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <vector>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/AbilityKit/ability_runtime/context_constant.h"

#define private public
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_permission_data_base_adapter_impl.h"

using namespace OHOS::NWeb;

class MockOhosWebPermissionDataBaseAdapterImpl : public OhosWebPermissionDataBaseAdapterImpl{
public:
    MockOhosWebPermissionDataBaseAdapterImpl(int data, int data2) {}
};

void FuzzTest(FuzzedDataProvider* fdp)
{
    OH_Rdb_Store* store = new OH_Rdb_Store;
    store->id = fdp->ConsumeIntegral<int64_t>();
    MockOhosWebPermissionDataBaseAdapterImpl* impl = new MockOhosWebPermissionDataBaseAdapterImpl(1, 2);
    if (fdp->ConsumeBool()) {
      impl->rdbStore_ = store;
    } else {
      impl->rdbStore_ = nullptr;
    }

    auto origin = fdp->ConsumeRandomLengthString(64);
    auto key = static_cast<WebPermissionType>(fdp->ConsumeIntegralInRange<int32_t>(0, 1));
    impl->ExistPermissionByOrigin(origin, key);

    origin = fdp->ConsumeRandomLengthString(64);
    key = static_cast<WebPermissionType>(fdp->ConsumeIntegralInRange<int32_t>(0, 1));
    auto result = fdp->ConsumeBool();
    impl->GetPermissionResultByOrigin(origin, key, result);

    origin = fdp->ConsumeRandomLengthString(64);
    key = static_cast<WebPermissionType>(fdp->ConsumeIntegralInRange<int32_t>(0, 1));
    result = fdp->ConsumeBool();
    impl->SetPermissionByOrigin(origin, key, result);
    
    origin = fdp->ConsumeRandomLengthString(64);
    key = static_cast<WebPermissionType>(fdp->ConsumeIntegralInRange<int32_t>(0, 1));
    impl->ClearPermissionByOrigin(origin, key);

    key = static_cast<WebPermissionType>(fdp->ConsumeIntegralInRange<int32_t>(0, 1));
    impl->ClearAllPermission(key);

    key = static_cast<WebPermissionType>(fdp->ConsumeIntegralInRange<int32_t>(0, 1));
    std::vector<std::string> origins;
    impl->GetOriginsByPermission(key, origins);

    auto mode = static_cast<AbilityRuntime_AreaMode>(fdp->ConsumeIntegralInRange<int>(0, 4));
    impl->GetAreaMode(mode);

    origin = fdp->ConsumeRandomLengthString(64);
    key = static_cast<WebPermissionType>(fdp->ConsumeIntegralInRange<int32_t>(0, 1));
    OhosWebPermissionDataBaseAdapterImpl::GetInstance().ExistPermissionByOrigin(origin, key);

    delete store;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
      return 0;
    }

    FuzzedDataProvider fdp(data, size);

    FuzzTest(&fdp);
    return 0;
}
