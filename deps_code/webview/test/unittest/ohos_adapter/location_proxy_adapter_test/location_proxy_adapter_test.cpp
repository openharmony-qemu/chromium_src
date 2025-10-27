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

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "location_proxy_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
namespace {
static constexpr double MIN_LATITUDE = -90.0;
static constexpr double MIN_LONGITUDE = -180.0;
} // namespace

class LocationProxyAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void LocationProxyAdapterTest::SetUpTestCase(void)
{}

void LocationProxyAdapterTest::TearDownTestCase(void)
{}

void LocationProxyAdapterTest::SetUp(void)
{}

void LocationProxyAdapterTest::TearDown(void)
{}

class LocationCallbackAdapterMock : public LocationCallbackAdapter {
public:
    LocationCallbackAdapterMock() = default;

    void OnLocationReport(
        const std::shared_ptr<LocationInfo> location)  override
    {}

    void OnLocatingStatusChange(const int status) override
    {}

    void OnErrorReport(const int errorCode) override
    {}
};

/**
 * @tc.name: LocationProxyAdapterTest_LocationInstance_001
 * @tc.desc: LocationInstance.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_LocationInstance_001, TestSize.Level1)
{
    std::shared_ptr<LocationProxyAdapter> proxyAdapter = LocationInstance::GetInstance().CreateLocationProxyAdapter();
    EXPECT_NE(proxyAdapter, nullptr);
    std::shared_ptr<LocationRequestConfig> requestConfig =
        LocationInstance::GetInstance().CreateLocationRequestConfig();
    EXPECT_NE(requestConfig, nullptr);
}

/**
 * @tc.name: LocationProxyAdapterTest_SetScenario_002
 * @tc.desc: SetScenario.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_SetScenario_002, TestSize.Level1)
{
    auto requestConfigImpl = std::make_shared<LocationRequestConfigImpl>();
    EXPECT_NE(requestConfigImpl, nullptr);
    int32_t scenario = -1;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::UNSET;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::NAVIGATION;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::TRAJECTORY_TRACKING;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::CAR_HAILING;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::DAILY_LIFE_SERVICE;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::NO_POWER;
    requestConfigImpl->SetScenario(scenario);

    requestConfigImpl->SetFixNumber(0);
    requestConfigImpl->SetMaxAccuracy(0);
    requestConfigImpl->SetDistanceInterval(0);
    requestConfigImpl->SetTimeInterval(0);
    int32_t priority = -1;
    requestConfigImpl->SetPriority(priority);
    priority = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_UNSET;
    requestConfigImpl->SetPriority(priority);
    priority = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_ACCURACY;
    requestConfigImpl->SetPriority(priority);
    priority = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_LOW_POWER;
    requestConfigImpl->SetPriority(priority);
    priority = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_FAST_FIRST_FIX;
    requestConfigImpl->SetPriority(priority);
    std::unique_ptr<OHOS::Location::RequestConfig>& requestConfig = requestConfigImpl->GetConfig();
    EXPECT_NE(requestConfig, nullptr);
}

/**
 * @tc.name: LocationProxyAdapterTest_SetScenario_003
 * @tc.desc: SetScenario.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_SetScenario_003, TestSize.Level1)
{
    auto requestConfigImpl = std::make_shared<LocationRequestConfigImpl>();
    EXPECT_NE(requestConfigImpl, nullptr);
    requestConfigImpl->config_ = nullptr;
    requestConfigImpl->SetScenario(0);
    requestConfigImpl->SetFixNumber(0);
    requestConfigImpl->SetMaxAccuracy(0);
    requestConfigImpl->SetDistanceInterval(0);
    requestConfigImpl->SetTimeInterval(0);
    requestConfigImpl->SetPriority(0);
}

/**
 * @tc.name: LocationProxyAdapterTest_LocationInfoImpl_004
 * @tc.desc: LocationInfoImpl.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_LocationInfoImpl_004, TestSize.Level1)
{
    std::unique_ptr<OHOS::Location::Location> location = std::make_unique<OHOS::Location::Location>();
    EXPECT_NE(location, nullptr);
    auto locationInfoImpl = std::make_shared<LocationInfoImpl>(location);
    EXPECT_NE(locationInfoImpl, nullptr);
    double latitude = locationInfoImpl->GetLatitude();
    EXPECT_NEAR(latitude, MIN_LATITUDE - 1, 0.01);
    latitude = locationInfoImpl->GetLongitude();
    EXPECT_NEAR(latitude, MIN_LONGITUDE - 1, 0.01);
    latitude = locationInfoImpl->GetAltitude();
    EXPECT_NEAR(latitude, 0.0, 0.01);
    float accuracy = locationInfoImpl->GetAccuracy();
    EXPECT_NEAR(accuracy, 0.0, 0.01);
    accuracy = locationInfoImpl->GetSpeed();
    EXPECT_NEAR(accuracy, 0.0, 0.01);
    double direction = locationInfoImpl->GetDirection();
    EXPECT_NEAR(direction, 0.0, 0.01);
    int64_t timeStamp = locationInfoImpl->GetTimeStamp();
    EXPECT_EQ(timeStamp, 0);
    timeStamp = locationInfoImpl->GetTimeSinceBoot();
    EXPECT_EQ(timeStamp, 0);
    std::unique_ptr<OHOS::Location::Location>& locationInfo = locationInfoImpl->GetLocation();
    EXPECT_NE(locationInfo, nullptr);
    locationInfoImpl->GetAdditions();
    EXPECT_NE(locationInfoImpl->location_, nullptr);
}

/**
 * @tc.name: LocationProxyAdapterTest_LocationInfoImpl_005
 * @tc.desc: LocationInfoImpl.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_LocationInfoImpl_005, TestSize.Level1)
{
    std::unique_ptr<OHOS::Location::Location> location = nullptr;
    auto locationInfoImpl = std::make_shared<LocationInfoImpl>(location);
    EXPECT_NE(locationInfoImpl, nullptr);
    double latitude = locationInfoImpl->GetLatitude();
    EXPECT_NEAR(latitude, 0.0, 0.01);
    latitude = locationInfoImpl->GetLongitude();
    EXPECT_NEAR(latitude, 0.0, 0.01);
    latitude = locationInfoImpl->GetAltitude();
    EXPECT_NEAR(latitude, 0.0, 0.01);
    float accuracy = locationInfoImpl->GetAccuracy();
    EXPECT_NEAR(accuracy, 0.0, 0.01);
    accuracy = locationInfoImpl->GetSpeed();
    EXPECT_NEAR(accuracy, 0.0, 0.01);
    double direction = locationInfoImpl->GetDirection();
    EXPECT_NEAR(direction, 0.0, 0.01);
    int64_t timeStamp = locationInfoImpl->GetTimeStamp();
    EXPECT_EQ(timeStamp, 0);
    timeStamp = locationInfoImpl->GetTimeSinceBoot();
    EXPECT_EQ(timeStamp, 0);
    locationInfoImpl->GetAdditions();
    EXPECT_EQ(locationInfoImpl->location_, nullptr);
}

/**
 * @tc.name: LocationProxyAdapterTest_EnableAbility_006
 * @tc.desc: EnableAbility.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_EnableAbility_006, TestSize.Level1)
{
    auto proxyAdapterImpl = std::make_shared<LocationProxyAdapterImpl>();
    EXPECT_NE(proxyAdapterImpl, nullptr);
    std::shared_ptr<LocationRequestConfig> requestConfig = std::make_shared<LocationRequestConfigImpl>();
    EXPECT_NE(requestConfig, nullptr);
    std::shared_ptr<LocationCallbackAdapter> callback = std::make_shared<LocationCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    bool result = proxyAdapterImpl->EnableAbility(true);
    EXPECT_FALSE(result);
    bool enabled = proxyAdapterImpl->IsLocationEnabled();
    if (enabled) {
        EXPECT_TRUE(enabled);
    } else {
        EXPECT_FALSE(enabled);
    }
    
    int32_t id = proxyAdapterImpl->StartLocating(requestConfig, nullptr);
    EXPECT_EQ(id, -1);
    id = proxyAdapterImpl->StartLocating(requestConfig, callback);
    EXPECT_EQ(id, -1);

    result = proxyAdapterImpl->StopLocating(-1);
    EXPECT_FALSE(result);
    result = proxyAdapterImpl->StopLocating(0);
    EXPECT_FALSE(result);
    result = proxyAdapterImpl->StopLocating(0);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: LocationProxyAdapterTest_EnableAbility_007
 * @tc.desc: EnableAbility.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_EnableAbility_007, TestSize.Level1)
{
    auto proxyAdapterImpl = std::make_shared<LocationProxyAdapterImpl>();
    EXPECT_NE(proxyAdapterImpl, nullptr);
    proxyAdapterImpl->enableAbilityFunc_ = nullptr;
    bool result = proxyAdapterImpl->EnableAbility(true);
    EXPECT_FALSE(result);
    proxyAdapterImpl->isEnableLocationFunc_ = nullptr;
    result = proxyAdapterImpl->IsLocationEnabled();
    EXPECT_FALSE(result);
    proxyAdapterImpl->startLocatingFunc_ = nullptr;
    std::shared_ptr<LocationRequestConfig> requestConfig = std::make_shared<LocationRequestConfigImpl>();
    EXPECT_NE(requestConfig, nullptr);
    std::shared_ptr<LocationCallbackAdapter> callback = std::make_shared<LocationCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    int32_t id = proxyAdapterImpl->StartLocating(requestConfig, callback);
    EXPECT_EQ(id, -1);
    proxyAdapterImpl->stopLocatingFunc_ = nullptr;
    result = proxyAdapterImpl->StopLocating(id);
    EXPECT_FALSE(result);
}
} // namespace OHOS::NWeb
