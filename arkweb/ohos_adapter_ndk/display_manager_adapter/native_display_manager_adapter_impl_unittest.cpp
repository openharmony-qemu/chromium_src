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
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
class MockOHNativeDisplayManagerSupport {
 public:
  static MockOHNativeDisplayManagerSupport& getInstance() {
    static MockOHNativeDisplayManagerSupport instance;
    return instance;
  }
  MOCK_METHOD(NativeDisplayManager_ErrorCode,
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi,
              (float* xDpi));
  MOCK_METHOD(NativeDisplayManager_ErrorCode,
              OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi,
              (float* yDpi));
  MOCK_METHOD(NativeDisplayManager_ErrorCode,
              OH_NativeDisplayManager_GetDefaultDisplayWidth,
              (int32_t* displayWidth));
  MOCK_METHOD(NativeDisplayManager_ErrorCode,
              OH_NativeDisplayManager_GetDefaultDisplayHeight,
              (int32_t* displayHeight));
  MOCK_METHOD(NativeDisplayManager_ErrorCode,
              OH_NativeDisplayManager_GetDefaultDisplayDensityDpi,
              (int32_t* densityDpi));
};

#ifdef __cplusplus
extern "C" {
#endif
NativeDisplayManager_ErrorCode
OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(float* xDpi) {
  return MockOHNativeDisplayManagerSupport::getInstance()
      .OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(xDpi);
}
NativeDisplayManager_ErrorCode
OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(float* yDpi) {
  return MockOHNativeDisplayManagerSupport::getInstance()
      .OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(yDpi);
}
NativeDisplayManager_ErrorCode OH_NativeDisplayManager_GetDefaultDisplayWidth(
    int32_t* displayWidth) {
  return MockOHNativeDisplayManagerSupport::getInstance()
      .OH_NativeDisplayManager_GetDefaultDisplayWidth(displayWidth);
}
NativeDisplayManager_ErrorCode OH_NativeDisplayManager_GetDefaultDisplayHeight(
    int32_t* displayHeight) {
  return MockOHNativeDisplayManagerSupport::getInstance()
      .OH_NativeDisplayManager_GetDefaultDisplayHeight(displayHeight);
}
NativeDisplayManager_ErrorCode
OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(int32_t* densityDpi) {
  return MockOHNativeDisplayManagerSupport::getInstance()
      .OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(densityDpi);
}
#ifdef __cplusplus
}
#endif

class FoldStatusListenerAdapterMock : public FoldStatusListenerAdapter {
public:
    FoldStatusListenerAdapterMock() = default;

    ~FoldStatusListenerAdapterMock() override = default;

    void OnFoldStatusChanged(FoldStatus foldstatus) override {}
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

TEST_F(NativeDisplayManagerAdapterImplTest, GetDpiTest_001) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
      .WillOnce(Return(DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED));
  int32_t ppi = display_adapter_->GetDpi();
  EXPECT_EQ(ppi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDpiTest_002) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
      .WillOnce(Return(DISPLAY_MANAGER_OK));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(_))
      .WillOnce(Return(DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED));
  int32_t ppi = display_adapter_->GetDpi();
  EXPECT_EQ(ppi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDpiTest_003) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(0), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(0), Return(DISPLAY_MANAGER_OK)));
  int32_t ppi = display_adapter_->GetDpi();
  EXPECT_EQ(ppi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDpiTest_004) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(0), Return(DISPLAY_MANAGER_OK)));
  int32_t ppi = display_adapter_->GetDpi();
  EXPECT_EQ(ppi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDpiTest_005) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayWidth(_))
      .WillOnce(Return(DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED));
  int32_t ppi = display_adapter_->GetDpi();
  EXPECT_EQ(ppi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDpiTest_006) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayWidth(_))
      .WillOnce(Return(DISPLAY_MANAGER_OK));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayHeight(_))
      .WillOnce(Return(DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED));
  int32_t ppi = display_adapter_->GetDpi();
  EXPECT_EQ(ppi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDpiTest_007) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayWidth(_))
      .WillOnce(DoAll(SetArgPointee<0>(0), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayHeight(_))
      .WillOnce(DoAll(SetArgPointee<0>(0), Return(DISPLAY_MANAGER_OK)));
  int32_t ppi = display_adapter_->GetDpi();
  EXPECT_EQ(ppi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDpiTest_008) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayWidth(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayHeight(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  int32_t ppi = display_adapter_->GetDpi();
  EXPECT_NE(ppi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDensityDpi_001) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(_))
      .WillOnce(Return(DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED));
  int32_t densityDpi = display_adapter_->GetDensityDpi();
  EXPECT_EQ(densityDpi, -1);
}

TEST_F(NativeDisplayManagerAdapterImplTest, GetDensityDpi_002) {
  EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
              OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(_))
      .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
  int32_t densityDpi = display_adapter_->GetDensityDpi();
  EXPECT_NE(densityDpi, -1);
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

    EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
                OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(_))
        .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
    EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
                OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(_))
        .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
    EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
                OH_NativeDisplayManager_GetDefaultDisplayWidth(_))
        .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
    EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
                OH_NativeDisplayManager_GetDefaultDisplayHeight(_))
        .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));

    int32_t width = display_adapter_->GetWidth();
    EXPECT_NE(width, -1);

    int32_t height = display_adapter_->GetHeight();
    EXPECT_NE(height, -1);

    float virtual_pixel = display_adapter_->GetVirtualPixelRatio();
    EXPECT_NE(virtual_pixel, -1);

    rotation_type = display_adapter_->GetRotation();

    EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
                OH_NativeDisplayManager_GetDefaultDisplayWidth(_))
        .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
    EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
                OH_NativeDisplayManager_GetDefaultDisplayHeight(_))
        .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
    int32_t ppi = display_adapter_->GetDpi();
    EXPECT_NE(ppi, -1);
    display_orientation = display_adapter_->GetDisplayOrientation();
    fold_status = display_adapter_->GetFoldStatus();

    EXPECT_CALL(MockOHNativeDisplayManagerSupport::getInstance(),
                OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(_))
        .WillOnce(DoAll(SetArgPointee<0>(1), Return(DISPLAY_MANAGER_OK)));
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