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

#ifndef DISPLAY_MANAGER_ADAPTER_IMPL_H
#define DISPLAY_MANAGER_ADAPTER_IMPL_H

#include <map>

#include "third_party/ohos_ndk/includes/ohos_adapter/display_manager_adapter.h"

#include "ohos_sdk/openharmony/native/sysroot/usr/include/window_manager/oh_display_info.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/window_manager/oh_display_manager.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/deviceinfo.h"

namespace OHOS::NWeb {
class NativeFoldStatusListenerAdapterImpl {
public:
    explicit NativeFoldStatusListenerAdapterImpl(std::shared_ptr<FoldStatusListenerAdapter> listener);
    ~NativeFoldStatusListenerAdapterImpl() = default;
    void OnFoldStatusChanged(NativeDisplayManager_FoldDisplayMode displayMode) ;
private:
    OHOS::NWeb::FoldStatus ConvertFoldStatus(NativeDisplayManager_FoldDisplayMode displayMode);
    std::shared_ptr<FoldStatusListenerAdapter> listener_;
};

class NativeDisplayAdapterImpl : public DisplayAdapter {
public:
    NativeDisplayAdapterImpl();
    ~NativeDisplayAdapterImpl() override = default;
    DisplayId GetId() override;
    int32_t GetWidth() override;
    int32_t GetHeight() override;
    float GetVirtualPixelRatio() override;
    RotationType GetRotation() override;
    OrientationType GetOrientation() override;
    int32_t GetDpi() override;
    DisplayOrientation GetDisplayOrientation() override;
    FoldStatus GetFoldStatus() override;
    bool IsFoldable() override;
    std::string GetName() override;
    int32_t GetAvailableWidth() override;
    int32_t GetAvailableHeight() override;
    bool GetAliveStatus() override;
    DisplayState GetDisplayState() override;
    int32_t GetDensityDpi() override;
    int32_t GetX() override;
    int32_t GetY() override;
    DisplaySourceMode GetDisplaySourceMode() override;
    int32_t GetPhysicalWidth() override;
    int32_t GetPhysicalHeight() override;
    float GetDefaultVirtualPixelRatio() override;
private:
    OHOS::NWeb::RotationType ConvertRotationType(NativeDisplayManager_Rotation type);
    OHOS::NWeb::OrientationType ConvertOrientationType(NativeDisplayManager_Orientation type);
    OHOS::NWeb::DisplayOrientation ConvertDisplayOrientationType(NativeDisplayManager_Orientation type);
    OHOS::NWeb::FoldStatus ConvertFoldStatus(NativeDisplayManager_FoldDisplayMode displayMode);
    OHOS::NWeb::DisplayState ConvertDisplayState(DisplayState state);
    OHOS::NWeb::DisplaySourceMode ConvertDisplaySourceMode(DisplaySourceMode mode);
};

using FoldStatusListenerMap =
    std::map<int32_t, std::shared_ptr<NativeFoldStatusListenerAdapterImpl>>;

class NativeDisplayManagerAdapterImpl : public DisplayManagerAdapter {
public:
    NativeDisplayManagerAdapterImpl() = default;
    ~NativeDisplayManagerAdapterImpl() override = default;
    DisplayId GetDefaultDisplayId() override;
    std::shared_ptr<DisplayAdapter> GetDefaultDisplay() override;
    uint32_t RegisterDisplayListener(std::shared_ptr<DisplayListenerAdapter> listener) override;
    bool UnregisterDisplayListener(uint32_t id) override;
    bool IsDefaultPortrait() override;
    static void DisplayChangeCallback(uint64_t displayId);
    uint32_t RegisterFoldStatusListener(std::shared_ptr<FoldStatusListenerAdapter> listener) override;
    bool UnregisterFoldStatusListener(uint32_t id) override;
    static FoldStatusListenerMap foldStatusReg_;
    std::shared_ptr<DisplayAdapter> GetPrimaryDisplay() override;
    std::vector<std::shared_ptr<DisplayAdapter>> GetAllDisplays() override;
};
}

#endif
