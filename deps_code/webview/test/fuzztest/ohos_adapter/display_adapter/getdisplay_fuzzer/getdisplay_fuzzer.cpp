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

#include "getdisplay_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "display_manager_adapter_impl.h"

using namespace OHOS::NWeb;
using namespace OHOS::Rosen;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

bool GetDisplayFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    DisplayManagerAdapterImpl display;
    display.GetDefaultDisplay();
    display.IsDefaultPortrait();
    std::shared_ptr<FoldStatusListenerAdapter> listener
        = std::make_shared<FoldStatusListenerAdapter>();
    display.RegisterFoldStatusListener(listener);
    uint32_t id = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_SET_NUMBER);
    display.UnregisterFoldStatusListener(id);
    display.GetPrimaryDisplay();
    display.GetAllDisplays();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GetDisplayFuzzTest(data, size);
    return 0;
}
