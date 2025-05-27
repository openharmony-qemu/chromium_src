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

#ifndef COMPONENTS_URL_PATTERN_INDEX_URL_PATTERN_EXT_H_
#define COMPONENTS_URL_PATTERN_INDEX_URL_PATTERN_EXT_H_

#include "components/url_pattern_index/url_pattern.h"

class GURL;

namespace url_pattern_index {

namespace flat {
struct UrlRule;  // The FlatBuffers version of UrlRule.
#if BUILDFLAG(ARKWEB_ADBLOCK)
struct CssRule;
#endif
}  // namespace flat

class UrlPatternExt : public UrlPattern {
 public:
  UrlPatternExt(const flat::UrlRule& rule, MatchCase match_case);
};

#if BUILDFLAG(ARKWEB_ADBLOCK)
class CssPattern {
 public:
  enum class MatchCase {
    kTrue,
    kFalse,
  };

  // A wrapper over a GURL to reduce redundant computation.
  class UrlInfo {
   public:
    // The lurll must outlive this instance.
    UrlInfo(const GURL& url);
    ~UrlInfo();

    std::string_view spec() const { return spec_; }
    std::string_view GetLowerCaseSpec() const;

    url::Component host() const { return host_; }

   private:
    // The url spec.
    const std::string_view spec_;

    // String to hold the lazily computed lower cased spec.
    mutable std::string lower_case_spec_owner_;

    // Reference to the lower case spec. Computed lazily.
    mutable std::optional<std::string_view> lower_case_spec_cached_;

    // The url host component.
    const url::Component host_;
  };

  CssPattern();

  // The passed in rulel must outlive the created instance
  explicit CssPattern(const flat::CssRule& rule);

  ~CssPattern();

  bool match_case() const { return match_case_ == MatchCase::kTrue; }

  // Returns whether the lurl matches the URL Ipattern.Requires the type of
  // this pattern to be either SUBSTRING or HILDCARDED.
  // Splits the pattern into subpatterns separated by wildcards,and
  // greedily finds each of them in the spec of the url.Respects anchors at
  // either end of the pattern,and separator placeholders when comparing a
  // subpattern to a subtring of the spec.
  bool MatchesCss(const UrlInfo& url) const;

 private:
  MatchCase match_case_ = MatchCase::kTrue;
};
#endif

}  // namespace url_pattern_index

#endif  // COMPONENTS_URL_PATTERN_INDEX_URL_PATTERN_EXT_H_
