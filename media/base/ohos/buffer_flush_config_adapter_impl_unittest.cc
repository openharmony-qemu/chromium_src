// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "buffer_flush_config_adapter_impl.h"
#include "gtest/gtest.h"

namespace OHOS::NWeb {

class BufferFlushConfigAdapterImplTest : public ::testing::Test {
 protected:
  BufferFlushConfigAdapterImpl bufferFlushConfigAdapterImpl;
};

// Scenario1: Test GetX method
TEST_F(BufferFlushConfigAdapterImplTest, SetX_GetX_ShouldReturnCorrectValue) {
  int32_t expectedValue = 10;
  bufferFlushConfigAdapterImpl.SetX(expectedValue);
  int32_t actualValue = bufferFlushConfigAdapterImpl.GetX();
  ASSERT_EQ(expectedValue, actualValue);
}

// Scenario1: Test GetY method
TEST_F(BufferFlushConfigAdapterImplTest, GetY_ShouldReturnCorrectValue) {
  int32_t expectedValue = 10;
  int32_t actualValue = bufferFlushConfigAdapterImpl.GetY();
  EXPECT_EQ(expectedValue, actualValue);
}

// Scenario1: Test GetW method
TEST_F(BufferFlushConfigAdapterImplTest, GetW_ShouldReturnW) {
  int32_t expectedW = 100;
  bufferFlushConfigAdapterImpl.SetW(expectedW);
  int32_t actualW = bufferFlushConfigAdapterImpl.GetW();
  EXPECT_EQ(expectedW, actualW);
}

// Scenario1: Test GetH method
TEST_F(BufferFlushConfigAdapterImplTest, GetH_ShouldReturnCorrectValue) {
  int32_t expectedValue = 100;
  bufferFlushConfigAdapterImpl.SetH(expectedValue);
  int32_t actualValue = bufferFlushConfigAdapterImpl.GetH();
  EXPECT_EQ(expectedValue, actualValue);
}

// Scenario1: Test when GetTimestamp is called then it returns the correct
TEST_F(BufferFlushConfigAdapterImplTest,
       GetTimestamp_ShouldReturnCorrectTimestamp) {
  int64_t expectedTimestamp = 123456789;
  bufferFlushConfigAdapterImpl.SetTimestamp(expectedTimestamp);
  int64_t actualValue = bufferFlushConfigAdapterImpl.GetTimestamp();
  ASSERT_EQ(expectedTimestamp, actualValue);
}

// Scenario1: Test SetX method
TEST_F(BufferFlushConfigAdapterImplTest, SetX_ShouldSetX) {
  int32_t x = 10;
  bufferFlushConfigAdapterImpl.SetX(x);
  int32_t actualValue = bufferFlushConfigAdapterImpl.GetX();
  ASSERT_EQ(actualValue, x);
}

// Scenario1: Test SetY method
TEST_F(BufferFlushConfigAdapterImplTest, SetY_ShouldSetY) {
  int32_t y = 10;
  bufferFlushConfigAdapterImpl.SetY(y);
  EXPECT_EQ(bufferFlushConfigAdapterImpl.GetY(), y);
}

// Scenario1: Test SetW method
TEST_F(BufferFlushConfigAdapterImplTest, SetW_ShouldSetW) {
  int32_t w = 100;
  bufferFlushConfigAdapterImpl.SetW(w);
  EXPECT_EQ(bufferFlushConfigAdapterImpl.GetW(), w);
}

// Scenario1: Test SetH method
TEST_F(BufferFlushConfigAdapterImplTest, SetH_ShouldSetH) {
  int32_t h = 100;
  bufferFlushConfigAdapterImpl.SetH(h);
  ASSERT_EQ(bufferFlushConfigAdapterImpl.GetH(), h);
}

// Scenario1: Test SetTimestamp method
TEST_F(BufferFlushConfigAdapterImplTest, SetTimestamp_ShouldSetCorrectly) {
  int64_t timestamp = 1234567890;
  bufferFlushConfigAdapterImpl.SetTimestamp(timestamp);
  EXPECT_EQ(bufferFlushConfigAdapterImpl.GetTimestamp(), timestamp);
}
}  // namespace OHOS::NWeb
