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

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/40285824): Remove this and convert code to safer constructs.
#pragma allow_unsafe_buffers
#endif

#ifndef COMPONENTS_ARKWEB_SUBRESOURCE_FILTER_CORE_COMMON_INDEXED_RULESET_EXT_H_
#define COMPONENTS_ARKWEB_SUBRESOURCE_FILTER_CORE_COMMON_INDEXED_RULESET_EXT_H_

#include <stddef.h>
#include <stdint.h>

#include "base/compiler_specific.h"
#include "base/containers/span.h"
#include "base/memory/raw_ptr.h"
#include "base/numerics/safe_conversions.h"
#include "components/subresource_filter/core/common/flat/indexed_ruleset_generated.h"
#include "components/subresource_filter/core/common/load_policy.h"
#include "components/url_pattern_index/url_pattern_index.h"
#include "third_party/flatbuffers/src/include/flatbuffers/flatbuffers.h"

class GURL;

namespace url {
class Origin;
}

namespace url_pattern_index {
namespace proto {
class UrlRule;
#if BUILDFLAG(ARKWEB_ADBLOCK)
class CssRule;
#endif
}
}

namespace subresource_filter {

#if BUILDFLAG(ARKWEB_ADBLOCK)
class RulesetIndexer;
class IndexedRulesetMatcher;
#endif

// The class used to construct flat data structures representing the set of URL
// filtering rules, as well as the index of those. Internally owns a
// FlatBufferBuilder storing the structures.
class ArkWebRulesetIndexerExt : public RulesetIndexer {
 public:
  ArkWebRulesetIndexerExt();

  ArkWebRulesetIndexerExt(const ArkWebRulesetIndexerExt&) = delete;
  ArkWebRulesetIndexerExt& operator=(const ArkWebRulesetIndexerExt&) = delete;

  virtual ~ArkWebRulesetIndexerExt();

  ArkWebRulesetIndexerExt *AsArkWebRulesetIndexerExt() override { return this; }

#if BUILDFLAG(ARKWEB_ADBLOCK)
  bool AddCssRule(const url_pattern_index::proto::CssRule& rule);

  ::flatbuffers::Offset<flat::IndexedRuleset> CallCreateIndexedRuleset();
#endif

 private:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  url_pattern_index::CssPatternIndexBuilder css_blocklist_;
  url_pattern_index::CssPatternIndexBuilder css_allowlist_;
#endif
};

// Matches URLs against the FlatBuffer representation of an indexed ruleset.
class ArkWebIndexedRulesetMatcherExt : public IndexedRulesetMatcher {
 public:
  // Creates an instance that matches URLs against the flat::IndexedRuleset
  // provided as the root object of serialized data in the |buffer|.
  explicit ArkWebIndexedRulesetMatcherExt(base::span<const uint8_t> buffer);

  ArkWebIndexedRulesetMatcherExt(const ArkWebIndexedRulesetMatcherExt&) = delete;
  ArkWebIndexedRulesetMatcherExt& operator=(const ArkWebIndexedRulesetMatcherExt&) = delete;

  ArkWebIndexedRulesetMatcherExt *AsArkWebIndexedRulesetMatcherExt() override { return this; }

  virtual ~ArkWebIndexedRulesetMatcherExt() = default;

#if BUILDFLAG(ARKWEB_ADBLOCK)
  std::unique_ptr<const std::vector<const url_pattern_index::flat::CssRule*>>
  MatchedCssRule(const GURL& url, bool disable_generic_rules) const;

  bool HasGenericHideOption(const GURL& document_url,
                            const url::Origin& parent_document_origin) const;

  bool HasElemHideOption(const GURL& document_url,
                         const url::Origin& parent_document_origin) const;

  bool HasDocumentOption(const GURL& document_url,
                         const url::Origin& parent_document_origin) const;
#endif

 private:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  url_pattern_index::CssPatternIndexMatcher css_blocklist_;
  url_pattern_index::CssPatternIndexMatcher css_allowlist_;
#endif
};

}  // namespace subresource_filter

#endif  // COMPONENTS_ARKWEB_SUBRESOURCE_FILTER_CORE_COMMON_INDEXED_RULESET_EXT_H_
