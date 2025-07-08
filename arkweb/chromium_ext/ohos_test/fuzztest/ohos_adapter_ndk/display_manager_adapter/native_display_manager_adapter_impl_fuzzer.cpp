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
#define private public
#include <fuzzer/FuzzedDataProvider.h>
#include <cstring>
#include <cstdint>
#include "native_display_manager_adapter_impl.h"
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/window_manager/oh_display_info.h"


using namespace OHOS::NWeb;

void FuzzNativeDisplayAdapterImpl(FuzzedDataProvider* fdp) {
    std::shared_ptr<FoldStatusListenerAdapter> listener =
     std::make_unique<FoldStatusListenerAdapter>();
    NativeFoldStatusListenerAdapterImpl adapter(listener);
    int mode = fdp->ConsumeIntegralInRange<int>(0, 5);
    adapter.OnFoldStatusChanged(static_cast<NativeDisplayManager_FoldDisplayMode>(mode));

    std::shared_ptr<NativeDisplayAdapterImpl> displayAdapter =
     std::make_unique<NativeDisplayAdapterImpl>();

    int rotation = fdp->ConsumeIntegralInRange<int>(0, 4);
    displayAdapter->ConvertRotationType(static_cast<NativeDisplayManager_Rotation>(rotation));

    int orientation = fdp->ConsumeIntegralInRange<int>(0, 5);
    displayAdapter->
    ConvertOrientationType(static_cast<NativeDisplayManager_Orientation>(orientation));
    displayAdapter->
    ConvertDisplayOrientationType(static_cast<NativeDisplayManager_Orientation>(orientation));

    int displayMode = fdp->ConsumeIntegralInRange<int>(0, 5);
    displayAdapter->
    ConvertFoldStatus(static_cast<NativeDisplayManager_FoldDisplayMode>(displayMode));

    displayAdapter->GetId();
    displayAdapter->GetWidth();
    displayAdapter->GetHeight();
    displayAdapter->GetVirtualPixelRatio();
    displayAdapter->GetRotation();
    displayAdapter->GetOrientation();
    displayAdapter->GetDpi();
    displayAdapter->GetDisplayOrientation();
    displayAdapter->GetFoldStatus();
    displayAdapter->IsFoldable();
    displayAdapter->GetName();
    displayAdapter->GetAvailableWidth();
    displayAdapter->GetAvailableHeight();
    displayAdapter->GetAliveStatus();
    displayAdapter->GetDisplayState();
    displayAdapter->GetDensityDpi();
    displayAdapter->GetX();
    displayAdapter->GetY();
    displayAdapter->GetDisplaySourceMode();
    displayAdapter->GetPhysicalWidth();
    displayAdapter->GetPhysicalHeight();
    displayAdapter->GetDefaultVirtualPixelRatio();

    std::shared_ptr<NativeDisplayManagerAdapterImpl> displaymanger =
     std::make_unique<NativeDisplayManagerAdapterImpl>();
    displaymanger->GetDefaultDisplayId();
    displaymanger->GetDefaultDisplay();
    uint32_t displayId = fdp->ConsumeIntegral<uint32_t>();
    displaymanger->DisplayChangeCallback(displayId);

    std::shared_ptr<DisplayListenerAdapter> displaylistener = nullptr;
    displaymanger->RegisterDisplayListener(displaylistener);
    uint32_t id = fdp->ConsumeIntegral<uint32_t>();
    displaymanger->UnregisterDisplayListener(id);
    displaymanger->IsDefaultPortrait();

    std::shared_ptr<FoldStatusListenerAdapter> flodlistener =
     std::make_unique<FoldStatusListenerAdapter>();
    displaymanger->RegisterFoldStatusListener(flodlistener);
    uint32_t flodId = fdp->ConsumeIntegral<uint32_t>();
    displaymanger.UnregisterFoldStatusListener(flodId);
    displaymanger->GetPrimaryDisplay();
    displaymanger->GetAllDisplays();
}


extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if((data == nullptr) || (size < sizeof(int32_t))) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    FuzzNativeDisplayAdapterImpl(&fdp);
    return 0;
}
 