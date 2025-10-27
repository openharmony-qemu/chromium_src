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

#include "convertorientation_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#define private public
#include "display_manager_adapter_impl.h"

using namespace OHOS::NWeb;
using namespace OHOS::Rosen;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

bool ConvertOrientationFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    sptr<Display> listener = nullptr;
    DisplayAdapterImpl display(listener);
    FuzzedDataProvider dataProvider(data, size);
    int32_t Adapter = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    display.ConvertOrientationType(static_cast<Orientation>(Adapter));
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ConvertOrientationFuzzTest(data, size);
    return 0;
}
