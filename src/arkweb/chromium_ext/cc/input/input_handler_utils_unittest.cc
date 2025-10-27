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

#include "arkweb/chromium_ext/cc/input/input_handler_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace{

class InputHandlerUtilsTest : public testing::Test {
 protected:
  void SetUp() override {
    InputHandlerUtils::pdf_delta_x_ = 0;
    InputHandlerUtils::pdf_delta_y_ = 0;
  }
};

TEST_F(InputHandlerUtilsTest, PdfOverSpeed001) {
  InputHandlerUtils::pdf_delta_x_ = 100;
  InputHandlerUtils::pdf_delta_y_ = -100;

  EXPECT_FALSE(InputHandlerUtils::PdfOverSpeed());
}

TEST_F(InputHandlerUtilsTest, PdfOverSpeed002) {
  InputHandlerUtils::pdf_delta_x_ = 500;
  EXPECT_TRUE(InputHandlerUtils::PdfOverSpeed());
}

TEST_F(InputHandlerUtilsTest, PdfOverSpeed003) {
  InputHandlerUtils::pdf_delta_x_ = -500;
  EXPECT_TRUE(InputHandlerUtils::PdfOverSpeed());
}

TEST_F(InputHandlerUtilsTest, PdfOverSpeed004) {
  InputHandlerUtils::pdf_delta_y_ = 500;
  EXPECT_TRUE(InputHandlerUtils::PdfOverSpeed());
}

TEST_F(InputHandlerUtilsTest, PdfOverSpeed005) {
  InputHandlerUtils::pdf_delta_y_ = -500;
  EXPECT_TRUE(InputHandlerUtils::PdfOverSpeed());
}

}  // namespace
}  // namespace cc