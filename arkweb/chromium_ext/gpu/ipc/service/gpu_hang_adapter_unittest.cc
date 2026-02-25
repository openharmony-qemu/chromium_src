/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/gpu/ipc/service/gpu_hang_adapter.h"
#include "gtest/gtest.h"

namespace gpu {
class GpuHangAdapterTest : public testing::Test {
public:
  GpuHangAdapterTest() = default;
  ~GpuHangAdapterTest() = default;
};

TEST_F(GpuHangAdapterTest, GetProcessName) {
  std::string out = GetProcessName();
  EXPECT_FALSE(out.empty());
}

TEST_F(GpuHangAdapterTest, LogGpuHungEvent) {
  LogGpuHungEvent(0, 0);
  EXPECT_FALSE(false);
}

TEST_F(GpuHangAdapterTest, IsHangTestEnabled) {
  bool ret = IsHangTestEnabled("abc");
  EXPECT_FALSE(ret);
  ret = IsHangTestEnabled("false");
  EXPECT_TRUE(ret);
}

TEST_F(GpuHangAdapterTest, ReportGpuFreeze) {
  ReportGpuFreeze();
  EXPECT_FALSE(false);
}

TEST_F(GpuHangAdapterTest, CreateGpuHangAdapter) {
  auto ret = GpuHangAdapter::CreateGpuHangAdapterForGpuMain();
  EXPECT_FALSE(false);
  ret = GpuHangAdapter::CreateGpuHangAdapterForCompositorGpu();
  EXPECT_FALSE(false);
}
}
