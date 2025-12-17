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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#define private public
#include "arkweb/ohos_adapter_ndk/utils/include/ashmem_adapter.h"
#undef private
using namespace OHOS::NWeb;

namespace OHOS::NWeb {

class AshmemTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(AshmemTest, ShmemAdapterClose001)
{
  ASSERT_NO_FATAL_FAILURE(ShmemAdapterClose(0));

  char* name = nullptr;
  int fd = ShmemAdapterCreate(name, 1);
  ASSERT_NO_FATAL_FAILURE(ShmemAdapterClose(fd));
}

TEST_F(AshmemTest, ShmemAdapterCreate001)
{
  char name[] = "abc";
  int result = ShmemAdapterCreate(name, -1);
  EXPECT_EQ(result, -1);
}

TEST_F(AshmemTest, ShmemAdapterCreate002)
{
  char* name = nullptr;
  int result = ShmemAdapterCreate(name, 1);
  EXPECT_GE(result, 0);
}

TEST_F(AshmemTest, ShmemAdapterCreate003)
{
  char name[] = "abc";
  int result = ShmemAdapterCreate(name, 1);
  EXPECT_GE(result, 0);
}

TEST_F(AshmemTest, ShmemAdapterMap001)
{
  auto result = ShmemAdapterMap(1, 1);
  EXPECT_NE(result, nullptr);
}

TEST_F(AshmemTest, ShmemAdapterUnmap001)
{
  char* name = nullptr;
  ASSERT_NO_FATAL_FAILURE(ShmemAdapterUnmap(name, 1));

  char str[] = "abc";
  ASSERT_NO_FATAL_FAILURE(ShmemAdapterUnmap(str, 1));
}
}