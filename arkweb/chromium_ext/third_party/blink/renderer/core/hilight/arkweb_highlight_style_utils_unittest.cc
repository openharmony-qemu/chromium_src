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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/hilight/arkweb_highlight_style_utils.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "components/shared_highlighting/core/common/fragment_directives_constants.h"
#include "third_party/blink/renderer/core/dom/document.h"
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
class ArkWebHighlightStyleUtilsTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    page_holder_ = std::make_unique<DummyPageHolder>();
    local_frame_ = &page_holder_->GetFrame();
    document_ = &page_holder_->GetDocument();
    dom_window_ = document_->domWindow();
  }

  void TearDown() override { page_holder_.reset(); }

  void SetArkwebAgentEnabled(bool enabled) {
    document_->GetSettings()->SetArkwebAgentEnabled(enabled);
  }

  std::unique_ptr<DummyPageHolder> page_holder_;
  Persistent<LocalFrame> local_frame_;
  Persistent<Document> document_;
  Persistent<LocalDOMWindow> dom_window_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(ArkWebHighlightStyleUtilsTest, GetTargetTextForegroundColor_default) {
  SetArkwebAgentEnabled(false);
  EXPECT_EQ(ArkWebHighlightStyleUtils::GetTargetTextForegroundColor(
                *document_, mojom::blink::ColorScheme::kLight),
            Color::kBlack);
}

TEST_F(ArkWebHighlightStyleUtilsTest, GetTargetTextForegroundColor_kLight) {
  SetArkwebAgentEnabled(true);
  EXPECT_EQ(ArkWebHighlightStyleUtils::GetTargetTextForegroundColor(
                *document_, mojom::blink::ColorScheme::kLight),
            Color::kBlack);
}

TEST_F(ArkWebHighlightStyleUtilsTest, GetTargetTextForegroundColor_kDark) {
  SetArkwebAgentEnabled(true);
  EXPECT_EQ(ArkWebHighlightStyleUtils::GetTargetTextForegroundColor(
                *document_, mojom::blink::ColorScheme::kDark),
            Color::kWhite);
}

TEST_F(ArkWebHighlightStyleUtilsTest, GetTargetTextBackgroundColor_default) {
  SetArkwebAgentEnabled(false);
  EXPECT_EQ(
      ArkWebHighlightStyleUtils::GetTargetTextBackgroundColor(
          *document_, mojom::blink::ColorScheme::kLight),
      Color::FromRGBA32(shared_highlighting::kFragmentTextBackgroundColorARGB));
}

TEST_F(ArkWebHighlightStyleUtilsTest, GetTargetTextBackgroundColor_kLight) {
  SetArkwebAgentEnabled(true);
  EXPECT_EQ(
      ArkWebHighlightStyleUtils::GetTargetTextBackgroundColor(
          *document_, mojom::blink::ColorScheme::kLight),
      Color::FromRGBA32(ArkWebHighlightStyleUtils::kTargetTextBackgroundColor));
}

TEST_F(ArkWebHighlightStyleUtilsTest, GetTargetTextBackgroundColor_kDark) {
  SetArkwebAgentEnabled(true);
  EXPECT_EQ(ArkWebHighlightStyleUtils::GetTargetTextBackgroundColor(
                *document_, mojom::blink::ColorScheme::kDark),
            Color::FromRGBA32(
                ArkWebHighlightStyleUtils::kTargetTextBackgroundColorDark));
}
}  // namespace blink