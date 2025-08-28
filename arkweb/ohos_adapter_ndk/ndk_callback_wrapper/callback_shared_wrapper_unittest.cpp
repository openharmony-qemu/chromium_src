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

#include <string>

#include "gtest/gtest.h"
#define private public
#include "callback_shared_wrapper.h"
#undef private

using namespace testing;

using namespace OHOS::NWeb;

class CallbackSharedWrapperTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        void TearDown() override {}
};

class PersonWrapper {
    public:
        PersonWrapper(const std::string& name, uint32_t age) : name_(name), age_(age) {}
        uint32_t GetAge() {
            return age_;
        }
        ~PersonWrapper() = default;
    private:
        std::string name_;
        uint32_t age_;
};

/**
 * @tc.name: AddCallback_WhenInputNullptr
 * @tc.number: CallbackSharedWrapperTest_001
 * @tc.desc: Test AddCallback method of CallbackSharedWrapper class
 */
TEST_F(CallbackSharedWrapperTest, AddCallback_WhenInputNullptr) {
    CallbackSharedWrapper<PersonWrapper> callback_wrapper;
    std::shared_ptr<PersonWrapper> person = nullptr;
    size_t index = callback_wrapper.AddCallback(person);
    EXPECT_EQ(index, 0);
}

/**
 * @tc.name: AddCallback_WhenInputNormal
 * @tc.number: CallbackSharedWrapperTest_002
 * @tc.desc: Test AddCallback method of CallbackSharedWrapper class
 */
TEST_F(CallbackSharedWrapperTest, AddCallback_WhenInputNormal) {
    CallbackSharedWrapper<PersonWrapper> callback_wrapper;
    std::shared_ptr<PersonWrapper> person = std::make_shared<PersonWrapper>("arkweb", 6);
    size_t index = callback_wrapper.AddCallback(person);
    EXPECT_EQ(index, 1);
}

/**
 * @tc.name: ClearAndSizeTest
 * @tc.number: CallbackSharedWrapperTest_003
 * @tc.desc: Test Size and Clear method of CallbackSharedWrapper class
 */
TEST_F(CallbackSharedWrapperTest, ClearAndSizeTest) {
    CallbackSharedWrapper<PersonWrapper> callback_wrapper;
    std::shared_ptr<PersonWrapper> person = std::make_shared<PersonWrapper>("arkweb", 6);
    size_t index = callback_wrapper.AddCallback(person);
    EXPECT_EQ(callback_wrapper.Size(), 1);
    callback_wrapper.Clear(index);
    EXPECT_EQ(callback_wrapper.Size(), 0);
}

/**
 * @tc.name: GetCallbackTest
 * @tc.number: CallbackSharedWrapperTest_004
 * @tc.desc: Test GetCallback method of CallbackSharedWrapper class
 */
TEST_F(CallbackSharedWrapperTest, GetCallbackTest) {
    CallbackSharedWrapper<PersonWrapper> callback_wrapper;
    std::shared_ptr<PersonWrapper> person = std::make_shared<PersonWrapper>("arkweb", 6);
    size_t index = callback_wrapper.AddCallback(person);
    std::shared_ptr<PersonWrapper> person_get = callback_wrapper.GetCallback(index);
    EXPECT_NE(person_get, nullptr);
    EXPECT_EQ(person_get->GetAge(), 6);
    size_t unvalid_index = 10;
    std::shared_ptr<PersonWrapper> person_invalid = callback_wrapper.GetCallback(unvalid_index);
    EXPECT_EQ(person_invalid, nullptr);
}