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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#define private public
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/first_screen_calculator.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/paint/timing/image_paint_timing_detector.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing_detector.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/core/paint/timing/text_paint_timing_detector.h"

namespace blink {
namespace {

class FirstScreenCalculatorTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    dummy_page_holder_ = std::make_unique<DummyPageHolder>(gfx::Size(800, 600));
    local_frame_ = &dummy_page_holder_->GetFrame();
    calculator_ = MakeGarbageCollected<FirstScreenCalculator>(local_frame_.get());
  }
  void TearDown() override {}

  std::unique_ptr<DummyPageHolder> dummy_page_holder_;
  raw_ptr<LocalFrame> local_frame_;
  Member<FirstScreenCalculator> calculator_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(FirstScreenCalculatorTest, NotifyImagePaint) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;
  calculator_->NotifyImagePaint(0, nullptr, 10, false);
  EXPECT_EQ(calculator_->viewport_rect_.size().GetArea(), 0);
  calculator_->user_scrolled_ = true;
  gfx::Rect rect(0, 0, 10, 10);
  gfx::RectF root_rectF(0, 0, 10, 10);
  gfx::Rect root_rect(0, 0, 10, 10);
  ImageResourceContent* content = ImageResourceContent::CreateNotStarted();
  ASSERT_NE(content, nullptr);
  ImageRecord record(1, content, 20, rect, root_rectF, 1);
  calculator_->NotifyImagePaint(0, &record, 10, false);
  EXPECT_EQ(calculator_->viewport_rect_.size().GetArea(), 0);
  calculator_->user_scrolled_ = false;
  record.lcp_rect_info_ = nullptr;
  calculator_->NotifyImagePaint(0, &record, 10, false);
  EXPECT_EQ(calculator_->viewport_rect_.size().GetArea(), 0);

  record.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect, root_rect);
  calculator_->user_scrolled_ = false;
  calculator_->NotifyImagePaint(0, &record, 10, false);
  EXPECT_EQ(calculator_->viewport_rect_.size().GetArea(), 480000);
}

TEST_F(FirstScreenCalculatorTest, NotifyTextPaint) {
  ASSERT_NE(calculator_, nullptr);
  base::TimeTicks timestamp = base::TimeTicks::Now();
  calculator_->user_scrolled_ = false;
  calculator_->NotifyTextPaint(nullptr, timestamp);
  EXPECT_EQ(calculator_->viewport_rect_.size().GetArea(), 0);

  calculator_->user_scrolled_ = true;
  gfx::Rect rect(0, 0, 10, 10);
  gfx::RectF root_rect(0, 0, 10, 10);
   const char* body_content =
      "<a id=one href='http://www.msn.com'>one</a><b id=two>two</b>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  ASSERT_NE(one, nullptr);
  TextRecord record(*one, 10, root_rect, rect, root_rect, 0);
  calculator_->NotifyTextPaint(&record, timestamp);
  EXPECT_EQ(calculator_->viewport_rect_.size().GetArea(), 0);

  calculator_->user_scrolled_ = false;
  record.lcp_rect_info_ = nullptr;
  calculator_->NotifyTextPaint(&record, timestamp);
  EXPECT_EQ(calculator_->viewport_rect_.size().GetArea(), 0);
}

TEST_F(FirstScreenCalculatorTest, AssignImagePaintTime) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->first_screen_paint_time_ = base::TimeTicks();
  calculator_->background_image_id_ = 10;
  
  gfx::Rect rect(0, 0, 10, 10);
  base::TimeTicks timestamp = base::TimeTicks::Now();
  calculator_->AssignImagePaintTime(10, rect, timestamp);
  EXPECT_EQ(calculator_->first_screen_paint_time_.ToInternalValue(), timestamp.ToInternalValue());

  calculator_->first_screen_paint_time_ = base::TimeTicks();
  calculator_->user_scrolled_ = true;
  calculator_->AssignImagePaintTime(9, rect, timestamp);
  EXPECT_EQ(calculator_->first_screen_paint_time_.ToInternalValue(), base::TimeTicks().ToInternalValue());

  calculator_->first_screen_paint_time_ = base::TimeTicks();
  calculator_->user_scrolled_ = false;
  calculator_->AssignImagePaintTime(9, rect, timestamp);
  EXPECT_EQ(calculator_->first_screen_paint_time_.ToInternalValue(), base::TimeTicks().ToInternalValue());

  calculator_->first_screen_paint_time_ = base::TimeTicks();
  calculator_->user_scrolled_ = true;
  calculator_->AssignImagePaintTime(9, rect, timestamp);
  EXPECT_EQ(calculator_->first_screen_paint_time_.ToInternalValue(), base::TimeTicks().ToInternalValue());
}

TEST_F(FirstScreenCalculatorTest, RemoveImageRecord) {

  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = true;
  EXPECT_EQ(calculator_->RemoveImageRecord(10), false);

  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;
  FirstScreenCalculator::PaintRectInfo info;
  calculator_->image_rects_map_[10] = info;
  calculator_->image_rects_map_[10].paint_time_ = base::TimeTicks::Now();
  EXPECT_EQ(calculator_->RemoveImageRecord(10), false);

  calculator_->image_rects_map_[10].paint_time_ = base::TimeTicks();
  EXPECT_EQ(calculator_->RemoveImageRecord(10), true);
}

TEST_F(FirstScreenCalculatorTest, OnUserScroll) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;
  calculator_->OnUserScroll();
  EXPECT_EQ(calculator_->HasUserScrolled(), true);
}

TEST_F(FirstScreenCalculatorTest, RestartRecordingFirstScreenPaint) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->RestartRecordingFirstScreenPaint();
  EXPECT_EQ(calculator_->user_scrolled_, false);
  EXPECT_EQ(calculator_->nearly_finished_, false);
  EXPECT_EQ(calculator_->first_screen_paint_time_, base::TimeTicks());
  EXPECT_EQ(calculator_->background_image_id_, 0ul);
}

TEST_F(FirstScreenCalculatorTest, OnFirstScreenInvoked) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->first_screen_paint_time_ = base::TimeTicks();
  calculator_->navigation_start_time_ = base::TimeTicks();
  calculator_->OnFirstScreenInvoked();
  EXPECT_EQ(calculator_->navigation_start_time_, base::TimeTicks());

  calculator_->first_screen_paint_time_ = base::TimeTicks::Now();
  calculator_->OnFirstScreenInvoked();
  EXPECT_NE(calculator_->navigation_start_time_, base::TimeTicks());
}

TEST_F(FirstScreenCalculatorTest, RestartTimerForFirstScreenDetection) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = true;
  calculator_->RestartTimerForFirstScreenDetection();
  EXPECT_EQ(calculator_->timer_.IsRunning(), false);

  calculator_->user_scrolled_ = false;
  calculator_->RestartTimerForFirstScreenDetection();
  EXPECT_EQ(calculator_->timer_.IsRunning(), true);
}

TEST_F(FirstScreenCalculatorTest, IsRectContainedByExistingRects) {
  ASSERT_NE(calculator_, nullptr);
  gfx::Rect root_rect(0, 0, 10, 10);
  EXPECT_EQ(calculator_->IsRectContainedByExistingRects(root_rect), false);

  gfx::Rect rect1(0, 0, 10, 10);
  FirstScreenCalculator::PaintRectInfo info1(rect1, base::TimeTicks());
  calculator_->image_rects_map_[10] = info1;
  EXPECT_EQ(calculator_->IsRectContainedByExistingRects(root_rect), true);

  gfx::Rect rect2(0, 0, 20, 20);
  FirstScreenCalculator::PaintRectInfo info2(rect2, base::TimeTicks());
  calculator_->image_rects_map_[10] = info2;
  EXPECT_EQ(calculator_->IsRectContainedByExistingRects(root_rect), true);

  calculator_->image_rects_map_.clear();
  calculator_->text_paint_rects_.emplace_back(info1);
  EXPECT_EQ(calculator_->IsRectContainedByExistingRects(root_rect), true);

  calculator_->text_paint_rects_.clear();
  calculator_->text_paint_rects_.emplace_back(info2);
  EXPECT_EQ(calculator_->IsRectContainedByExistingRects(root_rect), true);
}

TEST_F(FirstScreenCalculatorTest, DoesRectIntersectExistingRects) {
  ASSERT_NE(calculator_, nullptr);
  gfx::Rect root_rect(0, 0, 10, 10);
  EXPECT_EQ(calculator_->DoesRectIntersectExistingRects(root_rect), false);

  gfx::Rect rect1(0, 0, 10, 10);
  FirstScreenCalculator::PaintRectInfo info1(rect1, base::TimeTicks());
  calculator_->image_rects_map_[10] = info1;
  EXPECT_EQ(calculator_->DoesRectIntersectExistingRects(root_rect), false);

  gfx::Rect rect2(0, 0, 20, 20);
  FirstScreenCalculator::PaintRectInfo info2(rect2, base::TimeTicks());
  calculator_->image_rects_map_[10] = info2;
  EXPECT_EQ(calculator_->DoesRectIntersectExistingRects(root_rect), true);

  calculator_->image_rects_map_.clear();
  calculator_->text_paint_rects_.emplace_back(info1);
  EXPECT_EQ(calculator_->DoesRectIntersectExistingRects(root_rect), false);

  calculator_->text_paint_rects_.clear();
  calculator_->text_paint_rects_.emplace_back(info2);
  EXPECT_EQ(calculator_->DoesRectIntersectExistingRects(root_rect), true);
}

TEST_F(FirstScreenCalculatorTest, RemoveExistingRectsContainedByRect) {
  ASSERT_NE(calculator_, nullptr);
  gfx::Rect root_rect(0, 0, 10, 10);
  calculator_->RemoveExistingRectsContainedByRect(root_rect);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 0ul);

  gfx::Rect rect1(0, 0, 5, 5);
  FirstScreenCalculator::PaintRectInfo info1(rect1, base::TimeTicks());
  calculator_->image_rects_map_[10] = info1;
  calculator_->RemoveExistingRectsContainedByRect(root_rect);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 0ul);

  gfx::Rect rect2(0, 0, 20, 20);
  FirstScreenCalculator::PaintRectInfo info2(rect2, base::TimeTicks());
  calculator_->image_rects_map_[10] = info2;
  calculator_->RemoveExistingRectsContainedByRect(root_rect);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);

  calculator_->image_rects_map_.clear();
  calculator_->text_paint_rects_.clear();
  calculator_->text_paint_rects_.emplace_back(info1);
  calculator_->RemoveExistingRectsContainedByRect(root_rect);
  EXPECT_EQ(calculator_->text_paint_rects_.size(), 0ul);

  calculator_->text_paint_rects_.emplace_back(info2);
  calculator_->RemoveExistingRectsContainedByRect(root_rect);
  EXPECT_EQ(calculator_->text_paint_rects_.size(), 1ul);
}

TEST_F(FirstScreenCalculatorTest, IsRectTooSmallWhenNearlyFinished) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->viewport_rect_ = gfx::Rect(0,0,0,0);
  gfx::Rect root_rect(0, 0, 10, 10);
  EXPECT_EQ(calculator_->IsRectTooSmallWhenNearlyFinished(root_rect), false);

  gfx::Rect rect1(0, 0, 10, 10);
  calculator_->viewport_rect_ = rect1;
  calculator_->nearly_finished_ = false;
  calculator_->occupied_rect_ = rect1;
  EXPECT_EQ(calculator_->IsRectTooSmallWhenNearlyFinished(root_rect), false);
  EXPECT_EQ(calculator_->nearly_finished_, true);
}

TEST_F(FirstScreenCalculatorTest, GetViewportAreaAndTrimRect) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->viewport_rect_ = gfx::Rect(0,0,800,0);
  gfx::Rect root_rect(0, 0, 10, 10);
  EXPECT_EQ(calculator_->GetViewportAreaAndTrimRect(root_rect), true);
  EXPECT_EQ(calculator_->viewport_rect_, gfx::Rect(0,0,800,600));

  gfx::Rect rect1(1000, 1000, 10, 10);
  EXPECT_EQ(calculator_->GetViewportAreaAndTrimRect(rect1), false);
}

TEST_F(FirstScreenCalculatorTest, NotifyImagePaintWithVideo) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect(0, 0, 10, 10);
  gfx::RectF root_rectF(0, 0, 10, 10);
  gfx::Rect root_rect(0, 0, 10, 10);
  ImageResourceContent* content = ImageResourceContent::CreateNotStarted();
  ASSERT_NE(content, nullptr);
  ImageRecord record(1, content, 20, rect, root_rectF, 1);
  record.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect, root_rect);

  calculator_->NotifyImagePaint(0, &record, 10, true);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);

  auto it = calculator_->image_rects_map_.find(0);
  EXPECT_NE(it, calculator_->image_rects_map_.end());
  EXPECT_EQ(it->second.rect_.x(), 0);
  EXPECT_EQ(it->second.rect_.y(), 0);
}

TEST_F(FirstScreenCalculatorTest, NotifyImagePaintWithExistingId) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect(0, 0, 10, 10);
  gfx::RectF root_rectF(0, 0, 10, 10);
  gfx::Rect root_rect(0, 0, 10, 10);
  ImageResourceContent* content = ImageResourceContent::CreateNotStarted();
  ASSERT_NE(content, nullptr);
  ImageRecord record(1, content, 20, rect, root_rectF, 1);
  record.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect, root_rect);

  calculator_->NotifyImagePaint(100, &record, 10, false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);

  calculator_->NotifyImagePaint(100, &record, 10, false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);
}

TEST_F(FirstScreenCalculatorTest, NotifyImagePaintWithIntersectingRect) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect1(0, 0, 10, 10);
  gfx::Rect rect2(5, 5, 10, 10);
  gfx::RectF root_rectF(0, 0, 10, 10);
  gfx::Rect root_rect(0, 0, 10, 10);
  ImageResourceContent* content = ImageResourceContent::CreateNotStarted();
  ASSERT_NE(content, nullptr);

  ImageRecord record1(1, content, 20, rect1, root_rectF, 1);
  record1.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect1, root_rect);

  ImageRecord record2(2, content, 20, rect2, root_rectF, 1);
  record2.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect2, root_rect);

  calculator_->NotifyImagePaint(100, &record1, 10, false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);

  calculator_->NotifyImagePaint(200, &record2, 10, false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 2ul);
  EXPECT_EQ(calculator_->intersected_image_ids_.size(), 1ul);
  EXPECT_EQ(calculator_->intersected_image_ids_[0], 200ul);
}

TEST_F(FirstScreenCalculatorTest, NotifyImagePaintWithLargeImageAsBackground) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect(0, 0, 700, 500);
  gfx::RectF root_rectF(0, 0, 700, 500);
  gfx::Rect root_rect(0, 0, 700, 500);
  ImageResourceContent* content = ImageResourceContent::CreateNotStarted();
  ASSERT_NE(content, nullptr);
  ImageRecord record(1, content, 20, rect, root_rectF, 1);
  record.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect, root_rect);

  calculator_->NotifyImagePaint(100, &record, 10, false);
  EXPECT_EQ(calculator_->background_image_id_, 100);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 0ul);
}

TEST_F(FirstScreenCalculatorTest, NotifyTextPaintWithExistingPaintTime) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect(0, 0, 10, 10);
  gfx::RectF root_rect(0, 0, 10, 10);

  const char* body_content =
      "<a id=one href='http://www.msn.com'>one</a><b id=two>two</b>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  ASSERT_NE(one, nullptr);

  TextRecord record(*one, 10, root_rect, rect, root_rect, 0);
  record.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect, gfx::Rect(0, 0, 10, 10));

  base::TimeTicks timestamp1 = base::TimeTicks::Now();
  calculator_->NotifyTextPaint(&record, timestamp1);
  EXPECT_EQ(calculator_->first_screen_paint_time_, timestamp1);

  base::TimeTicks timestamp2 = timestamp1 + base::Milliseconds(100);
  calculator_->NotifyTextPaint(&record, timestamp2);
  EXPECT_EQ(calculator_->first_screen_paint_time_, timestamp2);

  base::TimeTicks timestamp3 = timestamp1 - base::Milliseconds(100);
  calculator_->NotifyTextPaint(&record, timestamp3);
  EXPECT_EQ(calculator_->first_screen_paint_time_, timestamp2);
}

TEST_F(FirstScreenCalculatorTest, NotifyTextPaintWithIntersectingRect) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect1(0, 0, 10, 10);
  gfx::Rect rect2(5, 5, 10, 10);
  gfx::RectF root_rect(0, 0, 10, 10);

  const char* body_content =
      "<a id=one href='http://www.msn.com'>one</a><b id=two>two</b>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  ASSERT_NE(one, nullptr);

  TextRecord record1(*one, 10, root_rect, rect1, root_rect, 0);
  record1.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect1, gfx::Rect(0, 0, 10, 10));

  TextRecord record2(*one, 10, root_rect, rect2, root_rect, 0);
  record2.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect2, gfx::Rect(0, 0, 10, 10));

  base::TimeTicks timestamp = base::TimeTicks::Now();
  calculator_->NotifyTextPaint(&record1, timestamp);
  EXPECT_EQ(calculator_->text_paint_rects_.size(), 1ul);

  calculator_->NotifyTextPaint(&record2, timestamp);
  EXPECT_EQ(calculator_->text_paint_rects_.size(), 2ul);
}

TEST_F(FirstScreenCalculatorTest, AssignImagePaintTimeWithBackgroundId) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->first_screen_paint_time_ = base::TimeTicks();
  calculator_->background_image_id_ = 10;

  gfx::Rect rect(0, 0, 10, 10);
  base::TimeTicks timestamp = base::TimeTicks::Now();

  calculator_->user_scrolled_ = false;
  calculator_->AssignImagePaintTime(10, rect, timestamp);
  EXPECT_EQ(calculator_->first_screen_paint_time_, timestamp);
}

TEST_F(FirstScreenCalculatorTest, AssignImagePaintTimeWithExistingPaintTime) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->first_screen_paint_time_ = base::TimeTicks();

  gfx::Rect rect(0, 0, 10, 10);
  FirstScreenCalculator::PaintRectInfo info(rect, base::TimeTicks::Now());
  calculator_->image_rects_map_[10] = info;

  base::TimeTicks timestamp = base::TimeTicks::Now();
  calculator_->AssignImagePaintTime(10, rect, timestamp);

  EXPECT_FALSE(calculator_->image_rects_map_[10].paint_time_.is_null());
}

TEST_F(FirstScreenCalculatorTest, AssignImagePaintTimeWithIntersectedId) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->first_screen_paint_time_ = base::TimeTicks();

  gfx::Rect rect(0, 0, 10, 10);
  FirstScreenCalculator::PaintRectInfo info(rect, base::TimeTicks());
  calculator_->image_rects_map_[10] = info;
  calculator_->intersected_image_ids_.emplace_back(10);

  base::TimeTicks timestamp = base::TimeTicks::Now();
  calculator_->AssignImagePaintTime(10, rect, timestamp);

  EXPECT_TRUE(calculator_->first_screen_paint_time_.is_null());
}

TEST_F(FirstScreenCalculatorTest, OnFirstScreenInvokedWithNullFrame) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->local_frame_ = nullptr;
  calculator_->first_screen_paint_time_ = base::TimeTicks::Now();

  calculator_->OnFirstScreenInvoked();
}

TEST_F(FirstScreenCalculatorTest, RestartTimerWithNullFrame) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;
  calculator_->local_frame_ = nullptr;

  calculator_->RestartTimerForFirstScreenDetection();
  EXPECT_FALSE(calculator_->timer_.IsRunning());
}

TEST_F(FirstScreenCalculatorTest, RemoveImageRecordWithPaintTime) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect(0, 0, 10, 10);
  FirstScreenCalculator::PaintRectInfo info(rect, base::TimeTicks::Now());
  calculator_->image_rects_map_[10] = info;

  EXPECT_EQ(calculator_->RemoveImageRecord(10), false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);
}

TEST_F(FirstScreenCalculatorTest, GetViewportAreaWithEmptyRect) {
  ASSERT_NE(calculator_, nullptr);

  gfx::Rect rect(0, 0, 10, 10);
  calculator_->viewport_rect_ = gfx::Rect(0, 0, 800, 600);

  gfx::Rect outside_rect(1000, 1000, 10, 10);
  EXPECT_EQ(calculator_->GetViewportAreaAndTrimRect(outside_rect), false);
}

TEST_F(FirstScreenCalculatorTest, GetPaintRectsWithNullPaintTime) {
  ASSERT_NE(calculator_, nullptr);

  gfx::Rect rect(0, 0, 10, 10);
  FirstScreenCalculator::PaintRectInfo info1(rect, base::TimeTicks());
  FirstScreenCalculator::PaintRectInfo info2(rect, base::TimeTicks::Now());

  calculator_->image_rects_map_[10] = info1;
  calculator_->image_rects_map_[20] = info2;
  calculator_->text_paint_rects_.emplace_back(info1);
  calculator_->text_paint_rects_.emplace_back(info2);

  std::vector<gfx::Rect> paint_rects;
  calculator_->GetPaintRects(paint_rects);

  EXPECT_EQ(paint_rects.size(), 2ul);
}

TEST_F(FirstScreenCalculatorTest, NotifyImagePaintContainmentCheck) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect1(0, 0, 20, 20);
  gfx::Rect rect2(0, 0, 10, 10);
  gfx::RectF root_rectF(0, 0, 20, 20);
  gfx::Rect root_rect(0, 0, 20, 20);

  ImageResourceContent* content = ImageResourceContent::CreateNotStarted();
  ASSERT_NE(content, nullptr);

  ImageRecord record1(1, content, 20, rect1, root_rectF, 1);
  record1.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect1, root_rect);

  ImageRecord record2(2, content, 20, rect2, root_rectF, 1);
  record2.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect2, root_rect);

  calculator_->NotifyImagePaint(100, &record1, 10, false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);

  calculator_->NotifyImagePaint(200, &record2, 10, false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);
}

TEST_F(FirstScreenCalculatorTest, NotifyImagePaintRemovesContainedRects) {
  ASSERT_NE(calculator_, nullptr);
  calculator_->user_scrolled_ = false;

  gfx::Rect rect1(0, 0, 10, 10);
  gfx::Rect rect2(0, 0, 20, 20);
  gfx::RectF root_rectF(0, 0, 20, 20);
  gfx::Rect root_rect(0, 0, 20, 20);

  ImageResourceContent* content = ImageResourceContent::CreateNotStarted();
  ASSERT_NE(content, nullptr);

  ImageRecord record1(1, content, 20, rect1, root_rectF, 1);
  record1.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect1, root_rect);

  ImageRecord record2(2, content, 20, rect2, root_rectF, 1);
  record2.lcp_rect_info_ = std::make_unique<LCPRectInfo>(rect2, root_rect);

  calculator_->NotifyImagePaint(100, &record1, 10, false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);

  calculator_->NotifyImagePaint(200, &record2, 10, false);
  EXPECT_EQ(calculator_->image_rects_map_.size(), 1ul);
  EXPECT_EQ(calculator_->image_rects_map_.count(100), 0ul);
  EXPECT_EQ(calculator_->image_rects_map_.count(200), 1ul);
}

TEST_F(FirstScreenCalculatorTest, IsRectTooSmallWhenNearlyFinishedTrue) {
  ASSERT_NE(calculator_, nullptr);

  gfx::Rect viewport(0, 0, 800, 600);
  calculator_->viewport_rect_ = viewport;
  calculator_->occupied_rect_ = gfx::Rect(0, 0, 750, 600);
  calculator_->nearly_finished_ = true;

  gfx::Rect small_rect(0, 0, 5, 5);
  EXPECT_EQ(calculator_->IsRectTooSmallWhenNearlyFinished(small_rect), true);
}

TEST_F(FirstScreenCalculatorTest, IsRectContainedByExistingRectsExactMatch) {
  ASSERT_NE(calculator_, nullptr);

  gfx::Rect rect(10, 10, 50, 50);
  FirstScreenCalculator::PaintRectInfo info(rect, base::TimeTicks());
  calculator_->image_rects_map_[1] = info;

  gfx::Rect same_rect(10, 10, 50, 50);
  EXPECT_EQ(calculator_->IsRectContainedByExistingRects(same_rect), true);
}

}  // namespace
}  // namespace blink