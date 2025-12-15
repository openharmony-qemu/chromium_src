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

#include "log_utils.h"

#include "stdlib.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <vector>

#include "base/strings/string_util.h"
#include "build/build_config.h"
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "url/url_constants.h"
#include "url/url_canon.h"
#include "url/url_canon_internal.h"
#include "url/url_canon_stdstring.h"
#include "url/third_party/mozilla/url_parse.h"

namespace {
void AppendIPv4Address(const unsigned char address[4], url::CanonOutput* output) {
  // Noise the ip addressed.
  for (int i = 0; i < 4; i++) {
    if (i % 2 == 0) {
      output->push_back('*');
      output->push_back('.');
      continue;
    }
    char str[16];
    url::_itoa_s(address[i], str, 10);

    for (int ch = 0; str[ch] != 0; ch++)
      output->push_back(str[ch]);

    if (i != 3)
      output->push_back('.');
  }    
}

// Searches for the longest sequence of zeros in |address|, and writes the
// range into |contraction_range|. The run of zeros must be at least 16 bits,
// and if there is a tie the first is chosen.
void ChooseIPv6ContractionRange(const unsigned char address[16],
                                url::Component* contraction_range) {
  // The longest run of zeros in |address| seen so far.
  url::Component max_range;

  // The current run of zeros in |address| being iterated over.
  url::Component cur_range;

  for (int i = 0; i < 16; i += 2) {
    // Test for 16 bits worth of zero.
    bool is_zero = (address[i] == 0 && address[i + 1] == 0);

    if (is_zero) {
      // Add the zero to the current range (or start a new one).
      if (!cur_range.is_valid())
        cur_range = Component(i, 0);
      cur_range.len += 2;
    }

    if (!is_zero || i == 14) {
      // Just completed a run of zeros. If the run is greater than 16 bits,
      // it is a candidate for the contraction.
      if (cur_range.len > 2 && cur_range.len > max_range.len) {
        max_range = cur_range;
      }
      cur_range.reset();
    }
  }
  *contraction_range = max_range;
}

void AppendIPv6Address(const unsigned char address[16], url::CanonOutput* output) {
  // We will output the address according to the rules in:
  // http://tools.ietf.org/html/draft-kawamura-ipv6-text-representation-01#section-4

  // Start by finding where to place the "::" contraction (if any).
  url::Component contraction_range;
  ChooseIPv6ContractionRange(address, &contraction_range);

  for (int i = 0; i <= 14;) {
    // We check 2 bytes at a time, from bytes (0, 1) to (14, 15), inclusive.
    DCHECK(i % 2 == 0);
    if (i == contraction_range.begin && contraction_range.len > 0) {
      // Jump over the contraction.
      if (i == 0)
        output->push_back(':');
      output->push_back(':');
      i = contraction_range.end();
    } else {
      // Consume the next 16 bits from |address|.
      int x = address[i] << 8 | address[i + 1];

      // Noise the ip addressed.
      if (i % 4 == 0) {
        i += 2;
        output->push_back('*');
        output->push_back(':');
        continue;
      }

      i += 2;

      // Stringify the 16 bit number (at most requires 4 hex digits).
      char str[5];
      _itoa_s(x, str, 16);
      for (int ch = 0; str[ch] != 0; ++ch)
        output->push_back(str[ch]);

      // Put a colon after each number, except the last.
      if (i < 16)
        output->push_back(':');
    }
  }
}
}  // namespace

namespace url {
const char kReplaceStr[] = "***";
const char kSchemeSeparator[] = "//";
const char kSchemeSeparatorForReport[] = "://";
const char kSchemeNotParseOne[] = "about:blank";
const char kSchemeNotParseTwo[] = "hwweb://newtab";
const size_t kSchemeSuffixLength = 3;
const size_t kAdditionalCharsToShow = 2;
int32_t FEEDSPAGE_TYPE = 5;
// static
bool LogUtils::IsSupportScheme(const std::string& url,
                               unsigned int urlLen,
                               unsigned int& colonIndex) {
  while (colonIndex < urlLen && url[colonIndex] != ':') {
    colonIndex++;
  }

  if (colonIndex == urlLen) {
    return false;
  }

  return IsSupportScheme(url.substr(0, colonIndex));
}

// static
bool LogUtils::IsSupportScheme(const std::string& scheme) {
  if (base::EqualsCaseInsensitiveASCII(scheme, url::kHttpScheme) ||
      base::EqualsCaseInsensitiveASCII(scheme, url::kHttpsScheme)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
       ||
      base::EqualsCaseInsensitiveASCII(scheme, url::kChromeExtensionScheme)
#endif
      ) {
    return true;
  }
  return false;
}

// static
void LogUtils::FixupUrlPart(const std::string& text,
                            const url::Component& part,
                            std::string& converted) {
  if (!part.is_valid()) {
    return;
  }

  converted.append(text, part.begin, part.len);
}

// static
void LogUtils::ConvertUrlHost(const std::string& url,
                              url::Parsed& parsed,
                              std::string& converted,
                              bool is_for_report) {
  FixupUrlPart(url, parsed.scheme, converted);
  if (!is_for_report) {
    converted.append(url::kSchemeSeparator);
  } else {
    converted.append(url::kSchemeSeparatorForReport);
  }

  if (parsed.username.is_valid()) {
    converted.append(kReplaceStr);
    if (parsed.password.is_valid()) {
      converted.append(":");
      converted.append(kReplaceStr);
    }
    converted.append("@");
  }

  FixupUrlPart(url, parsed.host, converted);

  if (parsed.port.is_valid()) {
    converted.append(":");
    FixupUrlPart(url, parsed.port, converted);
  }
  converted.append("/***");
  return;
}

// static
void LogUtils::ConvertUrlPath(const std::string& url,
                              const url::Component& part,
                              std::string& converted) {
  if (!part.is_valid()) {
    return;
  }

  int scanIndex = part.begin;
  if (IsURLSlash(url[scanIndex])) {
    converted.append("/");
  }
  converted.append(kReplaceStr);
  return;
}

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
// static
bool LogUtils::IsSupportParam(const std::string& param) {
  if (base::EqualsCaseInsensitiveASCII(param, url::kToAppParam) ||
      base::EqualsCaseInsensitiveASCII(param, url::kChannelParam) ||
      base::EqualsCaseInsensitiveASCII(param, url::kChannelIdParam)) {
    return true;
  }
  return false;
}
#endif

// static
void LogUtils::ConvertUrlQuery(const std::string& url,
                               const url::Component& part,
                               std::string& converted) {
  if (!part.is_valid()) {
    return;
  }

  converted.append("?");

  int i = 0;
  int appendStart = part.begin;
  int appendLast = part.begin;
  bool saveFlag = true;
  bool replaceFlag = false;
  for (i = part.begin; i < part.end(); i++) {
    std::string temp_str = "";
    if (url[i] == '=') {
      temp_str.append(url, appendStart, i - appendStart);
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
      if (IsSupportParam(temp_str)) {
#else
      if (temp_str == "to_app" || temp_str == "channel" ||
          temp_str == "channelid") {
#endif
        converted.append(url, appendStart, i - appendStart + 1);
        appendLast = i + 1;
        appendStart = i + 1;
        saveFlag = true;
        replaceFlag = false;
      } else {
        converted.append(kReplaceStr);
        saveFlag = false;
        replaceFlag = false;
        appendStart = part.end();
      }
      continue;
    }

    if (url[i] == '&') {
      if (saveFlag) {
        converted.append(url, appendLast, i - appendLast);
        saveFlag = false;
        replaceFlag = true;
      }
      converted.append("&");
      appendStart = i + 1;
    }
  }
  if (replaceFlag) {
    converted.append(kReplaceStr);
  } else if (appendStart <= part.end()) {
    converted.append(url, appendStart, part.end() - appendStart + 1);
  }
}

// static
void LogUtils::ConvertUrlRef(const std::string& url,
                             const url::Component& part,
                             std::string& converted) {
  if (!part.is_valid()) {
    return;
  }

  converted.append("#");
  converted.append(kReplaceStr);
}

// staic
std::string LogUtils::ConvertUrl(const std::string& url,
                                 int32_t scenario_type,
                                 bool should_noise_url) {
  if (!should_noise_url) {
    return url;
  }

  if (url.length() == 0) {
    return url;
  }

  if (url.find(kSchemeNotParseOne) == 0 || url.find(kSchemeNotParseTwo) == 0) {
    return url;
  }

  unsigned int colonIndex = 0;
  std::string converted;

  if (!IsSupportScheme(url, url.length(), colonIndex)) {
    if (colonIndex < url.length() - 1) {
      return url.substr(0, colonIndex + 1);
    } else {
      return url;
    }
  }

  url::Parsed parsed;
  url::ParseStandardURL(url.data(), url.length(), &parsed);

  ConvertUrlHost(url, parsed, converted, true);
  if (scenario_type == FEEDSPAGE_TYPE) {
    ConvertUrlPath(url, parsed.path, converted);
    ConvertUrlQuery(url, parsed.query, converted);
    ConvertUrlRef(url, parsed.ref, converted);
  } else {
    converted.append("/***");
  }
  return converted;
}

// static
std::string LogUtils::MaskHost(const std::string& host) {
  size_t dotPos = host.find('.');
  if (dotPos == std::string::npos) {
    return host;
  }
  
  return host.substr(0, dotPos + 1) + "***";
}



// static
std::string LogUtils::ConvertUrlWithMask(const std::string& url) {
  if (url.find(kSchemeNotParseOne) == 0 || url.find(kSchemeNotParseTwo) == 0) {
    return url;
  }

  unsigned int colonIndex = 0;
  if (!IsSupportScheme(url, url.length(), colonIndex)) {
    return MaskHost(url);
  }

  size_t endPos = colonIndex + kSchemeSuffixLength + kAdditionalCharsToShow;
  if (endPos > url.length()) {
    endPos = url.length();
  }

  std::string converted;
  converted.append(url.substr(0, endPos));
  converted.append("***");
  return converted;
}

// static
std::string LogUtils::ConvertPathWithMask(const std::string& file_path) {
    if (file_path.empty()) {
        return "";
    }
    std::string normalized_path = file_path;
    std::string::size_type pos = 0;
    // Replace '\\' "//"" with '/' 
    while ((pos = normalized_path.find('\\', pos)) != std::string::npos) {
        normalized_path[pos] = '/';
        pos++;
    }
    pos = 0;
    while ((pos = normalized_path.find("//", pos)) != std::string::npos) {
        normalized_path.replace(pos, 2, "/");
    }
    std::vector<std::string> parts;
    std::stringstream ss(normalized_path);
    std::string part;
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    if (parts.empty()) {
        return "";
    }
    std::string result = "";
    // Skip the drive letter(e.g "D:")
    size_t start_index = 0;
    if (parts.size() > 0 && parts[0].length() == 2 && parts[0][1] == ':') {
      start_index = 1;
    }
    for (size_t i = start_index; i < parts.size(); ++i) {
        std::string current_part = parts[i];
        if (i == parts.size() - 1) {
            // Remove file extension
            size_t dot_pos = current_part.find_last_of('.');
            if (dot_pos != std::string::npos) {
                current_part = current_part.substr(0, dot_pos);
            }
        }
        if (!current_part.empty()) {
            std::string masked_part;
            // Replace all characters except the first one with "***"
            masked_part += current_part[0];
            masked_part += "***";

            result += "/" + masked_part;
        }
    }
    return result;
}

std::string LogUtils::AnonymizeIpAddress(const net::IPEndPoint& ip_endpoint) {
  std::string str;
  url::StdStringCanonOutput output(&str);

  if (ip_endpoint.address().IsIPv4()) {
    AppendIPv4Address(ip_endpoint.address().bytes().data(), &output);
  } else if (ip_endpoint.address().IsIPv6()) {
    AppendIPv6Address(ip_endpoint.address().bytes().data(), &output);
  }

  output.Complete();
  return str;
}

}  // namespace url
