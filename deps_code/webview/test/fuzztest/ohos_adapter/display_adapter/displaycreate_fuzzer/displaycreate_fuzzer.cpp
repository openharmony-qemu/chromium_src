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
#define private public
#include "displaycreate_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "display_manager_adapter_impl.h"
#include "display_info.h"

using namespace OHOS::NWeb;
using namespace OHOS::Rosen;

namespace OHOS {
constexpr DisplayId MAX_DISPLAY_ID = 1000;
bool DisplayCreateFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::shared_ptr<DisplayListenerAdapter> listener = nullptr;
    DisplayListenerAdapterImpl display(listener);
    FuzzedDataProvider fuzzedData(data, size);
    DisplayId randmoId = fuzzedData.ConsumeIntegralInRange<DisplayId>(0, MAX_DISPLAY_ID);
    display.OnCreate(randmoId);
    return true;
}

void DisplayInstanceFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider fuzzedData(data, size);
    std::shared_ptr<DisplayListenerAdapter> listener = nullptr;
    DisplayListenerAdapterImpl display(listener);
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    if (displayPtr == nullptr) {
        return;
    }
    auto displayInfo = displayPtr->GetDisplayInfo();
    display.ConvertDisplayInfo(*displayInfo);

    std::shared_ptr<FoldStatusListenerAdapter> foldStatusListenerAdapter = 
        std::make_shared<FoldStatusListenerAdapter>();
    FoldStatusListenerAdapterImpl foldStatus(foldStatusListenerAdapter);
    int mode =  fuzzedData.ConsumeIntegralInRange<int>(0, 5);
    foldStatus.OnFoldStatusChanged(static_cast<NativeDisplayManager_FoldDisplayMode>(mode));
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DisplayCreateFuzzTest(data, size);
    OHOS::DisplayInstanceFuzzTest(data, size);
    return 0;
}
