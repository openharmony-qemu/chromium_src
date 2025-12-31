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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_view_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/page/focus_controller.h"

namespace blink {
namespace {

class AnimationMockChromeClient : public RenderingTestChromeClient {
 public:
  AnimationMockChromeClient() : has_scheduled_animation_(false) {}

  // ChromeClient
  MOCK_METHOD3(MockUpdateTooltipUnderCursor,
               void(LocalFrame*, const String&, TextDirection));
  void UpdateTooltipUnderCursor(LocalFrame& frame,
                                const String& tooltip_text,
                                TextDirection dir) override {
    MockUpdateTooltipUnderCursor(&frame, tooltip_text, dir);
  }

  void ScheduleAnimation(const LocalFrameView*,
                         base::TimeDelta = base::TimeDelta()) override {
    has_scheduled_animation_ = true;
  }
  bool has_scheduled_animation_;
};

class LocalFrameViewUtilsTest : public RenderingTest {
 protected:
  LocalFrameViewUtilsTest()
      : RenderingTest(MakeGarbageCollected<SingleChildLocalFrameClient>()),
        chrome_client_(MakeGarbageCollected<AnimationMockChromeClient>()) {}

  ~LocalFrameViewUtilsTest() override {
    testing::Mock::VerifyAndClearExpectations(&GetAnimationMockChromeClient());
  }

  RenderingTestChromeClient& GetChromeClient() const override {
    return *chrome_client_;
  }

  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    utils_ = std::make_unique<LocalFrameViewUtils>(GetDocument().View());
  }

  void TearDown() override {
    web_view_helper_.Reset();
    utils_ = nullptr;
  }

  AnimationMockChromeClient& GetAnimationMockChromeClient() const {
    return *chrome_client_;
  }

  Document& GetDocument() {
    return *static_cast<Document*>(
        web_view_helper_.LocalMainFrame()->GetDocument());
  }

  void CreateTallDocument() {
    SetBodyInnerHTML(R"HTML(
      <style>body { margin:0; }</style>
      <div style='height:2000px'></div>
    )HTML");
    GetDocument().View()->Resize(gfx::Size(400, 600));
    GetDocument().View()->UpdateAllLifecyclePhasesForTest();
  }

  void CreateShortDocument() {
    SetBodyInnerHTML(R"HTML(
      <style>body { margin:0; }</style>
      <div style='height:500px'></div>
    )HTML");
    GetDocument().View()->Resize(gfx::Size(400, 600));
    GetDocument().View()->UpdateAllLifecyclePhasesForTest();
  }

  std::unique_ptr<LocalFrameViewUtils> utils_;

 private:
  Persistent<AnimationMockChromeClient> chrome_client_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(LocalFrameViewUtilsTest, UpdateCompositedSelectionIfNeed) {
  SetBodyInnerHTML("<div></div>");
  utils_->UpdateCompositedSelectionIfNeed();
}

TEST_F(LocalFrameViewUtilsTest, UpdateCompositedSelectionIfNeed_NoFocusedFrame) {
  SetBodyInnerHTML("<div></div>");
  GetDocument().GetPage()->GetFocusController().SetFocusedFrame(nullptr);
  utils_->UpdateCompositedSelectionIfNeed();
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_NullDocument) {
  utils_->PerformLayoutOnPreload(nullptr);
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_HeightExceeded) {
  CreateTallDocument();
  utils_->PerformLayoutOnPreload(&GetDocument());
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_HeightNotExceeded) {
  CreateShortDocument();
  utils_->PerformLayoutOnPreload(&GetDocument());
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_NoLayoutObject) {
  SetBodyInnerHTML("");
  GetDocument().body()->remove();
  utils_->PerformLayoutOnPreload(&GetDocument());
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_EmptyUrl) {
  CreateShortDocument();
  GetDocument().SetURL(KURL(""));
  utils_->PerformLayoutOnPreload(&GetDocument());
}

TEST_F(LocalFrameViewUtilsTest, UpdateCompositedSelectionIfNeed1) {
  SetBodyInnerHTML("<div></div>");
  utils_->UpdateCompositedSelectionIfNeed();
}

TEST_F(LocalFrameViewUtilsTest, UpdateCompositedSelectionIfNeed_NoFocusedFrame1) {
  SetBodyInnerHTML("<div></div>");
  GetDocument().GetPage()->GetFocusController().SetFocusedFrame(nullptr);
  utils_->UpdateCompositedSelectionIfNeed();
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_NullDocument1) {
  utils_->PerformLayoutOnPreload(nullptr);
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_HeightExceeded1) {
  CreateTallDocument();
  utils_->PerformLayoutOnPreload(&GetDocument());
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_HeightNotExceeded1) {
  CreateShortDocument();
  utils_->PerformLayoutOnPreload(&GetDocument());
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_NoLayoutObject1) {
  SetBodyInnerHTML("");
  GetDocument().body()->remove();
  utils_->PerformLayoutOnPreload(&GetDocument());
}

TEST_F(LocalFrameViewUtilsTest, PerformLayoutOnPreload_EmptyUrl1) {
  CreateShortDocument();
  GetDocument().SetURL(KURL(""));
  utils_->PerformLayoutOnPreload(&GetDocument());
}

}  // namespace
}  // namespace blink