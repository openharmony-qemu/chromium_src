// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/battery/battery_status_manager_ohos.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace device {

namespace {

class BatteryStatusManagerOhosTest : public testing::Test {
 public:
  BatteryStatusManagerOhosTest() {}

  BatteryStatusManagerOhosTest(const BatteryStatusManagerOhosTest&) = delete;
  BatteryStatusManagerOhosTest& operator=(const BatteryStatusManagerOhosTest&) =
      delete;
};

TEST(BatteryStatusManagerOhosTest, NoBattery) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.battery_soc = 100;
  battery_info.is_battery_present = false;
  battery_info.estimated_remaining_charge_time = 0;
  battery_info.now_current = 0;
  battery_info.remaining_energy = 200;

  mojom::BatteryStatus default_status;
  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_EQ(default_status.charging, status.charging);
  EXPECT_EQ(default_status.charging_time, status.charging_time);
  EXPECT_EQ(default_status.discharging_time, status.discharging_time);
  EXPECT_EQ(default_status.level, status.level);
}

TEST(BatteryStatusManagerOhosTest, ChargingStatusNone) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_NONE;
  battery_info.battery_soc = 100;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time =
      std::numeric_limits<double>::infinity();
  battery_info.now_current = 1;
  battery_info.remaining_energy = 200;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_FALSE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(200, status.discharging_time);
  EXPECT_EQ(1, status.level);
}

TEST(BatteryStatusManagerOhosTest, ChargingStatusNoneDischargingTimeUnknown) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_NONE;
  battery_info.battery_soc = 100;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time =
      std::numeric_limits<double>::infinity();
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 200;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_FALSE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(1, status.level);
}

TEST(BatteryStatusManagerOhosTest, ChargingStatusEnable) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_ENABLE;
  battery_info.battery_soc = 0;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time = 100;
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 0;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(100, status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(0, status.level);
}

TEST(BatteryStatusManagerOhosTest, ChargingStatusFull) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_FULL;
  battery_info.battery_soc = 100;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time =
      ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.now_current = 0;
  battery_info.remaining_energy = 200;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_FALSE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(1, status.level);
}

TEST(BatteryStatusManagerOhosTest, DeviceStateUnknown) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_BUTT;
  battery_info.battery_soc = 0;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time =
      std::numeric_limits<double>::infinity();
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 0;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(0, status.level);
}

TEST(BatteryStatusManagerOhosTest, ChargingHalfFull) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_ENABLE;
  battery_info.battery_soc = 50;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time =
      std::numeric_limits<double>::infinity();
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 100;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);
}

TEST(BatteryStatusManagerOhosTest, UpdateChargingStatus) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_ENABLE;
  battery_info.battery_soc = 50;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time = 100;
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 100;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(100, status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);

  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_NONE;

  status = ComputeWebBatteryStatus(battery_info);
  EXPECT_FALSE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);
}

TEST(BatteryStatusManagerOhosTest, UpdateBatterySOC) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_ENABLE;
  battery_info.battery_soc = 100;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time = 0;
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 200;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(0, status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(1, status.level);

  battery_info.battery_soc = 50;
  battery_info.remaining_energy = 100;

  status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(0, status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);
}

TEST(BatteryStatusManagerOhosTest, UpdateEstimatedRemainingChargeTime) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_ENABLE;
  battery_info.battery_soc = 50;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time = 100;
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 100;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(100, status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);

  battery_info.estimated_remaining_charge_time = 50;

  status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(50, status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);
}

TEST(BatteryStatusManagerOhosTest, UpdateNowCurrent) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_NONE;
  battery_info.battery_soc = 100;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time =
      ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.now_current = 200;
  battery_info.remaining_energy = 200;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_FALSE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(1, status.discharging_time);
  EXPECT_EQ(1, status.level);

  battery_info.now_current = 100;

  status = ComputeWebBatteryStatus(battery_info);
  EXPECT_FALSE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(2, status.discharging_time);
  EXPECT_EQ(1, status.level);
}

TEST(BatteryStatusManagerOhosTest, UpdateRemainingEnergy) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_ENABLE;
  battery_info.battery_soc = 50;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time = 0;
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 100;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(0, status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);

  battery_info.remaining_energy = 50;

  status = ComputeWebBatteryStatus(battery_info);
  EXPECT_TRUE(status.charging);
  EXPECT_EQ(0, status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);
}

TEST(BatteryStatusManagerOhosTest, ChargingStatusDisable) {
  ohos::adapter::battery::BatteryInfo battery_info;
  battery_info.charging_status =
      ohos::adapter::battery::BatteryChargeState::CHARGE_STATE_DISABLE;
  battery_info.battery_soc = 50;
  battery_info.is_battery_present = true;
  battery_info.estimated_remaining_charge_time =
      std::numeric_limits<double>::infinity();
  battery_info.now_current = ohos::adapter::battery::INVALID_BATT_INT_VALUE;
  battery_info.remaining_energy = 100;

  mojom::BatteryStatus status = ComputeWebBatteryStatus(battery_info);
  EXPECT_FALSE(status.charging);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.charging_time);
  EXPECT_EQ(std::numeric_limits<double>::infinity(), status.discharging_time);
  EXPECT_EQ(.5, status.level);
}

}  // namespace

}  // namespace device
