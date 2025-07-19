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

#include "nweb_resize_helper.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
class NWebResizeHelperTest : public ::testing::Test {
 protected:
  void SetUp() override {
    helper_ = &NWebResizeHelper::GetInstance();
  }

  NWebResizeHelper* helper_;
};

TEST_F(NWebResizeHelperTest, SingletonPattern) {
  auto& instance1 = NWebResizeHelper::GetInstance();
  auto& instance2 = NWebResizeHelper::GetInstance();
  ASSERT_EQ(&instance1, &instance2);
}

TEST_F(NWebResizeHelperTest, TimestampOperations) {
  helper_->SetResizeStartTimeStamp();
  int64_t start_time = helper_->GetResizeStartTimeStamp();
  EXPECT_GT(start_time, 0);

  helper_->CaculateResizeTime();
  EXPECT_GE(helper_->GetResizeTime(), 0);
}

TEST_F(NWebResizeHelperTest, ResizeCalculation) {
  const uint32_t target = 100;
  const int64_t pre_length = 80;
  int64_t adjust = helper_->GetResizeAdjustValue(target, pre_length, true);

  helper_->SetResizeHeightAndWidth(200, 300);
  EXPECT_EQ(helper_->GetResizeAdjustValue(200, 150, true), 350);
}

TEST_F(NWebResizeHelperTest, DragResizeState) {
  helper_->SetDragResizeStart(true);
  EXPECT_TRUE(helper_->IsDragResizeStart());

  helper_->SetDragResizeStart(false);
  EXPECT_FALSE(helper_->IsDragResizeStart());
}

TEST_F(NWebResizeHelperTest, ParameterRefresh) {
  helper_->SetResizeHeightAndWidth(100, 200);
  helper_->RefreshParam();
  EXPECT_EQ(helper_->GetResizeAdjustValue(100, 0, true), 100);
}

TEST_F(NWebResizeHelperTest, RefreshStartTimeStamp) {
  helper_->RefreshStartTimeStamp();
}
}