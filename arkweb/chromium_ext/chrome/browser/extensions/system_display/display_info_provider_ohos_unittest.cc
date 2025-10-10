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

#include "arkweb/chromium_ext/chrome/browser/extensions/system_display/display_info_provider_ohos.h"
#include "extensions/browser/api/system_display/display_info_provider.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "ui/display/display.h"

namespace OHOS::NWeb {
namespace {

class MockDisplayManagerAdapter : public DisplayManagerAdapter {
 public:
  MockDisplayManagerAdapter() = default;
  ~MockDisplayManagerAdapter() override = default;

  MOCK_METHOD(DisplayId, GetDefaultDisplayId, (), (override));
  MOCK_METHOD(std::shared_ptr<DisplayAdapter>, GetDefaultDisplay, (), (override));
  MOCK_METHOD(ListenerId, RegisterDisplayListener, (std::shared_ptr<DisplayListenerAdapter>), (override));
  MOCK_METHOD(bool, UnregisterDisplayListener, (ListenerId), (override));
  MOCK_METHOD(bool, IsDefaultPortrait, (), (override));
  MOCK_METHOD(std::shared_ptr<DisplayAdapter>, GetPrimaryDisplay, (), (override));
  MOCK_METHOD(std::vector<std::shared_ptr<DisplayAdapter>>, GetAllDisplays, (), (override));
};

class MockDisplayAdapter : public DisplayAdapter {
 public:
  MockDisplayAdapter() = default;
  ~MockDisplayAdapter() override = default;

  MOCK_METHOD(DisplayId, GetId, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(float, GetVirtualPixelRatio, (), (override));
  MOCK_METHOD(RotationType, GetRotation, (), (override));
  MOCK_METHOD(OrientationType, GetOrientation, (), (override));
  MOCK_METHOD(int32_t, GetDpi, (), (override));
  MOCK_METHOD(DisplayOrientation, GetDisplayOrientation, (), (override));
  MOCK_METHOD(FoldStatus, GetFoldStatus, (), (override));
  MOCK_METHOD(bool, IsFoldable, (), (override));
  MOCK_METHOD(std::string, GetName, (), (override));
  MOCK_METHOD(int32_t, GetAvailableWidth, (), (override));
  MOCK_METHOD(int32_t, GetAvailableHeight, (), (override));
  MOCK_METHOD(bool, GetAliveStatus, (), (override));
  MOCK_METHOD(DisplayState, GetDisplayState, (), (override));
  MOCK_METHOD(int32_t, GetDensityDpi, (), (override));
  MOCK_METHOD(int32_t, GetX, (), (override));
  MOCK_METHOD(int32_t, GetY, (), (override));
  MOCK_METHOD(DisplaySourceMode, GetDisplaySourceMode, (), (override));
  MOCK_METHOD(int32_t, GetPhysicalWidth, (), (override));
  MOCK_METHOD(int32_t, GetPhysicalHeight, (), (override));
  MOCK_METHOD(float, GetDefaultVirtualPixelRatio, (), (override));
};

}  // namespace
}  // namespace OHOS::NWeb

namespace extensions {

class DisplayInfoProviderOhosTest : public testing::Test {
 protected:
  void SetUp() override {
    provider_ = std::make_unique<DisplayInfoProviderOhos>();
    auto mock_adapter = std::make_unique<OHOS::NWeb::MockDisplayManagerAdapter>();
    mock_adapter_ = mock_adapter.get();
    provider_->display_manager_adapter_ = std::move(mock_adapter);
  }

  void TearDown() override {
    provider_.reset();
  }

  void ResetDisplayManagerAdapter() {
    provider_->display_manager_adapter_.reset();
  }

  std::unique_ptr<DisplayInfoProviderOhos> provider_;
  OHOS::NWeb::MockDisplayManagerAdapter* mock_adapter_;
};

TEST_F(DisplayInfoProviderOhosTest, EmptyDisplayManagerAdapter) {
  std::vector<display::Display> empty_displays;
  DisplayInfoProvider::DisplayUnitInfoList empty_units;
  ResetDisplayManagerAdapter();

  EXPECT_CALL(*mock_adapter_, GetAllDisplays()).Times(0);

  provider_->UpdateDisplayUnitInfoForPlatform(empty_displays, empty_units);
}

TEST_F(DisplayInfoProviderOhosTest, EmptyDisplaysVector) {
  std::vector<display::Display> empty_displays;
  DisplayInfoProvider::DisplayUnitInfoList empty_units;

  EXPECT_CALL(*mock_adapter_, GetAllDisplays())
      .WillOnce(testing::Return(std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>>{}));

  provider_->UpdateDisplayUnitInfoForPlatform(empty_displays, empty_units);
}

TEST_F(DisplayInfoProviderOhosTest, ExistDisplays_HasNullptr) {
  auto mock_display = std::make_shared<OHOS::NWeb::MockDisplayAdapter>();
  EXPECT_CALL(*mock_display, GetId())
      .WillOnce(testing::Return(12345));

  std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>> displays;
  displays.push_back(nullptr);
  displays.push_back(mock_display);

  EXPECT_CALL(*mock_adapter_, GetAllDisplays())
      .WillOnce(testing::Return(displays));

  std::vector<display::Display> empty_displays;
  DisplayInfoProvider::DisplayUnitInfoList empty_units;

  provider_->UpdateDisplayUnitInfoForPlatform(empty_displays, empty_units);
}

TEST_F(DisplayInfoProviderOhosTest, WithUnits) {
  auto mock_display = std::make_shared<OHOS::NWeb::MockDisplayAdapter>();
  EXPECT_CALL(*mock_display, GetId())
      .WillOnce(testing::Return(12345));

  std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>> displays;
  displays.push_back(mock_display);

  EXPECT_CALL(*mock_adapter_, GetAllDisplays())
      .WillOnce(testing::Return(displays));

  std::vector<display::Display> empty_displays;
  DisplayInfoProvider::DisplayUnitInfoList units;
  units.emplace_back();
  units[0].id = "123";

  provider_->UpdateDisplayUnitInfoForPlatform(empty_displays, units);
}

TEST_F(DisplayInfoProviderOhosTest, MatchingId) {
  auto mock_display = std::make_shared<OHOS::NWeb::MockDisplayAdapter>();

  EXPECT_CALL(*mock_display, GetId())
      .WillOnce(testing::Return(12345));
  EXPECT_CALL(*mock_display, GetName())
      .WillOnce(testing::Return("MockDisplayName"));
  EXPECT_CALL(*mock_display, GetDisplayState())
      .WillOnce(testing::Return(OHOS::NWeb::DisplayState::ON));
  EXPECT_CALL(*mock_display, GetAliveStatus())
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_display, GetDefaultVirtualPixelRatio())
      .WillOnce(testing::Return(1.0));
  EXPECT_CALL(*mock_display, GetVirtualPixelRatio())
      .WillOnce(testing::Return(1.0));
  EXPECT_CALL(*mock_display, GetDensityDpi())
      .WillOnce(testing::Return(160))
      .WillRepeatedly(testing::Return(160));

  EXPECT_CALL(*mock_adapter_, GetAllDisplays())
      .WillOnce(testing::Return(std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>>{mock_display}));

  DisplayInfoProvider::DisplayUnitInfoList units;
  units.emplace_back();
  units[0].id = "12345";

  provider_->UpdateDisplayUnitInfoForPlatform({}, units);

  EXPECT_EQ(units[0].name, "MockDisplayName");
  EXPECT_EQ(units[0].active_state, api::system_display::ActiveState::kActive);
  EXPECT_TRUE(units[0].is_enabled);
  EXPECT_EQ(units[0].display_zoom_factor, 1.0);
  EXPECT_EQ(units[0].dpi_x, 160);
  EXPECT_EQ(units[0].dpi_y, 160);
}

TEST_F(DisplayInfoProviderOhosTest, MatchingId_DisplayStateIsOff_DefaultVirtualIsZero) {
  auto mock_display = std::make_shared<OHOS::NWeb::MockDisplayAdapter>();

  EXPECT_CALL(*mock_display, GetId())
      .WillOnce(testing::Return(12345));
  EXPECT_CALL(*mock_display, GetName())
      .WillOnce(testing::Return("MockDisplayName"));
  EXPECT_CALL(*mock_display, GetDisplayState())
      .WillOnce(testing::Return(OHOS::NWeb::DisplayState::OFF));
  EXPECT_CALL(*mock_display, GetAliveStatus())
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_display, GetDefaultVirtualPixelRatio())
      .WillOnce(testing::Return(0));
  EXPECT_CALL(*mock_display, GetDensityDpi())
      .WillOnce(testing::Return(160))
      .WillRepeatedly(testing::Return(160));

  EXPECT_CALL(*mock_display, GetVirtualPixelRatio()).Times(0);

  EXPECT_CALL(*mock_adapter_, GetAllDisplays())
      .WillOnce(testing::Return(std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>>{mock_display}));

  DisplayInfoProvider::DisplayUnitInfoList units;
  units.emplace_back();
  units[0].id = "12345";

  provider_->UpdateDisplayUnitInfoForPlatform({}, units);

  EXPECT_EQ(units[0].name, "MockDisplayName");
  EXPECT_EQ(units[0].active_state, api::system_display::ActiveState::kInactive);
  EXPECT_TRUE(units[0].is_enabled);
  EXPECT_EQ(units[0].dpi_x, 160);
  EXPECT_EQ(units[0].dpi_y, 160);
}

TEST_F(DisplayInfoProviderOhosTest, MatchingId_DisplayStateIsUnknown) {
  auto mock_display = std::make_shared<OHOS::NWeb::MockDisplayAdapter>();

  EXPECT_CALL(*mock_display, GetId())
      .WillOnce(testing::Return(12345));
  EXPECT_CALL(*mock_display, GetName())
      .WillOnce(testing::Return("MockDisplayName"));
  EXPECT_CALL(*mock_display, GetDisplayState())
      .WillOnce(testing::Return(OHOS::NWeb::DisplayState::UNKNOWN));
  EXPECT_CALL(*mock_display, GetAliveStatus())
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_display, GetDefaultVirtualPixelRatio())
      .WillOnce(testing::Return(1.0));
  EXPECT_CALL(*mock_display, GetVirtualPixelRatio())
      .WillOnce(testing::Return(2.0));
  EXPECT_CALL(*mock_display, GetDensityDpi())
      .WillOnce(testing::Return(160))
      .WillRepeatedly(testing::Return(160));

  EXPECT_CALL(*mock_adapter_, GetAllDisplays())
      .WillOnce(testing::Return(std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>>{mock_display}));

  DisplayInfoProvider::DisplayUnitInfoList units;
  units.emplace_back();
  units[0].id = "12345";

  provider_->UpdateDisplayUnitInfoForPlatform({}, units);

  EXPECT_EQ(units[0].name, "MockDisplayName");
  EXPECT_EQ(units[0].active_state, api::system_display::ActiveState::kInactive);
  EXPECT_TRUE(units[0].is_enabled);
  EXPECT_EQ(units[0].display_zoom_factor, 2.0);
  EXPECT_EQ(units[0].dpi_x, 160);
  EXPECT_EQ(units[0].dpi_y, 160);
}

}  // namespace extensions