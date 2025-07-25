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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_for_include.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/page_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"

namespace blink {

class MockDeviceUtils {
 public:
  static void SetIsTabletDevice(bool is_tablet) {
    is_tablet_device_ = is_tablet;
  }

  static bool IsTabletDevice() {
    return is_tablet_device_;
  }

 private:
  static bool is_tablet_device_;
};

bool MockDeviceUtils::is_tablet_device_ = false;

namespace base {
namespace ohos {
bool IsTabletDevice() {
  return MockDeviceUtils::IsTabletDevice();
}
}  // namespace ohos
}  // namespace base

class LocalFrameUtilTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    dummy_page_holder_ = std::make_unique<DummyPageHolder>(gfx::Size(800, 600));
    local_frame_ = &dummy_page_holder_->GetFrame();
    page_ = &dummy_page_holder_->GetPage();
    MockDeviceUtils::SetIsTabletDevice(false);
  }

  Document& GetDocument() {
    return *static_cast<Document*>(
        web_view_helper_.LocalMainFrame()->GetDocument());
  }

  void SetTextZoomFactor(float text_zoom_factor) {
    local_frame_->text_zoom_factor_ = text_zoom_factor;
  }

  void SetLayoutZoomFactor(float layout_zoom_factor) {
    local_frame_->layout_zoom_factor_ = layout_zoom_factor;
  }

  std::unique_ptr<DummyPageHolder> dummy_page_holder_;
  raw_ptr<LocalFrame> local_frame_;
  raw_ptr<Page> page_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_NonTablet) {
  MockDeviceUtils::SetIsTabletDevice(false);

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;
  
  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_Tablet_ZoomLessOrEqualOne) {
  MockDeviceUtils::SetIsTabletDevice(true);

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;
  
  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_Tablet_NoZoomChange) {
  MockDeviceUtils::SetIsTabletDevice(true);
  
  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = false;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;
  
  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsPage_NullPage) {
  float layout_zoom = 1.0f;
  float text_zoom = 1.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, nullptr);

  EXPECT_FALSE(result);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsPage_NoChange) {
  SetLayoutZoomFactor(1.5f);
  SetTextZoomFactor(1.2f);
  page_->GetSettings().SetTextZoomFactor(1.2f);

  float layout_zoom = 1.5f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_FALSE(result);
  EXPECT_EQ(text_zoom, 1.2f);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsPage_LayoutChanged) {
  SetLayoutZoomFactor(1.0f);
  SetTextZoomFactor(1.0f);
  page_->GetSettings().SetTextZoomFactor(1.0f);

  float layout_zoom = 1.5f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_TRUE(result);
  EXPECT_EQ(text_zoom, 1.0f);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsPage_TextChanged) {
  SetLayoutZoomFactor(1.0f);
  SetTextZoomFactor(1.0f);
  page_->GetSettings().SetTextZoomFactor(1.5f);

  float layout_zoom = 1.0f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_TRUE(result);
  EXPECT_EQ(text_zoom, 1.5f);
}

TEST_F(LocalFrameUtilTest, SetTextZoomFactorsExt) {
  LocalFrameUtil::SetTextZoomFactorsExt(local_frame_);
}

}  // namespace blink