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

#define private public
#include "third_party/blink/renderer/core/editing/selection_controller.h"
#undef private

class SelectionControllerForIncludeTest : public SelectionControllerTest {
 public:
  void SetStartSelect(bool start_select_);
  void SetClickSelection(bool click_in_selection_);
  void SetDownAllow(bool down_allows_);
  void SetMenuShow(bool menu_show_);
  bool GetClickSelection();
  bool GetDownAllow();
  bool GetMenuShow();
  void SetLongPress(HitTestResult& result);
  void SetLinkPress(HitTestResult& result);

  void TestBody() override {}
};

void SelectionControllerForIncludeTest::SetStartSelect(bool start_select_) {
  Controller().mouse_down_may_start_select_ = start_select_;
}

void SelectionControllerForIncludeTest::SetClickSelection(bool click_in_selection_) {
  Controller().mouse_down_was_single_click_in_selection_ = click_in_selection_;
}

void SelectionControllerForIncludeTest::SetDownAllow(bool down_allows_) {
  Controller().mouse_click_down_allows_ = down_allows_;
}

void SelectionControllerForIncludeTest::SetMenuShow(bool menu_show_) {
  Controller().mouse_menu_show_ = menu_show_;
}

bool SelectionControllerForIncludeTest::GetClickSelection() {
  return Controller().mouse_down_was_single_click_in_selection_;
}

bool SelectionControllerForIncludeTest::GetDownAllow() {
  return Controller().mouse_click_down_allows_;
}

bool SelectionControllerForIncludeTest::GetMenuShow() {
  return Controller().mouse_menu_show_;
}

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void SelectionControllerForIncludeTest::SetLongPress(HitTestResult& result) {
  Controller().last_long_press_hit_test_result_ = result;
}
#endif

void SelectionControllerForIncludeTest::SetLinkPress(HitTestResult& result) {
  Controller().last_link_hit_test_result_ = result;
}

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
TEST_F(SelectionControllerForIncludeTest, NotifyContextMenuWillShowTest_1stIf) {
  HitTestResult result;
  GetFrame()
      .GetEventHandler()
      .GetSelectionController()
      .NotifyContextMenuWillShow();
  EXPECT_TRUE(!result.InnerNode());
}

TEST_F(SelectionControllerForIncludeTest, NotifyContextMenuWillShowTest_2ndIf) {
  HitTestResult result;
  GetFrame()
      .GetEventHandler()
      .GetSelectionController()
      .NotifyContextMenuWillShow();
  EXPECT_TRUE(!result.InnerNode());
}

TEST_F(SelectionControllerForIncludeTest, FocusDocumentViewTest_1stIf) {
  HitTestResult result;
  GetFrame().GetEventHandler().GetSelectionController().FocusDocumentView();
  EXPECT_TRUE(!result.InnerNode());
}

TEST_F(SelectionControllerForIncludeTest,
       ShowSelectionByLastLongPressHitTestResultTest_1stIf) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
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

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  PhysicalOffset location_with_link(LayoutUnit(105), LayoutUnit(105));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  Element* link = GetDocument().getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  hit_test_result_.SetInnerNode(node);

  hit_test_result_.SetURLElement(link);
  hit_test_result_.OverrideNodeAndPosition(node, location_with_link);

  SetLongPress(hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLongPressHitTestResult();

  EXPECT_TRUE(result);
}

TEST_F(SelectionControllerForIncludeTest,
       ShowSelectionByLastLongPressHitTestResultTest_2ndIf) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id=target>This is the first line.</p>
    </body>
  )HTML");

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("target"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(node);

  SetLongPress(hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLongPressHitTestResult();

  EXPECT_TRUE(result);
}

TEST_F(SelectionControllerForIncludeTest,
       ShowSelectionByLastLongPressHitTestResultTest_3rdIf) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
    <body>
      <p id=target>This is the first line.</p>
    </body>
  )HTML");

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("target"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(nullptr);

  SetLongPress(hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLongPressHitTestResult();

  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest,
       ShowSelectionByLastLongPressHitTestResultTest_4thIf) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
    <body>
      <style>
        #target {
          user-select:none;
        }
      </style>
      <p id=target>This is the first line.</p>
    </body>
  )HTML");

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("target"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  hit_test_result_.SetInnerNode(node);

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  SetLongPress(hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLongPressHitTestResult();

  EXPECT_TRUE(result);
}

TEST_F(SelectionControllerForIncludeTest, SelectClosestWordFromLiveLinkTest_1stIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  const HitTestResult hit_test_result_ = hit_test_result;

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .SelectClosestWordFromLiveLink(hit_test_result_);
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, SelectClosestWordFromLiveLinkTest_2ndIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  hit_test_result.InnerNode()->SetLayoutObject(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .SelectClosestWordFromLiveLink(hit_test_result_);
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, SelectClosestWordFromLiveLinkTest_3rdIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  HitTestResult hit_test_result;

  hit_test_result.SetInnerNode(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .SelectClosestWordFromLiveLink(hit_test_result_);
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, SelectClosestWordFromLiveLinkTest_4thIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <a href='#'><span id='span'>click me</span></a>
      <p id='outside'>outside link</p>
    </body>
  )HTML");
  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Node* text_node =
      GetDocument().getElementById(AtomicString("outside"))->firstChild();
  ASSERT_TRUE(text_node != nullptr);

  HitTestResult hit_test_result;

  hit_test_result.SetInnerNode(text_node);
  hit_test_result.SetURLElement(To<Element>(
      GetDocument().getElementById(AtomicString("span"))->parentNode()));
  const HitTestResult hit_test_result_ = hit_test_result;

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .SelectClosestWordFromLiveLink(hit_test_result_);
  EXPECT_TRUE(result);
}

TEST_F(SelectionControllerForIncludeTest, SelectClosestWordFromLiveLinkTest_5thIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <a href='#' id='link'><span id='span'>click me</span></a>
    </body>
  )HTML");
  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Element* span = GetDocument().getElementById(AtomicString("span"));
  ASSERT_TRUE(span != nullptr);
  Node* node = span;

  HitTestResult hit_test_result;

  hit_test_result.SetInnerNode(node);
  hit_test_result.SetURLElement(
      GetDocument().getElementById(AtomicString("link")));
  const HitTestResult hit_test_result_ = hit_test_result;

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .SelectClosestWordFromLiveLink(hit_test_result_);
  EXPECT_TRUE(result);
}

TEST_F(SelectionControllerForIncludeTest, SelectClosestWordFromLiveLinkTest_6thIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <a href='#' id='link'>click me</a>
    </body>
  )HTML");
  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  Node* text_node =
      GetDocument().getElementById(AtomicString("link"))->firstChild();
  ASSERT_TRUE(text_node != nullptr);

  HitTestResult hit_test_result;

  hit_test_result.SetInnerNode(text_node);
  hit_test_result.SetURLElement(
      GetDocument().getElementById(AtomicString("link")));
  const HitTestResult hit_test_result_ = hit_test_result;

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .SelectClosestWordFromLiveLink(hit_test_result_);
  EXPECT_TRUE(result);
}
#endif

TEST_F(SelectionControllerForIncludeTest, MouseSelectMenuShowTest_1stIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "  range.deleteContents();"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  UpdateAllLifecyclePhasesForTest();

  bool result =
      GetFrame().GetEventHandler().GetSelectionController().MouseSelectMenuShow(
          true);
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, SetDataDetectorHitTest_1stIf) {
  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "  range.deleteContents();"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  UpdateAllLifecyclePhasesForTest();

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;

  hit_test_result.SetInnerNode(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults evnet_(mouse_event, location,
                                            hit_test_result_);

  GetFrame().GetEventHandler().GetSelectionController().SetDataDetectorHitTest(
      evnet_);
  EXPECT_FALSE(hit_test_result_.InnerNode());
}

TEST_F(SelectionControllerForIncludeTest, SetDataDetectorHitTest_2ndIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "  range.deleteContents();"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  UpdateAllLifecyclePhasesForTest();

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  hit_test_result.InnerNode()->SetLayoutObject(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults evnet_(mouse_event, location,
                                            hit_test_result_);

  GetFrame().GetEventHandler().GetSelectionController().SetDataDetectorHitTest(
      evnet_);
  EXPECT_TRUE(hit_test_result_.InnerNode());
}

TEST_F(SelectionControllerForIncludeTest, SetDataDetectorHitTest_3rdIf) {
  SetHtmlInnerHTML(R"HTML(
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

  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "  range.deleteContents();"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  UpdateAllLifecyclePhasesForTest();

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node =
      GetDocument().getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_text_ =
      MakeGarbageCollected<LayoutText>(node, String("Content"));
  ASSERT_TRUE(layout_text_ != nullptr);

  Element* link = GetDocument().getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  auto* style = link->GetComputedStyle();
  ASSERT_TRUE(style != nullptr);
  layout_text_->SetStyle(style);

  hit_test_result.SetInnerNode(node);
  hit_test_result.InnerNode()->SetLayoutObject(layout_text_);
  hit_test_result.SetURLElement(link);

  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults evnet_(mouse_event, location,
                                            hit_test_result_);

  GetFrame().GetEventHandler().GetSelectionController().SetDataDetectorHitTest(
      evnet_);
  EXPECT_TRUE(hit_test_result_.InnerNode());
}

TEST_F(SelectionControllerForIncludeTest, HandleMouseReleaseEventWithMenuExtTest_1stIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "  range.deleteContents();"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  UpdateAllLifecyclePhasesForTest();

  SelectionController& controller_ = Controller();
  SetStartSelect(true);
  SetClickSelection(true);

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  hit_test_result.InnerNode()->SetLayoutObject(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults evnet_(mouse_event, location,
                                            hit_test_result_);

  blink::SelectionControllerUtils::HandleMouseReleaseEventWithMenuExt(
      &controller_, evnet_);
  EXPECT_TRUE(GetClickSelection());
}

TEST_F(SelectionControllerForIncludeTest, HandleMouseReleaseEventWithMenuExtTest_2ndIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "  range.deleteContents();"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  UpdateAllLifecyclePhasesForTest();

  SelectionController& controller_ = Controller();
  SetStartSelect(true);
  SetClickSelection(false);

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  hit_test_result.InnerNode()->SetLayoutObject(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults evnet_(mouse_event, location,
                                            hit_test_result_);

  blink::SelectionControllerUtils::HandleMouseReleaseEventWithMenuExt(
      &controller_, evnet_);
  EXPECT_TRUE(!GetClickSelection());
}

TEST_F(SelectionControllerForIncludeTest, HandleMouseReleaseEventWithMenuExtTest_3rdIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "  range.deleteContents();"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  UpdateAllLifecyclePhasesForTest();

  SelectionController& controller_ = Controller();
  SetStartSelect(false);
  SetClickSelection(false);

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  hit_test_result.InnerNode()->SetLayoutObject(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults evnet_(mouse_event, location,
                                            hit_test_result_);

  blink::SelectionControllerUtils::HandleMouseReleaseEventWithMenuExt(
      &controller_, evnet_);
  EXPECT_FALSE(GetClickSelection());
}

TEST_F(SelectionControllerForIncludeTest, HandleMouseReleaseEventWithMenuExtTest_4thIf) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <style>
        #unselectable {
          user_select: none;
          user_modify: read-only;
        }
      </style>
      <div id='unselectable'>This is a sample text."</div>
    </body>
  )HTML");

  UpdateAllLifecyclePhasesForTest();

  SelectionController& controller_ = Controller();

  SetClickSelection(true);
  SetDownAllow(true);
  SetMenuShow(true);

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("unselectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults evnet_(mouse_event, location,
                                            hit_test_result_);

  blink::SelectionControllerUtils::HandleMouseReleaseEventWithMenuExt(
      &controller_, evnet_);
  EXPECT_FALSE(GetMenuShow());
}

TEST_F(SelectionControllerForIncludeTest, HandleMouseReleaseEventWithMenuExtTest_5thIf) {
  const char* body_content = "<span id=top>top</span><span id=host></span>";
  const char* shadow_content = "<span id=bottom>bottom</span>";
  SetBodyContent(body_content);
  ShadowRoot* shadow_root = SetShadowContent(shadow_content, "host");
  ASSERT_TRUE(shadow_root != nullptr);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  Node* bottom =
      shadow_root->getElementById(AtomicString("bottom"))->firstChild();
  ASSERT_TRUE(top != nullptr);
  ASSERT_TRUE(bottom != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 1))
                                         .Extend(PositionInFlatTree(bottom, 3))
                                         .Build(),
                                     TextGranularity::kCharacter);

  SelectionController& controller_ = Controller();
  SetStartSelect(true);
  SetClickSelection(false);
  SetDownAllow(false);
  SetMenuShow(false);

  WebMouseEvent mouse_event;
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("top"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  hit_test_result.InnerNode()->SetLayoutObject(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;

  const MouseEventWithHitTestResults evnet_(mouse_event, location,
                                            hit_test_result_);

  blink::SelectionControllerUtils::HandleMouseReleaseEventWithMenuExt(
      &controller_, evnet_);
  EXPECT_FALSE(GetDownAllow());
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExistTest_5thIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a sample test</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 1))
                                         .Extend(PositionInFlatTree(top, 4))
                                         .Build(),
                                     TextGranularity::kCharacter);

  blink::WebMouseEvent double_click(
      blink::WebMouseEvent::Type::kMouseDown, 0,
      blink::WebInputEvent::GetStaticTimeStampForTests());

  double_click.SetFrameScale(1);
  HitTestLocation location((gfx::Point(20, 5)));
  double_click.button = blink::WebMouseEvent::Button::kLeft;
  double_click.click_count = 2;
  double_click.SetModifiers(blink::WebInputEvent::Modifiers::kShiftKey);

  HitTestResult result;
  const MouseEventWithHitTestResults event_(double_click, location, result);
  bool result_ = GetFrame()
                     .GetEventHandler()
                     .GetSelectionController()
                     .HandleGestureTapIfSelectionExist(event_);

  EXPECT_FALSE(result_);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExistTest_6thIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a sample test</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 1))
                                         .Extend(PositionInFlatTree(top, 4))
                                         .Build(),
                                     TextGranularity::kCharacter);

  blink::WebMouseEvent double_click(
      blink::WebMouseEvent::Type::kMouseDown, 0,
      blink::WebInputEvent::GetStaticTimeStampForTests());

  double_click.SetFrameScale(1);
  HitTestLocation location((gfx::Point(20, 5)));
  double_click.SetModifiers(blink::WebInputEvent::Modifiers::kShiftKey);

  HitTestResult result;

  const MouseEventWithHitTestResults event_(double_click, location, result);

  bool result_ = GetFrame()
                     .GetEventHandler()
                     .GetSelectionController()
                     .HandleGestureTapIfSelectionExist(event_);

  EXPECT_FALSE(result_);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExistTest_7thIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a sample test</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 1))
                                         .Extend(PositionInFlatTree(top, 4))
                                         .Build(),
                                     TextGranularity::kCharacter);

  blink::WebMouseEvent double_click(
      blink::WebMouseEvent::Type::kMouseDown, 0,
      blink::WebInputEvent::GetStaticTimeStampForTests());

  double_click.SetFrameScale(1);
  HitTestLocation location((gfx::Point(20, 5)));
  double_click.button = blink::WebMouseEvent::Button::kLeft;

  HitTestResult result;

  const MouseEventWithHitTestResults event_(double_click, location, result);

  bool result_ = GetFrame()
                     .GetEventHandler()
                     .GetSelectionController()
                     .HandleGestureTapIfSelectionExist(event_);

  EXPECT_FALSE(result_);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExistTest_8thIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a sample test</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 0))
                                         .Extend(PositionInFlatTree(top, 20))
                                         .Build(),
                                     TextGranularity::kCharacter);

  blink::WebMouseEvent single_click(
      blink::WebMouseEvent::Type::kMouseDown, 0,
      blink::WebInputEvent::GetStaticTimeStampForTests());

  single_click.SetFrameScale(1);
  HitTestLocation location((gfx::Point(20, 5)));
  single_click.button = blink::WebMouseEvent::Button::kLeft;
  single_click.click_count = 1;
  single_click.SetModifiers(
      blink::WebInputEvent::Modifiers::kIsCompatibilityEventForTouch);
  Selection().AsFrameSelectionExt()->SetIsSelectAll(true);

  HitTestResult result;

  const MouseEventWithHitTestResults event_(single_click, location, result);

  bool result_ = GetFrame()
                     .GetEventHandler()
                     .GetSelectionController()
                     .HandleGestureTapIfSelectionExist(event_);

  EXPECT_FALSE(result_);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExistTest_9thIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a sample test</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 0))
                                         .Extend(PositionInFlatTree(top, 10))
                                         .Build(),
                                     TextGranularity::kCharacter);

  blink::WebMouseEvent single_click(
      blink::WebMouseEvent::Type::kMouseDown, 0,
      blink::WebInputEvent::GetStaticTimeStampForTests());

  single_click.SetFrameScale(1);
  HitTestLocation location((gfx::Point(20, 5)));
  single_click.button = blink::WebMouseEvent::Button::kLeft;
  single_click.click_count = 1;
  single_click.SetModifiers(
      blink::WebInputEvent::Modifiers::kIsCompatibilityEventForTouch);

  HitTestResult result;

  const MouseEventWithHitTestResults event_(single_click, location, result);

  LocalFrameView* view = GetFrame().View();
  GetFrame().SetView(nullptr);

  bool result_ = GetFrame()
                     .GetEventHandler()
                     .GetSelectionController()
                     .HandleGestureTapIfSelectionExist(event_);

  GetFrame().SetView(view);
  EXPECT_FALSE(result_);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExistTest_10thIf) {
  const char* body_content = "<span id = top>this is a sample test</span>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 0))
                                         .Extend(PositionInFlatTree(top, 10))
                                         .Build(),
                                     TextGranularity::kCharacter);

  blink::WebMouseEvent single_click(
      blink::WebMouseEvent::Type::kMouseDown, 0,
      blink::WebInputEvent::GetStaticTimeStampForTests());

  single_click.SetFrameScale(1);
  HitTestLocation location((gfx::Point(20, 5)));
  single_click.button = blink::WebMouseEvent::Button::kLeft;
  single_click.click_count = 1;
  single_click.SetModifiers(
      blink::WebInputEvent::Modifiers::kIsCompatibilityEventForTouch);

  HitTestResult result;

  const MouseEventWithHitTestResults event_(single_click, location, result);

  bool result_ = GetFrame()
                     .GetEventHandler()
                     .GetSelectionController()
                     .HandleGestureTapIfSelectionExist(event_);

  EXPECT_FALSE(result_);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExistTest_15thIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a sample test</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 0))
                                         .Extend(PositionInFlatTree(top, 20))
                                         .Build(),
                                     TextGranularity::kCharacter);

  blink::WebMouseEvent single_click(
      blink::WebMouseEvent::Type::kMouseDown, 0,
      blink::WebInputEvent::GetStaticTimeStampForTests());

  single_click.SetFrameScale(1);
  HitTestLocation location((gfx::Point(20, 5)));
  single_click.button = blink::WebMouseEvent::Button::kLeft;
  single_click.click_count = 1;
  single_click.SetModifiers(
      blink::WebInputEvent::Modifiers::kIsCompatibilityEventForTouch);

  HitTestResult result;

  SetMenuShow(true);

  const MouseEventWithHitTestResults event_(single_click, location, result);

  bool result_ = GetFrame()
                     .GetEventHandler()
                     .GetSelectionController()
                     .HandleGestureTapIfSelectionExist(event_);

  EXPECT_FALSE(result_);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExistTest_16thIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a sample test</span>"
      "<img src='test.img' style='width:100px;height:100px'>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  SetNonDirectionalSelectionIfNeeded(SelectionInFlatTree::Builder()
                                         .Collapse(PositionInFlatTree(top, 0))
                                         .Extend(PositionInFlatTree(top, 20))
                                         .Build(),
                                     TextGranularity::kCharacter);

  blink::WebMouseEvent single_click(
      blink::WebMouseEvent::Type::kMouseDown, 0,
      blink::WebInputEvent::GetStaticTimeStampForTests());

  single_click.SetFrameScale(1);
  HitTestLocation location((gfx::Point(20, 5)));
  single_click.button = blink::WebMouseEvent::Button::kLeft;
  single_click.click_count = 1;
  single_click.SetModifiers(
      blink::WebInputEvent::Modifiers::kIsCompatibilityEventForTouch);

  HitTestResult result;

  SetMenuShow(true);

  const MouseEventWithHitTestResults event_(single_click, location, result);

  bool result_ = GetFrame()
                     .GetEventHandler()
                     .GetSelectionController()
                     .HandleGestureTapIfSelectionExist(event_);

  EXPECT_FALSE(result_);
}

TEST_F(SelectionControllerForIncludeTest, HandleEmptyLineTest_1stIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a sample test</span>"
      "</div>";
  SetBodyContent(body_content);

  const blink::PositionInFlatTree pos;

  auto pos_ = blink::SelectionControllerUtils::HandleEmptyLine(nullptr, pos, 0);
  EXPECT_EQ(pos, pos_);
}

TEST_F(SelectionControllerForIncludeTest, HandleEmptyLineTest_2ndIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>this is a \n sample test</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->firstChild();
  ASSERT_TRUE(top != nullptr);

  const blink::PositionInFlatTree pos(top, 1);

  auto pos_ = blink::SelectionControllerUtils::HandleEmptyLine(top, pos, 1);
  EXPECT_NE(pos, pos_);
}

TEST_F(SelectionControllerForIncludeTest, HandleEmptyLineTest_3rdIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>"
      "<span>first</span>"
      "<span>     </span>"
      "</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->lastChild();
  ASSERT_TRUE(top != nullptr);

  const blink::PositionInFlatTree pos(top, 1);

  auto pos_ = blink::SelectionControllerUtils::HandleEmptyLine(top, pos, 1);
  EXPECT_NE(pos, pos_);
}

TEST_F(SelectionControllerForIncludeTest, HandleEmptyLineTest_4thIf) {
  const char* body_content =
      "<div id='sample' contenteditable>"
      "<span id = top>"
      "<span contenteditable=false>first</span>"
      "<span>     </span>"
      "</span>"
      "</div>";
  SetBodyContent(body_content);

  Node* top = GetDocument().getElementById(AtomicString("top"))->lastChild();
  ASSERT_TRUE(top != nullptr);

  const blink::PositionInFlatTree pos(top, 1);

  auto pos_ = blink::SelectionControllerUtils::HandleEmptyLine(top, pos, 1);
  EXPECT_EQ(pos, pos_);
}

TEST_F(SelectionControllerForIncludeTest, OffsetAdjustWhiteSpace01) {
  WTF::String str = "abc   ";
  int offset = 1, temp = 1;
  SelectionControllerUtils::OffsetAdjustWhiteSpace(offset, temp, str, false);
  EXPECT_EQ(offset, 1);
  EXPECT_EQ(temp, 1);
}

TEST_F(SelectionControllerForIncludeTest, OffsetAdjustWhiteSpace02) {
  WTF::String str = "abc   ";
  int offset = 5, temp = 5;
  SelectionControllerUtils::OffsetAdjustWhiteSpace(offset, temp, str, true);
  EXPECT_EQ(offset, 2);
  EXPECT_EQ(temp, 2);
}

TEST_F(SelectionControllerForIncludeTest, OffsetAdjustWhiteSpace03) {
  WTF::String str = "ab  c";
  int offset = 3, temp = 3;
  SelectionControllerUtils::OffsetAdjustWhiteSpace(offset, temp, str, true);
  EXPECT_EQ(offset, 1);
  EXPECT_EQ(temp, 1);
}

TEST_F(SelectionControllerForIncludeTest, OffsetAdjustWhiteSpace04) {
  WTF::String str = "   abc";
  int offset = 2, temp = 2;
  SelectionControllerUtils::OffsetAdjustWhiteSpace(offset, temp, str, true);
  EXPECT_EQ(offset, 2);
  EXPECT_EQ(temp, 2);

}
TEST_F(SelectionControllerForIncludeTest, OffsetAdjustWhiteSpace05) {
  WTF::String str = "a b";
  int offset = 0, temp = 0;
  SelectionControllerUtils::OffsetAdjustWhiteSpace(offset, temp, str, true);
  EXPECT_EQ(offset, 0);
  EXPECT_EQ(temp, 0);
}

TEST_F(SelectionControllerForIncludeTest, MaxOffsetTrimTailWhiteSpace01) {
  WTF::String str = "any content";
  unsigned len = 0;
  EXPECT_EQ(SelectionControllerUtils::MaxOffsetTrimTailWhiteSpace(str, len),
            0u);
}

TEST_F(SelectionControllerForIncludeTest, MaxOffsetTrimTailWhiteSpace02) {
  WTF::String str = "abc   ";
  unsigned len = 6;
  EXPECT_EQ(SelectionControllerUtils::MaxOffsetTrimTailWhiteSpace(str, len),
            2u);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExist01) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <div id='selectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetFrame().Detach(FrameDetachType::kRemove);
  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  mouse_event.SetModifiers(WebInputEvent::kShiftKey);
  HitTestLocation location((gfx::Point(5, 5)));
  HitTestResult hit_test_result;

  hit_test_result.SetInnerNode(nullptr);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults event_(mouse_event, location,
                                            hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .HandleGestureTapIfSelectionExist(event_);
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExist02) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <div id='selectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "  range.deleteContents();"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  UpdateAllLifecyclePhasesForTest();

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  mouse_event.click_count = 2;
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("selectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults event_(mouse_event, location,
                                            hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .HandleGestureTapIfSelectionExist(event_);
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, HandleGestureTapIfSelectionExist03) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <div id='selectable'>This is a sample text."</div>
    </body>
  )HTML");
  GetDocument().GetSettings()->SetScriptEnabled(true);
  Element* script = GetDocument().CreateRawElement(html_names::kScriptTag);
  ASSERT_TRUE(script != nullptr);
  script->setInnerHTML(
      "document.designMode = 'on';"
      "const selection = window.getSelection();"
      "const html = document.getElementsByTagName('html')[0];"
      "selection.collapse(html);"
      "const range = selection.getRangeAt(0);"

      "function selectstart() {"
      "  const body = document.getElementsByTagName('body')[0];"
      "  range.surroundContents(body);"
      "}"
      "document.addEventListener('selectstart', selectstart);");
  GetDocument().body()->AppendChild(script);
  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  mouse_event.SetModifiers(WebInputEvent::kShiftKey);
  HitTestLocation location((gfx::Point(5, 5)));
  SetCaretAtHitTestResult(
      GetFrame().GetEventHandler().HitTestResultAtLocation(location));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("selectable"));
  ASSERT_TRUE(node == nullptr);

  hit_test_result.SetInnerNode(node);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults event_(mouse_event, location,
                                            hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .HandleGestureTapIfSelectionExist(event_);
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, ShowSelectionByLastLinkHitTestResult00) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
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

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  PhysicalOffset location_with_link(LayoutUnit(105), LayoutUnit(105));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  Element* link = GetDocument().getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  auto* style = link->GetComputedStyle();
  ASSERT_TRUE(style != nullptr);

  hit_test_result_.SetInnerNode(node);

  hit_test_result_.SetURLElement(link);
  SetLinkPress(hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLinkHitTestResult();
  EXPECT_TRUE(result);
}

TEST_F(SelectionControllerForIncludeTest, ShowSelectionByLastLinkHitTestResult01) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <div id='selectable'>This is a sample text."</div>
    </body>
  )HTML");

  GetFrame().Detach(FrameDetachType::kRemove);
  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  mouse_event.SetModifiers(WebInputEvent::kShiftKey);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("selectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults event_(mouse_event, location,
                                            hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLinkHitTestResult();
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, ShowSelectionByLastLinkHitTestResult02) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
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

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  PhysicalOffset location_with_link(LayoutUnit(105), LayoutUnit(105));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_text_ =
      MakeGarbageCollected<LayoutText>(node, String("Content"));
  ASSERT_TRUE(layout_text_ != nullptr);

  Element* link = GetDocument().getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  auto* style = link->GetComputedStyle();
  ASSERT_TRUE(style != nullptr);
  layout_text_->SetStyle(style);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(nullptr);
  hit_test_result_.SetURLElement(link);
  SetLinkPress(hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLinkHitTestResult();
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, ShowSelectionByLastLinkHitTestResult03) {
  SetHtmlInnerHTML(R"HTML(
    <body>
      <div id='selectable'>This is a sample text."</div>
    </body>
  )HTML");
  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kIsCompatibilityEventForTouch,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetFrameScale(1);
  mouse_event.SetModifiers(WebInputEvent::kShiftKey);
  HitTestLocation location((gfx::Point(0, 0)));

  HitTestResult hit_test_result;
  Node* node = GetDocument().getElementById(AtomicString("selectable"));
  ASSERT_TRUE(node != nullptr);

  hit_test_result.SetInnerNode(node);
  const HitTestResult hit_test_result_ = hit_test_result;
  const MouseEventWithHitTestResults event_(mouse_event, location,
                                            hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLinkHitTestResult();
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, ShowSelectionByLastLinkHitTestResult06) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
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

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  PhysicalOffset location_with_link(LayoutUnit(105), LayoutUnit(105));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_text_ =
      MakeGarbageCollected<LayoutText>(node, String("Content"));
  ASSERT_TRUE(layout_text_ != nullptr);

  Element* link = GetDocument().getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  auto* style = link->GetComputedStyle();
  ASSERT_TRUE(style != nullptr);
  layout_text_->SetStyle(style);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(layout_text_);
  hit_test_result_.SetURLElement(link);
  SetLinkPress(hit_test_result_);
  link->remove();

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLinkHitTestResult();
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, ShowSelectionByLastLinkHitTestResult04) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
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
      <a id=linktarget '>Content</a>
    </body>
  )HTML");

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  PhysicalOffset location_with_link(LayoutUnit(105), LayoutUnit(105));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_text_ =
      MakeGarbageCollected<LayoutText>(node, String("Content"));
  ASSERT_TRUE(layout_text_ != nullptr);

  Element* link = GetDocument().getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  auto* style = link->GetComputedStyle();
  ASSERT_TRUE(style != nullptr);
  layout_text_->SetStyle(style);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(layout_text_);
  hit_test_result_.SetURLElement(link);
  SetLinkPress(hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLinkHitTestResult();
  EXPECT_FALSE(result);
}

TEST_F(SelectionControllerForIncludeTest, IsValidAISelection_ValidRange) {
  WTF::Vector<int8_t> select;
  select.push_back(0);
  select.push_back(5);
  EXPECT_TRUE(SelectionControllerUtils::IsValidAISelection(select));
}

TEST_F(SelectionControllerForIncludeTest, IsValidAISelection_ValidRangeWithZeroStart) {
  WTF::Vector<int8_t> select;
  select.push_back(0);
  select.push_back(3);
  EXPECT_TRUE(SelectionControllerUtils::IsValidAISelection(select));
}

TEST_F(SelectionControllerForIncludeTest, IsValidAISelection_FailCase) {
  WTF::Vector<int8_t> select;
  select.push_back(-1);
  select.push_back(-1);
  EXPECT_FALSE(SelectionControllerUtils::IsValidAISelection(select));
}

TEST_F(SelectionControllerForIncludeTest, IsValidAISelection_EmptySelection) {
  WTF::Vector<int8_t> select;
  select.push_back(5);
  select.push_back(5);
  EXPECT_FALSE(SelectionControllerUtils::IsValidAISelection(select));
}

TEST_F(SelectionControllerForIncludeTest, IsValidAISelection_PartialFailStart) {
  WTF::Vector<int8_t> select;
  select.push_back(-1);
  select.push_back(5);
  EXPECT_FALSE(SelectionControllerUtils::IsValidAISelection(select));
}

TEST_F(SelectionControllerForIncludeTest, IsValidAISelection_PartialFailEnd) {
  WTF::Vector<int8_t> select;
  select.push_back(5);
  select.push_back(-1);
  EXPECT_FALSE(SelectionControllerUtils::IsValidAISelection(select));
}

TEST_F(SelectionControllerForIncludeTest, IsValidAISelection_EmptyRangeAtZero) {
  WTF::Vector<int8_t> select;
  select.push_back(0);
  select.push_back(0);
  EXPECT_FALSE(SelectionControllerUtils::IsValidAISelection(select));
}

TEST_F(SelectionControllerForIncludeTest, ShowSelectionByLastLinkHitTestResult05) {
  GetDocument().documentElement()->setInnerHTML(R"HTML(
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
      <a id=linktarget href='about:blank'><span>click on span</span></a>
    </body>
  )HTML");

  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);

  PhysicalOffset location_with_link(LayoutUnit(105), LayoutUnit(105));

  HitTestResult hit_test_result_;
  Node* node =
      GetDocument().getElementById(AtomicString("linktarget"))->firstChild();
  ASSERT_TRUE(node != nullptr);

  auto* layout_text_ =
      MakeGarbageCollected<LayoutText>(node, String("Content"));
  ASSERT_TRUE(layout_text_ != nullptr);

  Element* link = GetDocument().getElementById(AtomicString("linktarget"));
  ASSERT_TRUE(link != nullptr);

  auto* style = link->GetComputedStyle();
  ASSERT_TRUE(style != nullptr);
  layout_text_->SetStyle(style);

  hit_test_result_.SetInnerNode(node);
  hit_test_result_.InnerNode()->SetLayoutObject(layout_text_);
  hit_test_result_.SetURLElement(link);
  SetLinkPress(hit_test_result_);

  bool result = GetFrame()
                    .GetEventHandler()
                    .GetSelectionController()
                    .ShowSelectionByLastLinkHitTestResult();
  EXPECT_TRUE(result);
}
