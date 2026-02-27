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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/media/media_remoting_interstitial_listener.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/events/mouse_event.h"
#include "third_party/blink/renderer/core/events/touch_event.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#include "third_party/blink/renderer/core/html/media/media_remoting_interstitial.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"

namespace blink {

class ProgressBarEventListenerFullCoverageTest : public PageTestBase {
 protected:
  void SetUp() override {
    PageTestBase::SetUp();
    GetDocument().documentElement()->setInnerHTML(
        "<div id='progress' style='position:absolute; left:10px; width:200px; height:20px;'></div>"
        "<video id='video'></video>");
    UpdateAllLifecyclePhasesForTest();
    progress_bar_ = GetDocument().getElementById(AtomicString("progress"));
    ASSERT_TRUE(progress_bar_);

    auto* video = GetDocument().getElementById(AtomicString("video"));
    auto* video_element = DynamicTo<HTMLVideoElement>(video);
    ASSERT_TRUE(video_element);
    interstitial_ = MakeGarbageCollected<MediaRemotingInterstitial>(*video_element);

    listener_ = MakeGarbageCollected<ProgressBarEventListener>(
        WeakMember<MediaRemotingInterstitial>(interstitial_), progress_bar_);
  }

  void TearDown() override {
    listener_ = nullptr;
    progress_bar_ = nullptr;
    interstitial_ = nullptr;
    PageTestBase::TearDown();
  }

  MouseEvent* CreateMouseEvent(const AtomicString& type, float client_x, uint16_t buttons = 0) {
    MouseEvent* event = MouseEvent::Create();
    LocalFrame& frame = GetFrame();
    LocalDOMWindow* dom_window = frame.DomWindow();
    ScriptState* script_state = ToScriptStateForMainWorld(&frame);
    event->initMouseEvent(script_state, type, true, true, dom_window, 0,
                          client_x, 0, client_x, 0, false, false, false, false,
                          0, nullptr, buttons);
    event->SetTarget(progress_bar_.Get());
    return event;
  }

  TouchEvent* CreateEmptyTouchEvent(const AtomicString& type) {
    TouchEvent* event = TouchEvent::Create();
    event->SetType(type);
    event->SetTargetTouches(TouchList::Create());
    event->SetChangedTouches(TouchList::Create());
    return event;
  }

  Persistent<Element> progress_bar_;
  Persistent<ProgressBarEventListener> listener_;
  Persistent<MediaRemotingInterstitial> interstitial_;
};

TEST_F(ProgressBarEventListenerFullCoverageTest, InvokeAllEventTypes) {
  Vector<AtomicString> types = {
      event_type_names::kClick,
      event_type_names::kMousedown,
      event_type_names::kMousemove,
      event_type_names::kMouseup,
      event_type_names::kMouseleave,
      event_type_names::kTouchstart,
      event_type_names::kTouchmove,
      event_type_names::kTouchend,
      event_type_names::kTouchcancel,
  };
  for (const auto& type : types) {
    Event* evt = Event::Create(type);
    EXPECT_NO_FATAL_FAILURE(listener_->Invoke(nullptr, evt));
  }
}

TEST_F(ProgressBarEventListenerFullCoverageTest, MouseDragSequence) {
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 50.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 80.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 120.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMouseup, 150.0f, 0));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 60.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMouseleave, 90.0f, 0));
}

TEST_F(ProgressBarEventListenerFullCoverageTest, TouchEmptyLists) {
  listener_->Invoke(nullptr, CreateEmptyTouchEvent(event_type_names::kTouchstart));
  listener_->Invoke(nullptr, CreateEmptyTouchEvent(event_type_names::kTouchmove));
  listener_->Invoke(nullptr, CreateEmptyTouchEvent(event_type_names::kTouchend));
  listener_->Invoke(nullptr, CreateEmptyTouchEvent(event_type_names::kTouchcancel));
}

TEST_F(ProgressBarEventListenerFullCoverageTest, InvalidWidthLogging) {
  GetDocument().documentElement()->setInnerHTML(
      "<div id='progress' style='position:absolute; left:10px; width:0px; height:20px;'></div>"
      "<video id='video'></video>");
  UpdateAllLifecyclePhasesForTest();
  progress_bar_ = GetDocument().getElementById(AtomicString("progress"));
  ASSERT_TRUE(progress_bar_);
  auto* video = GetDocument().getElementById(AtomicString("video"));
  auto* video_element = DynamicTo<HTMLVideoElement>(video);
  ASSERT_TRUE(video_element);
  interstitial_ = MakeGarbageCollected<MediaRemotingInterstitial>(*video_element);
  listener_ = MakeGarbageCollected<ProgressBarEventListener>(
      WeakMember<MediaRemotingInterstitial>(interstitial_), progress_bar_);
  ASSERT_TRUE(progress_bar_->GetLayoutObject());
  EXPECT_EQ(progress_bar_->OffsetWidth(), 0);

  testing::internal::CaptureStderr();
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kClick, 110.0f));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(output.find("Invalid progress bar width") != std::string::npos);

  testing::internal::CaptureStderr();
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 50.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 80.0f, 1));
  output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(output.find("Invalid progress bar width") != std::string::npos);
}

TEST_F(ProgressBarEventListenerFullCoverageTest, NullEventHandling) {
  EXPECT_NO_FATAL_FAILURE(listener_->Invoke(nullptr, nullptr));
}

TEST_F(ProgressBarEventListenerFullCoverageTest, NormalWidthPercentage) {
  testing::internal::CaptureStderr();
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kClick, 50.0f));
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(output.find("Invalid progress bar width") == std::string::npos);

  testing::internal::CaptureStderr();
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 30.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 80.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMouseup, 150.0f, 0));
  output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(output.find("Invalid progress bar width") == std::string::npos);
}

TEST_F(ProgressBarEventListenerFullCoverageTest, ProgressBarRemoved) {
  progress_bar_->remove();
  UpdateAllLifecyclePhasesForTest();
  EXPECT_FALSE(progress_bar_->GetLayoutObject());
  EXPECT_NO_FATAL_FAILURE(
      listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kClick, 50.0f)));
  EXPECT_NO_FATAL_FAILURE(
      listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 50.0f, 1)));
  EXPECT_NO_FATAL_FAILURE(
      listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 80.0f, 1)));
  EXPECT_NO_FATAL_FAILURE(
      listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMouseup, 150.0f, 0)));
}

TEST_F(ProgressBarEventListenerFullCoverageTest, DragOutOfBounds) {
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 50.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, -10.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 300.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 100.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMouseup, 150.0f, 0));
}

TEST_F(ProgressBarEventListenerFullCoverageTest, RapidDrag) {
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 50.0f, 1));
  for (int32_t x = 55; x <= 150; x += 5) {
    listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, static_cast<float>(x), 1));
  }
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMouseup, 150.0f, 0));
}

TEST_F(ProgressBarEventListenerFullCoverageTest, MouseLeaveAndReturn) {
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 50.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMouseleave, 90.0f, 0));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 70.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 120.0f, 1));
  listener_->Invoke(nullptr, CreateMouseEvent(event_type_names::kMouseup, 150.0f, 0));
}

TEST_F(ProgressBarEventListenerFullCoverageTest, NullInterstitial) {
  auto* null_listener = MakeGarbageCollected<ProgressBarEventListener>(
      WeakMember<MediaRemotingInterstitial>(), progress_bar_);
  EXPECT_NO_FATAL_FAILURE(
      null_listener->Invoke(nullptr, CreateMouseEvent(event_type_names::kClick, 50.0f)));
  EXPECT_NO_FATAL_FAILURE(
      null_listener->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousedown, 50.0f, 1)));
  EXPECT_NO_FATAL_FAILURE(
      null_listener->Invoke(nullptr, CreateMouseEvent(event_type_names::kMousemove, 80.0f, 1)));
}

}  // namespace blink