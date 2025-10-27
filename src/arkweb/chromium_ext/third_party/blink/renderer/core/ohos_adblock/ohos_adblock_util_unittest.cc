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
#include "arkweb/chromium_ext/third_party/blink/renderer/core/ohos_adblock/ohos_adblock_util.h"

#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_shadow_root_init.h"
#include "third_party/blink/renderer/core/css/resolver/style_resolver.h"
#include "third_party/blink/renderer/core/css/style_engine.h"
#include "third_party/blink/renderer/core/dom/comment.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/flat_tree_traversal.h"
#include "third_party/blink/renderer/core/dom/layout_tree_builder.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/core/dom/node_computed_style.h"
#include "third_party/blink/renderer/core/dom/processing_instruction.h"
#include "third_party/blink/renderer/core/dom/pseudo_element.h"
#include "third_party/blink/renderer/core/dom/shadow_root.h"
#include "third_party/blink/renderer/core/dom/slot_assignment_engine.h"
#include "third_party/blink/renderer/core/editing/testing/editing_test_base.h"
#include "third_party/blink/renderer/core/html/html_collection.h"
#include "third_party/blink/renderer/core/html/html_div_element.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"

namespace blink {

class OhosAdblockUtilTest : public EditingTestBase {};

TEST_F(OhosAdblockUtilTest, IsValidTarget_001) {
  const char* body_content =
      "<a id=one href='http://www.msn.com'>one</a><b id=two>two</b>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  Node* two = GetDocument().getElementById(AtomicString("two"));
  Element* target = To<Element>(one);
  EXPECT_NE(target, nullptr);
  int top = target->OffsetTop();
  int left = target->OffsetLeft();
  int height = target->OffsetHeight();
  int width = target->OffsetWidth();
  EXPECT_FALSE(one->CanStartSelection());
  EXPECT_FALSE(one->firstChild()->CanStartSelection());
  EXPECT_TRUE(two->CanStartSelection());
  EXPECT_TRUE(two->firstChild()->CanStartSelection());
  auto result = IsValidTarget(one, top, left, width, height);
  EXPECT_FALSE(result);
}

TEST_F(OhosAdblockUtilTest, IsValidTarget_002) {
  const char* body_content = "<div id=host><span id=one>one</span></div>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  Element* target = To<Element>(one);
  EXPECT_NE(target, nullptr);
  int top = target->OffsetTop();
  int left = target->OffsetLeft();
  int height = target->OffsetHeight();
  int width = target->OffsetWidth();
  auto result = IsValidTarget(one, top, left, width, height);
  EXPECT_TRUE(result);
  result = IsValidTarget(one, top + 1, left, width, height);
  EXPECT_FALSE(result);
  result = IsValidTarget(one, top, left + 1, width, height);
  EXPECT_FALSE(result);
  result = IsValidTarget(one, top, left, width - 1, height);
  EXPECT_FALSE(result);
  result = IsValidTarget(one, top, left, width, height - 1);
  EXPECT_FALSE(result);
}

TEST_F(OhosAdblockUtilTest, IsValidTarget_003) {
  const char* body_content =
      "<div id=host><span id=one>one</span><span id=two>two</span></div>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  Element* target = To<Element>(one);
  EXPECT_NE(target, nullptr);
  int top = target->OffsetTop();
  int left = target->OffsetLeft();
  int height = target->OffsetHeight();
  int width = target->OffsetWidth();
  auto result = IsValidTarget(one, top, left, width, height);
  EXPECT_FALSE(result);
}

TEST_F(OhosAdblockUtilTest, IsValidTarget_004) {
  const char* body_content = "<div id=host><span id=one>one</span></div>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  Element* target = To<Element>(one);
  EXPECT_NE(target, nullptr);
  int top = target->OffsetTop();
  int left = target->OffsetLeft();
  int height = target->OffsetHeight();
  int width = target->OffsetWidth();
  one->SetParentOrShadowHostNode(nullptr);
  auto result = IsValidTarget(nullptr, top, left, width, height);
  EXPECT_FALSE(result);
  result = IsValidTarget(one, top, left, width, height);
  EXPECT_FALSE(result);
}

TEST_F(OhosAdblockUtilTest, IsValidTarget_005) {
  SetBodyContent(R"HTML(<body><span id="id"></span></body>)HTML");
  Node* one = GetDocument().getElementById(AtomicString("id"));
  auto result = IsValidTarget(one, 0, 0, 0, 0);
  EXPECT_FALSE(result);
}

TEST_F(OhosAdblockUtilTest, IsValidTarget_006) {
  SetBodyContent(
    "<div id='host'><span id='one'></span><span id='two' style='display:block;width:0px;height:10px;'></span></div>");
  Node* one = GetDocument().getElementById(AtomicString("one"));
  Node* two = GetDocument().getElementById(AtomicString("two"));
  auto result = IsValidTarget(one, 0, 0, 10, 10);
  EXPECT_TRUE(result);
}

TEST_F(OhosAdblockUtilTest, IsValidTarget_007) {
  SetBodyContent(
    "<div id='host'><span id='one'></span><span id='two' style='display:block;width:10px;height:0px;'></span></div>");
  Node* one = GetDocument().getElementById(AtomicString("one"));
  Node* two = GetDocument().getElementById(AtomicString("two"));
  auto result = IsValidTarget(one, 0, 0, 10, 10);
  EXPECT_TRUE(result);
}

TEST_F(OhosAdblockUtilTest, IsValidTarget_008) {
  SetBodyContent("<div>hello</div>");
  Node* html_node = GetDocument().documentElement();
  ASSERT_NE(html_node, nullptr);
  EXPECT_EQ(html_node->parentNode(), &GetDocument());
  Element* html_element = To<Element>(html_node);
  int top = html_element->OffsetTop();
  int left = html_element->OffsetLeft();
  int width = html_element->OffsetWidth();
  int height = html_element->OffsetHeight();
  bool result = IsValidTarget(html_node, top, left, width, height);
  EXPECT_FALSE(result);
}

TEST_F(OhosAdblockUtilTest, PrefixedElementClassNames_001) {
  const char* body_content = "<div id=host><span id=one>one</span></div>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  PrefixedElementClassNames(*one, true);
  PrefixedElementClassNames(*one, false);
}

TEST_F(OhosAdblockUtilTest, PrefixedElementClassNames_002) {
  const char* body_content = "<div id=host><span id=one>one</span></div>";
  SetBodyContent(body_content);
  Node* one = GetDocument().getElementById(AtomicString("one"));
  PrefixedElementClassNames(*one, false);
}

TEST_F(OhosAdblockUtilTest, PrefixedElementClassNames_003) {
  SetBodyContent("<div id=\"root\"><b id=two></b><b id=t>t</b></div>");
  Element* host = GetDocument().firstElementChild();
  PrefixedElementClassNames(*host, false);
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_001) {
  GetDocument().body()->setHTMLUnsafe(R"HTML(
    <body>
      <template shadowrootmode="open">
        <slot id="slot1">
          <span id="fallback1"></span>
        </slot>
        <slot id="slot2">
          <span id="fallback2"></span>
        </slot>
      </template>
      <div id="slotted">one</div>
      <div id="not_slotted" slot="notfound"></div>
    </body>
  )HTML");
  Element* host = GetDocument().firstElementChild();
  ShadowRoot& shadow_root =
      host->AttachShadowRootForTesting(ShadowRootMode::kOpen);
  shadow_root.setInnerHTML("<div><slot name=x></slot></div>");
  UpdateAllLifecyclePhasesForTest();
  GetDomPathStep(*host, true, true, true);
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_002) {
  GetDocument().body()->setHTMLUnsafe(R"HTML(
    <input type="text">"hello"</input>
  )HTML");
  Element* host = GetDocument().firstElementChild();
  ShadowRoot& shadow_root =
      host->AttachShadowRootForTesting(ShadowRootMode::kOpen);
  shadow_root.setInnerHTML("<div><slot name=x></slot></div>");
  UpdateAllLifecyclePhasesForTest();
  GetDomPathStep(*host, false, true, true);
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_003) {
  SetBodyContent("<div id=\"root\"><b id=two></b><b id=t>t</b></div>");
  Element* root = GetDocument().getElementById(AtomicString("root"));
  Element* host = root->firstElementChild();
  host->CreateUserAgentShadowRoot();
  GetDomPathStep(*host, false, true, true);
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_004) {
  SetBodyContent("Text");
  Node* node = GetDocument().body()->firstChild();
  GetDomPathStep(*node, false, true, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_005) {
  SetBodyContent("<input id=\"text\">\"hello\"</input>");
  Node* node = GetDocument().body()->firstChild();
  Element* root = To<Element>(node);
  root->CreateUserAgentShadowRoot();
  GetDomPathStep(*root, false, true, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_006) {
  SetBodyContent("<input id=\"text\">\"hello\"</input>");
  Node* node = GetDocument().body()->firstChild();
  Element* root = To<Element>(node);
  root->CreateUserAgentShadowRoot();
  GetDomPathStep(*root, true, true, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_060) {
  SetBodyContent(
      "<html><head>o</head><body><b id=\"root\">one</b></body></html>");
  Node* node = GetDocument().getElementById(AtomicString("root"));
  Node* body = node->parentNode();
  Node* html = body->parentNode();
  GetDomPathStep(*html, true, true, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_061) {
  SetBodyContent("<html><body id=\"root\">\"hello\"</body></html>");
  Node* node = GetDocument().body()->firstChild();
  Node* root = node->parentNode();
  Node* root1 = root->parentNode();
  GetDomPathStep(*root, true, true, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_062) {
  SetBodyContent(
      "<html><head>o</head><body><b id=\"root\">one</b></body></html>");
  Node* node = GetDocument().getElementById(AtomicString("root"));
  Node* up = node->parentNode();
  Node* root = up->parentNode();
  Node* child = root->firstChild();
  GetDomPathStep(*child, true, true, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_007) {
  SetBodyContent(
      "<html><head>o</head><body><b id=\"root\">one</b></body></html>");
  Node* node = GetDocument().getElementById(AtomicString("root"));
  Node* up = node->parentNode();
  Node* html = up->parentNode();
  GetDomPathStep(*html, false, true, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_008) {
  SetBodyContent(
      "<div id=\"root\"><b id=\"one\"></b><b id=\"two\"></b></div>");
  Element* root = GetDocument().getElementById(AtomicString("one"));
  root->CreateUserAgentShadowRoot();
  Element* root1 = GetDocument().getElementById(AtomicString("two"));
  root1->CreateUserAgentShadowRoot();
  GetDomPathStep(*root, false, true, true);
  EXPECT_TRUE(root->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPathStep_009) {
  SetBodyContent(
      "<div id=\"root\"><div id=\"one\"></div><div id=\"two\"></div></div>");
  Element* root = GetDocument().getElementById(AtomicString("two"));
  root->AttachShadowRootForTesting(ShadowRootMode::kOpen);
  Element* root1 = GetDocument().getElementById(AtomicString("one"));
  root1->AttachShadowRootForTesting(ShadowRootMode::kOpen);
  GetDomPathStep(*root, false, true, true);
  EXPECT_TRUE(root->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPath_001) {
  SetBodyContent(
      "<html><head>o</head><body><b id=\"root\">one</b></body></html>");
  Node* node = GetDocument().getElementById(AtomicString("root"));
  Node* up = node->parentNode();
  Node* html = up->parentNode();
  GetDomPath(*html, false, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPath_002) {
  SetBodyContent(
      "<html><head>o</head><body><b id=\"root\">one</b></body></html>");
  Node* node = GetDocument().body()->firstChild();
  GetDomPath(*node, false, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPath_003) {
  SetBodyContent(
      "<html><head>o</head><body><b id=\"root\">one</b></body></html>");
  Node* node = GetDocument().getElementById(AtomicString("root"));
  Node* up = node->parentNode();
  Node* html = up->parentNode();
  GetDomPath(*html, false, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPath_004) {
  SetBodyContent("<input id=\"text\">\"hello\"</input>");
  Node* node = GetDocument().body()->firstChild();
  Element* root = To<Element>(node);
  root->CreateUserAgentShadowRoot();
  GetDomPath(*root, false, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPath_005) {
  SetBodyContent("<input id=\"text\">\"hello\"</input>");
  Node* node = GetDocument().body()->firstChild();
  Element* root = To<Element>(node);
  root->CreateUserAgentShadowRoot();
  GetDomPath(*root, false, true);
  EXPECT_TRUE(node->CanStartSelection());
}

TEST_F(OhosAdblockUtilTest, GetDomPath_006) {
  SetBodyContent("<form><input type='email' value='test@'></form>");
  HTMLCollection* inputs = GetDocument().getElementsByTagName(AtomicString("input"));
  ASSERT_GT(inputs->length(), 0);
  Element* input = To<Element>(inputs->item(0));
  ASSERT_NE(input, nullptr);
  std::string result = GetDomPath(*input, false, true);
  EXPECT_EQ(result, "HTML>BODY>FORM>INPUT[type=email]");
}

TEST_F(OhosAdblockUtilTest, GetDomPath_007) {
  SetBodyContent("<div id='app'><main><section><p class='intro'>Hello</p></section></main></div>");
  Element* app = GetDocument().getElementById(AtomicString("app"));
  ASSERT_NE(app, nullptr);
  HTMLCollection* ps = app->getElementsByTagName(AtomicString("p"));
  ASSERT_GT(ps->length(), 0);
  Element* p = To<Element>(ps->item(0));
  ASSERT_NE(p, nullptr);
  std::string result = GetDomPath(*p, false, false);
  EXPECT_EQ(result, "DIV#app>MAIN>SECTION>P");
}
}  // namespace blink