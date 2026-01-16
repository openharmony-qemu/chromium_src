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

#include "popup_utils.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/web_frame_widget_impl.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/page/page_popup_client.h"
#include "third_party/blink/renderer/core/page/page_utils.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/testing/testing_platform_support.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"

namespace blink {
class PopupUtilsTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    page_holder_ = std::make_unique<DummyPageHolder>();
    local_frame_ = &page_holder_->GetFrame();
    frame_view_ = &page_holder_->GetFrameView();
    page_ = &page_holder_->GetPage();
    chrome_client_ = page_->GetChromeClient();
  }

  void TearDown() override { page_holder_.reset(); }

  std::unique_ptr<DummyPageHolder> page_holder_;
  Persistent<LocalFrame> local_frame_;
  Persistent<LocalFrameView> frame_view_;
  Persistent<Page> page_;
  Persistent<ChromeClient> chrome_client_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(PopupUtilsTest, AddAvailRectInWebToData_001) {
  SegmentedBuffer data;
  PopupUtils::AddAvailRectInWebToData(nullptr, data);
  EXPECT_TRUE(data.empty());
}

TEST_F(PopupUtilsTest, AddAvailRectInWebToData_002) {
  SegmentedBuffer data;
  local_frame_->SetView(nullptr);
  PopupUtils::AddAvailRectInWebToData(local_frame_, data);
  EXPECT_TRUE(data.empty());
  local_frame_->SetView(
      MakeGarbageCollected<LocalFrameView>(*local_frame_, gfx::Size()));
}

TEST_F(PopupUtilsTest, AddAvailRectInWebToData_003) {
  SegmentedBuffer data;
  page_->SetChromeClientForTesting(nullptr);
  PopupUtils::AddAvailRectInWebToData(local_frame_, data);
  EXPECT_TRUE(data.empty());
  page_->SetChromeClientForTesting(chrome_client_);
}

TEST_F(PopupUtilsTest, AddAvailRectInWebToData_004) {
  SegmentedBuffer data;
  gfx::Rect test_rect(1, 2, 3, 4);
  char test_data[] =
      "availableRectInScreen: {x: 1,\ny: 2,\nwidth: 3,\nheight: 4,\n},\n";
  frame_view_->SetFrameRect(test_rect);
  PopupUtils::AddAvailRectInWebToData(local_frame_, data);

  Vector<char> flattened_data = data.CopyAs<Vector<char>>();
  size_t expected_len = strlen(test_data);
  EXPECT_EQ(flattened_data.size(), expected_len);

  std::string actual_string(flattened_data.data(), flattened_data.size());
  EXPECT_EQ(actual_string, test_data);
}
}  // namespace blink