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

#include "convertrotation_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#define private public
#include "display_manager_adapter_impl.h"

using namespace OHOS::NWeb;
using namespace OHOS::Rosen;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 10;

bool ConvertRotationFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    DisplayAdapterImpl display(nullptr);
    int32_t adapter = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    display.ConvertRotationType(static_cast<Rotation>(adapter));
    display.ConvertDisplayOrientationType(static_cast<OHOS::Rosen::DisplayOrientation>(adapter));
    display.ConvertFoldStatus(static_cast<NativeDisplayManager_FoldDisplayMode>(adapter));
    display.ConvertDisplayState(static_cast<OHOS::Rosen::DisplayState>(adapter));
    display.ConvertDisplaySourceMode(static_cast<OHOS::Rosen::DisplaySourceMode>(adapter));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    DisplayAdapterImpl display1(displayPtr);
    display1.GetId();
    display1.GetWidth();
    display1.GetHeight();
    display1.GetVirtualPixelRatio();
    display1.GetRotation();
    display1.GetOrientation();
    display1.GetDpi();
    display1.GetDisplayOrientation();
    display1.GetFoldStatus();
    display1.IsFoldable();
    display1.GetName();
    display1.GetAvailableWidth();
    display1.GetAvailableHeight();
    display1.GetAliveStatus();
    display1.GetDensityDpi();
    display1.GetX();
    display1.GetY();
    display1.GetDisplaySourceMode();
    display1.GetPhysicalWidth();
    display1.GetPhysicalHeight();
    display1.GetDefaultVirtualPixelRatio();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ConvertRotationFuzzTest(data, size);
    return 0;
}
