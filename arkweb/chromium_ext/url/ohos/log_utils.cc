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

#include <algorithm>
#include <memory>
#include <sstream>
#include <vector>

#include "base/strings/string_util.h"
#include "build/build_config.h"
#include "url/url_constants.h"

namespace url {

constexpr char kReplaceStr[] = "***";
constexpr char kSchemeSeparator[] = "//";
constexpr char kSchemeSeparatorForReport[] = "://";
constexpr char kSchemeNotParseOne[] = "about:blank";
constexpr char kSchemeNotParseTwo[] = "hwweb://newtab";
constexpr size_t kSchemeSuffixLength = 3;
constexpr size_t kAdditionalCharsToShow = 3;
constexpr size_t kMaxUrlLen = 128;
constexpr int32_t FEEDSPAGE_TYPE = 5;

// static
bool LogUtils::IsSupportScheme(const std::string_view url,
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
bool LogUtils::IsSupportScheme(const std::string_view scheme) {
  if (base::EqualsCaseInsensitiveASCII(scheme, url::kHttpScheme) ||
      base::EqualsCaseInsensitiveASCII(scheme, url::kHttpsScheme)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
      || base::EqualsCaseInsensitiveASCII(scheme, url::kChromeExtensionScheme)
#endif
#if BUILDFLAG(ARKWEB_RECOURCE_SCHEME)
      || base::EqualsCaseInsensitiveASCII(scheme, url::kResourcesScheme)
#endif
  ) {
    return true;
  }
  return false;
}

// static
void LogUtils::FixupUrlPart(const std::string_view text,
                            const url::Component& part,
                            std::string& converted) {
  if (!part.is_valid()) {
    return;
  }

  converted.append(text, part.begin, part.len);
}

// static
void LogUtils::ConvertUrlHost(const std::string_view url,
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
void LogUtils::ConvertUrlPath(const std::string_view url,
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
void LogUtils::ConvertUrlQuery(const std::string_view url,
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
void LogUtils::ConvertUrlRef(const std::string_view url,
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
    return std::string(url);
  }

  if (url.length() == 0) {
    return std::string(url);
  }

  if (url.find(kSchemeNotParseOne) == 0 || url.find(kSchemeNotParseTwo) == 0) {
    return std::string(url);
  }

  unsigned int colonIndex = 0;
  std::string converted;

  if (!IsSupportScheme(url, url.length(), colonIndex)) {
    if (colonIndex < url.length() - 1) {
      return std::string(url.substr(0, colonIndex + 1));
    } else {
      return std::string(url);
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
std::string LogUtils::MaskHost(const std::string_view host) {
  size_t dotPos = host.find('.');
  if (dotPos == std::string_view::npos) {
    return std::string(host);
  }

  return std::string(host.substr(0, dotPos + 1)) + "***";
}

std::string MaskMiddleTwoChars(std::string_view scheme) {
  std::string scheme_copy(scheme);

  if (scheme_copy.size() <= 2) {
    return scheme_copy;
  }

  size_t mid = (scheme_copy.size() - 1) / 2;
  scheme_copy[mid] = '*';
  scheme_copy[mid + 1] = '*';
  return scheme_copy;
}

// static
std::string LogUtils::ConvertUrlWithMask(const std::string& url) {
  std::string_view truncated_url = std::string_view(url).substr(0, kMaxUrlLen);
  if (truncated_url.find(kSchemeNotParseOne) == 0 ||
      truncated_url.find(kSchemeNotParseTwo) == 0) {
    return std::string(truncated_url);
  }

  unsigned int colonIndex = 0;
  if (!IsSupportScheme(truncated_url, truncated_url.length(), colonIndex)) {
    return MaskHost(truncated_url);
  }

  size_t endPos = colonIndex + kSchemeSuffixLength + kAdditionalCharsToShow;
  if (endPos > truncated_url.length()) {
    endPos = truncated_url.length();
  }

  std::string converted;
  converted.append(MaskMiddleTwoChars(truncated_url.substr(0, colonIndex)));
  converted.append(truncated_url.substr(colonIndex, endPos - colonIndex));
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

}  // namespace url
