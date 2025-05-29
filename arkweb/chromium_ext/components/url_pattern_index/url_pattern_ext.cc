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

#include "arkweb/chromium_ext/components/url_pattern_index/url_pattern_ext.h"

#include "base/check_op.h"
#include "base/notreached.h"
#include "base/numerics/checked_math.h"
#include "base/ranges/algorithm.h"
#include "base/strings/string_util.h"
#include "components/url_pattern_index/flat/url_pattern_index_generated.h"
#include "components/url_pattern_index/fuzzy_pattern_matching.h"
#include "components/url_pattern_index/string_splitter.h"
#include "url/gurl.h"
#include "url/third_party/mozilla/url_parse.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "components/url_pattern_index/flat/css_pattern_index_generated.h"
#endif

namespace url_pattern_index {

namespace {

proto::UrlPatternType ConvertUrlPatternType(flat::UrlPatternType type) {
  switch (type) {
    case flat::UrlPatternType_SUBSTRING:
      return proto::URL_PATTERN_TYPE_SUBSTRING;
    case flat::UrlPatternType_WILDCARDED:
      return proto::URL_PATTERN_TYPE_WILDCARDED;
    case flat::UrlPatternType_REGEXP:
      return proto::URL_PATTERN_TYPE_REGEXP;
    default:
      return proto::URL_PATTERN_TYPE_UNSPECIFIED;
  }
}

proto::AnchorType ConvertAnchorType(flat::AnchorType type) {
  switch (type) {
    case flat::AnchorType_NONE:
      return proto::ANCHOR_TYPE_NONE;
    case flat::AnchorType_BOUNDARY:
      return proto::ANCHOR_TYPE_BOUNDARY;
    case flat::AnchorType_SUBDOMAIN:
      return proto::ANCHOR_TYPE_SUBDOMAIN;
    default:
      return proto::ANCHOR_TYPE_UNSPECIFIED;
  }
}

std::string_view ConvertString(const flatbuffers::String* string) {
  return string ? std::string_view(string->data(), string->size())
                : std::string_view();
}

bool HasAnyUpperAscii(std::string_view string) {
  return base::ranges::any_of(string, base::IsAsciiUpper<char>);
}

}  // namespace

UrlPatternExt::UrlPatternExt(const flat::UrlRule& rule, MatchCase match_case)
    : UrlPattern(rule) {
  match_case_ = match_case;
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
CssPattern::UrlInfo::UrlInfo(const GURL& url)
    : spec_(url.possibly_invalid_spec()),
      host_(url.parsed_for_possibly_invalid_spec().host) {
  DCHECK(url.is_valid());
}

std::string_view CssPattern::UrlInfo::GetLowerCaseSpec() const {
  if (lower_case_spec_cached_) {
    return *lower_case_spec_cached_;
  }

  if (!HasAnyUpperAscii(spec_)) {
    lower_case_spec_cached_ = spec_;
  } else {
    lower_case_spec_owner_ = base::ToLowerASCII(spec_);
    lower_case_spec_cached_ = lower_case_spec_owner_;
  }

  return *lower_case_spec_cached_;
}

CssPattern::UrlInfo::~UrlInfo() = default;

CssPattern::CssPattern() = default;

CssPattern::CssPattern(const flat::CssRule& rule)
    : match_case_(rule.options() & flat::OptionFlag_IS_CASE_INSENSITIVE
                      ? MatchCase::kFalse
                      : MatchCase::kTrue) {}

CssPattern::~CssPattern() = default;

bool CssPattern::MatchesCss(const UrlInfo& url) const {
  return true;
}
#endif

}  // namespace url_pattern_index
