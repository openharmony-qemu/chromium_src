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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"

namespace blink {

class WebLocalFrameImplTest : public testing::Test {
 protected:
  void SetUp() override {
    helper_.InitializeAndLoad("about:blank");
    frame_impl_ = helper_.GetWebView()->MainFrameImpl();
  }

  void TearDown() override {
    helper_.Reset();
  }

  void SelectClosetWordAndShowSelectionMenu() {
    frame_impl_->SelectClosetWordAndShowSelectionMenu();
  }

  void OnDataDetectorSelectText() {
    frame_impl_->OnDataDetectorSelectText();
  }

  test::TaskEnvironment task_environment_;
  WebLocalFrameImpl* frame_impl_;
  frame_test_helpers::WebViewHelper helper_;
};

TEST_F(WebLocalFrameImplTest, DidSubresourceFiltered) {
  frame_impl_->DidSubresourceFiltered();
}

TEST_F(WebLocalFrameImplTest, GetGlobalAdblockEnabled) {
  frame_impl_->GetGlobalAdblockEnabled();
}

TEST_F(WebLocalFrameImplTest, SelectClosetWordAndShowSelectionMenu) {
  helper_.Reset();
  SelectClosetWordAndShowSelectionMenu();
}

TEST_F(WebLocalFrameImplTest, SelectClosetWordAndShowSelectionMenuWithValidView) {
  SelectClosetWordAndShowSelectionMenu();
}

TEST_F(WebLocalFrameImplTest, OnDataDetectorSelectText) {
  helper_.Reset();
  OnDataDetectorSelectText();
}

TEST_F(WebLocalFrameImplTest, OnDataDetectorSelectTextWithValidView) {
  OnDataDetectorSelectText();
}

TEST_F(WebLocalFrameImplTest, SelectRangeV2WithValidView) {
  gfx::Point position = gfx::Point(10, 10);
  ASSERT_NO_FATAL_FAILURE(frame_impl_->SelectRangeV2(position, true););
}

TEST_F(WebLocalFrameImplTest, SelectRangeV2WithValidViewAndBaseFalse) {
  gfx::Point position = gfx::Point(10, 10);
  ASSERT_NO_FATAL_FAILURE(frame_impl_->SelectRangeV2(position, false););
}

}  // namespace blink