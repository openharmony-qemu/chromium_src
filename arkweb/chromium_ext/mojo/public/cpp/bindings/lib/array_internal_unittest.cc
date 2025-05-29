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

#include "mojo/public/cpp/bindings/lib/array_internal.h"

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace mojo {
namespace internal {

using namespace testing;

class makeMessageWithArrayIndexTest : public ::testing::Test {
protected:
void SetUp() override {}
void TearDown() override {}
};

/**
 * @tc.name  : MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithValidParameters
 * @tc.number: 001
 * @tc.desc  : Test MakeMessageWithArrayIndex function with valid parameters
 */
TEST_F(makeMessageWithArrayIndexTest, MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithValidParameters) {
    const char* message = "Test message";
    size_t size = 10;
    size_t index = 5;
    std::string expected_message = "Test message: array size - 10; index - 5";
    EXPECT_EQ(MakeMessageWithArrayIndex(message, size, index), expected_message);
}

/**
 * @tc.name  : MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithZeroSize
 * @tc.number: 002
 * @tc.desc  : Test MakeMessageWithArrayIndex function with zero size
 */
TEST_F(makeMessageWithArrayIndexTest, MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithZeroSize) {
    const char* message = "Test message";
    size_t size = 0;
    size_t index = 5;
    std::string expected_message = "Test message: array size - 0; index - 5";
    EXPECT_EQ(MakeMessageWithArrayIndex(message, size, index), expected_message);
}

/**
 * @tc.name  : MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithZeroIndex
 * @tc.number: 003
 * @tc.desc  : Test MakeMessageWithArrayIndex function with zero index
 */
TEST_F(makeMessageWithArrayIndexTest, MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithZeroIndex) {
    const char* message = "Test message";
    size_t size = 10;
    size_t index = 0;
    std::string expected_message = "Test message: array size - 10; index - 0";
    EXPECT_EQ(MakeMessageWithArrayIndex(message, size, index), expected_message);
}

/**
 * @tc.name  : MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithNegativeIndex
 * @tc.number: 004
 * @tc.desc  : Test MakeMessageWithArrayIndex function with negative index
 */
TEST_F(makeMessageWithArrayIndexTest, MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithNegativeIndex) {
    const char* message = "Test message";
    size_t size = 10;
    size_t index = -5;
    std::string expected_message = "Test message: array size - 10; index - -5";
    EXPECT_EQ(MakeMessageWithArrayIndex(message, size, index), expected_message);
}

/**
 * @tc.name  : MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithNegativeSize
 * @tc.number: 005
 * @tc.desc  : Test MakeMessageWithArrayIndex function with negative size
 */
TEST_F(makeMessageWithArrayIndexTest, MakeMessageWithArrayIndex_ShouldReturnCorrectMessage_WhenCalledWithNegativeSize) {
    const char* message = "Test message";
    size_t size = -10;
    size_t index = 5;
    std::string expected_message = "Test message: array size - -10; index - 5";
    EXPECT_EQ(MakeMessageWithArrayIndex(message, size, index), expected_message);
}

class makeMessageWithExpectedArraySizeTest : public ::testing::Test {
protected:
void SetUp() override {}
void TearDown() override {}
};

/**
 * @tc.name  : MakeMessageWithExpectedArraySize_ShouldReturnCorrectMessage_WhenCalledWithValidParameters
 * @tc.number: 001
 * @tc.desc  : Test MakeMessageWithExpectedArraySize function with valid parameters
 */
TEST_F(makeMessageWithExpectedArraySizeTest, MakeMessageWithExpectedArraySize_ShouldReturnCorrectMessage_WhenCalledWithValidParameters)
{
    const char* message = "Test Message";
    size_t size = 10;
    size_t expected_size = 20;
    std::string expected_result = "Test Message: array size - 10; expected size - 20";
    EXPECT_EQ(MakeMessageWithExpectedArraySize(message, size, expected_size), expected_result);
}

/**
 * @tc.name  : MakeMessageWithExpectedArraySize_ShouldReturnCorrectMessage_WhenCalledWithZeroSize
 * @tc.number: 002
 * @tc.desc  : Test MakeMessageWithExpectedArraySize function with zero size
 */
TEST_F(makeMessageWithExpectedArraySizeTest, MakeMessageWithExpectedArraySize_ShouldReturnCorrectMessage_WhenCalledWithZeroSize)
{
    const char* message = "Test Message";
    size_t size = 0;
    size_t expected_size = 20;
    std::string expected_result = "Test Message: array size - 0; expected size - 20";
    EXPECT_EQ(MakeMessageWithExpectedArraySize(message, size, expected_size), expected_result);
}

/**
 * @tc.name  : MakeMessageWithExpectedArraySize_ShouldReturnCorrectMessage_WhenCalledWithZeroExpectedSize
 * @tc.number: 003
 * @tc.desc  : Test MakeMessageWithExpectedArraySize function with zero expected size
 */
TEST_F(makeMessageWithExpectedArraySizeTest, MakeMessageWithExpectedArraySize_ShouldReturnCorrectMessage_WhenCalledWithZeroExpectedSize)
{
    const char* message = "Test Message";
    size_t size = 10;
    size_t expected_size = 0;
    std::string expected_result = "Test Message: array size - 10; expected size - 0";
    EXPECT_EQ(MakeMessageWithExpectedArraySize(message, size, expected_size), expected_result);
}

/**
 * @tc.name  : MakeMessageWithExpectedArraySize_ShouldReturnCorrectMessage_WhenCalledWithZeroSizeAndExpectedSize
 * @tc.number: 004
 * @tc.desc  : Test MakeMessageWithExpectedArraySize function with zero size and expected size
 */
TEST_F(makeMessageWithExpectedArraySizeTest, MakeMessageWithExpectedArraySize_ShouldReturnCorrectMessage_WhenCalledWithZeroSizeAndExpectedSize)
{
    const char* message = "Test Message";
    size_t size = 0;
    size_t expected_size = 0;
    std::string expected_result = "Test Message: array size - 0; expected size - 0";
    EXPECT_EQ(MakeMessageWithExpectedArraySize(message, size, expected_size), expected_result);
}

}  // namespace internal
}  // namespace mojo