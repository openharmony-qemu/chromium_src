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

#include <gtest/gtest.h>

#include "arkweb/ohos_adapter_ndk/utils/include/ashmem_adapter.h"

namespace OHOS::NWeb {

class AshmemTest : public ::testing::Test {
 protected:
  void SetUp() override {
    fd_ = ShmemAdapterCreate("test_region", 4096);
    ASSERT_GE(fd_, 0) << "Failed to create ashmem region";
  }

  void TearDown() override {
    if (fd_ >= 0) {
      ShmemAdapterClose(fd_);
    }
  }

  int fd_{-1};
};

TEST_F(AshmemTest, ShmemAdapterCreate_001) {
  int fd = ShmemAdapterCreate("test_region", -1);
  EXPECT_EQ(fd, -1);

  fd = ShmemAdapterCreate(nullptr, 4096);
  EXPECT_GE(fd, 0);

  fd = ShmemAdapterCreate("test_region_new", 4096);
  EXPECT_GE(fd, 0);
  ShmemAdapterClose(fd);
}

TEST_F(AshmemTest, ShmemAdapterOthers_001) {
  int invalid_fd = -1;
  EXPECT_EQ(ShmemAdapterGetSize(invalid_fd), -1);
  EXPECT_EQ(ShmemAdapterGetSize(fd_), 4096);

  auto ret = ShmemAdapterSetProt(fd_, 100);
  EXPECT_EQ(ret, -1);

  EXPECT_EQ(ShmemAdapterGetProt(invalid_fd), -1);
  EXPECT_GE(ShmemAdapterGetProt(fd_), 0);

  void* addr = ShmemAdapterMap(invalid_fd, 0);
  ASSERT_NE(addr, nullptr);
  addr = ShmemAdapterMap(fd_, 0);
  ASSERT_NE(addr, nullptr);
  ShmemAdapterUnmap(addr, ShmemAdapterGetSize(fd_));
}

}  // namespace OHOS::NWeb