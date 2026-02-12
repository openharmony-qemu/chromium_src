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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/fragment_directive/text_fragment_anchor_utils.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "base/timer/timer.h"
#include "components/shared_highlighting/core/common/fragment_directives_constants.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/fragment_directive/text_fragment_handler.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/page/page_utils.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/testing/testing_platform_support.h"

namespace blink {
class TextFragmentAnchorUtilsTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    page_holder_ = std::make_unique<DummyPageHolder>();
    local_frame_ = &page_holder_->GetFrame();
    document_ = &page_holder_->GetDocument();
    dom_window_ = document_->domWindow();
    KURL url = KURL("test.com");
    text_fragment_anchor_ =
        TextFragmentAnchor::TryCreate(url, *local_frame_, true);
    text_fragment_anchor_utils_ = MakeGarbageCollected<TextFragmentAnchorUtils>(
        *local_frame_, true, text_fragment_anchor_);
  }

  void TearDown() override { page_holder_.reset(); }

  void SetArkwebAgentEnabled(bool enabled) {
    local_frame_->GetSettings()->SetArkwebAgentEnabled(enabled);
  }

  std::unique_ptr<DummyPageHolder> page_holder_;
  TextFragmentAnchor* text_fragment_anchor_;
  Persistent<TextFragmentAnchorUtils> text_fragment_anchor_utils_;
  Persistent<LocalFrame> local_frame_;
  Persistent<Document> document_;
  Persistent<LocalDOMWindow> dom_window_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(TextFragmentAnchorUtilsTest, StartHighlightFadeTimer_001) {
  text_fragment_anchor_utils_->frame_ = nullptr;
  text_fragment_anchor_utils_->StartHighlightFadeTimer();
}

TEST_F(TextFragmentAnchorUtilsTest, StartHighlightFadeTimer_002) {
  local_frame_->GetSettings()->SetArkwebAgentEnabled(false);
  local_frame_->GetSettings()->SetAgentNeedHighlight(false);
  text_fragment_anchor_utils_->StartHighlightFadeTimer();
  EXPECT_FALSE(local_frame_->GetSettings()->GetAgentNeedHighlight());
}

TEST_F(TextFragmentAnchorUtilsTest, StartHighlightFadeTimer_003) {
  local_frame_->GetSettings()->SetArkwebAgentEnabled(true);
  local_frame_->GetSettings()->SetAgentNeedHighlight(false);
  text_fragment_anchor_utils_->StartHighlightFadeTimer();
  EXPECT_FALSE(local_frame_->GetSettings()->GetAgentNeedHighlight());
}

TEST_F(TextFragmentAnchorUtilsTest, StartHighlightFadeTimer_004) {
  local_frame_->GetSettings()->SetArkwebAgentEnabled(true);
  local_frame_->GetSettings()->SetAgentNeedHighlight(true);
  text_fragment_anchor_utils_->StartHighlightFadeTimer();
  EXPECT_TRUE(local_frame_->GetSettings()->GetAgentNeedHighlight());
}

TEST_F(TextFragmentAnchorUtilsTest, ShouldIgnoreToken_001) {
  local_frame_->Loader().GetDocumentLoader()->DetachFromFrame(false);
  EXPECT_FALSE(TextFragmentAnchorUtils::ShouldIgnoreToken(
      *(local_frame_->Loader().GetDocumentLoader())));
}

TEST_F(TextFragmentAnchorUtilsTest, ShouldIgnoreToken_002) {
  local_frame_->GetSettings()->SetArkwebAgentEnabled(false);
  EXPECT_FALSE(TextFragmentAnchorUtils::ShouldIgnoreToken(
      *(local_frame_->Loader().GetDocumentLoader())));
}

TEST_F(TextFragmentAnchorUtilsTest, ShouldIgnoreToken_003) {
  local_frame_->GetSettings()->SetArkwebAgentEnabled(true);
  EXPECT_TRUE(TextFragmentAnchorUtils::ShouldIgnoreToken(
      *(local_frame_->Loader().GetDocumentLoader())));
}
}  // namespace blink