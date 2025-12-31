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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/layout/layout_object_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"

namespace blink {

class LayoutObjectUtilsTest : public RenderingTest {
 protected:
  void SetUp() override {
    RenderingTest::SetUp();
    SetBodyInnerHTML(R"HTML(
      <div id="target"></div>
      <img id="image">
    )HTML");
  }

  Element* GetTargetElement() {
    return GetElementById("target");
  }

  Element* GetImageElement() {
    return GetElementById("image");
  }

  LayoutObject* GetTargetLayoutObject() {
    return GetTargetElement()->GetLayoutObject();
  }

  LayoutObject* GetImageLayoutObject() {
    return GetImageElement()->GetLayoutObject();
  }

  void SetVisibility(Element* element, EVisibility visibility) {
    element->SetInlineStyleProperty(CSSPropertyID::kVisibility, 
                                  visibility == EVisibility::kVisible 
                                  ? "visible" 
                                  : "hidden");
    UpdateAllLifecyclePhasesForTest();
  }
  
  void SetPointerEvents(Element* element, const char* value) {
    element->SetInlineStyleProperty(CSSPropertyID::kPointerEvents, value);
    UpdateAllLifecyclePhasesForTest();
  }
  
  LayoutObjectUtils CreateUtils(LayoutObject* object) {
    return LayoutObjectUtils(object);
  }
  
  HitTestRequest CreateHitTestRequest(HitTestRequest::RequestType type) {
    return HitTestRequest(type);
  }

  HitTestRequest CreateIgnorePointerEventsRequest() {
    return HitTestRequest(static_cast<HitTestRequest::RequestType>(
        HitTestRequest::kActive | HitTestRequest::kIgnorePointerEventsNone));
  }
};

TEST_F(LayoutObjectUtilsTest, ImageVisibleWithHitTest) {
  Element* image = GetImageElement();
  SetVisibility(image, EVisibility::kVisible);

  LayoutObjectUtils utils = CreateUtils(GetImageLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kActive);

  EXPECT_TRUE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, ImageHiddenWithHitTest) {
  Element* image = GetImageElement();
  SetVisibility(image, EVisibility::kHidden);

  LayoutObjectUtils utils = CreateUtils(GetImageLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kActive);

  EXPECT_FALSE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, NonImageVisibleIgnorePointerEvents) {
  Element* target = GetTargetElement();
  SetVisibility(target, EVisibility::kVisible);
  SetPointerEvents(target, "none");

  LayoutObjectUtils utils = CreateUtils(GetTargetLayoutObject());
  HitTestRequest request = CreateIgnorePointerEventsRequest();

  EXPECT_TRUE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, NonImageVisibleWithHitTesting) {
  Element* target = GetTargetElement();
  SetVisibility(target, EVisibility::kVisible);
  SetPointerEvents(target, "auto");

  LayoutObjectUtils utils = CreateUtils(GetTargetLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kReadOnly);

  EXPECT_TRUE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, NonImageHidden) {
  Element* target = GetTargetElement();
  SetVisibility(target, EVisibility::kHidden);

  LayoutObjectUtils utils = CreateUtils(GetTargetLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kActive);

  EXPECT_FALSE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, NonImageVisibleWithoutHitTesting) {
  Element* target = GetTargetElement();
  SetVisibility(target, EVisibility::kVisible);
  SetPointerEvents(target, "none");

  LayoutObjectUtils utils = CreateUtils(GetTargetLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kReadOnly);

  EXPECT_FALSE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, ImageVisibleWithHitTest1) {
  Element* image = GetImageElement();
  SetVisibility(image, EVisibility::kVisible);

  LayoutObjectUtils utils = CreateUtils(GetImageLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kActive);

  EXPECT_TRUE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, ImageHiddenWithHitTest1) {
  Element* image = GetImageElement();
  SetVisibility(image, EVisibility::kHidden);

  LayoutObjectUtils utils = CreateUtils(GetImageLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kActive);

  EXPECT_FALSE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, NonImageVisibleIgnorePointerEvents1) {
  Element* target = GetTargetElement();
  SetVisibility(target, EVisibility::kVisible);
  SetPointerEvents(target, "none");

  LayoutObjectUtils utils = CreateUtils(GetTargetLayoutObject());
  HitTestRequest request = CreateIgnorePointerEventsRequest();

  EXPECT_TRUE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, NonImageVisibleWithHitTesting1) {
  Element* target = GetTargetElement();
  SetVisibility(target, EVisibility::kVisible);
  SetPointerEvents(target, "auto");

  LayoutObjectUtils utils = CreateUtils(GetTargetLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kReadOnly);

  EXPECT_TRUE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, NonImageHidden1) {
  Element* target = GetTargetElement();
  SetVisibility(target, EVisibility::kHidden);

  LayoutObjectUtils utils = CreateUtils(GetTargetLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kActive);

  EXPECT_FALSE(utils.ArkWebVisibleToHitTestRequest(request));
}

TEST_F(LayoutObjectUtilsTest, NonImageVisibleWithoutHitTesting1) {
  Element* target = GetTargetElement();
  SetVisibility(target, EVisibility::kVisible);
  SetPointerEvents(target, "none");

  LayoutObjectUtils utils = CreateUtils(GetTargetLayoutObject());
  HitTestRequest request = CreateHitTestRequest(HitTestRequest::kReadOnly);

  EXPECT_FALSE(utils.ArkWebVisibleToHitTestRequest(request));
}


}  // namespace blink