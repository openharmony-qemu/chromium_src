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
#include "third_party/blink/renderer/core/css/resolver/style_cascade.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/css/resolver/style_cascade_for_include.h"
#include "arkweb/build/features/features.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/testing/runtime_enabled_features_test_helpers.h"
#include "third_party/blink/renderer/core/loader/subresource_filter.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/platform/web_document_subresource_filter.h"

namespace blink {
namespace {

class DummyWebDocumentSubresourceFilter : public blink::WebDocumentSubresourceFilter {
 public:
  LoadPolicy GetLoadPolicy(const WebURL&, network::mojom::RequestDestination) override { return kAllow; }
  LoadPolicy GetLoadPolicyForWebSocketConnect(const WebURL&) override { return kAllow; }
  LoadPolicy GetLoadPolicyForWebTransportConnect(const WebURL&) override { return kAllow; }
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void ClearStatistics() override {}
  std::unique_ptr<std::string> GetElementHidingSelectors(const WebURL&, bool) override {
    return std::make_unique<std::string>();
  }
  bool HasGenericHideTypeOption(const WebURL&, const url::Origin&) override { return false; }
  bool HasElemHideTypeOption(const WebURL&, const url::Origin&) override { return false; }
  bool HasDocumentTypeOption(const WebURL&, const url::Origin&) override { return false; }
  void DidMatchCssRule(const WebURL&, const std::string&, bool) override {}
  void SetDidFinishLoad(bool) override {}
  bool GetDidFinishLoad() override { return true; }
  std::unique_ptr<std::vector<std::string>> GetUserDomPathSelectors(const blink::WebURL&, bool) override {
    return std::make_unique<std::vector<std::string>>();
  }
  void set_activation_state(bool) override {}
#endif
  void ReportDisallowedLoad() override {}
  bool ShouldLogToConsole() override { return false; }
};

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument());
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_AdblockFalse) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument());
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(false);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_TagIsHead) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument());
  auto qualifiedName = QualifiedName(g_null_atom, AtomicString("HEAD"), g_null_atom);
  cascade.State().GetElement().SetTagNameForCreateElementNS(qualifiedName);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_LoaderIsNull) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument());
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  auto* doc = &cascade.State().GetElement().GetDocument();
  doc->Shutdown();
  auto* loader = doc->Loader();
  EXPECT_EQ(loader, nullptr);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_SubresourceFilterIsNull) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument(), DocumentElement());
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_PropertyNotDisplay) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kWidth;
  TestCascade cascade(GetDocument(), DocumentElement());
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  auto* filter_impl = new DummyWebDocumentSubresourceFilter();
  EXPECT_NE(filter_impl, nullptr);
  loader->SetSubresourceFilter(filter_impl);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_DisplayNotNone) {
  EDisplay display_value = EDisplay::kBlock;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument(), DocumentElement());
  cascade.State().StyleBuilder().SetDisplay(EDisplay::kBlock);
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  auto* filter_impl = new DummyWebDocumentSubresourceFilter();
  EXPECT_NE(filter_impl, nullptr);
  loader->SetSubresourceFilter(filter_impl);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_DisplayEqualTmp) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument(), DocumentElement());
  cascade.State().StyleBuilder().SetDisplay(EDisplay::kNone);
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  auto* filter_impl = new DummyWebDocumentSubresourceFilter();
  EXPECT_NE(filter_impl, nullptr);
  loader->SetSubresourceFilter(filter_impl);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_AllTrue) {
  EDisplay display_value = EDisplay::kBlock;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument(), DocumentElement());
  cascade.State().StyleBuilder().SetDisplay(EDisplay::kNone);
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  auto* filter_impl = new DummyWebDocumentSubresourceFilter();
  EXPECT_NE(filter_impl, nullptr);
  loader->SetSubresourceFilter(filter_impl);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_UserAdblock_LoaderNull) {
  EDisplay display_value = EDisplay::kBlock;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument());
  auto* doc = &cascade.State().GetElement().GetDocument();
  doc->Shutdown();
  auto* loader = doc->Loader();
  EXPECT_EQ(loader, nullptr);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_UserAdblock_FilterNull) {
  EDisplay display_value = EDisplay::kBlock;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument(), DocumentElement());
  cascade.State().StyleBuilder().SetDisplay(EDisplay::kNone);
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  loader->AsArkWebDocumentLoaderExt()->SetUserSubresourceFilter(nullptr);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_UserAdblock_PropertyNotDisplay) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kWidth;
  TestCascade cascade(GetDocument(), DocumentElement());
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  LocalDOMWindow* context = nullptr;
  if (loader->GetFrame() && loader->GetFrame()->DomWindow()) {
    context = loader->GetFrame()->DomWindow();
  }
  EXPECT_NE(context, nullptr);
  std::unique_ptr<WebDocumentSubresourceFilter> web_filter = std::make_unique<DummyWebDocumentSubresourceFilter>();
  SubresourceFilter* filter = MakeGarbageCollected<SubresourceFilter>(context, std::move(web_filter));
  EXPECT_NE(filter, nullptr);
  loader->AsArkWebDocumentLoaderExt()->SetUserSubresourceFilter(filter);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_UserAdblock_DisplayNotNone) {
  EDisplay display_value = EDisplay::kBlock;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument(), DocumentElement());
  cascade.State().StyleBuilder().SetDisplay(EDisplay::kBlock);
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  LocalDOMWindow* context = nullptr;
  if (loader->GetFrame() && loader->GetFrame()->DomWindow()) {
    context = loader->GetFrame()->DomWindow();
  }
  EXPECT_NE(context, nullptr);
  std::unique_ptr<WebDocumentSubresourceFilter> web_filter = std::make_unique<DummyWebDocumentSubresourceFilter>();
  SubresourceFilter* filter = MakeGarbageCollected<SubresourceFilter>(context, std::move(web_filter));
  EXPECT_NE(filter, nullptr);
  loader->AsArkWebDocumentLoaderExt()->SetUserSubresourceFilter(filter);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}


TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_UserAdblock_DisplayEqualTmp) {
  EDisplay display_value = EDisplay::kNone;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument(), DocumentElement());
  cascade.State().StyleBuilder().SetDisplay(EDisplay::kNone);
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  LocalDOMWindow* context = nullptr;
  if (loader->GetFrame() && loader->GetFrame()->DomWindow()) {
    context = loader->GetFrame()->DomWindow();
  }
  EXPECT_NE(context, nullptr);
  std::unique_ptr<WebDocumentSubresourceFilter> web_filter = std::make_unique<DummyWebDocumentSubresourceFilter>();
  SubresourceFilter* filter = MakeGarbageCollected<SubresourceFilter>(context, std::move(web_filter));
  EXPECT_NE(filter, nullptr);
  loader->AsArkWebDocumentLoaderExt()->SetUserSubresourceFilter(filter);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, LookupAndApplyDeclarationExt_UserAdblock_AllTrue) {
  EDisplay display_value = EDisplay::kBlock;
  CSSPropertyID test_property = CSSPropertyID::kDisplay;
  TestCascade cascade(GetDocument(), DocumentElement());
  cascade.State().StyleBuilder().SetDisplay(EDisplay::kNone);
  auto* loader = cascade.State().GetElement().GetDocument().Loader();
  EXPECT_NE(loader, nullptr);
  LocalDOMWindow* context = nullptr;
  if (loader->GetFrame() && loader->GetFrame()->DomWindow()) {
    context = loader->GetFrame()->DomWindow();
  }
  EXPECT_NE(context, nullptr);
  std::unique_ptr<WebDocumentSubresourceFilter> web_filter = std::make_unique<DummyWebDocumentSubresourceFilter>();
  SubresourceFilter* filter = MakeGarbageCollected<SubresourceFilter>(context, std::move(web_filter));
  EXPECT_NE(filter, nullptr);
  loader->AsArkWebDocumentLoaderExt()->SetUserSubresourceFilter(filter);
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromAdblock(true);
  StyleCascadeObj->MutableMatchResult().SetDisplayNoneFromUserAdblock(true);
  StyleCascadeUtil::LookupAndApplyDeclarationExt(
      StyleCascadeObj,
      CSSProperty::Get(test_property),
      display_value);
}

TEST_F(StyleCascadeTest, GetEnvironmentVariableExt) {
  AtomicString var_name("--test-var");
  WTF::Vector<unsigned> indices;
  bool is_ua_scope = false;
  TestCascade cascade(GetDocument());
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  CSSVariableData* result = StyleCascadeUtil::GetEnvironmentVariableExt(
      StyleCascadeObj,
      var_name,
      indices,
      is_ua_scope);
}

TEST_F(StyleCascadeTest, GetUAScopeEnvironmentVariable) {
  AtomicString var_name("--ua-var");
  WTF::Vector<unsigned> indices;
  bool is_ua_scope = true;
  TestCascade cascade(GetDocument());
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  CSSVariableData* result = StyleCascadeUtil::GetEnvironmentVariableExt(
      StyleCascadeObj,
      var_name,
      indices,
      is_ua_scope);
}

TEST_F(StyleCascadeTest, GetEnvironmentVariableExt_ReturnValue) {
  AtomicString var_name("safe-area-inset-top");
  auto variable_data = CSSVariableData::Create("10px", false, true);
  DocumentStyleEnvironmentVariables& env_vars = GetDocument().GetStyleEngine().EnsureEnvironmentVariables();
  env_vars.ResolveVariable(var_name, WTF::Vector<unsigned>(), true);
  WTF::Vector<unsigned> indices;
  bool is_ua_scope = false;
  TestCascade cascade(GetDocument());
  raw_ptr<StyleCascade> StyleCascadeObj = &cascade.InnerCascade();
  CSSVariableData* result = StyleCascadeUtil::GetEnvironmentVariableExt(
      StyleCascadeObj,
      var_name,
      indices,
      is_ua_scope);
  EXPECT_NE(result, nullptr);
}

}  // namespace
}  // namespace blink
