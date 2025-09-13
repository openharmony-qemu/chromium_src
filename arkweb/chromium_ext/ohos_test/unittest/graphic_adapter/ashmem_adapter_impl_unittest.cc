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

#include "arkweb/ohos_adapter_ndk/interfaces/graphic_adapter.h"
#include "arkweb/ohos_adapter_ndk/utils/include/ashmem_adapter.h"

namespace OHOS::NWeb {

class AshmemAdapterImplTest : public ::testing::Test {};

TEST_F(AshmemAdapterImplTest, AshmemCreate_001) {
  int fd = AshmemAdapter::AshmemCreate("test_region", -1);
  EXPECT_EQ(fd, -1);

  fd = AshmemAdapter::AshmemCreate("test_region_new", 4096);
  EXPECT_GE(fd, 0);
  ShmemAdapterClose(fd);
}

}  // namespace OHOS::NWeb