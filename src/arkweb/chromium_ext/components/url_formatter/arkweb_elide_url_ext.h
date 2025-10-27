// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// This file defines utility functions for eliding URLs.

#ifndef COMPONENTS_URL_FORMATTER_ARKWEB_ELIDE_URL_EXT_H_
#define COMPONENTS_URL_FORMATTER_ARKWEB_ELIDE_URL_EXT_H_

#include <string>

#include "build/build_config.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "url/third_party/mozilla/url_parse.h"
#endif

class GURL;

namespace url_formatter {

#if BUILDFLAG(IS_ARKWEB_EXT)
/**
 * Parse the character string entered by the user and return the type of the
 * character string.
 * @param String the input to parse
 * @return  INVALID 0 invalid type
 *          URL     1 URL type
 *          QUERY   2 QUERY type
 *          UNKNOWN 3 UNKNOW type
 */
enum UrlType { INVALID = 0, URL = 1, QUERY = 2, UNKNOWN = 3 };

UrlType ParseInput(const std::u16string& input,
                   url::Parsed* parts,
                   std::u16string* scheme,
                   GURL* canonicalized_url);

#if BUILDFLAG(ARKWEB_TEST)
int TestNumNonHostComponents(const url::Parsed& parts);

UrlType TestGetInputTypeForScheme(const std::string& scheme);
#endif  // BUILDFLAG(ARKWEB_TEST)

#endif  // BUILDFLAG(IS_ARKWEB_EXT)

}  // namespace url_formatter

#endif  // COMPONENTS_URL_FORMATTER_ARKWEB_ELIDE_URL_EXT_H_
