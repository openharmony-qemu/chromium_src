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

#include "base/memory/weak_ptr.h"
#define private public
#include "ui/ozone/platform/headless/headless_screen_ohos.h"
#undef private
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/task_environment.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/display_manager_adapter.h"
#include "ui/display/display_list.h"
#include "ui/display/display.h"
#include "ui/display/display_observer.h"
#include "base/test/launcher/unit_test_launcher.h"
#include "base/test/test_suite.h"
#include "mojo/core/embedder/configuration.h"
#include "mojo/core/embedder/embedder.h"

using ::testing::Return;
int main(int argc, char** argv) {
  base::TestSuite test_suite(argc, argv);
  mojo::core::Init(mojo::core::Configuration());

  return base::LaunchUnitTestsSerially(
      argc, argv,
      base::BindOnce(&base::TestSuite::Run, base::Unretained(&test_suite)));
}

namespace ui {

using ::testing::_;
using namespace OHOS::NWeb;

namespace {
struct MockDisplayManagerAdapter : public OHOS::NWeb::DisplayManagerAdapter {
  MockDisplayManagerAdapter() = default;
  ~MockDisplayManagerAdapter() override = default;

  MOCK_METHOD(std::shared_ptr<OHOS::NWeb::DisplayAdapter>,
              GetPrimaryDisplay,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<DisplayAdapter>,
              GetDefaultDisplay,
              (),
              (override));
  MOCK_METHOD(DisplayId, GetDefaultDisplayId, (), (override));
  MOCK_METHOD(std::vector<std::shared_ptr<DisplayAdapter>>,
              GetAllDisplays,
              (),
              (override));
  ListenerId RegisterDisplayListener(
      std::shared_ptr<DisplayListenerAdapter> listener) override {
    return 0;
  }
  bool UnregisterDisplayListener(ListenerId id) override { return false; }
  bool IsDefaultPortrait() override { return false; }
};

class MockDisplayAdapter : public DisplayAdapter {
 public:
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

struct MockDisplayObserver : public display::DisplayObserver {
  MockDisplayObserver() = default;
  ~MockDisplayObserver() override = default;

  MOCK_METHOD1(OnDisplayAdded, void(const display::Display& new_display));
  MOCK_METHOD1(OnDisplaysRemoved,
               void(const display::Displays& removed_displays));
  MOCK_METHOD2(OnDisplayMetricsChanged,
               void(const display::Display& display, uint32_t changed_metrics));
};

}  // namespace

class HeadlessScreenOhosTest : public testing::Test {
protected:
  void SetUp() override {
    screen_ = std::make_unique<HeadlessScreenOhos>();
    screen_->Initialize();
    listener_ = screen_->display_listener_;
  }

  base::test::SingleThreadTaskEnvironment task_environment_;
  std::unique_ptr<HeadlessScreenOhos> screen_;
  std::shared_ptr<HeadlessScreenListener> listener_;
  display::Display dst_display{1, gfx::Rect(0, 0, 800, 600)};
};

TEST_F(HeadlessScreenOhosTest, GetAllDisplaysReturnsEmptyInitially) {
  auto displays = screen_->GetAllDisplays();
  EXPECT_FALSE(displays.empty());
}

TEST_F(HeadlessScreenOhosTest, PrimaryDisplayReturnsValidDisplay) {
  auto primary = screen_->GetPrimaryDisplay();
  EXPECT_NE(primary.id(), display::kInvalidDisplayId);
}

TEST_F(HeadlessScreenOhosTest, OnDisplayCreateAddsDisplay) {
  constexpr OHOS::NWeb::DisplayId test_id = 1;
  screen_->OnDisplayCreate(test_id);
  auto displays = screen_->GetAllDisplays();
  EXPECT_FALSE(displays.empty());
}

TEST_F(HeadlessScreenOhosTest, OnDisplayDestroyRemovesDisplay) {
  constexpr OHOS::NWeb::DisplayId test_id = 1;
  screen_->OnDisplayCreate(test_id);
  screen_->OnDisplayDestroy(test_id);
  auto displays = screen_->GetAllDisplays();
  EXPECT_FALSE(displays.empty());
}

TEST_F(HeadlessScreenOhosTest, AddObserverNotifiesOnDisplayChange) {
  MockDisplayObserver observer;
  screen_->AddObserver(&observer);
  screen_->OnDisplayCreate(1);
}

TEST_F(HeadlessScreenOhosTest, RemoveObserverStopsNotifications) {
  MockDisplayObserver observer;
  screen_->AddObserver(&observer);
  screen_->RemoveObserver(&observer);
    
  EXPECT_CALL(observer, OnDisplayAdded(testing::_)).Times(0);
  screen_->OnDisplayCreate(1);
}

TEST_F(HeadlessScreenOhosTest, ListenerOnCreateForwardsToScreen) {
  constexpr OHOS::NWeb::DisplayId test_id = 1;
  listener_->OnCreate(test_id);
}

TEST_F(HeadlessScreenOhosTest, ListenerOnDestroyForwardsToScreen) {
  constexpr OHOS::NWeb::DisplayId test_id = 1;
  listener_->OnDestroy(test_id);
}

TEST_F(HeadlessScreenOhosTest, ListenerOnChangeForwardsToScreen) {
  constexpr OHOS::NWeb::DisplayId test_id = 1;
  listener_->OnChange(test_id);
}

TEST_F(HeadlessScreenOhosTest, FetchDisplays) {
  display::DisplayList displays;
  screen_->display_manager_adapter_ = nullptr;
  bool result = screen_->FetchDisplays(displays);
  EXPECT_FALSE(result);
}

TEST_F(HeadlessScreenOhosTest, HeadlessScreenOhos_001) {
  std::unique_ptr<HeadlessScreenOhos> headless_screen_ =
      std::make_unique<HeadlessScreenOhos>();
  headless_screen_->display_listener_ = nullptr;
  headless_screen_.reset();
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_NullSrcDisplay) {
  bool result = screen_->ConvertDisplay(nullptr, dst_display);
  EXPECT_FALSE(result);
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_ModeIsNone) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::NONE));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_FALSE(result);
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_ModeIsEXTEND) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::EXTEND));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_Scale) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetVirtualPixelRatio())
      .WillRepeatedly(Return(1));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_FALSE(result);
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_ROTATION_0) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_0));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_ROTATION_90) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_90));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_ROTATION_180) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_180));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_ROTATION_270) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_270));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, ConvertDisplay_ROTATION_BUTT) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_BUTT));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, OnDisplayEvent) {
  constexpr OHOS::NWeb::DisplayId test_id = 1;
  const std::string& event = "create";
  screen_->display_manager_adapter_ = nullptr;
  display::DisplayList current_displays;
  bool result = screen_->FetchDisplays(current_displays);
  EXPECT_FALSE(result);
  screen_->OnDisplayEvent(event, test_id);
}

TEST_F(HeadlessScreenOhosTest, FetchDisplays_Empty) {
  display::DisplayList displays;
  auto manager = new MockDisplayManagerAdapter();
  screen_->display_manager_adapter_.reset(manager);
  auto adapter = std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*manager, GetDefaultDisplayId());
  EXPECT_CALL(*manager, GetPrimaryDisplay());
  EXPECT_CALL(*manager, GetAllDisplays()).WillOnce(testing::Invoke([=]() {
    std::vector<std::shared_ptr<DisplayAdapter>> ohos_displays{adapter};
    return ohos_displays;
  }));
  EXPECT_CALL(*adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::EXTEND));
  EXPECT_CALL(*adapter, GetId()).WillRepeatedly(Return(UINT64_MAX));
  EXPECT_FALSE(screen_->FetchDisplays(displays));
  EXPECT_TRUE(displays.displays().empty());
}

TEST_F(HeadlessScreenOhosTest, LayoutDisplays_001) {
  display::Displays displays = {
      display::Display(0, gfx::Rect(0, 0, 0, 0)),
      display::Display(1, gfx::Rect(0, 0, 1000, 1000))};
  screen_->LayoutDisplays(displays);
  EXPECT_EQ(displays[0].bounds().origin(), gfx::Point(0, 0));
  EXPECT_EQ(displays[1].bounds().origin(), gfx::Point(0, 0));
}

TEST_F(HeadlessScreenOhosTest, LayoutDisplays_002) {
  display::Displays displays = {
      display::Display(0, gfx::Rect(1000, 0, 1000, 1000)),
      display::Display(1, gfx::Rect(-1000, 0, 1000, 1000))};
  screen_->LayoutDisplays(displays);
  EXPECT_EQ(displays[0].bounds().origin(), gfx::Point(1000, 0));
  EXPECT_EQ(displays[1].bounds().origin(), gfx::Point(-1000, 0));
}

TEST_F(HeadlessScreenOhosTest, LayoutDisplays_003) {
  display::Displays displays = {
      display::Display(0, gfx::Rect(0, 0, 2000, 2000)),
      display::Display(1, gfx::Rect(2000, 0, 1000, 2000)),
      display::Display(2, gfx::Rect(1000, 0, 2000, 2000)),
      display::Display(3, gfx::Rect(2000, 3000, 1000, 1000)),
      display::Display(4, gfx::Rect(2000, -1000, 1000, 1000)),
      display::Display(5, gfx::Rect(3000, 2000, 1000, 1000)),
      display::Display(6, gfx::Rect(0, 1000, 2000, 2000)),
      display::Display(7, gfx::Rect(0, 2000, 2000, 1000)),
      display::Display(8, gfx::Rect(-1000, 2000, 1000, 1000)),
      display::Display(9, gfx::Rect(1000, 1000, 1000, 1000)),
      display::Display(10, gfx::Rect(-2000, 2000, 1000, 1000)),
      display::Display(11, gfx::Rect(2000, -2000, 1000, 1000)),
      display::Display(12, gfx::Rect(-500, 2000, 3000, 1000)),
      display::Display(13, gfx::Rect(-1000, -1000, 1000, 1000)),
      display::Display(14, gfx::Rect(500, -1000, 1000, 1000))};
  screen_->LayoutDisplays(displays);
  EXPECT_EQ(displays[0].bounds().origin(), gfx::Point(0, 0));
  EXPECT_EQ(displays[1].bounds().origin(), gfx::Point(2000, 0));
  EXPECT_EQ(displays[4].bounds().origin(), gfx::Point(2000, -1000));
  EXPECT_EQ(displays[7].bounds().origin(), gfx::Point(0, 2000));
  EXPECT_EQ(displays[8].bounds().origin(), gfx::Point(-1000, 2000));
  EXPECT_EQ(displays[13].bounds().origin(), gfx::Point(-1000, -1000));
  EXPECT_EQ(displays[14].bounds().origin(), gfx::Point(500, -1000));
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest001) {
  auto primary = screen_->GetPrimaryDisplay();
  EXPECT_NE(primary.id(), display::kInvalidDisplayId);

  constexpr OHOS::NWeb::DisplayId test_id = 1;
  screen_->OnDisplayCreate(test_id);
  screen_->OnDisplayDestroy(test_id);
  auto displays = screen_->GetAllDisplays();
  EXPECT_FALSE(displays.empty());

  MockDisplayObserver observer;
  screen_->AddObserver(&observer);
  screen_->RemoveObserver(&observer);

  constexpr OHOS::NWeb::DisplayId test_id2 = 2;
  listener_->OnCreate(test_id2);
  listener_->OnDestroy(test_id2);
  listener_->OnChange(test_id2);

  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest002) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::EXTEND));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest003) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetVirtualPixelRatio())
      .WillRepeatedly(Return(1));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_FALSE(result);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest004) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_BUTT));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest005) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_180));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest006) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_90));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest007) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_270));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest008) {
  std::shared_ptr<MockDisplayAdapter> display_adapter =
      std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*display_adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*display_adapter, GetRotation())
      .WillRepeatedly(Return(OHOS::NWeb::RotationType::ROTATION_BUTT));
  bool result = screen_->ConvertDisplay(display_adapter, dst_display);
  EXPECT_TRUE(result);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest009) {
  constexpr OHOS::NWeb::DisplayId test_id = 1;
  const std::string& event = "change";
  screen_->display_manager_adapter_ = nullptr;
  display::DisplayList current_displays;
  bool result = screen_->FetchDisplays(current_displays);
  EXPECT_FALSE(result);
  screen_->OnDisplayEvent(event, test_id);
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest010) {
  display::DisplayList displays;
  auto manager = new MockDisplayManagerAdapter();
  screen_->display_manager_adapter_.reset(manager);
  auto adapter = std::make_shared<MockDisplayAdapter>();
  EXPECT_CALL(*manager, GetAllDisplays()).WillOnce(testing::Invoke([=]() {
    std::vector<std::shared_ptr<DisplayAdapter>> ohos_displays{adapter};
    return ohos_displays;
  }));
  EXPECT_CALL(*manager, GetDefaultDisplayId());
  EXPECT_CALL(*manager, GetPrimaryDisplay());
  EXPECT_CALL(*adapter, GetDisplaySourceMode())
      .WillRepeatedly(Return(OHOS::NWeb::DisplaySourceMode::MAIN));
  EXPECT_CALL(*adapter, GetId()).WillRepeatedly(Return(UINT64_MAX));
  EXPECT_FALSE(screen_->FetchDisplays(displays));
  EXPECT_TRUE(displays.displays().empty());
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest011) {
  display::Displays displays = {
      display::Display(0, gfx::Rect(0, 0, 0, 0)),
      display::Display(1, gfx::Rect(0, 0, 100, 100))};
  screen_->LayoutDisplays(displays);
  EXPECT_EQ(displays[0].bounds().origin(), gfx::Point(0, 0));
  EXPECT_EQ(displays[1].bounds().origin(), gfx::Point(0, 0));
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest012) {
  display::Displays displays = {
      display::Display(0, gfx::Rect(100, 0, 100, 100)),
      display::Display(1, gfx::Rect(-100, 0, 100, 100))};
  screen_->LayoutDisplays(displays);
  EXPECT_EQ(displays[0].bounds().origin(), gfx::Point(100, 0));
  EXPECT_EQ(displays[1].bounds().origin(), gfx::Point(-100, 0));
}

TEST_F(HeadlessScreenOhosTest, DisplayHeadlessScreenTest013) {
  display::Displays displays = {
      display::Display(0, gfx::Rect(0, 0, 100, 3000)),
      display::Display(1, gfx::Rect(1000, 0, 1000, 2000)),
      display::Display(2, gfx::Rect(1000, 0, 2000, 2000)),
      display::Display(3, gfx::Rect(2000, 3000, 1000, 1000)),
      display::Display(4, gfx::Rect(3000, -2000, 1000, 1000)),
      display::Display(5, gfx::Rect(3000, 2000, 1000, 1000)),
      display::Display(6, gfx::Rect(0, 1000, 2000, 2000)),
      display::Display(7, gfx::Rect(500, 1000, 2000, 1000)),
      display::Display(8, gfx::Rect(-400, 3000, 1000, 1000)),
      display::Display(9, gfx::Rect(1000, 1000, 1000, 1000)),
      display::Display(10, gfx::Rect(-2000, 2000, 1000, 1000))};
  screen_->LayoutDisplays(displays);
  EXPECT_EQ(displays[0].bounds().origin(), gfx::Point(0, 0));
  EXPECT_EQ(displays[1].bounds().origin(), gfx::Point(4100, 0));
  EXPECT_EQ(displays[2].bounds().origin(), gfx::Point(2100, 0));
  EXPECT_EQ(displays[7].bounds().origin(), gfx::Point(2100, 2000));
  EXPECT_EQ(displays[8].bounds().origin(), gfx::Point(-400, 3000));
  EXPECT_EQ(displays[9].bounds().origin(), gfx::Point(2100, 3000));
  EXPECT_EQ(displays[10].bounds().origin(), gfx::Point(-2000, 2000));
}
} // namespace ui