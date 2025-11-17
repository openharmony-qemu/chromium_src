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
#include "third_party/blink/renderer/core/dom/container_node.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/html/html_meta_element.h"
#include "third_party/blink/renderer/core/page/viewport_description.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace blink {

class HTMLMetaElementForIncludeTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    document_ = &GetDocument();
    element_ = MakeGarbageCollected<HTMLMetaElement>(
        *document_, CreateElementFlags::ByParser(document_));
    document_->body()->AppendChild(element_.Get());
    UpdateAllLifecyclePhasesForTest();
  }

  bool isWarning_;
  ViewportDescription description_; 
  Persistent<Document> document_;
  Persistent<HTMLMetaElement> element_;
};

TEST_F(HTMLMetaElementForIncludeTest, ParseFlingDataX) {
  float referencenumX_  = std::numeric_limits<float>::max();
  ASSERT_TRUE(element_);
  EXPECT_EQ(description_.max_fling_velocity_x, referencenumX_);
  element_->ParseFlingData(document_, isWarning_, "max-fling-speed-x", "4000",
                           description_);
  EXPECT_EQ(description_.max_fling_velocity_x, 4000);
}

TEST_F(HTMLMetaElementForIncludeTest, ParseFlingDataY) {
  float referencenumY_  = std::numeric_limits<float>::max();
  ASSERT_TRUE(element_);
  EXPECT_EQ(description_.max_fling_velocity_y, referencenumY_);
  element_->ParseFlingData(document_, isWarning_, "max-fling-speed-y", "4500",
                           description_);
  EXPECT_EQ(description_.max_fling_velocity_x, 4500);
}
}  // namespace blink
