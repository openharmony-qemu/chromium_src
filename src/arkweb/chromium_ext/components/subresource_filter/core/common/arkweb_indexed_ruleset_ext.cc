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

#include "components/subresource_filter/core/common/indexed_ruleset.h"

#include "base/check.h"
#include "base/hash/hash.h"
#include "base/metrics/histogram_functions.h"
#include "base/not_fatal_until.h"
#include "base/strings/strcat.h"
#include "base/trace_event/trace_event.h"
#include "components/subresource_filter/core/common/first_party_origin.h"
#include "url/gurl.h"
#include "url/origin.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/logging.h"
#include "arkweb/chromium_ext/components/url_pattern_index/url_pattern_index_ext.h"
#endif

namespace subresource_filter {

namespace {
namespace proto = url_pattern_index::proto;
using FindRuleStrategy =
    url_pattern_index::UrlPatternIndexMatcher::FindRuleStrategy;
using EmbedderConditionsMatcher =
    url_pattern_index::UrlPatternIndexMatcher::EmbedderConditionsMatcher;
}

// RulesetIndexer --------------------------------------------------------------

#if BUILDFLAG(ARKWEB_ADBLOCK)
// This static assert is meant to catch cases where
// url_pattern_index:：kCssPatternIndexFormatVersion is incremented without
// updating RulesetIndexer::kIndexedFormatVersion.
static_assert(url_pattern_index::kCssPatternIndexFormatVersion == 1,
              "kCssPatternIndexFormatVersion has changed, make sure you've "
              "also updated RulesetIndexer::kIndexedFormatVersion above.");
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
ArkWebRulesetIndexerExt::ArkWebRulesetIndexerExt()
    : RulesetIndexer(),
      css_blocklist_(&builder_),
      css_allowlist_(&builder_) {}
#endif

ArkWebRulesetIndexerExt::~ArkWebRulesetIndexerExt() = default;

#if BUILDFLAG(ARKWEB_ADBLOCK)
bool ArkWebRulesetIndexerExt::AddCssRule(const proto::CssRule& rule) {
  auto offset =
      url_pattern_index::SerializeCssRule(rule, &builder_, &domain_map_);
  // Note:A zero offset.o means a "nullptr"offset. It is returned when the
  // rule has not been serialized.
  if (!offset.o) {
    LOG(ERROR) << "[AdBlock] Fail to serialize css rule:"
               << rule.css_selector();
    return false;
  }

  if (rule.semantics() == proto::RULE_SEMANTICS_BLOCKLIST) {
    css_blocklist_.IndexCssRule(offset);
  } else {
    css_allowlist_.IndexCssRule(offset);
  }

  return true;
}
#endif

::flatbuffers::Offset<flat::IndexedRuleset> ArkWebRulesetIndexerExt::CallCreateIndexedRuleset() {
  auto blocklist_offset = blocklist_.Finish();
  auto allowlist_offset = allowlist_.Finish();
  auto deactivation_offset = deactivation_.Finish();

#if BUILDFLAG(ARKWEB_ADBLOCK)
  auto css_blocklist_offset = css_blocklist_.Finish();
  auto css_allowlist_offset = css_allowlist_.Finish();
  LOG(INFO) << "[AdBlock] url whitelist fallback_rule size:"
            << allowlist_.GetFallbackRuleListSize();
  LOG(INFO) << "[AdBlock] url whitelist hash table size:"
            << allowlist_.GetNGramHashTableSize();
  LOG(INFO) << "[AdBlock] url blacklist fallback rule.size:"
            << blocklist_.GetFallbackRuleListSize();
  LOG(INFO) << "[AdBlock] url blacklist hash table size:"
            << blocklist_.GetNGramHashTableSize();
  LOG(INFO) << "[AdBlock] deactivation fallback rule size:"
            << deactivation_.GetFallbackRuleListSize();
  LOG(INFO) << "[AdBlock] deactivation hash table size:"
            << deactivation_.GetNGramHashTableSize();
  LOG(INFO) << "[AdBlock] css whitelist fallback rule size:"
            << css_allowlist_.GetFallbackRuleListSize();
  LOG(INFO) << "[AdBlock] css whitelist hash table size:"
            << css_allowlist_.GetNGramHashTableSize();
  LOG(INFO) << "[AdBlock] css whitelist no domain rule size:"
            << css_allowlist_.GetNoDomainRuleListSize();
  LOG(INFO) << "[AdBlock] css_blacklist fallback rule size:"
            << css_blocklist_.GetFallbackRuleListSize();
  LOG(INFO) << "[AdBlock] css_blacklist hash table size:"
            << css_blocklist_.GetNGramHashTableSize();
  LOG(INFO) << "[AdBlock] css_blacklist no domain rule size:"
            << css_blocklist_.GetNoDomainRuleListSize();
  LOG(INFO) << "[AdBlock] css_blacklist hash table actual used:"
            << css_blocklist_.GetNGramHashTableUsedSize();
  // LOG(INFO) << "[AdBlock] easylist version_in indexed ruleset = "
  //           << easylist_version_;
  auto url_rules_index_offset = flat::CreateIndexedRuleset(
      builder_, blocklist_offset, allowlist_offset, deactivation_offset,
      css_blocklist_offset, css_allowlist_offset);
  return url_rules_index_offset;
#endif
}

// IndexedRulesetMatcher -------------------------------------------------------

#if BUILDFLAG(ARKWEB_ADBLOCK)
ArkWebIndexedRulesetMatcherExt::ArkWebIndexedRulesetMatcherExt(base::span<const uint8_t> buffer)
    : IndexedRulesetMatcher(buffer),
      css_blocklist_(root_->css_blocklist_index()),
      css_allowlist_(root_->css_allowlist_index()) {}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
std::unique_ptr<const std::vector<const url_pattern_index::flat::CssRule*>>
ArkWebIndexedRulesetMatcherExt::MatchedCssRule(const GURL& document_url,
                                      bool disable_generic_rules) const {
  std::vector<const url_pattern_index::flat::CssRule*> blocklist_rules =
      css_blocklist_.FindMatchInSpecialRules(document_url);

  std::vector<const url_pattern_index::flat::CssRule*> allowlist_rules =
      css_allowlist_.FindMatchInSpecialRules(document_url);

  std::unique_ptr<std::vector<const url_pattern_index::flat::CssRule*>>
      match_result = std::make_unique<
          std::vector<const url_pattern_index::flat::CssRule*>>();

  for (auto* blocklist_rule : blocklist_rules) {
    bool is_hit_whitelist = false;
    for (auto* allowlist_rule : allowlist_rules) {
      if (!blocklist_rule || !allowlist_rule) {
        continue;
      }

      if (!(blocklist_rule->css_selector()) ||
          !(allowlist_rule->css_selector())) {
        continue;
      }

      if (blocklist_rule->css_selector()->str() ==
          allowlist_rule->css_selector()->str()) {
        is_hit_whitelist = true;
        break;
      }
    }

    if (!is_hit_whitelist) {
      match_result->push_back(blocklist_rule);
    }
  }

  if (!disable_generic_rules) {
    const url_pattern_index::FlatCssRuleList* generic_blocklist_rules =
        css_blocklist_.GetGenericCssRules();

    if (generic_blocklist_rules) {
      for (auto* blocklist_rule : *generic_blocklist_rules) {
        bool is_hit_whitelist = false;
        for (auto* allowlist_rule : allowlist_rules) {
          if (!blocklist_rule || !allowlist_rule) {
            continue;
          }

          if (!(blocklist_rule->css_selector()) ||
              !(allowlist_rule->css_selector())) {
            continue;
          }

          if (blocklist_rule->css_selector()->str() ==
              allowlist_rule->css_selector()->str()) {
            is_hit_whitelist = true;
            break;
          }
        }
        if (!is_hit_whitelist) {
          match_result->push_back(blocklist_rule);
        }
      }
    }
  }

  return match_result;
}

bool ArkWebIndexedRulesetMatcherExt::HasGenericHideOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return !!deactivation_.FindMatch(
      document_url, parent_document_origin, proto::ELEMENT_TYPE_UNSPECIFIED,
      url_pattern_index::proto::ACTIVATION_TYPE_GENERICHIDE,
      FirstPartyOrigin::IsThirdParty(document_url, parent_document_origin),
      false, EmbedderConditionsMatcher(), FindRuleStrategy::kAny,
      {} /* disabled_rule_ids */);
}

bool ArkWebIndexedRulesetMatcherExt::HasElemHideOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return !!deactivation_.FindMatch(
      document_url, parent_document_origin, proto::ELEMENT_TYPE_UNSPECIFIED,
      url_pattern_index::proto::ACTIVATION_TYPE_ELEMHIDE,
      FirstPartyOrigin::IsThirdParty(document_url, parent_document_origin),
      false, EmbedderConditionsMatcher(), FindRuleStrategy::kAny,
      {} /* disabled rule ids */);
}

bool ArkWebIndexedRulesetMatcherExt::HasDocumentOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return !!deactivation_.FindMatch(
      document_url, parent_document_origin, proto::ELEMENT_TYPE_UNSPECIFIED,
      url_pattern_index::proto::ACTIVATION_TYPE_DOCUMENT,
      FirstPartyOrigin::IsThirdParty(document_url, parent_document_origin),
      false, EmbedderConditionsMatcher(), FindRuleStrategy::kAny,
      {} /* disabled_rule_ids */);
}
#endif

}  // namespace subresource_filter
