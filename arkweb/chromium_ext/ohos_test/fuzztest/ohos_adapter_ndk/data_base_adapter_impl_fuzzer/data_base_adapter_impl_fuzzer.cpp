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
#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

#include "ohos_sdk/openharmony/native/sysroot/usr/include/AbilityKit/ability_runtime/context_constant.h"

#define private public
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_data_base_adapter_impl.h"

using namespace OHOS::NWeb;

class MockOhosWebDataBaseAdapterImpl : public OhosWebDataBaseAdapterImpl {
  public:
    MockOhosWebDataBaseAdapterImpl(int data, int data2) {}
};

void FuzzTest001(FuzzedDataProvider* fdp)
{
    auto ret = static_cast<AbilityRuntime_AreaMode>(fdp->ConsumeIntegralInRange(0, 4));
    OhosWebDataBaseAdapterImpl::GetInstance().GetAreaMode(ret);

    OH_Rdb_Store* store = new OH_Rdb_Store();
    store->id = fdp->ConsumeIntegral<int64_t>();
    MockOhosWebDataBaseAdapterImpl* impl = new MockOhosWebDataBaseAdapterImpl(0, 1);
    if (fdp->ConsumeBool()) {
      impl->rdbStore_ = store;
    } else {
      impl->rdbStore_ = nullptr;
    }

    auto host = fdp->ConsumeRandomLengthString(64);
    auto realm = fdp->ConsumeRandomLengthString(64);
    auto un = fdp->ConsumeRandomLengthString(64);
    auto pw = fdp->ConsumeRandomLengthString(64);
    auto pws = fdp->ConsumeIntegral<uint32_t>();
    impl->SaveHttpAuthCredentials(host, realm, un, pw.c_str());

    impl->GetHttpAuthCredentials(host, realm, un, pw.data(), pws);

    impl->ExistHttpAuthCredentials();

    impl->DeleteHttpAuthCredentials();

    delete store;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);

    FuzzTest001(&fdp);
    return 0;
}
