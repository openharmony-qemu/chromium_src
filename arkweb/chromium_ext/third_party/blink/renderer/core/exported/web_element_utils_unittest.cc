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

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/document_init.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/testing/null_execution_context.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#define protected public
#include "third_party/blink/renderer/core/exported/web_element_utils.h"
#undef protected
 
namespace blink {

class MockHTMLElement : public HTMLElement {
 public:
  explicit MockHTMLElement(const QualifiedName& tag_name, Document& document) 
      : HTMLElement(tag_name, document) {}
  MOCK_CONST_METHOD0(contentEditableNormalized, ContentEditableType());
};

class WebElementUtilsTest : public PageTestBase {
 public:
  void SetUp() override {
    PageTestBase::SetUp(gfx::Size());
    document_ = &GetDocument();
  }
  Document& document() const { return *document_; }
 private:
  Persistent<Document> document_;
};

TEST_F(WebElementUtilsTest, IsContentEditable) {
  WebElementUtils utils;
  EXPECT_FALSE(utils.IsContentEditable());

  auto *mock_div = MakeGarbageCollected<MockHTMLElement>(
  HTMLQualifiedName(html_names::kDivTag), document());
  utils.private_ = mock_div;
  EXPECT_FALSE(utils.IsContentEditable());
}

}  // namespace blink