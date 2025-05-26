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

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"
#include "third_party/blink/renderer/core/loader/subresource_filter.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/ohos_adblock/ohos_adblock_util.h"
#endif

namespace blink {

#if BUILDFLAG(ARKWEB_ADBLOCK)
void DidMatchCssRuleForInclude(const RuleData& rule_data,
                               const ElementResolveContext& context,
                               MatchResult& result) {
  if (auto* rule = rule_data.Rule()) {
    if (rule->IsForAdBlock()) {
      result.SetDisplayNoneFromAdblock(true);
      if (context.GetElement().GetDocument().Loader() &&
          context.GetElement().GetDocument().Loader()->GetSubresourceFilter()) {
        context.GetElement()
            .GetDocument()
            .Loader()
            ->GetSubresourceFilter()
            ->utils->DidMatchCssRule(
                context.GetElement().GetDocument().Url(),
                GetDomPath(context.GetElement(), false, true), true);
      }
    }
  }

  if (auto* rule = rule_data.Rule()) {
    if (rule->IsForUserAdBlock()) {
      if (context.GetElement().GetDocument().Loader() &&
          context.GetElement()
              .GetDocument()
              .Loader()
              ->AsArkWebDocumentLoaderExt()
              ->GetUserSubresourceFilter() &&
          !(context.GetElement()
                .GetDocument()
                .Loader()
                ->AsArkWebDocumentLoaderExt()
                ->GetUserSubresourceFilter()
                ->utils->GetDidFinishLoad())) {
        context.GetElement()
            .GetDocument()
            .Loader()
            ->AsArkWebDocumentLoaderExt()
            ->GetUserSubresourceFilter()
            ->utils->DidMatchCssRule(
                context.GetElement().GetDocument().Url(),
                GetDomPath(context.GetElement(), false, true));
      }
      result.SetDisplayNoneFromUserAdblock(true);
    }
  }
}
#endif
}  // namespace blink