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

#include <iostream>
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/paint_timing_detector_utils.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing_detector.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/platform/testing/url_test_helpers.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/renderer/core/html/html_div_element.h"
#include "third_party/blink/renderer/platform/heap/visitor.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"
#include "third_party/blink/renderer/core/layout/layout_view.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/platform/graphics/image.h"
#include "third_party/blink/renderer/core/style/style_image.h"
#include "third_party/blink/renderer/platform/graphics/paint/property_tree_state.h"
#include "third_party/blink/renderer/platform/loader/fetch/media_timing.h"
#include "third_party/blink/renderer/core/loader/resource/image_resource_content.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#endif

#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT) || BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/first_screen_calculator.h"
#endif

namespace blink {

class PaintTimingDetectorUtilsTest : public testing::Test {
protected:
  void SetUp() override {
    web_view_impl_ = helper_.InitializeAndLoad("about:blank");
  }
  void TearDown() override {
    helper_.Reset();
  }
  test::TaskEnvironment task_environment_;
  WebViewImpl* web_view_impl_;
  frame_test_helpers::WebViewHelper helper_;
  std::string base_url_{"http://www.test.com/"};
};

TEST_F(PaintTimingDetectorUtilsTest, Constructor) {
  auto test1 = web_view_impl_->MainFrameImpl();
  EXPECT_NE(test1, nullptr);
  LocalFrameView* view = test1->GetFrameView();
  EXPECT_NE(view, nullptr);
  auto test = view->GetPaintTimingDetector();
  EXPECT_NE(&test, nullptr);
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector);
  EXPECT_EQ(utils.paint_timing_detector_, paint_timing_detector);
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(PaintTimingDetectorUtilsTest, ConstructorWithNeedSupplementTrue) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  EXPECT_EQ(utils.paint_timing_detector_, paint_timing_detector);
  EXPECT_TRUE(utils.HaveSupplementForBL());
}

TEST_F(PaintTimingDetectorUtilsTest, ConstructorWithNeedSupplementFalse) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, false);
  EXPECT_EQ(utils.paint_timing_detector_, paint_timing_detector);
  EXPECT_FALSE(utils.HaveSupplementForBL());
}

TEST_F(PaintTimingDetectorUtilsTest, NotifyLcpForBlankless) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.NotifyLcpForBlankless();
}

TEST_F(PaintTimingDetectorUtilsTest, CheckNotifyLcpForBlankless) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.CheckNotifyLcpForBlankless();
}

TEST_F(PaintTimingDetectorUtilsTest, ForwardNotifyPaintFinishedTrue) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  bool result = utils.ForwardNotifyPaintFinished();
  EXPECT_TRUE(result);
}

TEST_F(PaintTimingDetectorUtilsTest, ForwardNotifyPaintFinishedFalse) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, false);
  bool result = utils.ForwardNotifyPaintFinished();
  EXPECT_FALSE(result);
}

TEST_F(PaintTimingDetectorUtilsTest, ForwardOnInputOrScroll) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.ForwardOnInputOrScroll();
}

TEST_F(PaintTimingDetectorUtilsTest, ForwardRestartRecordingLCP) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.ForwardRestartRecordingLCP();
}

TEST_F(PaintTimingDetectorUtilsTest, ForwardRestartRecordingLCPToUkm) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.ForwardRestartRecordingLCPToUkm();
}

TEST_F(PaintTimingDetectorUtilsTest, ForwardSoftNavigationDetected) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  LocalDOMWindow* window = nullptr;
  utils.ForwardSoftNavigationDetected(window);
}

TEST_F(PaintTimingDetectorUtilsTest, ForwardReportIgnoredContent) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.ForwardReportIgnoredContent();
}

TEST_F(PaintTimingDetectorUtilsTest, ForwardUpdateLcpCandidate) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.ForwardUpdateLcpCandidate();
}

TEST_F(PaintTimingDetectorUtilsTest, RestartRecordingForBlankless) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.RestartRecordingForBlankless();
}

TEST_F(PaintTimingDetectorUtilsTest, SyncIPTDFrameIdxToBLIPTD) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.SyncIPTDFrameIdxToBLIPTD(0);
}

TEST_F(PaintTimingDetectorUtilsTest, SyncIPTDFrameIdxToBLIPTDNonZero) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.SyncIPTDFrameIdxToBLIPTD(100);
}

TEST_F(PaintTimingDetectorUtilsTest, SyncTPTDFrameIdxToBLTPTD) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.SyncTPTDFrameIdxToBLTPTD(0);
}

TEST_F(PaintTimingDetectorUtilsTest, SyncTPTDFrameIdxToBLTPTDNonZero) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector, true);
  utils.SyncTPTDFrameIdxToBLTPTD(100);
}
#endif

#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT) || BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
TEST_F(PaintTimingDetectorUtilsTest, GetFirstScreenCalculator) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector);
  FirstScreenCalculator* calculator = utils.GetFirstScreenCalculator();
}

TEST_F(PaintTimingDetectorUtilsTest, RestartRecordingFirstScreenPaint) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector);
  utils.RestartRecordingFirstScreenPaint();
}

TEST_F(PaintTimingDetectorUtilsTest, OnUserScroll) {
  auto test1 = web_view_impl_->MainFrameImpl();
  LocalFrameView* view = test1->GetFrameView();
  PaintTimingDetector* paint_timing_detector = &view->GetPaintTimingDetector();
  PaintTimingDetectorUtils utils(paint_timing_detector);
  utils.OnUserScroll();
}
#endif

}  // namespace blink
