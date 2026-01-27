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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "third_party/blink/renderer/core/loader/empty_clients.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/page/drag_controller_ext.h"
#include "third_party/blink/renderer/core/page/page_utils.h"
#include "ui/gfx/geometry/insets.h"

namespace blink {
  
struct MockPageScaleConstraints : public PageScaleConstraints {
public:
  MockPageScaleConstraints() = default;

  MockPageScaleConstraints(float initial, float min, float max) : PageScaleConstraints(initial, min, max) {}
  MockPageScaleConstraints(float min, float max) : PageScaleConstraints(1.0f, min, max) {}
  bool operator==(const PageScaleConstraints& other) const {
      return (minimum_scale == other.minimum_scale && maximum_scale == other.maximum_scale);
  }
};

class PageUtilsTest : public PageTestBase {
public:
  void SetUp() override {
      PageTestBase::SetUp(gfx::Size());
      page_ = &GetPage();
      drag_controller_ = blink::MakeGarbageCollected<DragControllerExt>(page_);
      page_utils_ = blink::MakeGarbageCollected<PageUtils>(page_);
  }
  Page* page_;
  Member<DragControllerExt> drag_controller_;
  Persistent<PageUtils> page_utils_;
};

TEST_F(PageUtilsTest, IsInTextDraging_NullDragController) {
  EXPECT_FALSE(page_utils_->IsInTextDraging());
}

TEST_F(PageUtilsTest, IsHyperLinkDragging_NullDragController) {
  EXPECT_FALSE(page_utils_->IsHyperLinkDragging());
}

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
TEST_F(PageUtilsTest, SetScrollbarAvoidAreaTop_CallsChromeClient) {
  EmptyChromeClient* logger = blink::MakeGarbageCollected<EmptyChromeClient>();
  ChromeClient* client = logger;

  page_->SetChromeClientForTesting(client);
  page_utils_->SetScrollbarAvoidAreaTop(10.0);
}

TEST_F(PageUtilsTest, SetScrollbarAvoidAreaBottom_CallsChromeClient) {
  EmptyChromeClient* logger = blink::MakeGarbageCollected<EmptyChromeClient>();
  ChromeClient* client = logger;

  page_->SetChromeClientForTesting(client);
  page_utils_->SetScrollbarAvoidAreaBottom(5.0);
}

#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
TEST_F(PageUtilsTest, SafeAreaScaled_Scale) {
  page_->SetPageScaleFactor(2.0);
  page_utils_->SetSafeArea(gfx::Insets().set_top(10).set_left(10).set_bottom(10).set_right(10));
  gfx::Insets insets = page_utils_->SafeAreaScaled();
  EXPECT_EQ(insets.top(), 5);
  EXPECT_EQ(insets.bottom(), 5);
  EXPECT_EQ(insets.left(), 5);
  EXPECT_EQ(insets.right(), 5);
}
#endif

#if BUILDFLAG(IS_ARKWEB)
TEST_F(PageUtilsTest, IsNotPageScaleLimits) {
  MockPageScaleConstraints new_defaults = {10.0, 1.0, 4.0};
  page_utils_->IsNotPageScaleLimits(new_defaults);
}

TEST_F(PageUtilsTest, ResetPageScaleConstraints_Mobile) {
  page_utils_->ResetPageScaleConstraints(false);
  page_utils_->ResetPageScaleConstraints(true);
}

#endif
}  // namespace blink