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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unistd.h>
#include <cerrno>
#include <iostream>
#include "nweb.h"

#define private public
#include "nweb_engine_impl.h"

using namespace testing;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
class NWebEngineImplTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void) {}
  static void TearDownTestCase(void) {}
  void SetUp(void) {}
  void TearDown(void) {}
};

TEST_F(NWebEngineImplTest, TrimMemoryByPressureLevel) {
  int32_t memoryLevel = 0;
  NWebEngine::GetInstance()->TrimMemoryByPressureLevel(memoryLevel);
  EXPECT_NE(NWebEngine::GetInstance(), nullptr);
}
}  // namespace OHOS::NWeb