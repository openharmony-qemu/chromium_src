/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "third_party/blink/renderer/core/frame/ark_web.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/testing/testing_platform_support.h"

namespace blink {

class ArkWebTest : public RenderingTest {
protected:
    void SetUp() override {
        EnableCompositing();
        RenderingTest::SetUp();
        web_view_helper_.Initialize();
        page_holder_ = std::make_unique<DummyPageHolder>();
        local_frame_ = &page_holder_->GetFrame();
        document_ = &page_holder_->GetDocument();
        dom_window_ = document_->domWindow();
        ark_web_ = MakeGarbageCollected<ArkWeb>(dom_window_);
    }

    void TearDown() override {
        page_holder_.reset();
    }

    std::unique_ptr<DummyPageHolder> page_holder_;
    Persistent<LocalFrame> local_frame_;
    Persistent<Document> document_;
    Persistent<LocalDOMWindow> dom_window_;
    Persistent<ArkWeb> ark_web_;
    frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(ArkWebTest, SetScrollbarAvoidAreaTop) {

  DummyExceptionStateForTesting exception_state;
  const double test_margin = 15.0;

  ark_web_->setScrollbarAvoidAreaTop(test_margin, exception_state);
  
  EXPECT_FALSE(exception_state.HadException());
}

TEST_F(ArkWebTest, ScrollbarAvoidAreaTop) {

  DummyExceptionStateForTesting exception_state;
  const double test_margin = 15.0;

  ark_web_->setScrollbarAvoidAreaTop(test_margin, exception_state);
  double result_margin = ark_web_->scrollbarAvoidAreaTop(exception_state);
}

TEST_F(ArkWebTest, setScrollbarAvoidAreaBottom) {

  DummyExceptionStateForTesting exception_state;
  const double test_margin = 15.0;

  ark_web_->setScrollbarAvoidAreaBottom(test_margin, exception_state);
  
  EXPECT_FALSE(exception_state.HadException());
}

TEST_F(ArkWebTest, scrollbarAvoidAreaBottom) {

  DummyExceptionStateForTesting exception_state;
  const double test_margin = 15.0;

  ark_web_->setScrollbarAvoidAreaBottom(test_margin, exception_state);
  double result_margin = ark_web_->scrollbarAvoidAreaBottom(exception_state);
}

}