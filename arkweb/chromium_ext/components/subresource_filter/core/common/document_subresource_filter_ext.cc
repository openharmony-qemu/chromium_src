/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/components/subresource_filter/core/common/document_subresource_filter_ext.h"

#include "url/gurl.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#endif

namespace subresource_filter {

DocumentSubresourceFilterExt::DocumentSubresourceFilterExt(
    url::Origin document_origin,
    mojom::ActivationState activation_state,
    scoped_refptr<const MemoryMappedRuleset> ruleset,
    std::string_view uma_tag)
    : DocumentSubresourceFilter(document_origin,
                                activation_state,
                                ruleset,
                                uma_tag) {}

#if BUILDFLAG(ARKWEB_ADBLOCK)
std::unique_ptr<std::string> DocumentSubresourceFilterExt::GetSelectors(
    const GURL& document_url,
    bool disable_generic_rules) const {
  auto rules = FindMatchingCssRule(document_url, disable_generic_rules);
  if (!rules) {
    return nullptr;
  }

  std::unique_ptr<std::string> selectors = std::make_unique<std::string>();
  // Matched 21823 css rule(21636 generic and 187 special) in
  // https://www.baidu.com, selectors.size=992794
  selectors->reserve(1310720);  //(1024+256)*1024=1310720

  for (auto* rule : *rules) {
    if (!rule) {
      continue;
    }

    if (rule->css_selector()) {
      *selectors +=
          rule->css_selector()->str() + " {display: none !important;}\n";
    }
  }

  return selectors;
}

std::unique_ptr<const std::vector<const url_pattern_index::flat::CssRule*>>
DocumentSubresourceFilterExt::FindMatchingCssRule(
    const GURL& document_url,
    bool disable_generic_rules) const {
  if (activation_state_.filtering_disabled_for_document) {
    return nullptr;
  }

  if (document_url.SchemeIs(url::kDataScheme)) {
    return nullptr;
  }

  return ruleset_matcher_.MatchedCssRule(document_url, disable_generic_rules);
}

bool DocumentSubresourceFilterExt::HasGenericHideTypeOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return ruleset_matcher_.HasGenericHideOption(document_url,
                                               parent_document_origin);
}

bool DocumentSubresourceFilterExt::HasElemHideTypeOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return ruleset_matcher_.HasElemHideOption(document_url,
                                            parent_document_origin);
}

bool DocumentSubresourceFilterExt::HasDocumentTypeOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return ruleset_matcher_.HasDocumentOption(document_url,
                                            parent_document_origin);
}

void DocumentSubresourceFilterExt::DidMatchCssRule(const GURL& document_url,
                                                   const std::string& dom_path,
                                                   // unsigned rule_line_num,
                                                   bool is_for_report) {
  std::string tmp = document_url.spec() + "##" + dom_path;

  if (!GetDidFinishLoad() && is_for_report) {
    return;
  }

  for (auto& pair : statistics_.loads_disallowed_url_map) {
    if (tmp.find(pair.first) != tmp.npos) {
      return;
    }

    if (pair.first.find(tmp) != pair.first.npos) {
      statistics_.loads_disallowed_url_map.erase(pair.first);
      break;
    }
  }

  statistics_.loads_disallowed_url_map.insert(std::make_pair(
      tmp, url_pattern_index::proto::ElementType::ELEMENT_TYPE_OTHER));
}

void DocumentSubresourceFilterExt::ClearStatistics() {
  statistics_.num_loads_total = 0;
  statistics_.num_loads_evaluated = 0;
  statistics_.num_loads_matching_rules = 0;
  statistics_.num_loads_disallowed = 0;
  statistics_.loads_disallowed_url_map.clear();
}

std::unique_ptr<std::vector<std::string>>
DocumentSubresourceFilterExt::GetUserDomPathSelectors(
    const GURL& document_url,
    bool disable_generic_rules) const {
  auto rules = FindMatchingCssRule(document_url, disable_generic_rules);
  if (!rules) {
    return nullptr;
  }
  std::unique_ptr<std::vector<std::string>> selectors =
      std::make_unique<std::vector<std::string>>();
  for (auto* rule : *rules) {
    if (!rule) {
      continue;
    }
    if (rule->css_selector()) {
      selectors->push_back(rule->css_selector()->str());
    }
  }
  return selectors;
}
#endif

}  // namespace subresource_filter
