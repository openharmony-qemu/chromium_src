// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/url_pattern_index/url_pattern_index.h"

#include "arkweb/chromium_ext/components/url_pattern_index/url_pattern_index_ext.h"

#include <limits>
#include <string>
#include <string_view>
#include <utility>

#include "base/check_op.h"
#include "base/containers/contains.h"
#include "base/containers/flat_map.h"
#include "base/functional/callback.h"
#include "base/memory/raw_ref.h"
#include "base/no_destructor.h"
#include "base/not_fatal_until.h"
#include "base/notreached.h"
#include "base/numerics/safe_conversions.h"
#include "base/ranges/algorithm.h"
#include "base/strings/string_util.h"
#include "base/trace_event/trace_event.h"
#include "components/url_pattern_index/ngram_extractor.h"
#include "components/url_pattern_index/url_pattern.h"
#include "components/url_pattern_index/url_rule_util.h"
#include "url/gurl.h"
#include "url/origin.h"
#include "url/url_constants.h"
#include "url/url_util.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/logging.h"
#endif

namespace url_pattern_index {

#if BUILDFLAG(ARKWEB_ADBLOCK)
using FlatCssRuleList = flatbuffers::Vector<flatbuffers::Offset<flat::CssRule>>;
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
bool CssRuleDescendingPriorityComparator(const flat::CssRule* lhs,
                                         const flat::CssRule* rhs) {
  DCHECK(lhs);
  DCHECK(rhs);

  return lhs->priority() > rhs->priority();
}
#endif

ArkWebUrlPatternIndexBuilderExt::ArkWebUrlPatternIndexBuilderExt(flatbuffers::FlatBufferBuilder* flat_builder)
    : UrlPatternIndexBuilder(flat_builder) {}

ArkWebUrlPatternIndexBuilderExt::~ArkWebUrlPatternIndexBuilderExt() = default;

#if BUILDFLAG(ARKWEB_ADBLOCK)
class CssRuleFlatBufferConverter {
 public:
  explicit CssRuleFlatBufferConverter(const proto::CssRule& rule)
      : rule_(rule) {}

  CssRuleOffset SerializeConvertedRule(flatbuffers::FlatBufferBuilder* builder,
                                       FlatDomainMap* domain_map) const {
    if (!is_convertible_) {
      return CssRuleOffset();
    }

    FlatDomainsOffset domains_included_offset;
    FlatDomainsOffset domains_excluded_offset;

    if (rule_->domains_size()) {
      std::vector<FlatStringOffset> domains_included;
      std::vector<FlatStringOffset> domains_excluded;
      // Reserve only for ldomains_includedI because it is expected to be the
      // one used more frequently.
      domains_included.reserve(rule_->domains_size());

      for (const auto& domain_list_item : rule_->domains()) {
        const std::string& domain = domain_list_item.domain();

        // Non-ascii characters in domains are unsupported.
        if (!base::IsStringASCII(domain)) {
          return CssRuleOffset();
        }

        // Note: This is not always correct. Chrome's URL parser uses upper-case
        // for percent encoded hosts. E.g. https://,. com is encoded as
        // https://%2C.com.

        auto offset = builder->CreateSharedString(
            HasNoUpperAscii(domain) ? domain : base::ToLowerASCII(domain));

        if (domain_list_item.exclude()) {
          domains_excluded.push_back(offset);
        } else {
          domains_included.push_back(offset);
        }
      }

      // The domains are stored in sorted order to support fast matching.
      domains_included_offset =
          SerializeDomainList(std::move(domains_included), builder, domain_map);
      domains_excluded_offset =
          SerializeDomainList(std::move(domains_excluded), builder, domain_map);
    }

    // Characters in selector maybe Non-ascii,remove the ascii check.
    // if (!base:IsStringASCII(rule->css_selector()))
    // return CssRuleoffset();

    // Css selector is Case-Sensitive,can not transform to lower.
    auto css_selector_offset =
        builder->CreateSharedString(rule_->css_selector());

    return flat::CreateCssRule(*builder, options_, domains_included_offset,
                               domains_excluded_offset, css_selector_offset);
  }

 private:
  FlatDomainsOffset SerializeDomainList(std::vector<FlatStringOffset> domains,
                                        flatbuffers::FlatBufferBuilder* builder,
                                        FlatDomainMap* domain_map) const {
    // The comparator ensuring the domains order necessary for fast matching.
    auto precedes = [&builder](FlatStringOffset lhs, FlatStringOffset rhs) {
      return CompareDomains(
                 ToStringView(flatbuffers::GetTemporaryPointer(*builder, lhs)),
                 ToStringView(
                     flatbuffers::GetTemporaryPointer(*builder, rhs))) < 0;
    };

    if (domains.empty()) {
      return FlatDomainsOffset();
    }

    std::sort(domains.begin(), domains.end(), precedes);

    // Share domain lists if we've already serialized an exact duplicate. Note
    // that this can share excluded and included domain lists.
    DCHECK(domain_map);
    auto it = domain_map->find(domains);
    if (it == domain_map->end()) {
      auto offset = builder->CreateVector(domains);

      (*domain_map)[domains] = offset;
      return offset;
    }
    return it->second;
  }

  uint8_t options_ = 0;
  const raw_ref<const proto::CssRule> rule_;
  bool is_convertible_ = true;
};
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
CssRuleOffset SerializeCssRule(const proto::CssRule& rule,
                               flatbuffers::FlatBufferBuilder* builder,
                               FlatDomainMap* domain_map) {
  DCHECK(builder);
  CssRuleFlatBufferConverter converter(rule);
  return converter.SerializeConvertedRule(builder, domain_map);
}
#endif

// UrlPatternIndex -------------------------------------------------------------
#if BUILDFLAG(ARKWEB_ADBLOCK)
CssPatternIndexBuilder::CssPatternIndexBuilder(
    flatbuffers::FlatBufferBuilder* flat_builder)
    : flat_builder_(flat_builder) {
  DCHECK(flat_builder_);
}

CssPatternIndexBuilder::~CssPatternIndexBuilder() = default;

void CssPatternIndexBuilder::IndexCssRule(CssRuleOffset offset) {
  DCHECK(offset.o);

  const auto* rule = flatbuffers::GetTemporaryPointer(*flat_builder_, offset);
  DCHECK(rule);

#if DCHECK_IS_ON()
  // Sanity check that the rule does not have fields with non-ascii characters.
  if (rule->domains_included()) {
    for (auto* domain : *rule->domains_included()) {
      DCHECK(base::IsStringASCII(ToStringView(domain)));
    }
  }

  if (rule->domains_excluded()) {
    for (auto* domain : *rule->domains_excluded()) {
      DCHECK(base::IsStringASCII(ToStringView(domain)));
    }
  }

  // Case-insensitive patterns should be lower-cased.
#endif

  // Each domain of a CssRule should be calculated ngram and inserted into the
  // hash table, so as to ensure that we can query the corresponding selector
  // through the URL of the frame.Most CssRule has only one domain name
  // restriction,so this approach will not make the index-file excessively
  // bloated.
  bool should_fallback = false;
  if (rule->domains_included()) {
    for (auto* domain : *rule->domains_included()) {
      NGram ngram = 0;
      ngram = GetMostDistinctiveNGram(ToStringView(domain));
      if (ngram) {
        ngram_index_[ngram].push_back(offset);
        ngram_table_rule_size_++;
      } else {
        // Any ngram failed should be saved in fallback to ensure that
        // using page url can find matches from indexed's fallback_rules vector
        should_fallback = true;
      }
    }
  }

  if (rule->domains_excluded()) {
    for (auto* domain : *rule->domains_excluded()) {
      NGram ngram = 0;
      ngram = GetMostDistinctiveNGram(ToStringView(domain));
      if (ngram) {
        ngram_index_[ngram].push_back(offset);
        ngram_table_rule_size_++;
      } else {
        // Any ngram failed should be saved in fallback to ensure that
        // using page url can find matches from indexed's fallback rules vector
        should_fallback = true;
      }
    }
  }

  bool is_no_domain_css_rule =
      !rule->domains_included() && !rule->domains_excluded();
  if (is_no_domain_css_rule) {
    // only css blacklist will have no _domain_rules
    no_domain_rules_.push_back(offset);
  } else if (should_fallback) {
    fallback_rules_.push_back(offset);
  }
}

CssPatternIndexOffset CssPatternIndexBuilder::Finish() {
  std::vector<flatbuffers::Offset<flat::NGramToCssRules>> flat_hash_table(
      ngram_index_.table_size());

  flatbuffers::Offset<flat::NGramToCssRules> empty_slot_offset =
      flat::CreateNGramToCssRules(*flat_builder_);
  auto rules_comparator = [this](const CssRuleOffset& lhs,
                                 const CssRuleOffset& rhs) {
    return CssRuleDescendingPriorityComparator(
        flatbuffers::GetTemporaryPointer(*flat_builder_, lhs),
        flatbuffers::GetTemporaryPointer(*flat_builder_, rhs));
  };

  for (size_t i = 0, size = ngram_index_.table_size(); i != size; ++i) {
    const uint32_t entry_index = ngram_index_.hash_table()[i];
    if (entry_index >= ngram_index_.size()) {
      flat_hash_table[i] = empty_slot_offset;
      continue;
    }
    const MutableNGr_amIndex::EntryType& entry =
        ngram_index_.entries()[entry_index];
    // Retrieve a mutable reference to lentry.second and sort it in descending
    // order of priority.
    MutableCssRuleList& rule_list = ngram_index_[entry.first];
    std::sort(rule_list.begin(), rule_list.end(), rules_comparator);

    auto rules_offset = flat_builder_->CreateVector(rule_list);
    flat_hash_table[i] =
        flat::CreateNGramToCssRules(*flat_builder_, entry.first, rules_offset);
  }

  auto ngram_index_offset = flat_builder_->CreateVector(flat_hash_table);

  // Sort |fallback_rules_| in descending order of priority.
  std::sort(fallback_rules_.begin(), fallback_rules_.end(), rules_comparator);
  auto fallback_rules_offset = flat_builder_->CreateVector(fallback_rules_);
  auto no_domain_rules_offset = flat_builder_->CreateVector(no_domain_rules_);
  return flat::CreateCssPatternIndex(
      *flat_builder_, kNGramSize, ngram_index_offset, empty_slot_offset,
      fallback_rules_offset, no_domain_rules_offset);
}

NGram CssPatternIndexBuilder::GetMostDistinctiveNGram(
    std::string_view pattern) {
  size_t min_list_size = std::numeric_limits<size_t>::max();
  NGram best_ngram = 0;

  // To support case-insensitive matching, make sure the n-grams for |pattern|
  // are lower-cased.
  DCHECK(base::IsStringASCII(pattern));
  auto ngrams =
      CreateNGramExtractor<kNGramSize, NGram, NGramCaseExtraction::kLowerCase>(
          pattern, [](char c) { return c == '*' || c == '^'; });

  for (uint64_t ngram : ngrams) {
    const MutableCssRuleList* rules = ngram_index_.Get(ngram);
    const size_t list_size = rules ? rules->size() : 0;
    if (list_size < min_list_size) {
      // TODO(pkalinnikov): Pick random of the same-sized lists.
      min_list_size = list_size;
      best_ngram = ngram;
      if (list_size == 0) {
        break;
      }
    }
  }

  return best_ngram;
}
#endif

namespace {

#if BUILDFLAG(ARKWEB_ADBLOCK)
using FlatCssNGramIndex =
    flatbuffers::Vector<flatbuffers::Offset<flat::NGramToCssRules>>;
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
const std::vector<const flat::CssRule*> FindMatchAmongCandidates(
    const FlatCssRuleList* sorted_candidates,
    const url::Origin& document_origin,
    std::set<const flat::CssRule*>* unique_rules) {
  if (!sorted_candidates) {
    return std::vector<const flat::CssRule*>();
  }

  std::vector<const flat::CssRule*> rules;
  for (const flat::CssRule* rule : *sorted_candidates) {
    DCHECK_NE(rule, nullptr);
    if (DoesOriginMatchDomainList(document_origin, *rule)) {
      if (unique_rules && (unique_rules->find(rule) != unique_rules->end())) {
        continue;
      }

      rules.push_back(rule);

      if (unique_rules) {
        unique_rules->insert(rule);
      }
    }
  }

  return rules;
}

const std::vector<const flat::CssRule*> FindMatchInFlatCssPatternIndex(
    const flat::CssPatternIndex& index,
    const CssPattern::UrlInfo& url) {
  const FlatCssNGramIndex* hash_table = index.ngram_index();
  const flat::NGramToCssRules* empty_slot = index.ngram_index_empty_slot();
  DCHECK_NE(hash_table, nullptr);

  url::Origin origin = url::Origin::Create(GURL(url.spec()));

  NGramHashTableProber prober;
  // hash_table contains lower-cased domain's n-grams.
  // Use lower-cased extraction to find prospective matches.
  auto ngrams = CreateNGramExtractor<kNGramSize, uint64_t,
                                     NGramCaseExtraction::kLowerCase>(
      origin.host(), [](char) { return false; });

  std::set<const flat::CssRule*> unique_rules;
  std::vector<const flat::CssRule*> rules;

  for (uint64_t ngram : ngrams) {
    const size_t slot_index = prober.FindSlot(
        ngram, base::strict_cast<size_t>(hash_table->size()),
        [hash_table, empty_slot](NGram ngram, size_t slot_index) {
          const flat::NGramToCssRules* entry = hash_table->Get(slot_index);
          DCHECK_NE(entry, nullptr);
          return entry == empty_slot || entry->ngram() == ngram;
        });

    DCHECK_LT(slot_index, hash_table->size());

    const flat::NGramToCssRules* entry = hash_table->Get(slot_index);

    if (entry == empty_slot) {
      continue;
    }

    std::vector<const flat::CssRule*> matched_rules =
        FindMatchAmongCandidates(entry->rule_list(), origin, &unique_rules);

    if (matched_rules.empty()) {
      continue;
    }

    rules.insert(rules.end(), matched_rules.begin(), matched_rules.end());
  }

  std::vector<const flat::CssRule*> matched_rules =
      FindMatchAmongCandidates(index.fallback_rules(), origin, nullptr);

  if (!matched_rules.empty()) {
    rules.insert(rules.end(), matched_rules.begin(), matched_rules.end());
  }

  return rules;
}
#endif

}  // namespace


#if BUILDFLAG(ARKWEB_ADBLOCK)
bool DoesOriginMatchDomainList(const url::Origin& origin,
                               const flat::CssRule& rule) {
  const bool is_generic = !rule.domains_included();
  DCHECK(is_generic || rule.domains_included()->size());

  if (is_generic) {
    return false;
  }

  // Unique loriginl matches lists of exception domains only.
  if (origin.opaque()) {
    return is_generic;
  }

  size_t longest_matching_included_domain_length = 1;
  if (!is_generic) {
    longest_matching_included_domain_length =
        GetLongestMatchingSubdomain(origin.host(), *rule.domains_included());
  }

  if (longest_matching_included_domain_length && rule.domains_excluded()) {
    return GetLongestMatchingSubdomain(origin.host(),
                                       *rule.domains_excluded()) <
           longest_matching_included_domain_length;
  }

  return !!longest_matching_included_domain_length;
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
CssPatternIndexMatcher::CssPatternIndexMatcher(
    const flat::CssPatternIndex* flat_index)
    : flat_index_(flat_index) {
  DCHECK(!flat_index || flat_index->n() == kNGramSize);
  if (!flat_index) {
    LOG(INFO) << "[AdBlock] CssPatternIndexMatcher::ctor, flat_index is null";
  }
}

CssPatternIndexMatcher::~CssPatternIndexMatcher() = default;

CssPatternIndexMatcher::CssPatternIndexMatcher(CssPatternIndexMatcher&&) =
    default;

CssPatternIndexMatcher& CssPatternIndexMatcher::operator=(
    CssPatternIndexMatcher&&) = default;

const std::vector<const flat::CssRule*>
CssPatternIndexMatcher::FindMatchInSpecialRules(const GURL& url) const {
  if (!flat_index_) {
    LOG(INFO) << "[AdBlock] FindMatchInSpecialRules, flat_index is null";
    return std::vector<const flat::CssRule*>();
  }

  const std::vector<const flat::CssRule*> rules =
      FindMatchInFlatCssPatternIndex(*flat_index_, CssPattern::UrlInfo(url));
  return rules;
}

const FlatCssRuleList* CssPatternIndexMatcher::GetGenericCssRules() const {
  if (!flat_index_) {
    LOG(INFO) << "[AdBlock] GetGenericCssRules, flat_index is null";
    return nullptr;
  }
  return flat_index_->no_domain_rules();
}
#endif


}  // namespace url_pattern_index
