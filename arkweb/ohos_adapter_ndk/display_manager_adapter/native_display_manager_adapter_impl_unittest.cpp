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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#define private public
#include "native_display_manager_adapter_impl.h"

namespace OHOS::NWeb {

using ::testing::_;

class FoldStatusListenerAdapterMock : public FoldStatusListenerAdapter {
public:
    FoldStatusListenerAdapterMock() = default;

    ~FoldStatusListenerAdapterMock() override = default;

    void onFoldStatusChanged(FoldStatus foldstatus) override {}
};

class NativeDisplayManagerAdapterImplTest : public testing::Test {
protected:
    void SetUp() override{
        listener_ = std::make_shared<FoldStatusListenerAdapterMock>();
        listener_adapter_ = std::make_unique<NativeFoldStatusListenerAdapterImpl>(listener_);
        display_adapter_ = std::make_unique<NativeDisplayAdapterImpl>();
        manager_adapter_ = std::make_unique<NativeDisplayManagerAdapterImpl>();
    }

    std::shared_ptr<FoldStatusListenerAdapterMock> listener_;
    std::unique_ptr<NativeFoldStatusListenerAdapterImpl> listener_adapter_;
    std::unique_ptr<NativeDisplayAdapterImpl> display_adapter_;
    std::unique_ptr<NativeDisplayManagerAdapterImpl> manager_adapter_;

};

TEST_F(NativeDisplayManagerAdapterImplTest, FoldStatusListenerAdapterTest)
{

    listener_adapter_->OnFoldStatusChanged(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL);
    listener_adapter_->OnFoldStatusChanged(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN);
    listener_adapter_->OnFoldStatusChanged(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB);
    listener_adapter_->OnFoldStatusChanged(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION);
    listener_adapter_->OnFoldStatusChanged(static_cast<NativeDisplayManager_FoldDisplayMode>(999));

    std::shared_ptr<NativeFoldStatusListenerAdapterImpl> null_adapter_ = 
                                        std::make_unique<NativeFoldStatusListenerAdapterImpl>(nullptr);
    null_adapter_->OnFoldStatusChanged(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL);
}

TEST_F(NativeDisplayManagerAdapterImplTest, DisplayAdapterTest)
{
    RotationType rotation_type = display_adapter_->ConvertRotationType(DISPLAY_MANAGER_ROTATION_0);
    EXPECT_EQ(rotation_type, RotationType::ROTATION_0);
    rotation_type = display_adapter_->ConvertRotationType(DISPLAY_MANAGER_ROTATION_90);
    EXPECT_EQ(rotation_type, RotationType::ROTATION_90);
    rotation_type = display_adapter_->ConvertRotationType(DISPLAY_MANAGER_ROTATION_180);
    EXPECT_EQ(rotation_type, RotationType::ROTATION_180);
    rotation_type = display_adapter_->ConvertRotationType(DISPLAY_MANAGER_ROTATION_270);
    EXPECT_EQ(rotation_type, RotationType::ROTATION_270);
    rotation_type = display_adapter_->ConvertRotationType(static_cast<NativeDisplayManager_Rotation>(999));
    EXPECT_EQ(rotation_type, RotationType::ROTATION_BUTT);

    OrientationType orientation_type = display_adapter_
                                        ->ConvertOrientationType(static_cast<NativeDisplayManager_Orientation>(999));
    EXPECT_EQ(orientation_type, OrientationType::BUTT);

    DisplayOrientation display_orientation = display_adapter_
                                              ->ConvertDisplayOrientationType(DISPLAY_MANAGER_PORTRAIT);
    EXPECT_EQ(display_orientation, DisplayOrientation::PORTRAIT);
    display_orientation = display_adapter_->ConvertDisplayOrientationType(DISPLAY_MANAGER_LANDSCAPE);
    EXPECT_EQ(display_orientation, DisplayOrientation::LANDSCAPE);
    display_orientation = display_adapter_->ConvertDisplayOrientationType(DISPLAY_MANAGER_PORTRAIT_INVERTED);
    EXPECT_EQ(display_orientation, DisplayOrientation::PORTRAIT_INVERTED);
    display_orientation = display_adapter_->ConvertDisplayOrientationType(DISPLAY_MANAGER_LANDSCAPE_INVERTED);
    EXPECT_EQ(display_orientation, DisplayOrientation::LANDSCAPE_INVERTED);
    display_orientation = display_adapter_
                            ->ConvertDisplayOrientationType(static_cast<NativeDisplayManager_Orientation>(999));
    EXPECT_EQ(display_orientation, DisplayOrientation::UNKNOWN);

    FoldStatus fold_status = display_adapter_->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL);
    EXPECT_EQ(fold_status, FoldStatus::FULL);
    fold_status = display_adapter_->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN);
    EXPECT_EQ(fold_status, FoldStatus::MAIN);
    fold_status = display_adapter_->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB);
    EXPECT_EQ(fold_status, FoldStatus::SUB);
    fold_status = display_adapter_->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION);
    EXPECT_EQ(fold_status, FoldStatus::COORDINATION);
    fold_status = display_adapter_->ConvertFoldStatus(static_cast<NativeDisplayManager_FoldDisplayMode>(999));
    EXPECT_EQ(fold_status, FoldStatus::UNKNOWN);

    DisplayId display_id = display_adapter_->GetId();
    EXPECT_NE(display_id, static_cast<DisplayId>(-1));

    int32_t width = display_adapter_->GetWidth();
    EXPECT_NE(width, -1);

    int32_t height = display_adapter_->GetHeight();
    EXPECT_NE(height, -1);

    float virtual_pixel = display_adapter_->GetVirtualPixelRatio();
    EXPECT_NE(virtual_pixel, -1);

    rotation_type = display_adapter_->GetRotation();
    int32_t ppi = display_adapter_->GetDpi();
    display_orientation = display_adapter_->GetDisplayOrientation();
    fold_status = display_adapter_->GetFoldStatus();
    int32_t density_dpi = display_adapter_->GetDensityDpi();
    EXPECT_NE(density_dpi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, DisplayManagerAdapterTest)
{
    DisplayId display_id = manager_adapter_->GetDefaultDisplayId();
    EXPECT_NE(display_id, static_cast<DisplayId>(-1));

    bool is_default_portrait = manager_adapter_->IsDefaultPortrait();

    uint32_t reg_id = manager_adapter_->RegisterFoldStatusListener(listener_);
    bool is_unregistered = manager_adapter_->UnregisterFoldStatusListener(reg_id);

    reg_id = manager_adapter_->RegisterFoldStatusListener(nullptr);
    is_unregistered = manager_adapter_->UnregisterFoldStatusListener(reg_id);

}


}   // namespace OHOS::NWeb