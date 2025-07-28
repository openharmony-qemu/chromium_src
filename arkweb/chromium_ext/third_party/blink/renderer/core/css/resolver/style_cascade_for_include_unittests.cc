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

namespace blink {
namespace {

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

}  // namespace
}  // namespace blink
