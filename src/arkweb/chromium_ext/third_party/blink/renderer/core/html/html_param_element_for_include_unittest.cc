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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/html/html_object_element.h"
#include "third_party/blink/renderer/core/html/html_param_element.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"

namespace blink {

class HTMLParamElementForIncludeTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    document_ = &GetDocument();
    element_ = MakeGarbageCollected<HTMLParamElement>(*document_);
    document_->body()->AppendChild(element_.Get());
    UpdateAllLifecyclePhasesForTest();
  }

  Element::AttributeModificationParams CreateParams(const QualifiedName& qname,
                                                    const AtomicString old_value,
                                                    const AtomicString new_value) {
    return Element::AttributeModificationParams(
      qname,
      old_value,
      new_value,
      Element::AttributeModificationReason::kDirectly
    );
  }

  Persistent<Document> document_;
  Persistent<HTMLParamElement> element_;
};

TEST_F(HTMLParamElementForIncludeTest, AttributeChanged001) {
  auto params = CreateParams(html_names::kNameAttr, AtomicString(""), AtomicString(""));
  element_->AttributeChanged(params);
}

TEST_F(HTMLParamElementForIncludeTest, AttributeChanged002) {
  auto params = CreateParams(html_names::kNameAttr, AtomicString("1"), AtomicString("2"));
  element_->AttributeChanged(params);
}

}  // namespace blink