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

namespace subresource_filter {

namespace {

#if BUILDFLAG(ARKWEB_ADBLOCK)
std::unique_ptr<const std::vector<const url_pattern_index::flat::CssRule*>>
FindMatchingCssRule(const subresource_filter::MemoryMappedRuleset* ruleset,
                    const url::Origin& document_origin,
                    const GURL& request_url) {
  subresource_filter::mojom::ActivationState state;
  state.activation_level = subresource_filter::mojom::ActivationLevel::kEnabled;
  subresource_filter::DocumentSubresourceFilterExt filter(
      document_origin, state, ruleset, kSafeBrowsingRulesetConfig.uma_tag);
  return filter.FindMatchingCssRule(request_url, true);
}
#endif

}  // namespace

#if BUILDFLAG(ARKWEB_ADBLOCK)
std::unique_ptr<const std::vector<const url_pattern_index::flat::CssRule*>>
FilterTool::MatchCssRuleImpl(std::string_view document_origin,
                             std::string_view url) {
  return FindMatchingCssRule(ruleset_.get(), ParseOrigin(document_origin),
                             ParseRequestUrl(url));
}
#endif

class FilterToolUtils {
 public:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  static const url_pattern_index::flat::UrlRule* MatchRuleImplExt(
      const std::string& origin,
      const std::string& request_url,
      const std::string& request_type,
      bool& blocked,
      std::unordered_map<const url_pattern_index::flat::UrlRule*, int>
          matched_url_rules,
      std::unordered_map<const url_pattern_index::flat::CssRule*, int>
          matched_css_rules,
      raw_ptr<FilterTool> obj) {
    const url_pattern_index::flat::UrlRule* url_rule =
        obj->MatchUrlRuleImpl(origin, request_url, request_type, &blocked);
    if (url_rule) {
      matched_url_rules[url_rule] += 1;
    }

    std::unique_ptr<const std::vector<const url_pattern_index::flat::CssRule*>>
        css_rules = obj->MatchCssRuleImpl(origin, request_url);
    if (css_rules) {
      for (auto* css_rule : *css_rules) {
        if (css_rule) {
          matched_css_rules[css_rule] += 1;
        }
      }
    }
    return url_rule;
  }
#endif
};

}  // namespace subresource_filter
