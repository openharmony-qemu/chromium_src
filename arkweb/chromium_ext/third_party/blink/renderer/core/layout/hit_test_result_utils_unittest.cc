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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/layout/hit_test_result_utils.h"

namespace blink {

TEST_F(HitTestingTest, GetReplacedContentRect) {
  SetBodyInnerHTML(R"HTML(
    <style>
    div {
      width: 100px;
      height: 100px;
    }
    </style>

    <div id=target></div>
    <div id=occluder></div>
  )HTML");

  Element* target = GetElementById("target");
  HitTestResult result = HitTestForOcclusion(*target);
  gfx::Rect rect = result.ImageRect();
  Node* inner_node = result.InnerNode();
  ASSERT_TRUE(inner_node);

  inner_node->SetLayoutObject(nullptr);
  HitTestResultUtils utils(&result);
  gfx::Rect img_rect = utils.GetReplacedContentRect();
  EXPECT_EQ(rect.x(), img_rect.x());

  result.SetInnerNode(nullptr);
  img_rect = utils.GetReplacedContentRect();
  EXPECT_EQ(rect.x(), img_rect.x());
}

TEST_F(HitTestingTest, GetLinkElementRect) {
  SetBodyInnerHTML(R"HTML(
    <style>
    div {
      width: 100px;
      height: 100px;
    }
    </style>

    <div id=target></div>
    <div id=occluder></div>
  )HTML");

  Element* target = GetElementById("target");
  HitTestResult result = HitTestForOcclusion(*target);
  gfx::Rect rect;
  result.SetInnerNode(nullptr);
  HitTestResultUtils utils(&result);
  gfx::Rect img_rect = utils.GetLinkElementRect();
  EXPECT_EQ(rect.x(), img_rect.x());
}

}