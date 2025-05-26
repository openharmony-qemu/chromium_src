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

#include "native_display_manager_adapter_impl.h"

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <string>

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
constexpr float EPS = 0.0001f;
NativeFoldStatusListenerAdapterImpl::NativeFoldStatusListenerAdapterImpl(
    std::shared_ptr<FoldStatusListenerAdapter> listener) : listener_(listener) {}

OHOS::NWeb::FoldStatus NativeFoldStatusListenerAdapterImpl::ConvertFoldStatus(
    NativeDisplayManager_FoldDisplayMode displayMode)
{
    switch (displayMode) {
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL:
            return OHOS::NWeb::FoldStatus::FULL;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN:
            return OHOS::NWeb::FoldStatus::MAIN;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB:
            return OHOS::NWeb::FoldStatus::SUB;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION:
            return OHOS::NWeb::FoldStatus::COORDINATION;
        default:
            return OHOS::NWeb::FoldStatus::UNKNOWN;
    }
}

void NativeFoldStatusListenerAdapterImpl::OnFoldStatusChanged(NativeDisplayManager_FoldDisplayMode displayMode)
{
    if (listener_ != nullptr) {
        listener_->OnFoldStatusChanged(ConvertFoldStatus(displayMode));
    }
}

NativeDisplayAdapterImpl::NativeDisplayAdapterImpl() {}

OHOS::NWeb::RotationType NativeDisplayAdapterImpl::ConvertRotationType(NativeDisplayManager_Rotation type)
{
    switch (type) {
        case NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_0:
            return OHOS::NWeb::RotationType::ROTATION_0;
        case NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_90:
            return OHOS::NWeb::RotationType::ROTATION_90;
        case NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_180:
            return OHOS::NWeb::RotationType::ROTATION_180;
        case NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_270:
            return OHOS::NWeb::RotationType::ROTATION_270;
        default:
            return OHOS::NWeb::RotationType::ROTATION_BUTT;
    }
}

OHOS::NWeb::OrientationType NativeDisplayAdapterImpl::ConvertOrientationType(NativeDisplayManager_Orientation type)
{
    return OHOS::NWeb::OrientationType::BUTT;
}

OHOS::NWeb::DisplayOrientation NativeDisplayAdapterImpl::ConvertDisplayOrientationType(
    NativeDisplayManager_Orientation type)
{
    switch (type) {
        case NativeDisplayManager_Orientation::DISPLAY_MANAGER_PORTRAIT:
            return OHOS::NWeb::DisplayOrientation::PORTRAIT;
        case NativeDisplayManager_Orientation::DISPLAY_MANAGER_LANDSCAPE:
            return OHOS::NWeb::DisplayOrientation::LANDSCAPE;
        case NativeDisplayManager_Orientation::DISPLAY_MANAGER_PORTRAIT_INVERTED:
            return OHOS::NWeb::DisplayOrientation::PORTRAIT_INVERTED;
        case NativeDisplayManager_Orientation::DISPLAY_MANAGER_LANDSCAPE_INVERTED:
            return OHOS::NWeb::DisplayOrientation::LANDSCAPE_INVERTED;
        default:
            return OHOS::NWeb::DisplayOrientation::UNKNOWN;
    }
}

OHOS::NWeb::FoldStatus NativeDisplayAdapterImpl::ConvertFoldStatus(NativeDisplayManager_FoldDisplayMode displayMode)
{
    switch (displayMode) {
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL:
            return OHOS::NWeb::FoldStatus::FULL;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN:
            return OHOS::NWeb::FoldStatus::MAIN;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB:
            return OHOS::NWeb::FoldStatus::SUB;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION:
            return OHOS::NWeb::FoldStatus::COORDINATION;
        default:
            return OHOS::NWeb::FoldStatus::UNKNOWN;
    }
}

DisplayId NativeDisplayAdapterImpl::GetId()
{
    uint64_t displayId;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayId(&displayId);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return static_cast<DisplayId>(-1);
    }
    return displayId;
}

int32_t NativeDisplayAdapterImpl::GetWidth()
{
    int32_t width;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayWidth(&width);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return -1;
    }
    return width;
}

int32_t NativeDisplayAdapterImpl::GetHeight()
{
    int32_t height;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayHeight(&height);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return -1;
    }
    return height;
}

float NativeDisplayAdapterImpl::GetVirtualPixelRatio()
{
    float virtualPixel;
    NativeDisplayManager_ErrorCode errorCode
        = OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio(&virtualPixel);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return -1;
    }
    return virtualPixel;
}

RotationType NativeDisplayAdapterImpl::GetRotation()
{
    NativeDisplayManager_Rotation displayRotation;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayRotation(&displayRotation);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return RotationType::ROTATION_BUTT;
    }
    return ConvertRotationType(displayRotation);
}

OrientationType NativeDisplayAdapterImpl::GetOrientation()
{
    return OrientationType::BUTT;
}

int32_t NativeDisplayAdapterImpl::GetDpi()
{
    int32_t ppi = -1;
    float xDpi = 0.0f;
    float yDpi = 0.0f;
    int32_t width = 0;
    int32_t height = 0;

    NativeDisplayManager_ErrorCode errorCode
        = OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(&xDpi);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return ppi;
    }

    errorCode = OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(&yDpi);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return ppi;
    }

    if (xDpi < EPS || yDpi < EPS) {
        return ppi;
    }

    errorCode = OH_NativeDisplayManager_GetDefaultDisplayWidth(&width);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return ppi;
    }

    errorCode = OH_NativeDisplayManager_GetDefaultDisplayHeight(&height);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return ppi;
    }
    auto screenLength = sqrt(pow(width, 2) + pow(height, 2));
    auto phyScreenLength = sqrt(pow(width / xDpi, 2) + pow(height / yDpi, 2));
    if (phyScreenLength < EPS) {
        return ppi;
    }
    ppi = screenLength / phyScreenLength;
    WVLOG_I("dpi: %{public}d, xdpi: %{public}f,ydpi: %{public}f, width: %{public}d, height: %{public}d, "\
        "phyScreenLength: %{public}f", ppi, xDpi, yDpi,
        width, height, phyScreenLength);

    return ppi;
}

DisplayOrientation NativeDisplayAdapterImpl::GetDisplayOrientation()
{
    NativeDisplayManager_Orientation displayOrientation;
    NativeDisplayManager_ErrorCode errorCode
        = OH_NativeDisplayManager_GetDefaultDisplayOrientation(&displayOrientation);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return ConvertDisplayOrientationType(displayOrientation);
    }
    return DisplayOrientation::UNKNOWN;
}

FoldStatus NativeDisplayAdapterImpl::GetFoldStatus()
{
    NativeDisplayManager_FoldDisplayMode displayMode =
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_UNKNOWN;
    OH_NativeDisplayManager_GetFoldDisplayMode(&displayMode);
    return ConvertFoldStatus(displayMode);
}

bool NativeDisplayAdapterImpl::IsFoldable()
{
    return OH_NativeDisplayManager_IsFoldable();
}

std::string NativeDisplayAdapterImpl::GetName()
{
    return "";
}

int32_t NativeDisplayAdapterImpl::GetAvailableWidth()
{
    return 0;
}

int32_t NativeDisplayAdapterImpl::GetAvailableHeight()
{
    return 0;
}

bool NativeDisplayAdapterImpl::GetAliveStatus()
{
    return true;
}

DisplayState NativeDisplayAdapterImpl::GetDisplayState()
{
    return DisplayState::UNKNOWN;
}

int32_t NativeDisplayAdapterImpl::GetDensityDpi()
{
    int32_t densityDpi;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(&densityDpi);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return -1;
    }
    return densityDpi;
}

int32_t NativeDisplayAdapterImpl::GetX()
{
    return 0;
}

int32_t NativeDisplayAdapterImpl::GetY()
{
    return 0;
}

DisplaySourceMode NativeDisplayAdapterImpl::GetDisplaySourceMode()
{
    return DisplaySourceMode::NONE;
}

int32_t NativeDisplayAdapterImpl::GetPhysicalWidth()
{
    return 0;
}

int32_t NativeDisplayAdapterImpl::GetPhysicalHeight()
{
    return 0;
}

float NativeDisplayAdapterImpl::GetDefaultVirtualPixelRatio()
{
    return 0;
}

FoldStatusListenerMap NativeDisplayManagerAdapterImpl::foldStatusReg_;

DisplayId NativeDisplayManagerAdapterImpl::GetDefaultDisplayId()
{
    WVLOG_D("DisplayManagerAdapter::GetDefaultDisplayId.");
    uint64_t displayId;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayId(&displayId);
    if (DISPLAY_MANAGER_OK != errorCode) {
        return static_cast<DisplayId>(-1);
    }
    return displayId;
}

std::shared_ptr<DisplayAdapter> NativeDisplayManagerAdapterImpl::GetDefaultDisplay()
{
    return std::make_shared<NativeDisplayAdapterImpl>();
}

void NativeDisplayManagerAdapterImpl::DisplayChangeCallback(uint64_t displayId)
{
    WVLOG_D("DisplayManagerAdapter::DisplayChangeCallback.");
}

uint32_t NativeDisplayManagerAdapterImpl::RegisterDisplayListener(
    std::shared_ptr<DisplayListenerAdapter> listener)
{
    WVLOG_D("DisplayManagerAdapter::RegisterDisplayListener.");

    return 0;
}

bool NativeDisplayManagerAdapterImpl::UnregisterDisplayListener(uint32_t id)
{
    WVLOG_D("DisplayManagerAdapter::UnregisterDisplayListener.");

    return true;
}

bool NativeDisplayManagerAdapterImpl::IsDefaultPortrait()
{
    std::string deviceType = OH_GetDeviceType();
    return deviceType == "phone" || deviceType == "default";
}

void FoldChangeCallBack(NativeDisplayManager_FoldDisplayMode displayMode)
{
    for (auto& iter : NativeDisplayManagerAdapterImpl::foldStatusReg_) {
        iter.second->OnFoldStatusChanged(displayMode);
    }
}

uint32_t NativeDisplayManagerAdapterImpl::RegisterFoldStatusListener(
    std::shared_ptr<FoldStatusListenerAdapter> listener)
{
    std::shared_ptr<NativeFoldStatusListenerAdapterImpl> reg =
        std::make_shared<NativeFoldStatusListenerAdapterImpl> (listener);
    if (reg == nullptr) {
        return false;
    }

    uint32_t id = 1;
    if (OH_NativeDisplayManager_RegisterFoldDisplayModeChangeListener(
        FoldChangeCallBack, &id) == NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK) {
        foldStatusReg_.emplace(std::make_pair(id, reg));
        return id;
    } else {
        return 0;
    }
}

bool NativeDisplayManagerAdapterImpl::UnregisterFoldStatusListener(uint32_t id)
{
    FoldStatusListenerMap::iterator iter = foldStatusReg_.find(id);
    if (iter == foldStatusReg_.end()) {
        return false;
    }
    if (OH_NativeDisplayManager_UnregisterFoldDisplayModeChangeListener(id) ==
        NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK) {
        foldStatusReg_.erase(iter);
        return true;
    }
    return false;
}

std::shared_ptr<DisplayAdapter> NativeDisplayManagerAdapterImpl::GetPrimaryDisplay()
{
   return nullptr;
}

std::vector<std::shared_ptr<DisplayAdapter>> NativeDisplayManagerAdapterImpl::GetAllDisplays()
{
   return std::vector<std::shared_ptr<DisplayAdapter>>();
}
}
