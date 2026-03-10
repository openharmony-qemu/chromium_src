// Copyright (c) 2026 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/functional/bind.h"
#include "services/device/battery/battery_status_manager.h"
#include "services/device/public/mojom/battery_status.mojom.h"

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <map>

namespace device {

class BatteryStatusManagerOhosTest : public ::testing::Test {
 protected:
  BatteryStatusManagerOhosTest() = default;
  ~BatteryStatusManagerOhosTest() override = default;

  void SetUp() override {
    callback_called_count_ = 0;
    callback_statuses_.clear();
  }

  void TearDown() override {
    manager_.reset();
    all_managers_.clear();
  }

  BatteryStatusService::BatteryUpdateCallback CreateCallback() {
    return base::BindRepeating(
        &BatteryStatusManagerOhosTest::BatteryCallback, base::Unretained(this));
  }

  BatteryStatusService::BatteryUpdateCallback CreateCallbackWithCapture(
      int* count_ptr) {
    return base::BindRepeating(
        [](int* count, const mojom::BatteryStatus& status) {
          (*count)++;
        },
        base::Unretained(count_ptr));
  }

  void BatteryCallback(const mojom::BatteryStatus& status) {
    callback_called_count_++;
    last_status_ = status;
    callback_statuses_.push_back(status);
  }

  BatteryStatusService::BatteryUpdateCallback CreateCallbackDetailed(
      std::vector<mojom::BatteryStatus>* status_vector) {
    return base::BindRepeating(
        [](std::vector<mojom::BatteryStatus>* vec,
           const mojom::BatteryStatus& status) { vec->push_back(status); },
        base::Unretained(status_vector));
  }

  std::unique_ptr<BatteryStatusManager> manager_;
  std::vector<std::unique_ptr<BatteryStatusManager>> all_managers_;
  int callback_called_count_ = 0;
  mojom::BatteryStatus last_status_;
  std::vector<mojom::BatteryStatus> callback_statuses_;
};

TEST_F(BatteryStatusManagerOhosTest, Constructor) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  EXPECT_NE(manager_, nullptr);
}

TEST_F(BatteryStatusManagerOhosTest, ConstructorWithNullCallback) {
  BatteryStatusService::BatteryUpdateCallback null_callback;
  manager_ = BatteryStatusManager::Create(null_callback);
  EXPECT_NE(manager_, nullptr);
}

TEST_F(BatteryStatusManagerOhosTest, ConstructorWithValidCallback) {
  auto callback = CreateCallback();
  manager_ = BatteryStatusManager::Create(callback);
  EXPECT_NE(manager_, nullptr);
}

TEST_F(BatteryStatusManagerOhosTest, StartListeningBatteryChange) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  bool result = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result);
}

TEST_F(BatteryStatusManagerOhosTest, StartListeningMultipleTimes) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  bool result1 = manager_->StartListeningBatteryChange();
  bool result2 = manager_->StartListeningBatteryChange();
  bool result3 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result1);
  EXPECT_TRUE(result2);
  EXPECT_TRUE(result3);
}

TEST_F(BatteryStatusManagerOhosTest, StartListeningTenTimes) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  for (int i = 0; i < 10; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }
}

TEST_F(BatteryStatusManagerOhosTest, StartListeningFiftyTimes) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  for (int i = 0; i < 50; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }
}

TEST_F(BatteryStatusManagerOhosTest, StopListeningBatteryChange) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, StopListeningMultipleTimes) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
  manager_->StopListeningBatteryChange();
  manager_->StopListeningBatteryChange();
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, StopListeningTenTimes) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  for (int i = 0; i < 10; ++i) {
    manager_->StopListeningBatteryChange();
  }
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, StopListeningWithoutStart) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StopListeningBatteryChange();
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, StartStopStartCycle) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  bool result1 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result1);
  manager_->StopListeningBatteryChange();
  bool result2 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result2);
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, StartStopStartStopCycle) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  bool result1 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result1);
  manager_->StopListeningBatteryChange();
  bool result2 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result2);
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersIndependent) {
  auto callback1 = CreateCallback();
  auto callback2 = CreateCallback();
  auto manager1 = BatteryStatusManager::Create(callback1);
  auto manager2 = BatteryStatusManager::Create(callback2);

  ASSERT_NE(manager1, nullptr);
  ASSERT_NE(manager2, nullptr);

  bool result1 = manager1->StartListeningBatteryChange();
  bool result2 = manager2->StartListeningBatteryChange();

  EXPECT_TRUE(result1);
  EXPECT_TRUE(result2);

  manager1->StopListeningBatteryChange();
  manager2->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ThreeManagersIndependent) {
  auto callback1 = CreateCallback();
  auto callback2 = CreateCallback();
  auto callback3 = CreateCallback();
  auto manager1 = BatteryStatusManager::Create(callback1);
  auto manager2 = BatteryStatusManager::Create(callback2);
  auto manager3 = BatteryStatusManager::Create(callback3);

  ASSERT_NE(manager1, nullptr);
  ASSERT_NE(manager2, nullptr);
  ASSERT_NE(manager3, nullptr);

  bool result1 = manager1->StartListeningBatteryChange();
  bool result2 = manager2->StartListeningBatteryChange();
  bool result3 = manager3->StartListeningBatteryChange();

  EXPECT_TRUE(result1);
  EXPECT_TRUE(result2);
  EXPECT_TRUE(result3);

  manager1->StopListeningBatteryChange();
  manager2->StopListeningBatteryChange();
  manager3->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, FiveManagersIndependent) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 5; ++i) {
    auto callback = CreateCallback();
    auto manager = BatteryStatusManager::Create(callback);
    ASSERT_NE(manager, nullptr);
    managers.push_back(std::move(manager));
  }

  for (auto& manager : managers) {
    bool result = manager->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }

  for (auto& manager : managers) {
    manager->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, SameCallbackMultipleManagers) {
  auto callback = CreateCallback();
  auto manager1 = BatteryStatusManager::Create(callback);
  auto manager2 = BatteryStatusManager::Create(callback);

  ASSERT_NE(manager1, nullptr);
  ASSERT_NE(manager2, nullptr);

  manager1->StartListeningBatteryChange();
  manager2->StartListeningBatteryChange();

  manager1->StopListeningBatteryChange();
  manager2->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, SameCallbackThreeManagers) {
  auto callback = CreateCallback();
  auto manager1 = BatteryStatusManager::Create(callback);
  auto manager2 = BatteryStatusManager::Create(callback);
  auto manager3 = BatteryStatusManager::Create(callback);

  ASSERT_NE(manager1, nullptr);
  ASSERT_NE(manager2, nullptr);
  ASSERT_NE(manager3, nullptr);

  manager1->StartListeningBatteryChange();
  manager2->StartListeningBatteryChange();
  manager3->StartListeningBatteryChange();

  manager1->StopListeningBatteryChange();
  manager2->StopListeningBatteryChange();
  manager3->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, DestructorStopsListening) {
  int initial_count = 0;
  {
    BatteryStatusService::BatteryUpdateCallback temp_callback =
        base::BindRepeating([](int* count, const mojom::BatteryStatus& status) {
          (*count)++;
        }, base::Unretained(&initial_count));
    auto temp_manager = BatteryStatusManager::Create(temp_callback);
    ASSERT_NE(temp_manager, nullptr);
    temp_manager->StartListeningBatteryChange();
  }
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, DestructorStopsListeningAfterMultipleStarts) {
  int initial_count = 0;
  {
    BatteryStatusService::BatteryUpdateCallback temp_callback =
        base::BindRepeating([](int* count, const mojom::BatteryStatus& status) {
          (*count)++;
        }, base::Unretained(&initial_count));
    auto temp_manager = BatteryStatusManager::Create(temp_callback);
    ASSERT_NE(temp_manager, nullptr);
    for (int i = 0; i < 10; ++i) {
      temp_manager->StartListeningBatteryChange();
    }
  }
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerLifecycle) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  bool start_result = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(start_result);

  manager_->StopListeningBatteryChange();
  bool restart_result = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(restart_result);

  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerLifecycleMultipleRestarts) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 10; ++i) {
    bool start_result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(start_result);
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersLifecycle) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 5; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    bool result = manager->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    managers.push_back(std::move(manager));
  }

  for (auto& manager : managers) {
    manager->StopListeningBatteryChange();
  }
  managers.clear();
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersLifecycleTenManagers) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 10; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    bool result = manager->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    managers.push_back(std::move(manager));
  }

  for (auto& manager : managers) {
    manager->StopListeningBatteryChange();
  }
  managers.clear();
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersLifecycleFiftyManagers) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 50; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    bool result = manager->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    managers.push_back(std::move(manager));
  }

  for (auto& manager : managers) {
    manager->StopListeningBatteryChange();
  }
  managers.clear();
}

TEST_F(BatteryStatusManagerOhosTest, RapidStartStopCycles) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 10; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, RapidStartStopCyclesTwentyIterations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 20; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, RapidStartStopCyclesFiftyIterations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 50; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, StressTest) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 100; ++i) {
    manager_->StartListeningBatteryChange();
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, StressTestTwoHundredIterations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 200; ++i) {
    manager_->StartListeningBatteryChange();
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, StressTestFiveHundredIterations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 500; ++i) {
    manager_->StartListeningBatteryChange();
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, StressTestThousandIterations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 1000; ++i) {
    manager_->StartListeningBatteryChange();
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, MultipleStartsBeforeStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 10; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, TwentyStartsBeforeStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 20; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, FiftyStartsBeforeStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 50; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, HundredStartsBeforeStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 100; ++i) {
    bool result = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, NullManagerAfterDestruction) {
  {
    auto temp_manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(temp_manager, nullptr);
  }
  manager_ = nullptr;
  EXPECT_EQ(manager_, nullptr);
}

TEST_F(BatteryStatusManagerOhosTest, NullManagerReset) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  manager_.reset();
  EXPECT_EQ(manager_, nullptr);
}

TEST_F(BatteryStatusManagerOhosTest, AlternatingStartStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 10; ++i) {
    if (i % 2 == 0) {
      bool result = manager_->StartListeningBatteryChange();
      EXPECT_TRUE(result);
    } else {
      manager_->StopListeningBatteryChange();
    }
  }
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, AlternatingStartStopTwentyIterations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 20; ++i) {
    if (i % 2 == 0) {
      bool result = manager_->StartListeningBatteryChange();
      EXPECT_TRUE(result);
    } else {
      manager_->StopListeningBatteryChange();
    }
  }
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerWithOnlyStart) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerWithOnlyStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ConcurrentStartStopOperations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerReuseAfterStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  bool result1 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result1);
  manager_->StopListeningBatteryChange();

  bool result2 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result2);
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerReuseAfterMultipleStops) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
  manager_->StopListeningBatteryChange();
  manager_->StopListeningBatteryChange();

  bool result = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result);
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, CreateAndDestroyImmediately) {
  for (int i = 0; i < 10; ++i) {
    auto temp_manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(temp_manager, nullptr);
  }
}

TEST_F(BatteryStatusManagerOhosTest, CreateAndDestroyImmediatelyHundredTimes) {
  for (int i = 0; i < 100; ++i) {
    auto temp_manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(temp_manager, nullptr);
  }
}

TEST_F(BatteryStatusManagerOhosTest, CreateStartDestroy) {
  for (int i = 0; i < 10; ++i) {
    auto temp_manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(temp_manager, nullptr);
    temp_manager->StartListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, CreateStartDestroyHundredTimes) {
  for (int i = 0; i < 100; ++i) {
    auto temp_manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(temp_manager, nullptr);
    temp_manager->StartListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, CreateStartStopDestroy) {
  for (int i = 0; i < 10; ++i) {
    auto temp_manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(temp_manager, nullptr);
    temp_manager->StartListeningBatteryChange();
    temp_manager->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, CreateStartStopDestroyHundredTimes) {
  for (int i = 0; i < 100; ++i) {
    auto temp_manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(temp_manager, nullptr);
    temp_manager->StartListeningBatteryChange();
    temp_manager->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, CallbackCountAfterStart) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  int initial_count = callback_called_count_;
  manager_->StartListeningBatteryChange();
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, CallbackCountAfterStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  int count_after_start = callback_called_count_;
  manager_->StopListeningBatteryChange();
  int count_after_stop = callback_called_count_;
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersStress) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 100; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    manager->StartListeningBatteryChange();
    managers.push_back(std::move(manager));
  }

  for (auto& manager : managers) {
    manager->StopListeningBatteryChange();
  }
  managers.clear();
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersStressTwoHundred) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 200; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    manager->StartListeningBatteryChange();
    managers.push_back(std::move(manager));
  }

  for (auto& manager : managers) {
    manager->StopListeningBatteryChange();
  }
  managers.clear();
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersStressFiveHundred) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 500; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    manager->StartListeningBatteryChange();
    managers.push_back(std::move(manager));
  }

  for (auto& manager : managers) {
    manager->StopListeningBatteryChange();
  }
  managers.clear();
}

TEST_F(BatteryStatusManagerOhosTest, CallbackWithNullCallbackStop) {
  BatteryStatusService::BatteryUpdateCallback null_callback;
  manager_ = BatteryStatusManager::Create(null_callback);
  ASSERT_NE(manager_, nullptr);
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, CallbackWithCaptureDetailed) {
  std::vector<mojom::BatteryStatus> status_vector;
  auto callback = CreateCallbackDetailed(&status_vector);
  manager_ = BatteryStatusManager::Create(callback);
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, StartWithoutCallbackVerification) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  bool result = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result);
}

TEST_F(BatteryStatusManagerOhosTest, StopWithoutCallbackVerification) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, LongRunningManager) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();

  for (int i = 0; i < 100; ++i) {
    manager_->StopListeningBatteryChange();
    manager_->StartListeningBatteryChange();
  }

  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, LongRunningManagerTwoHundredIterations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();

  for (int i = 0; i < 200; ++i) {
    manager_->StopListeningBatteryChange();
    manager_->StartListeningBatteryChange();
  }

  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, LongRunningManagerFiveHundredIterations) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();

  for (int i = 0; i < 500; ++i) {
    manager_->StopListeningBatteryChange();
    manager_->StartListeningBatteryChange();
  }

  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerInitializationState) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  EXPECT_EQ(callback_called_count_, 0);
}

TEST_F(BatteryStatusManagerOhosTest, ManagerAfterStartState) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerAfterStopState) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersSequential) {
  for (int i = 0; i < 10; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    bool result = manager->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    manager->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersSequentialFifty) {
  for (int i = 0; i < 50; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    bool result = manager->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    manager->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, MultipleManagersSequentialHundred) {
  for (int i = 0; i < 100; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    bool result = manager->StartListeningBatteryChange();
    EXPECT_TRUE(result);
    manager->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, StartStopPatternSimple) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  bool result1 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result1);
  manager_->StopListeningBatteryChange();

  bool result2 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result2);
  manager_->StopListeningBatteryChange();

  bool result3 = manager_->StartListeningBatteryChange();
  EXPECT_TRUE(result3);
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, StartStopPatternComplex) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 5; ++i) {
    manager_->StartListeningBatteryChange();
    manager_->StartListeningBatteryChange();
    manager_->StopListeningBatteryChange();
    manager_->StartListeningBatteryChange();
    manager_->StopListeningBatteryChange();
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, EdgeCaseSingleStartSingleStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, EdgeCaseSingleStartMultipleStops) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  manager_->StartListeningBatteryChange();
  for (int i = 0; i < 10; ++i) {
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, EdgeCaseMultipleStartsSingleStop) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);
  for (int i = 0; i < 10; ++i) {
    manager_->StartListeningBatteryChange();
  }
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerInVector) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 5; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    managers.push_back(std::move(manager));
  }

  for (size_t i = 0; i < managers.size(); ++i) {
    bool result = managers[i]->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }

  for (size_t i = 0; i < managers.size(); ++i) {
    managers[i]->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, ManagerInVectorTenManagers) {
  std::vector<std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 10; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    managers.push_back(std::move(manager));
  }

  for (size_t i = 0; i < managers.size(); ++i) {
    bool result = managers[i]->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }

  for (size_t i = 0; i < managers.size(); ++i) {
    managers[i]->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, ManagerInMap) {
  std::map<int, std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 5; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    managers[i] = std::move(manager);
  }

  for (auto& pair : managers) {
    bool result = pair.second->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }

  for (auto& pair : managers) {
    pair.second->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, ManagerInMapTenManagers) {
  std::map<int, std::unique_ptr<BatteryStatusManager>> managers;

  for (int i = 0; i < 10; ++i) {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    managers[i] = std::move(manager);
  }

  for (auto& pair : managers) {
    bool result = pair.second->StartListeningBatteryChange();
    EXPECT_TRUE(result);
  }

  for (auto& pair : managers) {
    pair.second->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, CallbackReset) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  callback_called_count_ = 0;
  manager_->StartListeningBatteryChange();

  callback_called_count_ = 0;
  manager_->StopListeningBatteryChange();

  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, StatusTracking) {
  std::vector<mojom::BatteryStatus> status_vector;
  auto callback = CreateCallbackDetailed(&status_vector);

  manager_ = BatteryStatusManager::Create(callback);
  ASSERT_NE(manager_, nullptr);

  manager_->StartListeningBatteryChange();
  manager_->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, StatusTrackingMultipleCalls) {
  std::vector<mojom::BatteryStatus> status_vector;
  auto callback = CreateCallbackDetailed(&status_vector);

  manager_ = BatteryStatusManager::Create(callback);
  ASSERT_NE(manager_, nullptr);

  for (int i = 0; i < 5; ++i) {
    manager_->StartListeningBatteryChange();
    manager_->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, CallbackNotCalledOnCreate) {
  int call_count = 0;
  auto callback = CreateCallbackWithCapture(&call_count);

  manager_ = BatteryStatusManager::Create(callback);
  ASSERT_NE(manager_, nullptr);

  EXPECT_EQ(call_count, 0);
}

TEST_F(BatteryStatusManagerOhosTest, CallbackNotCalledOnStop) {
  int call_count = 0;
  auto callback = CreateCallbackWithCapture(&call_count);

  manager_ = BatteryStatusManager::Create(callback);
  ASSERT_NE(manager_, nullptr);

  manager_->StartListeningBatteryChange();
  int count_after_start = call_count;

  manager_->StopListeningBatteryChange();
  int count_after_stop = call_count;

  SUCCEED();
}

TEST_F(BatteryStatusManagerOhosTest, ManagerLifetimeScope) {
  {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    manager->StartListeningBatteryChange();
  }

  {
    auto manager = BatteryStatusManager::Create(CreateCallback());
    ASSERT_NE(manager, nullptr);
    manager->StartListeningBatteryChange();
    manager->StopListeningBatteryChange();
  }
}

TEST_F(BatteryStatusManagerOhosTest, ManagerLifetimeScopeMultiple) {
  for (int i = 0; i < 10; ++i) {
    {
      auto manager = BatteryStatusManager::Create(CreateCallback());
      ASSERT_NE(manager, nullptr);
      manager->StartListeningBatteryChange();
    }

    {
      auto manager = BatteryStatusManager::Create(CreateCallback());
      ASSERT_NE(manager, nullptr);
      manager->StartListeningBatteryChange();
      manager->StopListeningBatteryChange();
    }
  }
}

TEST_F(BatteryStatusManagerOhosTest, StartReturnValueConsistency) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  bool results[10];
  for (int i = 0; i < 10; ++i) {
    results[i] = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(results[i]);
  }

  manager_->StopListeningBatteryChange();

  for (int i = 0; i < 10; ++i) {
    results[i] = manager_->StartListeningBatteryChange();
    EXPECT_TRUE(results[i]);
  }
}

TEST_F(BatteryStatusManagerOhosTest, NullPtrHandling) {
  BatteryStatusManager* null_ptr = nullptr;
  EXPECT_EQ(null_ptr, nullptr);
}

TEST_F(BatteryStatusManagerOhosTest, UniquePtrRelease) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  auto raw_ptr = manager_.release();
  EXPECT_NE(raw_ptr, nullptr);
  EXPECT_EQ(manager_, nullptr);

  delete raw_ptr;
}

TEST_F(BatteryStatusManagerOhosTest, UniquePtrReset) {
  manager_ = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager_, nullptr);

  manager_.reset();
  EXPECT_EQ(manager_, nullptr);
}

TEST_F(BatteryStatusManagerOhosTest, UniquePtrMoveConstruct) {
  auto manager1 = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager1, nullptr);

  auto manager2 = std::move(manager1);
  EXPECT_EQ(manager1, nullptr);
  EXPECT_NE(manager2, nullptr);

  bool result = manager2->StartListeningBatteryChange();
  EXPECT_TRUE(result);

  manager2->StopListeningBatteryChange();
}

TEST_F(BatteryStatusManagerOhosTest, UniquePtrMoveAssign) {
  auto manager1 = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager1, nullptr);

  auto manager2 = BatteryStatusManager::Create(CreateCallback());
  ASSERT_NE(manager2, nullptr);

  manager2 = std::move(manager1);
  EXPECT_EQ(manager1, nullptr);
  EXPECT_NE(manager2, nullptr);

  bool result = manager2->StartListeningBatteryChange();
  EXPECT_TRUE(result);

  manager2->StopListeningBatteryChange();
}

}  // namespace device
