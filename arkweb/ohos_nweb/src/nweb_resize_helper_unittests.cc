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

#define private public
#include "nweb_resize_helper.h"
#undef private

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

TEST_F(NWebResizeHelperTest, HeightNegativeResult) {
  helper_->GetResizeAdjustValue(100, -60, true);
}

TEST_F(NWebResizeHelperTest, HeightBelowLastValue) {
  helper_->GetResizeAdjustValue(100, -30, true);
}

TEST_F(NWebResizeHelperTest, WidthFirstTimeResize) {
  helper_->GetResizeAdjustValue(200, 0, false);
}

TEST_F(NWebResizeHelperTest, WidthNegativeResult) {
  helper_->GetResizeAdjustValue(200, -60, false);
}

TEST_F(NWebResizeHelperTest, WidthBelowLastValue) {
  helper_->GetResizeAdjustValue(200, -30, false);
}

TEST_F(NWebResizeHelperTest, GetResizeAdjustValue_001) {
  uint32_t target_length = 200;
  int64_t resize_pre_length = -30;
  bool isHeight = true;
  helper_->resize_last_height_ = 500;
  int64_t result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
  EXPECT_EQ(result, 500);
}

TEST_F(NWebResizeHelperTest, GetResizeAdjustValue_002) {
  uint32_t target_length = 200;
  int64_t resize_pre_length = -30;
  bool isHeight = false;
  helper_->resize_last_width_ = 500;
  int64_t result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
  EXPECT_EQ(result, 500);
}

TEST_F(NWebResizeHelperTest, GetResizeAdjustValue_003) {
  uint32_t target_length = 200;
  int64_t resize_pre_length = -30;
  bool isHeight = false;
  helper_->resize_last_width_ = 100;
  int64_t result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
  EXPECT_EQ(result, 270);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest001) {
  auto& instance = NWebResizeHelper::GetInstance();
  
  helper_->SetDragResizeStart(true);
  EXPECT_TRUE(helper_->IsDragResizeStart());

  helper_->SetDragResizeStart(false);
  EXPECT_FALSE(helper_->IsDragResizeStart());

  helper_->SetResizeStartTimeStamp();
  int64_t start_time = helper_->GetResizeStartTimeStamp();
  EXPECT_GT(start_time, 0);

  helper_->CaculateResizeTime();
  EXPECT_GE(helper_->GetResizeTime(), 0);

  helper_->SetResizeHeightAndWidth(100, 200);
  helper_->RefreshParam();
  EXPECT_EQ(helper_->GetResizeAdjustValue(100, 0, true), 100);

  uint32_t target_length = 1000;
  int64_t resize_pre_length = 50;
  bool isHeight = false;
  helper_->resize_last_width_ = 500;
  int64_t result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
  target_length = 1500;
  resize_pre_length = 50;
  isHeight = true;
  helper_->resize_last_width_ = 1050;
  helper_->resize_last_height_ = 800;
  result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest002) {
  helper_->GetResizeAdjustValue(200, -50, true);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest003) {
  helper_->GetResizeAdjustValue(200, -20, true);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest004) {
  helper_->GetResizeAdjustValue(300, 0, false);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest005) {
  helper_->GetResizeAdjustValue(300, -50, false);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest006) {
  helper_->GetResizeAdjustValue(100, -10, false);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest007) {
  uint32_t target_length = 200;
  int64_t resize_pre_length = -30;
  bool isHeight = true;
  helper_->resize_last_height_ = 500;
  int64_t result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
  EXPECT_EQ(result, 500);

  target_length = 1000;
  resize_pre_length = 50;
  isHeight = true;
  helper_->resize_last_height_ = 500;
  result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest008) {
  uint32_t target_length = 200;
  int64_t resize_pre_length = -30;
  bool isHeight = false;
  helper_->resize_last_width_ = 500;
  int64_t result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
  EXPECT_EQ(result, 500);

  target_length = 500;
  resize_pre_length = 50;
  isHeight = false;
  helper_->resize_last_width_ = 500;
  result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest009) {
  uint32_t target_length = 200;
  int64_t resize_pre_length = -30;
  bool isHeight = false;
  helper_->resize_last_width_ = 100;
  int64_t result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
  EXPECT_EQ(result, 270);

  target_length = 500;
  resize_pre_length = 50;
  isHeight = true;
  helper_->resize_last_width_ = 270;
  helper_->resize_last_height_ = 300;
  result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest010) {
  uint32_t target = 100;
  int64_t pre_length = 80;
  int64_t adjust = helper_->GetResizeAdjustValue(target, pre_length, true);
  helper_->SetResizeHeightAndWidth(200, 300);
  EXPECT_EQ(helper_->GetResizeAdjustValue(200, 150, true), 350);

  target = 200;
  pre_length = 50;
  adjust = helper_->GetResizeAdjustValue(target, pre_length, true);
  helper_->SetResizeHeightAndWidth(300, 400);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest011) {
  helper_->SetDragResizeStart(true);
  EXPECT_TRUE(helper_->IsDragResizeStart());

  helper_->SetDragResizeStart(false);
  EXPECT_FALSE(helper_->IsDragResizeStart());

  helper_->SetResizeHeightAndWidth(100, 200);
  helper_->RefreshParam();
  EXPECT_EQ(helper_->GetResizeAdjustValue(100, 0, true), 100);
}

TEST_F(NWebResizeHelperTest, DragResizeHelperTest012) {
  helper_->CaculateResizeTime();
  EXPECT_GE(helper_->GetResizeTime(), 0);

  helper_->SetResizeHeightAndWidth(100, 200);
  helper_->RefreshParam();
  EXPECT_EQ(helper_->GetResizeAdjustValue(100, 0, true), 100);
  helper_->SetDragResizeStart(true);
  EXPECT_TRUE(helper_->IsDragResizeStart());

  helper_->SetDragResizeStart(false);
  EXPECT_FALSE(helper_->IsDragResizeStart());

  helper_->SetResizeStartTimeStamp();
  int64_t start_time = helper_->GetResizeStartTimeStamp();
  EXPECT_GT(start_time, 0);

  helper_->GetResizeAdjustValue(200, -50, true);
  helper_->GetResizeAdjustValue(500, 150, false);
  helper_->GetResizeAdjustValue(500, 150, false);
  helper_->GetResizeAdjustValue(500, -150, true);

  uint32_t target_length = 1000;
  int64_t resize_pre_length = -150;
  bool isHeight = false;
  helper_->resize_last_width_ = 500;
  int64_t result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
  target_length = 1200;
  resize_pre_length = 150;
  isHeight = true;
  helper_->resize_last_width_ = 950;
  helper_->resize_last_height_ = 800;
  result =
      helper_->GetResizeAdjustValue(target_length, resize_pre_length, isHeight);
}
}