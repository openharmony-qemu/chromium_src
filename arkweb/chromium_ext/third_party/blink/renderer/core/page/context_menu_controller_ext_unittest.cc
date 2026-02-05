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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/context_menu_controller_ext.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <optional>
#include <utility>

#include "third_party/blink/renderer/core/style/computed_style_base.h"
#include "third_party/blink/renderer/core/style/computed_style_constants.h"
#include "third_party/blink/renderer/core/style/computed_style_initial_values.h"

#define private public
#include "third_party/blink/renderer/core/layout/hit_test_result.h"
#include "third_party/blink/renderer/core/layout/layout_text.h"
#include "third_party/blink/renderer/core/style/computed_style.h"
#undef private

namespace blink {

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
TEST_F(ContextMenuControllerTest, ShouldShowFreeCopyMenuTest_1stIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #occluder {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 2;
        }
      </style>
      <img id=target src='http://test.png'>
    </body>
  )HTML");

  base::HistogramTester histograms;
  PhysicalOffset location(LayoutUnit(5), LayoutUnit(5));
  EXPECT_TRUE(ShowContextMenu(location, kMenuSourceLongPress));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(node);
  const HitTestResult hit_test_result = hit_test_result_;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  bool result = false;
  EXPECT_TRUE(ext != nullptr);
  result = ext->ShouldShowFreeCopyMenu(hit_test_result);

  EXPECT_TRUE(result);
}

TEST_F(ContextMenuControllerTest, ShouldShowFreeCopyMenuTest_2ndIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #occluder {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 2;
        }
      </style>
      <img id='target' src='http://test.png'>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  PhysicalOffset location(LayoutUnit(5), LayoutUnit(5));
  EXPECT_TRUE(ShowContextMenu(location, kMenuSourceLongPress));

  HitTestResult hit_test_result_;

  Node* node =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(nullptr);
  const HitTestResult hit_test_result = hit_test_result_;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  bool result = false;
  EXPECT_TRUE(ext != nullptr);
  result = ext->ShouldShowFreeCopyMenu(hit_test_result);

  EXPECT_FALSE(result);
}

TEST_F(ContextMenuControllerTest, ShouldShowFreeCopyMenuTest_3rdIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #occluder {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 2;
        }
      </style>
      <img id='target' src='http://test.png'>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  PhysicalOffset location(LayoutUnit(5), LayoutUnit(5));
  EXPECT_TRUE(ShowContextMenu(location, kMenuSourceLongPress));

  HitTestResult hit_test_result_;

  Node* node =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(nullptr);
  const HitTestResult hit_test_result = hit_test_result_;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  bool result = false;
  EXPECT_TRUE(ext != nullptr);
  result = ext->ShouldShowFreeCopyMenu(hit_test_result);

  EXPECT_FALSE(result);
}

TEST_F(ContextMenuControllerTest, ShouldShowFreeCopyMenuTest_4thIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
        #occluder {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 2;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);
  Element* input_element =
      document->getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(input_element != nullptr);

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(node != nullptr);

  auto* layout_text_ = MakeGarbageCollected<LayoutText>(node, String("sample"));
  ASSERT_TRUE(layout_text_ != nullptr);

  auto* style = input_element->GetComputedStyle();
  layout_text_->SetStyle(style);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(layout_text_);

  const HitTestResult hit_test_result = hit_test_result_;
  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  bool result = false;
  EXPECT_TRUE(ext != nullptr);
  result = ext->ShouldShowFreeCopyMenu(hit_test_result);

  EXPECT_TRUE(result);
}

TEST_F(ContextMenuControllerTest, ShouldShowFreeCopyMenuTest_5thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  PhysicalOffset location_with_image(LayoutUnit(5), LayoutUnit(5));
  PhysicalOffset location_with_link(LayoutUnit(105), LayoutUnit(105));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument()->getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_text_ =
      MakeGarbageCollected<LayoutText>(node, String("Content"));
  ASSERT_TRUE(layout_text_ != nullptr);

  Element* link = GetDocument()->getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  auto* style = link->GetComputedStyle();
  ASSERT_TRUE(style != nullptr);
  layout_text_->SetStyle(style);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(layout_text_);
  hit_test_result_.SetURLElement(link);
  const HitTestResult hit_test_result = hit_test_result_;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  bool result = false;
  EXPECT_TRUE(ext != nullptr);
  result = ext->ShouldShowFreeCopyMenu(hit_test_result);

  EXPECT_FALSE(result);
  EXPECT_TRUE(ShowContextMenu(location_with_image, kMenuSourceLongPress));
}

TEST_F(ContextMenuControllerTest, ShouldShowFreeCopyMenuTest_6thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  PhysicalOffset location_with_image(LayoutUnit(5), LayoutUnit(5));
  PhysicalOffset location_with_link(LayoutUnit(105), LayoutUnit(105));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument()->getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_text_ =
      MakeGarbageCollected<LayoutText>(node, String("Content"));
  ASSERT_TRUE(layout_text_ != nullptr);

  Element* link = GetDocument()->getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  auto* style = link->GetComputedStyle();
  ASSERT_TRUE(style != nullptr);
  layout_text_->SetStyle(style);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(layout_text_);
  hit_test_result_.SetURLElement(link);
  const HitTestResult hit_test_result = hit_test_result_;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  bool result = false;
  EXPECT_TRUE(ext != nullptr);
  result = ext->ShouldShowFreeCopyMenu(hit_test_result);

  EXPECT_FALSE(result);
  EXPECT_TRUE(ShowContextMenu(location_with_image, kMenuSourceLongPress));
}
#endif

TEST_F(ContextMenuControllerTest, GetChildImageUrlFromElementTest_1stIf) {
  const WebElement null_element;
  gfx::Point test_point(10, 10);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetChildImageUrlFromElement(null_element, test_point);
  EXPECT_TRUE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, GetChildImageUrlFromElementTest_2ndIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <div id ="container">No image here </div>
  )HTML");

  WebElement container =
      GetDocument()->getElementById(AtomicString("container"));
  gfx::Point test_point(5, 5);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetChildImageUrlFromElement(container, test_point);
  EXPECT_TRUE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, GetChildImageUrlFromElementTest_3rdIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <div id = "container">
      <img id=target src='http://test.png'>
      </div>
    </body>
  )HTML");

  WebElement container =
      GetDocument()->getElementById(AtomicString("container"));
  WebElement img = GetDocument()->getElementById(AtomicString("target"));
  gfx::Rect bounds = img.BoundsInWidget();
  gfx::Point test_point(bounds.x() + bounds.width() / 2,
                        bounds.y() + bounds.height() / 2);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetChildImageUrlFromElement(container, test_point);
  EXPECT_FALSE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, GetAbsoluteSrcUrlTest_1stIf) {
  const WebElement null_element;
  gfx::Point test_point(10, 10);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetAbsoluteSrcUrl(null_element);
  EXPECT_TRUE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, GetAbsoluteSrcUrlTest_2ndIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <div id ="container">No image here </div>
  )HTML");

  WebElement container =
      GetDocument()->getElementById(AtomicString("container"));
  const WebElement element = container;
  gfx::Point test_point(10, 10);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetAbsoluteSrcUrl(element);
  EXPECT_TRUE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, GetImgChildTest_1stIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <div id = "container">No image here </div>
  )HTML");

  const WebNode null_node =
      GetDocument()->getElementById(AtomicString("container"));
  gfx::Point test_point(10, 10);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  WebElement result = ext->GetImgChild(null_node, test_point);
  EXPECT_TRUE(result.IsNull());
}

TEST_F(ContextMenuControllerTest, GetImgChildTest_2ndIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <div id = "container">No image here </div>
  )HTML");

  const WebNode container =
      GetDocument()->getElementById(AtomicString("container"));
  gfx::Point test_point(5, 5);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  WebElement result = ext->GetImgChild(container, test_point);
  EXPECT_TRUE(result.IsNull());
}

TEST_F(ContextMenuControllerTest, GetImgChildTest_3rdIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <div id = "container">No image here </div>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  const WebNode container =
      GetDocument()->getElementById(AtomicString("container"));
  gfx::Point test_point(5, 5);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  WebElement result = ext->GetImgChild(container, test_point);
  EXPECT_TRUE(result.IsNull());
}

TEST_F(ContextMenuControllerTest, GetImgChildTest_4thIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <div id = "container">
       <img id="img1" style="position:absolute; left:10px; top:10px; width:50px;height:50px;">
       <img id="img2" style="position:absolute; left:70px; top:10px; width:50px;height:50px;">
    </div>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  const WebNode container =
      GetDocument()->getElementById(AtomicString("container"));
  gfx::Point test_point(95, 35);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  WebElement result = ext->GetImgChild(container, test_point);
  EXPECT_FALSE(result.IsNull());
}

TEST_F(ContextMenuControllerTest, GetImgChildTest_5thIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <div id = "container">
       <img id="img1" style="position:absolute; left:10px; top:10px; width:50px;height:50px;">
       <img id="img2" style="position:absolute; left:70px; top:10px; width:50px;height:50px;">
       <img id="img3" style="position:absolute; left:130px; top:10px; width:50px;height:50px;">
    </div>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  const WebNode container =
      GetDocument()->getElementById(AtomicString("container"));
  gfx::Point test_point(95, 35);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  WebElement result = ext->GetImgChild(container, test_point);
  EXPECT_FALSE(result.IsNull());
}

TEST_F(ContextMenuControllerTest, GetImgChildTest_6thIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <div id = "container">
       <img id="img1" style="position:absolute; left:10px; top:10px; width:50px;height:50px;">
       <img id="img2" style="position:absolute; left:70px; top:10px; width:50px;height:50px;">
    </div>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  const WebNode container =
      GetDocument()->getElementById(AtomicString("container"));
  gfx::Point test_point(200, 200);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  WebElement result = ext->GetImgChild(container, test_point);
  EXPECT_TRUE(result.IsNull());
}

TEST_F(ContextMenuControllerTest, GetAbsoluteUrlTest_1stIf) {
  const WebNode null_node;
  std::u16string fragment = u"test";

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetAbsoluteUrl(null_node, fragment);
  EXPECT_TRUE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, GetAbsoluteUrlTest_2ndIf) {
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <div id="container>
      <a id=linktarget href='about:blank'>Content</a>
      </div>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  const WebNode node_ =
      GetDocument()->getElementById(AtomicString("linktarget"));
  std::u16string fragment;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetAbsoluteUrl(node_, fragment);
  EXPECT_TRUE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, GetAbsoluteUrlTest_3rdIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <div id="container>
      <a id=linktarget href='about:blank'>Content</a>
      <img id=target src='http://test.png'>
      </div>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  const WebNode node_ = GetDocument()->getElementById(AtomicString("target"));
  std::u16string fragment = u"Content";

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetAbsoluteUrl(node_, fragment);
  EXPECT_TRUE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, GetAbsoluteUrlTest_4thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <div id="container>
      <a id=linktarget href='about:blank'>Content</a>
      <img id=target src='http://test.png'>
      </div>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  const WebNode node_ = GetDocument()->getElementById(AtomicString("target"));
  std::u16string fragment = u"Content";

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  GURL result = ext->GetAbsoluteUrl(node_, fragment);
  EXPECT_TRUE(result.is_empty());
}

TEST_F(ContextMenuControllerTest, FindImgUrlTest_1stIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  PhysicalOffset point(50, 50);

  ContextMenuData data_;

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("target"));
  ASSERT_TRUE(node != nullptr);

  Element* img = GetDocument()->getElementById(AtomicString("target"));
  ASSERT_TRUE(img != nullptr);

  auto* img_view_ = img->GetLayoutObject();
  ASSERT_TRUE(img_view_ != nullptr);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(img_view_);
  hit_test_result_.SetURLElement(img);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->FindImgUrl(data_, hit_test_result_, point);
  EXPECT_FALSE(data_.has_image_contents);
}

TEST_F(ContextMenuControllerTest, FindImgUrlTest_2ndIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  PhysicalOffset point(150, 150);

  ContextMenuData data_;

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument()->getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_view_ = MakeGarbageCollected<LayoutView>(document);
  ASSERT_TRUE(layout_view_ != nullptr);

  Element* link = GetDocument()->getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);
  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(layout_view_);
  hit_test_result_.SetURLElement(link);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->FindImgUrl(data_, hit_test_result_, point);
  EXPECT_FALSE(data_.has_image_contents);
}

TEST_F(ContextMenuControllerTest, FindImgUrlTest_3rdIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  PhysicalOffset point(200, 200);

  ContextMenuData data_;

  HitTestResult hit_test_result_;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->FindImgUrl(data_, hit_test_result_, point);
  EXPECT_FALSE(data_.has_image_contents);
}

TEST_F(ContextMenuControllerTest, FindImgUrlTest_4thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  PhysicalOffset point(10, 10);

  ContextMenuData data_;
  data_.src_url = GURL("http://test.png");

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument()->getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_view_ = MakeGarbageCollected<LayoutView>(document);
  ASSERT_TRUE(layout_view_ != nullptr);

  Element* img = GetDocument()->getElementById(AtomicString("target"));
  ASSERT_TRUE(img != nullptr);
  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(layout_view_);
  hit_test_result_.SetURLElement(img);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->FindImgUrl(data_, hit_test_result_, point);
  EXPECT_FALSE(data_.has_image_contents);
}

TEST_F(ContextMenuControllerTest, IsHitTestStopNodeTest_1stIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #videoPlayer {
          top: 200px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
        }
        #audioPlayer {
          top: 200px;
          left: 310px;
          position: absolute;
          width: 300px;
          z-index: 1;
        }
        #drawingCanvas {
          top: 420px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #000;
        }
        #flashObject {
          top: 420px;
          left: 310px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px dashed #666;
        }
        #pdfEmbed {
          top: 0;
          left: 620px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #999;
        }
      </style>
      <canvas id="drawingCanvas">
        Browser does not support the canvas tag.
      </canvas>
      <object id="flashObject">
        Browser does not support the object tag.
      </object>
      <embed id="pdfEmbed">
        <source  src="https://example.com/sample.pdf" type="application/pdf">
      </embed>
      <video id="videoPlayer" controls>
        <source src="https://example.com/sample.mp4" type="video/mp4">
        Browser does not support the video tag.
      </video>
      <audio id="audioPlayer" controls>
        <source src="https://example.com/sample.mp3" type="audio/mpeg">
        Browser does not support the audio tag.
      </audio>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument()->getElementById(AtomicString("audioPlayer"))->firstChild();
  ASSERT_TRUE(node != nullptr);
  const Node& node_ = *node;

  bool result = IsHitTestStopNode(node_);
  EXPECT_FALSE(result);
}

TEST_F(ContextMenuControllerTest, IsHitTestStopNodeTest_2ndIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #videoPlayer {
          top: 200px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
        }
        #audioPlayer {
          top: 200px;
          left: 310px;
          position: absolute;
          width: 300px;
          z-index: 1;
        }
        #drawingCanvas {
          top: 420px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #000;
        }
        #flashObject {
          top: 420px;
          left: 310px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px dashed #666;
        }
        #pdfEmbed {
          top: 0;
          left: 620px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #999;
        }
      </style>
      <canvas id="drawingCanvas">
        Browser does not support the canvas tag.
      </canvas>
      <object id="flashObject">
        Browser does not support the object tag.
      </object>
      <embed id="pdfEmbed">
        <source  src="https://example.com/sample.pdf" type="application/pdf">
      </embed>
      <video id="videoPlayer" controls>
        <source src="https://example.com/sample.mp4" type="video/mp4">
        Browser does not support the video tag.
      </video>
      <audio id="audioPlayer" controls>
        <source src="https://example.com/sample.mp3" type="audio/mpeg">
        Browser does not support the audio tag.
      </audio>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("audioPlayer"));
  ASSERT_TRUE(node != nullptr);
  const Node& node_ = *node;

  bool result = IsHitTestStopNode(node_);
  EXPECT_TRUE(result);
}

TEST_F(ContextMenuControllerTest, IsHitTestStopNodeTest_3rdIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #videoPlayer {
          top: 200px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
        }
        #audioPlayer {
          top: 200px;
          left: 310px;
          position: absolute;
          width: 300px;
          z-index: 1;
        }
        #drawingCanvas {
          top: 420px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #000;
        }
        #flashObject {
          top: 420px;
          left: 310px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px dashed #666;
        }
        #pdfEmbed {
          top: 0;
          left: 620px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #999;
        }
      </style>
      <canvas id="drawingCanvas">
        Browser does not support the canvas tag.
      </canvas>
      <object id="flashObject">
        Browser does not support the object tag.
      </object>
      <embed id="pdfEmbed">
        <source  src="https://example.com/sample.pdf" type="application/pdf">
      </embed>
      <video id="videoPlayer" controls>
        <source src="https://example.com/sample.mp4" type="video/mp4">
        Browser does not support the video tag.
      </video>
      <audio id="audioPlayer" controls>
        <source src="https://example.com/sample.mp3" type="audio/mpeg">
        Browser does not support the audio tag.
      </audio>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("videoPlayer"));
  ASSERT_TRUE(node != nullptr);
  const Node& node_ = *node;

  bool result = IsHitTestStopNode(node_);
  EXPECT_TRUE(result);
}

TEST_F(ContextMenuControllerTest, IsHitTestStopNodeTest_4thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #videoPlayer {
          top: 200px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
        }
        #audioPlayer {
          top: 200px;
          left: 310px;
          position: absolute;
          width: 300px;
          z-index: 1;
        }
        #drawingCanvas {
          top: 420px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #000;
        }
        #flashObject {
          top: 420px;
          left: 310px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px dashed #666;
        }
        #pdfEmbed {
          top: 0;
          left: 620px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #999;
        }
      </style>
      <canvas id="drawingCanvas">
        Browser does not support the canvas tag.
      </canvas>
      <object id="flashObject">
        Browser does not support the object tag.
      </object>
      <embed id="pdfEmbed">
        <source  src="https://example.com/sample.pdf" type="application/pdf">
      </embed>
      <video id="videoPlayer" controls>
        <source src="https://example.com/sample.mp4" type="video/mp4">
        Browser does not support the video tag.
      </video>
      <audio id="audioPlayer" controls>
        <source src="https://example.com/sample.mp3" type="audio/mpeg">
        Browser does not support the audio tag.
      </audio>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("pdfEmbed"));
  ASSERT_TRUE(node != nullptr);
  const Node& node_ = *node;

  bool result = IsHitTestStopNode(node_);
  EXPECT_TRUE(result);
}

TEST_F(ContextMenuControllerTest, IsHitTestStopNodeTest_5thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #videoPlayer {
          top: 200px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
        }
        #audioPlayer {
          top: 200px;
          left: 310px;
          position: absolute;
          width: 300px;
          z-index: 1;
        }
        #drawingCanvas {
          top: 420px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #000;
        }
        #flashObject {
          top: 420px;
          left: 310px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px dashed #666;
        }
        #pdfEmbed {
          top: 0;
          left: 620px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #999;
        }
      </style>
      <canvas id="drawingCanvas">
        Browser does not support the canvas tag.
      </canvas>
      <object id="flashObject">
        Browser does not support the object tag.
      </object>
      <embed id="pdfEmbed">
        <source  src="https://example.com/sample.pdf" type="application/pdf">
      </embed>
      <video id="videoPlayer" controls>
        <source src="https://example.com/sample.mp4" type="video/mp4">
        Browser does not support the video tag.
      </video>
      <audio id="audioPlayer" controls>
        <source src="https://example.com/sample.mp3" type="audio/mpeg">
        Browser does not support the audio tag.
      </audio>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("flashObject"));
  ASSERT_TRUE(node != nullptr);
  const Node& node_ = *node;

  bool result = IsHitTestStopNode(node_);
  EXPECT_TRUE(result);
}

TEST_F(ContextMenuControllerTest, IsHitTestStopNodeTest_6thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #videoPlayer {
          top: 200px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
        }
        #audioPlayer {
          top: 200px;
          left: 310px;
          position: absolute;
          width: 300px;
          z-index: 1;
        }
        #drawingCanvas {
          top: 420px;
          left: 0;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #000;
        }
        #flashObject {
          top: 420px;
          left: 310px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px dashed #666;
        }
        #pdfEmbed {
          top: 0;
          left: 620px;
          position: absolute;
          width: 300px;
          height: 200px;
          z-index: 1;
          border: 1px solid #999;
        }
      </style>
      <canvas id="drawingCanvas">
        Browser does not support the canvas tag.
      </canvas>
      <object id="flashObject">
        Browser does not support the object tag.
      </object>
      <embed id="pdfEmbed">
        <source  src="https://example.com/sample.pdf" type="application/pdf">
      </embed>
      <video id="videoPlayer" controls>
        <source src="https://example.com/sample.mp4" type="video/mp4">
        Browser does not support the video tag.
      </video>
      <audio id="audioPlayer" controls>
        <source src="https://example.com/sample.mp3" type="audio/mpeg">
        Browser does not support the audio tag.
      </audio>
      <img id=target src='http://test.png'>
      <a id=linktarget href='about:blank'>Content</a>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;
  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("drawingCanvas"));
  ASSERT_TRUE(node != nullptr);
  const Node& node_ = *node;

  bool result = IsHitTestStopNode(node_);
  EXPECT_TRUE(result);
}

TEST_F(ContextMenuControllerTest, SetArkWebMenuDataTest_1stIf) {
  ContextMenuData data_;
  HitTestResult hit_test_result_;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->SetArkWebMenuData(data_, hit_test_result_);
  EXPECT_TRUE(data_.media_type ==
              mojom::blink::ContextMenuDataMediaType::kImage);
}

TEST_F(ContextMenuControllerTest, SetArkWebMenuDataTest_2ndIf) {
  RegisterMockedImageURLLoad("http://test.png");
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <img id=target src='http://test.png'>
    </body>
  )HTML");

  ContextMenuData data_;
  HitTestResult hit_test_result_;

  Node* node = GetDocument()->getElementById(AtomicString("target"));
  ASSERT_TRUE(node != nullptr);
  hit_test_result_.SetInnerNode(node);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);
  ext->SetArkWebMenuData(data_, hit_test_result_);
  EXPECT_TRUE(data_.media_type ==
              mojom::blink::ContextMenuDataMediaType::kImage);
}

TEST_F(ContextMenuControllerTest, IsAILinkTest_1stIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <a id=linktarget href='https://example.com'>Link</a>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  ContextMenuData data_;
  data_.link_url = GURL();

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(node);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->IsAILink(data_, hit_test_result_);
  EXPECT_TRUE(data_.link_url.is_empty());
}

TEST_F(ContextMenuControllerTest, IsAILinkTest_2ndIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;
  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <a id=linktarget href='https://example.com'>Link</a>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  ContextMenuData data_;
  data_.link_url = GURL("https://example.com");

  HitTestResult hit_test_result_;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->IsAILink(data_, hit_test_result_);
  EXPECT_FALSE(data_.link_url.is_empty());
}

TEST_F(ContextMenuControllerTest, IsAILinkTest_3rdIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <a id=linktarget href='https://example.com'>Link</a>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  ContextMenuData data_;
  data_.link_url = GURL("https://example.com");

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(node);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->IsAILink(data_, hit_test_result_);
  EXPECT_FALSE(data_.link_url.is_empty());
}

TEST_F(ContextMenuControllerTest, IsAILinkTest_4thIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #linktarget {
          top: 100px;
          left: 100px;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
      <a id=linktarget href='https://example.com' ohosarkwebtype='ai'>Link</a>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  base::HistogramTester histograms;

  ContextMenuData data_;
  data_.link_url = GURL("https://example.com");

  HitTestResult hit_test_result_;
  Node* node = GetDocument()->getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(node);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->IsAILink(data_, hit_test_result_);
  EXPECT_FALSE(data_.link_url.is_empty());
}

TEST_F(ContextMenuControllerTest, GetImgUrlTest_1stIf) {
  ContextMenuData data_;
  HitTestResult hit_test_result_;
  WebMenuSourceType source_type_ = kMenuSourceLongPress;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->GetImgUrl(hit_test_result_, data_, source_type_);
  EXPECT_TRUE(data_.src_url.is_empty());
}

TEST_F(ContextMenuControllerTest, GetImgUrlTest_2ndIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <div id=container>No image container</div>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Node* node = GetDocument()->getElementById(AtomicString("container"));
  ASSERT_TRUE(node != nullptr);

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  WebMenuSourceType source_type_ = kMenuSourceLongPress;

  hit_test_result_.SetInnerNode(node);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->GetImgUrl(hit_test_result_, data_, source_type_);
  EXPECT_TRUE(data_.src_url.is_empty());
}

TEST_F(ContextMenuControllerTest, GetImgUrlTest_4thIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;
  frame_test_helpers::LoadFrame(LocalMainFrame(), R"HTML(data:text/html,
  <html>
    <body>
      <embed id="embed" type="application/x-webkit-test-webplugin"
       src="chrome-extension://test" original-url="http://www.test.pdf">
      </embed>
    </body>
  <html>
  )HTML");

  Document* document = GetDocument();
  ASSERT_TRUE(document != nullptr);
  ASSERT_TRUE(IsA<HTMLDocument>(document));

  Element* embed_element = document->getElementById(AtomicString("embed"));
  ASSERT_TRUE(IsA<HTMLEmbedElement>(embed_element));

  auto* embedded =
      DynamicTo<LayoutEmbeddedContent>(embed_element->GetLayoutObject());
  WebPluginContainerImpl* embedded_plugin_view = embedded->Plugin();
  ASSERT_TRUE(!!embedded_plugin_view);

  auto* test_plugin = DynamicTo<ContextMenuControllerTestPlugin>(
      embedded_plugin_view->Plugin());

  test_plugin->SetAttributesForTesting({true, ""});

  ASSERT_TRUE(ShowContextMenuForElement(embed_element, kMenuSourceMouse));
  ContextMenuData context_menu_data = GetWebFrameClient().GetContextMenuData();
  EXPECT_EQ(context_menu_data.media_type,
            mojom::blink::ContextMenuDataMediaType::kPlugin);
  EXPECT_FALSE(
      !!(context_menu_data.edit_flags & ContextMenuDataEditFlags::kCanCopy));
  EXPECT_EQ(context_menu_data.selected_text, "");
}

TEST_F(ContextMenuControllerTest, GetImgUrlTest_5thIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
     <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
      </style>
     <div id="target">
      <img id="img" src="http://test.png" alt="Test Image">
     </div>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Node* node = GetDocument()->getElementById(AtomicString("target"));
  ASSERT_TRUE(node != nullptr);

  Node* img_node = GetDocument()->getElementById(AtomicString("img"));
  ASSERT_TRUE(node != nullptr);

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  PhysicalOffset offset_(LayoutUnit(50), LayoutUnit(50));
  HitTestLocation location(offset_);
  PhysicalRect rect(PhysicalOffset(0, 0), PhysicalSize(100, 100));
  WebMenuSourceType source_type_ = kMenuSourceLongPress;
  EXPECT_TRUE(ShowContextMenu(offset_, kMenuSourceKeyboard));

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.AddNodeToListBasedTestResult(img_node, location, rect);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->GetImgUrl(hit_test_result_, data_, source_type_);
  EXPECT_TRUE(data_.src_url.is_empty());
}

TEST_F(ContextMenuControllerTest, GetImgUrlTest_6thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #hiddenancestor {
          top: 0;
          left: 0;
          position: absolute;
          width: 1px;
          height: 1px;
          z-index: 2;
        }
        #occluder {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 3;
        }
      </style>
      <p id='first'>This is a sample text."</p>
      <img id=target src='http://test.png'>
      <div id=hiddenancestor>
        <div id=occluder></div>
      </div>
    </body>
  )HTML");

  Persistent<MockEventListener> event_listener =
      MakeGarbageCollected<MockEventListener>();
  base::HistogramTester histograms;

  Element* hidden_ancestor =
      GetDocument()->getElementById(AtomicString("hiddenancestor"));
  hidden_ancestor->addEventListener(event_type_names::kContextmenu,
                                    event_listener);

  PhysicalOffset location(LayoutUnit(5), LayoutUnit(5));
  EXPECT_TRUE(ShowContextMenu(location, kMenuSourceLongPress));

  Node* node = GetDocument()->getElementById(AtomicString("hiddenancestor"));
  ASSERT_TRUE(node != nullptr);

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  HitTestLocation location_(location);
  PhysicalRect rect(PhysicalOffset(0, 0), PhysicalSize(100, 100));
  WebMenuSourceType source_type_ = kMenuSourceLongPress;

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.AddNodeToListBasedTestResult(node, location_, rect);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->GetImgUrl(hit_test_result_, data_, source_type_);
  EXPECT_TRUE(data_.media_type != mojom::ContextMenuDataMediaType::kImage);
}

TEST_F(ContextMenuControllerTest, GetImgUrlTest_7thIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
    </body>
  )HTML");

  Node* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();
  const auto& selected_start = Position(first_paragraph, 5);
  const auto& selected_end = Position(first_paragraph, 9);

  GetDocument()->GetFrame()->Selection().SetSelection(
      SelectionInDOMTree::Builder()
          .SetBaseAndExtent(selected_start, selected_end)
          .Build(),
      SetSelectionOptions());
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(GetDocument()->GetFrame()->Selection().SelectedText(), "is a");

  PhysicalOffset location(LayoutUnit(5), LayoutUnit(5));
  EXPECT_TRUE(ShowContextMenu(location, kMenuSourceKeyboard));
}

TEST_F(ContextMenuControllerTest, GetImgUrlTest_8thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first' style="background-image:url('http://test.png');">This is a sample text.</p>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);
  GetDocument()->View()->UpdateAllLifecyclePhases(DocumentUpdateReason::kTest);

  Element* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"));
  ASSERT_TRUE(first_paragraph != nullptr);

  const ComputedStyle* computed_style = first_paragraph->GetComputedStyle();
  if(!computed_style || !computed_style->HasBackgroundImage()) {
    return;
  }

  PhysicalOffset location(LayoutUnit(5), LayoutUnit(5));
  EXPECT_TRUE(ShowContextMenu(location, kMenuSourceLongPress));

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  HitTestLocation location_(location);
  PhysicalRect rect(PhysicalOffset(0, 0), PhysicalSize(100, 100));
  WebMenuSourceType source_type_ = kMenuSourceLongPress;

  hit_test_result_.SetInnerNode(first_paragraph);
  hit_test_result_.AddNodeToListBasedTestResult(first_paragraph, location_, rect);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);
  ext->GetImgUrl(hit_test_result_, data_, source_type_);
}

TEST_F(ContextMenuControllerTest, GetImgUrlTest_9thIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first' style="background-image:linear-gradient(to right, red, blue);">This is a sample text.</p>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Element* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"));
  ASSERT_TRUE(first_paragraph != nullptr);
  PhysicalOffset location(LayoutUnit(5), LayoutUnit(5));
  EXPECT_TRUE(ShowContextMenu(location, kMenuSourceLongPress));

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  HitTestLocation location_(location);
  PhysicalRect rect(PhysicalOffset(0, 0), PhysicalSize(100, 100));
  WebMenuSourceType source_type_ = kMenuSourceLongPress;

  hit_test_result_.SetInnerNode(first_paragraph);
  hit_test_result_.AddNodeToListBasedTestResult(first_paragraph, location_, rect);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);
  ext->GetImgUrl(hit_test_result_, data_, source_type_);
}

TEST_F(ContextMenuControllerTest, SetImageRectFromPotentialImageNodeTest_1stIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
    </body>
  )HTML");

  Node* first_paragraph = nullptr;

  ContextMenuData data_;
  data_.has_image_contents = false;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->SetImageRectFromPotentialImageNode(data_, first_paragraph);

  EXPECT_TRUE(data_.image_rect.IsEmpty());
}

TEST_F(ContextMenuControllerTest, SetImageRectFromPotentialImageNodeTest_2ndIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
    </body>
  )HTML");

  Node* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();
  ASSERT_TRUE(first_paragraph != nullptr);

  ContextMenuData data_;
  data_.has_image_contents = true;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->SetImageRectFromPotentialImageNode(data_, first_paragraph);

  EXPECT_TRUE(data_.image_rect.IsEmpty());
}

TEST_F(ContextMenuControllerTest, SetImageRectFromPotentialImageNodeTest_3rdIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
     <div id='dd'>
      <img id='first'src='htttp://test.png' alt='Sample Image'>
     </div>
    </body>
  )HTML");

  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Element* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"));
  ASSERT_TRUE(first_paragraph != nullptr);

  ASSERT_TRUE(first_paragraph->GetLayoutObject() != nullptr);
  ASSERT_TRUE(first_paragraph->GetLayoutBox() != nullptr);

  ContextMenuData data_;
  data_.has_image_contents = true;

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);
  ext->SetImageRectFromPotentialImageNode(data_, first_paragraph);

  EXPECT_TRUE(data_.image_rect.IsEmpty());
}

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
TEST_F(ContextMenuControllerTest, HandleArkWebContextMenuTest_2ndIf) {
  RegisterMockedImageURLLoad("http://test.png");
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
        }
        #hiddenancestor {
          top: 0;
          left: 0;
          position: absolute;
          width: 1px;
          height: 1px;
          z-index: 2;
        }
        #occluder {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 3;
        }
      </style>
      <p id='first'>This is a sample text."</p>
      <img id=target src='http://test.png'>
      <div id=hiddenancestor>
        <div id=occluder></div>
      </div>
    </body>
  )HTML");

  Persistent<MockEventListener> event_listener =
      MakeGarbageCollected<MockEventListener>();
  base::HistogramTester histograms;

  Element* hidden_ancestor =
      GetDocument()->getElementById(AtomicString("hiddenancestor"));
  hidden_ancestor->addEventListener(event_type_names::kContextmenu,
                                    event_listener);

  PhysicalOffset location(LayoutUnit(5), LayoutUnit(5));
  EXPECT_TRUE(ShowContextMenu(location, kMenuSourceLongPress));

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  LocalFrame* frame_ = GetDocument()->GetFrame();
  LocalFrame* selected_frame_ = GetDocument()->GetFrame();
  LocalFrameView* view = frame_->View();
  frame_->SetView(nullptr);

  Node* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();
  const auto& selected_start = Position(first_paragraph, 5);
  const auto& selected_end = Position(first_paragraph, 9);

  selected_frame_->Selection().SetSelection(
      SelectionInDOMTree::Builder()
          .SetBaseAndExtent(selected_start, selected_end)
          .Build(),
      SetSelectionOptions());
  base::RunLoop().RunUntilIdle();

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);

  ext->HandleArkWebContextMenu(data_, frame_, hit_test_result_,
                               selected_frame_, kMenuSourceShowFreeCopyMenu);
  frame_->SetView(view);

  EXPECT_TRUE(data_.image_rect.IsEmpty());
}

TEST_F(ContextMenuControllerTest, HandleArkWebContextMenuTest_3rdIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
    </body>
  )HTML");

  Node* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();

  const auto& selected_start = Position(first_paragraph,5);
  const auto& selected_end = Position(first_paragraph,9);

  GetDocument()->GetFrame()->Selection().SetSelection(
      SelectionInDOMTree::Builder()
          .SetBaseAndExtent(selected_start, selected_end)
          .Build(),
      SetSelectionOptions());
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(GetDocument()->GetFrame()->Selection().SelectedText(), "is a");

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  LocalFrame* frame_ = GetDocument()->GetFrame();
  LocalFrame* selected_frame_ = GetDocument()->GetFrame();

  data_.selected_text.clear();
  data_.is_selectable = true;

  Page* page = GetDocument()->GetPage();
  page->GetSettings().SetContextMenuCustomization(true);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);
  ext->HandleArkWebContextMenu(data_, frame_, hit_test_result_,
                               selected_frame_, kMenuSourceShowFreeCopyMenu);

  EXPECT_TRUE(!data_.is_selectable);
}

TEST_F(ContextMenuControllerTest, HandleArkWebContextMenuTest_4thIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
    </body>
  )HTML");

  Node* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  LocalFrame* frame_ = GetDocument()->GetFrame();
  LocalFrame* selected_frame_ = GetDocument()->GetFrame();

  data_.selected_text.clear();

  Page* page = GetDocument()->GetPage();
  page->GetSettings().SetContextMenuCustomization(true);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);
  ext->HandleArkWebContextMenu(data_, frame_, hit_test_result_,
                               selected_frame_, kMenuSourceTypeLast);

  EXPECT_TRUE(!data_.is_selectable);
}

TEST_F(ContextMenuControllerTest, HandleArkWebContextMenuTest_5thIf) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id='first'>This is a sample text."</p>
    </body>
  )HTML");

  Node* first_paragraph =
      GetDocument()->getElementById(AtomicString("first"))->firstChild();

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  LocalFrame* selected_frame_ = GetDocument()->GetFrame();

  data_.selected_text.clear();
  data_.link_url = GURL("https://example.com");

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);
  ext->HandleArkWebContextMenu(data_, nullptr, hit_test_result_,
                               selected_frame_, kMenuSourceTypeLast);

  EXPECT_TRUE(!data_.is_selectable);
}
#endif

TEST_F(ContextMenuControllerTest, GetImgUrlTest_001) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
     <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
          background-image: url("http://test.png");
        }
      </style>
     <div id="target">
      <img id="img" src="http://test.png" alt="Test Image">
     </div>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Node* node = GetDocument()->getElementById(AtomicString("target"));
  ASSERT_TRUE(node != nullptr);

  Node* img_node = GetDocument()->getElementById(AtomicString("img"));
  ASSERT_TRUE(node != nullptr);

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  PhysicalOffset offset_(LayoutUnit(50), LayoutUnit(50));
  HitTestLocation location(offset_);
  PhysicalRect rect(PhysicalOffset(0, 0), PhysicalSize(100, 100));
  WebMenuSourceType source_type_ = kMenuSourceLongPress;
  EXPECT_TRUE(ShowContextMenu(offset_, kMenuSourceKeyboard));

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.AddNodeToListBasedTestResult(img_node, location, rect);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  EXPECT_TRUE(ext != nullptr);
  ext->GetImgUrl(hit_test_result_, data_, source_type_);
  EXPECT_TRUE(data_.src_url.is_empty());
}

// Test for style_image null pointer check when using CSS gradient
// (HasBackgroundImage returns true but GetImage returns null)
TEST_F(ContextMenuControllerTest, GetImgUrlTest_StyleImageNull) {
  ContextMenuAllowedScope context_menu_allowed_scope;

  GetDocument()->documentElement()->setInnerHTML(R"HTML(
    <body>
     <style>
        #target {
          top: 0;
          left: 0;
          position: absolute;
          width: 100px;
          height: 100px;
          z-index: 1;
          background: linear-gradient(to right, red, blue);
        }
      </style>
     <div id="target">Text with gradient background</div>
    </body>
  )HTML");
  GetDocument()->UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Node* node = GetDocument()->getElementById(AtomicString("target"));
  ASSERT_TRUE(node != nullptr);

  ContextMenuData data_;
  HitTestResult hit_test_result_;
  PhysicalOffset offset_(LayoutUnit(50), LayoutUnit(50));
  HitTestLocation location(offset_);
  PhysicalRect rect(PhysicalOffset(0, 0), PhysicalSize(100, 100));
  WebMenuSourceType source_type_ = kMenuSourceLongPress;

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.AddNodeToListBasedTestResult(node, location, rect);

  auto& helper =
      web_view_helper_.GetWebView()->GetPage()->GetContextMenuController();
  auto ext = helper.AsContextMenuControllerExt();
  ASSERT_TRUE(ext != nullptr);
  // Should not crash even when style_image is null (gradient returns null)
  ext->GetImgUrl(hit_test_result_, data_, source_type_);
  EXPECT_TRUE(data_.src_url.is_empty());
}

}  // namespace blink
