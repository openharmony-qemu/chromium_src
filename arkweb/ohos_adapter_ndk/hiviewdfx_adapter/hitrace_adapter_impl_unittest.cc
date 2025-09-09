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

using namespace testing;

class HiTraceAdapterImplTest : public ::testing::Test {};

namespace OHOS::NWeb {
extern int ConvertToInt(const char *originValue, int defaultValue);
}

/**
 * @tc.name: HiTraceAdapterImplTest_ConvertToInt_001.
 * @tc.desc: test of ConvertToInt in HiTraceAdapterImplTest 
 * @tc.type: FUNC.
 * @tc.require:
 */
TEST_F(HiTraceAdapterImplTest, HiTraceAdapterImplTest_ConvertToInt_001) {
  char origin_value[] = "100";
  int default_value = 0;
  auto result = OHOS::NWeb::ConvertToInt(nullptr, default_value);
  EXPECT_EQ(result, 0);

  result = OHOS::NWeb::ConvertToInt(origin_value, default_value);
  EXPECT_EQ(result, 100);
}
