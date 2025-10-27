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

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CORE_COMMON_DOCUMENT_SUBRESOURCE_FILTER_EXT_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CORE_COMMON_DOCUMENT_SUBRESOURCE_FILTER_EXT_H_

#include "components/subresource_filter/core/common/document_subresource_filter.h"
#include "components/subresource_filter/core/common/memory_mapped_ruleset.h"
#include "url/origin.h"

namespace subresource_filter {

// Performs filtering of subresource loads in the scope of a given document.
class DocumentSubresourceFilterExt : public DocumentSubresourceFilter {
 public:
  DocumentSubresourceFilterExt(url::Origin document_origin,
                               mojom::ActivationState activation_state,
                               scoped_refptr<const MemoryMappedRuleset> ruleset,
                               std::string_view uma_tag);

  DocumentSubresourceFilterExt* AsDocumentSubresourceFilterExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void ClearStatistics();

  std::unique_ptr<std::string> GetSelectors(const GURL& url,
                                            bool disable_generic_rules) const;

  std::unique_ptr<const std::vector<const url_pattern_index::flat::CssRule*>>
  FindMatchingCssRule(const GURL& document_url,
                      bool disable_generic_rules = false) const;

  bool HasGenericHideTypeOption(
      const GURL& document_url,
      const url::Origin& parent_document_origin) const;

  bool HasElemHideTypeOption(const GURL& document_url,
                             const url::Origin& parent_document_origin) const;

  bool HasDocumentTypeOption(const GURL& document_url,
                             const url::Origin& parent_document_origin) const;

  const std::vector<const url_pattern_index::flat::CssRule*>
  FindMatchingCssRule(const GURL& subresource_url);

  void DidMatchCssRule(const GURL& document_url,
                       const std::string& dom_path,
                       bool is_for_report = false);

  void SetDidFinishLoad(bool did_load_finished) {
    did_load_finished_ = did_load_finished;
  }

  bool GetDidFinishLoad() { return did_load_finished_; }

  std::unique_ptr<std::vector<std::string>> GetUserDomPathSelectors(
      const GURL& document_url,
      bool disable_generic_rules) const;
#endif

 private:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  bool did_load_finished_ = false;
#endif
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CORE_COMMON_DOCUMENT_SUBRESOURCE_FILTER_EXT_H_
