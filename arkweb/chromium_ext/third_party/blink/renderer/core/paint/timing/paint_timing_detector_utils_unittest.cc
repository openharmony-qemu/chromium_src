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

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"
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

}  // namespace blink