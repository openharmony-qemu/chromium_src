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
#include "ui/ozone/platform/headless/headless_screen_ohos.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
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

int main(int argc, char** argv) {
  base::TestSuite test_suite(argc, argv);
  mojo::core::Init(mojo::core::Configuration());

  return base::LaunchUnitTestsSerially(
      argc, argv,
      base::BindOnce(&base::TestSuite::Run, base::Unretained(&test_suite)));
}

namespace ui {

using ::testing::_;

namespace {
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

} // namespace ui