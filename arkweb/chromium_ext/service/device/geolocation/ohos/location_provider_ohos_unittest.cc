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

#include "arkweb/chromium_ext/service/device/geolocation/ohos/location_provider_ohos.h"

#include <memory>
#include <vector>

#include "base/functional/bind.h"
#include "base/functional/callback_forward.h"
#include "base/memory/scoped_refptr.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace device {

namespace {

using ::testing::_;
using ::testing::ByMove;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::SaveArg;

class MockLocationInfo : public OHOS::NWeb::LocationInfo {
 public:
  MockLocationInfo() = default;
  ~MockLocationInfo() override = default;

  MOCK_METHOD(double, GetLatitude, (), (override));
  MOCK_METHOD(double, GetLongitude, (), (override));
  MOCK_METHOD(double, GetAltitude, (), (override));
  MOCK_METHOD(float, GetAccuracy, (), (override));
  MOCK_METHOD(float, GetSpeed, (), (override));
  MOCK_METHOD(double, GetDirection, (), (override));
  MOCK_METHOD(int64_t, GetTimeStamp, (), (override));
  MOCK_METHOD(int64_t, GetTimeSinceBoot, (), (override));
  MOCK_METHOD(std::vector<std::string>, GetAdditions, (), (override));
};

class MockLocationRequestConfig : public OHOS::NWeb::LocationRequestConfig {
 public:
  MockLocationRequestConfig() = default;
  ~MockLocationRequestConfig() override = default;

  MOCK_METHOD(void, SetScenario, (int32_t scenario), (override));
  MOCK_METHOD(void, SetFixNumber, (int32_t number), (override));
  MOCK_METHOD(void, SetMaxAccuracy, (int32_t maxAccuracy), (override));
  MOCK_METHOD(void, SetDistanceInterval, (int32_t disInterval), (override));
  MOCK_METHOD(void, SetTimeInterval, (int32_t timeInterval), (override));
  MOCK_METHOD(void, SetPriority, (int32_t priority), (override));
};

class MockLocationProxyAdapter : public OHOS::NWeb::LocationProxyAdapter {
 public:
  MockLocationProxyAdapter() = default;
  ~MockLocationProxyAdapter() override = default;

  MOCK_METHOD(int32_t, StartLocating,
              (std::shared_ptr<OHOS::NWeb::LocationRequestConfig> requestConfig,
               std::shared_ptr<OHOS::NWeb::LocationCallbackAdapter> callback),
              (override));
  MOCK_METHOD(bool, StopLocating, (int32_t callbackId), (override));
  MOCK_METHOD(bool, EnableAbility, (bool isEnabled), (override));
  MOCK_METHOD(bool, IsLocationEnabled, (), (override));
};

class MockLocationInstance : public OHOS::NWeb::LocationInstance {
 public:
  MockLocationInstance() = default;
  ~MockLocationInstance() override = default;

  MOCK_METHOD(std::shared_ptr<OHOS::NWeb::LocationProxyAdapter>,
              CreateLocationProxyAdapter,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<OHOS::NWeb::LocationRequestConfig>,
              CreateLocationRequestConfig,
              (),
              (override));
};

}  // namespace

class LocationProviderOhosTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_location_proxy_ = std::make_shared<MockLocationProxyAdapter>();
    mock_location_instance_ = std::make_shared<MockLocationInstance>();
  }

  void TearDown() override {
    provider_.reset();
    mock_location_proxy_.reset();
    mock_location_instance_.reset();
  }

  base::test::TaskEnvironment task_environment_{
      base::test::TaskEnvironment::TimeSource::MOCK_TIME};

  std::unique_ptr<LocationProviderOhos> provider_;
  std::shared_ptr<MockLocationProxyAdapter> mock_location_proxy_;
  std::shared_ptr<MockLocationInstance> mock_location_instance_;
};

TEST_F(LocationProviderOhosTest, Constructor) {
  provider_ = std::make_unique<LocationProviderOhos>();
  ASSERT_NE(provider_, nullptr);
}

TEST_F(LocationProviderOhosTest, SetUpdateCallback) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating([](const LocationProvider* provider,
                             mojom::GeopositionResultPtr position) {
      });

  provider_->SetUpdateCallback(callback);
}

TEST_F(LocationProviderOhosTest, GetPositionBeforeStart) {
  provider_ = std::make_unique<LocationProviderOhos>();
  const mojom::GeopositionResult* position = provider_->GetPosition();
  EXPECT_EQ(position, nullptr);
}

TEST_F(LocationProviderOhosTest, OnPermissionGranted) {
  provider_ = std::make_unique<LocationProviderOhos>();
  provider_->OnPermissionGranted();
}

TEST_F(LocationProviderOhosTest, FillDiagnostics) {
  provider_ = std::make_unique<LocationProviderOhos>();

  mojom::GeolocationDiagnostics diagnostics;
  provider_->FillDiagnostics(diagnostics);

  EXPECT_EQ(diagnostics.provider_state,
            mojom::GeolocationDiagnostics::ProviderState::kStopped);
}

TEST_F(LocationProviderOhosTest, StartProviderHighAccuracy) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating([](const LocationProvider* provider,
                             mojom::GeopositionResultPtr position) {
      });

  provider_->SetUpdateCallback(callback);
  provider_->StartProvider(true);

  task_environment_.RunUntilIdle();
}

TEST_F(LocationProviderOhosTest, StartProviderLowAccuracy) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating([](const LocationProvider* provider,
                             mojom::GeopositionResultPtr position) {
      });

  provider_->SetUpdateCallback(callback);
  provider_->StartProvider(false);

  task_environment_.RunUntilIdle();
}

TEST_F(LocationProviderOhosTest, StopProvider) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating([](const LocationProvider* provider,
                             mojom::GeopositionResultPtr position) {
      });

  provider_->SetUpdateCallback(callback);
  provider_->StartProvider(true);
  provider_->StopProvider();

  task_environment_.RunUntilIdle();
}

TEST_F(LocationProviderOhosTest, StartStopStartProvider) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating([](const LocationProvider* provider,
                             mojom::GeopositionResultPtr position) {
      });

  provider_->SetUpdateCallback(callback);
  provider_->StartProvider(true);
  provider_->StopProvider();
  provider_->StartProvider(false);

  task_environment_.RunUntilIdle();
}

TEST_F(LocationProviderOhosTest, MultipleStartProviderCalls) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating([](const LocationProvider* provider,
                             mojom::GeopositionResultPtr position) {
      });

  provider_->SetUpdateCallback(callback);
  provider_->StartProvider(true);
  provider_->StartProvider(false);
  provider_->StartProvider(true);

  task_environment_.RunUntilIdle();
}

TEST_F(LocationProviderOhosTest, MultipleStopProviderCalls) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating([](const LocationProvider* provider,
                             mojom::GeopositionResultPtr position) {
      });

  provider_->SetUpdateCallback(callback);
  provider_->StartProvider(true);
  provider_->StopProvider();
  provider_->StopProvider();
  provider_->StopProvider();

  task_environment_.RunUntilIdle();
}

TEST_F(LocationProviderOhosTest, ProviderUpdateCallbackNotCalledWhenNotRunning) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating(
          [](bool* callback_called, const LocationProvider* provider, mojom::GeopositionResultPtr position) {
            *callback_called = true;
          }, &callback_called);

  provider_->SetUpdateCallback(callback);

  auto position = mojom::Geoposition::New();
  position->latitude = 37.7749;
  position->longitude = -122.4194;

  provider_->ProviderUpdateCallback(
      mojom::GeopositionResult::NewPosition(std::move(position)));

  EXPECT_FALSE(callback_called);
}

TEST_F(LocationProviderOhosTest, ProviderUpdateCallbackNotCalledWhenCallbackIsNull) {
  provider_ = std::make_unique<LocationProviderOhos>();

  auto position = mojom::Geoposition::New();
  position->latitude = 37.7749;
  position->longitude = -122.4194;

  provider_->ProviderUpdateCallback(
      mojom::GeopositionResult::NewPosition(std::move(position)));
}

TEST_F(LocationProviderOhosTest, Destructor) {
  provider_ = std::make_unique<LocationProviderOhos>();

  bool callback_called = false;
  LocationProviderOhos::LocationProviderUpdateCallback callback =
      base::BindRepeating([](const LocationProvider* provider,
                             mojom::GeopositionResultPtr position) {
      });

  provider_->SetUpdateCallback(callback);
  provider_->StartProvider(true);

  task_environment_.RunUntilIdle();

  provider_.reset();
}

class LocationProviderCallbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    task_runner_ = base::MakeRefCounted<base::TestSimpleTaskRunner>();
    callback_ = std::make_shared<LocationProviderCallback>(task_runner_);
  }

  void TearDown() override {
    callback_.reset();
    task_runner_.reset();
  }

    std::shared_ptr<LocationProviderCallback> callback_;
  scoped_refptr<base::TestSimpleTaskRunner> task_runner_;
};

TEST_F(LocationProviderCallbackTest, Constructor) {
  ASSERT_NE(callback_, nullptr);
}

TEST_F(LocationProviderCallbackTest, SetUpdateCallback) {
  bool callback_called = false;
  LocationProviderCallback::UpdateCallback update_callback =
      base::BindRepeating(
          [](bool* callback_called, mojom::GeopositionResultPtr position) {
            *callback_called = true;
          }, &callback_called);

  callback_->SetUpdateCallback(update_callback);
}

TEST_F(LocationProviderCallbackTest, GetPositionBeforeUpdate) {
  const mojom::GeopositionResult* position = callback_->GetPosition();
  EXPECT_EQ(position, nullptr);
}

TEST_F(LocationProviderCallbackTest, OnNewLocationAvailableWithNullLocation) {
  bool callback_called = false;
  LocationProviderCallback::UpdateCallback update_callback =
      base::BindRepeating(
          [](bool* callback_called, mojom::GeopositionResultPtr position) {
            *callback_called = true;
          }, &callback_called);

  callback_->SetUpdateCallback(update_callback);
  callback_->OnNewLocationAvailable(nullptr);

  task_runner_->RunPendingTasks();

  EXPECT_FALSE(callback_called);
}

TEST_F(LocationProviderCallbackTest, OnNewLocationAvailableWithValidLocation) {
  auto mock_location = std::make_shared<MockLocationInfo>();
  EXPECT_CALL(*mock_location, GetLatitude()).WillOnce(Return(37.7749));
  EXPECT_CALL(*mock_location, GetLongitude()).WillOnce(Return(-122.4194));
  EXPECT_CALL(*mock_location, GetAltitude()).WillOnce(Return(100.0));
  EXPECT_CALL(*mock_location, GetAccuracy()).WillOnce(Return(10.0f));
  EXPECT_CALL(*mock_location, GetDirection()).WillOnce(Return(45.0));
  EXPECT_CALL(*mock_location, GetSpeed()).WillOnce(Return(5.0f));

  mojom::GeopositionResultPtr received_position;
  LocationProviderCallback::UpdateCallback update_callback =
      base::BindRepeating(
          [](mojom::GeopositionResultPtr* received_position, mojom::GeopositionResultPtr position) {
            *received_position = position.Clone();
          }, &received_position);

  callback_->SetUpdateCallback(update_callback);
  callback_->OnNewLocationAvailable(mock_location);

  task_runner_->RunPendingTasks();

  ASSERT_FALSE(received_position.is_null());
  ASSERT_TRUE(received_position->is_position());
  EXPECT_EQ(received_position->get_position()->latitude, 37.7749);
  EXPECT_EQ(received_position->get_position()->longitude, -122.4194);
  EXPECT_EQ(received_position->get_position()->altitude, 100.0);
  EXPECT_EQ(received_position->get_position()->accuracy, 10.0f);
  EXPECT_EQ(received_position->get_position()->heading, 45.0);
  EXPECT_EQ(received_position->get_position()->speed, 5.0f);
}

TEST_F(LocationProviderCallbackTest, OnNewErrorAvailable) {
  mojom::GeopositionResultPtr received_position;
  LocationProviderCallback::UpdateCallback update_callback =
      base::BindRepeating(
          [](mojom::GeopositionResultPtr* received_position, mojom::GeopositionResultPtr position) {
            *received_position = position.Clone();
          }, &received_position);

  callback_->SetUpdateCallback(update_callback);
  callback_->OnNewErrorAvailable("Test error message");

  task_runner_->RunPendingTasks();

  ASSERT_FALSE(received_position.is_null());
  ASSERT_TRUE(received_position->is_error());
  EXPECT_EQ(received_position->get_error()->error_code,
            mojom::GeopositionErrorCode::kPositionUnavailable);
  EXPECT_EQ(received_position->get_error()->error_message, "Test error message");
}

TEST_F(LocationProviderCallbackTest, OnErrorReportWithLocationGetFailed) {
  mojom::GeopositionResultPtr received_position;
  LocationProviderCallback::UpdateCallback update_callback =
      base::BindRepeating(
          [](mojom::GeopositionResultPtr* received_position, mojom::GeopositionResultPtr position) {
            *received_position = position.Clone();
          }, &received_position);

  callback_->SetUpdateCallback(update_callback);
  callback_->OnErrorReport(
      LocationProviderCallback::LOCATION_GET_FAILED);

  task_runner_->RunPendingTasks();

  ASSERT_FALSE(received_position.is_null());
  ASSERT_TRUE(received_position->is_error());
  EXPECT_EQ(received_position->get_error()->error_code,
            mojom::GeopositionErrorCode::kPositionUnavailable);
  EXPECT_EQ(received_position->get_error()->error_message,
            "Failed to get location!");
}

TEST_F(LocationProviderCallbackTest, OnErrorReportWithUnknownError) {
  mojom::GeopositionResultPtr received_position;
  LocationProviderCallback::UpdateCallback update_callback =
      base::BindRepeating(
          [](mojom::GeopositionResultPtr* received_position, mojom::GeopositionResultPtr position) {
            *received_position = position.Clone();
          }, &received_position);

  callback_->SetUpdateCallback(update_callback);
  callback_->OnErrorReport(999);

  task_runner_->RunPendingTasks();

  ASSERT_FALSE(received_position.is_null());
  ASSERT_TRUE(received_position->is_error());
  EXPECT_EQ(received_position->get_error()->error_code,
            mojom::GeopositionErrorCode::kPositionUnavailable);
  EXPECT_EQ(received_position->get_error()->error_message,
            "Unknown error during the locating occured!");
}

TEST_F(LocationProviderCallbackTest, OnLocationReportWithValidLocation) {
  auto mock_location = std::make_shared<MockLocationInfo>();
  EXPECT_CALL(*mock_location, GetLatitude()).WillOnce(Return(37.7749));
  EXPECT_CALL(*mock_location, GetLongitude()).WillOnce(Return(-122.4194));
  EXPECT_CALL(*mock_location, GetAltitude()).WillOnce(Return(100.0));
  EXPECT_CALL(*mock_location, GetAccuracy()).WillOnce(Return(10.0f));
  EXPECT_CALL(*mock_location, GetDirection()).WillOnce(Return(45.0));
  EXPECT_CALL(*mock_location, GetSpeed()).WillOnce(Return(5.0f));

  mojom::GeopositionResultPtr received_position;
  LocationProviderCallback::UpdateCallback update_callback =
      base::BindRepeating(
          [](mojom::GeopositionResultPtr* received_position, mojom::GeopositionResultPtr position) {
            *received_position = position.Clone();
          }, &received_position);

  callback_->SetUpdateCallback(update_callback);
  callback_->OnLocationReport(mock_location);

  task_runner_->RunPendingTasks();

  ASSERT_FALSE(received_position.is_null());
  ASSERT_TRUE(received_position->is_position());
  EXPECT_EQ(received_position->get_position()->latitude, 37.7749);
  EXPECT_EQ(received_position->get_position()->longitude, -122.4194);
}

TEST_F(LocationProviderCallbackTest, OnLocatingStatusChange) {
  callback_->OnLocatingStatusChange(1);
  callback_->OnLocatingStatusChange(0);
}

TEST_F(LocationProviderCallbackTest, GetPositionAfterUpdate) {
  auto mock_location = std::make_shared<MockLocationInfo>();
  EXPECT_CALL(*mock_location, GetLatitude()).WillOnce(Return(37.7749));
  EXPECT_CALL(*mock_location, GetLongitude()).WillOnce(Return(-122.4194));
  EXPECT_CALL(*mock_location, GetAltitude()).WillOnce(Return(100.0));
  EXPECT_CALL(*mock_location, GetAccuracy()).WillOnce(Return(10.0f));
  EXPECT_CALL(*mock_location, GetDirection()).WillOnce(Return(45.0));
  EXPECT_CALL(*mock_location, GetSpeed()).WillOnce(Return(5.0f));

  callback_->OnNewLocationAvailable(mock_location);

  task_runner_->RunPendingTasks();

  const mojom::GeopositionResult* position = callback_->GetPosition();
  ASSERT_NE(position, nullptr);
  ASSERT_TRUE(position->is_position());
  EXPECT_EQ(position->get_position()->latitude, 37.7749);
  EXPECT_EQ(position->get_position()->longitude, -122.4194);
}

TEST_F(LocationProviderCallbackTest, MultipleLocationUpdates) {
  auto mock_location1 = std::make_shared<MockLocationInfo>();
  EXPECT_CALL(*mock_location1, GetLatitude()).WillOnce(Return(37.7749));
  EXPECT_CALL(*mock_location1, GetLongitude()).WillOnce(Return(-122.4194));
  EXPECT_CALL(*mock_location1, GetAltitude()).WillOnce(Return(100.0));
  EXPECT_CALL(*mock_location1, GetAccuracy()).WillOnce(Return(10.0f));
  EXPECT_CALL(*mock_location1, GetDirection()).WillOnce(Return(45.0));
  EXPECT_CALL(*mock_location1, GetSpeed()).WillOnce(Return(5.0f));

  auto mock_location2 = std::make_shared<MockLocationInfo>();
  EXPECT_CALL(*mock_location2, GetLatitude()).WillOnce(Return(40.7128));
  EXPECT_CALL(*mock_location2, GetLongitude()).WillOnce(Return(-74.0060));
  EXPECT_CALL(*mock_location2, GetAltitude()).WillOnce(Return(50.0));
  EXPECT_CALL(*mock_location2, GetAccuracy()).WillOnce(Return(5.0f));
  EXPECT_CALL(*mock_location2, GetDirection()).WillOnce(Return(90.0));
  EXPECT_CALL(*mock_location2, GetSpeed()).WillOnce(Return(10.0f));

  mojom::GeopositionResultPtr received_position1;
  mojom::GeopositionResultPtr received_position2;
  LocationProviderCallback::UpdateCallback update_callback =
      base::BindRepeating(
          [](mojom::GeopositionResultPtr* received_position, mojom::GeopositionResultPtr position) {
            *received_position = position.Clone();
          }, &received_position1);

  callback_->SetUpdateCallback(update_callback);
  callback_->OnNewLocationAvailable(mock_location1);
  task_runner_->RunPendingTasks();

  update_callback =
      base::BindRepeating(
          [](mojom::GeopositionResultPtr* received_position, mojom::GeopositionResultPtr position) {
            *received_position = position.Clone();
          }, &received_position2);
  callback_->SetUpdateCallback(update_callback);
  callback_->OnNewLocationAvailable(mock_location2);
  task_runner_->RunPendingTasks();

  ASSERT_FALSE(received_position1.is_null());
  EXPECT_EQ(received_position1->get_position()->latitude, 37.7749);

  ASSERT_FALSE(received_position2.is_null());
  EXPECT_EQ(received_position2->get_position()->latitude, 40.7128);
}

TEST_F(LocationProviderCallbackTest, UpdateCallbackWithoutSet) {
  auto mock_location = std::make_shared<MockLocationInfo>();
  EXPECT_CALL(*mock_location, GetLatitude()).WillOnce(Return(37.7749));
  EXPECT_CALL(*mock_location, GetLongitude()).WillOnce(Return(-122.4194));
  EXPECT_CALL(*mock_location, GetAltitude()).WillOnce(Return(100.0));
  EXPECT_CALL(*mock_location, GetAccuracy()).WillOnce(Return(10.0f));
  EXPECT_CALL(*mock_location, GetDirection()).WillOnce(Return(45.0));
  EXPECT_CALL(*mock_location, GetSpeed()).WillOnce(Return(5.0f));

  callback_->OnNewLocationAvailable(mock_location);

  task_runner_->RunPendingTasks();

  const mojom::GeopositionResult* position = callback_->GetPosition();
  ASSERT_NE(position, nullptr);
}

}  // namespace device
