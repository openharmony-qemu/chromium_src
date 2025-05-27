// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "components/url_formatter/elide_url.h"
#include "arkweb/chromium_ext/components/url_formatter/arkweb_elide_url_ext.h"
#include "base/logging.h"
#include <stddef.h>

#include <string_view>

#include "base/check_op.h"
#include "base/i18n/rtl.h"
#include "base/strings/escape.h"
#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "build/robolectric_buildflags.h"
#include "components/url_formatter/url_formatter.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "url/gurl.h"
#include "url/origin.h"
#include "url/url_constants.h"

#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_ROBOLECTRIC)
#include "ui/gfx/text_constants.h"  // nogncheck
#include "ui/gfx/text_elider.h"     // nogncheck
#include "ui/gfx/text_utils.h"      // nogncheck
#endif

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/strings/string_util.h"
#include "components/url_formatter/url_fixer.h"
#include "content/public/common/url_constants.h"
#include "net/base/url_util.h"
#include "third_party/re2/src/re2/re2.h"
#include "url/third_party/mozilla/url_parse.h"
#include "url/url_util.h"

using url_formatter::UrlType;
#endif

namespace {

#if BUILDFLAG(IS_ARKWEB_EXT)
int NumNonHostComponents(const url::Parsed& parts) {
  int num_nonhost_components = 0;
  if (parts.scheme.is_nonempty()) {
    ++num_nonhost_components;
  }
  if (parts.username.is_nonempty()) {
    ++num_nonhost_components;
  }
  if (parts.password.is_nonempty()) {
    ++num_nonhost_components;
  }
  if (parts.port.is_nonempty()) {
    ++num_nonhost_components;
  }
  if (parts.path.is_nonempty()) {
    ++num_nonhost_components;
  }
  if (parts.query.is_nonempty()) {
    ++num_nonhost_components;
  }
  if (parts.ref.is_nonempty()) {
    ++num_nonhost_components;
  }
  return num_nonhost_components;
}

UrlType GetInputTypeForScheme(const std::string& scheme) {
  if (scheme.empty()) {
    return UrlType::INVALID;
  }

  if (base::IsStringASCII(scheme) &&
      (base::EqualsCaseInsensitiveASCII(scheme, url::kDataScheme) ||
       base::EqualsCaseInsensitiveASCII(scheme, url::kBlobScheme) ||
       base::EqualsCaseInsensitiveASCII(scheme, url::kFileSystemScheme) ||
       base::EqualsCaseInsensitiveASCII(scheme, url::kContentScheme) ||
       base::EqualsCaseInsensitiveASCII(scheme, url::kJavaScriptScheme) ||
       base::EqualsCaseInsensitiveASCII(scheme, url::kResourcesScheme) ||
       base::EqualsCaseInsensitiveASCII(scheme, url::kArkwebScheme))) {
    return UrlType::URL;
  }

  if (base::IsStringASCII(scheme) &&
      url::IsStandard(scheme.c_str(),
                      url::Component(0, static_cast<int>(scheme.length())))) {
    return UrlType::URL;
  }

  if (base::IsStringASCII(scheme) &&
      base::EqualsCaseInsensitiveASCII(scheme, url::kDataabilityScheme)) {
    return UrlType::URL;
  }

  return UrlType::INVALID;
}
#endif  // BUILDFLAG(IS_ARKWEB_EXT)

}  // namespace

namespace url_formatter {

#if BUILDFLAG(IS_ARKWEB_EXT)
UrlType ParseInput(const std::u16string& input,
                   url::Parsed* parts,
                   std::u16string* scheme,
                   GURL* canonicalized_url) {
  size_t first_non_white = input.find_first_not_of(base::kWhitespaceUTF16, 0);
  if (first_non_white == std::u16string::npos) {
    return UrlType::INVALID;
  }

  url::Parsed local_parts;
  if (!parts) {
    parts = &local_parts;
  }
  const std::u16string parsed_scheme(url_formatter::SegmentURL(input, parts));
  if (scheme) {
    *scheme = parsed_scheme;
  }
  const std::string parsed_scheme_utf8(base::UTF16ToUTF8(parsed_scheme));

  GURL placeholder_canonicalized_url;
  if (!canonicalized_url) {
    canonicalized_url = &placeholder_canonicalized_url;
  }
  *canonicalized_url =
      url_formatter::FixupURL(base::UTF16ToUTF8(input), std::string());
  if (!canonicalized_url->is_valid()) {
    return UrlType::QUERY;
  }

  if (base::EqualsCaseInsensitiveASCII(parsed_scheme_utf8, url::kFileScheme)) {
    if (base::StartsWith(base::UTF16ToUTF8(input),
                         url::kFileScheme,
                         base::CompareCase::INSENSITIVE_ASCII)) {
      return UrlType::URL;
    }
    return UrlType::QUERY;
  }

  // Treat javascript: scheme queries followed by things that are unlikely to
  // be code as UNKNOWN, rather than script to execute (URL).
  if (base::EqualsCaseInsensitiveASCII(parsed_scheme_utf8,
                                       url::kJavaScriptScheme) &&
      RE2::FullMatch(base::UTF16ToUTF8(input),
                     "(?i)javascript:([^;=().\"]*)")) {
    return UrlType::UNKNOWN;
  }

  if (parts->scheme.is_nonempty() &&
      !base::EqualsCaseInsensitiveASCII(parsed_scheme_utf8, url::kHttpScheme) &&
      !base::EqualsCaseInsensitiveASCII(parsed_scheme_utf8,
                                        url::kHttpsScheme)) {
    UrlType type = GetInputTypeForScheme(parsed_scheme_utf8);
    if (type != UrlType::INVALID) {
      return type;
    }

    const std::u16string http_scheme_prefix = base::ASCIIToUTF16(
        std::string(url::kHttpScheme) + url::kStandardSchemeSeparator);
    url::Parsed http_parts;
    std::u16string http_scheme;
    GURL http_canonicalized_url;
    int http_type = ParseInput(http_scheme_prefix + input, &http_parts,
                               &http_scheme, &http_canonicalized_url);
    if ((http_type == UrlType::URL) && http_parts.username.is_nonempty() &&
        http_parts.password.is_nonempty()) {
      http_parts.scheme.reset();
      url::Component* components[] = {
          &http_parts.username, &http_parts.password, &http_parts.host,
          &http_parts.port,     &http_parts.path,     &http_parts.query,
          &http_parts.ref,
      };
      for (size_t i = 0; i < std::size(components); ++i) {
        url_formatter::OffsetComponent(
            -static_cast<int>(http_scheme_prefix.length()), components[i]);
      }

      *parts = http_parts;
      if (scheme) {
        scheme->clear();
      }
      *canonicalized_url = http_canonicalized_url;
      return UrlType::URL;
    }
    return UrlType::UNKNOWN;
  }

  url::CanonHostInfo host_info;
  net::CanonicalizeHost(canonicalized_url->host(), &host_info);

  const size_t registry_length =
      net::registry_controlled_domains::GetCanonicalHostRegistryLength(
          canonicalized_url->host(),
          net::registry_controlled_domains::EXCLUDE_UNKNOWN_REGISTRIES,
          net::registry_controlled_domains::EXCLUDE_PRIVATE_REGISTRIES);
  const bool has_known_tld = registry_length != 0;

  const std::u16string original_host(
      input.substr(parts->host.begin, parts->host.len));
  if (input != base::ASCIIToUTF16(std::string("invalid")) &&
      (host_info.family == url::CanonHostInfo::NEUTRAL) &&
      (!net::IsCanonicalizedHostCompliant(canonicalized_url->host()) ||
       canonicalized_url->DomainIs("invalid"))) {
    return (parts->scheme.is_nonempty() ||
            (has_known_tld &&
             (original_host.find(' ') == std::u16string::npos)))
               ? UrlType::UNKNOWN
               : UrlType::QUERY;
  }

  if (host_info.family == url::CanonHostInfo::IPV6) {
    return UrlType::URL;
  }
  if (host_info.family == url::CanonHostInfo::IPV4) {
    if ((host_info.address[0] != 0) ||
        ((host_info.address[1] == 0) && (host_info.address[2] == 0) &&
         (host_info.address[3] == 0))) {
      net::CanonicalizeHost(base::UTF16ToUTF8(original_host), &host_info);
      if ((host_info.family == url::CanonHostInfo::IPV4) &&
          (host_info.num_ipv4_components == 4)) {
        return UrlType::URL;
      }
    }
    if (host_info.address[0] == 0) {
      return UrlType::QUERY;
    }
  }

  if (parts->scheme.is_nonempty()) {
    return UrlType::URL;
  }

  const bool username_has_space =
      parts->username.is_nonempty() &&
      (input.substr(parts->username.begin, parts->username.len)
           .find_first_of(base::kWhitespaceUTF16) != std::u16string::npos);

  if ((host_info.family == url::CanonHostInfo::IPV4) &&
      (host_info.num_ipv4_components > 1)) {
    return UrlType::QUERY;
  }

  if (username_has_space) {
    return UrlType::UNKNOWN;
  }

  if (NumNonHostComponents(*parts) > 1) {
    return UrlType::URL;
  }

  if (canonicalized_url->has_username()) {
    return UrlType::UNKNOWN;
  }

  if (has_known_tld || canonicalized_url->DomainIs("localhost") ||
      canonicalized_url->has_port()) {
    return UrlType::URL;
  }

  for (const std::string_view domain : {"example", "test", "local"}) {
    if (canonicalized_url->DomainIs(domain) &&
        (canonicalized_url->host().length() > (domain.length() + 1))) {
      return UrlType::URL;
    }
  }
  return UrlType::UNKNOWN;
}
#endif  // BUILDFLAG(IS_ARKWEB_EXT)

}  // namespace url_formatter
