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
#include "content/browser/ohos/date_time_chooser_ohos.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace content {
class DateTimeChooserOHOSTest : public ::testing::Test {
 public:
  void SetUp() override {
    chooser_ = std::make_unique<DateTimeChooserOHOS>(nullptr);
  }

  void TearDown() override { chooser_.reset(); }

 protected:
  std::unique_ptr<DateTimeChooserOHOS> chooser_;
};

TEST_F(DateTimeChooserOHOSTest, NotifyResult_CallbackIsNull) {
  testing::internal::CaptureStderr();
  chooser_->NotifyResult(true, 123.45);
  std::string err_log = testing::internal::GetCapturedStderr();
  EXPECT_NE(err_log.find("DateTimeCallback is null"), std::string::npos);
}
}  // namespace content