/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "buffer_flush_config_adapter_impl.h"
#include "gtest/gtest.h"

namespace OHOS::NWeb {

class BufferFlushConfigAdapterImplTest : public ::testing::Test {
    protected:
        BufferFlushConfigAdapterImpl bufferFlushConfigAdapterImpl;
};

TEST_F(BufferFlushConfigAdapterImplTest, SetX_GetX_ShouldReturnCorrectValue) {
    int32_t expectedValue = 10;
    bufferFlushConfigAdapterImpl.SetX(expectedValue);
    int32_t actualValue = bufferFlushConfigAdapterImpl.GetX();
    ASSERT_EQ(expectedValue, actualValue);
}

TEST_F(BufferFlushConfigAdapterImplTest, GetY_ShouldReturnCorrectValue) {
    int32_t expectedValue = 10;
    bufferFlushConfigAdapterImpl.SetY(expectedValue);
    int32_t actualValue = bufferFlushConfigAdapterImpl.GetY();
    EXPECT_EQ(expectedValue, actualValue);
}

TEST_F(BufferFlushConfigAdapterImplTest, GetW_ShouldReturnW) {
    int32_t expectedW = 100;
    bufferFlushConfigAdapterImpl.SetW(expectedW);
    int32_t actualW = bufferFlushConfigAdapterImpl.GetW();
    EXPECT_EQ(expectedW, actualW);
}

TEST_F(BufferFlushConfigAdapterImplTest, GetH_ShouldReturnCorrectValue) {
    int32_t expectedValue = 100;
    bufferFlushConfigAdapterImpl.SetH(expectedValue);
    int32_t actualValue = bufferFlushConfigAdapterImpl.GetH();
    EXPECT_EQ(expectedValue, actualValue);
}

TEST_F(BufferFlushConfigAdapterImplTest,
       GetTimeStamp_ShouldReturnCorrectTimestamp) {
    int32_t expectedTimestamp = 123456789;
    bufferFlushConfigAdapterImpl.SetTimestamp(expectedTimestamp);
    int32_t actualValue = bufferFlushConfigAdapterImpl.GetTimestamp();
    ASSERT_EQ(expectedTimestamp, actualValue);
}

TEST_F(BufferFlushConfigAdapterImplTest, SetX_ShouldSetX) {
    int32_t x = 10;
    bufferFlushConfigAdapterImpl.SetX(x);
    int32_t actualValue = bufferFlushConfigAdapterImpl.GetX();
    ASSERT_EQ(actualValue, x);
}

TEST_F(BufferFlushConfigAdapterImplTest, SetY_ShouldSetY) {
    int32_t y = 10;
    bufferFlushConfigAdapterImpl.SetY(y);
    EXPECT_EQ(bufferFlushConfigAdapterImpl.GetY(), y);
}

TEST_F(BufferFlushConfigAdapterImplTest, SetW_ShouldSetW) {
    int32_t w = 100;
    bufferFlushConfigAdapterImpl.SetW(w);
    EXPECT_EQ(bufferFlushConfigAdapterImpl.GetW(), w);
}

TEST_F(BufferFlushConfigAdapterImplTest, SetH_ShouldSetH) {
    int32_t h = 100;
    bufferFlushConfigAdapterImpl.SetH(h);
    ASSERT_EQ(bufferFlushConfigAdapterImpl.GetH(), h);
}

TEST_F(BufferFlushConfigAdapterImplTest, SetTimestamp_ShouldSetCorrectly) {
    int32_t timestamp = 1234567890;
    bufferFlushConfigAdapterImpl.SetTimestamp(timestamp);
    EXPECT_EQ(bufferFlushConfigAdapterImpl.GetTimestamp(), timestamp);
}
} // namespace OHOS::MWeb
