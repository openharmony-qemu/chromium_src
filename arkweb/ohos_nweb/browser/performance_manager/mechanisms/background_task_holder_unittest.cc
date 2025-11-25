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

#include <cstdint>
#include <memory>
#include <gmock/gmock.h>

#include "background_task_adapter.h"
#include "gtest/gtest.h"

#undef private
#define private public
#include "background_task_holder.h"
#undef private
#define private public

using namespace testing;
using namespace OHOS::NWeb;
using namespace performance_manager;
using namespace performance_manager::mechanism;

class MockApplicationStateChangeCallback : public ApplicationStateChangeCallback {
public:
    MockApplicationStateChangeCallback() = default;
    ~MockApplicationStateChangeCallback() = default;

    bool foregroundCalled = false;
    bool backgroundCalled = false;

    void NotifyApplicationForeground() override {
        foregroundCalled = true;
    }

    void NotifyApplicationBackground() override {
        backgroundCalled = true;
    }
};

class BackgroundStateChangeCallbackTest : public ::testing::Test {
protected:
    void SetUp() override {
        callback = std::make_shared<MockApplicationStateChangeCallback>();
        backgroundStateChangeCallback = new BackgroundStateChangeCallback();
    }

    void TearDown() override {
        delete backgroundStateChangeCallback;
    }

    std::shared_ptr<MockApplicationStateChangeCallback> callback;
    BackgroundStateChangeCallback* backgroundStateChangeCallback;
};

TEST_F(BackgroundStateChangeCallbackTest, RegisterCallback) {
    backgroundStateChangeCallback->RegisterBackgroundTaskPolicyCallback(callback);

    backgroundStateChangeCallback->NotifyApplicationForeground();
    backgroundStateChangeCallback->NotifyApplicationBackground();

    EXPECT_TRUE(callback->foregroundCalled);
    EXPECT_TRUE(callback->backgroundCalled);
}

TEST_F(BackgroundStateChangeCallbackTest, MultipleCallbacks) {
    auto callback1 = std::make_shared<MockApplicationStateChangeCallback>();
    auto callback2 = std::make_shared<MockApplicationStateChangeCallback>();

    backgroundStateChangeCallback->RegisterBackgroundTaskPolicyCallback(callback1);
    backgroundStateChangeCallback->RegisterBackgroundTaskPolicyCallback(callback2);

    backgroundStateChangeCallback->NotifyApplicationForeground();
    backgroundStateChangeCallback->NotifyApplicationBackground();

    EXPECT_TRUE(callback1->foregroundCalled);
    EXPECT_TRUE(callback1->backgroundCalled);
    EXPECT_TRUE(callback2->foregroundCalled);
    EXPECT_TRUE(callback2->backgroundCalled);
}