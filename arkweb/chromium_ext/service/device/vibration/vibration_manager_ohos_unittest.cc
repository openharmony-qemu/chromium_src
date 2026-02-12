/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "services/device/vibration/vibration_manager_ohos.h"

using namespace device;
using namespace testing;

class VibrationManagerOhosTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(VibrationManagerOhosTest, VibrateSuccess) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(100);
}

TEST_F(VibrationManagerOhosTest, VibrateWithZeroDuration) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(0);
}

TEST_F(VibrationManagerOhosTest, VibrateWithNegativeDuration) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(-100);
}

TEST_F(VibrationManagerOhosTest, VibrateWithLongDuration) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(5000);
}

TEST_F(VibrationManagerOhosTest, VibrateWithMaxInt64Duration) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(INT64_MAX);
}

TEST_F(VibrationManagerOhosTest, (int64_t)Duration) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(5000);
}

TEST_F(VibrationManagerOhosTest, CancelSuccess) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformCancel();
}

TEST_F(VibrationManagerOhosTest, VibrateThenCancel) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(200);
  manager.PlatformCancel();
}

TEST_F(VibrationManagerOhosTest, MultipleVibrateCalls) {
  VibrationManagerOhos manager(nullptr);
  for (int i = 0; i < 10; i++) {
    manager.PlatformVibrate(50);
  }
}

TEST_F(VibrationManagerOhosTest, MultipleCancelCalls) {
  VibrationManagerOhos manager(nullptr);
  for (int i = 0; i < 5; i++) {
    manager.PlatformCancel();
  }
}

TEST_F(VibrationManagerOhosTest, CancelThenVibrate) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformCancel();
  manager.PlatformVibrate(100);
}

TEST_F(VibrationManagerOhosTest, VibrateCancelCycle) {
  VibrationManagerOhos manager(nullptr);
  for (int i = 0; i < 5; i++) {
    manager.PlatformVibrate(50 + i * 10);
    manager.PlatformCancel();
  }
}

TEST_F(VibrationManagerOhosTest, LongDurationVibration) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(10000);
}

TEST_F(VibrationManagerOhosTest, VeryShortDurationVibration) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(5);
}

TEST_F(VibrationManagerOhosTest, MultipleVibrateCancelSequence) {
  VibrationManagerOhos manager(nullptr);
  manager.PlatformVibrate(100);
  manager.PlatformCancel();
  manager.PlatformVibrate(200);
  manager.PlatformCancel();
}

TEST_F(VibrationManagerOhosTest, RapidCancelVibrate) {
  VibrationManagerOhos manager(nullptr);
  for (int i = 0; i < 10; i++) {
    manager.PlatformVibrate(10);
    manager.PlatformCancel();
  }
}
