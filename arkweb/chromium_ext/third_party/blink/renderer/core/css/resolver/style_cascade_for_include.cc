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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/css/resolver/style_cascade_for_include.h"

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "third_party/blink/renderer/core/loader/document_loader.h"
#include "third_party/blink/renderer/core/loader/subresource_filter.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/ohos_adblock/ohos_adblock_util.h"
#endif

namespace blink {

void StyleCascadeUtil::LookupAndApplyDeclarationExt(
    raw_ptr<StyleCascade> StyleCascadeObj,
    const CSSProperty& property,
    blink::EDisplay& tmp) {
  if (StyleCascadeObj->match_result_.GetDisplayNoneFromAdblock() &&
      StyleCascadeObj->state_.GetElement().tagName().UpperASCII() != "HEAD" &&
      StyleCascadeObj->state_.GetElement().GetDocument().Loader() &&
      StyleCascadeObj->state_.GetElement()
          .GetDocument()
          .Loader()
          ->GetSubresourceFilter()) {
    if (property.GetCSSPropertyName().ToAtomicString().Utf8() == "display" &&
        StyleCascadeObj->state_.StyleBuilder().Display() ==
            blink::EDisplay::kNone &&
        StyleCascadeObj->state_.StyleBuilder().Display() != tmp) {
      StyleCascadeObj->state_.GetElement()
          .GetDocument()
          .Loader()
          ->GetSubresourceFilter()
          ->utils->DidMatchCssRule(
              StyleCascadeObj->state_.GetElement().GetDocument().Url(),
              GetDomPath(StyleCascadeObj->state_.GetElement(), false, true));
      LOG(INFO) << "[AdBlock] Element("
                << StyleCascadeObj->state_.GetElement().ToString()
                << " ) dom path:"
                << GetDomPath(StyleCascadeObj->state_.GetElement(), false,
                              true);
    }
  }

  if (StyleCascadeObj->match_result_.GetDisplayNoneFromUserAdblock() &&
      StyleCascadeObj->state_.GetElement().tagName().UpperASCII() != "HEAD" &&
      StyleCascadeObj->state_.GetElement().GetDocument().Loader() &&
      StyleCascadeObj->state_.GetElement()
          .GetDocument()
          .Loader()
          ->AsArkWebDocumentLoaderExt()
          ->GetUserSubresourceFilter()) {
    if (property.GetCSSPropertyName().ToAtomicString().Utf8() == "display" &&
        StyleCascadeObj->state_.StyleBuilder().Display() ==
            blink::EDisplay::kNone &&
        StyleCascadeObj->state_.StyleBuilder().Display() != tmp) {
      StyleCascadeObj->state_.GetElement()
          .GetDocument()
          .Loader()
          ->AsArkWebDocumentLoaderExt()
          ->GetUserSubresourceFilter()
          ->utils->DidMatchCssRule(
              StyleCascadeObj->state_.GetElement().GetDocument().Url(),
              GetDomPath(StyleCascadeObj->state_.GetElement(), false, true));
      LOG(INFO) << "[User AdBlock] Element("
                << StyleCascadeObj->state_.GetElement().ToString()
                << ") dom path:"
                << GetDomPath(StyleCascadeObj->state_.GetElement(), false,
                              true);
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
      LOG_FEEDBACK(INFO) << "[User AdBlock] Element("
                         << StyleCascadeObj->state_.GetElement().ToString()
                         << ") dom path:"
                         << GetDomPath(StyleCascadeObj->state_.GetElement(),
                                       false, true);
#endif
    }
  }
}

CSSVariableData* StyleCascadeUtil::GetEnvironmentVariableExt(
    raw_ptr<const StyleCascade> StyleCascadeObj,
    const AtomicString& name,
    WTF::Vector<unsigned>& indices,
    bool is_ua_scope) {
  CSSVariableData* result =
      StyleCascadeObj->state_.GetDocument()
          .GetStyleEngine()
          .EnsureEnvironmentVariables()
          .ResolveVariable(name, std::move(indices), !is_ua_scope);
  LOG(DEBUG) << __func__ << " " << name << "="
             << (result ? result->OriginalText().Utf8() : std::string("null"));
  return result;
}

}  // namespace blink
