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

#include "gtest/gtest.h"
#include "hitrace_adapter_impl.h"

using namespace testing;
using namespace OHOS::NWeb;

class HiTraceAdapterImplTest : public ::testing::Test {};

namespace OHOS::NWeb {
int ConvertToInt(const char *originValue, int defaultValue);
}

/**
 * @tc.name: HiTraceAdapterImplTest_ConvertToInt_001.
 * @tc.desc: test of ConvertToInt in HiTraceAdapterImplTest
 * @tc.type: FUNC.
 */
TEST_F(HiTraceAdapterImplTest, HiTraceAdapterImplTest_ConvertToInt_001) {
  char origin_value[] = "100";
  int default_value = 0;
  auto result = OHOS::NWeb::ConvertToInt(nullptr, default_value);
  EXPECT_EQ(result, 0);

  result = OHOS::NWeb::ConvertToInt(origin_value, default_value);
  EXPECT_EQ(result, 100);
}

/**
 * @tc.name: HiTraceAdapterImplTest_ConvertToUint64_001.
 * @tc.desc: test of ConvertToUint64 in HiTraceAdapterImplTest
 * @tc.type: func.
 */
TEST_F(HiTraceAdapterImplTest, HiTraceAdapterImplTest_ConvertToUint64_001) {
  HiTraceAdapterImpl hi_trace_adapter_impl; 
  char origin_value[] = "100";
  uint64_t default_value = 0;
  auto result = hi_trace_adapter_impl.ConvertToUint64(nullptr, default_value);
  EXPECT_EQ(result, 0);

  result = hi_trace_adapter_impl.ConvertToUint64(origin_value, default_value);
  EXPECT_EQ(result, 100);

  char invaid_value[] = "test";
  result = hi_trace_adapter_impl.ConvertToUint64(invaid_value, default_value);
  EXPECT_EQ(result, 0);
}
