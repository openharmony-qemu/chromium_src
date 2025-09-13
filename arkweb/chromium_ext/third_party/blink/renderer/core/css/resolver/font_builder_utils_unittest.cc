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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/css/resolver/font_builder_utils.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/css/resolver/font_builder.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/document_init.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/platform/fonts/font_description.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/platform/testing/testing_platform_support.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "third_party/blink/renderer/core/testing/null_execution_context.h"

#if BUILDFLAG(ARKWEB_CSS_FONT)
constexpr float epsilon = 0.001f;
#endif

namespace blink {

class FontBuilderUtilsTest : public PageTestBase {
 protected:

  void SetUp() override {
    PageTestBase::SetUp(gfx::Size());
    document_ = MakeGarbageCollected<Document>(
        DocumentInit::Create().ForTest(execution_context.GetExecutionContext()));
  }
  ScopedNullExecutionContext execution_context;
  Member<Document> document_;
};

#if BUILDFLAG(ARKWEB_CSS_FONT)
TEST_F(FontBuilderUtilsTest, UpdateWeightScale_WithoutSettings) {
  FontBuilder font_builder(document_.Get());
  FontBuilderUtils font_builder_utils_(&font_builder);

  FontDescription font_description;
  font_description.SetWeight(FontSelectionValue(400));
  font_builder_utils_.UpdateWeightScale(font_description);
  
}

#endif  // BUILDFLAG(ARKWEB_CSS_FONT)

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
TEST_F(FontBuilderUtilsTest, UpdateFixedFontSize_WithoutSettings) {
  FontBuilder font_builder(document_.Get());
  FontBuilderUtils font_builder_utils_(&font_builder);

  FontDescription font_description;
  font_builder_utils_.UpdateFixedFontSize(font_description);
  
}

#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

}  // namespace blink
