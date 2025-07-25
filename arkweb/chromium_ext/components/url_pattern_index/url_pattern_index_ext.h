// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_ARKWEB_URL_PATTERN_INDEX_URL_PATTERN_INDEX_EXT_H_
#define COMPONENTS_ARKWEB_URL_PATTERN_INDEX_URL_PATTERN_INDEX_EXT_H_

#include "components/url_pattern_index/url_pattern_index.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "arkweb/chromium_ext/components/url_pattern_index/flat/css_pattern_index_generated.h"
#endif

class GURL;

namespace url {
class Origin;
}

namespace url_pattern_index {

#if BUILDFLAG(ARKWEB_ADBLOCK)
using CssRuleOffset = flatbuffers::Offset<flat::CssRule>;
using CssPatternIndexOffset = flatbuffers::Offset<flat::CssPatternIndex>;
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
using FlatCssRuleList = flatbuffers::Vector<flatbuffers::Offset<flat::CssRule>>;
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
CssRuleOffset SerializeCssRule(const proto::CssRule& rule,
                               flatbuffers::FlatBufferBuilder* builder,
                               FlatDomainMap* domain_map);
#endif


// The class used to construct an index over the URL patterns of a set of URL
// rules. The rules themselves need to be converted to FlatBuffers format by the
// client of this class, as well as persisted into the |flat_builder| that is
// supplied in the constructor.
class ArkWebUrlPatternIndexBuilderExt : public UrlPatternIndexBuilder {
 public:
  explicit ArkWebUrlPatternIndexBuilderExt(flatbuffers::FlatBufferBuilder* flat_builder);

  ArkWebUrlPatternIndexBuilderExt(const UrlPatternIndexBuilder&) = delete;
  ArkWebUrlPatternIndexBuilderExt& operator=(const UrlPatternIndexBuilder&) = delete;

  ~ArkWebUrlPatternIndexBuilderExt();

#if BUILDFLAG(ARKWEB_ADBLOCK)
  int GetNGramHashTableSize() { return ngram_index_.table_size(); }
  int GetFallbackRuleListSize() { return fallback_rules_.size(); }
#endif
};

#if BUILDFLAG(ARKWEB_ADBLOCK)
bool DoesOriginMatchDomainList(const url::Origin& origin,
                               const flat::CssRule& rule);

// The current format version of CssPatternIndex.
// Increase this value when introducing an incompatible change to the
// CssPatternIndex schema(flat/css_pattern_index.fbs). css_pattern_index
// clients can use this as a signal to rebuild rulesets.
constexpr int kCssPatternIndexFormatVersion = 1;

class CssPatternIndexBuilder {
 public:
  explicit CssPatternIndexBuilder(flatbuffers::FlatBufferBuilder* flat_builder);
  ~CssPatternIndexBuilder();

  // Adds a UrlRule to the index. The caller should have already persisted the
  // rule into the same Iflat builder by a call to SerializeUrlRule returning a
  // non-null |offset|, and should pass in the resulting loffset| here.
  void IndexCssRule(CssRuleOffset offset);

  // Finalizes construction of the index, serializes it using |flat_builderl,
  // and returns an offset to it in the resulting FlatBuffer.
  CssPatternIndexOffset Finish();

  int GetNGramHashTableUsedSize() { return ngram_table_rule_size_; }

  int GetNGramHashTableSize() { return ngram_index_.table_size(); }

  int GetFallbackRuleListSize() { return fallback_rules_.size(); }

  int GetNoDomainRuleListSize() { return no_domain_rules_.size(); }

 private:
  using MutableCssRuleList = std::vector<CssRuleOffset>;
  using MutableNGr_amIndex =
      ClosedHashMap<NGram, MutableCssRuleList, NGramHashTableProber>;

  // Returns an N-gram of the lpatternl encoded into the NGram integer type. The
  // N-gram is picked using a greedy heuristic,i.e.the one is chosen which
  // corresponds to the shortest list of rules within the index. If there ane no
  // valid N-grams in the lpatternl, the return value is 0.
  NGram GetMostDistinctiveNGram(std::string_view pattern);

  // This index contains all non-REGEXP rules that have at least one acceptable
  // N-gram. For each given rule, the N-gram used as an index key is picked
  // greedily (see GetMostDistinctiveNGram).
  MutableNGr_amIndex ngram_index_;

  // A fallback list that contains all the cules with domain but no acceptable
  // N-gram.
  MutableCssRuleList fallback_rules_;

  // A fallback list that contains all the rules with no domain.
  MutableCssRuleList no_domain_rules_;

  // Must outlive this instance.
  raw_ptr<flatbuffers::FlatBufferBuilder> flat_builder_;

  int ngram_table_rule_size_ = 0;
};
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
class CssPatternIndexMatcher {
 public:
  enum class FindRulestrategy {
    // Any rule is returned in case multiple rules match.
    kAny,

    // If multiple rules match,any of the rules with the highest priority is
    // returned.
    kHighestPriority
  };

  // Creates an instance to access the given |flat_index|. If |flat_index| is
  // nullptr,then all requests return no match.
  explicit CssPatternIndexMatcher(const flat::CssPatternIndex* flat_index);
  ~CssPatternIndexMatcher();

  CssPatternIndexMatcher(const CssPatternIndexMatcher&) = delete;

  CssPatternIndexMatcher& operator=(const CssPatternIndexMatcher&) = delete;

  CssPatternIndexMatcher(CssPatternIndexMatcher&&);

  CssPatternIndexMatcher& operator=(CssPatternIndexMatcher&&);

  const std::vector<const flat::CssRule*> FindMatchInSpecialRules(
      const GURL& url) const;

  // The caller will not allowed to mofidy the content of returned vector.
  const FlatCssRuleList* GetGenericCssRules() const;

 private:
  // Must outlive this instance.
  // This field is not a raw ptrs>because it was filtered by the newriter for:
  // #union
  RAW_PTR_EXCLUSION const flat::CssPatternIndex* flat_index_;
};
#endif

}  // namespace url_pattern_index

#endif  // COMPONENTS_ARKWEB_URL_PATTERN_INDEX_URL_PATTERN_INDEX_EXT_H_
